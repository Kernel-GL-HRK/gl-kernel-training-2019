#!/bin/bash
if [[ $# -ne 1 ]]
then
    echo -e "Need set path to the local Linux repository."
    exit 0
fi
if [[ ! -d $1 ]]
then
    echo "The $1 isnt a directory."
    exit
fi
cd $1
if [[ ! -d .git ]]
	then
    echo "The $1 doesnt contain repository."
    exit 0
fi


echo -e "Amount $(git log --oneline | grep -c revert) commits with word 'revert' contain in the commits./n"
echo -e "In the repository contains:/n"
echo "1.Amount $(git ls-files '*.py'|wc -l) *.py files."
echo "2.Amount $(git ls-files '*.cpp'|wc -l) *.cpp files;"
echo -e "3.Amount $(git ls-files '*.c'|wc -l) *.c files;/n"

echo "Amount of lines in the source code in *.py files by author:"
git ls-files '*.py' | while read f; do git blame --line-porcelain $f | grep '^author ' | sed 's/^author //'; done | sort -f| uniq -ci | sort -nr
