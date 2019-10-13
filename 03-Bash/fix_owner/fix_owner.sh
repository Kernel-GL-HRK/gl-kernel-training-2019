! #bin/bash

#Function is used to chnage owner of file/dir/link
#$1 - home directory, $2 - id of user, $3 - force_flag 

function change_owner  {
    local FILES=$(find $1 -maxdepth 1 -not -user $2 2>/dev/null)
    local ANSWER="n"
    for item in $FILES; do
        if [[ $3 -eq 1 ]]; then
            chown  $2 "$item"
        else
            read -p "Do you want to change the owner for the $item [y/N]: " ANSWER
            if [[ $ANSWER = [yY] ]]; then
                chown  $2 "$item"
            fi    
        fi
    done
}

#Function is used to chnage group of file/dir/link
#$1 - home directory, $2 - id of group, $3 - force_flag 

function change_group  {
    local FILES=$(find $1 -maxdepth 1 -not -group $2 2>/dev/null)
    local ANSWER="n"
    for item in $FILES; do
        if [[ $3 -eq 1 ]]; then
            chgrp  $2 "$item"
        else
            read -p "Do you want to change the group for the $item [y/N]: " ANSWER
            if [[ $ANSWER = [yY] ]]; then
                chgrp  $2 "$item"
            fi    
        fi
    done
}

if [[ $1 != "-h" && $1 != "--help" ]]; then
	if [[ $1 != "-f" && $1 != "--force" ]]; then
		if [[ $1 != "" ]]; then
			echo "Wrong key $1, use --help key"
			exit 0
		fi
	fi
fi

if [[ $1 = "-h" || $1 = "--help" ]]; then
    HELP_FLAG=1
    echo "-f, --force		force fixing for owner and group"
    echo "-h, --help		help information"
else
    if [[ $1 = "-f" || $1 = "--force" ]]; then
        FORCE_FLAG=1	
    else     
        FORCE_FLAG=0
    fi
HELP_FLAG=0
fi

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

