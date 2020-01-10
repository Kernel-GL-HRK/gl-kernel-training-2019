#!/bin/bash

display_help() {
  echo
  echo "Usage: $0"
  echo
  echo "    --C, get temperature in Celsius "
  echo "    --F, get temperature in Fahrenheit "
  echo "    -h, --help - print these options"
  echo
  echo
}

if [ "$1" = "--help" ] || [ "$1" = "--h" ] ||\
	[[ "$1" != "--C" && "$1" != "--F" ]] ; then
	display_help
	exit 1
fi

i2cdetect -y 0
i2cset -y 0 0x68 0x6b 0x80
i2cset -y 0 0x68 0x6b 0

while true; do

thex=$(i2cget -y 0 0x68 0x41 w)
tl=$(($thex%256))
th=$(($thex/256))
t=$(echo "$tl*256+$th" | bc)

if [ t > 32767 ]; then
	t=$(($t-65536))
fi

tC=$(echo "scale=5; ($t)/340+36.53" | bc)
tF=$(echo "scale=5; ((($tC*9)/5)+32)" | bc )

TIME=$(date +%H:%M:%S)

if [ "$1" = "--C"  ]; then
	echo $TIME The temperature is $tC C HEX: $thex
else
	echo $TIME The temperature is $tF F HEX: $thex
fi

sleep 1

done
