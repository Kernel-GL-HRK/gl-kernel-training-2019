#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Incorrect usage."
    echo "Path to the local Linux source repository is required."
    exit
elif [ ! -d $1 ]; then
    echo "$1 is not a directory."
    exit
fi

cd $1

if [ ! -d .git ]; then
    echo "$1 is not a git repository directory."
    exit
fi

echo "Repository contains:"
echo "$(git ls-files '*.c' | wc -l) *.c files;"
echo "$(git ls-files '*.cpp' | wc -l) *.cpp files;"
echo "$(git ls-files '*.py' | wc -l) *.py files."
echo ""

REV_NUM=$(git log --oneline | grep -c revert)
echo "There are $REV_NUM commits with word 'revert' in the name of the commit."
echo ""

echo "Number of lines of code in *.py files by author:"
git ls-files '*.py' | \
    while read f; do
        git blame --line-porcelain $f | grep '^author ' | sed 's/^author //'
    done | sort -f | uniq -ic | sort -nr

cd - &>/dev/null
