#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;
    using std::cerr;
#include <fstream>
    using std::ifstream;
    using std::istream;
#include <vector>
    using std::vector;
#include <string>
    using std::string;
#include <cstdlib>
#include <ctime>
#define Clause vector<int>

struct Formula {
    int v, c;
    Clause* clauses;

    ~Formula() { delete [] clauses; }

    int evaluate(bool* values) {
        int e = 0;
        for (int i = 0; i < c; i++) {
            for (int var : clauses[i]) {
                if (var < 0) {
                    if (!values[-var - 1]) {
                        e++; break;
                    }
                } else {
                    if (values[var - 1]) {
                        e++; break;
                    }
                }
            }
        }
        return e;
    }

    inline int testFlip(int i, bool* solution) {
        solution[i] = !solution[i];
        int e = evaluate(solution);
        solution[i] = !solution[i];
        return e;
    }
};

int gsat(Formula& formula, bool* solution) {
    int best = 0;
    int vbest = formula.testFlip(0, solution);
    for (int i = 1; i < formula.v; i++) {
        int vi = formula.testFlip(i, solution);
        if (vi > vbest) {
            best = i;
            vbest = vi;
        }
    }
    solution[best] = !solution[best];
    return vbest;
}

int test(Formula& formula) {
    bool solution[formula.v];
    for (int i = 0; i < formula.v; i++) {
        solution[i] = rand() % 2;
    }
    int current = formula.evaluate(solution);
    cout << "initial solution: " << current << endl;
    while (true) {
        int e = gsat(formula, solution);
        if (e > current)
            current = e;
        else
            break;
    };
    cout << "final solution: " << current << endl;
    return current;
}

void readFormula(Formula& f, istream& stream) {
    string s;
    int c; // current clause
    while (stream >> s) {
        if (s == "c") {
            // Comments
            getline(stream, s);
        } else if (s == "p") {
            // New formula
            stream >> s // "cnf"
                   >> f.v // number of variables
                   >> f.c; // number of clauses
            f.clauses = new Clause[f.c];
            c = 0;
        } else {
            // New clause
            int var;
            while (stream >> var && var != 0) {
                f.clauses[c].push_back(var);
            }
            c++;
        }
    }
}

int main() {
    srand(time(NULL));

    ifstream file("flat75-1.cnf");
    if (!file) {
        cerr << "Unable to open file";
        return -1;
    }
    Formula formula;
    readFormula(formula, file);
    file.close();
    test(formula);
    return 0;
}
