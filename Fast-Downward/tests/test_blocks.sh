#!/bin/bash
echo > output.txt
echo > results_blocks.txt
for i in "4-0" "4-1" "4-2" "8-0" "8-1" "8-2" "12-0" "12-1" "12-2" "16-1"; do
	echo "  instance $i"
	echo "  instance $i" >> output.txt
	echo

	echo "	Default"
	(
		ulimit -t 60
		ulimit -v 2000000
		./fast-downward.py ipc/blocks/domain.pddl "ipc/blocks/probBLOCKS-$i.pddl" --search "astar(ipdb(num_samples=1000))" | grep "Plan cost\|Expanded [[:digit:]]\|Search time\|Total time\|Hill climbing generator collection"
	) > temp
	cat temp
	cat temp >> output.txt
	NODES=$(cat temp | grep "Expanded" | tr -d "Expanded " | tr -d " state(s).")
	TOTALTIME=$(cat temp | grep "Total" | tr -d "Total time: " | tr -d " s")
	SEARCHTIME=$(cat temp | grep "Search" | tr -d "Search time: " | tr -d " s")
	PREPROCESSTIME=$(python -c "print $TOTALTIME-$SEARCHTIME")
	if ! [[ -z "${NODES}" ]];
	then
		echo "default	$i	$NODES	$PREPROCESSTIME	$SEARCHTIME	$TOTALTIME" >> results_blocks.txt
	else
		echo "default	$i	-	-	-	-" >> results_blocks.txt
	fi

	echo "	Double"
	(
		ulimit -t 60
		ulimit -v 2000000
		./fast-downward.py ipc/blocks/domain.pddl "ipc/blocks/probBLOCKS-$i.pddl" --search "astar(ipdb(num_samples=1000,neighborhood=2))" | grep "Plan cost\|Expanded [[:digit:]]\|Search time\|Total time\|Hill climbing generator collection"
	) > temp
	cat temp
	cat temp >> output.txt
	NODES=$(cat temp | grep "Expanded" | tr -d "Expanded " | tr -d " state(s).")
	TOTALTIME=$(cat temp | grep "Total" | tr -d "Total time: " | tr -d " s")
	SEARCHTIME=$(cat temp | grep "Search" | tr -d "Search time: " | tr -d " s")
	PREPROCESSTIME=$(python -c "print $TOTALTIME-$SEARCHTIME")
	if ! [[ -z "${NODES}" ]];
	then
		echo "double	$i	$NODES	$PREPROCESSTIME	$SEARCHTIME	$TOTALTIME" >> results_blocks.txt
	else
		echo "double	$i	-	-	-	-" >> results_blocks.txt
	fi

	echo "	Double+"
	(
		ulimit -t 60
		ulimit -v 2000000
		./fast-downward.py ipc/blocks/domain.pddl "ipc/blocks/probBLOCKS-$i.pddl" --search "astar(ipdb(num_samples=1000,neighborhood=3))" | grep "Plan cost\|Expanded [[:digit:]]\|Search time\|Total time\|Hill climbing generator collection"
	) > temp
	cat temp
	cat temp >> output.txt
	NODES=$(cat temp | grep "Expanded" | tr -d "Expanded " | tr -d " state(s).")
	TOTALTIME=$(cat temp | grep "Total" | tr -d "Total time: " | tr -d " s")
	SEARCHTIME=$(cat temp | grep "Search" | tr -d "Search time: " | tr -d " s")
	PREPROCESSTIME=$(python -c "print $TOTALTIME-$SEARCHTIME")
	if ! [[ -z "${NODES}" ]];
	then
		echo "double+	$i	$NODES	$PREPROCESSTIME	$SEARCHTIME	$TOTALTIME" >> results_blocks.txt
	else
		echo "double+	$i	-	-	-	-" >> results_blocks.txt
	fi

	echo "	Initial"
	(
		ulimit -t 60
		ulimit -v 2000000
		./fast-downward.py ipc/blocks/domain.pddl "ipc/blocks/probBLOCKS-$i.pddl" --search "astar(ipdb(num_samples=1000,pdb_max_size=1))" | grep "Plan cost\|Expanded [[:digit:]]\|Search time\|Total time\|Hill climbing generator collection"
	) > temp
	cat temp
	cat temp >> output.txt
	NODES=$(cat temp | grep "Expanded" | tr -d "Expanded " | tr -d " state(s).")
	TOTALTIME=$(cat temp | grep "Total" | tr -d "Total time: " | tr -d " s")
	SEARCHTIME=$(cat temp | grep "Search" | tr -d "Search time: " | tr -d " s")
	PREPROCESSTIME=$(python -c "print $TOTALTIME-$SEARCHTIME")
	if ! [[ -z "${NODES}" ]];
	then
		echo "initial	$i	$NODES	$PREPROCESSTIME	$SEARCHTIME	$TOTALTIME" >> results_blocks.txt
	else
		echo "initial	$i	-	-	-	-" >> results_blocks.txt
	fi

	echo "	Blind"
	(
		ulimit -t 60
		ulimit -v 2000000
		./fast-downward.py ipc/blocks/domain.pddl "ipc/blocks/probBLOCKS-$i.pddl" --search "astar(blind())" | grep "Plan cost\|Expanded [[:digit:]]\|Search time\|Total time\|Hill climbing generator collection"
	) > temp
	cat temp
	cat temp >> output.txt
	NODES=$(cat temp | grep "Expanded" | tr -d "Expanded " | tr -d " state(s).")
	TOTALTIME=$(cat temp | grep "Total" | tr -d "Total time: " | tr -d " s")
	SEARCHTIME=$(cat temp | grep "Search" | tr -d "Search time: " | tr -d " s")
	PREPROCESSTIME=$(python -c "print $TOTALTIME-$SEARCHTIME")
	if ! [[ -z "${NODES}" ]];
	then
		echo "blind	$i	$NODES	$PREPROCESSTIME	$SEARCHTIME	$TOTALTIME" >> results_blocks.txt
	else
		echo "blind	$i	-	-	-	-" >> results_blocks.txt
	fi

	echo
done
rm temp