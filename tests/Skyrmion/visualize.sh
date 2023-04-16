#!/bin/bash
###This is a bash script that generates plots using Gnuplot.
#The script first sets a variable "run_folder" to the current working directory using the command PWD. Then it removes a folder called "Plots" if it already exists, and creates a new one with the same name.
#The script then runs a loop that goes from 10 to 29 in increments of -1. Within the loop, it creates a file called "plot.data" by using the paste command to merge two input files: "atoms-coords.data" and "spins-000000$i.data". The variable $i is used to substitute the loop counter in the file name "spins-000000$i.data".
#The script then creates a Gnuplot script called "plot.gnu" using a "here document" (a way of including multi-line text in a script). The Gnuplot script sets various parameters for the plot, including the output format (PNG), the size, the color range, the ranges for the x, y, and z axes, and the palette for the colors. It also sets the layout for the plot to have two subplots, with a title that includes the current loop counter. Finally, it specifies how the data should be plotted, including vectors and points.
#The script then runs the Gnuplot script using the command gnuplot plot.gnu, and renames the output file "out_config.png" to "Plots/config_$i'.png'", where $i is again the loop counter.
#Overall, the script generates a series of plots with vector fields and points, based on input data files, and saves them in a folder called "Plots".
###


###
run_folder=$PWD
rm -r Plots
mkdir Plots
for i in $(seq 10 1 30)
do

paste atoms-coords.data spins-000000$i.data > plot.data

cat <<EOF >plot.gnu
set terminal pngcairo size 1200,600 font "Helvetica,14" background rgb 'gray'
set output 'out_config.png'
set view 0,0
set size square
set cbrange [-1:1]
set xrange [-1:21]
set yrange [-1:21]
set palette defined (-1 "blue", 0 "white", 1 "red")
set xlabel "x";set ylabel "y"; set zlabel "z"
set multiplot layout 1,2 title 'Frame $i'
sp "plot.data" u 3:4:5:(\$6):(\$7):(\$8):8 w vectors palette lw 1.5 t 'M', '' u 3:4:5:8 w points palette pt 7 ps 1 t 'Mz'
set view 90,0
sp "plot.data" u 3:4:5:(\$6):(\$7):(\$8):8 w vectors palette lw 1.5 t 'M', '' u 3:4:5:8 w points palette pt 7 ps 1 t 'Mz'

unset multiplot
EOF

gnuplot plot.gnu
mv out_config.png Plots/config_$i'.png'


done