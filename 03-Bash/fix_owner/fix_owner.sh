#!/bin/sh

if [ -n "$1" ]; then
	case "$1" in
		-f | --force)
			FORCE_FLAG="1"
			;;
		-h | --help)
		echo "Usage: fix_owner.sh [OPTION]"
		echo "Search for files / folders with ID and GROUP_ID in"
		echo "the user's home directory that do not match the"
		echo "FILE_ID and FILE_GROUP_ID of this user."
		echo "Options (no more than one argument):"
		echo -e "\t-h, --help     display this help and exit"
		echo -e "\t-f, --force    fix without asknowledgement\n"
		exit
		;;
		*)
		echo "Parameter is fault"
		exit
		;;
	esac
fi
# chown
# chgrp
echo "Search files and dirs..."
while read -r line
do
	USER_ID="$(echo $line | cut -d: -f3)"
	GROUP_ID="$(echo $line | cut -d: -f4)"
	HOME_DIR="$(echo $line | cut -d: -f6)"
	if [ "$USER_ID" -ge 1000 ] && [ "$USER_ID" -lt 60000 ]; then
		#echo "line = $line"
		#echo "USER_ID=$USER_ID"
		#echo "GROUP_ID=$GROUP_ID"
		#echo "HOME_DIR=$HOME_DIR"
		find "$HOME_DIR" -maxdepth 1 -type f,d -printf '%p %U %G\n' | while read -r fid
		do
			echo "$fid"
			FILE_NAME="$(echo $fid | cut -d ' ' -f1)"
			FILE_UID="$(echo $fid | cut -d ' ' -f2)"
			FILE_GID="$(echo $fid | cut -d ' ' -f3)"
			if [[ "$FILE_UID" -gt 1000 && "$FILE_UID" -ne "$USER_ID" ]] || [[ "$FILE_GID" -gt 1000 && "$FILE_GID" -ne "$GROUP_ID" ]]; then
				if [ -n "$FORCE_FLAG" ]; then
					response="y"
				else
					read -p "-> Change [y/N] ?"  response </dev/tty
				fi
				case "$response" in
				y)
					if [[ "$FILE_UID" -gt 1000 && "$FILE_UID" -ne "$USER_ID" ]]; then
						chown "$USER_ID" "$FILE_NAME" 2>/dev/null
						if [ "$?" -ne 0 ]; then
					                echo "Operation not permitted"
					                exit 1
						fi
						echo -e "\t-> Chenging: $FILE_NAME $FILE_UID -> USER ID fix to $USER_ID"
					fi
					if [[ "$FILE_GID" -gt 1000 && "$FILE_GID" -ne "$GROUP_ID" ]]; then
                                                chgrp "$GROUP_ID" "$FILE_NAME" 2>/dev/null
                                                if [ "$?" -ne 0 ]; then
                                                        echo "Operation not permitted"
                                                        exit 1
                                                fi
                                                echo -e "\t-> Chenging: $FILE_NAME $FILE_GID -> GROUP ID fix to $GROUP_ID"
                                        fi

					;;
				esac
			fi
		done
	fi
done < "/etc/passwd"
