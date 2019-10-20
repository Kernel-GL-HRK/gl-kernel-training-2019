#!/bin/sh

for i in `seq 1 $#`;
do
	NAME="$(echo $1 | cut -f1 -d'.')"
	STR="$(cat ./food_market/$1 | sed -n '1p')"
	echo "Have bought $NAME - $STR" > "./kitchen/products/$1"
	shift
done

