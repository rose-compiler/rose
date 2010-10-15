include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc


# DQ (1/18/2009): Added demo directory to support showoffs :-).
# DQ "config" and "src" must preceed any other directories, and 
# "tutorial" and "exampleTranslators" must preceed "docs" since 
# they generate data that is subsequently used in the generated 
# ROSE documentation (examples, input codes, and generated output).
# SUBDIRS = config src projects tests tutorial exampleTranslators docs developersScratchSpace tools proposals scripts
# SUBDIRS = libltdl config src projects tests tutorial exampleTranslators docs tools scripts demo binaries

# DQ (2/6/2010): changed the order of tests, the projects directory now follows the tests directory.
# This was something that was discussed in email previously with the group.  Thei point is to have the
# first tests run by make check be the most relevant tests (e.g. can we compile C++ code).
# SUBDIRS = libltdl config src tests projects tutorial exampleTranslators docs tools scripts demo binaries

# DQ (4/17/2010): Support for minimal configuration of ROSE (e.g. "--enable-only-fortran").
SUBDIRS = libltdl config src
if ROSE_BUILD_TESTS_DIRECTORY_SUPPORT
   SUBDIRS += tests
endif
if ROSE_BUILD_PROJECTS_DIRECTORY_SUPPORT
   SUBDIRS += projects
endif
if ROSE_BUILD_TUTORIAL_DIRECTORY_SUPPORT
 # If we can't process the tutorial directory then we can't generate example translators or the documentation
   SUBDIRS += tutorial exampleTranslators docs 
endif
SUBDIRS += tools scripts 
if ROSE_BUILD_TUTORIAL_DIRECTORY_SUPPORT
 # If we can't process the tutorial directory then we can't run demos
   SUBDIRS += demo
endif
SUBDIRS += binaries

ACLOCAL_AMFLAGS = -I ./config -I ./acmacros -I ./libltdl -I /usr/share/aclocal

# Force rose.docs to be make by default 
# (if rose.docs.in is modified)
all-local: rose.docs 

all: remove_sage3basic_h_pch

# DQ (10/2/10): Of course this will over-rise the "make install" rule, so we can't 
# allow exactly this (what was I thinking?). Commented out until I get a better fix.
# DQ (9/29/2010): Next three lines are a modification suggested by Scott Warren to 
# support Eclipse.  Supports Eclipse feature called "makefile project with existing code" 
# which wants to have a single target to force the build.
#.PHONY: all-install

#all-install: all install

#install: all

remove_sage3basic_h_pch:
	touch $(top_builddir)/src/frontend/SageIII/sage3basic.h.pch
	rm    $(top_builddir)/src/frontend/SageIII/sage3basic.h.pch

# These are policies that we'd like to enforce for ROSE developers.  Things like:
#    1. source files should not use CR+LF line termination common on Windows platforms
#    2. header files should not include certain ROSE headers like "rose.h"
#    3. file names must be unique on a case-insensitive file system
#    4. header files must not have names that conflict with headers in other directories
# etc.
enforce_policies:
	(cd $(top_srcdir) && ./scripts/policies-checker.sh)


# DQ (6/29/2004): I don't think we need this!
# LIBS = @LIBS@ ${SAGE_LIBS}

# It seems that it is better to handle the config directory with its own Makefile.am
# This allows us to hide autoconf accessory files (config.guess, etc.)
# Copy the config directory to the distribution: why?
# (because it has the file: config/Makefile.for.ROSE.includes.and.libs)
# EXTRA_DIST = stamp-h.in stamp-h1.in GNU_HEADERS config
# EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in GNU_HEADERS COPYWRITE ROSE_ResearchPapers
# EXTRA_DIST = cmake CMakeLists.txt rose_config.h.in.cmake stamp-h.in stamp-h1.in rose.docs.in COPYRIGHT \
#              LicenseInformation ROSE_ResearchPapers README.OSX README.Cygwin build ChangeLog2 bincompat-sig
EXTRA_DIST = cmake CMakeLists.txt rose_config.h.in.cmake stamp-h.in stamp-h1.in rose.docs.in COPYRIGHT \
             LicenseInformation ROSE_ResearchPapers README.OSX README.Cygwin build ChangeLog2 bincompat-sig

