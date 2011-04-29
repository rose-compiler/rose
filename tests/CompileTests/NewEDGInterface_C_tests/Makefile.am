include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################################################
# These are C testcodes that test the new EDG Interface (using EDG 4.0) #
#########################################################################

bin_PROGRAMS = newEDGInterfaceTranslator

# This tests the ability to read and regnerate the input application and compile it using the backend compiler.
newEDGInterfaceTranslator_SOURCES = newEDGInterfaceTranslator.C

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt

EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
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
test2002_01.C \
test2003_04.C \
test2003_05.C \
test2003_06.C \
test2003_10.C \
test2003_11.C \
test2003_13.C \
test2003_14.C \
test2003_16.C \
test2003_18.C \
test2003_20.C \
test2003_21.C \
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
test2004_23.C \
test2004_28.C \
test2004_29.C \
test2004_49.C \
test2004_53.C \
test2004_60.C \
test2004_61.C \
test2004_74.C \
test2004_94.C \
test2004_105.C \
test2004_107.C \
test2004_108.C \
test2004_114.C \
test2004_118.C \
test2004_131.C \
test2004_152.C \
test2004_153.C \
test2004_154.C \
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
test2005_125.C \
test2005_157.C \
test2005_159.C \
test2005_164.C \
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
test2006_50.C \
test2006_52.C \
test2006_55.C \
test2006_56.C \
test2006_57.C \
test2006_62.C \
test2006_69.C \
test2006_75.C \
test2006_82.C \
test2006_83.C \
test2006_93.C \
test2006_97.C \
test2006_99.C \
test2006_103.C \
test2006_106.C \
test2006_113.C \
test2006_137.C \
test2006_140.C \
test2006_142.C \
test2006_145.C \
test2006_147.C \
test2006_148.C \
test2006_149.C \
test2006_150.C \
test2006_151.C \
test2006_154.C \
test2006_181.C \
test2008_04.C \
X.C \
test2003_23.C \
test2003_24.C \
test2003_25.C \
test2004_96.C \
test2004_102.C \
test2005_16.C \
test2005_116.C \
test2006_76.C \
test2006_146.C \
test2003_12.C \
test2003_17.C \
test2005_175.C \
test2005_177.C \
test2006_15.C \
test2006_59.C \
test2006_71.C \
test2006_98.C \
test2006_101.C \
test2006_139.C \
test2006_160.C


if !ROSE_USE_EDG_VERSION_4_3
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
test2004_19.C \
test2005_111.C \
test2005_165.C \
test2006_88.C \
test2001_26.C \
test2003_15.C \
test2003_19.C \
test2004_14.C \
test2004_72.C \
test2004_163.C \
test2005_56.C \
test2006_40.C \
test2006_53.C \
test2006_91.C \
test2006_104.C \
test2006_105.C \
test2006_107.C \
test2006_108.C \
test2006_109.C \
test2006_111.C \
test2006_116.C \
test_int_lexemes.C \
test_int_lexemes_donot_pass.C \
test_simple_int.C \
test_wchars.C
endif

# TV (03/19/2011): Fail during EDG parsing if 'newEDGInterfaceTranslator' is call from the libtool script. No problem with the binary.
# test2006_94.C

# TV (03/19/2011): Fail on post-processing (by GCC) because '__LP64__' is not define on with EDG 4.0
# test2006_152.C
# test2006_153.C

# DQ (12/4/2006): Removed because it is not a valid C file (to gcc 3.3.2), and added below!
# test2004_06.C
# test2004_155.C
# test2005_158.C
# test2005_166.C
# test2005_192.C
# test2006_80.C
# test2006_102.C

# Removed since it is not a ".C" file and we do translation from, ".C" names to ".o" names that might be failing.
# test_CplusplusMacro_C.c
# test_CplusplusMacro_Cpp.C

# Configure-time automake conditional
if ROSE_USE_LONG_MAKE_CHECK_RULE

# This is the long test of ROSE (takes a while!)

TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

else
# Shorten the list of code to compile to simplify checks using "make distcheck"
# Since these file reference file in the ../Cxx_tests direcotry we have to 
# shorten this file list but not more than it is done there, else "make distcheck"
# will fail.
TESTCODES_REQUIRED_TO_PASS = \
test2001_02.C \
test2001_03.C 

endif

# code that didn't pass gcc 4.1.2
TESTCODE_FAILING_WITH_GCC = \
test2004_06.C \
test2004_07.C \
test2004_155.C

