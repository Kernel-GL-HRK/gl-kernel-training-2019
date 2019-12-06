#!/bin/bash

I2C_BUS="0"
I2C_ADDR="0x68"
TEMP_HI_REG="0x41"
TEMP_LO_REG="0x42"
PWR_MGMT_1_REG="0x6b"

RET=$(i2cset -y $I2C_BUS $I2C_ADDR $PWR_MGMT_1_REG 0 b)

if [[ $RET == *"Error"* ]]; then
    echo "Error - cannot initialize"
    exit 1
fi

while [ 1 ]; do
    CURR_TIME=$(date +%T)
    HEX_TEMP_HI=$(i2cget -y $I2C_BUS $I2C_ADDR $TEMP_HI_REG b)
    HEX_TEMP_LO=$(i2cget -y $I2C_BUS $I2C_ADDR $TEMP_LO_REG b)
    TMP=$(( HEX_TEMP_HI << 8 | HEX_TEMP_LO ))
    if (( $TMP > 32767 )); then
        TMP=$(( $TMP - 65536 ))
    fi
    TMPC=$(echo "scale=4;$TMP/340+36.53" | bc)
    TMPF=$(echo "scale=4;9/5*$TMPC+32" | bc)
    echo "$CURR_TIME | $TMPC C | $TMPF F"
    sleep 1
done
