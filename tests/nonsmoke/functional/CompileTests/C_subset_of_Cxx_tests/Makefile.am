include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

######################################################
# These are C testcodes targeting aspects of both C  #
# and C++ that can be compiled using a C++ compiler  #
######################################################

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
   test2005_56.C \
   test2005_62.C \
   test2005_84.C \
   test2005_92.C \
   test2005_95.C \
   test2005_100.C \
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
   test2008_04.C \
   test2010_16.C \
   test_int_lexemes.C \
   test_int_lexemes_donot_pass.C \
   test_simple_int.C \
   test_wchars.C \
   X.C

TESTCODE_CURRENTLY_FAILING = \
   test2003_12.C \
   test2003_17.C \
   test2004_07.C \
   test2004_06.C \
   test2004_151.C \
   test2004_155.C \
   test2005_158.C \
   test2005_166.C \
   test2005_175.C \
   test2005_177.C \
   test2005_192.C \
   test2005_200.C \
   test2005_201.C \
   test2006_15.C \
   test2006_33.C \
   test2006_49.C \
   test2006_59.C \
   test2006_71.C \
   test2006_80.C \
   test2006_98.C \
   test2006_101.C \
   test2006_102.C \
   test2006_139.C \
   test2006_155.C \
   test2006_156.C \
   test2006_157.C \
   test2006_158.C \
   test2006_159.C \
   test2006_160.C \
   test2006_165.C \
   test2006_44.C \
   test_CplusplusMacro_Cpp.C



# DQ (9/17/2020): This is a new organization of the files that are passing only specific compilers.
if USING_INTEL_COMPILER
# INTEL compiler
#      EXAMPLE_TESTCODES_REQUIRED_TO_PASS += 

else
if USING_CLANG_COMPILER
# CLANG compiler
#      TESTCODE_CURRENTLY_FAILING += 

#      EXAMPLE_TESTCODES_REQUIRED_TO_PASS += 
else
if OS_MACOSX
#      EXAMPLE_TESTCODES_REQUIRED_TO_PASS += 
else
#  EXAMPLE_TESTCODES_REQUIRED_TO_PASS += 

# GNU compiler
if ROSE_USING_GCC_VERSION_LATER_10_1
      # skip these codes when using GNU 10.1 (testing)
else
      # DQ (7/23/2020): These are failing code for EDG 6.0 and GNU 10.1 (debugging this).
#     EXAMPLE_TESTCODES_REQUIRED_TO_PASS += 
endif
    # The test codes fail CLANG 10.0
      EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
      test2005_111.C 
endif
endif
endif



if ROSE_USE_LONG_MAKE_CHECK_RULE
  TESTCODES_REQUIRED_TO_PASS = \
     $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
else
  TESTCODES_REQUIRED_TO_PASS = \
     test2001_02.C \
     test2001_03.C 
endif

TESTCODES = \
   $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = \
   $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS) \
   $(TESTCODE_CURRENTLY_FAILING)

#########
# Targets lists

PASSING_TEST_Source_passed = ${TESTCODES:.C=.C.passed}
TEST_Source_passed = ${ALL_TESTCODES:.C=.C.passed}

#########
# Translator to be used: calling "make TEST_TRANSLATOR=../../testBackend" uses the backend compiler only

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

../../testBackend:
	cd ../..; $(MAKE) testBackend

testCompiler:
	ln -s `which @BACKEND_C_COMPILER@` testCompiler

TEST_TRANSLATOR=../../testTranslator

#########
# ROSE_FLAGS

LANG_FLAGS = -std=c

ROSE_FLAGS =
if !ROSE_USE_CLANG_FRONTEND
   ROSE_FLAGS += --edg:no_warnings --edg:restrict
endif
ROSE_FLAGS += -w -rose:verbose 0 -rose:skip_unparse_asm_commands

#########
# RTH test targets

$(TEST_Source_passed): %.passed: $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
	@cp $(srcdir)/../Cxx_tests/$(@:.C.passed=.C) $(@:.C.passed=.c)
	@$(RTH_RUN) \
	   CMD="$(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) -I$(srcdir)/../Cxx_tests -c $(@:.C.passed=.c)" \
	   TITLE="TEST: $(TEST_TRANSLATOR) $(@:.C.passed=.C)" \
	   $(top_srcdir)/scripts/test_exit_status $@
	@rm -f $(@:.C.passed=.c)

test2005_56.C.passed: $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
	@cp $(srcdir)/../Cxx_tests/$(@:.C.passed=.C) $(@:.C.passed=.c)
	@$(RTH_RUN) \
	   CMD="$(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) -I$(srcdir)/../Cxx_tests -DTEST_STRING_MACRO=\"\\\"Thu Apr 14 08:18:33 PDT 2005\"\\\" -c $(@:.C.passed=.c)" \
	   TITLE="TEST: $(TEST_TRANSLATOR) $(@:.C.passed=.C)" \
	   $(top_srcdir)/scripts/test_exit_status $@
	@rm -f $(@:.C.passed=.c)

#
############

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for C language subset of C++ (some features in C are disallowed in C++ and are tested in C_tests directory)."
	@$(MAKE) $(PASSING_TEST_Source_passed)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/C_subset_of_Cxx_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest

status:
	@$(MAKE) $(TEST_Source_passed)

