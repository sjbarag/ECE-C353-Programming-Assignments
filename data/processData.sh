#!/bin/bash
#
# Re-orders data so that it can be easily plotted by gnuplot
# Author: Sean Barag <sjb89@drexel.edu>
IN="data_10.txt"
OUT="data_processed.txt"
OUT2="data_calculated.txt"

# split data into separate files
for i in 2 4 8 16
do
	cat $IN | grep "^$i" | awk '{print $3}' > /tmp/pData$i.txt
done

# paste files together
paste /tmp/pData2.txt /tmp/pData4.txt /tmp/pData8.txt /tmp/pData16.txt > $OUT

# delete temporary files
rm /tmp/pData{2,4,8,16}.txt

# calculate min, max, and avg
python calcData.py > $OUT2
