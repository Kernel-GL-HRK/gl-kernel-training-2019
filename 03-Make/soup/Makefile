#!/usr/bin/make -f

PARAM = ""
PATH_KITCHEN = ./kitchen
RESOURCES = potato.txt carrot.txt onion.txt meat.txt

all: $(PATH_KITCHEN)/table/soup.txt

VPATH = $(PATH_KITCHEN) $(PATH_KITCHEN)/table $(PATH_KITCHEN)/table/pot

FILES = $(addprefix $(PATH_KITCHEN)/table/, $(RESOURCES))
POT = $(addprefix $(PATH_KITCHEN)/table/pot/, pot.txt)
SHELL_SCRIPT = soup_script.sh

create_kitchen_directories:
ifneq ("$(PARAM)", "-k")
	./create_dir.sh $(PATH_KITCHEN)
	./create_dir.sh $(PATH_KITCHEN)/table
	./create_dir.sh $(PATH_KITCHEN)/table/pot
else
	echo "#!/bin/bash" > $(SHELL_SCRIPT)
	echo "./create_dir.sh $(PATH_KITCHEN)" >> $(SHELL_SCRIPT)
	echo "./create_dir.sh $(PATH_KITCHEN)/table" >> $(SHELL_SCRIPT)
	echo "./create_dir.sh $(PATH_KITCHEN)/table/pot" >> $(SHELL_SCRIPT)
endif

potato.txt: create_kitchen_directories
ifneq ("$(PARAM)", "-k")
	echo "Take 500 g. potates" > $(PATH_KITCHEN)/table/$@
else
	echo "echo \"Take 500 g. potates\" > $(PATH_KITCHEN)/table/$@" >> $(SHELL_SCRIPT)
endif

carrot.txt: create_kitchen_directories
ifneq ("$(PARAM)", "-k")
	echo "Take 200 g. carrots" > $(PATH_KITCHEN)/table/$@
else
	echo "echo \"Take 200 g. carrots\" > $(PATH_KITCHEN)/table/$@" >> $(SHELL_SCRIPT)
endif

onion.txt: create_kitchen_directories
ifneq ("$(PARAM)", "-k")
	echo "Take 100 g. onions" > $(PATH_KITCHEN)/table/$@
else
	echo "echo \"Take 100 g. onions\" > $(PATH_KITCHEN)/table/$@" >> $(SHELL_SCRIPT)
endif

meat.txt: create_kitchen_directories
ifneq ("$(PARAM)", "-k")
	echo "Take 500 g. meat" > $(PATH_KITCHEN)/table/$@
else
	echo "echo \"Take 500 g. meat\" > $(PATH_KITCHEN)/table/$@" >> $(SHELL_SCRIPT)
endif

pot: create_kitchen_directories
ifneq ("$(PARAM)", "-k")
	./prepare_pot.sh 2000ml $(POT)
else
	echo "./prepare_pot.sh 2000ml $(POT)" >> $(SHELL_SCRIPT)
endif

$(PATH_KITCHEN)/table/soup.txt: potato.txt carrot.txt onion.txt meat.txt pot
ifneq ("$(PARAM)", "-k")
	cat $(FILES) $(POT) > $@
	./prepare_soup.sh $@
else
	echo "cat $(FILES) $(POT) > $@" >> $(SHELL_SCRIPT)
	echo "./prepare_soup.sh $@" >> $(SHELL_SCRIPT)
endif

clean:
	./delete_dir.sh $(PATH_KITCHEN)
	rm -f $(SHELL_SCRIPT)
