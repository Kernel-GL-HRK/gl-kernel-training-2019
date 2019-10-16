#!/bin/bash

print_working_directory="$(pwd)"

function help_MD5()
{
    echo "Script for find duplicte files"
    echo "Search files with same MD5 checksum"
    echo -e "\t-h, --help\tprint this message and exit."
}
