#define DEBUG 0
#define ALL_SOLUTIONS 1

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
		if (DEBUG)
			cout << "Clause " << c 
				 << "(START=" << offset << 
				 ", SIZE=" << size[c] << "): ";
		for (int lit : formula.clauses[c]) {
			CellA& cell = cells[offset];
			cell.l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
			cell.c = c;
			if (DEBUG)
				cout << cell.l << " ";
			CellA& litCell = cells[cell.l];
			litCell.c++;
			cell.b = litCell.b;
			cell.f = cell.l; // Head
			litCell.b = cells[litCell.b].f = offset; // Tail
			offset++;
		}
		if (DEBUG)
			cout << endl;
	}

	// DEBUG table
	if (DEBUG) {
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

	uint a, d, l, moves[formula.v], n_solutions = 0;

	initialize: { // A1
		a = formula.c;
		d = 0;
	}

	choose: { // A2 or A2*
		if (DEBUG)
			cout << " CHOOSE: "
				 << "a=" << a 
				 << "; d=" << d 
				 << endl; 

		// Choose literal
		l = 2 * d; // positive lit

		if (ALL_SOLUTIONS) {
			if (d == formula.v) {
				// Visit solution
				for (uint v = 0; v < formula.v; v++) {
					solution[v] = !(moves[v] & 1);
					cout << " " << solution[v];
				}
				cout << endl;
				n_solutions++;
				goto backtrack;
			}
			moves[d] = 0;
		} else {
			l += cells[l].c <= cells[l+1].c; // or negative lit, if it occurs more times
			moves[d] = l & 1 // 0 if positive, 1 if negative
					 + 4 * (cells[l^1].c == 0); // if the opposite literal does not appear
			if (cells[l].c == a) {
				goto success;
			}
		}

	}

	remove: { // A3
		if (DEBUG)
			cout << "REMOVE: "
				 << "l^1=" << (l ^ 1)
				 << "; m_" << d << "=" << moves[d]
				 << "; C(l^1)=" << cells[l ^ 1].c
				 << endl; 

		// Remove l^1 from clauses
		uint p = cells[l ^ 1].f; // visit all cells pointed by cell l^1
		while (p >= n) {
			uint clause = cells[p].c;
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
		if (DEBUG)
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
		if (DEBUG)
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
		if (DEBUG)
			cout << "BACKTRACK: "
				 << "d=" << d
				 << endl;
		if (d == 0)
			goto terminate;
		d--;
		l = 2 * d + (moves[d] & 1); // Get previously chosen literal
	}

	reactivate: { // A7
		if (DEBUG)
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
		if (DEBUG)
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
		if (DEBUG)
			cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = !(moves[v] & 1);
			if (DEBUG)
				cout << " " << solution[v];
		}
		if (DEBUG)
			cout << endl;
		return formula.evaluate(solution);
    }

	terminate: {
		return n_solutions;
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
		if (DEBUG)
			cout << "Clause " << c 
				 << "(START=" << offset << 
				 ", SIZE=" << formula.clauses[c].size() << "): ";
		for (int lit : formula.clauses[c]) {
            uint l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
            L[offset] = l;
			if (DEBUG)
				cout << l << " ";
            offset++;
		}
		if (DEBUG)
			cout << endl;
        uint wl = L[start[c]]; // First literal
        // Insert in the head of w[wl]
        link[c] = w[wl];
        w[wl] = c;
    }
    start[formula.c] = offset;
    
    if (DEBUG) {
		for (uint i = 0; i < offset; i++)
			cout << "\t" << i;
		cout << endl << "L:";
		for (uint i = 0; i < offset; i++)
			cout << "\t" << L[i];
		cout << endl;

		for (uint i = 0; i < formula.v * 2; i++)
			cout << (i%2 ? "\t" : "\t!") << (i/2);
		cout << endl << "w:";
		for (uint i = 0; i < formula.v * 2; i++)
			cout << "\t" << w[i];
		cout << endl;

		for (uint i = 0; i < formula.c; i++)
			cout << "\t" << i;
		cout << endl << "link:";
		for (uint i = 0; i < formula.c; i++)
			cout << "\t" << link[i];
		cout << endl << endl;
    }

    uint d, l, moves[formula.v];
    
    initialize: { // B1
        d = 0;
    }
    
    choose: { // B2
		if (DEBUG)
			cout << " CHOOSE: "
				 << "d=" << d
				 << endl; 
        if (d >= formula.v)
            goto success;
        moves[d] = w[2*d] == -1 || w[2*d + 1] != -1;
        l = 2 * d + moves[d];
    }
    
    remove: { // B3
		if (DEBUG)
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
		if (DEBUG)
			cout << "ADVANCE: "
				 << "d=" << d
                 << "; l^1=" << (l^1)
				 << endl; 
        w[l^1] = -1;
        d++;
        goto choose;
    }
    
    retry: { // B5
		if (DEBUG)
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
        if (DEBUG)
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
		if (DEBUG)
			cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = !(moves[v] & 1);
			if (DEBUG)
				cout << " " << solution[v];
		}
		if (DEBUG)
			cout << endl;
		return formula.evaluate(solution);
    }
    
    fail: {
        return 0;
    }
}

