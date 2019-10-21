#!/usr/bin/make 
#--------------------------------------------------
#Inialisation Home
Home=./home

#Inialisation tools for Soup event
Kitchen="$(Home)/kitchen"
Fridge=$(Kitchen)/fridge
Closet=$(Kitchen)/closet
Kitchen_table=$(Kitchen)/table
Knife=$(Kitchen_table)/knife
Chairs=$(Kitchen_table)/chairs
Stove=$(Kitchen)/stove
Box=$(closet)/box
Slow_cooker=$(kitchen)/slow_cooker
Sink=$(kitchen)/sink

#Inialisation Bedroom
Bedroom=$(Home)/bedroom

#Inialisation Restroom
Restroom=$(Home)/restroom

#Inialisation Dinning room
Dinning_room=$(Home)/dinning_room

#Inialisation tools for dinner
Сutlery=$(Dinning_room)/cutlery
Chairs_Dining_room=$(Dinning_room)/chairs_dining_room
Dinning_table=$(Dinning_room)/table

#Main locations for Soup event
Food_Market=./food_market
Meat=$(Food_Market)/meat
Pickled_Vagetebl=$(Food_Market)/pickled_vagetebl
Spice=$(Food_Market)/spice
Vegetable=$(Food_Market)/vegetable

#Additional locations from Wife
AnimalFood=$(Food_Market)/animal_food
Candies_Cakes=$(Food_Market)/candies_cakes
Milk_Yogurts=$(Food_Market)/milk_yogurts

.PHONY: clean
clean:
	rm  -rf $(Home)
	rm  -rf $(Food_Market)
