#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
#include <vector>
	using std::vector;
#include <stack>
	using std::stack;
#include <set>
	using std::set;
#include <algorithm>
    using std::sort;
#include <cstdlib>
#include <ctime>
#include <climits>
#define Node set<int>

// Edge

struct Edge {
    int weight, orig, dest;
    Edge(int w, int o, int d) { weight = w; orig = o; dest = d; }
    inline int neighbor(int v) {
        return orig == v ? dest : orig;
    }
};
bool compareEdges(const Edge& e1, const Edge& e2) {
    return (e1.weight < e2.weight);
}

class Graph {
public:
    int n; // Number of nodes
    Node* nodes; // Each node is a list of edge indexes (edge's position in the graph's edge list)
    vector<Edge> edges; // List of edges

	// Creates a random graph with n nodes and edges with probability p.
    Graph(int n, double p) {
        nodes = new Node[n];
        this->n = n;
        while (true) {
            // Sets edges randomly
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (p > rand() * 1.0 / RAND_MAX) {
                        int w = rand() % 100 + 1;
                        nodes[i].insert(edges.size());
                        nodes[j].insert(edges.size());
                        edges.push_back(Edge(w, i, j));
                    }
                }
            }
            if (isConnected()) {
                return;
            } else {
                for (int i = 0; i < n; i++)
                    nodes[i].clear();
                edges.clear();
            }
        }
    }

	// Sorts edges by weight.
    void sortByWeight() {
        sort(edges.begin(), edges.end(), compareEdges);
        for (int i = 0; i < n; i++)
            nodes[i].clear();
        for (int e = 0; e < (int) edges.size(); e++) {
            nodes[edges[e].orig].insert(e);
            nodes[edges[e].dest].insert(e);
        }
    }

	// Creates an initial spanning tree.
    int* initializeTree() {
        // Initialize tree
        int* tree = new int[n - 1];
        int t = 0;
		// Initialize "visited" flags
        bool visited[n];
        for (int i = 1; i < n; i++)
            visited[i] = false;
        visited[0] = true;
        // Initialize stack
        stack<int> open;
        open.push(0);
        while(!open.empty()) {
            int current = open.top();
            open.pop();
            for (auto it = nodes[current].begin(); it != nodes[current].end(); ++it) {
				int e = *it; // Neighbor edge of current node
				int v = edges[e].neighbor(current); // Neighbor node
                if (!visited[v]) { // Neighbor was not visited before
                    visited[v] = true;
                    open.push(v);
                    tree[t++] = e; // Insert edge in the spanning tree
                }
            }
        }
        return tree;
    }

	// Calculates total cost of tree.
    inline int treeCost(int* tree) {
        int c = 0;
        for (int i = 0; i < n - 1; i++) {
            c += edges[tree[i]].weight;
        }
        return c;
    }

	// Prints each node's neighbors;
    void print() {
        for (int i = 0; i < n; i++) {
            cout << i << " -> ";
            for (auto it = nodes[i].begin(); it != nodes[i].end(); ++it) {
				int e = *it; // Neighbor edge of node i
				int v = edges[e].neighbor(i); // Neighbor of node i
                cout << v << "(" << edges[e].weight << ") ";
			}
            cout << endl;
        }
    }

	// Prints tree's edges.
    void printTree(int* tree) {
        cout << edges[tree[0]].orig << "-" << edges[tree[0]].dest;
        for (int i = 1; i < n - 1; i++)
            cout << ", " << edges[tree[i]].orig << "-" << edges[tree[i]].dest;
        cout << endl << "cost: " << treeCost(tree) << endl;
    }

private:

	// Checks if graph is connected using DFS.
    inline bool isConnected() {
		// Initialize "visited" flags
        bool visited[n];
        for (int i = 1; i < n; i++)
            visited[i] = false;
        visited[0] = true;
        // Initialize stack
        stack<int> open;
        open.push(0);
        while(!open.empty()) {
            int current = open.top();
            open.pop();
            for (auto it = nodes[current].begin(); it != nodes[current].end(); ++it) {
				int e = *it; // Neighbor edge of node i
				int v = edges[e].neighbor(current); // Neighbor of node i
				if (!visited[v]) {
					visited[v] = true;
					open.push(v);
				}
            }
        }
        // If any of the nodes was not visited
        for (int i = 0; i < n; i++)
            if (!visited[i])
                return false;
        return true;
    }

};

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
inline void bestImprovement(Graph& graph, int* tree) {
	// TODO
}

// Modifies tree until a local optima is found.
inline void localSearch(Graph& graph, int* tree) {
    int indexes[graph.edges.size()];
    for (int i = 0; i < graph.edges.size(); i++)
        indexes[i] = -1;
    for (int i = 0; i < graph.n - 1; i++)
        indexes[tree[i]] = i;
    while(!firstImprovement(graph, tree, indexes));
}

inline void test(int n, double p) {
    Graph graph(n, p);
    graph.sortByWeight();
    graph.print();
    int* tree = graph.initializeTree();
    cout << endl << "Initial tree: " << endl;
    graph.printTree(tree);
    localSearch(graph, tree);
    cout << endl << "Optimal tree: " << endl;
    graph.printTree(tree);
}

int main() {
    //srand (time(NULL));
    srand(100);

    test(100, 0.1);

    for(int i = 11; i < 10; i++) {

        test(100, 0.1);
        test(100, 0.2);
        test(100, 0.3);

        test(250, 0.05);
        test(250, 0.1);
        test(250, 0.2);

        test(500, 0.05);
        test(500, 0.1);
        test(500, 0.2);

    }

    return 0;
}
