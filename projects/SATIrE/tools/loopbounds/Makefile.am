EXTRA_DIST = loopbounds.pl \
	types.pl while2for.pl validator.pl comefrom.pl

if WITH_SWI_PROLOG

bin_PROGRAMS=loopbounds
loopbounds_SOURCES=

DRIVER=$(bindir)/satire_driver --run-pointsto-analysis --analysis=interval --no-analysis-annotation --gdl-nodeformat=varid \$$@ --output-term=tmp_satire.term

loopbounds: loopbounds.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o loopbounds -c $(srcdir)/loopbounds.pl
	perl -pi -e 's!exec(.*)--.*!$(DRIVER)>/dev/null || $(DRIVER) && exec \1 <tmp_satire.term ; exit 1!' loopbounds

endif
