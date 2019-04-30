#include "formula.cpp"
#include <ostream>
    using std::ostream;
#include <chrono>
using namespace std::chrono;

class Search {
public:

	Search (Formula& f, uint p) : formula(f), period(p) {}

    virtual void test(ostream& out, bool* initSolution, uint maxTime) {

        uint i = 0, best = 0, retries = 0;
        bool solution[formula.v];
        formula.copySolution(initSolution, solution);
        high_resolution_clock::time_point t1, t2;
        t1 = high_resolution_clock::now(); t2 = t1;
        for (uint current = formula.evaluate(solution); i < formula.c; i++) {
            if (current == formula.c) {
                best = current;
                break;
            }
            if (duration_cast<milliseconds>(t2 - t1).count() > maxTime) {
                cout << "NA\tNA";
                return;
            }
            if (i >= period) {
                formula.resetSolution(solution);
                if (current > best)  { 
                    best = current;
                }
                i = 0; retries++;
            }
            current = search(solution);
            t2 = high_resolution_clock::now();
        }
        out << duration_cast<milliseconds>(t2 - t1).count() * 0.001f << '\t' << retries;
    }
protected:

	Formula& formula;
	uint period;
	
	virtual int search(bool* solution) = 0;
	
};

class WalkSAT : public Search {
public:
	WalkSAT(Formula& f, uint p) : Search(f, p) {}
	
protected:

    int search(bool* solution) {
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
	GSAT(Formula& f, uint p) : Search(f, p) {}

protected:
    int search(bool* solution) {
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
	TabuGSAT(Formula& f, uint p) : Search(f, p) {
		tabu = new uint[f.v];
		for (uint i = 0; i < f.v; i++) {
			
		}
	}
	
	~TabuGSAT() { delete [] tabu; }
	
protected:

	uint* tabu;

    int search(bool* solution) {
        uint best = -1;
        uint vbest = 0;
        uint eq = 1;
        for (uint i = 0; i < formula.v; i++) {
			if (tabu[i] > 0)
				continue;
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

// Constructive

void greedy(Formula& formula, bool* solution) {
    // TODO
}
