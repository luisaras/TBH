
class Search {

public:

    void test(ostream& out, Formula& formula, bool* initSolution, 
            int period, uint maxTime) {

        int i = 0, best = 0, retries = 0;
        bool solution[formula.v];
        formula.copySolution(initSolution, solution);
        high_resolution_clock::time_point t1, t2;
        t1 = high_resolution_clock::now(); t2 = t1;
        for (int current = formula.evaluate(solution); i < formula.c; i++) {
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
            current = search(formula, solution);
            t2 = high_resolution_clock::now();
        }
        out << duration_cast<milliseconds>(t2 - t1).count() * 0.001f << '\t' << retries;
    }

};


class WalkSAT : Search {
public:

    int search(Formula& formula, bool* solution) {
        vector<int> unsat = formula.unsatClauses(solution);
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

    int search(bool* solution) = 0;

};

class GSAT : Search {

    int search(Formula& formula, bool* solution) {
        int best = 0;
        int vbest = formula.testFlip(0, solution);
        int eq = 1;
        for (int i = 1; i < formula.v; i++) {
            int vi = formula.testFlip(i, solution);
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

class TabuGSAT : Search {

    int search(Formula& formula, bool* solution) {
        int best = 0;
        int vbest = formula.testFlip(0, solution);
        int eq = 1;
        for (int i = 1; i < formula.v; i++) {
            int vi = formula.testFlip(i, solution);
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