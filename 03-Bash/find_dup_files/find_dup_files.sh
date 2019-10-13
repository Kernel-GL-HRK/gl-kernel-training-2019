#!/bin/sh

_DIR=""

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

##########_MAIN_#########

parse_args $@

if [ "$_help" = true ]; then
	help
	exit 0
fi

echo "DIR: $_DIR";

if [ ! -d $_DIR ] || [ -z $_DIR ]; then
	_DIR=$(pwd)
fi

echo "DIR: $_DIR";

