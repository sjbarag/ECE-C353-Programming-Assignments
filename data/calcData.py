#!/usr/bin/python
#
# Performs calculations on data to get the mean, min, and max
# Author: Sean Barag <sjb89@drexel.edu>

# open the data file and read it
lines = []
f = open('data_processed.txt', 'r')
for line in f:
	lines.append(line.strip().split('\t'))
f.close()

# transpose it.  Gotta love list comprehensions
cols = []
for i in range(4):
	cols.append([row[i] for row in lines])


# open the other data file, because I forgot to pull the single-threaded data
# out in the previous script.
f = open('data_100.txt', 'r')
slines = []
for line in f:
	if line != '\n':
		slines.append(line.strip().split('\t'))
f.close()

# add the single-threaded data
cols.insert(0, [row[1] for row in slines])

# calculate and print
print "#num threads\taverage\t\tmin\t\tmax"
for i in range(len(cols)):
	print 2**(i), "\t\t", sum(float(v) for v in cols[i])/len(cols[i]), "\t", min(cols[i]), "\t", max(cols[i])
