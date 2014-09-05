all: testIntervalLattice

testIntervalLattice: ../IntervalLattice.h testIntervalLattice.C
	g++ -DCT_IGNORE_COLORS_BOOLOPTIONS testIntervalLattice.C ../AType.C ../Miscellaneous.C -o testIntervalLattice

clean:
	rm -f testIntervalLattice

check: testIntervalLattice
	./testIntervalLattice
