#!/bin/bash

POTATO=$(cat kitchen/table/potato.txt)
echo "Peel $1 g of the $POTATO cut it and put to the pot and boil 20-30 min" > $2

