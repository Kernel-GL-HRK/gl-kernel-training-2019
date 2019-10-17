! #bin/bash

#Function is used to change owner of file/dir/link
#$1 - home directory, $2 - id of user, $3 - force_flag 

function change_owner  {
    find $1 -maxdepth 1 -not -user $2 2>/dev/null | while read -r line; do
        if [[ $3 -eq 1 ]]; then
            chown  $2 "$item"
        else
            echo "Owner of $item in the home directory $1 isn't equal to $2 and must be changed" 
        fi
    done
}

#Function is used to change group of file/dir/link
#$1 - home directory, $2 - id of group, $3 - force_flag 

function change_group  {
    find $1 -maxdepth 1 -not -group $2 2>/dev/null | while read -r line; do
        if [[ $3 -eq 1 ]]; then
            chgrp  $2 "$item"
        else
            echo "Group of $item in the home directory $1 isn't equal to $2 and must be changed" 
        fi
    done
}

HELP_FLAG=0
case $1 in
    "-h" | "--help")
        HELP_FLAG=1
        echo "-f, --force		force fixing for owner and group"
        echo "-h, --help		help information"
    ;;
    "-f" | "--force")
        FORCE_FLAG=1
    ;;
    "")
        FORCE_FLAG=0
        ;;
    *)
        echo "Wrong key $1. Use $0 --help"
        exit 0
    ;;
esac

USER_ID=$(cut -d : -f 3  /etc/passwd)

if [[ $HELP_FLAG -eq 0 ]]; then
    for item in $USER_ID; do
        if [[ $item -ge 1000 ]]; then
            USER_NAME=$(id -nu "$item")
            USER_GROUP=$(id -g "$item")
            HOME_DIR=$( getent passwd "$USER_NAME" | cut -d: -f 6 )
       	    change_owner $HOME_DIR $item $FORCE_FLAG
            change_group $HOME_DIR $USER_GROUP $FORCE_FLAG
        fi
    done
fi
