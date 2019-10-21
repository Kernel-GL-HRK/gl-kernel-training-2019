#!/usr/bin/make 

#Description the structure locations:
#
#home
	#kitchen_room
		#table
			#knife
		#cupboard
			#spoon
			#pot
			#sink
			#plate
		#fridge
		#stove
		#kitchen_table
	#dinning_room
		#dinner_table
#food_market
	#vegetable_section
		#potato.txt
		#carrot.txt
		#onion.txt
		#cabbage.txt
	#meat_section
		#meat.txt
	#spice_section
		#peper.txt
		#salt.txt
		#sugar.txt
	#bread_section
		#bread.txt

.PHONY: clear

HOME=./home
KITCHEN="$(HOME)/kitchen_room"
KITCHEN_TABLE=$(KITCHEN)/table
CUPBOARD=$(KITCHEN)/cupboard
FRIDGE=$(KITCHEN)/fridge
STOVE=$(KITCHEN)/stove
SINK=$(KITCHEN)/sink
KNIFE=$(KITCHEN_TABLE)/knife
POT=$(CUPBOARD)/pot
PLATE=$(CUPBOARD)/plate

DINNING_ROOM=$(HOME)/dinning_room
DINNING_TABLE=$(DINNING_ROOM)/table

MARKET=./market
VEGETABLE_SECTION=$(MARKET)/vegetable_section
MEAT_SECTION=$(MARKET)/meat_section
SPICE_SECTION=$(MARKET)/spice_section
BREAD_SECTION=$(MARKET)/bread_section

SCRIPT=./script
F_INIT_SH=init.sh
F_BUY_SH=buy.sh
F_WASH_SH=wash.sh
F_K_TABLE_SH=k_table.sh
F_COOKING_SH=cooking.sh
F_SOUP_SH=soup.sh


BUILD_SH=./build.sh

all: init soup.txt
ifeq ($(ARG),-k) 
	@echo "#!/bin/sh" > $(BUILD_SH)
	@echo "bash $(SCRIPT)/$(F_INIT_SH)" >> $(BUILD_SH)
	@echo "bash $(SCRIPT)/$(F_BUY_SH)" >> $(BUILD_SH)
	@echo "bash $(SCRIPT)/$(F_WASH_SH)" >> $(BUILD_SH)
	@echo "bash $(SCRIPT)/$(F_K_TABLE_SH)" >> $(BUILD_SH)
	@echo "bash $(SCRIPT)/$(F_COOKING_SH)" >> $(BUILD_SH)
	@echo "bash $(SCRIPT)/$(F_SOUP_SH) " >> $(BUILD_SH)
	@echo "echo Bon Appetit" >> $(BUILD_SH)
else
	@bash $(SCRIPT)/$(F_INIT_SH)
	@bash $(SCRIPT)/$(F_BUY_SH)
	@bash $(SCRIPT)/$(F_WASH_SH)
	@bash $(SCRIPT)/$(F_K_TABLE_SH)
	@bash $(SCRIPT)/$(F_COOKING_SH)
	@bash $(SCRIPT)/$(F_SOUP_SH); 
	@echo "Bon Appetit"
endif
	

init:
	@mkdir -p $(SCRIPT)
	@echo "#!/bin/sh" > $(SCRIPT)/$(F_INIT_SH)
	@echo "echo Building_house Ok:" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "mkdir -p $(HOME) $(KITCHEN) $(DINNING_ROOM)" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo Furnishings_house Ok:" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "mkdir -p $(KITCHEN_TABLE) $(CUPBOARD) $(DINNING_TABLE) $(FRIDGE)" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo water > $(CUPBOARD)/water.txt" >> $(SCRIPT)/$(F_INIT_SH)
	
	@echo "echo Building_market Ok:" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "mkdir -p $(MARKET) $(VEGETABLE_SECTION) $(MEAT_SECTION) $(SPICE_SECTION) $(BREAD_SECTION) $(SINK)	\
		$(POT) $(KNIFE) $(PLATE) $(STOVE)"  >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo Delivery of goods Ok:" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo potato > $(VEGETABLE_SECTION)/potato.txt" >> $(SCRIPT)/$(F_INIT_SH) 
	@echo "echo carrot > $(VEGETABLE_SECTION)/carrot.txt" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo onion > $(VEGETABLE_SECTION)/onion.txt" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo cabbage > $(VEGETABLE_SECTION)/cabbage.txt" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo meat > $(MEAT_SECTION)/meat.txt" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo peper>$(SPICE_SECTION)/peper.txt" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo salt >$(SPICE_SECTION)/salt.txt" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo sugar>$(SPICE_SECTION)/sugar.txt" >> $(SCRIPT)/$(F_INIT_SH)
	@echo "echo bread>$(BREAD_SECTION)/bread.txt" >> $(SCRIPT)/$(F_INIT_SH)

