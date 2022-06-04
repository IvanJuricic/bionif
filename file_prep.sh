#!/bin/bash
file='one_seq_copy'
tail -n +2 "$file" > "$file.tmp" && mv "$file.tmp" "$file"
echo -n $(tr -d "\n" < "$file") > "$file"
#echo "$file"
