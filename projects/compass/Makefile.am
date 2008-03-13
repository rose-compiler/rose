include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# The space at the beginning of this line makes it be interpreted by Make
# rather than Automake
# This file will be built automatically by Make if it is not already there (or
# is out of date)
@INCLUDE_COMPASS_MAKEFILE_INC@

SUBDIRS = compassSupport util compassVerifier tests

BUILT_SOURCES = buildCheckers.C checkers.h

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -I.

bin_PROGRAMS = compassMain

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) compassSupport/libCompass.la

lib_LTLIBRARIES = libCompassCheckers.la
libCompassCheckers_la_SOURCES = buildCheckers.C checkers.h
libCompassCheckers_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) compassSupport/libCompass.la

# This rule is from an Automake-generated Makefile, but we need it to override
# Automake because some dependencies come from Make (and are not visible to
# Automake)
libCompassCheckers.la: $(libCompassCheckers_la_OBJECTS) $(libCompassCheckers_la_DEPENDENCIES) $(COMPASS_CHECKER_OBJS)
	$(CXXLINK) -rpath $(libdir) $(libCompassCheckers_la_LDFLAGS) $(libCompassCheckers_la_OBJECTS) $(COMPASS_CHECKER_OBJS) $(libCompassCheckers_la_LIBADD) $(LIBS)

compassMain_SOURCES = compassMain.C checkers.h
compassMain_LDADD = -lCompassCheckers $(ROSE_LIBS)

# This does not contribute to the link line
compassMain_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) compassSupport/libCompass.la $(compass_detector_libs) libCompassCheckers.la

CHECKER_LIST_WITHOUT_COMMENTS: $(srcdir)/CHECKER_LIST
	grep -v '^#' < $(srcdir)/CHECKER_LIST | awk '{print $$0 " " toupper(substr($$0, 0, 1)) substr($$0, 2)}' > CHECKER_LIST_WITHOUT_COMMENTS

compass_makefile.inc: CHECKER_LIST_WITHOUT_COMMENTS $(srcdir)/buildCompassMakefileInc.sh
	$(srcdir)/buildCompassMakefileInc.sh $(srcdir) > compass_makefile.inc

buildCheckers.C: CHECKER_LIST_WITHOUT_COMMENTS
	sed -n '1,/@@@/p' < $(srcdir)/buildCheckers.C.in > buildCheckers.C
	cut -f 2 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,BUILD_ONE_CHECKER(&),' >> buildCheckers.C
	sed -n '/@@@/,$$p' < $(srcdir)/buildCheckers.C.in >> buildCheckers.C

checkers.h: CHECKER_LIST_WITHOUT_COMMENTS
	cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,#include "&/&.h",' > checkers.h

compass_parameters: CHECKER_LIST_WITHOUT_COMMENTS
	echo "# this is an automatically generated file " > compass_parameters
	echo "Compass.RuleSelection=RULE_SELECTION" >> compass_parameters
	echo '' >> compass_parameters
	cat CHECKER_LIST_WITHOUT_COMMENTS | while read ch chupper; do if [ -f $(srcdir)/$$ch/compass_parameters ]; then grep -v '^#' $(srcdir)/$$ch/compass_parameters >> compass_parameters; fi; done

# Verify that all checkers conform to specific rules (fast, copies checkers to a single file)
oneBigVerify:
	cd compassVerifier; $(MAKE) oneBigVerify

# Verify that all checkers conform to specific rules (slow, runs compasson each checker)
verify:
	cd compassVerifier; $(MAKE) verify

alwaysPerformThis:
#	DQ (1/5/2008): Since we have commented out this test (see CHECKER_LIST), 
#	the $(srcdir)/typeTypedef is unavailable, plus we don't need this file!
#	cp -f $(srcdir)/typeTypedef/ale3d-typedefs.sn $(top_builddir)/projects/compass/.

# Test Compass using a small test file
test: compassMain alwaysPerformThis compass_parameters
	./compassMain $(srcdir)/tests/exampleTest_1.C

# Test Compass using a small test file
testRose: compassMain alwaysPerformThis compass_parameters
	./compassMain -I$(top_builddir) $(INCLUDES) $(srcdir)/tests/exampleTest_2.C

# Test Compass using a large file within ROSE
testCxx_Grammar: compassMain alwaysPerformThis compass_parameters
	./compassMain -I$(top_builddir) $(INCLUDES) $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

testAllInputsTogether: compassMain alwaysPerformThis compass_parameters
	./compassMain $(COMPASS_CHECKER_TESTS)

