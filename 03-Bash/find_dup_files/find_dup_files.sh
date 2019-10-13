#!/bin/sh

_DIR=""
_list_file="list.txt"
_list_all_file="list_all.txt"
_list_md5sum_file="md5sum.txt"

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

#1-input_list_file 2-md5sum_file 3-all_info
creat_list_md5sum(){
	echo -n > $3
	echo -n > $2
	while read -r _line;do
		_md5sum=$(get_md4sum "$_line" | tr -d '\n')
		echo -n $_md5sum >> $3
		echo $_md5sum >> $2

		echo -n "\t$(du -b "$_line" | cut -f1)" >> $3
		echo "\t\"$_line\"" >> $3
	done < $1
}

#1-md5sum_file 2-all_info
compare_and_show_results(){
	local old_md5sum=""
	while read -r _line;do
		if [ "$old_md5sum" = "$_line" ]; then
			echo "$(cat $2 | grep $_line)"
		fi
		old_md5sum=$_line
	done < $1
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
creat_list_md5sum $_list_file $_list_md5sum_file $_list_all_file
sort $_list_md5sum_file -o $_list_md5sum_file
compare_and_show_results $_list_md5sum_file $_list_all_file

