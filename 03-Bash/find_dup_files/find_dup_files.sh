#!/bin/bash

print_working_directory="$(pwd)"

function help_MD5()
{
    echo "Script for find duplicte files"
    echo "Search files with same MD5 checksum"
    echo -e "\t-h, --help\tprint this message and exit."
}
function find_files()
{
echo "Search in $print_working_directory"
find "$print_working_directory" -maxdepth 1 -type f -printf "size:%s\t" -exec md5sum {} \; | sort -k2 | uniq -f1 -w32 -D
}

if [[ $# -gt 1 ]]; then
    echo "Wrong choice"
    help_MD5
elif [[ $# -eq 1 ]]; then
    arg="$1"
    case $arg in
        -h|--help)
            help_MD5
            ;;
        -*)
            echo "Wrong choice"
            help_MD5
            ;;
        *)
            if [[ -d $1 ]]; then
                find_files
            else
                echo "$1 Wrong directory."
            fi
            ;;
    esac
else
    find_files
fi
