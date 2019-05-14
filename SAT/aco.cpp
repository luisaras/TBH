#include "formula.cpp"
#include <map>
	using std::map;
#include <cmath>
#include <random>

class ACO {
public:

	ACO(Formula& f, double a, double b, double r, uint m = 10) : formula(f), unif(0, 1) {
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
		for (int i = 0; i < formula.v; i++)
			tau[0][i] = tau[1][i] = 1;

		// Static: Percentage of clauses that contain variable i
		for (int i = 0; i < formula.v; i++) {
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
		uint best = step(g, solution);
		if (g[best] == 1) 
			return best;

		for (int i = 0; i < formula.v; i++)
			tau[0][i] = tau[1][i] = 0;

		while (true) {
			update_tau(g);
			uint new_best = step(g, solution);
			if (g[new_best] == 1) 
				return new_best;
			best = new_best;
			cout << g[best] << endl;
		}

		return -1;
	}

protected:

	// Random
	std::uniform_real_distribution<double> unif;
	std::default_random_engine re;

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

	inline uint step(double* g, bool* solution) {
		uint best = 0;
		for (int i = 0; i < m; i++) {
			build(solutions[i]);
			uint c = formula.evaluate(solutions[i]);
			if (c == formula.c) {
				formula.copySolution(solutions[i], solution);
				return c;
			}
			g[i] = c * 1.0 / formula.c;
			if (g[i] > g[best])
				best = i;
		}
		return best;
	}

	inline uint build(bool* solution) {
		for (int i = 0; i < formula.v; i++) {
			double prob0 = alpha * pow(tau[0][i], alpha) * pow(eta[0][i], beta);
			double prob1 = alpha * pow(tau[1][i], alpha) * pow(eta[1][i], beta);
			solution[i] = unif(re) * (prob0 + prob1) >= prob0;
		}
	}

	inline void update_tau(double* g) {
		for (int i = 0; i < formula.v; i++) {
			double v[2];
			v[0] = v[1] = 0;
			for (uint s; s < m; s++) {
				v[solutions[s][i]] += g[s];
			}
			tau[0][i] = tau[0][i] * rho + v[0];
			tau[1][i] = tau[1][i] * rho + v[1];
		}
	}

};