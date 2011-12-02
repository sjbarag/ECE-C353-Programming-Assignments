# Gnuplot plot-generating script for ECE-C353's final project
# Note that this script requires the LUA Gnuplot terminal, which is not
# distributed in all packages, and the gnuplot-lua-tikz LaTeX package to
# process it.  This can be found here:
#
# http://www.sfr-fresh.com/unix/misc/gnuplot-4.4.2.tar.gz:t/gnuplot-4.4.2/share/LaTeX/gnuplot-lua-tikz.sty
#
# Execute this script with $gnuplot plots.gp
#
# Author: Sean Barag <sjb89@drexel.edu>

set terminal tikz color solid size 4.25in,2.25in
set border 15
set xrange [0:18]

set title 'Multithreaded Recursive Text Search'
set xlabel 'Number of Threads'
set ylabel 'Time (s)'
unset key

set output "../report/img/plot/multiThreadPlot.tex"
plot 'data_calculated.txt' using 1:2:3:4 with yerrorbars
