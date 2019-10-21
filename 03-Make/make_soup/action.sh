#!/bin/bash

_tmp=$(mktemp)

if [ ! -d $3 ] ; then mkdir $3 ; fi
if [ ! -d $4 ] ; then mkdir $4 ; fi

PRODUCTS="$1"
ACTION="$2"
DIR_BUILD_SOUP="$3"
DIR_SRC_SOUP="$4"

case $ACTION in
    buy)
        echo $PRODUCTS | tr ':' '\n' > $_tmp
        while read -r _line;do
            echo "Buy $_line" >>  "$DIR_BUILD_SOUP/$ACTION.o"
            echo "Buy $_line"  >> "$DIR_SRC_SOUP/$_line.c"
        done < $_tmp
        ;;

    cooking_meat|cooking_vegetables|cooking_frying)
        echo $PRODUCTS | tr ' ' '\n' > $_tmp
        while read -r _line;do
            if [ $ACTION = cooking_frying ]; then
                echo "Cut and Put $_line to cooking_pot"  >>  "$DIR_BUILD_SOUP/$ACTION.o"
            else
                echo "Put $_line to cooking_pot"  >>  "$DIR_BUILD_SOUP/$ACTION.o"
            fi
        done < $_tmp
        ;;

     *)
        echo "Unknown action"
        ;;
esac

rm $_tmp

