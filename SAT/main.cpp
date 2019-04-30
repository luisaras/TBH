#include <cstdlib>
#include <ctime>
#include <climits>

#include "search.cpp"
#define SAMPLES 15

void test(string& formulaName, string& searchName, uint timeLimit) {

	// Get formula
	string formulaFile = formulaName + ".cnf";
	Formula formula(formulaFile);
	
	// Run algorithm
    for(int i = 1; i <= SAMPLES; i++) {

		srand(i * 100);
	    bool initSolution[formula.v];
	    formula.resetSolution(initSolution);

    	cout << searchName << '\t' 
    		 << formulaName << '\t'
    		 << i << '\t';

    	// Get search algorithm
	    if (searchName == "gsat") {
	    	GSAT s(formula, formula.v * 3);
	    	s.test(cout, initSolution, timeLimit);
	    } else if (searchName == "walksat") { 
		    WalkSAT s(formula, formula.v * 3);
		    s.test(cout, initSolution, timeLimit);
		} else if (searchName == "tabu") {
			TabuGSAT s(formula, formula.v * 3);
			s.test(cout, initSolution, timeLimit);
		} else {
			cout << "Search not recognized: " << searchName << endl;
		}
		
    	cout << endl;
	}

}

int main(int argc, char* argv[]) {
	// minutes -> seconds -> milliseconds
	//uint timeLimit = 20 * 60 * 1000;

	string inst[] = { "flat50-1", "par8-5-c", "flat75-1", "flat100-1" };
		
	string search(argv[1]);
	string file(argv[2]);
	uint timeLimit = atoi(argv[3]);

	test(file, search, timeLimit);

    return 0;
}
