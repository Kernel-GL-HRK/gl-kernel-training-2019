#!/bin/bash

OUT_FILE=out.txt

./program_07 $1 | sort -n > $OUT_FILE

gnuplot -persist <<-EOFMarker
# set terminal pdf
# set output "out.pdf"
set xlabel "num alloc memory, byte"
set ylabel "latency, ns"
set format y '%g' 
set logscale y
set logscale x
plot \
"$OUT_FILE" using 1:2 with lines title columnheader,\
"$OUT_FILE" using 1:4 with lines title columnheader,\
"$OUT_FILE" using 1:6 with lines title columnheader
EOFMarker
