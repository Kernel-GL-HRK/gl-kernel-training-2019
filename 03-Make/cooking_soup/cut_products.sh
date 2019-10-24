#!/bin/bash
#echo "base=$#"
while (( "$#" )); do
#	echo "current_position=$1"
	PRODUCT="$(echo $1 | cut -d'.' -f1)"
#	echo "PRODUCT=$PRODUCT"
	WEIGHT="$(cat ./food_market/$1)"
#	echo "WEIGHT=$WEIGHT"
	VALUE="$(cat ./kitchen/products/$1 | cut -f6 -d' ')"
#	echo "VAL=$VAL"
	NUMBER=$RANDOM
	let "NUMBER %= 50"
	let "VALUE = VALUE / NUMBER"
  	echo "The $PRODUCT at $VALUE to $NUMBER slices" > \
		"./kitchen/sliced_products/$1"
	shift
done

