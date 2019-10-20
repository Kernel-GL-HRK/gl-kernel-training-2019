#!/bin/bash

OIL=$(cat kitchen/table/oil.txt)
echo "Put $1 g of the $OIL to the hot frying pan" >> $2

