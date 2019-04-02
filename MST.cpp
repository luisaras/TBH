#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
#include <vector>
    using std::vector;
#include <cstdlib>
#include <ctime>
#include <climits>

struct Edge {
    int weight;
    int dest;
    Edge(int w, int d) { weight = w; dest = d; }
};

struct Node {
    vector<Edge> neighbors;
};

void generateGraph(Node* graph, int n, double p) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; i++) {
            if (p > rand() / INT_MAX) {
                int w = rand() % 100 + 1;
                graph[i].neighbors.push_back(Edge(w, j));
                graph[j].neighbors.push_back(Edge(w, i));
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
