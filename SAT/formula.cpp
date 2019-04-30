#include <vector>
    using std::vector;
#include <string>
    using std::string;
    using std::stoi;
#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
    using std::cerr;
#include <fstream>
    using std::ifstream;
    using std::istream;
    
#define Clause vector<int>

struct Formula {
    uint v, c;
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
	    uint currentClause;
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

    uint evaluate(bool* values) {
        uint e = 0;
        for (uint i = 0; i < c; i++) {
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

    inline uint testFlip(int i, bool* solution) {
        solution[i] = !solution[i];
        int e = evaluate(solution);
        solution[i] = !solution[i];
        return e;
    }
    
    inline vector<uint> unsatClauses(bool* solution) {
		vector<uint> s;
		for (uint i = 0; i < c; i++) {
			if (!isSat(i, solution))
				s.push_back(i);
		}
		return s;
	}

	inline void resetSolution(bool* solution) {
	    for (uint i = 0; i < v; i++)
	        solution[i] = rand() % 2;
	}

	inline void copySolution(bool* orig, bool* dest) {
		for (uint i = 0; i < v; i++)
			dest[i] = orig[i];
	}
	
};

inline uint index(int var) {
	return var < 0 ? -var - 1 : var - 1;
}
