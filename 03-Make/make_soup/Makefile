SOUP_NAME=french_soup

PRODUCTS?="Water:Chicken_fillet:Cream_Cheese:Potato:Onion:\
Carrot:Butter:Salt:Ground_black_pepper:Greens:Bay_leaf:\
Croutons:Black_pepper_peas"

SRC_CUT?=$(DIR_SRC_SOUP)/Potato.c $(DIR_SRC_SOUP)/Onion.c \
$(DIR_SRC_SOUP)/Carrot.c $(DIR_SRC_SOUP)/Cream_Cheese.c

OBJS_CUT?=$(DIR_BUILD_SOUP)/Potato.o $(DIR_BUILD_SOUP)/Onion.o \
$(DIR_BUILD_SOUP)/Carrot.o $(DIR_BUILD_SOUP)/Cream_Cheese.o

DIR_BUILD_SOUP:=build_soup
DIR_SRC_SOUP:=src_soup

###-MAIN-###

$(SOUP_NAME): $(DIR_BUILD_SOUP)/buy.o $(DIR_BUILD_SOUP)/bouillon.o
	echo "-=BUY=-" >> $@
	cat $^ >> $@

$(DIR_BUILD_SOUP)/bouillon.o: $(DIR_BUILD_SOUP)/cooking_meat.o \
$(DIR_BUILD_SOUP)/cooking_vegetables.o $(DIR_BUILD_SOUP)/cooking_frying.o
	cat $^ >> $@

$(DIR_BUILD_SOUP)/cooking_meat.o: $(DIR_SRC_SOUP)/Chicken_fillet.c \
$(DIR_SRC_SOUP)/Water.c $(DIR_SRC_SOUP)/Salt.c \
$(DIR_SRC_SOUP)/Ground_black_pepper.c $(DIR_SRC_SOUP)/Bay_leaf.c
	echo "-=cooking_meat=-" >> $@
	./action.sh "$^" cooking_meat $(DIR_BUILD_SOUP) $(DIR_SRC_SOUP)

$(DIR_BUILD_SOUP)/cooking_vegetables.o: $(DIR_SRC_SOUP)/Potato.c \
$(DIR_SRC_SOUP)/Onion.c $(DIR_SRC_SOUP)/Carrot.c $(DIR_SRC_SOUP)/Cream_Cheese.c
	echo "-=cooking_vegetables=-" >> $@
	./action.sh "$^" cooking_vegetables $(DIR_BUILD_SOUP) $(DIR_SRC_SOUP)


#$(DIR_BUILD_SOUP)/Onion.o $(DIR_BUILD_SOUP)/Carrot.o $(DIR_BUILD_SOUP)/Salt.o Ground_black_pepper.o
$(DIR_BUILD_SOUP)/cooking_frying.o: $(OBJS_CUT)
	echo "-=cooking_frying=-" >> $@
	./action.sh "$^" cooking_frying $(DIR_BUILD_SOUP) $(DIR_SRC_SOUP)

$(OBJS_CUT): $(SRC_CUT)
	@echo "Cut $^ into $@" > $@

$(DIR_BUILD_SOUP)/buy.o:
	./action.sh $(PRODUCTS) buy $(DIR_BUILD_SOUP) $(DIR_SRC_SOUP)

clean:
	-rm -rf $(DIR_BUILD_SOUP) $(DIR_SRC_SOUP) $(SOUP_NAME)

