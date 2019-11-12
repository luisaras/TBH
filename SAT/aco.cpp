#include <cmath>
#include <random>

class ACO {
public:

	ACO(Formula& f, double a, double b, double r, uint m = 10) : formula(f) {
		solutions = new bool*[m];
		for (int i = 0; i < m; i++)
			solutions[i] = new bool[f.v];
		tau[0] = new double[f.v];
		tau[1] = new double[f.v];
		eta[0] = new double[f.v];
		eta[1] = new double[f.v];
		this->m = m;
		alpha = a;
		beta = b;
		rho = r;
	}

	~ACO() {
		for (int i = 0; i < m; i++)
			delete [] solutions[i];
		delete [] solutions;
		delete [] tau[0];
		delete [] eta[0];
		delete [] tau[1];
		delete [] eta[1];
	}

	uint search(bool* solution) {
		
		for (int i = 0; i < formula.v; i++) {
			// Dynamic: all 1
			tau[0][i] = tau[1][i] = 1;
			// Static: Percentage of clauses that contain variable i
			eta[0][i] = eta[1][i] = 0;
			for (uint c : formula.varClauses[0][i])
				eta[0][i]++;
			for (uint c : formula.varClauses[1][i])
				eta[1][i]++;
			eta[0][i] /= formula.c;
			eta[1][i] /= formula.c;
		}

		// Initial solutions
		double g[m];
		uint best = step(g); // g of current best solution
		formula.copySolution(solutions[best], solution);
		best = g[best];

		uint convergence = 0;
		while (true) {
			if (best == formula.c || convergence == m) {
				return best;
			}
			update_tau(best * 1.0 / formula.c, solution);
			uint new_best = step(g);
			if (g[new_best] <= best) {
				convergence++;
			} else {
				convergence = 0;
				best = g[new_best];
				formula.copySolution(solutions[new_best], solution);
			}
		}

		return 0;
	}

protected:

	// Parameters
	uint m;
	double alpha, beta, rho;

	// Execution data
	bool** solutions;
	double* tau[2];
	double* eta[2];
	Formula& formula;
	uint best_solution;
	uint best_value = 0;
	uint steps = 0;

	inline uint step(double* g) {
		uint best = 0;
		for (int i = 0; i < m; i++) {
			build(solutions[i]);
			uint c = formula.evaluate(solutions[i]);
			if (c == formula.c) {
				return c;
			}
			g[i] = c;
			if (c > g[best])
				best = i;
		}
		steps++;
		return best;
	}

	inline uint build(bool* solution) {
		for (int i = 0; i < formula.v; i++) {
			double prob0 = pow(tau[0][i], alpha) * pow(eta[0][i], beta);
			double prob1 = pow(tau[1][i], alpha) * pow(eta[1][i], beta);
			double r = rand() * 1.0 / RAND_MAX;
			solution[i] = r * (prob0 + prob1) < prob1;
		}
	}

	inline void update_tau(double gs, bool* s) {
		for (int i = 0; i < formula.v; i++) {
			double v[2] = {0, 0};
			v[s[i]] = gs;
			tau[0][i] = tau[0][i] * (1 - rho) + v[0] * rho;
			tau[1][i] = tau[1][i] * (1 - rho) + v[1] * rho;
		}
	}

};
