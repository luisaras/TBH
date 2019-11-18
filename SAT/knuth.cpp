
#define PRINT 1

struct CellA {
	int f, b, c = 0, l = 0;
};

bool compareVars(int x, int y) { 
    return abs(x) > abs(y); 
}

inline bool assert(bool v) {
    if (!v)
        exit(-1);
}

uint knuth_a(Formula& formula, bool* solution) {
	// Construct lists
	const uint n = 2 * formula.v;
	uint size[formula.c], start[formula.c];
	CellA cells[n + formula.litCount()];
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
		if (PRINT)
			cout << "Clause " << c 
				 << "(START=" << offset << 
				 ", SIZE=" << size[c] << "): ";
		for (int lit : formula.clauses[c]) {
			CellA& cell = cells[offset];
			cell.l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
			cell.c = c;
			if (PRINT)
				cout << cell.l << " ";
			CellA& litCell = cells[cell.l];
			litCell.c++;
			cell.b = litCell.b;
			cell.f = cell.l; // Head
			litCell.b = cells[litCell.b].f = offset; // Tail
			offset++;
		}
		if (PRINT)
			cout << endl;
	}

	// Print table
	if (PRINT) {
		for (uint i = 0; i < offset; i++)
			cout << "\t" << i;
		cout << endl << "L:";
		for (uint i = 0; i < offset; i++)
			cout << "\t" << cells[i].l;
		cout << endl << "F:";
		for (uint i = 0; i < offset; i++)
			cout << "\t" << cells[i].f;
		cout << endl << "B:";
		for (uint i = 0; i < offset; i++)
			cout << "\t" << cells[i].b;
		cout << endl << "C:";
		for (uint i = 0; i < offset; i++)
			cout << "\t" << cells[i].c;
		cout << endl << endl;
	}

	// Algoritm A

	uint a, d, l, moves[formula.v];

	initialize: { // A1
		a = formula.c;
		d = 0;
	}

	choose: { // A2
		if (PRINT)
			cout << " CHOOSE: "
				 << "a=" << a 
				 << "; d=" << d 
				 << endl; 
		l = 2 * d; // Pos lit
		l += cells[l].c <= cells[l + 1].c; // or neg lit
		moves[d] = l & 1 + // 0 if positive, 1 if negative
			4 * (cells[l ^ 1].c == 0); // if the opposite literal does not appear
		if (cells[l].c == a) {
			goto success;
		}
	}

	remove: { // A3
		if (PRINT)
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
		if (PRINT)
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
		if (PRINT)
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
		if (PRINT)
			cout << "BACKTRACK: "
				 << "d=" << d
				 << endl;
		if (d == 0)
			goto fail;
		d--;
		l = 2 * d + (moves[d] & 1);
	}

	reactivate: { // A7
		if (PRINT)
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
		if (PRINT)
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

	success: {
		if (PRINT)
			cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = !(moves[v] & 1);
			if (PRINT)
				cout << " " << solution[v];
		}
		if (PRINT)
			cout << endl;
		return formula.evaluate(solution);
    }

	fail: {
		return 0;
    }
    
}

