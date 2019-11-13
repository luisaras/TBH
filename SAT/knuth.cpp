
struct CellA {
	int f, b, c = 0, l = 0;
};

bool compareVars(int x, int y) { 
    return abs(x) > abs(y); 
} 

uint knuth_a(Formula& formula, bool* solution) {
	// Construct lists
	const uint n = 2 * formula.v;
	uint size[formula.c], start[formula.c];
	vector<CellA> cells(n + formula.litCount());
	// Literal cells
	for (uint v = 0; v < formula.v; v++) {
		uint pos = v * 2;
		uint neg = pos + 1;
		cells[pos].l = cells[pos].f = cells[pos].b = pos;
		cells[neg].l = cells[neg].f = cells[neg].b = neg;
	}
	// Clause cells
	int offset = n;
	for (uint c = 0; c < formula.c; c++) {
		size[c] = formula.clauses[c].size();
		start[c] = offset;
		sort(formula.clauses[c].begin(), formula.clauses[c].end(), compareVars);
		cout << "Clause " << c << "(START=" << offset << ", SIZE=" << size[c] << "): ";
		for (int lit : formula.clauses[c]) {
			CellA& cell = cells[offset];
			cell.l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
			cell.c = c;
			cout << cell.l << " ";
			CellA& litCell = cells[cell.l];
			litCell.c++;
			cell.b = litCell.b;
			cell.f = cell.l; // Head
			litCell.b = cells[litCell.b].f = offset; // Tail
			offset++;
		}
		cout << endl;
	}

	// Print table
	for (uint i = 0; i < cells.size(); i++)
		cout << "\t" << i;
	cout << endl << "L:";
	for (uint i = 0; i < cells.size(); i++)
		cout << "\t" << cells[i].l;
	cout << endl << "F:";
	for (uint i = 0; i < cells.size(); i++)
		cout << "\t" << cells[i].f;
	cout << endl << "B:";
	for (uint i = 0; i < cells.size(); i++)
		cout << "\t" << cells[i].b;
	cout << endl << "C:";
	for (uint i = 0; i < cells.size(); i++)
		cout << "\t" << cells[i].c;

	cout << endl << endl;

	// Algoritm A

	uint a, d, l, moves[formula.v];

	initialize: { // A1
		a = formula.c;
		d = 0;
	}

	choose: { // A2
		cout << " CHOOSE: "
			 << "a=" << a 
			 << "; d=" << d 
			 << endl; 
		l = 2 * d; // Pos lit
		l += cells[l].c <= cells[l + 1].c; // or neg lit
		moves[d] = l & 1 + // 0 if positive, 1 if negative
			4 * (cells[l ^ 1].c == 0); // if the opposite literal does not appear
		if (cells[l].c == a) {
			goto sucess;
		}
	}

	remove: { // A3
		cout << "REMOVE: "
			 << "l^1=" << (l ^ 1)
			 << "; m_" << d << "=" << moves[d]
			 << "; C(l^1)=" << cells[l ^ 1].c
			 << endl; 
		// Remove l^1 from clauses
		uint p = cells[l ^ 1].f; // visit all cells pointed by cell l^1
		while (p >= n) {
			//cout << p << endl;
			uint clause = cells[p].c;
			//cout << clause << " " << size[clause] << endl;
			if (size[clause] > 1) {
				size[clause]--;
				p = cells[p].f;
			} else {
				p = cells[p].b;
				// Revert
				while (p >= n) {
					//cout << p << endl;
					uint clause = cells[p].c;
					size[clause]++;
					p = cells[p].b;
				}
				goto retry;
			}
		}
	}

	deactivate: { // A4
		cout << "DEACTIVATE: "
			 << "C(l)=" << cells[l].c
			 << endl;
		uint p = cells[l].f; // visit all cells pointed by cell l
		while (p >= n) {
			uint clause = cells[p].c;
			uint i = start[clause];
			p = cells[p].f;
			for (int s = i; s < i + size[clause] - 1; s++) {
				CellA& cell = cells[s];
				uint q = cell.f;
				uint r = cell.b;
				cells[q].b = r;
				cells[r].f = q;
				cells[cell.l].c--;
			}
		}
		a -= cells[l].c;
		d++;
		goto choose;
	}

	retry: { // A5
		cout << " RETRY: "
			 << "l=" << l
			 << "; m_" << d << "=" << moves[d]
			 << endl;
		if (moves[d] < 2) {
			moves[d] = 3 - moves[d];
			l = 2 * d + (moves[d] & 1);
			goto remove;
		}
	}

	backtrack: { // A6
		cout << "BACKTRACK: "
			 << "d=" << d
			 << endl;
		if (d == 0)
			goto fail;
		d--;
		l = 2 * d + (moves[d] & 1);
	}

	reactivate: { // A7
		cout << "REACTIVATE: "
			 << "l=" << l
			 << endl;
		a += cells[l].c;
		uint p = cells[l].b; // visit all cells pointed by cell l
		while (p >= n) {
			uint clause = cells[p].c;
			uint i = start[clause];
			p = cells[p].b;
			for (int s = i; s < i + size[clause] - 1; s++) {
				CellA& cell = cells[s];
				uint q = cell.f;
				uint r = cell.b;
				cells[q].b = cells[r].f = s;
				cells[cell.l].c++;
			}
		}
	}

	unremove: { // A8
		// Reinsert l^1 in clauses
		cout << "UNREMOVE: "
			 << "(l^1)=" << (l ^ 1)
			 << "; a=" << a
			 << endl;
		uint p = cells[l ^ 1].f; // visit all cells pointed by cell l^1
		while (p >= n) {
			uint clause = cells[p].c;
			size[clause]++;
			p = cells[p].f;
		}
		goto retry;
	}

	sucess:
		cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = !(moves[v] & 1);
			cout << " " << solution[v];
		}
		cout << endl;
		return formula.evaluate(solution);

	fail:
		return 0;
}