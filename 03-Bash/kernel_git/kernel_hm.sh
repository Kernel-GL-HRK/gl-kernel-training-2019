#!/bin/sh

display_help() {
  echo "Usage: $0 [target directory]"
  echo
}

if [ "$#" -ne 1 ] || [ "$1" = "--h" ] || [ "$1" = "--help" ]; then
  display_help
  exit 1
fi

if [ ! -d "$1" ]; then
  echo target directory does not exists
  exit 1
fi

cd $1

# Print the quantity of files with extention .c, .cpp and *.py.
echo Quantity of files with extention .c in target diretory:
ls -lR | grep --count \.c$
echo Quantity of files with extention .cpp in target diretory:
ls -lR | grep --count \.cpp$
echo Quantity of files with extention .py in target diretory:
ls -lR | grep --count \.py$

# Find the quantity of commits with word 'revert' in commit name.
echo
echo Quantity of commits with word \'revert\'
git rev-list HEAD --count --grep='revert'

# How many lines for each author are in all *.py files in summury.
find -name "*.py" -exec \
git blame --line-porcelain {} \; | sed -n 's/^author //p' | sort | uniq -c | sort -rn

echo
echo name of executed script:
echo $0
