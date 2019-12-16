#!/bin/bash

x=1
max_x=100
./build/mpu6050_user -r

echo "Start reading $max_x points."

start_time=$(($(date +%s%N)))
while [ $x -le $max_x ]; do

./build/mpu6050_user -m > /dev/null

x=$(( $x + 1 ))
#echo $x
done

end_time=$(($(date +%s%N)))

echo "$(( (end_time - start_time)/max_x )) nanoseconds/points"
echo "$(( ((end_time - start_time)/max_x)/1000000 )).$(( ((end_time - start_time)/max_x)%1000000 )) ms/points"