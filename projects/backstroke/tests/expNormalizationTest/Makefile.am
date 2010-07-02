include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

# This file does not compile using ROSE (skip testing it for now)
bin_PROGRAMS = expNormTest 
expNormTest_SOURCES = main.C

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -L../../libBackstroke.la

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES =

TEST_Objects=test_norm.C 

$(TEST_Objects):expNormTest 
	@cp $(srcdir)/$(@:.o=.C) $(@:.o=.C)
	./norm -c $(@:.o=.C)      		

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

mytest:
	@$(MAKE) $(TEST_Objects)
