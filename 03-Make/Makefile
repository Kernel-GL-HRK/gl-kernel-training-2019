#!/usr/bin/make 

#--------------------------------------------------
#Variable parametrs
Key = -k
Wife = W
#--------------------------------------------------

#Inialisation Home and scripts dir for event
Home=./home
Scr=./scr

#Inialisation tools for Soup event
Kitchen="$(Home)/kitchen"
Fridge=$(Kitchen)/fridge
Closet=$(Kitchen)/closet
Kitchen_table=$(Kitchen)/table
Knife=$(Kitchen_table)/knife
Chairs=$(Kitchen_table)/chairs
Stove=$(Kitchen)/stove
Pot=$(closet)/pot
Slow_cooker=$(kitchen)/slow_cooker
Sink=$(kitchen)/sink

#Inialisation Bedroom
Bedroom=$(Home)/bedroom

#Inialisation Restroom
Restroom=$(Home)/restroom

#Inialisation Dinning room
Dinning_room=$(Home)/dinning_room

#Inialisation tools for dinner
Cutlery=$(Dinning_room)/cutlery
Chairs_Dining_room=$(Dinning_room)/chairs_dining_room
Dinning_table=$(Dinning_room)/table

#Main locations for Soup event
Food_Market=./food_market
Meat=$(Food_Market)/meat
Pickled_Vagetebl=$(Food_Market)/pickled_vagetebl
Spice=$(Food_Market)/spice
Water=$(Food_Market)/water
Vegetable=$(Food_Market)/vegetable

#Additional locations from Wife
AnimalFood=$(Food_Market)/animal_food
Candies_Cakes=$(Food_Market)/candies_cakes
Milk_Yogurts=$(Food_Market)/milk_yogurts

#init scripts for soup event
Init_sh=init.sh
purchase_sh=purchase.sh
wash_sh=wash.sh
boil_sh=boil.sh
fry_sh=fry.sh
cat_sh=cat.sh
cooking_sh=cooking.sh
soup_sh=soup.sh

dev_sh=./dev.sh

all: init soup.txt
init: SHELL:=/bin/bash
init:
	@mkdir -p $(Scr)
	@echo "#!/bin/bash" > $(Scr)/$(Init_sh)
	@echo "mkdir -p $(Home) $(Restroom) $(Bedroom) $(Kitchen) $(dinning_room)" >> $(Scr)/$(Init_sh)
	@echo "mkdir -p $(kitchen_table) $(closet) $(Dinning_table) $(Fridge)" >> $(Scr)/$(Init_sh)
	@echo "mkdir -p $(Food_Market) $(Vegetable) $(Pickled_Vagetebl) $(Water) $(Meat) $(Spice) $(Sink) $(Pot) $(Knife) $(Slow_cooker) $(Stove)"  >> $(Scr)/$(Init_sh)
	
	@echo "echo meat > $(Meat)/meat.txt" >> $(Scr)/$(Init_sh)
	@echo "echo peper > $(Spice)/peper.txt" >> $(Scr)/$(Init_sh)
	@echo "echo salt > $(Spice)/salt.txt" >> $(Scr)/$(Init_sh)
	@echo "echo sugar > $(Spice)/sugar.txt" >> $(Scr)/$(Init_sh)
	@echo "echo water > $(Water)/water.txt" >> $(Scr)/$(Init_sh)
	@echo "echo potato > $(Vegetable)/potato.txt" >> $(Scr)/$(Init_sh) 
	@echo "echo carrot > $(Vegetable)/carrot.txt" >> $(Scr)/$(Init_sh)
	@echo "echo onion > $(Vegetable)/onion.txt" >> $(Scr)/$(Init_sh)
	@echo "echo cabbage > $(Vegetable)/cabbage.txt" >> $(Scr)/$(Init_sh)
	@echo "echo dog_fod > $(AnimalFood)/dog_fod.txt" >> $(Scr)/$(Init_sh)
	@echo "echo cake > $(Candies_Cakes)/cake.txt" >> $(Scr)/$(Init_sh)
	@echo "echo milk > $(Milk_Yogurts)/milk.txt" >> $(Scr)/$(Init_sh)
	@echo "echo p_tomato > $(Pickled_Vagetebl)/p_tomato.txt" >> $(Scr)/$(Init_sh)


soup.txt: SHELL:=/bin/bash
soup.txt: cook.o
	@echo "mv $(Slow_cooker)/pot $(Dinning_table)" >> $(Scr)/$(soup_sh)

