#!/bin/sh

DIR="$(pwd)"
FILE="file.txt"

if [ -n "$1" ]; then
	DIR="$1"
fi

echo "Search in $DIR"
find "$DIR" -maxdepth 1 -type f -printf "size:%s\t" -exec md5sum {} \; > "$FILE"
sort -k2 "$FILE" | uniq -f1 -w32 -D

