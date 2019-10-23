#!/usr/bin/make -f


soup: slice_meat slice_patato add_water
	cat kitchen/table/meat_sliced.txt >> kitchen/table/pot/soup.txt
	cat kitchen/table/potato_sliced.txt >> kitchen/table/pot/soup.txt
	sed -i -e 's/cold/boiled/g' kitchen/table/pot/soup.txt

check_kitchen:	
	./check_dirs.sh kitchen

check_table: check_kitchen	
	./check_dirs.sh kitchen/table	

check_pot: check_kitchen check_table
	./check_dirs.sh kitchen/table/pot

add_water: check_pot		
	./get_water_from_faucet.sh 2000 kitchen/table/pot/soup.txt

slice_meat: prep_meat check_table
	mv kitchen/meat.txt kitchen/table/meat_sliced.txt

prep_meat: check_kitchen
	echo "200g meat" >> kitchen/meat.txt

slice_patato: prep_patato check_table
	mv kitchen/potato.txt kitchen/table/potato_sliced.txt
	
prep_patato: check_kitchen
	echo "500g potato" >> kitchen/potato.txt

clean:
	find kitchen -type f -name "*.txt" -delete
