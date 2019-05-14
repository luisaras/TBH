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
#include <unordered_set>
	using std::unordered_set;
	using std::pair;
    
#define Clause vector<int>

inline uint index(int var) {
	return var < 0 ? -var - 1 : var - 1;
}

struct Formula {
	string name;

    uint v, c;
    Clause* clauses;

    unordered_set<uint>* varClauses[2];

    Formula(string& name) { 
    	this->name = name;
    	string fileName = name + ".cnf";
    	fileName = "instances/" + fileName;
    	read(fileName); 
   	}

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
	            varClauses[0] = new unordered_set<uint>[v];
	            varClauses[1] = new unordered_set<uint>[v];
	            currentClause = 0;
	        } else if (s == "%") {
	        	break;
	        } else {
	            // New clause
	            clauses[currentClause].push_back(stoi(s));
	            int var;
	            while (stream >> var && var != 0) {
	                clauses[currentClause].push_back(var);
	                if (var < 0) {
						varClauses[0][-var - 1].insert(currentClause);
					} else {
						varClauses[1][var - 1].insert(currentClause);
					}
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

    inline uint satCount(uint clause, bool* values) {
    	uint c = 0;
		for (int var : clauses[clause]) {
			if (var < 0) {
				if (!values[-var - 1])
					c++;
			} else {
				if (values[var - 1])
					c++;
			}
		}
		return c;
	}
    
    inline bool isSat(uint clause, bool* values) {
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

	inline void resetSolution(bool* solution) {
	    for (uint i = 0; i < v; i++)
	        solution[i] = rand() % 2;
	}

	inline void copySolution(bool* orig, bool* dest) {
		for (uint i = 0; i < v; i++)
			dest[i] = orig[i];
	}
	
};

struct Solution {
public:

	uint value;
	bool* attribution = 0;

	Solution (Formula& f) {
		reset(f);
	}

	~Solution() {
		delete [] attribution;
		delete [] satClauses;
	}

	void reset(Formula& f) {
		if (attribution) {
			delete [] attribution;
			delete [] satClauses;
		}
		attribution = new bool[f.v];
		satClauses = new uint[f.c];
		for (int i = 0; i < f.v; i++)
			attribution[i] = rand() % 2;
		updateSat(f);
	}

	void updateSat(Formula& f) {
		value = 0;
		for (uint c = 0; c < f.c; c++) {
			satClauses[c] = f.satCount(c, attribution);
			if (satClauses[c] > 0)
				value++;
		}
	}

	uint testFlip(Formula& f, uint i) {
		uint value = this->value;
		value += testFlip(f, i, 0, attribution[i] == 1);
		value += testFlip(f, i, 1, attribution[i] == 0);
		return value;
	}

	void flip(Formula& f, uint i) {
		attribution[i] = !attribution[i];
		flip(f, i, 0);
		flip(f, i, 1);
	}

	inline uint getUnsat(Formula& f, uint c) {
        for (int i = 0; i < f.c; i++) {
            if (satClauses[i] == 0) {
                if (c > 0)
                    c--;
                else
                    return i;
            }
        }
	}

private:

	uint* satClauses = 0;

	inline void flip(Formula& f, uint i, bool v) {
		for (uint c : f.varClauses[v][i]) {
			if (satClauses[c] == 1) {
				value--;
				satClauses[c] = 0;
			} else if (satClauses[c] == 0 && attribution[i] == v) {
				value++;
				satClauses[c]++;
			}
		}
	}

	inline int testFlip(Formula& f, uint i, bool v, uint increase) {
		int value = 0;
		for (uint c : f.varClauses[v][i]) {
			if (satClauses[c] == 1) {
				value--;
			} else if (satClauses[c] == 0) {
				value += increase;
			}
		}
		return value;
	}
	
};
