#!/bin/bash

# For every user in system with id >= 1000 check home folder.

local_users=($( awk -F: '($3 >= 1000) {printf "%s\n",$1}' /etc/passwd))

for local_user in "${local_users[@]}";
do
  echo $local_user
  user_path=$(grep $local_user /etc/passwd|cut -f6 -d":")
  if [  -d "$user_path"  ];
    then
      echo "$user_path direcroty is present"
    else
      echo "$user_path direcroty is present"
  fi
done

echo
echo name of executed script:
echo $0
