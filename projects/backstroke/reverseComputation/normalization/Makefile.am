include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

# This file does not compile using ROSE (skip testing it for now)
bin_PROGRAMS = norm 
norm_SOURCES = main.C

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES =

TEST_Objects=test_norm.C 

$(TEST_Objects):norm 
	@cp $(srcdir)/test/$(@:.o=.C) $(@:.o=.C)
	./norm -I$(srcdir)/test -c $(@:.o=.C)      		

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

mytest:clean
	@$(MAKE) $(TEST_Objects)
