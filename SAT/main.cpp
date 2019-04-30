#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
    using std::cerr;
#include <fstream>
    using std::ifstream;
    using std::istream;
#include <ostream>
    using std::ostream;
#include <string>
    using std::string;
    using std::stoi;
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>
#include "formula.cpp"
#include "search.cpp"
using namespace std::chrono;

void test(string& formulaName, string& searchName, uint timeLimit) {
	srand(0);

	// Get formula
	string formulaFile = formulaName + ".cnf";
	Formula formula(formulaFile);
	
	// Run algorithm
    for(int i = 1; i <= 15; i++) {

	    bool initSolution[formula.v];
	    formula.resetSolution(initSolution);

    	cout << searchName << '\t' 
    		 << formulaName << '\t'
    		 << i << '\t';

    	// Get search algorithm
	    if (searchName == 0) {
	    	GSAT s;
	    	s.test(cout, formula, initSolution, formula.v * 3, timeLimit);
	    } else if (search == "walksat") { 
		    WalkSat s;
		    s.test(cout, formula, initSolution, formula.v * 3, timeLimit);
		} else if (search == "tabu") {
			TabuGSAT s;
			s.test(cout, formula, initSolution, formula.v * 3, timeLimit);
		} else {
			TabuGSAT s;
			s.test(cout, formula, initSolution, (uint) -1, timeLimit);
		}
		
    	cout << endl;
	}

}

int main(int argc, char* argv[]) {
	// minutes -> seconds -> milliseconds
	int timeLimit = 20 * 60 * 1000;

	string inst[] = { "flat50-1", "par8-5-c", "flat75-1", "flat100-1" };
		
	string search(argv[1]);
	string file(argv[2]);
	uint timeLimit = atoi(argv[3]);

	test(file, search, timeLimit);

    return 0;
}
