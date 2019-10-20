#!/bin/bash

cat kitchen/table/receipt.txt > kitchen/table/receipt_ready.txt
cat kitchen/table/pot/add_water.txt >> kitchen/table/receipt_ready.txt
cat kitchen/table/pot/add_chicken.txt >> kitchen/table/receipt_ready.txt
cat kitchen/table/pot/add_salt.txt >> kitchen/table/receipt_ready.txt
cat kitchen/table/fpan/frying.txt >> kitchen/table/receipt_ready.txt
cat kitchen/table/pot/add_potato.txt >> kitchen/table/receipt_ready.txt
cat kitchen/table/pot/add_greenpeas.txt >> kitchen/table/receipt_ready.txt
cat kitchen/table/pot/add_frying.txt >> kitchen/table/receipt_ready.txt

echo "sup is ready" >> kitchen/table/receipt_ready.txt
