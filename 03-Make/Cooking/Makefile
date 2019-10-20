#!/usr/bin/make -f 

DIRMARKET=food-market

SOURCE += salt.txt
SOURCE += chicken.txt
SOURCE += onion.txt
SOURCE += potato.txt
SOURCE += oil.txt
SOURCE += greenpeas.txt
SOURCE += carrot.txt

FOOD = $(addprefix $(DIRMARKET)/, $(SOURCE))
     #$(addprefix src/,foo bar)

buy_food: $(FOOD)
	./buy_food_from_market.sh

.PHONY: clean

clean:
	-rm ./kitchen/table/*.txt
