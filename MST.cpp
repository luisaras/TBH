#include "Algorithm.cpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>
using namespace std::chrono;

inline void compare(int n, double p) {
    Graph graph(n, p);
    graph.sortByWeight();
    //graph.print();
    int indexes[graph.edges.size()];

    int* tree;
    high_resolution_clock::time_point t1, t2;

    // First improvement

    tree = graph.initializeTree();
    initializeIndexes(graph, tree, indexes);
    t1 = high_resolution_clock::now();
    while(!firstImprovement(graph, tree, indexes));
    t2 = high_resolution_clock::now();
    cout << "First improvement time: \t" << duration_cast<microseconds>( t2 - t1 ).count() << endl;
    delete[] tree;

    // Best improvement

    tree = graph.initializeTree();
    initializeIndexes(graph, tree, indexes);
    t1 = high_resolution_clock::now();
    while(!bestImprovement(graph, tree, indexes));
    t2 = high_resolution_clock::now();
    cout << "Best improvement time: \t\t" << duration_cast<microseconds>( t2 - t1 ).count() << endl;
    delete[] tree;

}

void test(int n, double p) {
    Graph graph(n, p);
    //graph.sortByWeight();
    graph.print();

    int* tree = graph.initializeTree();
    cout << endl << "Initial tree: " << endl;
    graph.printTree(tree);

    int indexes[graph.edges.size()];
    initializeIndexes(graph, tree, indexes);

    while(!bestImprovement(graph, tree, indexes));

    cout << endl << "Optimal tree: " << endl;
    graph.printTree(tree);

    delete[] tree;
}

int main() {
    //srand (time(NULL));
    srand(100);
    test(50, 0.5);

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
