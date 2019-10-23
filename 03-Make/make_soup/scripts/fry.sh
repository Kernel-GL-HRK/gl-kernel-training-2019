#!/bin/bash

varCarrot=$(cat $1)
varOnion=$(cat $2)

echo "fryed $varCarrot and $varOnion" > $3
