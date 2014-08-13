all: Problem1.exe Problem1.log

#	$(shell for i in `seq 14`; do $(MAKE) clean && $(MAKE) -j1 Problem$$i.log)

csv:
	for i in `seq 19`; do \
	  awk -f log2csv.awk <Problem$$i.log >Problem$$i-ltl-qc.csv; \
          echo "merge exisiting and newly found results: $$i"; \
	  touch Problem$$i-assert.csv; \
	  touch Problem$$i-qc.csv; \
	  cat Problem$$i-ltl-qc.csv Problem$$i-assert.csv Problem$$i-qc.csv \
              | sort -n | uniq >tmp.csv ; \
	  mv tmp.csv Problem$$i-qc.csv ; \
        done

#cp Problem$$i-qc.csv ../regressiondata/ ; MS: we don't copy over the old data anymore

RERS=$(patsubst %,Problem%.log, $(shell seq 54))
ISP=../tests/rers/inputsequencepack
rers: $(RERS)

clean:
	rm -f *.exe formulae.hs *.o *.hi qc

%.log: %.exe qc.hs
	cp qc.hs qc-$<.hs
	ghc -O3 -DLTL_FILE=\"$(patsubst %.exe,%.hs,$<)\" -cpp -threaded --make -o qc-$< qc-$<.hs
	./qc-$< >$@

%.exe: $(ISP)/%_pmod.c qc.hs
	gcc -Wno-return-type -O3 $< -o $@
	sh ltl2haskell.sh \
            $(patsubst $(ISP)/Problem%_pmod.c,../tests/rers/properties%.txt,$<) \
          > $(patsubst %.exe,%.hs,$@)
	sh isequence2haskell.sh \
            $(patsubst $(ISP)/Problem%_pmod.c,$(ISP)/Problem%.isequence.txt,$<) $(patsubst %.exe,%.inputs,$@) \
          >>$(patsubst %.exe,%.hs,$@)
