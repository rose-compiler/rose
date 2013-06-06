#!/bin/sh
set -v
rm -f param2
rm -f lsh-clone-results
rm -f clone-list

LSH_EXEC=~/REPOSITORY-SRC/Deckard/src/lsh/bin/enumBuckets

${LSH_EXEC} -R 0.7 -M 10000000000 -b 2 -A -f `ls --sort=size vdb_* | head -1` -p param2 
for i in vdb_*; do
	echo $i
        #The num vectors paramater is ignored by the lsh implementation. So we can use the same paramater file all over
	${LSH_EXEC} -R 0.7 -M 10000000000 -b 2 -A -f $i -p param2 > clones-$i
	sed -n '/Mean query time:/,$d; 11,$p' < clones-$i >> clone-list
	grep -H 'buckets.*points' clones-$i >> lsh-clone-results
	rm clones-$i
done

