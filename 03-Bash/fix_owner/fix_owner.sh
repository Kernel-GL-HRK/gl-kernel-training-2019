! #bin/bash

STATUS_FORCE=0

function parsing_got_arguments  {
	case $1 in
		"-f"| "--force")
			STATUS_FORCE=1
		;;
		"-h"| "--help")
			echo "-f, --force	force fixing for owner and group"
			echo "-h, --help	help information"
			exit 0
		    ;;
		"")
			STATUS_FORCE=0
		;;
		*)
			echo "Key $1 is mistake, please use $0 --help for more information"
			exit 0
		;;
	esac
}

function get_and_check_all_user_in_system  {
	ID_OF_ALL_USERS=$(cut -d : -f 3  /etc/passwd)
	BASE_USER_ID=$(id -u)
	    for current_user_id in $ID_OF_ALL_USERS; do
		if [[ $current_user_id -ge 1000 ]]; then
			NAME_OF_USER=$(id -nu "$current_user_id")
			GROUP_OF_USER=$(id -g "$current_user_id")
			USER_DIR=$( getent passwd "$NAME_OF_USER" | cut -d: -f 6)
#			echo $NAME_OF_USER
#			echo $GROUP_OF_USER
#			echo $USER_DIR
			find $USER_DIR -maxdepth 1 -not -user $current_user_id 2>/dev/null | while read -r line; do
				if [[ $STATUS_FORCE -eq 1 ]]; then
					chown $current_user_id $line 
				else
					read -p "Do you want to change owner, current owner id=$current_user_id,  for $line [y/n] > " ANSWER </dev/tty
					if [[ $ANSWER = "y"  ]]
					then
						chown $current_user_id $line			
					fi					

				fi
			done
			find $USER_DIR -maxdepth 1 -not -group $GROUP_OF_USER 2>/dev/null | while read -r line; do
			        if [[ $STATUS_FORCE -eq 1 ]]; then
			            chgrp  $current_user_id $line
			        else
					read -p "Do you want to change grup, current owner id=$current_user_id, $current_user_id for $line [y/n] > " ANSWER </dev/tty
					if [[ $ANSWER = "y"  ]]
					then
						chgrp $current_user_id $line			
					fi
        			fi
    			done
		fi
	    done
}

parsing_got_arguments $1
get_and_check_all_user_in_system
