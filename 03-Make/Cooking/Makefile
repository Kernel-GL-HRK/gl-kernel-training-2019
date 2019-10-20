#!/usr/bin/make -f 

DIRMARKET=food-market
PATHPOT=kitchen/table/pot/

SOURCE += salt.txt
SOURCE += chicken.txt
SOURCE += onion.txt
SOURCE += potato.txt
SOURCE += oil.txt
SOURCE += greenpeas.txt
SOURCE += carrot.txt

FOOD = $(addprefix $(DIRMARKET)/, $(SOURCE))

receipt_ready.txt: buy_food add_frying
	./ready.sh

add_frying: kitchen/table/pot add_greenpeas frying
	./put_frying_to_pot.sh kitchen/table/pot/add_frying.txt

add_greenpeas: kitchen/table/pot add_potato frying
	./put_greenpeas_to_pot.sh 200 kitchen/table/pot/add_greenpeas.txt

add_potato: kitchen/table/pot add_salt
	./put_potate_to_pot.sh 500 kitchen/table/pot/add_potato.txt

frying: kitchen/table/fpan/ add_carrot add_onion add_oil
	./frying.sh kitchen/table/fpan/frying.txt

add_carrot: kitchen/table/fpan/ add_onion
	./put_carrot_to_fpan.sh 1 kitchen/table/fpan/add_carrot.txt

add_onion: kitchen/table/fpan add_oil
	./put_onion_to_fpan.sh 2 kitchen/table/fpan/add_onion.txt

add_oil: kitchen/table/fpan buy_food
	./put_oil_to_fpan.sh 50 kitchen/table/fpan/add_oil.txt

add_salt: kitchen/table/pot add_chicken
	./put_salt_to_pot.sh kitchen/table/pot/add_salt.txt

add_chicken: kitchen/table/pot add_water
	./put_chicken_to_pot.sh 500 kitchen/table/pot/add_chicken.txt

add_water: kitchen/table/pot buy_food
	./get_water_from_faucet.sh 2000 kitchen/table/pot/add_water.txt

buy_food: $(FOOD)
	./buy_food_from_market.sh kitchen/table/receipt.txt


.PHONY: clean

clean:
	find "kitchen/table/" -type f -name "*.txt" -delete 
