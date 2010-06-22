include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = stateSaving

# This file does not compile using ROSE (skip testing it for now)
if !ROSE_USING_ROSE_ANALYSIS
   bin_PROGRAMS = reverse 
   reverse_SOURCES = eventReverser.C expAndStmtProcessor.C facilityBuilder.C utilities.C
endif

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = eventReverser.h facilityBuilder.h utilities.h test

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES =

TEST_Objects=test0.C test1.C test2.C test3.C 

$(TEST_Objects): reverse
	@cp $(srcdir)/test/$(@:.o=.C) $(@:.o=.C)
#./reverse -I$(srcdir)/test -c $(@:.o=.C) -DROSE_LANGUAGE_MODE=0       		
	./reverse -I$(srcdir)/test -c $(@:.o=.C)      		
	@g++ -I$(srcdir)/test rose_$(@:.o=.C) -o $(@:.o=.C).o -w
	./$(@:.o=.C).o		

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

mytest:
	@$(MAKE) $(TEST_Objects)
