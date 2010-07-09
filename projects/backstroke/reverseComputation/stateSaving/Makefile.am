include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# This file does not compile using ROSE (skip testing it for now)
#bin_PROGRAMS = stateSaving
stateSaving_SOURCES = main.C stateSaver.C  ../facilityBuilder.C

INCLUDES = $(ROSE_INCLUDES) ../
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = stateSaver.h  ../facilityBuilder.h

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES =

TEST_Objects=test0.C 

$(TEST_Objects):stateSaving 
	@cp $(srcdir)/test/$(@:.o=.C) $(@:.o=.C)
	./stateSaving -I$(srcdir)/test -c $(@:.o=.C)      		
	@g++ -I$(srcdir)/../test rose_$(@:.o=.C) -o $(@:.o=.C).o -w
	./$(@:.o=.C).o		

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

mytest:
	@$(MAKE) $(TEST_Objects)
