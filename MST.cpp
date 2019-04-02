#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
#include <vector>
	using std::vector;
#include <stack>
	using std::stack;
#include <cstdlib>
#include <ctime>
#include <climits>

struct Edge {
    int weight, orig, dest;
    Edge(int w, int o, int d) { weight = w; orig = o; dest = d; }
};

struct Node {
    vector<Edge> neighbors;
};

bool isConnected(Node* graph, int n) {
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
		vector<Edge>& neighbors = graph[current].neighbors;
		for (uint i = 0; i < neighbors.size(); i++) {
			open.push(neighbors[i].dest);
		}
	}
	for (int i = 0; i < n; i++)
		if (!visited[i]) 
			return false;
	return true;
}

void generateGraph(Node* graph, int n, double p) {
	while (true) {
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				if (p > rand() / INT_MAX) {
					int w = rand() % 100 + 1;
					graph[i].neighbors.push_back(Edge(w, i, j));
					graph[j].neighbors.push_back(Edge(w, j, i));
				}
			}
		}
		if (isConnected(graph, n))
			return;
		else {
			for (int i = 0; i < n; i++) {
				graph[i].neighbors.clear();
			}
		}
	}
}

int treeCost(Edge** tree, int n) {
	int c = 0;
	for (int i = 0; i < n; i++) {
		c += tree[i]-> weight;
	}
	return c;
}

void initializeTree(Node* graph, Edge** tree, int n) {
	bool visited[n];
	for (int i = 0; i < n; i++)
		visited[i] = false;
	n -= 2;
	stack<int> open;
	open.push(0);
	while(!open.empty()) {
		int current = open.top();
		open.pop();
		vector<Edge>& neighbors = graph[current].neighbors;
		for (uint i = 0; i < neighbors.size(); i++) {
			int dest = neighbors[i].dest;
			if (!visited[dest]) {
				visited[dest] = true;
				open.push(dest);
				tree[n] = &neighbors[i];
				n--;
			}
		}
	}
}

void firstImprovement(Node* graph, Edge** tree, int n) {
	
}

void bestImprovement(Node* graph, Edge** tree, int n) {
	
}

void test(int n, double p) {
    Node graph[n];
    Edge* tree[n - 1];
    generateGraph(graph, n, p);
    initializeTree(graph, tree, n);
    firstImprovement(graph, tree, n);
}

int main() {
    srand (time(NULL));

    for(int i = 0; i < 10; i++) {

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
