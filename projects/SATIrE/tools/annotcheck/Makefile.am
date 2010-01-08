EXTRA_DIST = gencode.pl

if WITH_SWI_PROLOG

bin_PROGRAMS=annotcheck
annotcheck_SOURCES=

annotcheck: gencode.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o annotcheck -c $(srcdir)/gencode.pl

endif
