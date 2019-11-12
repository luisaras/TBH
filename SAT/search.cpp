#include <algorithm>
    using std::sort;
#include <ostream>
    using std::ostream;
#include <chrono>
using namespace std::chrono;

#include "formula.cpp"
#include "aco.cpp"
#include "greedy.cpp"
#include "knuth.cpp"

// Reset types
#define NONE 0
#define STOP 1
#define RESET 2

// Greedy
#define BEST_G_ALPHA 0.2
#define BEST_C_ALPHA 0.2

// Tabu
#define BEST_DMIN f.v * 0.04
#define BEST_DMAX f.v * 0.48

// UMDA
#define BEST_C_UMDA 100000
#define BEST_L_UMDA 0.1

// PBIL
#define BEST_C_PBIL 100000
#define BEST_L_PBIL 0.1
#define BEST_PM 0.02
#define BEST_M 0.05

// MIMIC
#define BEST_C_MIMIC 10000

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
        steps = 0; retries = 0; executionTime = 0;

        high_resolution_clock::time_point start = high_resolution_clock::now();

        // Initial solution (random or greedy)
        Solution solution(formula);
        resetSolution(formula, solution);
        uint last = 0, current = best = solution.value;

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
                if (current > best)
                    best = current;
                i = 0; retries++;
            }
            // Update solution
            last = current;
            current = step(solution);
            if (maxLocal == NONE) {
                if (current > best)
                    best = current;
            }
            executionTime = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
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

    UMDA(Formula& f, uint s, uint c, double l) : Search(f, (uint) -1, NONE) {
        this->s = s; 
        this->c = c;
        this->l = l;
        model = new double[f.v];
    }

    UMDA(Formula& f) : UMDA(f, BEST_C_UMDA / 2, BEST_C_UMDA, BEST_L_UMDA) {}

    ~UMDA() { delete [] model; }

    bool test(uint maxTime) {
        for (int i = 0; i < formula.v; i++)
            model[i] = 0.5;
        return Search::test(maxTime);
    }

protected:

    uint s, c;
    double l;
    double* model;

    int step(Solution& solution) {
        // Create new solutions.
        bool solutions[c][formula.v];
        uint values[c];
        uint order[c];
        double mean = 0;
        for (uint i = 0; i < c; i++) {
            for (uint v = 0; v < formula.v; v++) {
                solutions[i][v] = rand() * 1.0 / RAND_MAX < model[v];
            }
            values[i] = formula.evaluate(solutions[i]);
            order[i] = i;
            mean += values[i];
        }
        mean /= c;
        // Select best.         
        sort(order, order+c, [&values](uint i, uint j) {
            return values[i] > values[j]; 
        });
        if (values[order[0]] > solution.value) {
        	formula.copySolution(solutions[order[0]], solution.attribution);
        	solution.updateSat(formula);
        }
        // Update model.
        for (uint v = 0; v < formula.v; v++) {
        	// Fraction of best solutions that contain variable v as true 
            double freq = 0; 
            for (uint i = 0; i < s; i++) {
                uint id = order[i];
                freq += solutions[id][v] * (values[id] - mean) / mean;
            }
            freq /= s;
            // Update probabilities
            model[v] = (1 - l) * model[v] + l * (freq - model[v]);
        }
        return values[order[0]];
    }

};

class PBIL : public UMDA {
public:

	PBIL (Formula& f, uint c, double l, double m, double pm) : UMDA(f, c / 2, c, l) {
		this->m = m;
		this->pm = pm;
	}

	PBIL (Formula& f) : PBIL(f, BEST_C_PBIL, BEST_L_PBIL, BEST_M, BEST_PM) {}

protected:

	double m, pm;

	int step(Solution& solution) {
		int result = UMDA::step(solution);
		for (uint v = 0; v < formula.v; v++) {
			if (rand() < pm * RAND_MAX)
				model[v] = (1 - m) * model[v] + m * (1.0 * rand() / RAND_MAX);
		}
		return result;
	}

};

class MIMIC : public Search {
public:

	MIMIC (Formula& f, uint c) : Search(f, (uint) -1, NONE) {
        this->c = c;
        order = new uint[f.v]; // Probability order[k] depends on value of order[k - 1]
        model[0] = new double[f.v]; // P(x_k | !x_k-1)
        model[1] = new double[f.v]; // P(x_k |  x_k-1)
        population = new bool*[c];
	}

    MIMIC(Formula& f) : MIMIC(f, BEST_C_MIMIC) {}

    ~MIMIC() { 
        for (uint i = 0; i < c; i++)
            delete [] population[i];
        delete [] population;
    }

