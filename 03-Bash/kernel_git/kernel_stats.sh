#!/bin/sh

_DIR=""

help(){
cat <<EOF
For use: $(basename $0) [DIR]
-h, --help - print program description and these options.
EOF
}

parse_args(){
	case "$1" in
		-h | --help ) _help=true; break ;;
		-- ) shift; break ;;
		-* | --*= ) echo "Error!" ; help; break ;;
		* ) _DIR=$1; break ;;
	esac
}

count_files_in_dir(){
	echo "count *.c :\t $(find "$1" -type f -name "*.c" | wc -l)"
	echo "count *.cpp :\t $(find "$1" -type f -name "*.cpp" | wc -l)"
	echo "count *.py :\t $(find "$1" -type f -name "*.py" | wc -l)"
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

count_files_in_dir $_DIR

