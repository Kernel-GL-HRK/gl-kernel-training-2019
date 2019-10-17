#!/bin/bash




if [[ ! -d $1  ]]
then
	echo "Wrong directory"
	exit
fi

cd $1

echo "Quantity of files with extention .c: $(git ls-files '*.c' | wc -l)"
echo "Quantity of files with extention .cpp: $(git ls-files '*.cpp' | wc -l)"
echo "Quantity of files with extention .py: $(git ls-files '*.py' | wc -l)"

echo "Quantity of commits with word 'revert' in commit name: $(git log --oneline | grep -c revert | wc -l)"

getFilesPy(){
	$(find . -type f | grep '*.py') | while read file
	do
		echo $(git blame --line-porcelain "$file")	
	done
}

sed -n 's/^author //' $(getFilesPy) | sort | uniq -c | sort -r
