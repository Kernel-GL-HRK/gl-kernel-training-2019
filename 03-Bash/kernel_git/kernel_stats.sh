#!/bin/bash

case $1 in
    "-h" | "--help")
        echo "Program prints statistics for kernel files"
        exit 0
        ;;
    "")
        DIR=$(pwd)
        ;;
    *)
        if [[ -d $1 ]]; then        
            DIR=$1
        else
            exit 0	
        fi
;;
esac

cd "$1"
function c_cnt_calc {
    echo "$(find $1 -type f -name "*.c" | wc -l)"
}

function cpp_cnt_calc {
    echo "$(find $1 -type f -name "*.cpp" | wc -l)"
}

function py_cnt_calc {
    echo "$(find $1 -type f -name "*.py" | wc -l)"
}

function revert_cnt_calc {
    echo "$(git log --oneline --grep="revert" master | wc -l) "
}

function search_author {
    find "$1" -type f -name '*.py' -exec git blame --line-porcelain {} \; | grep "author " | cut -d" " --fields=2,3 | sort | uniq -c
}

echo "Count of .c files - $(c_cnt_calc "$DIR")"
echo "Count of .cpp files - $(cpp_cnt_calc  "$DIR")"
echo "Count of .py files - $(py_cnt_calc "$DIR")"
echo "Count of revert words - $(revert_cnt_calc "$DIR")"
echo "Count of lines for each authors in .py files:"
echo "$(search_author "$DIR")"
