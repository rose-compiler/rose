all: testIntervalLattice

testIntervalLattice: ../GenericIntervalLattice.h ../NumberIntervalLattice.h ../NumberIntervalLattice.C testIntervalLattice.C
	g++ -DCT_IGNORE_COLORS_BOOLOPTIONS testIntervalLattice.C ../AType.C ../Miscellaneous.C ../NumberIntervalLattice.C -o testIntervalLattice -I${BOOST_HOME}/include

clean:
	rm -f testIntervalLattice

check: testIntervalLattice
	./testIntervalLattice
