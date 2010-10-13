include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_SQLITE_DATABASE

bin_PROGRAMS = testLinearization testScopeMatching

testLinearization_SOURCES = \
	testLinearization.C  

testLinearization_LDADD = $(ROSE_LIBS)

testScopeMatching_SOURCES = \
	testScopeMatching.C  

testScopeMatching_LDADD = $(ROSE_LIBS)

# DQ (3/8/2009): Fixed error: -I. and $(BOOST_CPPFLAGS) is included in $(ROSE_INCLUDES).
# INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.
INCLUDES = $(ROSE_INCLUDES) 

TESTCODES_LINEARIZATION = testLin1.c \
                          testLin2.c


TEST_Linearization = ${TESTCODES_LINEARIZATION:.c=.o}
$(TEST_Linearization): testLinearization $(srcdir)/$(@:.o=.C)
	rm -f $(@:.o=.c).out
	./testLinearization -c $(srcdir)/tests/$(@:.o=.c)
	if [ "`diff $(srcdir)/tests/$(@:.o=.c).out $(@:.o=.c).out | wc -l`" != "0" ]; then exit 1; fi


check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/MacroRewrapper: make check rule start                          ***"
	@echo "************************************************************************************"
	@$(MAKE) $(TEST_Linearization)
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/MacroRewrapper: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 

clean-local:
	rm -rf *.out

endif

EXTRA_DIST =  tests

