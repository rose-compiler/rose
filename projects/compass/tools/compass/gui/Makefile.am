include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

if ROSE_USE_QROSE

$(compass_build_tooldir)/compass/libCompassCheckers.la:
	cd .. && $(MAKE) libCompassCheckers.la

bin_PROGRAMS = compassMainGui

compassMainGui_SOURCES = \
	timer.C \
	compassGui.C \
	compassResult.C \
	compassViolation.C \
	compassChecker.C \
	compassInterface.C \
	compassMainGui.C

compassMainGui_LDADD = -L$(compass_build_tooldir)/compass -lCompassCheckers $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

# INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) $(QROSE_CXXFLAGS) $(BOOST_CPPFLAGS) -I$(compass_support_dir) -I$(compass_srcdir) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir) -I.
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I$(compass_support_dir) -I$(compass_srcdir) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir) -I.

compassMainGui_DEPENDENCIES = $(compass_build_tooldir)/compass/libCompassCheckers.la

compass_parameters:
	cd .. && $(MAKE) compass_parameters
	cp -f ../compass_parameters .

test: compass_parameters compassMainGui $(compass_test_dir)/exampleTest_1.C
	env COMPASS_PARAMETERS=./compass_parameters ./compassMainGui $(compass_test_dir)/exampleTest_1.C

test2: compass_parameters compassMainGui $(compass_test_dir)/exampleTest_1.C $(compass_test_dir)/exampleTest_2.C
	env COMPASS_PARAMETERS=./compass_parameters ./compassMainGui $(compass_test_dir)/exampleTest_1.C $(compass_checker_dir)/allowedFunctions/allowedFunctionsTest1.C

check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compass/tools/compass/gui: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = compass_parameters

EXTRA_DIST = \
        compassChecker.h \
        compassGui.h \
        compassInterface.h \
        compassResult.h \
        compassViolation.h \
        icons.h \
        timer.h \
	disks.xpm 

endif
