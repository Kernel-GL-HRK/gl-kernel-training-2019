#!/bin/sh

for i in `seq 1 $#`;
do
	NAME="$(echo $1 | cut -f1 -d'.')"
	VAL="$(cat ./kitchen/products/$1 | sed -n '1p' | cut -f5 -d' ')"
	case "$NAME" in
	"meat")
	let "VAL = VAL / 50"
  	echo "Shop $NAME at 50 grams to $VAL slices, add them to the pot" > \
		"./kitchen/sliced/$1"
	;;
	"potatoes")
	let "VAL = VAL / 13"
	echo "Shop $NAME at 13 grams to $VAL slices, add them to the pot" > \
		"./kitchen/sliced/$1"
	;;
	"onions")
	let "VAL = VAL / 8"
	echo "Shop $NAME at 8 grams to $VAL slices, add them to the pot" > \
		"./kitchen/sliced/$1"
	;;
	"carrot")
	let "VAL = VAL /25"
	echo "Shop $NAME at 25 grams to $VAL slices, add them to the pot" > \
		"./kitchen/sliced/$1"
	;;
	*)
	;;
	esac
	shift
done

