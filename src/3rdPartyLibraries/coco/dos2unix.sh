#!/bin/sh
# Script to delete DOS RETURN caracter (\r) from source files

for i in $1 $2 $3 $4 $5 $6 $7 $8 $9
do
   if test -f $i
   then
     echo $i
     echo "" >> $i
     cat $i | sed 's///g' > /tmp/dos2unix
     \cp /tmp/dos2unix $i
     \rm -f /tmp/dos2unix
   fi
done



