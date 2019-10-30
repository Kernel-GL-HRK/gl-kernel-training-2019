#!/bin/bash


current_dir="$(pwd)"
PARAMS=""
function view_help {
	echo "Script find_dup_files.sh usage information."
	echo "SYNOPSIS"
	echo "find_dup_files.sh [DIR] [-h]"
	echo "Display information about files with identical md5,"
	echo "if the directory is not specified,"
	echo -e "then the current\n directory is by default.\n"
	echo "Options:"
	echo -e "\t -h, --help     display this help and exit\n"
}


function find_similar_files {
	echo "Found in the directory $1"
	find "$1" -maxdepth 1  -type f -printf "size:%s\t" \
	 -exec md5sum {} \; | sort -k2 | uniq -w32 -f1  -D
}

function wrong_option {
	echo "Wrong option"
}

while (( "$#" )); do
  case "$1" in
    --help | -h)
      view_help
      exit 0
      ;;
    --)
      shift 1
#      break
      ;;
    -*|--*=)
      echo "Error: Unsupported flag $1" >&2
      exit 1
      ;;
    *)
      current_dir=$1
      echo "current_dir=$current_dir"
      break
      ;;
  esac
done



find_similar_files $current_dir
