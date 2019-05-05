#include "formula.cpp"
#include <map>
	using std::map;

uint greedy(Formula& formula, bool* solution, double alpha) {
	// Satisfied clauses
	bool sat[formula.c];
	for(int i = 0; i < formula.c; i++)
		sat[i] = false;

	// Set of possible values for each variable
	set<int> N;
	for (int i = 0; i < formula.v; i++) {
		N.insert(i + 1);
		N.insert(-i - 1);
	}

	while (!N.empty()) {
		map<int, uint> u;

		// Find best / worst w values
		uint best = 0, worst = -1;
		for (int v : N) {
			uint w = 0;
			// Number of unsat clauses that contain variable v
			if (v > 0) {
				for (int c : formula.varClauses[v - 1]) {
					if (c > 0 && !sat[c - 1]) {
						w++;
					}
				}
			} else {
				for (int c : formula.varClauses[-v - 1]) {
					if (c < 0 && !sat[-c - 1]) {
						w++;
					}
				}
			}
			// Update best/worst
			if (w > best)
				best = w;
			else if (w < worst)
				worst = w;
			u[v] = w;
		}

		// Select alpha% best
		double limit = alpha * worst + (1 - alpha) * best;
		for (int v : N) {
			if (u[v] < limit)
				u.erase(v);
		}

		// Choose one randomly
		auto item = u.begin();
		std::advance(item, rand() % u.size());
		int v = item->first;

		// Set variable
		if (v > 0) {
			// Set as true
			solution[v - 1] = true;
			for (int c : formula.varClauses[v - 1]) {
				if (c > 0) {
					sat[c - 1] = true;
				}
			}
		} else {
			// Set as false
			solution[-v - 1] = false;
			for (int c : formula.varClauses[-v - 1]) {
				if (c < 0) {
					sat[-c - 1] = true;
				}
			}
		}
		N.erase(v);
		N.erase(-v);

	}
	return formula.evaluate(solution);
}