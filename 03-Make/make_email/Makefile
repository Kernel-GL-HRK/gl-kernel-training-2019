#!/usr/bin/make -f 

# Author: Yevgen Kovalyov ekovalyov@gmail.com
# This Makefile generates e-mail letter from scratch

#VARIABLES

NAME = mr. Boss
SIGNER = mr. Signer
DATE := `LC_ALL=en_EN.utf8 date`
#$(eval export DATE=$$(sh -c "LC_ALL=en_EN.utf8 date"))

# RULES:
# 1st rule is executed by default
letter_signed.txt: letter.txt
	cp $< $@
	echo "Signed by ${SIGNER} ${DATE}" >> $@

header_template.txt:
	echo "Dear %%NAME%%," >> $@

header.txt: header_template.txt
	sed "-es/%%NAME%%/${NAME}/" <$< >$@

body.txt:
	echo "Here is the letter provided for the testing purposes" > $@

footer.txt:
	echo " best regards," > $@
	echo " mr. Sender" >> $@

letter.txt: header.txt body.txt footer.txt
	cat $^ >$@

clean:
	rm *.txt
