#!/bin/bash

if ! [ -f  "$2"  ]; then
    echo "Take "$1" liters of water and salt water. After that put the pot on fire" > $2
fi
