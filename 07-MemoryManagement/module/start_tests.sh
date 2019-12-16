#!/bin/bash

TMP=$(mktemp)

OUT_FILE=out.txt
NUM_TESTS=5

echo > $OUT_FILE

for (( i=1; i<=$NUM_TESTS; i++ ))
do
	cat /sys/module_07/mem_test >> $OUT_FILE
done

sort -n $OUT_FILE > $TMP

tail -n +$(( $NUM_TESTS+1 )) $TMP > $OUT_FILE



gnuplot -persist <<-EOFMarker
#set terminal pdf
#set output "$OUT_FILE.pdf"
set xlabel "num alloc memory, byte"
set ylabel "latency, ns"
set format y '%g'
set logscale y
set logscale x
plot \
"$OUT_FILE" using 1:2 with lines title columnheader,\
"$OUT_FILE" using 1:4 with lines title columnheader,\
"$OUT_FILE" using 1:6 with lines title columnheader,\
"$OUT_FILE" using 1:8 with lines title columnheader
EOFMarker

rm $TMP
