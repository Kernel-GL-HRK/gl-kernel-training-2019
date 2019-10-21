#!/bin/bash

PRODUCTS=grocery/products

RIBS=$PRODUCTS/smoked_pork_ribs.txt
PEA=$PRODUCTS/pea.txt
POTATO=$PRODUCTS/potato.txt
ONION=$PRODUCTS/onion.txt
CARROT=$PRODUCTS/carrot.txt
OIL=$PRODUCTS/oil.txt
SALT=$PRODUCTS/salt.txt
PEPPER=$PRODUCTS/pepper.txt
BAY_LEAF=$PRODUCTS/bay_leaf.txt

echo "Список продуктов:"
echo -e "\t$(cat $RIBS) (500 гр);"
echo -e "\t$(cat $PEA) (500 гр);"
echo -e "\t$(cat $POTATO) (4 шт);"
echo -e "\t$(cat $ONION) (2 шт);"
echo -e "\t$(cat $CARROT) (1 шт);"
echo -e "\t$(cat $OIL) (для пассеровки лука и моркови)"
echo -e "\t$(cat $SALT) (по вкусу);"
echo -e "\t$(cat $PEPPER) (по вкусу);"
echo -e "\t$(cat $BAY_LEAF) (2 шт)."
echo ""