# Liao, 12/11/2009. we should force this rule to run every time we want to make a distribution
# otherwise the stale one can go into the distribution package
cleanSig:
	rm -rf bincompat-sig
bincompat-sig: cleanSig
	$(srcdir)/scripts/bincompat-sig > $@

# At some point we want to put tals into the distribution (likely just PDF files, but not yet).
# ROSE_Talks

# clean-local explanation:
# Remove template repositories.  No source code is compiled, but configuration
#   tests may have created template repositories here.
clean-local:
	rm -rf Templates.DB ii_files ti_files *.csv

# Not sure if this should be part of the clean-local rule.
# chmod -R +rwx rose-0.9.5a; rm -rf rose-0.9.5a

# DQ (9/8/2006): Modified to remove backend specific header files
uninstall-local:
	rm -rf $(DESTDIR)$(includedir)/*_HEADERS

distclean-local:
	-rm -rf $(top_builddir)/include-staging/

# DQ (10/14/2010): We want to make sure that this does not go out to the install tree
# it also should not be in the distribution (since we want rose_config.h.in to be put 
# into the distribution instead; from which then rose_config.h is generated).
# nodist_include_HEADERS = rose_config.h
noinst_HEADERS = rose_config.h

# DQ (4/5/2009): This rule is not called when compiling this directory, it is only
# called when compiling the /src/util directory (perhaps it should be located in 
# that directory's Makefile.am.
# DQ (12/3/2007): Added rose_paths.h so that it would be copied to the install tree.
# JJW (8/25/2008): Changed to a .c file
include $(top_srcdir)/config/build_rose_paths.Makefile

check-local: enforce_policies
	@echo '************************************************************'
	@echo '****** make check rule complete (terminated normally) ******'
	@echo '************************************************************'

install-exec-local:
	@echo '*****************************************************************'
	@echo '***** make install-exec rule complete (terminated normally) *****'
	@echo '*****************************************************************'

install-data-local:
	-rm -rf $(DESTDIR)$(includedir)/*_HEADERS
	-mkdir $(DESTDIR)$(includedir)
	cp -R $(top_builddir)/include-staging/* $(DESTDIR)$(includedir)/
	@echo '*****************************************************************'
	@echo '***** make install-data rule complete (terminated normally) *****'
	@echo '*****************************************************************'

# DQ (4/22/2005): Set this so that make distcheck will use the same 
# --with-edg_source_code=true/false option as were used at configure.
# JJW (5/14/2008): Add the Boost flag the same way, using an internal
# variable from AX_BOOST_* to find the argument to --with-boost when
# this copy of ROSE was originally configured.
DISTCHECK_CONFIGURE_FLAGS = --with-boost=$(ac_boost_path)

# DQ (7/25/2008): If ROSE was originallly configured with Fortran (by tuning 
# on the java support) then make sure it is tested as part of the distcheck rule.
if ROSE_USE_OPEN_FORTRAN_PARSER
#  DISTCHECK_CONFIGURE_FLAGS += --with-java
   DISTCHECK_CONFIGURE_FLAGS += --with-java=$(JAVA_PATH)
endif

# DQ (7/25/2008): If ROSE was originallly configured to use the HPC Toolkit
# then make sure it is tested as part of the distcheck rule.
if ROSE_BUILD_ROSEHPCT
   DISTCHECK_CONFIGURE_FLAGS += --enable-rosehpct
endif

# DQ (7/25/2008): If ROSE was originallly configured to use the javaport
# work (generation of Java interface functions for ROSE using SWIG) then 
# make sure it is tested as part of the distcheck rule.
if ENABLE_JAVAPORT
   DISTCHECK_CONFIGURE_FLAGS += --with-javaport
endif

# DQ (4/5/2010): Use the same version of OFP as specified on the original configure command line.
DISTCHECK_CONFIGURE_FLAGS += \
   --enable-ofp-version=@ROSE_OFP_MAJOR_VERSION_NUMBER@.@ROSE_OFP_MINOR_VERSION_NUMBER@.@ROSE_OFP_PATCH_VERSION_NUMBER@

# SRIRAM (8/2/2010): changing distcheck configuration for LLVM translator
if ROSE_USE_LLVM
DISTCHECK_CONFIGURE_FLAGS += --with-llvm=$(llvm_path)
endif

# DQ (4/22/2005): To make clear when binaries are in the distribution as 
# opposed to EDG source code we rename the final generated distribution.
# JJW (8/3/2008): This is only used for source distributions now --
# source_with_binary_edg_dist does its own renaming
rename_distribution:
	new_name="$(distdir)-source-with-EDG-source.tar.gz"; \
	cp $(distdir).tar.gz $$new_name

# We reset distdir as shown on
# http://jcalcote.wordpress.com/2008/02/23/autoconf-macros-exposed-at3/ to add the SVN
# revision number
# The final distribution package should be
# rose-0.9.5a-8286.tar.gz
#
# Liao 12/14/2009
# We don't use subversion anymore
# We convert the number of git commits to a pseudo revision number instead
distdir = $(PACKAGE)-$(VERSION)$(DOT_SVNREV)

# Automake hook-ish rule to be run after dist rule (but hook runs before gzip so we can't use hook)
# We could build our own ROSE specific wrapper for dist and distcheck.
dist-after:
	@echo "Running dist hook rule ..."
	@$(MAKE) rename_distribution

# hook rule to be run after distcheck rule
distcheck-after:
	@echo "Running distcheck hook rule ..."
	@$(MAKE) rename_distribution

# CLEANFILES = rose_binary_compatibility_signature rose_binary_compatibility_signature_src
CLEANFILES = rose_binary_compatibility_signature bincompat-sig

find_rose_lock_files:
#	du -a | grep rose_performance_report_lockfile
	find . -name rose_performance_report_lockfile -print

# New rule to simplify generation of documentation (we want to avoid using the
# automake generated "docs" rule because the generation of documentation is
# dependent upon separate tools which the user might not have available
# (true for bothe the LaTeX and html documentation).
# DQ (7/23/2004): Since there is a "docs" subdirectory we need to force the build!
FORCE_DOCS_TO_BE_MADE:

# DQ (7/25/2008): Running "make docs" introduces dependences that are inappropriate
# for a distribution.
docs: FORCE_DOCS_TO_BE_MADE
	@if [ -f $(top_srcdir)/src/midend/midend.docs ]; then \
	   echo "    Generate all possible documentation cd docs; make documentation;"; \
	   $(MAKE) -Ctutorial docs && \
	   $(MAKE) -Cprojects/haskellport docs && \
	   $(MAKE) -Ctests/roseTests/astInterfaceTests check && \
	   $(MAKE) -Ctests/roseTests/ompLoweringTests check && \
	   $(MAKE) -Ctests/CompileTests/UPC_tests check && \
	   $(MAKE) -Cprojects/UpcTranslation/tests check && \
	   $(MAKE) -Cprojects/autoParallelization/tests check && \
	   $(MAKE) -Cdocs docs && \
	   $(MAKE) -C$(compass_build_tooldir)/compass docs; \
	else \
	   echo "Please look at the ROSE web site for documentation (www.roseCompiler.org)"; \
	fi

if ROSE_HAS_EDG_SOURCE

# 1
# This rule generates the simple binary tarball for the directories: EDG and EDG_SAGE_Connection
binary_edg_tarball: 
	$(MAKE) -C src/frontend/CxxFrontend binary_edg_tarball

upload_edg_binary:
	$(MAKE) -C src/frontend/CxxFrontend upload_edg_binary

endif

# Makefile rule to test the Hudson runTest script (used for all Hudson tests).
# Note that empty ROSE_TEST_BUILD_STYLE maps to a minimal build.
testHudsonRunScript:
	export ROSE_TEST_BUILD_SKIP_BUILD_SCRIPT=yes; \
   export ROSE_TEST_GCC_VERSION=4.2.4; \
   export ROSE_TEST_BUILD_STYLE=; \
   export ROSE_TEST_BOOST_PATH=${MY_BOOST_ROOT}; \
   export ROSE_TEST_JAVA_PATH=${JAVA_PATH}; \
   cd $(top_srcdir); \
   $(top_srcdir)/scripts/hudson/runTest

# This will cause the Hudson environment to be dumped to a file so that
# it can be sourced to reproduce the environment required to run tests
# in the hudson tmp directories as part of debugging.
dumpHudsonEnv:
	export ROSE_DUMP_HUDSON_ENV=yes; $(MAKE) testHudsonRunScript


# $(top_srcdir)/scripts/hudson/testHudsonRunScript.sh;


# DQ (12/16/2009): Added interesting "git" feature by Liao.
# It is unclear if it is very meaningful since it represents 
# individual work style more than anything else (but it's fun).
CheckinCounts:
	@echo "*****************************************************"
	@echo "Generated list of checkins to ROSE sumarized by user."
	@echo "*****************************************************"
	cd $(top_srcdir); git shortlog -s -n
	@echo "*****************************************************"


# This rule is useful for identifing source files where "rose_config.h" should be included.
FindFileRequiringRoseConfigHeaderShort:
#	grep "#define" rose_config.h | cut --delimiter=" " -f 2 
#	@list=`grep \"\#define\" rose_config.h | cut --delimiter=" " -f 2`; for f in $$list; do 
#	list=`ls -l | cut --delimiter=" " -f 2`; for f in $$list; do
	list=`grep \#define rose_config.h | cut --delimiter=" " -f 2`; \
   for f in $$list; do \
        echo "Searching for -->$$f<-- macro"; \
        grep -r $$f $(top_srcdir)/projects/* | grep "\.C:" | cut --delimiter=":" -f 1; \
   done

# This rule is useful for identifing header files where "rose_config.h" is required and 
# thus should be fixed to not require this file.
FindFileRequiringRoseConfigHeaderLong:
	list=`grep \#define rose_config.h | cut --delimiter=" " -f 2`; \
   for f in $$list; do \
        echo "Searching for -->$$f<-- macro"; \
        grep -r $$f $(top_srcdir)/projects/* | grep "\.h:"; \
   done


# DQ (2/12/2010): Report the areas of ROSE where we have skipped some tests of ROSE 
# either doing analysis, code generation, or translation of the ROSE source code.
ProblemAreas:
	echo "Directories where ROSE does not analyze code for some ROSE source files."
	grep -r ROSE_USING_ROSE_ANALYSIS $(top_srcdir) | grep Makefile.am
	echo "Directories where ROSE does not generate code for some ROSE source files."
	grep -r ROSE_USING_ROSE_CODE_GENERATION $(top_srcdir) | grep Makefile.am
	echo "Directories where ROSE does not translate (compile) code for some ROSE source files."
	grep -r ROSE_USING_ROSE_TRANSLATE $(top_srcdir) | grep Makefile.am
	echo "Directories where ROSE does not properly handled AST File I/O for at least some ROSE source files."
	grep -r ROSE_USING_ROSE_AST_FILE_IO $(top_srcdir) | grep Makefile.am
	echo "Files where ROSE does not analyze some amount of code (test of USE_ROSE in each ROSE source file)."
	grep -rl USE_ROSE $(top_srcdir) | grep "\.C"
	echo "Files where ROSE does not analyze some amount of code (test of CXX_IS_ROSE_ANALYSIS in each ROSE source file)."
	grep -rl CXX_IS_ROSE_ANALYSIS $(top_srcdir) | grep "\.C"
	echo "Files where ROSE does not analyze some amount of code (test of CXX_IS_ROSE_CODE_GENERATION"
	echo "in each ROSE source file)."
	grep -rl CXX_IS_ROSE_CODE_GENERATION $(top_srcdir) | grep "\.C"
#	echo "Files where ROSE does not analyze some amount of code (test of CXX_IS_ROSE_TRANSLATOR in each ROSE source file)."
#	grep -rl CXX_IS_ROSE_TRANSLATOR $(top_srcdir) | grep "\.C"



# Report the version number of the ROSE translator built.
PrintRoseTranslatorVersion: $(top_builddir)/tests/testAnalysis
	$(top_builddir)/tests/testAnalysis --version

release_binary_compatibility_signature=$(shell ${top_srcdir}/scripts/bincompat-sig)

PrintBinaryCompatibilitySignature:
	@echo "release_binary_compatibility_signature = ${release_binary_compatibility_signature}"
