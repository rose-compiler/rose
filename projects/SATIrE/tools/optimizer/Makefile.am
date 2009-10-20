EXTRA_DIST = unroll.pl

if WITH_SWI_PROLOG

bin_PROGRAMS=unroll
unroll_SOURCES=

unroll: unroll.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o unroll -c $(srcdir)/unroll.pl

endif
