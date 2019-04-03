#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
#include <vector>
	using std::vector;
#include <stack>
	using std::stack;
#include <algorithm>
    using std::sort;
#include <cstdlib>
#include <ctime>
#include <climits>
#define Node vector<int>

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

    Graph(int n, double p) {
        nodes = new Node[n];
        this->n = n;
        while (true) {
            // Sets edges randomly
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (p > rand() * 1.0 / RAND_MAX) {
                        int w = rand() % 100 + 1;
                        nodes[i].push_back(edges.size());
                        nodes[j].push_back(edges.size());
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

    void sortByWeight() {
        sort(edges.begin(), edges.end(), compareEdges);
        for (int i = 0; i < n; i++)
            nodes[i].clear();
        for (int e = 0; e < edges.size(); e++) {
            nodes[edges[e].orig].push_back(e);
            nodes[edges[e].dest].push_back(e);
        }
    }

    int* initializeTree() {
        int* tree = new int[n - 1];
        bool visited[n];
        for (int i = 0; i < n; i++)
            visited[i] = false;
        int e = 0;
        stack<int> open;
        open.push(0);
        while(!open.empty()) {
            int current = open.top();
            open.pop();
            for (int i = 0; i < nodes[current].size(); i++) {
                int dest = edges[nodes[current][i]].dest;
                if (!visited[dest]) {
                    visited[dest] = true;
                    open.push(dest);
                    tree[e++] = nodes[current][i];
                }
                int orig = edges[nodes[current][i]].orig;
                if (!visited[orig]) {
                    visited[orig] = true;
                    open.push(orig);
                    tree[e++] = nodes[current][i];
                }
            }
        }
        return tree;
    }

    inline int treeCost(int* tree) {
        int c = 0;
        for (int i = 0; i < n; i++) {
            c += edges[tree[i]].weight;
        }
        return c;
    }

    void print() {
        for (int i = 0; i < n; i++) {
            cout << i << " -> ";
            for (int j = 0; j < nodes[i].size(); j++)
                cout << edges[nodes[i][j]].dest << " ";
            cout << endl;
        }
    }

    void printTree(int* tree) {
        cout << edges[tree[0]].orig << "-" << edges[tree[0]].dest;
        for (int i = 1; i < n - 1; i++)
            cout << ", " << edges[tree[i]].orig << "-" << edges[tree[i]].dest;
        cout << endl << "cost: " << treeCost(tree);
    }

private:
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
            for (int i = 0; i < nodes[current].size(); i++) {
                int dest = edges[nodes[current][i]].dest;
                if (dest == current)
                    dest = edges[nodes[current][i]].orig;
                open.push(dest);
            }
        }
        for (int i = 0; i < n; i++)
            if (!visited[i])
                return false;
        return true;
    }

};

inline void availableEdges(bool* edges, int e, int* tree, int n) {
    for (int i = 0; i < e; i++)
        edges[i] = true;
    for (int i = 0; i < n; i++)
        edges[tree[i]] = false;
}

inline vector<int> findCycle(Graph graph, int* tree, int e, bool* available) {
    vector<int> cycle;
    int root = graph.edges[e].dest;
    // TODO
    return cycle;
}

void firstImprovement(Graph graph, int* tree) {
    graph.sortByWeight();
    bool edges[graph.edges.size()];
    availableEdges(edges, graph.edges.size(), tree, graph.n - 1);
    int edgei;
    Edge* bestEdge;
    for (int e = 0; e < graph.edges.size(); e++) {
        if (edges[e]) {
            vector<int> cycle = findCycle(graph, tree, e, edges);
            sort(cycle.begin(), cycle.end());

        }
    }
}

void bestImprovement(Graph graph, int* tree) { }

void test(int n, double p) {
    Graph graph(n, p);
    int* tree = graph.initializeTree();
    firstImprovement(graph, tree);
    //bestImprovement(graph, tree);
    graph.printTree(tree);
}

int main() {
    srand (time(NULL));

    test(5, 0.2);

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
