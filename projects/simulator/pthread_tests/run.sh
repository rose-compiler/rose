cat $1 | sed s/'\r'//g > out.txt
cp out.txt $1
