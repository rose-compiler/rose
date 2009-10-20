EXTRA_DIST = loopbounds.pl \
	types.pl while2for.pl validator.pl comefrom.pl

if WITH_SWI_PROLOG

bin_PROGRAMS=loopbounds
loopbounds_SOURCES=

##
# We need to inject this wrapper into the SWI-Prolog excutable image:
##
USAGE="Usage: \$$0 file1.c ...\n  Perform loop-bound analysis on the input files.\n\nOutput: a Termite file that can be piped to unparse or term2c\n        to create annotated sources.\n\nPlease report bugs to adrian\@complang.tuwien.ac.at\n"

ARGCHECK=if [ \$$\# -eq 0 ]; then echo $(USAGE); exit 1; fi
DRIVER=$(bindir)/satire_driver --run-pointsto-analysis --analysis=interval --no-analysis-annotation --gdl-nodeformat=varid --callstringlength=2 \$$@ --output-term=tmp_satire.term

loopbounds: loopbounds.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o loopbounds -c $(srcdir)/loopbounds.pl
	perl -pi -e 's!exec(.*)--.*!$(ARGCHECK); $(DRIVER)>/dev/null || $(DRIVER) && exec \1 <tmp_satire.term ; exit 1!' loopbounds

endif
