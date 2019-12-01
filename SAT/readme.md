
Compile:
g++ -std=c++14 main.cpp -o main

Test knuth algorithms:
* ./main knuth INST ALG
* INST = the file name (without .cnf) from "instances" folder;
* ALG = A, B or D.
* Example: ./main knuth very-simple A

Change PRINT directive in knuth.cpp to debug, and ALL_SOLUTIONS to fetch all satisfying assignments.
