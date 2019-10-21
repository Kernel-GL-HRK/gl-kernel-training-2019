! #bin/bash

RUN=0
FORCE=0

if [[ $1 = "-h" || $1 = "--help" ]]; then	
	echo -e "Usage:\nfix_owner <options>\n"
    echo -e "For every user in system with id >= 1000 check home folder,"
    echo -e "fixes UID and GID of items in the home directory."
	echo -e "\nOptions:"
    echo -e "-f, --force  -  fix UID and GID without user confirmation"
    echo -e "-h, --help  - print this message"
	RUN=0
elif [[ $1 = "-f" || $1 = "--force" ]]; then
	echo "FORCE ON"
	RUN=1
	prm1="-exec"
elif [[ $1 = "" ]]; then
	echo "FORCE OFF"
	RUN=1
	prm1="-ok"
else 
	echo "Incorrect key. Use -h or --help for help"
fi

USER_ID=$(cut -d : -f 3  /etc/passwd)

if [[ $RUN = 1 ]]; then
for item in $USER_ID; do
	if [ $item -ge 1000 -a $item -le 60000 ]; then 
		USER_NAME=$(id -nu "$item")
		USER_GROUP=$(id -g "$item")
		HOME_DIR=$( getent passwd "$USER_NAME" | cut -d: -f 6 )

		echo $USER_NAME " - " $item " : " $USER_GROUP " -- " $HOME_DIR

		echo "Find by user"
		find $HOME_DIR -not -user $USER_NAME -ls $prm1 chown $USER_NAME {} \;
		echo "Find by group"
		find $HOME_DIR -not -group $USER_GROUP -ls $prm1 chgrp $USER_NAME {} \;
	fi
done
fi
