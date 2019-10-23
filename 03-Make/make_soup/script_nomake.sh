#!/bin/sh

mkdir -p ./food_market/
mkdir -p ./kitchen/pot
mkdir -p ./kitchen/products
mkdir -p ./kitchen/sliced
echo "$1" > ./food_market/meat.txt
echo "$2" > ./food_market/potatoes.txt
echo "$3" > ./food_market/onions.txt
echo "$4" > ./food_market/carrot.txt
echo "$5" > ./food_market/salt.txt
echo "$6" > ./kitchen/water.txt

if [ -f ./buy.sh ]; then
	./buy.sh meat.txt potatoes.txt onions.txt carrot.txt salt.txt
else
	exit 1
fi

if [ -f ./slice.sh ]; then
        ./slice.sh meat.txt potatoes.txt onions.txt carrot.txt salt.txt
else
	exit 1
fi

if [[ -f./water.sh && -f ./kitchen/water.txt ]]; then
	./water.sh water.txt
else
	exit 1
fi

cp kitchen/sliced/* kitchen/pot
sed "-es/Have bought salt/Add salt to soup/" < \
	kitchen/products/salt.txt > kitchen/pot/salt.txt
echo "<<<<<<<<<< Cooking soup >>>>>>>>>>"
cat ./kitchen/products/*.txt
sort -r ./kitchen/pot/*.txt
echo "Done!"
