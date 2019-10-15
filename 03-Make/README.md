# Make

- [Make manual](https://www.gnu.org/software/make/manual/html_node/index.html)

## Examples
Examples are from Lectures notes:
* [make_email](make_email) - in this example we are making signed letter from parts;
* [make_simplest](make_simplest) - example of creating simple C program using gcc.

## Homework

0. Fix logical error in project `make_simplest`

1. Create makefile which contains steps for cooking soup.  
The rules of makefile should contains action for moving files
of changing their content in different folders like
```
/kitchen
  /table
    /pot
    /spoon
  /knife
/dinning-room
/food-market
```
The resources should be in files. (e.g. `potato.txt`, `carrot.txt`, `water.txt`)

2. The cooking process should change file location or generate new files.

3. Some or all action can be implemented using `*.sh` files like
```
get_water_from_faucet.sh:
	echo "water,cold,$1ml" >> $2
```
In rules we can use scipt:
```
add_water: kitchen/table/pot
	get_water_from_faucet.sh 2000 ./kitchen/table/pot
```

4. Target clean should remove all generated resources.

5. Running makefile with option `-k` should generated script which can be used without make.
