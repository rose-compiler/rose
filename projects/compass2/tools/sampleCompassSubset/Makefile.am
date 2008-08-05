#################################################################################
# This is a sample of how one would create a subset of compass as a separate 
# tool
#
################################################################################

include $(top_srcdir)/projects/compass2/src/compassSupport/compass.inc

bin_PROGRAMS = sampleCompassSubset

sampleCompassSubset_SOURCES = compassMain.C checkers.h
sampleCompassSubset_LDADD = -lCompassCheckers $(ROSE_LIBS)

sampleCompassSubset_DEPENDENCIES = $(compass_support_bdir)/libCompass.la $(compass_detector_libs) libCompassCheckers.la

test: sampleCompassSubset compass_parameters $(compass_test_dir)/exampleTest_1.C
	env COMPASS_PARAMETERS=./compass_parameters ./sampleCompassSubset $(compass_test_dir)/exampleTest_1.C 

docs:
	@echo "Write your documentation rule here"

check-local:
	@$(MAKE) test
	@echo "*****************************************************************************"
	@echo "*** sampleCompassSubset: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************" 

EXTRA_DIST = CHECKER_LIST RULE_SELECTION

clean-local:
	rm -f compass_parameters *.ti *.lo *.la

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_parameters buildCheckers.C checkers.h compass_makefile.inc

################################################################################