uint knuth_b_star(Formula& formula, bool* solution) {
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
		if (DEBUG)
			cout << "Clause " << c 
				 << "(START=" << offset << 
				 ", SIZE=" << formula.clauses[c].size() << "): ";
		for (int lit : formula.clauses[c]) {
            uint l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
            L[offset] = l;
			if (DEBUG)
				cout << l << " ";
            offset++;
		}
		if (DEBUG)
			cout << endl;
        uint wl = L[start[c]]; // First literal
        // Insert in the head of w[wl]
        link[c] = w[wl];
        w[wl] = c;
    }
    start[formula.c] = offset;
    
    if (DEBUG) {
		for (uint i = 0; i < offset; i++)
			cout << "\t" << i;
		cout << endl << "L:";
		for (uint i = 0; i < offset; i++)
			cout << "\t" << L[i];
		cout << endl;

		for (uint i = 0; i < formula.v * 2; i++)
			cout << (i%2 ? "\t" : "\t!") << (i/2);
		cout << endl << "w:";
		for (uint i = 0; i < formula.v * 2; i++)
			cout << "\t" << w[i];
		cout << endl;

		for (uint i = 0; i < formula.c; i++)
			cout << "\t" << i;
		cout << endl << "link:";
		for (uint i = 0; i < formula.c; i++)
			cout << "\t" << link[i];
		cout << endl << endl;
    }

    uint d, l, moves[formula.v];
    
    initialize: { // B1
        d = 0;
    }
    
    choose: { // B2
		if (DEBUG)
			cout << " CHOOSE: "
				 << "d=" << d
				 << endl; 
        if (d >= formula.v)
            goto success;
        moves[d] = w[2*d] == -1 || w[2*d + 1] != -1;
        l = 2 * d + moves[d];
    }
    
    remove: { // B3
		if (DEBUG)
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
		if (DEBUG)
			cout << "ADVANCE: "
				 << "d=" << d
                 << "; l^1=" << (l^1)
				 << endl; 
        w[l^1] = -1;
        d++;
        goto choose;
    }
    
    retry: { // B5
		if (DEBUG)
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
        if (DEBUG)
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
		if (DEBUG)
			cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = !(moves[v] & 1);
			if (DEBUG)
				cout << " " << solution[v];
		}
		if (DEBUG)
			cout << endl;
		return formula.evaluate(solution);
    }
    
    fail: {
        return 0;
    }
}

