#include "greedy.cpp"
#include <ostream>
    using std::ostream;
#include <chrono>
using namespace std::chrono;

#define NONE 0
#define STOP 1
#define RESET 2

#define BEST_ALPHA 1
#define BEST_DMIN 0
#define BEST_DMAX f.v / 3

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
        bool solution[formula.v];
        resetSolution(formula, solution);
        uint last = 0, current = formula.evaluate(solution);

        for (uint i = 0; true; i++, steps++) {
            // Check if global/local maximum is found
            if (current == formula.c || (maxLocal == STOP && current == last)) {
                best = current;
                break;
            }
            // Check if time-out
            if (executionTime > maxTime) {
                return false;
            }
            // Reset solution if reached max step count
            if (i >= period || (maxLocal == RESET && current == last)) {
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
        
        return true;
    }
    
protected:

	Formula& formula;
	uint period;
    short maxLocal;
	
	virtual int step(bool* solution) = 0;

    virtual void resetSolution(Formula& formula, bool* solution) {
        formula.resetSolution(solution);
    }

};

class WalkSAT : public Search {
public:

	WalkSAT(Formula& f, uint p, short maxLocal = NONE) : Search(f, p, maxLocal) {}
	
protected:

    int step(bool* solution) {
        vector<uint> unsat = formula.unsatClauses(solution);
        int c = rand() % unsat.size();
        Clause& clause = formula.clauses[unsat[c]];
        int best = 0;
        int vbest = formula.testFlip(index(clause[0]), solution);
        for (int i = 1; i < (int) clause.size(); i++) {
            int vi = formula.testFlip(index(clause[i]), solution);
            if (vi > vbest) {
                best = i;
                vbest = vi;
            }
        }
        int i = index(clause[best]);
        solution[i] = !solution[i];
        return vbest;
    }

};

class GSAT : public Search {
public:

	GSAT(Formula& f, uint p, short maxLocal = NONE) : Search(f, p, maxLocal) {}

protected:

    int step(bool* solution) {
        uint best = 0;
        uint vbest = formula.testFlip(0, solution);
        uint eq = 1;
        for (uint i = 1; i < formula.v; i++) {
            uint vi = formula.testFlip(i, solution);
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
        solution[best] = !solution[best];
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

    int step(bool* solution) {
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
            uint vi = formula.testFlip(i, solution);
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
			solution[best] = !solution[best];
		}
        return vbest;
    }

};

class GRASP : public GSAT {
public:

    GRASP(Formula& formula, short maxLocal = NONE, double alpha = BEST_ALPHA) : 
            GSAT(formula, (uint) -1, maxLocal) {
        this->alpha = alpha;
    }

protected:

    double alpha;

    void resetSolution(Formula& formula, bool* solution) {
        greedy(formula, solution, alpha);
        steps += formula.v;
    }

};
