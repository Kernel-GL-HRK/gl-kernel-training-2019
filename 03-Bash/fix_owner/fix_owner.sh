#!/bin/bash 

declare -i force
force=0

function printHelp()
{
    echo "$0 - check user\`s"
    echo "-f, --force - fix without asknowledgement,"
    echo "-h, --help - print these options."
    exit 0
}

# check options
if [ $# -gt 0 ]; then
    case $1 in 
        -h | --help) printHelp;; 
        -f | --force) force=1;;
        *) exit 1;;
    esac
fi


file="/etc/passwd"
IFS=$'\n'
for user_line in $(cat $file)
do
    declare -i user_id
    user_id=$(echo $user_line | cut -d ':' -f3)
    if [ $user_id -lt 1000 ]; then
        continue
    fi
    user_name=$(echo $user_line | cut -d ':' -f1)
    user_home=$(echo $user_line | cut -d ':' -f6)
    user_group=$(echo $user_line | cut -d ':' -f4)

    # echo "$user_name=$user_id:$user_group-$user_home"

    homeFileList=$(mktemp)
    find $user_home -maxdepth 1 > "$homeFileList"

    declare -i once
    once=0
    for homeFile in $(cat $homeFileList)
    do
        if [ $once -eq 0 ]; then 
            once=1
            continue
        fi

        fileOwner=$(stat -c '%U' $homeFile)
        fileGroup=$(stat -c '%g' $homeFile)
        if [ "$fileOwner" != "$user_name" ]; then
            echo "the $homeFile does not belong to you"
            echo "want to appropriate?"
            echo -n "y/n :"
            if [ $force -eq 1 ]
            then
                decision="y"
                echo "forced"
            else
                read decision
            fi
            if [ $decision == "y" ]; then
                echo "try to modify owner"
                echo "sudo chown $user_name $homeFile"
                sudo chown $user_name $homeFile
                if [ $(stat -c '%U' $homeFile) = "$user_name" ];then
                    echo "successfully assigned"
                else
                    echo "failed to assign"
                fi
            fi
        fi
        if [ "$fileGroup" != "$user_group" ]; then
            echo "the $homeFile set with another group"
            echo "want to change group?"
            echo -n "y/n :"
            if [ $force -eq 1 ]
            then
                decision="y"
                echo "forced"
            else
                read decision
            fi
            if [ $decision == "y" ]; then
                echo "try to modify group"
                echo "sudo chgrp $user_name $homeFile"
                sudo chgrp "$user_group" "$homeFile"
                if [ $(stat -c '%g' $homeFile) = "$user_group" ];then
                    echo "successfully assigned"
                else
                    echo "failed to assign"
                fi
            fi
        fi

    done

done