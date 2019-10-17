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

# check options for script
if [ $# -ne 0 ]; then
    case $1 in 
        -h) Help;; 
        -f) fix_force=1;;
        *) exit 1;;
    esac
fi

for user_line in $(cat $user_file)
do
    declare -i user_id
    user_id=$(echo $user_line | cut -d ':' -f3)
    if [[ ($user_id >= 1000) && ($user_id <= 60000) ]]; then
		find $user_home -maxdepth 1 > temp_file
	    temp_file="homeFileList"
		continue
    fi
#collect about user
	user_name=$(echo $user_line | cut -d ':' -f1)
    user_home=$(echo $user_line | cut -d ':' -f6)
    user_group=$(echo $user_line | cut -d ':' -f4)

    for homeFile in $(cat $temp_file)
    do
      fileOwner=$(stat -c '%U' $homeFile)
      if [[ "$fileOwner" != "$user_name" ]]; then
          echo "$homeFile belong root"
          echo "appropriate?"
          echo -n "Y/N :"
          if [ $force -eq 1 ]
          then
              decision="Y"
              echo "forced"
          else
                read decision
          fi
          if [[ $decision == "Y" ]]; then
              echo "change owner $user_name $homeFile"
              sudo chown $user_name $homeFile
              if [ $(stat -c '%U' $homeFile) = "$user_name" ];then
                  echo "assign succes"
              else
                  echo "assign fail"
              fi
          fi
      fi
  done
done
rm ${temp_file} 