uint knuth_b(Formula& formula, bool* solution) {
    // Watch lists (list of clauses that watch l)
    int w[formula.v * 2];
    for (uint l = 0; l < formula.v * 2; l++)
        w[l] = -1;
    int link[formula.c];
    // Clause cells
    uint start[formula.c + 1];
    int L[formula.litCount()];
    uint offset = 0;
    for (uint c = 0; c < formula.c; c++) {
		start[c] = offset;
        sort(formula.clauses[c].begin(), formula.clauses[c].end(), compareVars);
		if (PRINT)
			cout << "Clause " << c 
				 << "(START=" << offset << 
				 ", SIZE=" << formula.clauses[c].size() << "): ";
		for (int lit : formula.clauses[c]) {
            uint l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
            L[offset] = l;
			if (PRINT)
				cout << l << " ";
            offset++;
		}
		if (PRINT)
			cout << endl;
        uint wl = L[start[c]]; // First literal
        // Insert in the head of w[wl]
        link[c] = w[wl];
        w[wl] = c;
    }
    start[formula.c] = offset;
    
    uint d, l, moves[formula.v];
    
    initialize: { // B1
        d = 0;
    }
    
    choose: { // B2
		if (PRINT)
			cout << " CHOOSE: "
				 << "d=" << d
				 << endl; 
        if (d >= formula.v)
            goto success;
        moves[d] = w[2*d] == -1 || w[2*d + 1] != -1;
        l = 2 * d + moves[d];
    }
    
    remove: { // B3
		if (PRINT)
			cout << "REMOVE: "
				 << "l=" << l
				 << "; m_" << d << "=" << moves[d]
				 << endl; 
        // Remove l^1 from all clauses watch lists
        int j = w[l^1]; // First clause that watches !l
        while (j != -1) {
            int i = start[j]; // First cell of that clause
            assert(j < formula.c);
            int maxi = start[j+1]; // First cell of the previous clause
            int jj = link[j]; // Next clause that watches !l
            for (int k = i + 1; true; k++) {
                if (k == maxi) {
                    w[l^1] = j;
                    goto retry;
                }
                int ll = L[k]; // Other possible literal for that clause to watch
                if ((ll / 2) > d || (moves[ll / 2] + ll) % 2 == 0) {
                    // Change watched literal
                    L[i] = ll;
                    L[k] = l^1;
                    link[j] = w[ll];
                    w[ll] = j;
                    j = jj;
                    break;
                }
            }
        }
    }
    
    advance: { // B4
		if (PRINT)
			cout << "ADVANCE: "
				 << "d=" << d
                 << "; l^1=" << (l^1)
				 << endl; 
        w[l^1] = -1;
        d++;
        goto choose;
    }
    
    retry: { // B5
		if (PRINT)
			cout << "RETRY: "
				 << "l=" << l
				 << "; m_" << d << "=" << moves[d]
				 << endl; 
        if (moves[d] < 2) {
            moves[d] = 3 - moves[d];
            l = 2*d + (moves[d] & 1);
            goto remove;
        }
    }
    
    backtrack: { // B6
        if (PRINT)
			cout << "BACKTRACK: "
				 << "d=" << d
				 << endl; 
        if (d == 0) {
            goto fail;
        }
        d--;
        goto retry;
    }
    
	success: {
		if (PRINT)
			cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = !(moves[v] & 1);
			if (PRINT)
				cout << " " << solution[v];
		}
		if (PRINT)
			cout << endl;
		return formula.evaluate(solution);
    }
    
    fail: {
        return 0;
    }
}

