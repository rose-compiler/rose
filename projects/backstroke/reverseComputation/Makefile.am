include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = eventDetection

# This file does not compile using ROSE (skip testing it for now)
if !ROSE_USING_ROSE_ANALYSIS
   bin_PROGRAMS = reverse
   reverse_SOURCES = reverse.cpp facilityBuilder.C
endif

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = reverse.h facilityBuilder.h test

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES =

TEST_Objects=test0.c test1.c test2.c test3.c 

$(TEST_Objects): reverse
	@cp $(srcdir)/test/$(@:.o=.c) $(@:.o=.c)
	./reverse -I$(srcdir)/test -c $(@:.o=.c) -rose:C99 -DROSE_LANGUAGE_MODE=0       		
	@g++ -I$(srcdir)/test rose_$(@:.o=.c) -o $(@:.o=.c).o -w
	./$(@:.o=.c).o		

check-local:
#@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
