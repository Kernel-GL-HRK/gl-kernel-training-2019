 #!/bin/sh
 
 
 
if [[ $1 = "-h" || $1 = "--help" ]]; then
    echo "$0 for all users 1000 <= id < 6000 , checks and fixes UID in the home directory"
    echo "Usage: $0 [OPTION]"
    echo "-f, --force - fix without acknowledgement"
    echo "-h, --help     display this help and exit"
    exit 0
fi
 
 LIST_ID_USERS=$(cut -d : -f 3  /etc/passwd)

# echo "$user_id"
 
 for USER_ID in $LIST_ID_USERS; do
    if [[ ($USER_ID -ge 1000) && ($USER_ID -le 60000)]]; then
        name_user=$(id -nu "$USER_ID")
        echo "name - $name_user"
        name_group=$(id -g "$USER_ID")
        echo "group - $name_group"
        
        home_dir=$(grep "$USER_ID" /etc/passwd | cut -d: -f 6)
        #echo "$home_dir"
        
        LIST_FILES=$(find $home_dir -maxdepth 1 -not -user $name_user)
            #echo "$LIST_FILES"
            for FILE in $LIST_FILES; do
            if [[ ($1 = "-f") || ($1 = "--force")]]; then
                    chown $name_user  "$LIST_FILES"
                else
                    read -p "Change the user ownership for the $FILE [Y/N]: " ANSWER
                    if [[ $ANSWER = [Y] ]]; then
                        chown $name_user  "$FILE"
                    fi
            fi
        done
        
        LIST_FILE=$(find $home_dir -maxdepth 1 -not -group $name_group)
           # echo "$LIST_FILE"
            for GROUP in $LIST_FILE; do
            if [[ ($1 = "-f") || ($1 = "--force")]]; then
                    chgrp $name_group  "$GROUP"
                else
                    read -p "Change the group ownership for the $ [Y/N]: " ANSWER
                    if [[ $ANSWER = [Y] ]]; then
                        chgrp $name_group  "$GROUP"
                    fi
            fi
        done
        
    fi
    
 done
 
 
 exit 0
