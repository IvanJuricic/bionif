#!/bin/bash
file='one_seq'

#echo "Preparing original file.....\n"
#tail -n +2 "$file" | sponge "$file"
#echo -n $(tr -d "\n" < "$file") > "$file"

gcc -o test main.c lib.c
gcc -o generate generator.c

echo "Generating test files....."
#./generate $file

for i in 10 20 50 100 1000
do
    #echo "aa $i"
    ./test $file $i
done
