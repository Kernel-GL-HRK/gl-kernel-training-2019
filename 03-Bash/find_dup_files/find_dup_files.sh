! #bin/bash

if [[ $1 = "-h" || $1 = "--help" ]]; then
    HELP_FLAG=1
    echo "Program prints file names and file size for files with same md5 checksum"
    exit 0
else
    if [[ $1 = "" ]]; then
        DIR=$(pwd)
    else     
        DIR=$1
    fi
HELP_FLAG=0
fi

if [[ -d "$DIR" ]]; then
    touch -c temp_hash_file.md5
    :> temp_hash_file.md5
    LIST_FILES=$(find $DIR -maxdepth 1 -type f)
        for item in $LIST_FILES; do
            md5sum "$item" >> temp_hash_file.md5
        done
	UNIQ_FILES=$(uniq -D -w 32 temp_hash_file.md5 | sed 's/\s\+/:/' | cut -d : -f 2)
        for item in $UNIQ_FILES; do
            NAME=$(ls -s $item | sed 's/\s\+/,/' | cut -d , -f 2)
            SIZE=$(ls -s $item | sed 's/\s\+/,/' | cut -d , -f 1)
            echo "$NAME $SIZE"
        done
    rm -f temp_hash_file.md5
else
    echo "Fail! This isn't dir"
fi

