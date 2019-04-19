#include <queue>
    using std::priority_queue;

struct AStarPath {
    int* tree;
    int g = 0;
    int h = 0;
    AStarPath(int* tree, int g, int h) {
        this->tree = tree; this->g = g; this->h = h;
    }
};

struct Smallest {
    inline bool operator()(const AStarPath& left, const AStarPath& right) {
        int fl = left.g + left.h;
        int fr = right.g + right.h;
        if (fl == fr)
            return left.h > right.h;
        return fl > fr;
    }
};

struct Greatest {
    inline bool operator()(const AStarPath& left, const AStarPath& right) {
        int fl = left.g + left.h;
        int fr = right.g + right.h;
        if (fl == fr)
            return left.g < right.g;
        return fl < fr;
    }
};

template <class comp>
inline int astar(Graph& graph, int* init, int optimal, int h) {
    priority_queue<AStarPath, vector<AStarPath>, comp> open;
    int E = graph.edges.size();
    int indexes[E];
    open.push(AStarPath(init, 0, graph.treeCost(init) - optimal));
    while (!open.empty()) {
        AStarPath current = open.top();
        int cost = graph.treeCost(current.tree);
        if (cost == optimal) {
                while (!open.empty()) {
                    delete open.top().tree;
                open.pop();
            }
            return current.g / h;
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
                    int* child = graph.copyTree(current.tree);
                    child[indexes[r]] = a;
                    int ccost = cost + addedCost - removedCost;
                    open.push(AStarPath(child, current.g + h, ccost - optimal));
                }
            }
        }
        delete [] current.tree;
    }
    return -1;
}

int astar_longestPath(Graph& graph, int* init, int optimal) {
    return astar<Greatest>(graph, init, optimal, graph.leastExchange());
}

int astar_shortestPath(Graph& graph, int* init, int optimal) {
    return astar<Smallest>(graph, init, optimal, graph.greatestExchange());
}