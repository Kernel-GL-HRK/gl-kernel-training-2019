#!/bin/bash
function view_help {
	echo "-h, --help	help information"
	echo "This programm for print temperature from mpu6050 in Celsius and Fahrenheit"
}

function conv_16_bit_hex_dec_signed {
    res=$(printf "%d" "0x$1")
    (( res > 32767 )) && (( res -= 65536 ))
    echo $res
}
function get_temperature {
    H_BYTE_MPU6060=$(i2cget -y 0 0x68 0x41 b)
    L_BYTE_MPU6060=$(i2cget -y 0 0x68 0x42 b)
    TWO_BYTE_HEX="${H_BYTE_MPU6060:2}${L_BYTE_MPU6060:2}"
    TWO_BYTE_DEC_convert=$(conv_16_bit_hex_dec_signed $TWO_BYTE_HEX )
#    echo "TWO_BYTE_DEC=$TWO_BYTE_DEC_convert"
    echo "Temp Celsiy $( echo "scale=3; $TWO_BYTE_DEC_convert/340+36.53" | bc)"
    echo "Temp Fahrenheit $(echo "scale=3; (($TWO_BYTE_DEC_convert/340+36.53)*9)/5+32" | bc)"
	LONG_WORD=$(i2cget -y 0 0x68 0x41 w)
	CONVERT_LONG_WORD=$(((LONG_WORD & 0xFF)<<8 | (LONG_WORD>>8) & 0xFF))
	(( CONVERT_LONG_WORD > 32767 )) && (( CONVERT_LONG_WORD -= 65536 ))
	Celsiy=$(bc -l <<< "scale=20;$CONVERT_LONG_WORD/340+36.53")
	Fahrenheit=$(bc -l <<< "scale=20;$Celsiy*9/5+32")
	echo "Temp long_word"
	echo "Celsius $Celsiy "
	echo "Fahrenheit $Fahrenheit"
}
function get_temp_C_and_F {
	echo "Get temperature from mpu6050 in Celsius and Fahrenheit"
	CHECK_DEV=$(i2cget -y 0 0x68 0x75)
	if [[ "$CHECK_DEV" -eq "0x68" ]]; then
	    i2cset -y 0 0x68 0x6B 0x00
	    if [ $? != 0 ]; then
        	echo "Can not reset mpu6050!"
	        exit 1
	    else
		echo "Reset mpu6050!"
	    fi
	else
	    echo "!!!!mpu6050 do not answer!!!!"
	    exit 1
	fi
	while true;
	do
	    echo "                       Temperature                               "
	    echo "-----------------------------------------------------------------"
	    echo "Timestamp: $(date +%H:%M:%S.%N)"
	    get_temperature
	    echo "-----------------------------------------------------------------"
	    sleep 1
	done
}
function parsing_got_arguments  {
	case $1 in
		"-h"| "--help")
			view_help
			exit 0
		;;
		"")
			get_temp_C_and_F
		;;
		*)
			echo "Key $1 is mistake, please use $0 --help for more information"
			exit 0
		;;
	esac
}
parsing_got_arguments $1
