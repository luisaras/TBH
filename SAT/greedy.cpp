#include "formula.cpp"

struct Var {
	Var (int i) { id = 1; }
	int id;
	int w = 0;
};

struct wcomp {
    bool operator() (const Var& lv, const Var& rv) const {
        return lv.w > rv.w;
    }
};

void greedy(Formula& formula, bool* solution, uint k) {
	
	// Set of undefined variables, ordenered by w
	set<Var, wcomp> N;
	for (int i = 0; i < formula.v; i++) {
		Var vp(i + 1);
		Var vn(-i - 1);
		for (int c : formula.varClauses[c]) {
			if (c > 0)
				vp.w++;
			else
				vn.w++;
		}
		N.insert(vp);
		N.insert(vn);
	}
	
	
	
	for (int i = 0; i < formula.v; i++) {
		uint w = formula.testFlip(i, solution);
		u.insert(i);
	}
	while (!N.empty()) {
		
	}
}
