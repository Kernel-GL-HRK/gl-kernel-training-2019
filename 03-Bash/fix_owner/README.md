# Homework Bash 1:

Create the script 'fix_owner.sh'

For every user in system with id >= 1000 check home folder.
In case of inequality of user's id and file/dir/link owner's id 
ask for fixing it.
Do the same for default user's group file/dir/link group id.
Script should support command-line options:
  -f, --force - fix without asknowledgement,
  -h, --help - print these options.

Tips:
* use user's list from /etc/passwd
* for folder traversal use find utility
* for get users and group ids from line delimeted by : use cut -d\:
