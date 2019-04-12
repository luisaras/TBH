#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
    using std::cerr;
#include <fstream>
    using std::ifstream;
    using std::istream;
#include <vector>
    using std::vector;
#include <string>
    using std::string;
    using std::stoi;
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>
using namespace std::chrono;
#define Clause vector<int>


struct Formula {
    int v, c;
    Clause* clauses;

    ~Formula() { delete [] clauses; }

    int evaluate(bool* values) {
        int e = 0;
        for (int i = 0; i < c; i++) {
			if (isSat(i, values)) {
				e++;
			}
        }
        return e;
    }
    
    inline bool isSat(int clause, bool* values) {
		for (int var : clauses[clause]) {
			if (var < 0) {
				if (!values[-var - 1]) {
					return true;
				}
			} else {
				if (values[var - 1]) {
					return true;
				}
			}
		}
		return false;
	}

    inline int testFlip(int i, bool* solution) {
        solution[i] = !solution[i];
        int e = evaluate(solution);
        solution[i] = !solution[i];
        return e;
    }
    
    inline vector<int> unsatClauses(bool* solution) {
		vector<int> s;
		for (int i = 0; i < c; i++) {
			if (!isSat(i, solution))
				s.push_back(i);
		}
		return s;
	}
	
};

inline int index(int var) {
	return var < 0 ? -var - 1 : var - 1;
}

int walksat(Formula& formula, bool* solution) {
	vector<int> unsat = formula.unsatClauses(solution);
	int c = rand() % unsat.size();
	Clause& clause = formula.clauses[unsat[c]];
	//cout << "clause " << c << " of " << unsat.size() << " " << clause.size() << endl;
	int best = 0;
	int vbest = formula.testFlip(index(clause[0]), solution);
	for (int i = 1; i < (int) clause.size(); i++) {
		//cout << "var " << i << endl;
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

int gsat(Formula& formula, bool* solution) {
    int best = 0;
    int vbest = formula.testFlip(0, solution);
    for (int i = 1; i < formula.v; i++) {
        int vi = formula.testFlip(i, solution);
        if (vi > vbest) {
            best = i;
            vbest = vi;
        }
    }
    solution[best] = !solution[best];
    return vbest;
}

Formula readFormula(string& name) {
	ifstream stream(name);
	Formula f;
    if (!stream) {
        cerr << "Unable to open file: " << name;
        return f;
    }
    string s;
    int c; // current clause
    while (stream >> s) {
        if (s == "c") {
            // Comments
            getline(stream, s);
        } else if (s == "p") {
            // New formula
            stream >> s // "cnf"
                   >> f.v // number of variables
                   >> f.c; // number of clauses
            f.clauses = new Clause[f.c];
            c = 0;
        } else if (s == "%") {
        	break;
        } else {
            // New clause
            f.clauses[c].push_back(stoi(s));
            int var;
            while (stream >> var && var != 0) {
                f.clauses[c].push_back(var);
            }
            c++;
        }
    }
    stream.close();
    return f;
}

void resetSolution(bool* solution, int n) {
    for (int i = 0; i < n; i++)
        solution[i] = rand() % 2;
}

void testSearch(Formula& formula, bool* initSolution, int (*search)(Formula&, bool*), 
		int retry, uint maxit = UINT_MAX) {
	bool solution[formula.v];
	for (int i = 0; i < formula.v; i++)
		solution[i] = initSolution[i];
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	int i = 0, best = 0;
	for (int current = formula.evaluate(solution); i < formula.c && maxit > 0; i++, maxit--) {
	    if (i >= retry) {
	    	resetSolution(solution, formula.v);
	    	cout << "Best solution: " << current << endl;
	    	if (current > best) best = current;
	    	i = 0;
	    }
	    current = search(formula, solution);
	    if (current == formula.c) {
	    	best = current;
	    	break;
	    }
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	cout << best << " clauses in " << duration_cast<microseconds>(t2 - t1).count() << "ms"
		<< " and " << i << " iterations. " << endl;
}

void test(string name) {
	Formula formula = readFormula(name);
	cout << formula.c << " clauses." << endl;
    bool initSolution[formula.v];
    resetSolution(initSolution, formula.v);
    cout << "GSAT:" << endl;
    testSearch(formula, initSolution, gsat, formula.v * 3);
    cout << "WalkSAT:" << endl;
    testSearch(formula, initSolution, walksat, formula.v * 3);
}

int main() {
    srand(0);

	test("flat75-1.cnf");

    return 0;
}
