#!/bin/bash

TABLE=kitchen/table
STOVE=kitchen/stove

SOUP=soup.txt
NOMAKE=nomake.sh

TITLE=misc/title.sh
ACTIONS_TITLE=misc/actions_title.sh

PRODUCTS_LIST=grocery/products_list.sh
DUNK_PEA=$TABLE/dunk_pea.sh
BOUILLON=$STOVE/bouillon.sh
ADD_PEA=$STOVE/add_pea.sh
SLICE_POTATO=$TABLE/slice_potato.sh
SLICE_ONION_AND_CARROT=$TABLE/slice_onion_and_carrot.sh
FRY_ONION_AND_CARROT=$STOVE/fry_onion_and_carrot.sh
ADD_SPICE=$STOVE/add_spice.sh

function write_to_file()
{
    cat $1 | sed 's/#\!\/bin\/bash//' | while read -r line; do
        if [[ "$line" =~ ^echo.* ]]; then
            echo "$line >> $SOUP"
        else
            echo $line
        fi
    done >> $NOMAKE
}

echo "#!/bin/bash" > $NOMAKE

echo "" > $NOMAKE

echo "if [ \$# -eq 1 ]; then
    case \$1 in
        clean)
            rm -f $SOUP
            ;;
        *)
            ;;
    esac
    exit
fi" >> $NOMAKE

write_to_file $TITLE
write_to_file $PRODUCTS_LIST
write_to_file $ACTIONS_TITLE
write_to_file $DUNK_PEA
write_to_file $BOUILLON
write_to_file $ADD_PEA
write_to_file $SLICE_POTATO
write_to_file $SLICE_ONION_AND_CARROT
write_to_file $FRY_ONION_AND_CARROT
write_to_file $ADD_SPICE

chmod +x $NOMAKE
