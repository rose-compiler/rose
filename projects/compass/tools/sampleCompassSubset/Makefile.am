#################################################################################
# This is a sample of how one would create a subset of compass as a separate 
# tool
#
################################################################################

include $(top_srcdir)/projects/compass/src/compassSupport/compass.inc

BUILT_SOURCES = buildCheckers.C checkers.h

lib_LTLIBRARIES = libSampleCompassSubsetCheckers.la
libSampleCompassSubsetCheckers_la_SOURCES = buildCheckers.C checkers.h
libSampleCompassSubsetCheckers_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

libSampleCompassSubsetCheckers.la: $(libSampleCompassSubsetCheckers_la_OBJECTS) $(libSampleCompassSubsetCheckers_la_DEPENDENCIES) $(COMPASS_CHECKER_OBJS)
	$(CXXLINK) -rpath $(libdir) $(libSampleCompassSubsetCheckers_la_LDFLAGS) $(libSampleCompassSubsetCheckers_la_OBJECTS) $(COMPASS_CHECKER_OBJS) $(libSampleCompassSubsetCheckers_la_LIBADD) $(LIBS)

bin_PROGRAMS = sampleCompassSubset

sampleCompassSubset_SOURCES = compassMain.C checkers.h
sampleCompassSubset_LDADD = -lSampleCompassSubsetCheckers $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

sampleCompassSubset_DEPENDENCIES = $(compass_support_bdir)/libCompass.la $(compass_detector_libs) libSampleCompassSubsetCheckers.la

test: sampleCompassSubset compass_parameters $(compass_test_dir)/exampleTest_1.C
	env COMPASS_PARAMETERS=./compass_parameters ./sampleCompassSubset -rose:skip_unparser -rose:skipfinalCompileStep $(compass_test_dir)/exampleTest_1.C 

docs:
	@echo "Write your documentation rule here"

check-local:
# DQ (9/10/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
	@echo "***********************************"
	@echo "*** Testing sampleCompassSubset ***"
	@echo "***********************************"
if !OS_VENDOR_DEBIAN
	@$(MAKE) test
endif
	@echo "*****************************************************************************"
	@echo "*** sampleCompassSubset: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************" 

EXTRA_DIST = CHECKER_LIST RULE_SELECTION.in

clean-local:
	rm -f compass_parameters *.ti *.lo *.la rose_*.[cC]

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_parameters buildCheckers.C checkers.h compass_makefile.inc

################################################################################
