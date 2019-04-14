#include "Algorithm.cpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>
#define SAMPLES 10
using namespace std::chrono;

struct Result {
	double first_its = 0;
	double best_its = 0;
	double first_time = 0;
	double best_time = 0;
};

void runSearch(Graph& graph, Result& result) {
	high_resolution_clock::time_point t1, t2;
	int indexes[graph.edges.size()];
	int* tree;

	// First improvement

	t1 = high_resolution_clock::now();
	while(firstImprovement(graph, tree, indexes)) result.first_its++;
	t2 = high_resolution_clock::now();
	result.first_time += duration_cast<milliseconds>( t2 - t1 ).count();
	delete[] tree;

	// Best improvement

	tree = graph.initializeTree();
	graph.initializeIndexes(tree, indexes);
	t1 = high_resolution_clock::now();
	while(bestImprovement(graph, tree, indexes)) result.best_its++;
	t2 = high_resolution_clock::now();
	result.best_time += duration_cast<milliseconds>( t2 - t1 ).count();
	delete[] tree;
}

void findPaths(Graph& graph, int i) {
	graph.sortByWeight();
	int* tree = graph.initializeTree();
	cout << "Initial cost: " << graph.treeCost(tree) << endl;
	cout << "Minimal cost: " << kruskal(graph) << endl;
	cout << (i + 1) << ": " << shortestPath(graph, tree) << endl;
}

void test(int n, double p) {
	Result result;

	cout << "n = " << n << ", p = " << p << endl;

	for (int i = 0; i < SAMPLES; i++) {

		Graph graph(n, p);

		//runSearch(graph, result);
		findPaths(graph, i);

    
	}
	/*
	result.first_its /= SAMPLES;
	result.best_its /= SAMPLES;
	result.first_time /= SAMPLES;
	result.best_time /= SAMPLES;
    
    cout << "First Improvement: " << result.first_time << " ms, " << result.first_its << " iterations." << endl;
    cout << "Best Improvement: " << result.best_time << " ms, " << result.best_its << " iterations." << endl;*/
    cout << endl;

}

int main() {
    srand(0);
    test(8, 0.1);
/*
	test(100, 0.1);
	test(100, 0.2);
	test(100, 0.3);

	test(250, 0.05);
	test(250, 0.1);
	test(250, 0.2);

	test(500, 0.05);
	test(500, 0.1);
	test(500, 0.2);*/

    return 0;
}
