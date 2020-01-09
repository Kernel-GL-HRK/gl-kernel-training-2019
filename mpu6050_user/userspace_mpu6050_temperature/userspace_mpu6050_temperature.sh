#!/bin/bash

declare -i Word=0
declare -i SWord=0
declare -i TC=0
declare -i TCi=0
declare -i TCf=0
declare -i TF=0
declare -i TFi=0
declare -i TFf=0

# start sensor
i2cset -y 0 0x68 0x6b 0x00

#infinity loop
while :
do
# get data from mpu6050
Word=$(i2cget -y 0 0x68 0x41 w)

# convert to signed value
let "SWord=Word<<8|Word>>8"
let "SWord&=65535"

if [[ SWord -gt 32767 ]]; then
    SWord=$(bc <<< "$SWord-65536")
fi

# get temperature Celsius * 100
TC=$(bc <<< "$SWord*100/340+3656")

TCi=TC/100 # get integer part
TCf=TC%100 # get fractional part

# convert Celsium to Fahrenheit * 1000
TF=$(bc <<< "$TC*18+32000")

TFi=TF/1000 # get integer part
TFf=TF%1000/10 # get fractional part

date # show timestamp
printf "Tc = %i.%02iC, Tf = %i.%02iF\n\n"\
 $TCi $TCf $TFi $TFf #show current celsius and fahrenheit temperature

sleep 1 # wait 1 second
done
