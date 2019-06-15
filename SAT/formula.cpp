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

    ~Formula() { 
    	delete [] clauses; 
    	delete [] varClauses[0];
    	delete [] varClauses[1];
    }

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
	            int var = stoi(s);
	            clauses[currentClause].push_back(var);
	            varClauses[var > 0][index(var)].insert(currentClause);
	            while (stream >> var && var != 0) {
	                clauses[currentClause].push_back(var);
	                varClauses[var > 0][index(var)].insert(currentClause);
	            }
	            currentClause++;
	        }
	    }
 	    stream.close();
	}

    uint evaluate(bool* values) const {
        uint e = 0;
        for (uint i = 0; i < c; i++) {
			if (isSat(i, values)) {
				e++;
			}
        }
        return e;
    }

    inline uint satCount(uint clause, bool* values) const {
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
    
    inline bool isSat(uint clause, bool* values) const {
		for (int var : clauses[clause]) {
			if (var < 0) {
				if (!values[-var - 1])
					return true;
			} else {
				if (values[var - 1])
					return true;
			}
		}
		return false;
	}

	inline uint hammingWeight(bool* s) {
		uint h = 0;
		for (uint i = 0; i < v; i++)
			h += s[i];
		return h;
	}

	inline void copySolution(bool* orig, bool* dest) const {
		for (uint i = 0; i < v; i++)
			dest[i] = orig[i];
	}

	inline void resetSolution(bool* solution) const {
		for (int i = 0; i < v; i++)
			solution[i] = rand() % 2;
	}
	
};

struct Solution {
public:

	uint value = 0;
	bool* attribution = 0;

	Solution (const Formula& f) {
		attribution = new bool[f.v];
		satCounts = new uint[f.c];
	}

	~Solution() {
		delete [] attribution;
		delete [] satCounts;
	}

	void reset(const Formula& f) {
		f.resetSolution(attribution);
		updateSat(f);
	}

	void updateSat(const Formula& f) {
		value = 0;
		for (uint c = 0; c < f.c; c++) {
			satCounts[c] = f.satCount(c, attribution);
			value += satCounts[c] > 0;
		}
	}

	inline uint getUnsat(const Formula& f, uint c) const {
        for (int i = 0; i < f.c; i++) {
            if (satCounts[i] == 0) {
                if (c > 0)
                    c--;
                else
                    return i;
            }
        }
	}

	inline uint evaluate(uint c) const {
		uint value = 0;
		for (uint i = 0; i < c; i++)
			if (satCounts[i] > 0)
				value++;
		return value;
	}

	uint testFlip(const Formula& f, uint i) const {
		if (attribution[i]) {
			// Set as false
			return testFlip(f.varClauses[0][i], f.varClauses[1][i]);
		} else {
			// Set as true
			return testFlip(f.varClauses[1][i], f.varClauses[0][i]);
		}
	}

	void flip(const Formula& f, uint i) {
		if (attribution[i]) {
			// Set as false
			flip(f.varClauses[0][i], f.varClauses[1][i]);
		} else {
			// Set as true
			flip(f.varClauses[1][i], f.varClauses[0][i]);
		}
		attribution[i] = !attribution[i];
	}

private:

	uint* satCounts = 0;

	inline int testFlip(const unordered_set<uint>& up, const unordered_set<uint>& down) const {
		uint value = this->value;
		for (uint c : up)
			value += satCounts[c] == 0;
		for (uint c : down)
			value -= satCounts[c] == 1;
		return value;
	}

	inline void flip(const unordered_set<uint>& up, const unordered_set<uint>& down) {
		for (uint c : up) {
			value += satCounts[c] == 0;
			satCounts[c]++;
		}
		for (uint c : down) {
			value -= satCounts[c] == 1;
			satCounts[c]--;
		}
	}
	
};
