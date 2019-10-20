#!/bin/bash

# by foods in the market

CHIKEN=$(cat food-market/chicken.txt)
POTATO=$(cat food-market/potato.txt)
SALT=$(cat food-market/salt.txt)
ONION=$(cat food-market/onion.txt)
CARROT=$(cat food-market/carrot.txt)
OIL=$(cat food-market/oil.txt)
GREENPEAS=$(cat food-market/greenpeas.txt)

echo "Buy $CHIKEN, $POTATO, $CARROT, $SALT, $ONION, $OIL, $GREENPEAS in the food market" > $1

cp food-market/chicken.txt kitchen/table/
cp food-market/potato.txt kitchen/table/
cp food-market/carrot.txt kitchen/table/
cp food-market/onion.txt kitchen/table/
cp food-market/salt.txt kitchen/table/
cp food-market/oil.txt kitchen/table/
cp food-market/greenpeas.txt kitchen/table/



