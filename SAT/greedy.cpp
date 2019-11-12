#include <unordered_map>
	using std::unordered_map;

uint greedy(Formula& formula, bool* solution, double alpha) {
	// Satisfied clauses
	bool sat[formula.c];
	for(int i = 0; i < formula.c; i++)
		sat[i] = false;

	// Set of possible values for each variable
	unordered_set<int> N;
	for (int i = 0; i < formula.v; i++) {
		N.insert(i + 1);
		N.insert(-i - 1);
	}

	while (!N.empty()) {
		unordered_map<int, uint> u;

		// Find best / worst w values
		uint best = 0, worst = -1;
		for (int v : N) {
			uint w = 0;
			// Number of unsat clauses that contain variable v
			if (v > 0) {
				for (uint c : formula.varClauses[1][v - 1])
					w += (sat[c] == 0);
			} else {
				for (uint c : formula.varClauses[0][-v - 1])
					w += (sat[c] == 0);
			}
			// Update best/worst
			if (w > best)
				best = w;
			if (w < worst)
				worst = w;
			u[v] = w;
		}

		// Select alpha% best
		uint limit = ceil(alpha * worst + (1 - alpha) * best);
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
			for (uint c : formula.varClauses[1][v - 1]) {
				sat[c] = true;
			}
		} else {
			// Set as false
			solution[-v - 1] = false;
			for (uint c : formula.varClauses[0][-v - 1]) {
				sat[c] = true;
			}
		}
		N.erase(v);
		N.erase(-v);

	}
	return formula.evaluate(solution);
}