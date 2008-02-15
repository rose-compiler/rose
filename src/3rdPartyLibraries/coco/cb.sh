#!/bin/sh

for i in $1 $2 $3 $4 $5 $6 $7 $8 $9
do
   if test -f $i
   then
     echo $i
     echo "" >> $i
     cb $i > /tmp/dos2unix
     \cp /tmp/dos2unix $i
     \rm -f /tmp/dos2unix
   fi
done
