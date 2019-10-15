#!/bin/sh

_DIR=""
_file_stat=$(mktemp)
_file_count_stat=$(mktemp)

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

#1(in)-file with statistic, 2(out)-file with count all lines of the author in all files
count_all_statistic(){
	local count_line=""
	local old_line=""
	echo -n > $2
	while read column1 column2 column3 ; do

		if [ "$old_line" = "$column1" ]; then
			count_line=$(expr $count_line + ${column2})
		else
			echo "$old_line $count_line" >> $2
			echo "$old_line $count_line"
			count_line=$(expr $column2)
		fi

		old_line=$column1
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

count_files_in_dir $_DIR
count_comits $_DIR
save_statistic $_DIR $_file_stat

sort $_file_stat -o $_file_stat

echo ""
echo "Statistics for all files."

count_all_statistic $_file_stat $_file_count_stat
rm $_file_stat $_file_count_stat

