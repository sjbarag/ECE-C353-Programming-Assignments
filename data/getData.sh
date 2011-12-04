#!/bin/bash
#
# Gets data for the ECE-C353 final project
# Author: Sean Barag <sjb89@drexel.edu>
SIG_TEXT="Scriptable output:"
STRING="kandasamy"
DIR="/home/DREXEL/nk78"
NUM_RUNS=100
OUT="data_100.txt"

> $OUT
for loop in {1..$NUM_RUNS}
do
	echo "Running... ($loop/$NUM_RUNS)"
	for threads in 2 4 8 16
	do
		out/work_crew $STRING $DIR $threads | grep "$SIG_TEXT" | sed "s/$SIG_TEXT\t//" >> $OUT
	done
done
echo "All done!"