cook.o: table_products.o fry_products.o boil_products.o
	@echo "mv $(Pot) $(kitchen_table)" >> $(Scr)/$(cooking_sh)
	@echo "mv $(Knife) $(kitchen_table)/pot" >> $(Scr)/$(cooking_sh)
	@echo "mv $(kitchen_table)/* $(Slow_cooker)">> $(Scr)/$(cooking_sh)
	@echo "mv $(spice)/* $(Slow_cooker)">> $(Scr)/$(cooking_sh)
	@echo "echo timer 45 min" >> $(Scr)/$(cooking_sh)

fry_products.o:
	@echo "echo fry all for fry" >> $(Scr)/$(cat_sh)
	@echo "for carrot in \$$($($(sink))find -type f); do \mv \$$carrot $(Slow_cooker); \echo \"fry \" \$$carrot; \done" >> $(Scr)/$(fry_sh)

table_products.o: purchase_products.o wash_products.o
	@echo "echo cut all for cut" >> $(Scr)/$(cat_sh)
	@echo "for thing in \$$($(cd $(kitchen_table))find -type f); do \mv \$$thing $(Knife); \echo \"cut \" \$$thing; \done"  >> $(Scr)/$(cat_sh)

boil_products.o:
	@echo " for water in \$$($(cd $(closet))find -type f); do \mv \$$water $(pot); \echo \"pour \" \$$water; \done" >> $(Scr)/$(boil_sh)
	
wash_products.o:
	@echo "for meat in \$$($(cd $(Fridge))find -type f); do \mv \$$meat $(Sink); \echo \"wash \" \$$meat; \done" >> $(Scr)/$(wash_sh)
	@echo "for vegetable in \$$($(cd $(Closet))find -type f); do \mv \$$vegetable $(Sink); \echo \"wash \" \$$vegetable; \done" >> $(Scr)/$(wash_sh)

purchase_products.o:
	@echo "for vegetable in \$$($(cd $(Vegetable))find -type f); do \cp  -r \$$vegetable $(Closet); \echo \"purchase \" \$$vegetable; \done" >> $(Scr)/$(purchase_sh)
	@echo "for meat in \$$($(cd $(Meat))find -type f); do \cp  -r \$$meat $(Fridge); \echo \"purchase-> \" \$$meat; \done" >> $(Scr)/$(purchase_sh)
	@echo "for spice in \$$($(cd $(Spice)) find -type f); do \cp  -r \$$spice $(closet); \echo \"purchase-> \" \$$spice; \done" >> $(Scr)/$(purchase_sh)

ifeq (Wife,W) 
purchase_products_wife.o:
	@echo "for dog_fod in \$$($(cd $(AnimalFood)) find -type f); do \cp  -r \$$dog_fod $(Closet); \echo \"purchase \" \$$dog_fod; \done" >> $(Scr)/$(purchase_sh)
	@echo "for cake in \$$($(cd $(Candies_Cakes))find -type f); do \cp  -r \$$cake $(Closet); \echo \"purchase \" \$$cake; \done" >> $(Scr)/$(purchase_sh)
	@echo "for milk in \$$($(cd $(Milk_Yogurts))find -type f); do \cp  -r \$$milk $(Fridge); \echo \"purchase \" \$$milk; \done" >> $(Scr)/$(purchase_sh)
endif

#Part with generating script if it need
ifeq (Key,-k) 
	@echo "#!/bin/bash" > $(dev_sh)
	@echo "bash $(Scr)/$(Init_sh)" >> $(dev_sh)
	@echo "bash $(Scr)/$(purchase_sh)" >> $(dev_sh)
	@echo "bash $(Scr)/$(wash_sh)" >> $(dev_sh)
	@echo "bash $(Scr)/$(boil_sh)" >> $(dev_sh)
	@echo "bash $(Scr)/$(fry_sh)" >> $(dev_sh)
	@echo "bash $(Scr)/$(cat_sh)" >> $(dev_sh)
	@echo "bash $(Scr)/$(cooking_sh)" >> $(dev_sh)
	@echo "bash $(Scr)/$(soup_sh) " >> $(dev_sh)
else
	@bash $(Scr)/$(Init_sh)
	@bash $(Scr)/$(purchase_sh)
	@bash $(Scr)/$(wash_sh)
	@bash $(Scr)/$(boil_sh)
	@bash $(Scr)/$(fry_sh)
	@bash $(Scr)/$(cat_sh)
	@bash $(Scr)/$(cooking_sh)
	@bash $(Scr)/$(soup_sh); 
endif

.PHONY: clean
clean:
	rm  -rf $(Home)
	rm  -rf $(Scr)
	rm  -rf $(dev_sh)
	