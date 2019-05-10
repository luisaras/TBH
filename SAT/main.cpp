#include <cstdlib>
#include <ctime>
#include <climits>

#include "search.cpp"
#define SAMPLES 10

// Test an exact local search method.
// searchName: gsat, walksat or tabu
void testLocal(Formula& formula, string& searchName, uint timeLimit) {
	// Get search method
	Search* s;
    if (searchName == "gsat") {
    	s = new GSAT(formula, formula.v * 3);
    } else if (searchName == "walksat") { 
	    s = new WalkSAT(formula, formula.v * 3);
	} else if (searchName == "tabu") {
		s = new TabuGSAT(formula, formula.v * 3);
	} else if (searchName == "grasp") {
		s = new GRASP(formula, RESET);
	} else {
		cout << "Search not recognized: " << searchName << endl;
		return;
	}
	// alg  instance  rep  time  steps
    for(int i = 1; i <= SAMPLES; i++) {
		srand(i * 100);
	    // Print name
    	cout << searchName << '\t' 
    		 << formula.name << '\t'
    		 << i << '\t';
    	// Run
		bool success = s->test(timeLimit);
		// Print results
		if (success) {
			cout << s->executionTime * 0.001f << '\t' << s->steps << endl;
		} else {
			cout << "NA\tNA" << endl;
		}
	}
	delete s;
}

// Test a D interval for tabu search.
void testD(Formula& formula, uint timeLimit, uint dmin, uint dmax, uint dp) {
	// dmin  dmax  instance  rep  time  steps
    for(int i = 1; i <= SAMPLES; i++) {
		srand(i * 100);
	    // Print name
    	cout << dmin << '\t'
			 << dmax << '\t'
    		 << formula.name << '\t'
    		 << i << '\t';
    	// Run
	    TabuGSAT s(formula, dp, dmin, dmax);
		bool success = s.test(timeLimit);
		// Print results
		if (success) {
    		cout << s.executionTime * 0.001f << '\t' << s.steps << endl;
		} else {
			cout << "NA\tNA" << endl;
		}
	}
}

// Test a k value for a given greedy method.
// grasp: true will use GRASP method, false will use just a greedy constructive method.
void testAlpha(Formula& formula, uint k, bool grasp) {
	// alpha  instance  rep  time  v steps
	for (int i = 1; i <= 1000; i++) {
		// Print name
		cout << (k * 0.2) << '\t'
			 << formula.name << '\t'
			 << i << '\t';
		if (grasp) {
			// Run Greedy + GSAT
			GRASP s(formula, k * 0.2, STOP);
			s.test((uint) -1);
			cout << s.executionTime * 0.001f << '\t' << s.best << '\t' << s.steps << endl;
		} else {
			// Run Greedy
			high_resolution_clock::time_point t1, t2;
			bool solution[formula.v];
			t1 = high_resolution_clock::now();
			uint result = greedy(formula, solution, k * 0.2);
			t2 = high_resolution_clock::now();
			cout << duration_cast<milliseconds>(t2 - t1).count() * 0.001f << '\t' << result << endl;
		}
	}
}

int main(int argc, char* argv[]) {
	srand(time(NULL));

	string testType(argv[1]);
	string formulaName(argv[2]);
	Formula f(formulaName);

	if (testType == "local") {
		// instance alg minutes
		string searchName(argv[3]);
		uint timeLimit = atoi(argv[4]); // milliseconds
		testLocal(f, searchName, timeLimit);
	} else if (testType == "d") {
		// instance interval minutes
		uint interval = atoi(argv[3]);
		uint timeLimit = atoi(argv[4]); // milliseconds
		if (interval == 0)
			testD(f, timeLimit, 0, f.v, f.v * 3);
		else if (interval == 1) 
			testD(f, timeLimit, 0, f.v / 3, f.v * 3);
		else if (interval == 2) 
			testD(f, timeLimit, f.v / 3, f.v * 2 / 3, f.v * 3);
		else if (interval == 3) 
			testD(f, timeLimit, f.v * 2 / 3, f.v, f.v * 3);
		else if (interval == 4) 
			testD(f, timeLimit, f.v / 2, f.v / 2, f.v * 3);
		else if (interval == 5)
			testD(f, timeLimit, f.v / 4, f.v / 4, f.v * 3);
		else if (interval == 6)
			testD(f, timeLimit, f.v / 10, f.v / 10, f.v * 3);
	} else if (testType == "alpha") {
		// instance alg k
		uint k = atoi(argv[4]);
		string searchName(argv[3]);
		testAlpha(f, k, searchName == "grasp");
	}
	
    return 0;
}