soup.txt: cooking.o
	@echo "mv $(CUPBOARD)/plate $(KITCHEN_TABLE)" >> $(SCRIPT)/$(F_SOUP_SH)
	@echo "for item in \$$(find $(STOVE)/pot -type f); do \
		mv \$$item $(KITCHEN_TABLE)/plate; \
	done;" >>  $(SCRIPT)/$(F_SOUP_SH)
	@echo "mv $(KITCHEN_TABLE)/plate $(DINNING_TABLE)" >> $(SCRIPT)/$(F_SOUP_SH)
	
cooking.o: table_products.o  
	@echo "echo cooking" >> $(SCRIPT)/$(F_COOKING_SH)
	@echo "mv $(POT) $(KITCHEN_TABLE)" >> $(SCRIPT)/$(F_COOKING_SH)
	@echo "mv $(KNIFE) $(KITCHEN_TABLE)/pot" >> $(SCRIPT)/$(F_COOKING_SH)
	@echo "mv $(CUPBOARD)/water.txt $(KITCHEN_TABLE)/pot" >> $(SCRIPT)/$(F_COOKING_SH)
	@echo "echo put on the stove">> $(SCRIPT)/$(F_COOKING_SH)
	@echo "mv $(KITCHEN_TABLE)/* $(STOVE)">> $(SCRIPT)/$(F_COOKING_SH)
	@echo "echo add spice" >> $(SCRIPT)/$(F_COOKING_SH)
	@echo "for item in \$$(find $(CUPBOARD) -type f); do \
		mv \$$item $(STOVE)/pot; \
		echo \"spice-> \" \$$item; \
	done" >> $(SCRIPT)/$(F_COOKING_SH)
	@echo "echo wait" >> $(SCRIPT)/$(F_COOKING_SH)

table_products.o: buy_products.o wash_products.o
	@echo "echo cut products" >> $(SCRIPT)/$(F_K_TABLE_SH)
	
	@echo "for item in \$$(find $(KITCHEN_TABLE) -type f); do \
		 mv \$$item $(KNIFE); \
		 echo \"cut-> \" \$$item; \
	done"  >> $(SCRIPT)/$(F_K_TABLE_SH)
	
wash_products.o:
	@echo "echo wash products" >> $(SCRIPT)/$(F_WASH_SH)
	
	@echo " for item in \$$(find $(FRIDGE) -type f); do \
		 mv \$$item $(SINK); \
		 echo \"washed-> \" \$$item; \
	done" >> $(SCRIPT)/$(F_WASH_SH)
	
	@echo "for item in \$$(find $(SINK) -type f); do \
		 mv \$$item $(KITCHEN_TABLE); \
	done" >> $(SCRIPT)/$(F_WASH_SH)
	
buy_products.o:
	@echo "echo buy products" >> $(SCRIPT)/$(F_BUY_SH)
	
	@echo "for item in \$$(find $(VEGETABLE_SECTION) -type f); do \
		 cp  -r \$$item $(FRIDGE); \
		  echo \"buy-> \" \$$item; \
	done" >> $(SCRIPT)/$(F_BUY_SH)
	
	@echo "for item in \$$(find $(MEAT_SECTION) -type f); do \
		 cp  -r \$$item $(FRIDGE); \
		 echo \"buy-> \" \$$item; \
	done" >> $(SCRIPT)/$(F_BUY_SH)
	
	@echo "for item in \$$(find $(SPICE_SECTION) -type f); do \
		 cp  -r \$$item $(CUPBOARD); \
		 echo \"buy-> \" \$$item; \
	done" >> $(SCRIPT)/$(F_BUY_SH)
	
	@echo "for item in \$$(find $(BREAD_SECTION) -type f); do \
		 cp  -r \$$item $(DINNING_TABLE); \
		 echo \"buy-> \" \$$item; \
	done" >> $(SCRIPT)/$(F_BUY_SH)
	
	
clean:
	rm  -rf $(HOME)
	rm  -rf $(MARKET)
	rm  -rf $(SCRIPT)
	rm  -f  $(BUILD_SH)
