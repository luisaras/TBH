#include <stack>
    using std::stack;

double bestExchange;
double startBest;
bool (*isBetter)(double, double);
int optimal;

bool isGreater(double left, double right) {
    return left > right;
}

bool isLess(double left, double right) {
    return left < right;
}

int dfs(Graph& graph, int* tree) {
    int cost = graph.treeCost(tree);
    if (cost == optimal) {
        delete [] tree;
        return 0;
    }
    int indexes[graph.edges.size()];
    graph.initializeIndexes(tree, indexes);
    double bestLength = startBest;
    for (int a = 0; a < graph.edges.size(); a++) { // Search for an edge to add
        if (indexes[a] >= 0) // Ignore if edge is already in the tree
            continue;
        int addedCost = graph.edges[a].weight; // Cost of edge to insert
        set<int> cycle = graph.findCycle(a, indexes); // Edges in the cycle closed by the new edge
        for (int r : cycle) { 
            int removedCost = graph.edges[r].weight; // Cost of edge to remove
            if (removedCost > addedCost) { 
                // If there is improvement, swap
                int* child = graph.copyTree(tree);
                child[indexes[r]] = a;
                // The best path length possible to the optimal
                int ccost = cost + addedCost - removedCost;
                int h = (ccost - optimal) / bestExchange; 
                // Only include it if best path length possible from this node is better than current best
                if (!isBetter(bestLength, h)) {
                    int g = dfs(graph, child) + 1;
                    if (isBetter(g, bestLength))
                        bestLength = g;
                }
            }
        }
    }
    delete [] tree;
    return bestLength;
}

int dfs_longestPath(Graph& graph, int* init, int _optimal) {
    optimal = _optimal;
    isBetter = isGreater;
    bestExchange = graph.leastExchange();
    startBest = 0;
    return dfs(graph, init);
}

int dfs_shortestPath(Graph& graph, int* init, int _optimal) {
    optimal = _optimal;
    isBetter = isLess;
    bestExchange = graph.greatestExchange();
    startBest = INT_MAX;
    return dfs(graph, init);
}