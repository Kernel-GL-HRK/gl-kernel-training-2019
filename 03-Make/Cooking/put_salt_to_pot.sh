#!/bin/bash

SALT=$(cat kitchen/table/salt.txt)
echo "Put the $SALT to the pot and mix it" > $1

