#!/bin/bash

if  [ -f  "$1"  ]; then
    echo "Cut vegetables and meat" >> $1
    echo "Put meat into the pot and  boil bouillon about 1 hour" >> $1
    echo "Put vegetables into the pot and cook until cooked" >> $1
fi
