#!/bin/sh

DIR="$(pwd)"
FWORD="revert"

if [ -n "$1" ]; then
	case "$1" in
		-h | --help)
			echo "Usage: kernel_stats.sh [OPTION] or [DIR]"
			echo -e "Search for the count of files like .c, cpp and .py. Search git commits name with word \"revert\".\nSearch for authors in git on all .py files. (the current directory by default).\n"
			echo "Options (no more than one argument):"
			echo -e "	-h, --help     display this help and exit\n"
			exit
		;;
		*) DIR="$1"
		;;
	esac
fi

echo "Work dir: $DIR"
echo "Search files (*.c, *.cpp and .py)..."
echo "Finded files .c: $(find $DIR -type f -name '*.c' | wc -l)"
echo "Finded files .cpp: $(find $DIR -type f -name '*.cpp' | wc -l)"
echo "Finded files .py: $(find $DIR -type f -name '*.py' | wc -l)"
if [ -d "$DIR/.git/" ]; then
	echo "Search commits name with word \"$FWORD\"..."
	echo "Finded commits: $(git -C $DIR log --oneline | grep -c -w $FWORD)"
	echo "Search the count of authors in .py files..."
	find "$DIR" -type f -name '*.py' -exec git -C "$DIR" blame HEAD --porcelain {} \; | grep "^author " | sort -k2 | uniq -f2 -c
else
	echo "No .git"
fi
