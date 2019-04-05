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
#define Clause vector<int>

struct Formula {
    int v, c;
    Clause* clauses;
    ~Formula() { delete [] clauses; }
};

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
    ifstream file("flat75-1.cnf");
    if (!file) {
        cerr << "Unable to open file";
        return -1;
    }
    Formula f;
    readFormula(f, file);
    file.close();
    return 0;
}
