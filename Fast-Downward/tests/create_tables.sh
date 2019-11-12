#!/bin/bash
./test_8puzzle.sh
./test_gripper.sh
./test_blocks.sh
./test_sokoban.sh
for a in default double double+ initial blind; do
	echo > "results_$a.txt"
	for p in gripper blocks 8puzzle sokoban; do
		cat "results_$p.txt" | grep $a >> "results_$a.txt"
	done
done