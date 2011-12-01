#!/bin/bash
# Gets data for the ECE-C353 final project
SIG_TEXT="Scriptable output:"
> data.txt
for loop in {1..10}
do
	for threads in 2 4 8 16
	do
		out/work_crew double ~/school $threads | grep "$SIG_TEXT" | sed "s/$SIG_TEXT\t//" >> data.txt
	done
done
