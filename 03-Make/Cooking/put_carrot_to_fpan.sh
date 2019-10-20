#!/bin/bash

CARROT=$(cat kitchen/table/carrot.txt)
echo "Chop the $1 $CARROT and put it to the frying pan and fry it" > $2

#cp kitchen/table/carrot.txt $1
