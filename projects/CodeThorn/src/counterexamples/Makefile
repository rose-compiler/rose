all:
	# $(MAKE) clean && $(MAKE) -j1 Problem1.log
	# $(MAKE) clean && $(MAKE) -j1 Problem2.log
	# $(MAKE) clean && $(MAKE) -j1 Problem3.log
	# $(MAKE) clean && $(MAKE) -j1 Problem4.log
	# $(MAKE) clean && $(MAKE) -j1 Problem5.log
	# $(MAKE) clean && $(MAKE) -j1 Problem6.log
	# $(MAKE) clean && $(MAKE) -j1 Problem7.log
	# $(MAKE) clean && $(MAKE) -j1 Problem8.log
	# $(MAKE) clean && $(MAKE) -j1 Problem9.log
	# $(MAKE) clean && $(MAKE) -j1 Problem10.log
	# $(MAKE) clean && $(MAKE) -j1 Problem11.log
	# $(MAKE) clean && $(MAKE) -j1 Problem12.log
	# $(MAKE) clean && $(MAKE) -j1 Problem13.log
	# $(MAKE) clean && $(MAKE) -j1 Problem14.log
	$(MAKE) clean && $(MAKE) -j1 Problem15.log
	$(MAKE) clean && $(MAKE) -j1 Problem16.log
	$(MAKE) clean && $(MAKE) -j1 Problem17.log
	$(MAKE) clean && $(MAKE) -j1 Problem18.log
	$(MAKE) clean && $(MAKE) -j1 Problem19.log

csv:
	for i in `seq 19`; do awk -f log2csv.awk <Problem$$i.log >Problem$$i-ltl-qc.csv; done

RERS=$(patsubst %,Problem%.log, $(shell seq 13))
rers: $(RERS)

clean:
	rm -f *.exe formulae.hs *.o *.hi qc

%.log: %.hs %.exe
#       these filenames are hardcoded for now
	mv $< formulae.hs
	mv $(patsubst %.hs,%.exe, $<) machine.exe

	ghc -cpp -threaded --make -o qc qc.hs
	./qc >$@

%.exe: ../tests/%.c
	gcc -O3 $< -o $@
	sh ltl2haskell.sh < $(patsubst %.c,%.ltl,$<) >$(patsubst %.exe,%.hs,$@)

%.exe %.hs: ../tests/rers/%.c
	 # patch the source code to collect data about assert reachability
	echo "#include <stdlib.h>" >_tmp.c
	F=$(patsubst ../tests/rers/%.c,%,$<) && \
	cat $< | sed 's/error_\([0-9]\+\): assert(0);/FILE* f=fopen("'$$F'-assert.csv","aw");fprintf(f,"%d,yes,9\\n",\1);fclose(f); error_\1: assert(0);/g' >>_tmp.c
	gcc -O3 _tmp.c -o $@
	rm _tmp.c
	sh ltl2haskell.sh \
          < $(patsubst ../tests/rers/Problem%.c,../tests/rers/properties%.txt,$<) \
          > $(patsubst %.exe,%.hs,$@)
