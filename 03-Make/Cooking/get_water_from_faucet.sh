#!/bin/bash

WATER=$(cat kitchen/faucet/water.txt)
echo "Put of $1 ml of $WATER to the pot" > $2

