#!/bin/bash

if [[ $1 = "-h" || $1 = "--help" ]]; then
    HELP_FLAG=1
    echo "Program prints temperature in degrees Celsius and Fahrenheit"
    echo "Parameter defines timeout in seconds. By default timeout is 1 s"
    exit 0
else
    if [[ $1 = "" ]]; then
        TIME_INTERVAL=1
    else
        TIME_INTERVAL=$1
    fi
HELP_FLAG=0
fi

WHO_AM_I=$(i2cget -y 0 0x68 0x75)

if [[ "$WHO_AM_I" = "0x68" ]]; then
    i2cset -y 0 0x68 0x6B 0x00
    if [ $? -ne 0 ]; then
        echo "Fail while reset device!"
        exit 1
    fi
else
    echo "Device not responding!"
    exit 1
fi

function temp_read {
    local HIGH_BYTE=$(i2cget -y 0 0x68 0x41 b)
    local LOW_BYTE=$(i2cget -y 0 0x68 0x42 b)
    local TEMP_REG=$(( HIGH_BYTE << 8 | LOW_BYTE ))
    (( TEMP_REG > 32767 )) && (( TEMP_REG -= 65536 ))
    TEMP_C=$(bc -l <<< "scale=3;$TEMP_REG/340+36.53")
    TEMP_F=$(bc -l <<< "scale=3;$TEMP_C*9/5+32")
    echo -e "\e[32mT_Celsius = $TEMP_C\e[0m"
    echo -e "\e[33mT_Fahrenheit = $TEMP_F\e[0m"
}

while true; do
    echo -e "\e[31m*******************Temperature***********************\e[0m"
    echo "Timestamp: $(date +%H:%M:%S.%N)"
    temp_read
    echo -e "\e[31m*****************************************************\e[0m"
    sleep $TIME_INTERVAL
done
