EXTRA_DIST = gencode.pl types.pl

if WITH_SWI_PROLOG

bin_PROGRAMS=annotcheck
gencode_SOURCES=

annotcheck: gencode.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o annotcheck -c $(srcdir)/gencode.pl

endif
