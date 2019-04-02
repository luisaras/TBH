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
    int weight, dest;
    Edge(int w, int d) { weight = w; dest = d; }
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
					graph[i].neighbors.push_back(Edge(w, j));
					graph[j].neighbors.push_back(Edge(w, i));
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

void test(int n, double p) {
    Node graph[n];
    generateGraph(graph, n, p);
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
