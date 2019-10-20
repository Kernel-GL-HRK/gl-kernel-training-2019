#!/bin/bash

SALT=$(cat grocery/products/salt.txt)
PEPPER=$(cat grocery/products/pepper.txt)
BAY_LEAF=$(cat grocery/products/bay_leaf.txt)

echo -e "7.\tДобавьте $SALT и $PEPPER по вкусу."

echo -e "\tТакже положите в кастрюлю тщательно вымытый $BAY_LEAF."
