
#include <map>

class UMDA {
private:

	uint p, s, c;

public:

	UMDA(uint p, uint s, uint c) {
		this->p = p; this->s = s; this->c = c;
	}

	void estimate(Formula& formula) {
		// Initial solutions.
		bool solutions[p][formula.v];
		for (uint i = 0; i < p; i++)
			formula.resetSolution(solutions[i]);

		while (true) {
			// Create model.
			double model[formula.v];
			for (uint v = 0; v < formula.v; v++) {
				model[v] = 0;
				for (uint i = 0; i < p; i++) {
					model[v] += solutions[i][v];
				}
				model[v] /= p;
			}
			// Create new solutions.
			bool newSolutions[c][formula.v];
			uint values[c];
			uint order[c];
			for (uint i = 0; i < c; i++) {
				for (uint v = 0; v < formula.v; v++) {
					newSolutions[i][v] = rand() * 1.0 / RAND_MAX < model[v];
				}
				values[i] = formula.evaluate(newSolutions[i]);
				order[i] = i;
			}
			// Select best.			
			for (int i = 0; i < c; i++)
			bool compareInterval(Interval i1, Interval i2) 
{ 
    return (i1.start < i2.start); 
} 
			sort(order, order+c, compareInterval); 

			uint values[c];
			uint best = 0, worst = -1;
			for (uint i = 0; i < c; i++) {
				values[i] = formula.evaluate(newSolutions[i]);
				if (values[i] < worst)
					worst = values[i];
				if (values[i] > best)
					best = values[i];
			}
			for (uint i = 0; i < c; i++) {

			}
		}

	}

};

class MIMIC {
	


};