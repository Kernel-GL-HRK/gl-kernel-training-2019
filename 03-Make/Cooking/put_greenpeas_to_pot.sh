#!/bin/bash

GREENPEAS=$(cat kitchen/table/greenpeas.txt)
echo "Put $1 g of the $GREENPEAS to the pot and boil 3 min" > $2

