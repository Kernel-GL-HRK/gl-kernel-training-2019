#!/usr/bin/make -f

PATHMARKET=food-market
PATHPOT=kitchen/table/pot/
PATHFPAN=kitchen/table/fpan/

SOURCE += salt.txt
SOURCE += chicken.txt
SOURCE += onion.txt
SOURCE += potato.txt
SOURCE += oil.txt
SOURCE += greenpeas.txt
SOURCE += carrot.txt

FOOD = $(addprefix $(PATHMARKET)/, $(SOURCE))

receipt_ready.txt: buy_food add_frying
	./ready.sh

add_frying: add_greenpeas frying ${PATHPOT}
	echo "Put the frying to the pot and boil 3 min" \
	> ${PATHPOT}add_frying.txt

add_greenpeas: add_potato ${PATHPOT}
	echo "Put 100 g of the $$(cat kitchen/table/greenpeas.txt)" \
	" to the pot and boil 3 min" > ${PATHPOT}add_greenpeas.txt

add_potato: add_salt ${PATHPOT}
	echo "Peel 50 g of the $$(cat kitchen/table/potato.txt),"\
	" cut it, put to the pot"\
	" and boil 20-30 min" > ${PATHPOT}add_potato.txt

frying: add_carrot add_onion add_oil ${PATHFPAN}
	cat ${PATHFPAN}add_oil.txt > ${PATHFPAN}frying.txt
	cat ${PATHFPAN}add_onion.txt >> ${PATHFPAN}frying.txt
	cat ${PATHFPAN}add_carrot.txt >> ${PATHFPAN}frying.txt

add_carrot: add_onion ${PATHFPAN}
	echo "Chop one $$(cat kitchen/table/carrot.txt),"\
	" put it to the frying pan and fry it" > \
	${PATHFPAN}add_carrot.txt

add_onion: add_oil ${PATHFPAN}
	echo "Cut 1-2 st. of the $$(cat kitchen/table/onion.txt),"\
	" put it to the frying pan and fry it" \
	> ${PATHFPAN}add_onion.txt

add_oil: buy_food ${PATHFPAN}
	echo "Put 50 g of the $$(cat kitchen/table/oil.txt)" \
	" to the hot frying pan" > ${PATHFPAN}add_oil.txt

add_salt: add_chicken ${PATHPOT}
	echo "Put the $$(cat kitchen/table/salt.txt) to the pot and mix it" > \
	${PATHPOT}add_salt.txt

add_chicken: add_water ${PATHPOT}
	echo "Put 500 g of the $$(cat kitchen/table/chicken.txt) to the pot" \
	" and boil 60 min" > ${PATHPOT}add_chicken.txt

add_water: buy_food ${PATHPOT}
	./get_water_from_faucet.sh 2000 kitchen/table/pot/add_water.txt

buy_food: create_res $(FOOD)
	./buy_food_from_market.sh kitchen/table/receipt.txt

create_res:
	./create_res.sh ${PATHMARKET} kitchen/faucet/

.PHONY: clean

clean:
	find -type f -name "*.txt" -delete
