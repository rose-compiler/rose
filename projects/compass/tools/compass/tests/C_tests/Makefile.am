include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

# bin_PROGRAMS = statements

# This transltor output source position info
# statements_SOURCES = statements.C
# statements_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

# DQ (8/14/2004) Developer's Note:
# This permits the use of the pair of libraries -lrose -ledg OR the longer 
# list of libraries (the later option is much faster for development work).
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# Turn on GNU Coverage Tool (gcov)
# CXX += -fprofile-arcs -ftest-coverage

# Configure-time automake conditional
if ROSE_USE_LONG_MAKE_CHECK_RULE

TESTCODES_REQUIRED_TO_PASS = \
test_CplusplusMacro_C.C \
test2001_02.C \
test2001_03.C \
test2001_05.C \
test2001_07.C \
test2001_08.C \
test2001_09.C \
test2001_14.C \
test2001_17.C \
test2001_23.C \
test2001_24.C \
test2001_25.C \
test2001_26.C \
test2002_01.C \
test2003_04.C \
test2003_05.C \
test2003_06.C \
test2003_10.C \
test2003_11.C \
test2003_13.C \
test2003_14.C \
test2003_15.C \
test2003_16.C \
test2003_18.C \
test2003_19.C \
test2003_20.C \
test2003_21.C \
test2003_23.C \
test2003_24.C \
test2003_25.C \
test2003_26.C \
test2003_29.C \
test2003_30.C \
test2003_31.C \
test2003_32.C \
test2003_35.C \
test2003_36.C \
test2004_03.C \
test2004_04.C \
test2004_05.C \
test2004_13.C \
test2004_14.C \
test2004_19.C \
test2004_23.C \
test2004_28.C \
test2004_29.C \
test2004_49.C \
test2004_53.C \
test2004_60.C \
test2004_61.C \
test2004_72.C \
test2004_74.C \
test2004_94.C \
test2004_96.C \
test2004_102.C \
test2004_105.C \
test2004_107.C \
test2004_108.C \
test2004_114.C \
test2004_118.C \
test2004_131.C \
test2004_152.C \
test2004_153.C \
test2004_154.C \
test2004_163.C \
test2005_16.C \
test2005_20.C \
test2005_21.C \
test2005_22.C \
test2005_23.C \
test2005_26.C \
test2005_48.C \
test2005_62.C \
test2005_84.C \
test2005_92.C \
test2005_95.C \
test2005_100.C \
test2005_111.C \
test2005_116.C \
test2005_125.C \
test2005_157.C \
test2005_159.C \
test2005_164.C \
test2005_165.C \
test2005_169.C \
test2005_170.C \
test2005_176.C \
test2005_180.C \
test2005_182.C \
test2005_184.C \
test2005_187.C \
test2005_191.C \
test2005_198.C \
test2005_202.C \
test2005_209.C \
test2006_07.C \
test2006_10.C \
test2006_12.C \
test2006_14.C \
test2006_16.C \
test2006_23.C \
test2006_34.C \
test2006_40.C \
test2006_50.C \
test2006_52.C \
test2006_53.C \
test2006_55.C \
test2006_56.C \
test2006_57.C \
test2006_62.C \
test2006_69.C \
test2006_75.C \
test2006_76.C \
test2006_82.C \
test2006_83.C \
test2006_88.C \
test2006_91.C \
test2006_93.C \
test2006_94.C \
test2006_97.C \
test2006_99.C \
test2006_103.C \
test2006_104.C \
test2006_105.C \
test2006_106.C \
test2006_107.C \
test2006_108.C \
test2006_109.C \
test2006_111.C \
test2006_113.C \
test2006_116.C \
test2006_137.C \
test2006_140.C \
test2006_142.C \
test2006_145.C \
test2006_146.C \
test2006_147.C \
test2006_148.C \
test2006_149.C \
test2006_150.C \
test2006_151.C \
test2006_152.C \
test2006_153.C \
test2006_154.C \
test2006_181.C \
test2007_77.C \
test2007_81.C \
test2007_82.C \
test2007_92.C \
test2007_94.C \
test2007_98.C \
test2007_102.C \
test2007_103.C \
test_int_lexemes.C \
test_int_lexemes_donot_pass.C \
test_simple_int.C \
test_wchars.C \
X.C

else

# Shorten the list of code to compile to simplify checks using "make distcheck"
# Since these file reference file in the ../Cxx_tests direcotry we have to 
# shorten this file list but not more than it is done there, else "make distcheck"
# will fail.
TESTCODES_VERIFIED_TO_PASS = \
test2001_02.C \
test2001_03.C

endif

# JJW 5-22-2008 This test is not portable to Mac OS X
# test2006_44.C


TESTCODE_CURRENTLY_FAILING = 

# test2007_95.C -- broken test case

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

## Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# JJW: Add touch command so tests will not be run more than once
# $(TEST_Objects): preprocessor $(srcdir)/$(@:.o=.C)
$(TEST_Objects): ../../compassMain ../../compass_parameters
#	$(VALGRIND) ./statements $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir)/../Cxx_tests -c $(srcdir)/../Cxx_tests/$(@:.o=.C)
	echo "Copy the file to the local build tree so that the we can process it with compass (not required by compass) ..."
	cp $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.o=.C) $(@:.o=.c)
	env COMPASS_PARAMETERS=../../compass_parameters $(VALGRIND) ../../compassMain --compass:silent $(ROSE_FLAGS) -I$(top_builddir) -I$(top_srcdir)/tests/CompileTests/Cxx_tests $(TESTCODE_INCLUDES) -rose:includeFile $(@:.o=)  -c $(@:.o=.c) && touch $@

../../compass_parameters:
	cd ../..; $(MAKE) compass_parameters

../../compassMain:
	cd ../..; $(MAKE) compassMain

check-local:
# DQ (9/11/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
if !OS_VENDOR_DEBIAN
	@echo "Tests Compass (and its checkers) using a cross-section of the ROSE C test codes..."
	@$(MAKE) $(PASSING_TEST_Objects)
endif
	@echo "*************************************************************************************************"
	@echo "****** ROSE/projects/compass/tests/C_tests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************"

distclean-local:
	rm -rf Makefile 

clean-local:
	rm -rf *.o *.ps *.dot *.c
