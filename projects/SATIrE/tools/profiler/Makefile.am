EXTRA_DIST = frequency.pl types.pl visicfg.pl

if WITH_SWI_PROLOG

bin_PROGRAMS=frequency
frequency_SOURCES=

frequency: frequency.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o frequency -c $(srcdir)/frequency.pl

endif
