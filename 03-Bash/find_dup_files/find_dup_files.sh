#!/bin/bash

display_help() {
  echo "Usage: $0 [target directory]"
  echo
  echo "    -h, --help - print these options"
  echo
}

declare DYR_PATH

if [ "$#" -eq 0 ]; then
  DYR_PATH="."
else
  if [ "$1" = "--h" ] || [ "$1" = "--help" ]; then
    display_help
    exit 1
  else
    DYR_PATH="$1"
  fi
fi

for pfile in $(find $DYR_PATH -type f -name "*");
do
  echo $pfile -
  MDSUM=($(md5sum $pfile))
  if [ ! -z "$MDSUM" ]; then
    for sfile in $(find $DYR_PATH -type f -exec md5sum {} + | grep "^$MDSUM");
    do
      if [ $sfile != $pfile ] && [ -f "$sfile" ]; then
        echo $MDSUM :
        echo size of $sfile \file is $(stat -c%s "$sfile")
      fi
    done
  fi
done

echo
echo name of executed script:
echo $0
