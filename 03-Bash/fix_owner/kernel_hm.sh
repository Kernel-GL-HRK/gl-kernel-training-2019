#!/bin/bash

display_help() {
  echo
  echo "Usage: $0 [target directory]"
  echo
  echo "    -f, --force - fix without asknowledgement"
  echo "    -h, --help - print these options"
  echo
  echo
}

ask_user_yes_no() {

  read -p "Would you like to change owner? y/n " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]
  then
    return 0
  fi
  return 1;
}

if [ "$1" = "--help" ] || [ "$1" = "--h" ]; then
  display_help
  exit 1
fi

# For every user in system with id >= 1000 check home folder.
# In case of inequality of user's id and file/dir/link owner's id
local_users=($( awk -F: '($3 >= 1000) {printf "%s\n",$1}' /etc/passwd))

for local_user in $(awk -F: '($3 >= 1000) {printf "%s\n",$1}' /etc/passwd);
do
  echo $local_user
  user_path=$(grep $local_user /etc/passwd|cut -f6 -d":")

  if [ -d "$user_path" ]; then
    echo "$user_path direcroty is present"

    USERID=$(grep $local_user /etc/passwd|cut -f3 -d":")
    echo Userid is $USERID

    for cur_file in $(find "${user_path}" -name '*.*');
    do
      # get user and group id of owner
      OWENER_USERID=$(stat -c '%u' $cur_file)
      OWENER_GROUPID=$(stat -c '%g' $cur_file)

      if [ -z "$OWENER_USERID" ] || [ -z "$OWENER_GROUPID" ] || [ $USERID != $OWENER_USERID ] || [ $USERID != $OWENER_GROUPID ]; then
        echo Mismatch between User/Group ID and real Owner User ID
        echo Real Owner Userid is $OWENER_USERID
        echo Real Owner UserGroupid is $OWENER_GROUPID
        if [ "$1" = "-f" ] || [ "$1" = "--force" ] || ask_user_yes_no; then
          chown -R $USERID:$USERID $cur_file
          echo permission \for $cur_file \file  changed
        fi
      fi
    done
  else
    echo "$user_path direcroty is not present"
  fi
done

echo
echo name of executed script:
echo $0
