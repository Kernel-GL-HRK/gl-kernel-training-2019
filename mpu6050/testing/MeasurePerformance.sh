#!/bin/sh

if [[ $1 = "-h" || $1 = "--help" ]]; then
	echo "-h, --help     display this help and exit"
	echo "[time] 	     testing time ms (default 1(s))"
	exit 0
fi

TIMEOUT=${1:-1000}

LIST_FILES="list.file.txt"
echo "Start of measurement"
echo "File:"  "Number"

while IFS= read -r file
do
	COUNT=0
	let "END=($(date +%s%3N) + $TIMEOUT)"
	while [ $END -gt $(date +%s%3N) ]
	do
		cat $file > dev/null
		COUNT=$((COUNT+1))
	done

echo "$file" "$COUNT"
done < "$LIST_FILES"

echo "End of measurement"
exit 0