uint knuth_d(Formula& formula, bool* solution) {
    // Watch lists (list of clauses that watch l)
    int w[formula.v * 2 + 2];
    for (uint l = 0; l < formula.v * 2 + 2; l++)
        w[l] = 0;
    int link[formula.c+1];
    // Clause cells
    uint start[formula.c+2];
    int L[formula.litCount()];
    uint offset = 0;
    for (uint c = 1; c <= formula.c; c++) {
		start[c] = offset;
        sort(formula.clauses[c-1].begin(), formula.clauses[c-1].end(), compareVars);
		if (PRINT)
			cout << "Clause " << c 
				 << "(START=" << offset << 
				 ", SIZE=" << formula.clauses[c-1].size() << "): ";
		for (int lit : formula.clauses[c-1]) {
            uint l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
            L[offset] = l + 2;
			if (PRINT)
				cout << L[offset] << " ";
            offset++;
		}
		if (PRINT)
			cout << endl;
        uint wl = L[start[c]]; // First literal
        // Insert in the head of w[wl]
        link[c] = w[wl];
        w[wl] = c;
    }
    start[formula.c+1] = offset;
    
    int d, h, t, k, f, moves[formula.v+1], h_d[formula.v+1], values[formula.v+1], next[formula.v+1];
    
    auto m = [&](int d) -> int& {
    	return moves[d];
    };

    auto x = [&](int d) -> int& {
    	return values[h_d[d]];
    };

    auto h_is_unit = [&](int ll) -> int {
        int j = w[ll];
        while (j != 0) {
            int p = start[j] + 1;
            if (p == start[j + 1])
                return 1;
            while (x(L[p] / 2) == L[p] & 1) {
                p++;
                if (p == start[j + 1])
                    return 1;
            }
            j = link[j];
        }
        return 0;
    };
    
    initialize: { // D1
    	moves[0] = d = h = t = 0;
        for (k = formula.v; k >= 1; k--) {
            h_d[k] = k;
            x(k) = -1;
            if (w[2 * k] != 0 || w[2 * k + 1] != 0) {
                next[k] = h;
                h = k;
                if (t == 0)
                    t = k;
            }
        }
        if (t != 0)
        	next[t] = h;
    }
    
    check_success: { // D2
    	if (PRINT)
    		cout << "SUCCESS?: "
    			 << "d=" << d
    			 << "; h[d+1]=" << h_d[d+1]
    			 << endl;
        if (t == 0)
            goto success;
        k = t;
    }
    
    look: { // D3
    	if (PRINT)
    		cout << "LOOK FOR UNIT: "
    			 << "k=" << k
    			 << "; next[k]=" << next[k]
    			 << endl;
        h = next[k];
        int f = h_is_unit(2 * h) + 2 * h_is_unit(2 * h + 1);
        if (f == 3)
            goto backtrack;
        if (f == 0)
            if (h != t) {
                k = h;
                goto look;
            }
        else {
            m(d+1) = f + 3;
            t = k;
            goto moveon;
        }   
    }
    
    branch: { // D4
      	if (PRINT)
    		cout << "BRANCH: "
    			 << "t=" << t
    			 << "; next[t]=" << next[t]
    			 << endl;
        h = next[t];
        m(d+1) = w[2*h] == 0 || w[2*h+1] != 0;
    }
    
    moveon: { // D5
      	if (PRINT)
    		cout << "MOVE ON: "
    			 << "d=" << d
    			 << "; k=" << k
    			 << "; h=" << h
    			 << "; t=" << t
    			 << endl;
        d++;
        h_d[d] = k = h;
        if (t == k) {
            t = 0;
        } else {
            next[t] = h = next[k];
        }
    }
    
    update: { // D6

        int b = (m(d)+1)%2;
        x(k) = b;
        int l = 2*k + b;
        int j = w[l];
        w[l] = 0;
      	if (PRINT)
    		cout << "UPDATE: "
    			 << "d=" << d
    			 << "; m(d)=" << m(d)
    			 << "; l=" << l
    			 << endl;
        while (j != 0) {
            int jj = link[j];
            int i = start[j];
            int p = i + 1;
            while (x(L[p]>>1) != (L[p]&1))
                p++;
            int ll = L[p];
            L[p] = l;
            L[i] = ll;
            p = w[ll];
            int q = w[l^1];
            if (p == 0 && q == 0 && x(ll / 2) == -1) {
                if (t == 0) {
                    t = h = ll / 2;
                } else {
                    next[ll / 2] = h;
                    h = ll / 2;
                }
                next[t] = h;
            }
            link[j] = p;
            w[ll] = j;
            j = jj;
        }
        goto check_success;
    }
    
    backtrack: { // D7
      	if (PRINT)
    		cout << "BACKTRACK: "
    			 << "d=" << d
    			 << "; m(d)=" << m(d)
    			 << "; k=" << k
    			 << endl;
        t = k;
        while(m(d) >= 2) {
            k = h_d[d];
            x(k) = -1;        
            if (w[2*k] != 0 || w[2*k+1] != 0) {
                next[k] = h;
                h = k;
                next[t] = h;
            }
            d--;
        }
    }
    
    check_failure: { // D8
      	if (PRINT)
    		cout << "FAILURE?: "
    			 << "d=" << d
    			 << "; m(d)=" << m(d)
    			 << "; h[d]=" << h_d[d]
    			 << endl;
        if (d > 0) {
            m(d) = 3 - m(d);
            k = h_d[d];
            goto update;
        }
        goto fail;
    }
    
	success: {
		if (PRINT)
			cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = !values[v+1];
			if (PRINT)
				cout << " " << solution[v];
		}
		if (PRINT)
			cout << endl;
		return formula.evaluate(solution);
    }

	fail: {
		return 0;
    }
    
}