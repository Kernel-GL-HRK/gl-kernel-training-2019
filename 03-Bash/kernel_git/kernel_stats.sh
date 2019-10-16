! #bin/bash

run=1

if [[ $1 = "-h" || $1 = "--help" ]]; then
	echo -e "Usage:kernel_stats <options>\n\n"
    echo -e "Print the quantity of files with extention .c, .cpp and *.py."
    echo -e "Find the quantity of commits with word 'revert' in commit name."
    echo -e "How many lines for each author are in *.py files."
	echo -e "\nOptions:\n [dir name] - search folder. (default current dir)"
    echo -e " -h, --help  - print this message"
	run=0
elif [[ $1 = "" ]]; then
	cur_dir=$(pwd)
else
	cur_dir=$1
fi

if [[ $run = 1 ]]; then
	echo $cur_dir

	c_files=$(find $cur_dir -name  "*.c" | wc -l)
	echo "Quantity of files with extention .c = $c_files"

	cpp_files=$(find $cur_dir -name  "*.cpp" | wc -l)
	echo "Quantity of files with extention .cpp = $cpp_files"

	py_files=$(find $cur_dir -name  "*.py" | wc -l)
	echo "Quantity of files with extention .py = $py_files"

	qu_revert=$(cd $cur_dir; git log --oneline --grep="revert" master | wc -l)
	echo "Quantity of commits with word 'revert' in commit name = $qu_revert"

	echo "lines for each author are in *.py files:"
	cd $cur_dir
	find "$cur_dir" -type f -name '*.py' -exec git blame HEAD --line-porcelain {} \; | \
        grep "^author " | sort | uniq -c | tr -s ' ' | cut -d" " --fields=2,4-
fi

