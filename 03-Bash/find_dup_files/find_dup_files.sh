#!/bin/bash

getList(){
	find . -type f -exec md5sum {} \; | sort | uniq -D -w32
}

showHelp(){
	echo "Print file names and file size for files with same md5 checksum."
	echo "-h | -help - show help"
}

getDupFiles(){
	getList | while read line 
	do
		file=$(echo $line | tr -s ' ' | cut -d ' ' -f 2)
		size=$(stat -c "%s" $file)
		echo "$file $size"
	done
}

if [[ $# -eq 1 ]]
then
	if [[ $1 = "-h" || $1 = "--help" ]]
	then
		showHelp
	else
	cd $1
		getDupFiles
	fi
else
	getDupFiles
fi
