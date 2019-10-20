#!/usr/bin/make -f

NOMAKE=nomake.sh

PRODUCTS = grocery/products

TABLE = kitchen/table
STOVE = kitchen/stove

SOUP = soup.txt

RIBS = $(PRODUCTS)/smoked_pork_ribs.txt
PEA = $(PRODUCTS)/pea.txt
POTATO = $(PRODUCTS)/potato.txt
ONION = $(PRODUCTS)/onion.txt
CARROT = $(PRODUCTS)/carrot.txt
OIL = $(PRODUCTS)/oil.txt
SALT = $(PRODUCTS)/salt.txt
PEPPER = $(PRODUCTS)/pepper.txt
BAY_LEAF = $(PRODUCTS)/bay_leaf.txt

TITLE = misc/title.sh
ACTIONS_TITLE = misc/actions_title.sh

PRODUCTS_LIST = grocery/products_list

DUNK_PEA = $(TABLE)/dunk_pea
BOUILLON = $(STOVE)/bouillon
ADD_PEA = $(STOVE)/add_pea
SLICE_POTATO = $(TABLE)/slice_potato
SLICE_ONION_AND_CARROT = $(TABLE)/slice_onion_and_carrot
FRY_ONION_AND_CARROT = $(STOVE)/fry_onion_and_carrot
ADD_SPICE = $(STOVE)/add_spice

STEPS = $(PRODUCTS_LIST).res \
	$(DUNK_PEA).res \
	$(BOUILLON).res \
	$(ADD_PEA).res \
	$(SLICE_POTATO).res \
	$(SLICE_ONION_AND_CARROT).res \
	$(FRY_ONION_AND_CARROT).res \
	$(ADD_SPICE).res

$(SOUP): $(STEPS)
ifneq ($(ARGS), -k)
	@$(TITLE) >> $@
	@cat $(PRODUCTS_LIST).res >> $@
	@$(ACTIONS_TITLE) >> $@
	@cat $(DUNK_PEA).res >> $@
	@cat $(BOUILLON).res >> $@
	@cat $(ADD_PEA).res >> $@
	@cat $(SLICE_POTATO).res >> $@
	@cat $(SLICE_ONION_AND_CARROT).res >> $@
	@cat $(FRY_ONION_AND_CARROT).res >> $@
	@cat $(ADD_SPICE).res >> $@
else
	@misc/create_no_make_script.sh
endif

$(PRODUCTS_LIST).res: $(PRODUCTS_LIST).sh $(PEA) $(RIBS) $(POTATO) $(ONION) \
	$(CARROT) $(OIL) $(SALT) $(PEPPER) $(BAY_LEAF)
ifneq ($(ARGS), -k)
	@$(PRODUCTS_LIST).sh > $@
endif

$(DUNK_PEA).res: $(DUNK_PEA).sh $(PEA)
ifneq ($(ARGS), -k)
	@$(DUNK_PEA).sh > $@
endif

$(BOUILLON).res: $(BOUILLON).sh $(ONION) $(RIBS)
ifneq ($(ARGS), -k)
	@$(BOUILLON).sh > $@
endif

$(ADD_PEA).res: $(ADD_PEA).sh $(PEA)
ifneq ($(ARGS), -k)
	@$(ADD_PEA).sh > $@
endif

$(SLICE_POTATO).res: $(SLICE_POTATO).sh $(POTATO)
ifneq ($(ARGS), -k)
	@$(SLICE_POTATO).sh > $@
endif

$(SLICE_ONION_AND_CARROT).res: $(SLICE_ONION_AND_CARROT).sh $(ONION) $(CARROT)
ifneq ($(ARGS), -k)
	@$(SLICE_ONION_AND_CARROT).sh > $@
endif

$(FRY_ONION_AND_CARROT).res: $(FRY_ONION_AND_CARROT).sh $(ONION) $(CARROT) \
	$(OIL)
ifneq ($(ARGS), -k)
	@$(FRY_ONION_AND_CARROT).sh > $@
endif

$(ADD_SPICE).res: $(ADD_SPICE).sh $(SALT) $(PEPPER) $(BAY_LEAF)
ifneq ($(ARGS), -k)
	@$(ADD_SPICE).sh > $@
endif

clean:
	@rm -f $(SOUP) $(PRODUCTS_LIST).res $(TABLE)/*.res $(STOVE)/*.res $(NOMAKE)
