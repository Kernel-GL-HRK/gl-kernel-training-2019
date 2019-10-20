#!/bin/bash

#help function
stats_help() {
  echo "Usage: $0 [target directory]"
}

#check if need felp?
if [[( "$#" -ne 1 ) || ( "$1" = "--help" )]]; then
  stats_help
  exit 1
fi

# Quantity of files with extention .c. 
echo Quantity of .c files in diretory
find $(pwd) -type f -name '*.c' | wc -l
# Quantity of files with extention .cpp.
echo Quantity of .cpp files in diretory
find $(pwd) -type f -name '*.cpp' | wc -l
# Quantity of files with extention .py.
echo Quantity of .py files in diretory
find $(pwd) -type f -name '*.py' | wc -l

# Quantity of commits with word 'revert'.
echo Quantity of commits with word "\'revert\'"
git rev-list HEAD --count --grep='revert'

# Lines for each author are in .py files.
find -name "*.py" -exec \
git blame --line-porcelain {} \; | sed -n 's/^author //p' | sort | uniq -c | sort -rn