uint knuth_d(Formula& formula, bool* solution) {
    // Watch lists (list of clauses that watch l)
    const int lit_count = formula.litCount();
    int w[formula.v * 2];
    for (uint l = 0; l < formula.v * 2; l++)
        w[l] = -1;
    int link[formula.c];
    // Clause cells
    uint start[formula.c+1];
    int L[lit_count];
    uint offset = 0;
    for (uint c = 0; c < formula.c; c++) {
		start[c] = offset;
        sort(formula.clauses[c].begin(), formula.clauses[c].end(), compareVars);
		if (DEBUG)
			cout << "Clause " << c 
				 << "(START=" << offset << 
				 ", SIZE=" << formula.clauses[c].size() << "): ";
		for (int lit : formula.clauses[c]) {
            uint l = lit > 0 ? 2 * (lit - 1) : 2 * (-lit - 1) + 1;
            L[offset] = l;
			if (DEBUG)
				cout << l << " ";
            offset++;
		}
		if (DEBUG)
			cout << endl;
        uint wl = L[start[c]]; // First literal
        // Insert in the head of w[wl]
        link[c] = w[wl];
        w[wl] = c;
    }
    start[formula.c] = offset;

    if (DEBUG) {
		for (uint i = 0; i < lit_count; i++)
			cout << "\t" << i;
		cout << endl << "L:";
		for (uint i = 0; i < lit_count; i++)
			cout << "\t" << L[i];
		cout << endl;

		for (uint i = 0; i < formula.v * 2; i++)
			cout << (i%2 ? "\t" : "\t!") << (i/2);
		cout << endl << "w:";
		for (uint i = 0; i < formula.v * 2; i++)
			cout << "\t" << w[i];
		cout << endl;

		for (uint i = 0; i < formula.c; i++)
			cout << "\t" << i;
		cout << endl << "link:";
		for (uint i = 0; i < formula.c; i++)
			cout << "\t" << link[i];
		cout << endl;

    }
    
    int d, h, t, k, moves[formula.v], d_var[formula.v], values[formula.v], next[formula.v];
    // d is the search depth
    // h, t are the first and last elements of the ring
    // k is the current variable being set
    // moves are the state of each variable

    // Checks if the given literal appears in unit clauses
    auto h_is_unit = [&](int l) -> int {
        int j = w[l];
        while (j != -1) {
            int p = start[j] + 1;
            if (p == start[j + 1])
                return 1;
            while (values[L[p]>>1] == (L[p]&1)) {
                p++;
                if (p == start[j + 1])
                    return 1;
            }
            j = link[j];
        }
        return 0;
    };
    
    initialize: { // D1
    	h = t = -1;
        for (k = formula.v - 1; k >= 0; k--) {
            values[k] = -1;
            // If l or !l are being watched
            if (w[2 * k] != -1 || w[2 * k + 1] != -1) {
                next[k] = h;
                h = k;
                if (t == -1)
                    t = k;
            } else {
            	next[k] = -1;
            }
        }
        if (t != -1)
        	next[t] = h; // Loop ring
        d = -1;
    }

    if (DEBUG) {
		for (uint i = 0; i < formula.v; i++)
			cout << "\t" << i;
		cout << endl << "next:";
		for (uint i = 0; i < formula.v; i++)
			cout << "\t" << next[i];
		cout << endl << endl;
    }
    
    check_success: { // D2
    	if (DEBUG)
    		cout << "SUCCESS?: "
    			 << "d=" << d
    			 << "; t=" << t
    			 << endl;
        if (t == -1) // No more undefined variables
            goto success;
        k = t; // Current variable is the last one in the ring
    }
    
    look: { // D3
        h = next[k]; // Move the head to the next variable
        int f = h_is_unit(2 * h) + 2 * h_is_unit(2 * h + 1);
    	if (DEBUG)
    		cout << "LOOK FOR UNIT: "
    			 << "k=" << k
    			 << "; next[k]=" << next[k]
    			 << "; f=" << f
    			 << endl;
        if (f == 3)
        	// Both l and !l appear in unit clauses (bad!)
            goto backtrack;
        if (f == 0)
        	// Neither appear in unit clauses
            if (h != t) {
            	// Head did not reach last element (we still have to search for variables in unit clauses)
                k = h;
                goto look;
            } else {
            	// No variable is in unit clauses; we need to branch
            	goto branch;
            }
        else {
        	// Either l (f=1) or !l (f=2) appears in unit clauses
            moves[d+1] = f + 3;
            t = k;
            goto moveon;
        }
    }
    
    branch: { // D4
      	if (DEBUG)
    		cout << "BRANCH: "
    			 << "t=" << t
    			 << "; next[t]=" << next[t]
    			 << endl;
        h = next[t]; // Reset h to the beginning of the ring
        moves[d+1] = w[2*h] == -1 || w[2*h+1] != -1; // Set move as 0 or 1
    }
    
    moveon: { // D5
      	if (DEBUG)
    		cout << "MOVE ON: "
    			 << "d=" << d
    			 << "; h=" << h
    			 << "; t=" << t
    			 << endl;
        d++;
        d_var[d] = k = h; // Select first variable in the ring
        if (t == h) { 
        	// k is the last variable in the ring
            t = -1;
        } else {
        	// Remove k from the ring
        	h = next[k];
            next[t] = h;
        }
    }
    
    update: { // D6
        bool value = !(moves[d] & 1); // 1 is positive, 0 is negative
        values[k] = value;
        int l = 2*k + value; // negation of chosen literal
        int j = w[l];
        w[l] = -1; // Erase l's watch list
      	if (DEBUG)
    		cout << "UPDATE: "
    			 << "d=" << d
    			 << "; k=" << k
    			 << "; b=" << value
    			 << "; l^1=" << l
    			 << endl;
        while (j != -1) {
            int i = start[j];
            // Pick a literal that was not falsified yet
            int p = i + 1;
            while (values[L[p]>>1] == (L[p]&1)) // L[p]&1 is 1 if negative and 0 if position
                p++; 
            // NOTE: always finds one, otherwise it would be an unit clause and detected at D3
            int ll = L[p]; // Next watched literal
            L[p] = l; // Old watched literal
            L[i] = ll; // Replace
            p = w[ll]; // Watchlist of the new watched literal
            int q = w[ll^1]; // Watchlist of the old watched literal negated
            if (p == -1 && q == -1 && values[ll>>1] == -1) {
            	// If new watched literal is not defined and its watchlists were empty,
            	// insert new variable (from new watched literal) in the ring
                if (t == -1) { // If ring is empty
                    t = h = ll / 2;
                } else {
                    next[ll / 2] = h;
                    h = ll / 2;
                }
                next[t] = h;
            }
            // Update watchlist of new watched literal
            int jj = link[j];
            link[j] = p;
            w[ll] = j;
            j = jj;
        }
        goto check_success;
    }
    
    backtrack: { // D7
      	if (DEBUG)
    		cout << "BACKTRACK: "
    			 << "d=" << d
    			 << "; m=" << moves[d]
    			 << "; k=" << k
    			 << endl;
        t = k; // Set current variable as the last in the ring (?)
        while(d != -1 && moves[d] >= 2) { // Undo all tries that were two-sidely unsucessful
            k = d_var[d];
            values[k] = -1;
            if (w[2*k] != -1 || w[2*k+1] != -1) {
            	// Reinsert k in beginning of the ring if at least one of its watchlists is not empty
                next[k] = h;
                h = k;
                next[t] = h;
            }
            d--;
        }
    }
    
    check_failure: { // D8
      	if (DEBUG)
    		cout << "FAILURE?: "
    			 << "d=" << d
    			 << "; m=" << moves[d]
    			 << "; k=" << d_var[d]
    			 << endl;
        if (d >= 0) {
            moves[d] = 3 - moves[d];
            k = d_var[d];
            goto update;
        } else {
        	goto fail;
    	}
    }
    
	success: {
		if (DEBUG)
			cout << "Solution:";
		for (uint v = 0; v < formula.v; v++) {
			solution[v] = values[v];
			if (DEBUG)
				cout << " " << solution[v];
		}
		if (DEBUG)
			cout << endl;
		return formula.evaluate(solution);
    }

	fail: {
		return 0;
    }
    
}