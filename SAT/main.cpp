#include <cstdlib>
#include <ctime>
#include <climits>

#include "search.cpp"
#define SAMPLES 15

void test(string& formulaName, string& searchName, uint timeLimit) {

	// Get formula
	string formulaFile = formulaName + ".cnf";
	Formula formula(formulaFile);
	
	 bool initSolution[formula.v];
	// Run algorithm
    for(int i = 1; i <= SAMPLES; i++) {

		srand(i * 100);
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

void testTabu(string& formulaName, uint timeLimit, uint dmin, uint dmax, uint dp) {
	// Get formula
	string formulaFile = formulaName + ".cnf";
	Formula formula(formulaFile);
	
	bool initSolution[formula.v];
	// Run algorithm
    for(int i = 1; i <= SAMPLES; i++) {

		srand(i * 100);
	    formula.resetSolution(initSolution);

    	cout << "tabu\t" 
			 << dmin << '\t'
			 << dmax << '\t'
    		 << formulaName << '\t'
    		 << i << '\t';

    	// Get search algorithm
	    TabuGSAT s(formula, formula.v * 3, dmin, dmax);
		s.test(cout, initSolution, timeLimit);
		
    	cout << endl;
	}
}

void testGreedy(string& formulaName, uint k) {
	// Get formula
	string formulaFile = formulaName + ".cnf";
	Formula formula(formulaFile);
	bool initSolution[formula.v];

    high_resolution_clock::time_point t1, t2;
	
	for (int i = 1; i <= 100; i++) {
		cout << (k * 0.2) << '\t'
			 << formulaName << '\t'
			 << i << '\t';

		t1 = high_resolution_clock::now();
		uint result = greedy(formula, initSolution, k * 0.2);
		t2 = high_resolution_clock::now();

		cout << duration_cast<milliseconds>(t2 - t1).count() * 0.001f << '\t' << result << endl;
	}

}

int main(int argc, char* argv[]) {
	// minutes -> seconds -> milliseconds
	uint timeLimit = 20 * 60 * 1000;
	string inst[] = { "par8-5-c", "flat50-1", "flat75-1", "flat100-1" };

	// Comparison tests
	string gsat("gsat");
	string walksat("walksat");
	string tabu("tabu");
	for (int i = 0; i < 4; i++) {
		test(inst[i], gsat, timeLimit);
		test(inst[i], walksat, timeLimit);
		test(inst[i], tabu, timeLimit);
	}

/*
	// Tabu D tests
	uint v[] = { 75, 150, 225, 350 };
	for (int i = 0; i < 4; i++) {
		testTabu(inst[i], timeLimit, 0, v[i], v[i] * 3);
		testTabu(inst[i], timeLimit, 0, v[i] / 3, v[i] * 3);
		testTabu(inst[i], timeLimit, v[i] / 3, v[i] * 2 / 3, v[i] * 3);
		testTabu(inst[i], timeLimit, v[i] * 2 / 3, v[i], v[i] * 3);
	}
*/

/*
	string search(argv[1]);
	string file(argv[2]);
	uint timeLimit = atoi(argv[3]);

	test(file, search, timeLimit);
*/
	
    return 0;
}
