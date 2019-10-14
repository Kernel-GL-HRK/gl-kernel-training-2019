#!/bin/bash

# $1 - linux kernel sources path

if [ $# -lt 1 ]
then
    echo "set kernel path as parametr"
    exit 1
fi

echo "*.c :$(find "$1" -name "*.c" -type f | wc -l)"
echo "*.cpp :$(find "$1" -name "*.cpp" -type f | wc -l)"
echo "*.c :$(find "$1" -name "*.py" -type f | wc -l)"

cd "$1"
git checkout master 2&>1 /dev/null

echo "revert commit count: $(git log --oneline | grep revert | wc -l)"

autors_list=$(mktemp)
git ls-files '*.py' | while read py_file
do
    echo "py_file: $py_file"
    git blame --line-porcelain "$py_file" >> $autors_list
done

sed -n 's/^author //p' "$autors_list"| sort | uniq -c | sort -rn

# cat $autors_list
