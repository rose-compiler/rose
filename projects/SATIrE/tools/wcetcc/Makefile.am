EXTRA_DIST = wcetcc.pl types.pl

if WITH_SWI_PROLOG

bin_PROGRAMS=wcetcc
wcetcc_SOURCES=

wcetcc: wcetcc.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o wcetcc -c $(srcdir)/wcetcc.pl

endif
