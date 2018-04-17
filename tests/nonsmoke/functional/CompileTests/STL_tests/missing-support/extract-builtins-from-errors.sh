builtin out.txt | sed 's/return//g'| tr '{}' '   ' | sed 's/      //' | sed 's/  //g' | sort -u > missing-builtin-functions.txt
