#!/bin/sh

STR="$(cat ./kitchen/$1 | sed -n '1p')"
echo "Water, cold, $STR add to pot" > "./kitchen/pot/$1"
echo "Waiting for the soup to boil" >> "./kitchen/pot/$1"

