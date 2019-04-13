#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
    using std::cerr;
#include <fstream>
    using std::ifstream;
    using std::istream;
#include <ostream>
    using std::ostream;
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

    Formula() {}
    Formula(string& name) { read(name); }
    ~Formula() { delete [] clauses; }

	void read(string& name) {
		ifstream stream(name);
	    if (!stream) {
	        cerr << "Unable to open file: " << name;
	        return;
	    }
	    string s;
	    int currentClause;
	    while (stream >> s) {
	        if (s == "c") {
	            // Comments
	            getline(stream, s);
	        } else if (s == "p") {
	            // New formula
	            stream >> s // "cnf"
	                   >> v // number of variables
	                   >> c; // number of clauses
	            clauses = new Clause[c];
	            currentClause = 0;
	        } else if (s == "%") {
	        	break;
	        } else {
	            // New clause
	            clauses[currentClause].push_back(stoi(s));
	            int var;
	            while (stream >> var && var != 0) {
	                clauses[currentClause].push_back(var);
	            }
	            currentClause++;
	        }
	    }
	    stream.close();
	}

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

	inline void resetSolution(bool* solution) {
	    for (int i = 0; i < v; i++)
	        solution[i] = rand() % 2;
	}

	inline void copySolution(bool* orig, bool* dest) {
		for (int i = 0; i < v; i++)
			dest[i] = orig[i];
	}
};

inline int index(int var) {
	return var < 0 ? -var - 1 : var - 1;
}

int walksat(Formula& formula, bool* solution) {
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

int gsat(Formula& formula, bool* solution) {
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

void testSearch(ostream& out, Formula& formula, bool* initSolution, 
		int (*search)(Formula&, bool*), int period, uint maxTime) {

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

void test(string& formulaName, string& searchName, uint timeLimit) {
	srand(0);

	// Get search algorithm
	int (*search)(Formula&, bool*);
    if (searchName == "gsat") {
    	search = gsat;
    } else { 
	    search = walksat;
	}

	// Get formula
	string formulaFile = formulaName + ".cnf";
	Formula formula(formulaFile);
	
	// Run algorithm
    bool initSolution[formula.v];
    formula.resetSolution(initSolution);
    for(int i = 1; i <= 15; i++) {
    	cout << searchName << '\t' 
    		 << formulaName << '\t'
    		 << i << '\t';
    	testSearch(cout, formula, initSolution, search, formula.v * 3, timeLimit);
    	cout << endl;
	}

}

int main(int argc, char* argv[]) {
	// minutes -> seconds -> milliseconds
	int timeLimit = 20 * 60 * 1000;

	string inst[] = { "flat50-1", "par8-5-c", "flat75-1", "flat100-1" };
	string gsat = "gsat";
	string walksat = "walksat";

	test(inst[2], walksat, timeLimit);
	test(inst[3], gsat, timeLimit);
	test(inst[3], walksat, timeLimit);
		
	//string search(argv[1]);
	//string file(argv[2]);
	//uint timeLimit = atoi(argv[3]);
	//test(file, search, timeLimit);

    return 0;
}
