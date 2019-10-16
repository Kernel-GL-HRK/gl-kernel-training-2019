#!/bin/bash 

# temp varieble (int) for fixforce
declare -i fix_force=0;
# user_file for all script
user_file="/etc/passwd"
# temp file for find utility
temp_file=$(mktemp)          

#crate function Help
function Help()
{
    echo "$0  - check user"
    echo "-f, - fix without asknowledgement,"
    echo "-h, - print help options."
    exit 0
}
