#!/bin/bash


current_dir="$(pwd)"

function view_help {
	echo "Script find_dup_files.sh usage information."
	echo "SYNOPSIS"
	echo "find_dup_files.sh [DIR] [-h]"
	echo -e "Display information about files with identical md5, if the directory is not specified, then the current directory is by default.\n"
	echo "Options:"
	echo -e "\t -h, --help     display this help and exit\n"
	exit 0
}


function find_similar_files {

	echo "Found in the $current_dir"
	find "$current_dir" -maxdepth 1  -printf "size:%s\t"  -type f  -exec md5sum {} \; | sort -k2 | uniq -w32 -f1  -D

}

function wrong_option {

	echo "Wrong option"
}

if [[ -n "$1" ]];then
	case "$1" in
		--help | -h) view_help 
		;;
		*) 
		wrong_option
		;;
	esac

fi

find_similar_files $current_dir


