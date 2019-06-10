#include "greedy.cpp"
#include <algorithm>
    using std::sort;
#include <ostream>
    using std::ostream;
#include <chrono>
using namespace std::chrono;

#define NONE 0
#define STOP 1
#define RESET 2

#define BEST_G_ALPHA 0.2
#define BEST_C_ALPHA 0.2
#define BEST_DMIN 0
#define BEST_DMAX f.v / 3

#define BEST_C_UMDA f.v * 10

class Search {
public:

    uint steps, retries, best;
    long executionTime = 0;

	Search (Formula& f, uint period, short maxLocal = NONE) : formula(f) {
        this->maxLocal = maxLocal;
        this->period = period;
    }

    virtual bool test(uint maxTime) {
        // Stats
        steps = 0; retries = 0; best = 0; executionTime = 0;

        high_resolution_clock::time_point start = high_resolution_clock::now();

        // Initial solution (random or greedy)
        Solution solution(formula);
        resetSolution(formula, solution);
        uint last = 0, current = solution.value;

        for (uint i = 0; true; i++, steps++) {
            // Check if global/local maximum is found
            if (current == formula.c || (maxLocal == STOP && current <= last)) {
                best = current;
                break;
            }
            // Check if time-out
            if (executionTime > maxTime) {
                return false;
            }
            // Reset solution if reached max step count
            if (i >= period || (maxLocal == RESET && current <= last)) {
                resetSolution(formula, solution);
                if (current > best)  {
                    best = current;
                }
                i = 0; retries++;
            }
            // Update solution
            last = current;
            current = step(solution);
            executionTime = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
        }
        if (best != formula.evaluate(solution.attribution)) {
            cout << best << " " << formula.evaluate(solution.attribution) << endl;
        }
        return true;
    }
    
protected:

	Formula& formula;
	uint period;
    short maxLocal;
	
	virtual int step(Solution& solution) = 0;

    virtual void resetSolution(Formula& formula, Solution& solution) {
        solution.reset(formula);
    }

};

class WalkSAT : public Search {
public:

	WalkSAT(Formula& f, uint p, short maxLocal = NONE) : Search(f, p, maxLocal) {}
	
protected:

    int step(Solution& solution) {
        uint c = solution.getUnsat(formula, rand() % (formula.c - solution.value));
        Clause& clause = formula.clauses[c];
        uint best = index(clause[0]);
        uint vbest = solution.testFlip(formula, best);
        uint eq = 1;
        for (uint ci = 1; ci < clause.size(); ci++) {
            uint i = index(clause[ci]);
            uint vi = solution.testFlip(formula, i);
            if (vi > vbest) {
                best = i;
                vbest = vi;
            } else if (vi == vbest) {
                eq++;
                if (rand() % eq == 0) {
                    best = i; vbest = vi;
                }
            }
        }
        solution.flip(formula, best);
        return vbest;
    }

};

class GSAT : public Search {
public:

	GSAT(Formula& f, uint p, short maxLocal = NONE) : Search(f, p, maxLocal) {}

protected:

    int step(Solution& solution) {
        uint best = 0;
        uint vbest = solution.testFlip(formula, 0);
        uint eq = 1;
        for (uint i = 1; i < formula.v; i++) {
            uint vi = solution.testFlip(formula, i);
            if (vi > vbest) {
                best = i; vbest = vi;
                eq = 1;
            } else if (vi == vbest) {
                eq++;
                if (rand() % eq == 0) {
                    best = i; vbest = vi;
                }
            }
        }
        solution.flip(formula, best);
        return vbest;
    }

};

class TabuGSAT : public Search {
public:

	TabuGSAT(Formula& f, uint dp, uint dmin, uint dmax) : Search(f, (uint) -1, NONE) {
		tabu = new uint[f.v];
		for (uint i = 0; i < f.v; i++)
			tabu[i] = 0;
		this->dmin = dmin;
		this->dmax = dmax;
		// d-Period
		this->dp = dp;
		dcount = 0;
		d = newd();
	}

    TabuGSAT(Formula& f, uint dp) : TabuGSAT(f, dp, BEST_DMIN, BEST_DMAX) {}
	
	~TabuGSAT() { delete [] tabu; }
	
protected:

	uint dmin, dmax, d, dp, dcount;
	uint* tabu;
	
	inline uint newd() {
		return rand() % (dmax - dmin + 1) + dmin;
	}

    int step(Solution& solution) {
        if (dcount == dp) {
			dcount = 0;
			d = newd();
		}
        uint best = -1;
        uint vbest = 0;
        uint eq = 1;
        for (uint i = 0; i < formula.v; i++) {
			if (tabu[i] > 0) {
				tabu[i]--;
				continue;
			}
            uint vi = solution.testFlip(formula, i);
            if (vi > vbest) {
                best = i; vbest = vi;
                eq = 1;
            } else if (vi == vbest) {
                eq++;
                if (rand() % eq == 0) {
                    best = i; vbest = vi;
                }
            }
        }
        dcount++;
        if (best != (uint) -1) {
			tabu[best] = d;
            solution.flip(formula, best);
		}
        return vbest;
    }

};

class GRASP : public GSAT {
public:

    GRASP(Formula& formula, short maxLocal = NONE, double alpha = BEST_G_ALPHA) : 
            GSAT(formula, (uint) -1, maxLocal) {
        this->alpha = alpha;
    }

protected:

    double alpha;

    void resetSolution(Formula& formula, Solution& solution) {
        greedy(formula, solution.attribution, alpha);
        solution.updateSat(formula);
        steps += formula.v;
    }

};

class UMDA : public Search {
public:

    UMDA(Formula& f, uint s, uint c) : Search(f, (uint) -1, NONE) {
        this->s = s; 
        this->c = c;
        model = new double[f.v];
        for (int i = 0; i < f.v; i++)
            model[i] = 0.5;
    }

    UMDA(Formula& f) : UMDA(f, BEST_C_UMDA / 2, BEST_C_UMDA) {}

    ~UMDA() { delete [] model; }

protected:

    uint s, c;
    double* model;

    int step(Solution& solution) {
        // Create new solutions.
        bool solutions[c][formula.v];
        uint values[c];
        uint order[c];
        for (uint i = 0; i < c; i++) {
            for (uint v = 0; v < formula.v; v++) {
                solutions[i][v] = rand() * 1.0 / RAND_MAX < model[v];
            }
            values[i] = formula.evaluate(solutions[i]);
            order[i] = i;
        }
        // Select best.         
        sort(order, order+c, [&values](uint i, uint j) {
            return values[i] > values[j]; 
        });
        formula.copySolution(solutions[order[0]], solution.attribution);
        // Update model.
        //cout << endl;
        for (uint v = 0; v < formula.v; v++) {
            model[v] = 0;
            for (uint i = 0; i < s; i++) {
                uint id = order[i];
                model[v] += solutions[id][v];
            }
            model[v] /= s;
            //cout << model[v] << " ";
        }
        cout << values[order[0]] << endl;
        return values[order[0]];
    }

};