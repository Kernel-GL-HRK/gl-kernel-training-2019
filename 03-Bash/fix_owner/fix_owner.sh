! #bin/bash

echo "test fix_owner.sh"
STATUS_FORCE=0


function parsing_got_arguments  {
	case $1 in
		"-f"| "--force")
			STATUS_FORCE=1
		;;
		"-h"| "--help")
			echo "-f, --force	force fixing for owner and group"
			echo "-h, --help	help information"
			exit 0
		    ;;
		"")
			STATUS_FORCE=0
		;;
		*)
			echo "Key $1 is mistake, please use $0 --help for more information"
			exit 0
		;;
	esac
}


parcing_got_arguments $1

if [[ $STATUS_FORCE -eq 1 ]]; then
	echo "STATUS_FORCE=1"	
else 
	echo "STATUS_FORCE=0"	
fi







