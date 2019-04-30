#include <vector>
    using std::vector;

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