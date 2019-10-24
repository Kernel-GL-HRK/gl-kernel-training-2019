#!/bin/bash
#echo "base=$#"
while (( "$#" )); do
#	echo $1
	PRODUCT="$(echo $1 | cut -d'.' -f1)"
#	echo $PRODUCT
	WEIGHT="$(cat ./food_market/$1)"
#	echo $WEIGHT
	echo -e "The $PRODUCT was bought - $WEIGHT" > "./kitchen/products/$1"
	shift
done

