#!/bin/bash
file='sequence'
output='prepared_one_seq'

echo "Preparing original file....."
tail -n +2 "$file" > "$output"
echo -n $(tr -d "\n" < "$output") > "$output"

gcc -o test main.c lib.c
gcc -o generate generator.c

if [ -s "test_seq_10" ]
then
    echo ""
else
    echo "Generating test files....."
    ./generate $output
fi

for i in 10 20 50 100 1000 10000
do
    #echo "aa $i"
    ./test $output $i
done
