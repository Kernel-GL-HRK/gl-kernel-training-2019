#!/bin/sh

_help=false
_force=false

help(){
cat <<EOF
For use:
-f, --force - fix without asknowledgement,
-h, --help - print these options.
EOF
}

parse_args(){
	while true; do
		case "$1" in
			-h | --help ) _help=true; shift ;;
			-f | --force ) _force=true; shift ;;
			-- ) shift; break ;;
			-* | --*= ) echo "Error!" ; help; break ;;
			* ) break ;;
		esac
	done
}

get_user(){
	echo $1 | cut -d\: -f 1
}

get_id(){
	echo $1 | cut -d\: -f 3
}

get_grup(){
	echo $1 | cut -d\: -f 4
}

get_home_dir(){
	echo $1 | cut -d\: -f 6
}

user_validation(){
	if [  $1 -eq 1000 ] || [ $1 -gt 1000 ];then
		return 0
	fi
	return 1
 }

 get_file_id(){
	echo $(ls -nd "$1" | cut --delimiter=" " -f 3)
}

get_file_grup(){
	echo $(ls -nd "$1" | cut --delimiter=" " -f 4)
}

##########_MAIN_#########

parse_args $@

echo "Use help is $_help"
echo "Use forse is $_forse"

if [ "$_help" = true ]; then
	help
	exit 0
fi

while read -r line;do
	USER=$(get_user $line)
	ID=$(get_id $line)
	GRUP=$(get_grup $line)
	HOME_DIR=$(get_home_dir $line)

	user_validation $ID
	if [  $? -eq 0 ]; then
		FILE_ID_USER=$(get_file_id $HOME_DIR)

		if [ $FILE_ID_USER -ne $ID ]; then
			echo "The directory $HOME_DIR doesn't belong to the user $USER with id $ID !"
		fi

	fi
done < /etc/passwd