# These are the failures that we are interested in!
C_TESTCODE_CURRENTLY_FAILING = \
test2005_158.C \
test2005_166.C \
test2005_192.C \
test2005_200.C \
test2005_201.C \
test2006_33.C \
test2006_49.C \
test2006_80.C \
test2006_102.C \
test2006_155.C \
test2006_156.C \
test2006_157.C \
test2006_158.C \
test2006_159.C \
test2006_165.C

if ROSE_USE_EDG_VERSION_4_3
C_TESTCODE_CURRENTLY_FAILING += \
test2004_19.C \
test2005_111.C \
test2005_165.C \
test2006_88.C \
test2001_26.C \
test2003_15.C \
test2003_19.C \
test2004_14.C \
test2004_72.C \
test2004_163.C \
test2005_56.C \
test2006_40.C \
test2006_53.C \
test2006_91.C \
test2006_104.C \
test2006_105.C \
test2006_107.C \
test2006_108.C \
test2006_109.C \
test2006_111.C \
test2006_116.C \
test_int_lexemes.C \
test_int_lexemes_donot_pass.C \
test_simple_int.C \
test_wchars.C
endif

# test2004_151.C was modified to now be a C++ code and removed.

# JJW 5-22-2008 This file is not portable to Mac OS X
# test2006_44.C

# DQ: I think that we are only interested in the C codes that fail (but we can decide that later)
TESTCODE_CURRENTLY_FAILING = $(C_TESTCODE_CURRENTLY_FAILING) 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = -rose:C --edg:no_warnings -w

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# Note that for these C versions of the C++ tests we get the files directly from the Cxx_tests directory
# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.C)
$(TEST_Objects): ./newEDGInterfaceTranslator
	@echo "Copy the file to the local build tree so that we can change the file name suffix (to be a C language file) ..."
	cp $(srcdir)/../Cxx_tests/$(@:.o=.C) $(@:.o=.c)
	./newEDGInterfaceTranslator $(ROSE_FLAGS) -I$(srcdir)/../Cxx_tests -c $(@:.o=.c)
	rm -f $(@:.o=.c)

# This requires a special makefile rule
# Note that this will not link since gcc (typically) is used as the backend compiler and the source file lacks a "*.c" suffix.
test2005_56.o: newEDGInterfaceTranslator
#	../../testTranslator -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -c $(srcdir)/../Cxx_tests/test2005_56.C -o test2005_56.o
	@echo "Copy the file to the local build tree so that we can change the file name suffix (to be a C language file) ..."
	cp $(srcdir)/../Cxx_tests/$(@:.o=.C) $(@:.o=.c)
	./newEDGInterfaceTranslator $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -c $(@:.o=.c)
	rm -f $(@:.o=.c)

# CURRENT_DIRECTORY = `pwd`
# QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
# $(QMTEST_Objects): ../../testTranslator
#	@echo "Calling QMTEST_Objects rule for Cpp_tests directory: "
#	qm.sh o rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/../Cxx_tests/$(@:.qmt=.C)

# This must be placed after the more general rule to work properly!
# test2005_56.qmt: ../../testTranslator
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -I$(srcdir) -c $(srcdir)/../Cxx_tests/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
# include $(top_srcdir)/config/QMTest_makefile.inc

check-local:
if ROSE_USE_NEW_EDG_INTERFACE
	@echo "Tests for C language subset of C++ (some features in C are disallowed in C++ and are tested in C_tests directory)."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*************************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/NewEDGInterface_C_tests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************************"
else
	@echo "****************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/NewEDGInterface_C_tests: (ROSE not configured for these tests) ******"
	@echo "****************************************************************************************************"
endif

clean-local:
	rm -f *.o rose_*.[cC] test_*.[cC]
	rm -rf QMTest

C_TESTCODE_CURRENTLY_FAILING_Objects = ${C_TESTCODE_CURRENTLY_FAILING:.C=.o}

$(C_TESTCODE_CURRENTLY_FAILING_Objects): ./newEDGInterfaceTranslator
	@cp $(srcdir)/../Cxx_tests/$(@:.o=.C) $(@:.o=.c)
	@if ./newEDGInterfaceTranslator $(ROSE_FLAGS) -I$(srcdir)/../Cxx_tests -c $(@:.o=.c) &> /dev/null; \
	  then echo "***" $(@:.o=.c) " has passed !!! ***"; \
	fi;
	@rm -f $(@:.o=.c)

check-failures:
	@$(MAKE) $(C_TESTCODE_CURRENTLY_FAILING_Objects)

