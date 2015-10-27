include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

CXXLD = $(GCJ) # ensure Java stuff links correctly

AM_CPPFLAGS = -I$(top_builddir)/projects/FiniteStateModelChecker/MopsForROSE $(ROSE_INCLUDES)

bin_PROGRAMS = mopscc1 mopsCheck

COMMON_SOURCE_FILES = genCfg.C

mopscc1_SOURCES   = cc1.C $(COMMON_SOURCE_FILES)
mopsCheck_SOURCES = check.C $(COMMON_SOURCE_FILES)

noinst_HEADERS = genCfg.h

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -L$(top_builddir)/projects/FiniteStateModelChecker/MopsForROSE -lMopsForROSE

TEST1_FILES = eg1/exec.fsa eg1/setuidexec.mfsa eg1/setuid.fsa eg1/test.c 

test1: mopsCheck $(patsubst %,$(srcdir)/%,$(TEST1_FILES))
	cd $(srcdir)/eg1 && $(shell pwd)/mopsCheck setuidexec.mfsa test.c > mops_collected_output.out; \
   grep "The program" mops_collected_output.out

TEST2_FILES = eg2/semaphore.fsa eg2/sem.C eg2/sem.mfsa

# The following test case will not work until the expression-granularity CFG is in place
test2: mopsCheck $(patsubst %,$(srcdir)/%,$(TEST2_FILES))
	cd $(srcdir)/eg2 && $(shell pwd)/mopsCheck sem.mfsa sem.C

check-local:
	@$(MAKE) test1
	@echo "*********************************************************************************************"
	@echo "*** ROSE/projects/FiniteStateModelChecker: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************"

EXTRA_DIST = $(TEST1_FILES) $(TEST2_FILES)
