! #bin/bash

run=1

if [[ $1 = "-h" || $1 = "--help" ]]; then
	echo -e "Usage:\nfind_dup_files <options>\n\nDuplicate file search."
	echo -e "\nOptions:\n [dir name] - search folder. (default current dir)\n -h, --help  - print this message"
	run=0
elif [[ $1 = "" ]]; then
	cur_dir=$(pwd)
else
	cur_dir=$1
fi

if [[ $run = 1 ]]; then
	echo -e "\n$cur_dir\n"
	find "$cur_dir" -not -type d -printf "size:%s, md5sum:" -exec md5sum {} \; | sort -k2 | uniq -w32 -D 
fi

