#!/bin/bash

# set default dir
sourceDir=$(pwd)

# set new source dir if available
if [ $# -ne 0 ]; then
    if [ $1 = "-h" -o $1 = "--help" ]; then
        echo "$0 [-h | --help | DIRECTORY] "
        echo "$0 - srcipt check files in DIRECTORY with same md5 sum"
        echo "      -h | --help - print this help"
        exit 0
    else
        sourceDir=$1
    fi
fi

cd $sourceDir
# create list of files in dir
find . * -maxdepth 0 -type f > fileList

# check if file list created
if [ ! -e fileList ]; then
    echo cant create fileList
    exit 1
fi

#create md5sum filelist
echo "" > md5FileList
#create file with checksum for each file
cat fileList | while read line
do
    md5sum "$line" >> md5FileList
done

sed '/^$/d' md5FileList | sort > Sorted_md5FileList

# compare contents of *.md5sum files
cat Sorted_md5FileList | while read currLine
do
    currMD5=$(echo "$currLine" | cut -d ' ' -f1)
    tmp=$(echo "$currLine" | cut -c1-34 )
    currFile=$(echo ${currLine#${tmp}})
    # echo "currMD5 = $currMD5"
    # echo "currFile = $currFile"
    declare -i sameLine
    sameLine=0

    cat Sorted_md5FileList | while read nextLine
    do
        nextMD5=$(echo "$nextLine" | cut -d ' ' -f1)
        tmp=$(echo "$nextLine" | cut -c1-34 )
        nextFile=$(echo ${nextLine#${tmp}})
        # echo "nextMD5 = $nextMD5"
        # echo "nextFile = $nextFile"
        # ignore the same file
        if [ $sameLine -eq 0 ]; then
            if [ "$currFile" != "$nextFile" ]; then
                # continue searching
                continue
            else
                # finded the same file in fileList
                sameLine=1
                continue
            fi
        fi
    
        if [ $currMD5 = $nextMD5 ]; then
            # echo "$currMD5 = $nextMD5"
            tmp=$(wc -c "$currFile")
            file1Size=$(echo $tmp | cut -d ' ' -f1)
            tmp=$(wc -c "$nextFile")
            file2Size=$(echo $tmp | cut -d ' ' -f1)
            echo "sum eq : $currFile - $file1Size | $nextFile - $file2Size"
        fi
    done
done

# clean up
rm fileList
rm md5FileList
rm Sorted_md5FileList
