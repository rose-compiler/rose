#################################################################################
# This is a sample of how one would create a subset of compass as a separate 
# tool
#
################################################################################

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

compass_srcdir=$(srcdir)/../
compass_builddir=$(top_builddir)/projects/compass/

include $(compass_srcdir)/compass_infrastructure.inc

INCLUDES = $(ROSE_INCLUDES) -I$(compass_srcdir)/compassSupport $(COMPASS_INCLUDES) -I$(compass_srcdir) -I.

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_builddir)/compassSupport/libCompass.la

lib_LTLIBRARIES = libCompassCheckers.la
libCompassCheckers_la_SOURCES = buildCheckers.C checkers.h
libCompassCheckers_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_builddir)/compassSupport/libCompass.la

BUILT_SOURCES = buildCheckers.C checkers.h

libCompassCheckers.la: $(libCompassCheckers_la_OBJECTS) $(libCompassCheckers_la_DEPENDENCIES) $(COMPASS_CHECKER_OBJS)
	$(CXXLINK) -rpath $(libdir) $(libCompassCheckers_la_LDFLAGS) $(libCompassCheckers_la_OBJECTS) $(COMPASS_CHECKER_OBJS) $(libCompassCheckers_la_LIBADD) $(LIBS)

bin_PROGRAMS = sampleCompassSubset

sampleCompassSubset_SOURCES = compassMain.C checkers.h
sampleCompassSubset_LDADD = -lCompassCheckers $(ROSE_LIBS)

sampleCompassSubset_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) $(compass_builddir)/compassSupport/libCompass.la $(compass_detector_libs) libCompassCheckers.la

test: sampleCompassSubset compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./sampleCompassSubset $(compass_srcdir)/tests/exampleTest_1.C

check-local:
	@$(MAKE) test
	@echo "*****************************************************************************"
	@echo "*** ROSE/projects/compass/compassVerifier: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************" 

EXTRA_DIST = CHECKER_LIST RULE_SELECTION

clean-local:
	rm -f compass_parameters *.ti *.lo *.la

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_parameters buildCheckers.C checkers.h compass_makefile.inc

################################################################################
