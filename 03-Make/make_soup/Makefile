#!/usr/bin/make -f

# Author: Dmitry Domnin
# This Makefile generates soup

PAR ?= "-"
MEAT="500 gram"
POTATOES="1000 gram"
ONIONS="200 gram"
CARROT="300 gram"
SALT="50 gram"
WATER="2000 ml"

PRODUCTS = meat.txt \
	potatoes.txt \
	onions.txt \
	carrot.txt \
	salt.txt

ifneq ($(PAR), -k)
soup: dirs $(PRODUCTS) buy slice get_water
	@cp kitchen/sliced/* kitchen/pot
	@sed "-es/Have bought salt/Add salt to soup/" < \
		kitchen/products/salt.txt > kitchen/pot/salt.txt
	@echo "<<<<<<<<<< Cooking soup >>>>>>>>>>"
	@cat ./kitchen/products/*.txt
	@sort -r ./kitchen/pot/*.txt
	@echo "Done!"

meat.txt:
	echo $(MEAT) > ./food_market/$@
potatoes.txt:
	echo $(POTATOES) > ./food_market/$@
onions.txt:
	echo $(ONIONS) > ./food_market/$@
carrot.txt:
	echo $(CARROT) > ./food_market/$@
salt.txt:
	echo $(SALT) > ./food_market/$@
water.txt:
	echo $(WATER) > ./kitchen/$@
dirs:
	mkdir -p ./food_market/
	mkdir -p ./kitchen/pot
	mkdir -p ./kitchen/products
	mkdir -p ./kitchen/sliced

buy: buy.sh
	./$< $(PRODUCTS)

slice: slice.sh
	./$< $(PRODUCTS)

get_water: water.sh water.txt
	./$< water.txt

clean:
	rm -Rf ./food_market
	rm -Rf ./kitchen
else
soup_nomake: script_nomake.sh
	./$<
endif

