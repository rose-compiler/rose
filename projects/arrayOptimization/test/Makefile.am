
EXTRA_DIST = funcs.annot  interp1.C  redBlack1.C rose_redBlack1.C.save  rose_redBlack1_split.C.save \
             TestDriver floatArray.annot  Index.annot  rose_interp1.C.save  simpleA++.h

# to deactivate tests comment out check-local.
check-local:
	@echo "SKIPPING TEST CODES RUN BY \"make check\" rule in projects/arrayOptimization/test directory ..."
#	DQ (11/8/2008): Commented out as requested by Liao.
#	$(MAKE) FORCE_TEST_CODES_TO_RUN

FORCE_TEST_CODES_TO_RUN:
	$(srcdir)/TestDriver $(srcdir) $(top_builddir)/projects/arrayOptimization

