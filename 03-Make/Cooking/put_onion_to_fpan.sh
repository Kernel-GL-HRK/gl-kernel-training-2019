#!/bin/bash

ONION=$(cat kitchen/table/onion.txt)
echo "Cut $1 st. of the $ONION put it to the frying pan and fry it" > $2

