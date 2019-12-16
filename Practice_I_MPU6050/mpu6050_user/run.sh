#!/bin/bash

DATA=$(pwd)/data.dat

echo > $DATA

./build/mpu6050_user -r

while true; do

clear

./build/mpu6050_user -m >> $DATA

gnuplot -persist <<-EOFMarker
set terminal dumb
plot "$DATA" using 1:2 with lines,
plot "$DATA" using 1:3 with lines
EOFMarker

sleep 1
done
