include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) ../../normalizations/libBackstrokeNormalizations.la

bin_PROGRAMS = expNormTest 
expNormTest_SOURCES = main.C

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES =

TEST_Objects=test_norm.C 

$(TEST_Objects):expNormTest 
	@cp $(srcdir)/$(@:.o=.C) $(@:.o=.C)
	./expNormTest -c $(@:.o=.C)      		

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

mytest:
	@$(MAKE) $(TEST_Objects)