# Flymake Support in ROSE
check-syntax: compass_parameters
#	@$(top_builddir)/projects/compass/compassMain --compass:silent -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -rose:excludeFile .code -rose:excludeFile .macro -rose:includeFile $(CHK_SOURCES) $(CHK_SOURCES)
#	$(top_builddir)/projects/compass/compassMain --compass:silent -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -rose:includeFile $(CHK_SOURCES:_flymake.C=) $(CHK_SOURCES)
	$(top_builddir)/projects/compass/compassMain --compass:silent -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES) -rose:includeFile $(CHK_SOURCES:_flymake.C=) $(CHK_SOURCES)

testCompass:
	$(MAKE) "CHK_SOURCES=/home/dquinlan/ROSE/NEW_ROSE/projects/compass/compassMain.C" check-syntax

check-local:
	@$(MAKE) test
	@echo "*****************************************************************************"
	@echo "*** ROSE/projects/compass: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************"

# all: CHECKER_LIST 
# CHECKER_LIST: regenerate

regenerate:
	@echo "**************************************************************"
	@echo "*** Regenerating automatically generated files for compass ***"
	@echo "**************************************************************"
#	When using "make distcheck" rule we need to make the read only srcdir writable.
	chmod -R +w $(srcdir)
#	Call the compass_submission_setup directly (can also be called by the user)
	/bin/bash $(srcdir)/compassRepository/bin/compass_submission_setup.sh $(srcdir)/compassRepository $(srcdir) $(top_builddir)/projects/compass "regenerate"
	@echo "Builddir $(top_builddir)"
	@echo "/bin/bash $(srcdir)/compassRepository/bin/compass_submission_setup.sh $(srcdir)/compassRepository $(srcdir) $(top_builddir)/projects/compass \"regenerate\" "

regenerate_skip_compass_makefile_inc:
	@echo "***********************************************************************************************"
	@echo "*** Regenerating automatically generated files for compass (except for compass_makefile.inc ***"
	@echo "***********************************************************************************************"
#	When using "make distcheck" rule we need to make the read only srcdir writable.
	chmod -R +w $(srcdir)
#	Call the compass_submission_setup directly (can also be called by the user)
	/bin/bash $(srcdir)/compassRepository/bin/compass_submission_setup.sh $(srcdir)/compassRepository $(srcdir) $(top_builddir)/projects/compass "regenerate" "skip_compass_makefile"
	@echo "Builddir $(top_builddir)"
	@echo "/bin/bash $(srcdir)/compassRepository/bin/compass_submission_setup.sh $(srcdir)/compassRepository \
	                $(srcdir) $(top_builddir)/projects/compass \"regenerate\" "

# EXTRA_DIST = compass_parameters tests

# Build variable for checker header files
# COMPASS_HEADER_FILES = checkers.h ${COMPASS_CHECKER_SRCS:.C=.h}

# Automake distributions require headers and test codes. Note: We can't list the COMPASS_CHECKER_DIRECTORIES here since the list it too long and breaks automake (or something).
# EXTRA_DIST = compass_parameters tests compassRepository instructions.txt $(COMPASS_CHECKER_TEX_FILES) $(COMPASS_HEADERS) $(COMPASS_CHECKER_TESTS) CHECKER_LIST acknowledgments.tex addYourOwnDetector.tex 
# EXTRA_DIST = compass_parameters tests checkers.h compassRepository $(COMPASS_CHECKER_DIRECTORIES) CHECKER_LIST acknowledgments.tex addYourOwnDetector.tex instructions.txt 
# EXTRA_DIST = compass_parameters tests checkers.h compassRepository CHECKER_LIST acknowledgments.tex emacs.tex new_checkers.tex addYourOwnDetector.tex instructions.txt RULE_SELECTION
EXTRA_DIST = tests compassRepository CHECKER_LIST introduction.tex acknowledgments.tex usingCompass.tex appendix.tex instructions.txt RULE_SELECTION emacs_screenshot.jpg emacs_compass_config.el CompassScreenshot.pdf ToolGear_gui_compass_01.pdf compassQRoseMain.C QRose.txt

# We have to copy all the header files, latex files, test codes, etc. This can't be put 
# on the EXTRA_DIST line because it make the line too long and that causes problems.
# So we have a dist-hook rule.
dist-hook:
#	We don't want the buildCheckers.C file to be part of the distribution since it is a generated file.
#	This may mean that the rules for the regeneration of file could depend on either compass.h or buildCheckers.C.
#	Copy all the checker directories into the distribution.

	@for i in $(COMPASS_CHECKER_DIRECTORIES); do \
		cp -rf $(srcdir)/$$i $(distdir); \
	done;

distclean-local:
	rm -rf Templates.DB ale3d-typedefs.sn *.sn 

clean-local:
	rm -f compass.* *.sty *.ti a.out *.tex *.lo *.la emacs_screenshot.jpg
	rm -f CHECKER_LIST $(COMPASS_CHECKER_TEX_FILES)

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_makefile.inc compass_parameters buildCheckers.C checkers.h
