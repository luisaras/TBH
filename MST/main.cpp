#include "Algorithm.cpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <string>
using namespace std::chrono;

//-----------------------------------------------------------------------------
// Local Search
//-----------------------------------------------------------------------------

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
	tree = graph.initializeTree();
	graph.initializeIndexes(tree, indexes);
	t1 = high_resolution_clock::now();
	for(int i = 0; firstImprovement(graph, tree, indexes); i++) result.first_its++;
	t2 = high_resolution_clock::now();
	result.first_time += duration_cast<milliseconds>( t2 - t1 ).count();
	cout << "First Improvement:" << result.first_time << "ms, " << result.first_its << endl;
	delete[] tree;

	// Best improvement
	tree = graph.initializeTree();
	graph.initializeIndexes(tree, indexes);
	t1 = high_resolution_clock::now();
	for(int i = 0; bestImprovement(graph, tree, indexes); i++) result.best_its++;
	t2 = high_resolution_clock::now();
	result.best_time += duration_cast<milliseconds>( t2 - t1 ).count();
	cout << "Best Improvement:" << result.best_time << "ms, " << result.best_its << endl;
	delete[] tree;
}

void test(int n, double p, int samples = 10) {
	Result result;
	cout << "n = " << n << ", p = " << p << endl;
	for (int i = 0; i < samples; i++) {
		Graph graph(n, p);
		runSearch(graph, result);
	}
	if (samples > 1) {
		result.first_its /= samples;
		result.best_its /= samples;
		result.first_time /= samples;
		result.best_time /= samples;
	    cout << "First Improvement Avg: " << result.first_time << " ms, " << result.first_its << " iterations." << endl;
	    cout << "Best Improvement Avg: " << result.best_time << " ms, " << result.best_its << " iterations." << endl;
	}
    cout << endl;
}

//-----------------------------------------------------------------------------
// Path Finder
//-----------------------------------------------------------------------------

void findPaths(char test, Graph& graph, int optimal) {
	high_resolution_clock::time_point t1, t2;
	int result;
	t1 = high_resolution_clock::now();
	if (test == '1') {
		result = astar_shortestPath(graph, graph.initializeTree(), optimal);
		cout << "Shortest Path (A*): ";
	} else if (test == '2') {
		result = dfs_shortestPath(graph, graph.initializeTree(), optimal);
		cout << "Shortest Path (DFS): ";
	} else if (test == '3') {
		result = astar_longestPath(graph, graph.initializeTree(), optimal);
		cout << "Longest Path (A*): ";
	} else {
	t2 = high_resolution_clock::now();
		result = dfs_longestPath(graph, graph.initializeTree(), optimal);
		cout << "Longest Path (DFS): ";
	}
	t2 = high_resolution_clock::now();
	cout << result << " in " << duration_cast<milliseconds>(t2-t1).count() << "ms" << endl;
}

int getOptimal(Graph& graph) {
	Graph copy(graph);
	copy.sortByWeight();
	return kruskal(copy);
}

void testPaths(char test, int n, double p) {
	Result result;
	cout << "n = " << n << ", p = " << p << endl;
	Graph graph(n, p);
	int optimal = getOptimal(graph);
	int* tree = graph.initializeTree();
	cout << "Initial cost: " << graph.treeCost(tree) << endl;
	cout << "Minimal cost: " << optimal << endl;
	delete [] tree;
	findPaths(test, graph, optimal);
}

//-----------------------------------------------------------------------------

void createDummy(int n, double p) {
	Graph graph(n, p);
}

int main(int argc, char** argv) {
    std::string type(argv[1]);
    int n = std::stoi(argv[2]);
    double p = std::stod(argv[3]);
    int samples = argc > 4 ? std::stoi(argv[4]) : 0;

    if (type.find("path") != std::string::npos) {
    	srand(samples);
    	testPaths(type.back(), n, p);
    } else {
    	srand(0);
    	test(n, p, samples == 0 ? 1 : samples);
    }

	// test(100, 0.1);
	// test(100, 0.2);
	// test(100, 0.3);

	// test(250, 0.05);
	// test(250, 0.1);
	// test(250, 0.2);

	// test(500, 0.05);
	// test(500, 0.1);
	// test(500, 0.2);
	// test(500, 0.2);

    return 0;
}
