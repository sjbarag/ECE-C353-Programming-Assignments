#!/bin/bash
DATAFILE="data.txt"
SIG_TEXT="Scriptable output:"

# reset data file
echo "" > "$DATAFILE"

for threads in 2 4 8 16
do
	echo "Getting data for $threads threads..."
	for elem in $(seq 1000000 1000000 100000000)
	do
		out/histogram $elem $threads | grep "$SIG_TEXT" | sed "s/$SIG_TEXT\t //" >> data$threads.txt
	done
done
