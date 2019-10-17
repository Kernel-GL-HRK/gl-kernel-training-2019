#!/bin/bash

showHelp(){
	echo "This script fix owner and group for files"
	echo "-h|--help - help"
	echo "-f|--force - fix without awcknowledge"
	echo "If ther is no parameter, function fix owner with awcknowledge" 
}

getUsers(){
	while read userLine
	do
	uId=$(echo "$userLine" | cut -d ":" -f 3) 
	if [[ uId -ge 1000 && uId -le 60000 ]]
	then echo  "$userLine" | cut -d ":" -f3,4,6 
	fi
	done < /etc/passwd
}

fixWithoutAcknowledge(){
	getUsers | while read userInfo
	do 
		currUserId=$(echo "$userInfo" | cut -d: -f 1)
		currGroupId=$(echo "$userInfo" | cut -d: -f 2)
		currHomeFolder=$(echo "$userInfo" | cut -d: -f 3)

		find "$currHomeFolder" | while read fileLine
		do
			fileOwnerId=$(stat -c "%u" "$fileLine")
			fileGroupId=$(stat -c "%g" "$fileLine")

			if [[ "$fileOwnerId" -ge 1000 && "$fileOwnerId" -le 60000 ]]
			then
				if [[ "$fileOwnerId" -ne "currUserId" ]]
				then
					chown "$currUserId" "$fileLine"
				fi
			fi

			if [[ "$fileGroupId" -ge 1000 && "$fileGroupId" -le 60000 ]]
			then
				if [[ "$fileGroupId" -ne "$currGroupId" ]]
				then
					chgrp "$currGroupId" "$fileLine"
				fi
			fi
		done
	done 
}

getListForChanging(){
	getUsers | while read userInfo
	do
		currUserId=$(echo "$userInfo" | cut -d: -f 1)
		currGroupId=$(echo "$userInfo" | cut -d: -f 2)
		currHomeFolder=$(echo "$userInfo" | cut -d: -f 3)

		find "$currHomeFolder" | while read fileLine
		do
			fileOwnerId=$(stat -c "%u" "$fileLine")
			fileGroupId=$(stat -c "%g" "$fileLine")

			if [[ "$fileOwnerId" -ge 1000 && "$fileOwnerId" -le 60000 ]]
			then
				if [[ "$fileOwnerId" -ne "currUserId" ]]
				then
					echo "owner:$currUserId:$fileLine"
				fi
			fi

			if [[ "$fileGroupId" -ge 1000 && "$fileGroupId" -le 60000 ]]
			then
				if [[ "$fileGroupId" -ne "$currGroupId" ]]
				then
					echo "group:$currGroupId:$fileLine"
				fi
			fi
		done
	done
}

fixWithAcknowledge(){
	FileChangeList=$(getListForChanging)

	for fileChInfo in $FileChangeList
	do
		owGrChange=$(echo "$fileChInfo" | cut -d: -f 1)
		owGrId=$(echo "$fileChInfo" | cut -d: -f 2)
		fileName=$(echo "$fileChInfo" | cut -d: -f 3)

		read -p "Do you want to change $owGrChange for $fileName [y/n] > " answer
		if [[ $answer = "y"  ]]
		then
			if [[ $owGrChange = "owner" ]]
			then
				chown $owGrId $fileName
			else
				chgrp $owGrId $fileName
			fi
		fi
	done
}

if [ $# -eq 1 ]
then
	if [[ $1 = "-h" || $1 = "--help" ]]
	then
	showHelp
	elif [[ $1 = "-f" || $1 = "--force" ]]
	then
	fixWithoutAcknowledge
	else 
	echo "wrong parameter"
	fi
elif [ $# -eq 0 ]
then
	fixWithAcknowledge
else
	echo "Error!"
fi
