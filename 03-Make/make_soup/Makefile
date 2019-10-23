TABLE = ./data/kitchen/table
SHOP = ./data/shop
KNIFE = ./data/kitchen/knife
PAN = ./data/kitchen/pan
POT = ./data/kitchen/pot

NOMAKESCRIPT = ./nomake.sh

#cook soup
$(POT)/soup.txt: $(TABLE)/water.txt $(TABLE)/meat.txt  $(KNIFE)/cutPotato.txt $(PAN)/fry.txt $(TABLE)/solt.txt
ifeq ($(ARG), -k) 
	echo "./scripts/cooksoup.sh $(POT)/soup.txt $(TABLE)/water.txt $(TABLE)/meat.txt  $(KNIFE)/cutPotato.txt $(PAN)/fry.txt $(TABLE)/solt.txt" >> $(NOMAKESCRIPT) 
else
	./scripts/cooksoup.sh $@ $(TABLE)/water.txt $(TABLE)/meat.txt  $(KNIFE)/cutPotato.txt $(PAN)/fry.txt $(TABLE)/solt.txt
endif

# fry
$(PAN)/fry.txt: $(KNIFE)/cutCarrot.txt $(KNIFE)/cutOnion.txt
ifeq ($(ARG), -k)
	echo "./scripts/fry.sh $(KNIFE)/cutCarrot.txt $(KNIFE)/cutOnion.txt $@" >> $(NOMAKESCRIPT)
else
	./scripts/fry.sh $(KNIFE)/cutCarrot.txt $(KNIFE)/cutOnion.txt $@
endif

# cut wegetables
$(KNIFE)/cutPotato.txt: $(TABLE)/potato.txt
ifeq ($(ARG), -k)
	echo "./scripts/cutweg.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/cutweg.sh $< $@
endif

$(KNIFE)/cutCarrot.txt: $(TABLE)/carrot.txt
ifeq ($(ARG), -k)
	echo "./scripts/cutweg.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/cutweg.sh $< $@
endif

$(KNIFE)/cutOnion.txt: $(TABLE)/onion.txt
ifeq ($(ARG), -k)
	echo "./scripts/cutweg.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/cutweg.sh $< $@
endif

# buy grocerice
$(TABLE)/potato.txt: $(SHOP)/potato.txt
ifeq ($(ARG), -k)
	echo "./scripts/getFromShop.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/getFromShop.sh $< $@
endif

$(TABLE)/carrot.txt: $(SHOP)/carrot.txt
ifeq ($(ARG), -k)
	echo "./scripts/getFromShop.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/getFromShop.sh $< $@
endif

$(TABLE)/onion.txt: $(SHOP)/onion.txt
ifeq ($(ARG), -k)
	echo "./scripts/getFromShop.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/getFromShop.sh $< $@
endif

$(TABLE)/water.txt: $(SHOP)/water.txt
ifeq ($(ARG), -k)
	echo "#!/bin/bash" > $(NOMAKESCRIPT)
	echo " " >> $(NOMAKESCRIPT)
	echo "./scripts/getFromShop.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/getFromShop.sh $< $@
endif

$(TABLE)/meat.txt: $(SHOP)/meat.txt
ifeq ($(ARG), -k)
	echo "./scripts/getFromShop.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/getFromShop.sh $< $@
endif

$(TABLE)/solt.txt: $(SHOP)/solt.txt
ifeq ($(ARG), -k)
	echo "./scripts/getFromShop.sh $< $@" >> $(NOMAKESCRIPT)
else
	./scripts/getFromShop.sh $< $@
endif

# clean all created files
clean:
	./scripts/deleteProgresFiles.sh
