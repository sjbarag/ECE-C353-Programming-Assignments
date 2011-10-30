#!/bin/bash
DATAFILE="data.txt"
SIG_TEXT="Scriptable output:"

# reset data file
echo "" > "$DATAFILE"

for i in $(seq 1000000 1000000 100000000)
do
	out/histogram $i | grep "$SIG_TEXT" | sed "s/$SIG_TEXT\t //" >> data.txt
done
