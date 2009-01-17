include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

if ROSE_USE_SQLITE_DATABASE

$(compass_build_tooldir)/compass/libCompassCheckers.la:
	cd .. && $(MAKE) libCompassCheckers.la

bin_PROGRAMS = buildInterpreter

buildInterpreter_SOURCES = \
        roseQMGen.C \
        stringManipulations.C \
        commandLinePreProcessing.C \
        commandLineProcessing.C \
        qmgen.C

# DQ (1/12/2009): Removed -lrt since it fails on Mac OSX
# buildInterpreter_LDADD = $(ROSE_LIBS) -lrt
buildInterpreter_LDADD = $(ROSE_LIBS)

AM_CXXFLAGS = $(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS) -DBOOST_REGEX_MATCH_EXTRA -DRCFILE=\""$(top_builddir)/projects/compass/tools/compass/buildInterpreter/rqmgc"\"

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.

buildInterpreter_DEPENDENCIES = ${builddir}/rqmgc

${builddir}/rqmgc:
	cp -f ${srcdir}/rqmgc .

compass_parameters:
	cd .. && $(MAKE) compass_parameters
	cp -f ../compass_parameters .

test: buildInterpreter compass_parameters  
	export ROSE_REGRESSION_ROOT=`readlink -f ../../../../../tests/CompileTests/C_tests/`
	echo "Cleaning ../../../../../tests/CompileTests/C_tests/ "
	$(MAKE)  -C ../../../../../tests/CompileTests/C_tests/ testTranslator=`pwd`/buildInterpreter clean
	echo "Making ../../../../../tests/CompileTests/C_tests/ "
	$(MAKE)  -C ../../../../../tests/CompileTests/C_tests/ testTranslator=`pwd`/buildInterpreter check


#test: compass_parameters compassMainGui $(compass_test_dir)/exampleTest_1.C
#	env COMPASS_PARAMETERS=./compass_parameters ./compassMainGui $(compass_test_dir)/exampleTest_1.C

check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compass/tools/compass/buildInterpreter: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = compass_parameters rqmgc

endif


# DQ (1/11/2009): Modified this, since it was not checked into SVN and does not appear to be required.
# EXTRA_DIST = compassChecker.h compassGui.h compassInterface.h compassResult.h compassViolation.h icons.h timer.h disks.xpm 
EXTRA_DIST = commandLinePreProcessing.h  commandLineProcessing.h  defs.h  extern.h  qmgen.h  stringManipulations.h rqmgc

