#!/bin/bash

if [[ ! -n $1 ]]; then
    echo "Error: missing git path. $0 [path repository]"
    exit 2
fi

cd "$1"

if [[ ! -n "$(git branch)" ]]; then
    echo "Error: missing git repository"
    exit 2
fi

echo "Numb file *.c:   $(find . -name "*.c" -type f | wc -l)"
echo "Numb file *.cpp: $(find . -name "*.cpp" -type f | wc -l)"
echo "Numb file *.py:  $(find . -name "*.py" -type f | wc -l)"
echo "Numb revert: $(git log --oneline | grep rever | wc -l)"

list_author="$(mktemp ./pwd.XXXXXX)"

for file_py in $(find . -type f | grep '\.py' ); do
    echo "file_py: $file_py"
    git blame --line-porcelain "$file_py" >>  $list_author
done

sed -n 's/^author //p' "$list_author"| sort | uniq -c | sort -rn

rm "$list_author"
exit 0
