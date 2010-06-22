include $(top_srcdir)/projects/compass/src/compassSupport/compass.inc

SUBDIRS = doc tests gui gui2 buildInterpreter

BUILT_SOURCES = buildCheckers.C checkers.h

lib_LTLIBRARIES = libCompassCheckers.la
libCompassCheckers_la_SOURCES = buildCheckers.C checkers.h
libCompassCheckers_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

libCompassCheckers.la: $(libCompassCheckers_la_OBJECTS) $(libCompassCheckers_la_DEPENDENCIES) $(COMPASS_CHECKER_OBJS)
	$(CXXLINK) -rpath $(libdir) $(libCompassCheckers_la_LDFLAGS) $(libCompassCheckers_la_OBJECTS) $(COMPASS_CHECKER_OBJS) $(libCompassCheckers_la_LIBADD) $(LIBS)

bin_PROGRAMS = compassMain

compassMain_SOURCES = compassMain.C checkers.h
compassMain_LDADD = -lCompassCheckers $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

compassMain_DEPENDENCIES = $(compass_support_bdir)/libCompass.la $(compass_detector_libs) libCompassCheckers.la 


test: compassMain compass_parameters $(compass_test_dir)/exampleTest_1.C 
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain -rose:skip_unparser -rose:skipfinalCompileStep $(compass_test_dir)/exampleTest_1.C

testCmdLineMashup: compassMain compass_parameters $(compass_test_dir)/exampleTest_1.C $(srcdir)/NOTES
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain -rose:skip_unparser -rose:skipfinalCompileStep $(srcdir)/NOTES $(compass_test_dir)/exampleTest_1.C

docs:
	cd doc; $(MAKE) docs

check-local:
	@echo "*****************************************************************************"
	@echo "*** Testing blank AST checker generator ***"
	@echo "*****************************************************************************"
# DQ (9/10/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
if !OS_VENDOR_DEBIAN
	@$(MAKE) testNewChecker
	@echo "***********************"
	@echo "*** Testing compass ***"
	@echo "***********************"
	@$(MAKE) test
endif
	@echo "*****************************************************************************"
	@echo "*** ROSE/projects/compass/tools/compass: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************"

distclean-local:
	rm -rf Templates.DB ale3d-typedefs.sn *.sn

clean-local:
	rm -f *.ti a.out *.lo *.la rose_*.[cC]

EXTRA_DIST = CHECKER_LIST RULE_SELECTION.in certExample.txt ChangeLog NOTES \
	emacs_compass_config.el

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_makefile.inc compass_parameters buildCheckers.C checkers.h compassCheckerDocs.tex


################################################################################

# Test Compass using a small test file
testRose: compassMain compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain -I$(top_builddir) $(INCLUDES) $(srcdir)/tests/exampleTest_2.C

# Test Compass using a large file within ROSE
testCxx_Grammar: compassMain compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain -I$(top_builddir) $(INCLUDES) $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

testAllInputsTogether: compassMain compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain $(COMPASS_CHECKER_TESTS)

# Flymake Support in ROSE
check-syntax: compassMain compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain --compass:silent -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -rose:includeFile $(CHK_SOURCES:_flymake.C=) $(CHK_SOURCES)
#       @$(top_builddir)/projects/compass/compassMain --compass:silent -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -rose:excludeFile .code -rose:excludeFile .macro -rose:includeFile $(CHK_SOURCES) $(CHK_SOURCES)
#       $(top_builddir)/projects/compass/compassMain --compass:silent -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -rose:includeFile $(CHK_SOURCES:_flymake.C=) $(CHK_SOURCES)

#testEmacs: compassMain compass_parameters
#	env COMPASS_PARAMETERS=./compass_parameters ./compassMain --compass:silent -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -rose:includeFile $(srcdir)/testEmacs.C $(srcdir)/testEmacs.C

#testEmacsSize: compassMain compass_parameters
#	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) $(srcdir)/testEmacs.C > wc.out
#	wc -l wc.out

# DQ (1/15/2009): This fails in a Boost problem of some sort!
testCompass:
	$(MAKE) "CHK_SOURCES=$(compass_support_dir)/compassMain.C" check-syntax

# This runs compass on the compass binary, small because this is dynamically linked
demo_small: test
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain .libs/compassMain

# Linux binary selected from Linux
demo_linux: test
	env COMPASS_PARAMETERS=./compass_parameters ./compassMain $(top_srcdir)/projects/BinQ/linux-ls

#demo_bufferOverflow:
#	./compassMain $(top_srcdir)/projects/BinQ/buffer2.bin

# Note that gzip.c appears to take over ten minutes in the Defuse Analysis (something is wrong here).
demo_compassGUI_source:
#	./gui/compassMainGui $(top_srcdir)/developersScratchSpace/Dan/C_tests/gzip.c
	./gui/compassMainGui $(compass_test_dir)/exampleTest_1.C

demo_compassGUI_binary:
	./gui/compassMainGui $(top_srcdir)/projects/BinQ/buffer2.bin

demo_compassGUI_compass_binary:
	./gui/compassMainGui .libs/compassMain

