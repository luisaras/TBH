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
		// Initialize "visited" flags
        bool visited[n];
        for (int i = 0; i < n; i++)
            visited[i] = false;
        // Initialize tree
        int* tree = new int[n - 1];
        int t = 0;
        stack<int> open;
        open.push(0);
        while(!open.empty()) {
            int current = open.top();
            open.pop();
            for (auto it = nodes[current].begin(); it != nodes[current].end(); ++it) {
				int e = *it; // Neighbor edge of current node
				int v = current == edges[e].dest ? edges[e].orig : edges[e].dest; // Neighbor node
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
        for (int i = 0; i < n; i++) {
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
				int v = edges[e].dest == i ? edges[e].orig : edges[e].dest; // Neighbor of node i
                cout << v << " ";
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
        bool visited[n];
        for (int i = 0; i < n; i++)
            visited[i] = false;
        stack<int> open;
        open.push(0);
        while(!open.empty()) {
            int current = open.top();
            open.pop();
            if (visited[current])
                continue;
            visited[current] = true;
            for (auto it = nodes[current].begin(); it != nodes[current].end(); ++it) {
				int e = *it; // Neighbor edge of node i
				int v = edges[e].dest == current ? edges[e].orig : edges[e].dest; // Neighbor of node i
                open.push(v);
            }
        }
        for (int i = 0; i < n; i++)
            if (!visited[i])
                return false;
        return true;
    }

};

// Sets "available" flags for each edge in the graph.
inline void availableEdges(bool* edges, int e, int* tree, int n) {
    for (int i = 0; i < e; i++)
        edges[i] = true;
    for (int i = 0; i < n; i++)
        edges[tree[i]] = false;
}

// Gets a set of edges in 
inline set<int> findCycle(Graph graph, int* tree, int e, bool* available) {
    set<int> cycle;
    
    //int root = graph.edges[e].dest;
    // TODO
    return cycle;
}

// Modifies the given tree of the first variation that shows improvement.
inline bool firstImprovement(Graph& graph, int* tree, bool* available) {
    for (int e = 0; e < (int) graph.edges.size(); e++) {
        if (available[e]) {
			int cost = graph.edges[e].weight;
            set<int> cycle = findCycle(graph, tree, e, available);
			for (auto it = cycle.begin(); it != cycle.end(); ++it) {
				if (graph.edges[*it].weight < cost) {
					available[tree[e]] = true;
					available[*it] = false;
					tree[e] = *it;
					return false;
				}
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
    bool available[graph.edges.size()];
    availableEdges(available, graph.edges.size(), tree, graph.n - 1);
    while(!firstImprovement(graph, tree, available));
}

inline void test(int n, double p) {
    Graph graph(n, p);
    graph.sortByWeight();
    graph.print();
    int* tree = graph.initializeTree();
    cout << endl << "Initial tree: " << endl;
    graph.printTree(tree);
    //localSearch(graph, tree);
    //cout << endl << "Optimal tree: " << endl;
    //graph.printTree(tree);
}

int main() {
    //srand (time(NULL));
    srand(100);

    test(5, 0.5);

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
