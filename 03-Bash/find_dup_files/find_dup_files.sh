#!/bin/bash

function print_help()
{
    echo "Usage: find_dup_files [OPTION] [PATH]"
    echo "Searches for files with MD5 checksum (128-bit) collisions in the"
    echo "specified with PATH directory (by default in current directory)."
    echo ""
    echo "Option:"
    echo -e "\t-h, --help\tprint this message and exit."
}

function find_dup_files()
{
    SEARCH_PATH=$1
    PREV_FILE_PATH=""
    PREV_CHECKSUM=""

    if [ "${SEARCH_PATH:0:1}" != "/" ]; then
        cd $SEARCH_PATH
        SEARCH_PATH=$(pwd)
        cd - &>/dev/null
    fi

    find $SEARCH_PATH -type f -exec md5sum {} \; | sort | \
        while read -r checksum file_path; do
            if [ "$PREV_CHECKSUM" = "" ]; then
                PREV_CHECKSUM=$checksum
                PREV_FILE_PATH=$file_path
                continue
            fi
            if [ "$PREV_CHECKSUM" = "$checksum" ]; then
                echo "MD5 checksum of file:"
                echo ">>> $file_path"
                echo "with size of $(stat -c%s "$file_path") bytes"
                echo "is equal to MD5 checksum of file:"
                echo ">>> $PREV_FILE_PATH"
                echo "wiht size of $(stat -c%s "$PREV_FILE_PATH") bytes."
                echo ""
            else
                PREV_CHECKSUM=$checksum
                PREV_FILE_PATH=$file_path
            fi
        done
}

if [ $# -gt 1 ]; then
    echo "Incorrect usage!"
    print_help
elif [ $# -eq 1 ]; then
    arg="$1"
    case $arg in
        -h|--help)
            print_help
            ;;
        -*)
            echo "Incorrect usage!"
            print_help
            ;;
        *)
            if [ -d $1 ]; then
                find_dup_files $@
            else
                echo "$1 is not a directory."
            fi
            ;;
    esac
else
    find_dup_files $(pwd)
fi
