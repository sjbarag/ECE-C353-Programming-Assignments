#!/usr/bin/python
f = open('data_processed.txt', 'r')
lines = []
for line in f:
	lines.append(line.strip().split('\t'))
f.close()

cols = []
for i in range(4):
	cols.append([row[i] for row in lines])

f = open('data_100.txt', 'r')
slines = []
for line in f:
	if line != '\n':
		slines.append(line.strip().split('\t'))
f.close()

cols.insert(0, [row[1] for row in slines])


print "#num threads\taverage\t\tmin\t\tmax"
for i in range(len(cols)):
	print 2**(i), "\t\t", sum(float(v) for v in cols[i])/len(cols[i]), "\t", min(cols[i]), "\t", max(cols[i])
