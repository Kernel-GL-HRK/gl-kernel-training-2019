#!/bin/sh

_help=false
_force=false
_check_list_file=$(mktemp)

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

#1-dir 2-file_check_list
creat_list_for_check(){
	echo $1 > $2
	find $1 -name "*" | sed 's/ /\\ /g' >> $2
}

#1-dir/file 2-USER 3-ID 4-GRUP
check_item(){
	if [ -d $1 ]; then
		local FILE_ID_USER=$(get_file_id $1)
		local FILE_GRUP_USER=$(get_file_grup $1)

		if [ $FILE_ID_USER -ne $3 ] || [ $FILE_GRUP_USER -ne $4 ]; then
			if [ "$_force" = true ]; then
				echo chown $3:$4 $1
			else
				if [ $FILE_ID_USER -ne $3 ]; then
					echo "The directory $1 doesn't belong to the user $2 with id $3 !"
				fi
				if [ $FILE_GRUP_USER -ne $4 ]; then
					echo "The directory $1 doesn't belong to the user $2 with grup $4 !"
				fi

				read -p "To fix? [y/n]" _key < /dev/tty

				case "${_key}" in
				    [yY]) echo chown $3:$4 $1 ;echo "Fix!"  ;;
				esac
			fi
		fi
		if [ $? -ne 0 ]; then
			echo "FILE_ID_USER:$FILE_ID_USER FILE_GRUP_USER:$FILE_GRUP_USER"
		fi
	fi
}

##########_MAIN_#########

parse_args $@

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

		if [ -d $HOME_DIR ]; then
			creat_list_for_check $HOME_DIR $_check_list_file

			while read -r check_file;do
				check_item $check_file $USER $ID $GRUP
			done < $_check_list_file
		fi

	fi
done < /etc/passwd

rm $_check_list_file
