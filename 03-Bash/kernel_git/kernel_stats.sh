#!/bin/sh

FWORD="revert"

if [ -n "$1" ];then
	DIR="$1"
else
	DIR="$(pwd)"
fi

echo "Work dir: $DIR"
echo "Finded files .c: $(find $DIR -type f -name '*.c' | wc -l)"
echo "Finded files .cpp: $(find $DIR -type f -name '*.cpp' | wc -l)"
echo "Finded files .py: $(find $DIR -type f -name '*.py' | wc -l)"
echo "Finded commits with word \"$FWORD\": $(git -C $DIR log --oneline | grep -c -w $FWORD)"
