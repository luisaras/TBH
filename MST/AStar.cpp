#include <queue>
    using std::priority_queue;

struct TreePath {
    int* tree;
    int g = 0;
    int h = 0;
    TreePath(int* tree, int g, int h) {
        this->tree = tree; this->g = g; this->h = h;
    }
};

struct Smallest {
    inline bool operator()(const TreePath& left, const TreePath& right) {
        int fl = left.g + left.h;
        int fr = right.g + right.h;
        if (fl == fr)
            return left.h > right.h;
        return fl > fr;
    }
};

struct Greatest {
    inline bool operator()(const TreePath& left, const TreePath& right) {
        int fl = left.g + left.h;
        int fr = right.g + right.h;
        if (fl == fr)
            return left.g < right.g;
        return fl < fr;
    }
};

template <class comp>
inline int astar(Graph& graph, int* init, int optimal, int h) {
    priority_queue<TreePath, vector<TreePath>, comp> open;
    int E = graph.edges.size();
    int indexes[E];
    open.push(TreePath(new int[E], 0, graph.treeCost(init) - optimal));
    for (int i = 0; i < E; i++) {
        open.top().tree[i] = init[i];
    }
    while (!open.empty()) {
        TreePath current = open.top();
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
                    int* child = new int[E];
                    for (int e = 0; e < E; e++)
                        child[e] = current.tree[e];
                    child[indexes[r]] = a;
                    int ccost = cost + addedCost - removedCost;
                    open.push(TreePath(child, current.g + h, ccost - optimal));
                }
            }
        }
        delete [] current.tree;
    }
    return -1;
}

int longestPath(Graph& graph, int* init, int optimal) {
    return astar<Greatest>(graph, init, optimal, graph.lightestEdge());
}

int shortestPath(Graph& graph, int* init, int optimal) {
    return astar<Smallest>(graph, init, optimal, graph.heaviestEdge());
}