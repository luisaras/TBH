#include "Graph.cpp"

// indexes[i] = k means that edge i is the k-th edge in the tree. k = -1 if not in the tree.
inline void initializeIndexes(Graph& graph, int* tree, int* indexes) {
    for (int i = 0; i < (int) graph.edges.size(); i++)
        indexes[i] = -1;
    for (int i = 0; i < graph.n - 1; i++)
        indexes[tree[i]] = i;
}

// Auxiliary function.
bool findCycle(int current, int parent, int goal, Graph& graph, int* indexes, set<int>& cycle) {
    for (auto it = graph.nodes[current].begin(); it != graph.nodes[current].end(); ++it) {
        int e = *it; // Neighbor edge
        if (indexes[e] == -1) continue; // Ignore if not in the tree
        int v = graph.edges[e].neighbor(current);
        if (v == parent) continue; // Ignore if current node came from this edge
        if (v == goal || findCycle(v, current, goal, graph, indexes, cycle)) {
            cycle.insert(e);
            return true;
        }
    }
    return false;
}

// Finds the edges in the cycle. O(V).
set<int> findCycle(Graph& graph, int edge, int* indexes) {
    set<int> cycle;
    int start = graph.edges[edge].dest;
    int goal = graph.edges[edge].orig;
    // Insert cycle edges
    findCycle(start, goal, goal, graph, indexes, cycle);
    return cycle;
}

// Modifies the given tree of the first variation that shows improvement.
inline bool firstImprovement(Graph& graph, int* tree, int* indexes) {
    for (int a = 0; a < (int) graph.edges.size(); a++) {
        if (indexes[a] >= 0) // Ignore if edge is already in the tree
            continue;
        int addedCost = graph.edges[a].weight; // Cost of edge to insert
        set<int> cycle = findCycle(graph, a, indexes); // Edges in the cycle closed by the new edge
        for (int r : cycle) {
            int removedCost = graph.edges[r].weight; // Cost of edge to remove
            if (addedCost < removedCost) {
                // Replace edge
                tree[indexes[r]] = a;
                indexes[a] = indexes[r];
                indexes[r] = -1;
                return true;
            }
        }
    }
    return false;
}

// Modifies the given tree of the variation that shows the best improvement.
inline bool bestImprovement(Graph& graph, int* tree, int* indexes) {
    int bestDiff = 0;
    int removed, added;
    for (int a = 0; a < (int) graph.edges.size(); a++) { // Search for an edge to add
        if (indexes[a] >= 0) // Ignore if edge is already in the tree
            continue;
        int addedCost = graph.edges[a].weight; // Cost of edge to insert
        set<int> cycle = findCycle(graph, a, indexes); // Edges in the cycle closed by the new edge
        for (int r : cycle) { // Search for an edge to remove
            int costDiff = addedCost - graph.edges[r].weight; // Cost of edge to remove
            if (costDiff < bestDiff) {
                // Store best replacement
                bestDiff = costDiff;
                removed = r;
                added = a;
            }
        }
    }
    if (bestDiff < 0) {
		// Replace edge
        tree[indexes[removed]] = added;
        indexes[added] = indexes[removed];
        indexes[removed] = -1;
        return true;
    } else {
        return false;
    }
}

// Union find.
inline int findSet(int* sets, int i) {
	while (sets[i] != i)
		i = sets[i];
	return i;
}

inline void kruskal(Graph& graph, int* tree, int* indexes) {
	graph.sortByWeight();
	int sets[graph.n];
	for (int i = 0; i < graph.n; i++)
		sets[i] = i;
	int t = 0;
	for (int e = 0; e < (int) graph.edges.size(); e++) {
		int orig = graph.edges[e].orig;
		int dest = graph.edges[e].dest;
		int origset = findSet(sets, orig);
		int destset = findSet(sets, dest);
		if (origset != destset) {
			tree[t++] = e;
			sets[origset] = destset;
		}
	}
}
