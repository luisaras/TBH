#include "Graph.cpp"

// Modifies the given tree of the first variation that shows improvement.
inline bool firstImprovement(Graph& graph, int* tree, int* indexes) {
    for (int a = 0; a < (int) graph.edges.size(); a++) {
        if (indexes[a] >= 0) // Ignore if edge is already in the tree
            continue;
        int addedCost = graph.edges[a].weight; // Cost of edge to insert
        set<int> cycle = graph.findCycle(a, indexes); // Edges in the cycle closed by the new edge
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
        set<int> cycle = graph.findCycle(a, indexes); // Edges in the cycle closed by the new edge
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

void kruskal(Graph& graph, int* tree) {
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

int kruskal(Graph& graph) {
    int tree[graph.edges.size()];
    kruskal(graph, tree);
    return graph.treeCost(tree);
}

int shortestPath(Graph& graph, int* init) {
    int E = graph.edges.size();
    int indexes[E];
    int optimal = kruskal(graph);
    int heaviest = graph.heaviestEdge();
    queue open;
    open.push(TreePath(init, 0, (graph.treeCost(init) - optimal) * 1.0f / heaviest));
    while (!open.empty()) {
        TreePath current = open.top();
        if (current.h == 0)
            return current.g;
        open.pop();
        graph.initializeIndexes(current.tree, indexes);
        int cost = graph.treeCost(current.tree);
        for (int a = 0; a < E; a++) { // Search for an edge to add
            if (indexes[a] >= 0) // Ignore if edge is already in the tree
                continue;
            int addedCost = graph.edges[a].weight; // Cost of edge to insert
            set<int> cycle = graph.findCycle(a, indexes); // Edges in the cycle closed by the new edge
            for (int r : cycle) { // Search for an edge to remove
                int removedCost = graph.edges[r].weight; // Cost of edge to remove
                if (removedCost > addedCost) { // If there is improvement
                    // Store best replacement
                    int* child = new int[E];
                    for (int e = 0; e < E; e++)
                        child[e] = current.tree[e];
                    child[indexes[r]] = a;
                    int ccost = cost + addedCost - removedCost;
                    open.push(TreePath(child, current.g + 1, (ccost - optimal) * 1.0f / heaviest));
                }
            }
        }
        delete [] current.tree;
    }
    return -1;
}