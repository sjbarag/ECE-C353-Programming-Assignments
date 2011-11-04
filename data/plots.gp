# Gnuplot plot-generating script for ECE-C353 Homework 4
# Note that this script requires the LUA Gnuplot terminal, which is not
# distributed in all packages, and the gnuplot-lua-tikz LaTeX package to
# process it.  This can be found here:
#
# http://www.sfr-fresh.com/unix/misc/gnuplot-4.4.2.tar.gz:t/gnuplot-4.4.2/share/LaTeX/gnuplot-lua-tikz.sty
#
# Execute this script with $gnuplot plots.gp
# Some of the settings have been omitted here, as they are already contained in
# my personal .gnuplotrc file.
#
# Author: Sean Barag <sjb89@drexel.edu>

set terminal tikz color solid size 3in,2.25in

# --- 2 threads ----
set border 15
set key left top
set xtics 50000000
set title 'Histogram Improvements: 2 Threads'
set xlabel 'Number of Elements'
set ylabel 'Execution Time (s)'
set output "../report/2threadsPlot.tex"
plot "data2.txt" using 1:2 with lines title 'Single-Threaded', \
	"data2.txt" using 1:3 with lines title 'Multi-Threaded'

# --- 4 threads ----
set border 15
set key left top
set xtics 50000000
set title 'Histogram Improvements: 4 Threads'
set xlabel 'Number of Elements'
set ylabel 'Execution Time (s)'
set output "../report/4threadsPlot.tex"
plot "data4.txt" using 1:2 with lines title 'Single-Threaded', \
	"data4.txt" using 1:3 with lines title 'Multi-Threaded'

# --- 8 threads ----
set border 15
set key left top
set xtics 50000000
set title 'Histogram Improvements: 8 Threads'
set xlabel 'Number of Elements'
set ylabel 'Execution Time (s)'
set output "../report/8threadsPlot.tex"
plot "data8.txt" using 1:2 with lines title 'Single-Threaded', \
	"data8.txt" using 1:3 with lines title 'Multi-Threaded'

# --- 16 threads ----
set border 15
set key left top
set xtics 50000000
set title 'Histogram Improvements: 16 Threads'
set xlabel 'Number of Elements'
set ylabel 'Execution Time (s)'
set output "../report/16threadsPlot.tex"
plot "data16.txt" using 1:2 with lines title 'Single-Threaded', \
	"data16.txt" using 1:3 with lines title 'Multi-Threaded'
