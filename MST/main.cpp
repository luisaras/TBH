#include "Algorithm.cpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>
using namespace std::chrono;

inline void compare(int n, double p) {
    Graph graph(n, p);
    /*graph.print();
    cout << "Edges: ";
    for (Edge e : graph.edges) {
		cout << e.orig << "-" << e.dest << " ";
	}
	cout << endl;*/
    
	int* tree;
	int indexes[graph.edges.size()];
    high_resolution_clock::time_point t1, t2;

    // First improvement

    tree = graph.initializeTree();
    initializeIndexes(graph, tree, indexes);
    cout << endl << "First Improvement:" << endl;
    t1 = high_resolution_clock::now();
    while(firstImprovement(graph, tree, indexes));
    t2 = high_resolution_clock::now();
    graph.printTree(tree);
    cout << "Time: " << duration_cast<microseconds>( t2 - t1 ).count() << endl;
    delete[] tree;

    // Best improvement

    tree = graph.initializeTree();
    initializeIndexes(graph, tree, indexes);
    cout << endl << "Best Improvement:" << endl;
    t1 = high_resolution_clock::now();
    while(bestImprovement(graph, tree, indexes));
    t2 = high_resolution_clock::now();
    graph.printTree(tree);
    cout << "Time: " << duration_cast<microseconds>( t2 - t1 ).count() << endl;
    delete[] tree;

    // Kruskal

    tree = new int[graph.n - 1];
    cout << endl << "Kruskal:" << endl;
    t1 = high_resolution_clock::now();
    kruskal(graph, tree, indexes);
    t2 = high_resolution_clock::now();
    graph.printTree(tree);
    cout << "Time: " << duration_cast<microseconds>( t2 - t1 ).count() << endl;
    delete[] tree;

	cout << endl;

}

void test(int n, double p) {
    Graph graph(n, p);
    graph.sortByWeight();
    graph.print();

    int* tree = graph.initializeTree();
    cout << endl << "Initial tree: " << endl;
    graph.printTree(tree);

    int indexes[graph.edges.size()];
    initializeIndexes(graph, tree, indexes);

    while(bestImprovement(graph, tree, indexes));

    cout << endl << "Optimal tree: " << endl;
    graph.printTree(tree);

    delete[] tree;
}

int main() {
    //srand (time(NULL));
    srand(0);
    compare(50, 0.8);

    for(int i = 11; i < 10; i++) {

        compare(100, 0.1);
        compare(100, 0.2);
        compare(100, 0.3);

        compare(250, 0.05);
        compare(250, 0.1);
        compare(250, 0.2);

        compare(500, 0.05);
        compare(500, 0.1);
        compare(500, 0.2);

    }

    return 0;
}
