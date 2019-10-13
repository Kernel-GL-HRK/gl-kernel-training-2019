! #bin/bash

if [[ $1 = "-h" || $1 = "--help" ]]; then
    echo "Program prints file names and file size for files with same md5 checksum"
    exit 0
else
    if [[ $1 = "" ]]; then
        DIR=$(pwd)
    else     
        DIR=$1
    fi
fi

if [[ -d "$DIR" ]]; then
    find $DIR -maxdepth 1 -type f -exec md5sum {} \; | sort | uniq -D -w 32 | tr -s ' ' | cut -d ' ' -f 2- | \
    while read line; do
        echo "$line" | sed 's/ /\\ /g' | xargs ls -lh | tr -s ' ' | cut -d ' ' -f 5,9-
    done
else
    echo "Fail! This isn't dir"
fi
