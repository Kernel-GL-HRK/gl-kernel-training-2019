#!/bin/bash

FORCE=0

function print_help()
{
    echo "Usage: fix_owner <OPTION>"
    echo "For each user in a system with a UID greater than or equal"
    echo "to 1000 and lower than or equal to 60000, checks and fixes"
    echo "UID and GID of items\nin the home directory (recursively)."
    echo ""
    echo "Option:"
    echo -e "\t-f, --force\tfix UID and GID without user confirmation"
    echo -e "\t-h, --help\tprint this message and exit"
}

# $1 -- "UID" or "GID"
# $2 -- file name
function need_fix_prompt()
{
    ANS_ACQUIRED=0
    while [ $ANS_ACQUIRED -eq 0 ]; do
        read -p "Do you want to change $1 for $2 [y/n]? " choice </dev/tty
        case $choice in
            [Yy]*)
                echo 1
                ANS_ACQUIRED=1
                ;;
            [Nn]*)
                echo 0
                ANS_ACQUIRED=1
                ;;
        esac
    done
}

function fix_owner()
{    
    if [ "$EUID" -ne 0 ]; then
        echo "You have no permission."
        exit
    fi
    
    CHOWN=0
    CHGRP=0
    
    awk -F : '{print $3, $4, $6}' /etc/passwd | while read -r uid gid home; do
        if [ $uid -ge 1000 -a $uid -le 60000 ]; then
            find $home -exec stat -c"%u %g %n" {} \; | \
                while read -r fuid fgid fname; do
                    if [ $fuid -ne $uid ]; then
                        if [ $FORCE -eq 0 ]; then
                            CHOWN=$(need_fix_prompt "UID" $fname)
                        else
                            CHOWN=1
                        fi
                        if [ $CHOWN -eq 1 ]; then
                            chown $uid $fname
                        fi
                    fi
                    if [ $fgid -ne $gid ]; then
                        if [ $FORCE -eq 0 ]; then
                            CHGRP=$(need_fix_prompt "GID" $fname)
                        else
                            CHGRP=1
                        fi
                        if [ $CHGRP -eq 1 ]; then
                            chgrp $uid $fname
                        fi
                    fi
                done
        fi
    done
}

if [ $# -gt 1 ]; then
    echo "Incorrect usage!\n"
    print_help
elif [ $# -eq 0 ]; then
    fix_owner
else
    case $1 in
        -h|--help)
            print_help
            ;;
        -f|--force)
            FORCE=1
            fix_owner
            ;;
        *)
            echo "Incorrect usage!\n"
            print_help
            ;;
    esac
fi
