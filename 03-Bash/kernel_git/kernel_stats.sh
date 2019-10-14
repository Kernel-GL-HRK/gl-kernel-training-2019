#!/bin/sh

_DIR=""
_file_stat=$(mktemp)

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

count_comits(){
	local tmp_dir="$(pwd)"
	cd $1
	echo "count word \"revert\" in commits:\t $(git log --oneline | grep revert | wc -l)"
	cd $tmp_dir
}

save_statistic(){
	local tmp_dir="$(pwd)"
	local _py_files=$(mktemp)
	local _py_authors=$(mktemp)
	local _tmp_file=$(mktemp)

	cd $1
	echo $_py_files
	find . -type f -name "*.py"  > $_py_files
	echo -n > $2

	while read -r _line;do
		echo "file: $_line"
		git blame -e "$_line" > $_tmp_file
		sync
		cat $_tmp_file | grep -i -o '[A-Z0-9._%+-]\+@[A-Z0-9.-]\+\.[A-Z]\{2,4\}' | sort | uniq > $_py_authors
		while read -r _author;do
			info="$_author\t$(cat $_tmp_file | grep "$_author" | wc -l)\t$_line"
			echo "$info" >> $2
			echo "$info"
		done < $_py_authors
	done < $_py_files
	rm $_py_files $_tmp_file $_py_authors
	cd $tmp_dir
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
count_comits $_DIR
save_statistic $_DIR $_file_stat

rm $_file_stat

