#!/bin/bash

#DD_OIL=$(cat kitchen/table/fpan/add_oil.txt)
#ADD_ONION=$(cat kitchen/table/fpan/add_onion.txt)
#ADD_CARROT=$(cat kitchen/table/fpan/add_carrot.txt)

#echo $ADD_OIL > $1
#echo $ADD_ONION >> $1
#echo $ADD_CARROT >> $1

cat kitchen/table/fpan/add_oil.txt > kitchen/table/fpan/frying.txt
cat kitchen/table/fpan/add_onion.txt >> kitchen/table/fpan/frying.txt
cat kitchen/table/fpan/add_carrot.txt >> kitchen/table/fpan/frying.txt
