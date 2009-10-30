if WITH_SWI_PROLOG

bin_PROGRAMS=unroll annot-unweave annot-weave annot-transform

unroll_SOURCES=unroll.pl
unroll: unroll.pl

annot_unweave_SOURCES=annot-unweave.pl
annot-unweave: annot-unweave.pl

annot_transform_SOURCES=annot-transform.pl
annot-transform: annot-transform.pl

annot_weave_SOURCES=annot-weave.pl
annot-weave: annot-weave.pl

%: %.pl Makefile
	TERMITE_LIB=$(top_srcdir)/src/termite/termite \
	  $(SWI_PLC) -o $@ -c $<

endif
