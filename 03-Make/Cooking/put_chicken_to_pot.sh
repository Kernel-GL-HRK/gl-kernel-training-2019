#!/bin/bash

CHICKEN=$(cat kitchen/table/chicken.txt)
echo "Put $1 g of the $CHICKEN to the pot and boil 60 min" > $2

