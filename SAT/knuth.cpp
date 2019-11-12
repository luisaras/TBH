
struct CellA {
	int f, b, c = 0, l = 0;
};

bool compareVars(int x, int y) { 
    return abs(x) > abs(y); 
} 

uint knuth_a(Formula& formula, bool* solution) {
	// Construct lists
	const uint n = 2 * formula.v + 2;
	uint size[formula.c], start[formula.c];
	vector<CellA> cells(n + formula.litCount());
	// Literal cells
	for (uint v = 0; v < formula.v; v++) {
		uint pos = v * 2 + 2;
		uint neg = pos + 1;
		cells[pos].l = pos - 2;
		cells[pos].f = cells[pos].b = pos;
		cells[neg].l = neg - 2;
		cells[neg].f = cells[neg].b = neg;
	}
	// Clause cells
	int offset = n;
	for (uint c = 0; c < formula.c; c++) {
		size[c] = formula.clauses[c].size();
		start[c] = offset;
		sort(formula.clauses[c].begin(), formula.clauses[c].end(), compareVars);
		for (int lit : formula.clauses[c]) {
			CellA& cell = cells[offset];
			cell.l = lit > 0 ? 2 * (lit + 1) : 2 * (-lit - 1) + 1;
			cell.c = c;
			CellA& litCell = cells[2 + cell.l];
			litCell.c++;
			cell.b = litCell.b;
			cell.f = 2 + cell.l; // Head
			cells[litCell.b].f = litCell.b = offset; // Tail
			offset++;
		}
	}


	// Algoritm A

	uint a, d, l, moves[formula.v];

	initialize: { // A1
		a = formula.c;
		d = 1;
	}

	choose: { // A2
		l = 2 * d;
		if (cells[l].c <= cells[l + 1].c)
			l++;
		moves[d] = l & 1; // 0 if positive, 1 if negative
		cout << cells[l].c << " " << a << endl;
		if (cells[l ^ 1].c == 0) // if the opposite literal does not appear
			moves[d] += 4;
		if (cells[l].c == a) {
			goto sucess;
		}
	}

	remove: { // A3
		// Remove l^1 from clauses
		uint p = cells[l ^ 1].f; // visit all cells pointed by cell l^1
		cout << p << endl;
		while (p >= n) {
			uint clause = cells[p].c;
			cout << clause << size[clause] << endl;
			if (size[clause] > 1) {
				size[clause]--;
				p = cells[p].f;
			} else {
				p = cells[p].b;
				// Revert
				while (p >= n) {
					uint clause = cells[p].c;
					size[clause]++;
					p = cells[p].b;
				}
				goto retry;
			}
		}
	}

	deactivate: { // A4
		uint p = cells[l].f; // visit all cells pointed by cell l
		while (p >= n) {
			uint clause = cells[p].c;
			p = cells[p].f;
			for (int s = 0; s < size[clause] - 1; s++) {
				CellA& cell = cells[start[clause] + s];
				uint q = cell.f;
				uint r = cell.b;
				cells[q].b = r;
				cells[r].f = q;
				cells[cell.l].c--;
			}
		}
		a = a - cells[l].c;
		d = d + 1;
		goto choose;
	}

	retry: { // A5
		if (moves[d] < 2) {
			moves[d] = 3 - moves[d];
			l = 2 * d + (moves[d] & 1);
			goto remove;
		}
	}

	backtrack: { // A6
		if (d == 1)
			goto fail;
		cout << d << endl;
		l = 2 * d + (moves[d] & 1);
	}

	reactivate: { // A7
		uint p = cells[l].b; // visit all cells pointed by cell l
		while (p >= n) {
			uint clause = cells[p].c;
			p = cells[p].b;
			for (int s = 0; s < size[clause] - 1; s++) {
				CellA& cell = cells[start[clause] + s];
				uint q = cell.b;
				uint r = cell.f;
				cells[q].b = cells[r].f = start[clause] + s;
				cells[cell.l].c++;
			}
		}
		a = a + cells[l].c;
	}

	unremove: { // A8
		// Reinsert l^1 in clauses
		uint p = cells[l ^ 1].f; // visit all cells pointed by cell l^1
		while (p >= n) {
			uint clause = cells[p].c;
			size[clause]++;
		}
		goto retry;
	}

	sucess:
		for (uint v = 0; v < formula.v; v++)
			solution[v] = moves[v] & 1;
		return formula.evaluate(solution);

	fail:
		return 0;
}