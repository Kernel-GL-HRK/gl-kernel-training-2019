#!/bin/bash

if [[ $1 = "-h" || $1 = "--help" ]]; then
	echo -e "Test program for printing temperature from MPU6050."
else
	echo -e "time, degrees Celsius, degrees Fahrenheit"
	i2cset -y 0 0x68 0x6B 0x00
	while true; do
		ALL=$(($(i2cget -y 0 0x68 0x41 b) << 8  \
			| $(i2cget -y 0 0x68 0x42 b)))
		(( ALL > 32767 )) && (( ALL -= 65536 ))
		TC=$(bc -l <<< "scale=2;$ALL/340+36.53")
		TF=$(bc -l <<< "scale=2;$TC*9/5+32")
		echo "$(date +%H:%M:%S), $TC, $TF"
		sleep 1
	done
fi
