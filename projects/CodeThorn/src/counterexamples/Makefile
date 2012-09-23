all: rers_mini1.log Problem1.log

clean:
	rm -f *.exe formulae.hs *.o *.hi qc

%.log: %.hs %.exe
#       these filenames are hardcoded for now
	cp $< formulae.hs
	cp $(patsubst %.hs,%.exe, $<) machine.exe

	ghc -cpp -threaded --make -o qc qc.hs
	./qc # >$@

%.exe: ../tests/%.c
	gcc -O3 $< -o $@
	sh ltl2haskell.sh < $(patsubst %.c,%.ltl,$<) >$(patsubst %.exe,%.hs,$@)

%.exe %.hs: ../tests/rers/%.c
	gcc -O3 $< -o $@
	sh ltl2haskell.sh \
          < $(patsubst ../tests/rers/Problem%.c,../tests/rers/properties%.txt,$<) \
          > $(patsubst %.exe,%.hs,$@)
