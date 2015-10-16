#!/usr/bin/sh
cat /dev/null > info
types=("Makefile" "*.asm" "*.c" "*.h")
for i in ${types[@]}
do
    find . -iname $i > tmp
    cat tmp >> info
done
sed -i 's/ /\\&/g' info
cat info | xargs wc -l
rm info tmp
