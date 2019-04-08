#include "Graph.cpp"

inline void initializeIndexes(Graph& graph, int* tree, int* indexes) {
    for (int i = 0; i < graph.edges.size(); i++)
        indexes[i] = -1;
    for (int i = 0; i < graph.n - 1; i++)
        indexes[tree[i]] = i;
}

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
    for (int e = 0; e < graph.edges.size(); e++) {
        if (indexes[e] >= 0) // Ignore if edge is already in the tree
            continue;
        int addedCost = graph.edges[e].weight; // Cost of edge to insert
        set<int> cycle = findCycle(graph, e, indexes);
        for (auto it = cycle.begin(); it != cycle.end(); ++it) {
            int removedCost = graph.edges[*it].weight; // cost of edge to remove
            if (addedCost < removedCost) {
                // Replace edge
                tree[indexes[*it]] = e;
                indexes[e] = indexes[*it];
                indexes[*it] = -1;
                return false;
            }
        }
    }
    return true;
}

// Modifies the given tree of the variation that shows the best improvement.
inline bool bestImprovement(Graph& graph, int* tree, int* indexes) {
    int bestDiff = 0;
    int removed, added;
    for (int e = 0; e < graph.edges.size(); e++) {
        if (indexes[e] >= 0) // Ignore if edge is already in the tree
            continue;
        int addedCost = graph.edges[e].weight; // Cost of edge to insert
        set<int> cycle = findCycle(graph, e, indexes);
        for (auto it = cycle.begin(); it != cycle.end(); ++it) {
            int costDiff = graph.edges[*it].weight - addedCost; // cost of edge to remove
            if (costDiff > bestDiff) {
                // Replace edge
                bestDiff = costDiff;
                removed = *it;
                added = e;
            }
        }
    }
    if (bestDiff > 0) {
        tree[indexes[removed]] = added;
        indexes[added] = indexes[removed];
        indexes[removed] = -1;
        return false;
    } else {
        return true;
    }
}

inline prim(Graph& graph, int* tree, int* indexes) {

}
