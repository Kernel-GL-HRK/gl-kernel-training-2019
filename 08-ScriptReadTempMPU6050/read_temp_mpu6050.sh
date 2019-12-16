#!/bin/sh

if [[ $1 = "-h" || $1 = "--help" ]]; then
    	echo "-h, --help     display this help and exit"
	echo "[numb] 	     numbers Ñsample (default 1)"
    	exit 0
fi

if [[ ! -n $1 ]]; then
	NUMB_REP=1
else
	NUMB_REP=$1
fi

COUNT=0

i2cset -y 0 0x68 0x6B 0
sleep 0.1

while [ $COUNT -lt $NUMB_REP ]
do
	RD_REG=$(i2cget -y 0 0x68 0x41 w)
	VALUE_REG=$(((RD_REG & 0xFF)<<8 | (RD_REG>>8) & 0xFF))
	(( VALUE_REG > 32767 )) && (( VALUE_REG -= 65536 ))
	C=$(bc -l <<< "scale=4;$VALUE_REG/340+36.53")
	F=$(bc -l <<< "scale=4;$C*9/5+32")
	echo "Time     Celsius    Fahrenheit"
	echo "$(date +%H:%M:%S)" "$C" $'\xc2\xb0'C "$F" $'\xc2\xb0'F
	COUNT=$((COUNT+1))
done

exit 0
