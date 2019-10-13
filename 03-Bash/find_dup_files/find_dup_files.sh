#!/bin/sh

_DIR=""
_list_file="list.txt"

help(){
cat <<EOF
For use: $(basename $0) [DIR]
-h, --help - print program description and these options.
EOF
}

parse_args(){
	while true; do
		case "$1" in
			-h | --help ) _help=true; break ;;
			-- ) shift; break ;;
			-* | --*= ) echo "Error!" ; help; break ;;
			* ) _DIR=$1; break ;;
		esac
	done
}

#1-dir 2-file_check_list
creat_list_for_check(){
	echo -n "" > $2
	find $1 -type f -name "*" | sed 's/ /\\ /g' >> $2
}

get_md4sum(){
	md5sum "$1" | cut --delimiter=" " -f 1
}

##########_MAIN_#########

parse_args $@

if [ "$_help" = true ]; then
	help
	exit 0
fi

if [ ! -d "$_DIR" ] || [ -z "$_DIR" ]; then
	_DIR=$(pwd)
fi

creat_list_for_check $_DIR $_list_file

