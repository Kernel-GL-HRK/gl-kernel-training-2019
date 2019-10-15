#!/bin/sh

file=/etc/passwd

if [ -n "$1" ]; then
	case "$1" in
		-f | --force)
			FORCE_FLAG="1"
			;;
		-h | --help)
			echo "Usage: fix_owner.sh [OPTION]"
			echo -e "...\n"
			echo "Options (no more than one argument):"
			echo -e "	-h, --help     display this help and exit\n"
			echo -e "       -f, --force    fix without asknowledgement\n"
			exit
		;;
	esac
fi

while read -r line
do
	USER_ID="$(echo $line | cut -d: -f3)"
	GROUP_ID="$(echo $line | cut -d: -f4)"
	HOME_DIR="$(echo $line | cut -d: -f6)"
	if [ "$USER_ID" -ge 1000 ] && [ "$USER_ID" -lt 60000 ]; then
		echo "line = $line"
		echo "USER_ID=$USER_ID"
		echo "GROUP_ID=$GROUP_ID"
		echo "HOME_DIR=$HOME_DIR"
	fi
done < "$file"
