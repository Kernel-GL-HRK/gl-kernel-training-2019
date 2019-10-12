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

parse_args $@
