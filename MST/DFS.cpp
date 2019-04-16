#include <stack>
    using std::stack;

struct TreePath {
    int* tree;
    float g = 0;
    TreePath(int* tree, float g) {
        this->tree = tree; this->g = g;
    }
};

bool isGreater(float left, float right) {
    return left < right;
}

bool isLess(float left, float right) {
    return left > right;
}

int dfs(Graph& graph, int* init, int optimal, 
        bool (*isBetter)(float, float), float best, float limit) {
    int E = graph.edges.size();
    int indexes[E];
    stack<TreePath> open;
    open.push(TreePath(init, 0));
    while (!open.empty()) {
        TreePath current = open.top();
        int cost = graph.treeCost(current.tree);
        if (cost == optimal) {
            if (isBetter(current.g, best)) {
                best = current.g;
            }
        }
        graph.initializeIndexes(current.tree, indexes);
        open.pop();
        for (int a = 0; a < E; a++) { // Search for an edge to add
            if (indexes[a] >= 0) // Ignore if edge is already in the tree
                continue;
            int addedCost = graph.edges[a].weight; // Cost of edge to insert
            set<int> cycle = graph.findCycle(a, indexes); // Edges in the cycle closed by the new edge
            for (int r : cycle) { // Search for an edge to remove
                int removedCost = graph.edges[r].weight; // Cost of edge to remove
                if (removedCost > addedCost) { 
                    // If there is improvement
                    int* child = new int[E];
                    for (int e = 0; e < E; e++)
                        child[e] = current.tree[e];
                    child[indexes[r]] = a;
                    int ccost = cost + addedCost - removedCost;
                    float h = (ccost - optimal) / limit; // The best path length possible to the optimal
                    if (isBetter(h, best)) // Only include it if h is better than current best path found
                        open.push(TreePath(child, current.g + 1));
                }
            }
        }
        delete [] current.tree;
    }
    return best;
}

int longestPath(Graph& graph, int* init, int optimal) {
    return dfs(graph, init, optimal, isGreater, 0, graph.lightestEdge());
}

int shortestPath(Graph& graph, int* init, int optimal) {
    return dfs(graph, init, optimal, isLess, INT_MAX, graph.heaviestEdge());
}