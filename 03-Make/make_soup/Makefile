#!/usr/bin/bash

.PHONY: clean

BUILD_DIR=./build_dir
KITCHEN_DIR=$(BUILD_DIR)/kitchen
POT_DIR=$(KITCHEN_DIR)/pot
TABLE_DIR=$(KITCHEN_DIR)/table
MARKET_DIR=$(BUILD_DIR)/market

SCRIPTS_DIR=./scripts
INIT_SH=init.sh
BUY_SH=buy.sh
SLICE_SH=slice.sh
COOK_SH=cook.sh

BUILD_SH=./build.sh


all: init soup
ifeq ($(ARG),-k) 
	echo "#!/bin/sh" > $(BUILD_SH)
	echo "bash $(SCRIPTS_DIR)/$(INIT_SH)" >> $(BUILD_SH)
	echo "bash $(SCRIPTS_DIR)/$(BUY_SH)" >> $(BUILD_SH)
	echo "bash $(SCRIPTS_DIR)/$(SLICE_SH)" >> $(BUILD_SH)
	echo "bash $(SCRIPTS_DIR)/$(COOK_SH)" >> $(BUILD_SH)
else
	bash $(SCRIPTS_DIR)/$(INIT_SH)
	bash $(SCRIPTS_DIR)/$(BUY_SH)
	bash $(SCRIPTS_DIR)/$(SLICE_SH)
	bash $(SCRIPTS_DIR)/$(COOK_SH)
endif

init:
	mkdir -p $(SCRIPTS_DIR)

	echo "#!/bin/bash" > $(SCRIPTS_DIR)/$(INIT_SH)
	echo "mkdir -p $(BUILD_DIR) $(MARKET_DIR) $(KITCHEN_DIR) $(POT_DIR) \
	$(TABLE_DIR)" >> $(SCRIPTS_DIR)/$(INIT_SH)
	echo "touch $(KITCHEN_DIR)/water" >> $(SCRIPTS_DIR)/$(INIT_SH)
	echo "touch $(MARKET_DIR)/potato" >> $(SCRIPTS_DIR)/$(INIT_SH)
	echo "touch $(MARKET_DIR)/carrot" >> $(SCRIPTS_DIR)/$(INIT_SH)

	echo "\
	for (( i=1; i<=100; i++ )); do \
		echo potato_slice_\$$i >> $(MARKET_DIR)/potato ;\
		echo carrot_slice_\$$i >> $(MARKET_DIR)/carrot ;\
	done" >> $(SCRIPTS_DIR)/$(INIT_SH)

# test_init: init
# 	bash $(SCRIPTS_DIR)/$(INIT_SH)

soup: buy_vegetables slice_vegetables
	echo "mv \$$(find $(KITCHEN_DIR) -type f) $(POT_DIR)" > $(SCRIPTS_DIR)/$(COOK_SH)
	echo "rm \$$(find $(POT_DIR) -type f)" >> $(SCRIPTS_DIR)/$(COOK_SH)
	echo "touch $(POT_DIR)/soup" >> $(SCRIPTS_DIR)/$(COOK_SH)

# test_soup: soup
# 	bash $(SCRIPTS_DIR)/$(COOK_SH)

slice_vegetables:
	echo "\
	for file in \$$(find $(TABLE_DIR) -type f); do \
		cat \$$file | while read line; do \
			echo -n '' > $(TABLE_DIR)/\$$line ; \
		done ;\
		rm \$$file ; \
	done" >> $(SCRIPTS_DIR)/$(SLICE_SH)


# test_slice_vegetables: slice_vegetables
# 	bash $(SCRIPTS_DIR)/$(SLICE_SH)

buy_vegetables:
	echo "cp \$$(find $(MARKET_DIR) -type f) $(TABLE_DIR)" > $(SCRIPTS_DIR)/$(BUY_SH)

# test_buy_vegetables: buy_vegetables
# 	bash $(SCRIPTS_DIR)/$(BUY_SH)

clean:
	rm -rf $(SCRIPTS_DIR)
	rm -rf $(BUILD_DIR)
