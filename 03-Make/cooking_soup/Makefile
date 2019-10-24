#!/usr/bin/make -f

GOAL="Soup"

POTATOES = potatoes
CABBAGE = cabbage
ONIONS = onions
MEAT = meat
CARROT = carrot
SALT = salt

QUANTITY_WATER="3 l"
QUANTITY_POTATOES="800 g"
QUANTITY_CABBAGE="1000 g"
QUANTITY_ONIONS="350 g"
QUANTITY_MEAT="500 g"
QUANTITY_CARROT="200 g"
QUANTITY_SALT="50 g"

FOOD_MARKET = food_market
KITCHEN = kitchen
TABLE = table
SPOON = spoon
KNIFE = knife
POT = pot
SLICED_PRODUCTS = sliced_products
DINNING_ROOM = dinning_room
PRODUCTS = products


TOTAL_GOAL_PRODUCTS = $(POTATOES).txt $(CABBAGE).txt \
			$(ONIONS).txt $(MEAT).txt \
			$(CARROT).txt $(SALT).txt

CREATE_WITHOUT_MAKE = "create_without_make.sh"

$(GOAL): create_directories $(TOTAL_GOAL_PRODUCTS) buy_products cut_products \
	 move_sliced_products_to_pot bouling
ifneq ($(ARG),-k)
	@cat ./$(KITCHEN)/$(TABLE)/$(POT)/*.txt
	@echo "$(GOAL) is cooked!"
else
	@echo "cat ./$(KITCHEN)/$(TABLE)/$(POT)/*.txt" >> $(CREATE_WITHOUT_MAKE)
	@echo "echo '$(GOAL) is cooked!'" >> $(CREATE_WITHOUT_MAKE)
endif
create_directories:
ifneq ($(ARG),-k)
	echo "ARG=$(ARG)"
	mkdir -vp ./$(FOOD_MARKET)/
	mkdir -vp ./$(KITCHEN)/$(TABLE)/$(SPOON)
	mkdir -vp ./$(KITCHEN)/$(TABLE)/$(KNIFE)
	mkdir -vp ./$(KITCHEN)/$(TABLE)/$(POT)
	mkdir -vp ./$(KITCHEN)/$(PRODUCTS)
	mkdir -vp ./$(KITCHEN)/$(SLICED_PRODUCTS)
	mkdir -vp ./$(DINNING_ROOM)
	touch ./$(KITCHEN)/water.txt
else
	touch ./$(CREATE_WITHOUT_MAKE)
	chmod a+x ./$(CREATE_WITHOUT_MAKE)
	@echo "#!/bin/bash" >> $(CREATE_WITHOUT_MAKE)
	@echo "mkdir -vp ./$(FOOD_MARKET)/" >> $(CREATE_WITHOUT_MAKE)
	@echo "mkdir -vp ./$(KITCHEN)/$(TABLE)/$(SPOON)" >> $(CREATE_WITHOUT_MAKE)
	@echo "mkdir -vp ./$(KITCHEN)/$(TABLE)/$(KNIFE)" >> $(CREATE_WITHOUT_MAKE)
	@echo "mkdir -vp ./$(KITCHEN)/$(TABLE)/$(POT)" >> $(CREATE_WITHOUT_MAKE)
	@echo "mkdir -vp ./$(KITCHEN)/$(PRODUCTS)" >> $(CREATE_WITHOUT_MAKE)
	@echo "mkdir -vp ./$(KITCHEN)/$(SLICED_PRODUCTS)" >> $(CREATE_WITHOUT_MAKE)
	@echo "mkdir -vp ./$(DINNING_ROOM)" >> $(CREATE_WITHOUT_MAKE)
	@echo "touch ./$(KITCHEN)/water.txt" >> $(CREATE_WITHOUT_MAKE)
endif

potatoes.txt:
ifneq ($(ARG),-k)
	echo $(QUANTITY_POTATOES) > ./$(FOOD_MARKET)/$@
else
	@echo "echo $(QUANTITY_POTATOES) > ./$(FOOD_MARKET)/$@" >> $(CREATE_WITHOUT_MAKE)
endif

cabbage.txt:
ifneq ($(ARG),-k)
	echo $(QUANTITY_CABBAGE) > ./$(FOOD_MARKET)/$@
else
	@echo "echo $(QUANTITY_CABBAGE) > ./$(FOOD_MARKET)/$@" >> $(CREATE_WITHOUT_MAKE)
endif

onions.txt:
ifneq ($(ARG),-k)
	echo $(QUANTITY_ONIONS) > ./$(FOOD_MARKET)/$@
else
	@echo "echo $(QUANTITY_ONIONS) > ./$(FOOD_MARKET)/$@" >> $(CREATE_WITHOUT_MAKE)
endif

meat.txt:
ifneq ($(ARG),-k)
	echo $(QUANTITY_MEAT) > ./$(FOOD_MARKET)/$@
else
	@echo "echo $(QUANTITY_MEAT) > ./$(FOOD_MARKET)/$@" >> $(CREATE_WITHOUT_MAKE)
endif

carrot.txt:
ifneq ($(ARG),-k)
	echo $(QUANTITY_CARROT) > ./$(FOOD_MARKET)/$@
else
	@echo "echo $(QUANTITY_CARROT) > ./$(FOOD_MARKET)/$@" >> $(CREATE_WITHOUT_MAKE)
endif

salt.txt:
ifneq ($(ARG),-k)
	echo $(QUANTITY_SALT) > ./$(FOOD_MARKET)/$@
else
	@echo "echo $(QUANTITY_SALT) > ./$(FOOD_MARKET)/$@" >> $(CREATE_WITHOUT_MAKE)
endif


water.txt:
ifneq ($(ARG),-k)
	echo $(QUANTITY_WATER) > ./$(KITCHEN)/$@
else
	@echo "echo $(QUANTITY_WATER) > ./$(KITCHEN)/$@" >> $(CREATE_WITHOUT_MAKE)
endif


buy_products: buy_products.sh
ifneq ($(ARG),-k)
	./$< $(TOTAL_GOAL_PRODUCTS)
else
	@echo "./$< $(TOTAL_GOAL_PRODUCTS)" >> $(CREATE_WITHOUT_MAKE)
endif

cut_products: cut_products.sh
ifneq ($(ARG),-k)
	./$< $(TOTAL_GOAL_PRODUCTS)
else
	@echo "./$< $(TOTAL_GOAL_PRODUCTS)" >> $(CREATE_WITHOUT_MAKE)
endif


move_sliced_products_to_pot:
ifneq ($(ARG),-k)
	@cp $(KITCHEN)/$(SLICED_PRODUCTS)/* ./$(KITCHEN)/$(TABLE)/$(POT)
else
	@echo "cp $(KITCHEN)/$(SLICED_PRODUCTS)/* ./$(KITCHEN)/$(TABLE)/$(POT)" >> $(CREATE_WITHOUT_MAKE)
endif


bouling: bouling.sh
ifneq ($(ARG),-k)
	./$< $(TOTAL_GOAL_PRODUCTS)
else
	@echo "./$< $(TOTAL_GOAL_PRODUCTS)" >> $(CREATE_WITHOUT_MAKE)
endif


clean:
	rm -Rf ./$(FOOD_MARKET)
	rm -Rf ./$(KITCHEN)
	rm -Rf ./$(DINNING_ROOM)
