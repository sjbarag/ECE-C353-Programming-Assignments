#!/usr/bin/python
f = open('data_processed.txt', 'r')
lines = []
for line in f:
	lines.append(line.strip().split('\t'))

cols = []
for i in range(4):
	cols.append([row[i] for row in lines])

print "#num threads\taverage\t\tmin\t\tmax"
for i in range(4):
	print 2**(i+1), "\t\t", sum(float(v) for v in lines[i])/len(lines[i]), "\t", min(lines[i]), "\t", max(lines[i])
