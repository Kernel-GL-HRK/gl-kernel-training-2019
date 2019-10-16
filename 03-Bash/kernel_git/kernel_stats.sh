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
