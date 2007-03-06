#!/bin/bash

CFILES=`ls tests/*.C`
# echo "$CFILES"

echo "Tests:" > test.log
for arg in $CFILES
do
	echo "$arg"
	date >> test.log
	echo "------------------------------------" >> test.log
	echo "--$arg--" >> test.log
	generatePrologTerm $arg > $arg.pl 2> $arg.toprolog.err.log
	cat $arg.pl | termparser > $arg.unparsed.C 2> $arg.torose.err.log
	mv orig_out.txt $arg.rose.C
	diff $arg.rose.C $arg.unparsed.C >> test.log
done

echo "All tests succeeded (terminated)"

mv -f tests/*.log tests/results/
mv -f tests/*.pl tests/results/
mv -f tests/*.unparsed.C tests/results/
mv -f tests/*.rose.C tests/results/
mv -f *.dot tests/results/
mv -f *.pdf tests/results/
	
