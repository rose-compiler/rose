all: rers_mini1.log

%.log: %.hs %.exe
#       these filenames are hardcoded for now
	cp $< formulae.hs
	cp $(patsubst %.hs,%.exe, $<) machine.exe

	ghc -cpp -threaded --make -o qc qc.hs
	./qc # >$@

%.exe: ../tests/%.c
	gcc -O3 $< -o $@

%.hs: ../tests/%.ltl ltl2haskell.sh
	sh ltl2haskell.sh < $< > $@
