include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

if ROSE_USE_QT

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

compassMainGui_LDADD = -L$(compass_support_bdir) -L$(compass_build_tooldir)/compass -lCompassCheckers -lCompass $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

# INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) $(QROSE_CXXFLAGS) $(BOOST_CPPFLAGS) -I$(compass_support_dir) -I$(compass_srcdir) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir) -I.

if ROSE_MPI
INCLUDES = -DROSE_MPI $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I$(compass_support_dir) -I$(compass_srcdir) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir) -I.
else
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I$(compass_support_dir) -I$(compass_srcdir) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir) -I.
endif



compassMainGui_DEPENDENCIES = $(compass_build_tooldir)/compass/libCompassCheckers.la

compass_parameters:
	cd .. && $(MAKE) compass_parameters
	cp -f ../compass_parameters .

test: compass_parameters compassMainGui $(compass_test_dir)/exampleTest_1.C
	env COMPASS_PARAMETERS=./compass_parameters ./compassMainGui $(compass_test_dir)/exampleTest_1.C

test2: compass_parameters compassMainGui $(compass_test_dir)/exampleTest_1.C $(compass_test_dir)/exampleTest_2.C
	env COMPASS_PARAMETERS=./compass_parameters ./compassMainGui $(compass_test_dir)/exampleTest_1.C $(compass_checker_dir)/allowedFunctions/allowedFunctionsTest1.C

# This demo copys a collection of C source files and builds an SQLite database of there command lines and uses that with Compass.
demo: compass_parameters compassMainGui $(compass_test_dir)/exampleTest_1.C
	$(MAKE) -C ../buildInterpreter test
#	DQ (2/22/2009): In the SciDAC distribution there are no header files in this directiry, but there are in my version and also in the internal SVN).
#	cp $(top_srcdir)/tests/CompileTests/C_tests/*.c $(top_srcdir)/tests/CompileTests/C_tests/*.h .
	cp $(top_srcdir)/tests/CompileTests/C_tests/*.[ch] .
	env COMPASS_PARAMETERS=./compass_parameters ./compassMainGui --outputDb $(top_builddir)/tests/CompileTests/C_tests/test.db
	rm -f *.c *.h

check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compass/tools/compass/gui: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = compass_parameters

endif

clean-local:
	rm -f *.c *.h

EXTRA_DIST = \
        compassChecker.h \
        compassGui.h \
        compassInterface.h \
        compassResult.h \
        compassViolation.h \
        icons.h \
        timer.h \
        disks.xpm 


