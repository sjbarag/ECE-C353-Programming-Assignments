#!/bin/bash
# Gets data for the ECE-C353 final project
SIG_TEXT="Scriptable output:"
STRING="kandasamy"
DIR="/home/DREXEL/nk78"
NUM_RUNS=10

> data.txt
for loop in {1..100}
do
	echo "Running... ($loop/$NUM_RUNS)"
	for threads in 2 4 8 16
	do
		out/work_crew $STRING $DIR $threads | grep "$SIG_TEXT" | sed "s/$SIG_TEXT\t//" >> data2.txt
	done
	# convenient line break
	echo >> data2.txt
done
echo "All done!"
