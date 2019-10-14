#!/bin/sh

display_help() {
  echo "Usage: $0 [target directory]"
  echo
}

if [ "$#" -ne 1 ] || [ "$1" = "--help" ]; then
  display_help
  exit 1
fi

if [ ! -d "$1" ]; then
  echo target directory does not exists
  exit 1
fi

cd $1
echo Quantity of files with extention .c in target diretory:
ls -lR | grep --count \.c$
echo Quantity of files with extention .cpp in target diretory:
ls -lR | grep --count \.cpp$
echo Quantity of files with extention .py in target diretory:
ls -lR | grep --count \.py$

echo
echo name of executed script:
echo $0