    bool test(uint maxTime) {
        init();
        return Search::test(maxTime);
    }

protected:

    uint c;
	uint* order;
    double* model[2];
    bool** population;
    double theta, l = 1;

    void init() {
        // Initial population
        for (uint i = 0; i < c; i++)
            population[i] = new bool[formula.v];
        for (uint k = 0; k < formula.v; k++) {
            // Uniform probabilities
            model[0][k] = model[1][k] = 0.5;
            order[k] = k;
        }
        createPopulation();
        findPermutation();
        // Average fitness
        theta = 0;
        for (uint i = 0; i < c; i++)
            theta += 1.0 * formula.evaluate(population[i]) / c;
    }

	int step(Solution& solution) {
        // New population
        createPopulation();
        uint fittest = 0;
        uint fitness[c];
        for (uint i = 0; i < c; i++) {
            fitness[i] = formula.evaluate(population[i]);
            if (fitness[i] > fitness[fittest])
                fittest = i;
        }

        // Empirical probablity of first variable
        uint s = 0;
        uint id = order[0];
        double p[2] = {0, 0};
        for (uint i = 0; i < c; i++) {
            if (fitness[i] >= theta) {
                p[1] += population[i][id];
                s++;
            }
        }
        p[1] /= s;
        // Update
        model[1][id] = (1 - l) * model[1][id] + l * p[1];

        // Conditional probabilities of next variables
        for (uint k = 1; k < formula.v; k++) {
            id = order[k];
            p[0] = p[1] = 0;
            uint previous = order[k - 1];
            for (uint i = 0; i < c; i++) {
                if (fitness[i] >= theta) {
                    bool value = population[i][previous];
                    p[value] += population[i][id] * 1.0 / c;
                }
            }
            // Update
            model[0][id] = (1 - l) * model[0][id] + l * p[0];
            model[1][id] = (1 - l) * model[1][id] + l * p[1];
        }

        // Update theta and best solution.
        theta = 0;
        for (uint i = 0; i < c; i++)
            theta += 1.0 * formula.evaluate(population[i]) / c;
        if (fitness[fittest] > solution.value) {
            formula.copySolution(population[fittest], solution.attribution);
            solution.updateSat(formula);
        }
        return fitness[fittest];
	}

    void createPopulation() {
        for (uint i = 0; i < c; i++) {
            uint id = order[0];
            population[i][id] = rand() < model[1][id] * RAND_MAX;
            for (uint k = 1; k < formula.v; k++) {
                id = order[k];
                uint previous = order[k - 1];
                double* model = this->model[population[i][previous]];
                population[i][id] = rand() < model[id] * RAND_MAX;
            }
        }
    }

    void findPermutation() {
        unordered_set<uint> unset;
        for (uint v = 0; v < formula.v; v++)
            unset.insert(v);
        order[0] = firstVariable();
        for (uint k = 1; k < formula.v; k++) {
            unset.erase(order[k - 1]);
            order[k] = nextVariable(unset, order[k - 1]);
        }
    }

    uint firstVariable() {
        double best_h = 10000;
        uint best;
        for (uint k = 0; k < formula.v; k++) {
            // Probability
            double p0, p1 = 0;
            for (uint i = 0; i < c; i++)
                p1 += population[i][k];
            p1 /= c;        // P(x=1)
            p0 = 1 - p1;    // P(x=0)
            // Entropy
            double h = -p1 * log2(p1)
                       -p0 * log2(p0);
            if (h < best_h) {
                best = k;
                best_h = h;
            }
        }
        return best;
    }

    uint nextVariable(unordered_set<uint>& unset, int previous) {
        double best_h = 10000;
        uint best;
        for (uint k : unset) {
            // Probability
            double p00, p01 = 0, p10, p11 = 0;
            double p0, p1 = 0;
            for (uint i = 0; i < c; i++) {
                if (population[i][k]) {
                    if (population[i][previous])
                        p11++;
                    else
                        p10++;
                } else {
                    if (population[i][previous])
                        p01++;
                    else
                        p00++;
                }
            }
            p11 /= c;       // P(x=1,y=1)
            p10 /= c;       // P(x=1,y=0)
            p01 /= c;       // P(x=0,y=1)
            p00 /= c;       // P(x=0,y=0)
            p1 = p11 + p10; // P(x=1)
            p0 = p01 + p00; // P(x=0)
            // Entropy
            double h = -p00 * log2(p00) / p0
                       -p01 * log2(p01) / p0
                       -p10 * log2(p10) / p1
                       -p11 * log2(p11) / p1;
            if (h < best_h) {
                best = k;
                best_h = h;
            }
        }
        return best;
    }

};