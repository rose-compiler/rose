EXTRA_DIST = lb.sh.in loopbounds.pl \
	types.pl while2for.pl validator.pl comefrom.pl

if WITH_SWI_PROLOG
bin_PROGRAMS=loopbounds
loopbounds_SOURCES=

loopbounds: $(builddir)/lb.sh loopbounds.pl
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PL) -q -t main -o loopbounds -c $(srcdir)/loopbounds.pl
	perl -pi -e 's/exec(.*)--.*/satire_driver --run-pointsto-analysis \$$@ --output-term=tmp_satire.term || exit \$$?\nexec \1 <tmp_satire.term/' loopbounds

endif

