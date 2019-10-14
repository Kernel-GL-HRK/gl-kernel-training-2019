#!/bin/sh

DIR="$(pwd)"

if [ -n "$1" ]; then
	case "$1" in
		-h | --help)
			echo "Usage: find_dup_files.sh [OPTION] or [DIR]"
			echo -e "List information about the FILEs with identical md5 (the current directory by default).\n"
			echo "Options (no more than one argument):"
			echo -e "	-h, --help     display this help and exit\n"
			exit
		;;
		*) DIR="$1"
		;;
	esac
fi

echo "Search in $DIR"
find "$DIR" -maxdepth 1 -type f -printf "size:%s\t" -exec md5sum {} \; | sort -k2 | uniq -f1 -w32 -D
