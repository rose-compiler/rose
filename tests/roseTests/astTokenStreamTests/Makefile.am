include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
EXTRA_DIST =  tests
bin_PROGRAMS =
MOSTLYCLEANFILES =

# if ROSE_USE_SQLITE_DATABASE

INCLUDES = $(ROSE_INCLUDES) 

EDG_VERSION = @ROSE_EDG_MAJOR_VERSION_NUMBER@

#------------------------------------------------------------------------------------------------------------------------
# Scope matching
bin_PROGRAMS += testScopeMatching
testScopeMatching_SOURCES = testScopeMatching.C  
testScopeMatching_LDADD = $(ROSE_LIBS)

# NOTE: testScopeMatching was apparently never tested.

#------------------------------------------------------------------------------------------------------------------------
# Linearization
bin_PROGRAMS += testLinearization
testLinearization_SOURCES = testLinearization.C  
testLinearization_LDADD = $(ROSE_LIBS)

LIN_SPECIMENS = testLin1.c testLin2.c
LIN_TEST_TARGETS = $(addprefix lin_, $(addsuffix .passed, $(LIN_SPECIMENS)))
LIN_TEST_CONFIG = $(srcdir)/testLinearization.conf
$(LIN_TEST_TARGETS): lin_%.passed: ${srcdir}/tests/% testLinearization $(LIN_TEST_CONFIG)
	@$(RTH_RUN) SPECIMEN=$< EDG=$(EDG_VERSION) $(LIN_TEST_CONFIG) $@

EXTRA_DIST += $(LIN_TEST_CONFIG)

MOSTLYCLEANFILES += $(LIN_TEST_TARGETS) $(LIN_TEST_TARGETS:.passed=.failed)



#------------------------------------------------------------------------------------------------------------------------
# Token representation

TESTCODES_REQUIRED_TO_PASS = \
     test_CplusplusMacro_C.c \
     gconv_info.c \
     testCvsCpp.c \
     testAnsiC.c \
     commentTest.c \
     YardenPragmaPackExample.c \
     test2003_12.c \
     test2004_20.c \
     test2005_172.c \
     test2005_186.c \
     test2006_110.c \
     test2006_114.c \
     test2006_133.c \
     test2006_135.c \
     test2007_177.c \
     test2008_01.c \
     test2008_02.c \
     test2009_01.c \
     test2009_02.c \
     test2009_03.c \
     test2009_04.c \
     test2009_07.c \
     test2009_08.c \
     test2009_16.c \
     test2009_21.c \
     test2009_22.c \
     test2010_01.c \
     test2010_06.c \
     test2010_07.c \
     test2010_08.c \
     test2010_09.c \
     test2010_13.c \
     test2010_15.c \
     test2010_17.c \
     test2011_05.c \
     test2012_01.c \
     test2012_02.c \
     test2012_03.c \
     test2012_04.c \
     test2012_05.c \
     test2012_06.c \
     test2012_07.c \
     test2012_08.c \
     test2012_09.c \
     test2012_10.c \
     test2012_11.c \
     test2012_12.c \
     test2012_13.c \
     test2012_14.c \
     test2012_15.c \
     test2012_16.c \
     test2012_18.c \
     test2012_20.c \
     test2012_21.c \
     test2012_22.c \
     test2012_23.c \
     test2012_24.c \
     test2012_25.c \
     test2012_26.c \
     test2012_27.c \
     test2012_29.c \
     test2012_30.c \
     test2012_32.c \
     test2012_33.c \
     test2012_34.c \
     test2012_35.c \
     test2012_36.c \
     test2012_37.c \
     test2012_38.c \
     test2012_39.c \
     test2012_40.c \
     test2012_42.c \
     test2012_41.c \
     test2012_43.c \
     test2012_46.c \
     test2012_47.c \
     test2012_48.c \
     test2012_49.c \
     test2012_50.c \
     test2012_51.c \
     test2012_52.c \
     test2012_53.c \
     test2012_54.c \
     test2012_55.c \
     test2012_56.c \
     test2012_57.c \
     test2012_58.c \
     test2012_59.c \
     test2012_60.c \
     test2012_61.c \
     test2012_62.c \
     test2012_63.c \
     test2012_64.c \
     test2012_65.c \
     test2012_66.c \
     test2012_67.c \
     test2012_68.c \
     test2012_69.c \
     test2012_70.c \
     test2012_71.c \
     test2012_72.c \
     test2012_73.c \
     test2012_74.c \
     test2012_75.c \
     test2012_76.c \
     test2012_77.c \
     test2012_79.c \
     test2012_80.c \
     test2012_81.c \
     test2012_82.c \
     test2012_83.c \
     test2012_84.c \
     test2012_85.c \
     test2012_86.c \
     test2012_87.c \
     test2012_88.c \
     test2012_89.c \
     test2012_90.c \
     test2012_92.c \
     test2012_93.c \
     test2012_94.c \
     test2012_95.c \
     test2012_96.c \
     test2012_97.c \
     test2012_98.c \
     test2012_99.c \
     test2012_100.c \
     test2012_101.c \
     test2012_102.c \
     test2012_103.c \
     test2012_104.c \
     test2012_105.c \
     test2012_106.c \
     test2012_107.c \
     test2012_108.c \
     test2012_109.c \
     test2012_110.c \
     test2012_111.c \
     test2012_112.c \
     test2012_113.c \
     test2012_114.c \
     test2012_115.c \
     test2012_116.c \
     test2012_117.c \
     test2012_118.c \
     test2012_119.c \
     test2012_120.c \
     test2012_121.c \
     test2012_122.c \
     test2012_123.c \
     test2012_124.c \
     test2012_125.c \
     test2012_126.c \
     test2012_127.c \
     test2012_129.c \
     test2012_130.c \
     test2012_131.c \
     test2012_133.c \
     test2012_134.c \
     test2012_135.c \
     test2012_136.c \
     test2012_137.c \
     test2012_139.c \
     test2012_140.c \
     test2012_141.c \
     test2012_142.c \
     test2012_143.c \
     test2012_144.c \
     test2012_146.c \
     test2012_147.c \
     test2012_148.c \
     test2012_149.c \
     test2012_150.c \
     test2012_151.c \
     test2012_152.c \
     test2012_153.c \
     test2012_154.c \
     test2012_156.c \
     test2012_157.c \
     test2012_158.c \
     test2012_159.c \
     test2012_162.c \
     test2012_164.c \
     test2012_165.c \
     test2012_166.c \
     test2012_167.c \
     test2012_168.c \
     test2012_169.c \
     test2012_170.c \
     test2012_171.c \
     test2012_172.c \
     test2012_174.c \
     test2012_175.c \
     test2013_01.c \
     test2013_02.c \
     test2013_03.c \
     test2013_05.c \
     test2013_06.c \
     test2013_07.c \
     test2013_08.c \
     test2013_09.c \
     test2013_10.c \
     test2013_11.c \
     test2013_12.c \
     test2013_13.c \
     test2013_14.c \
     test2013_15.c \
     test2013_16.c \
     test2013_18.c \
     test2013_20.c \
     test2013_21.c \
     test2013_22.c \
     test2013_23.c \
     test2013_24.c \
     test2013_25.c \
     test2013_26.c \
     test2013_27.c \
     test2013_28.c \
     test2013_29.c \
     test2013_30.c \
     test2013_31.c \
     test2013_32.c \
     test2013_33.c \
     test2013_34.c \
     test2013_35.c \
     test2013_36.c \
     test2013_37.c \
     test2013_38.c \
     test2013_39.c \
     test2013_40.c \
     test2013_41.c \
     test2013_42.c \
     test2013_43.c \
     test2013_44.c \
     test2013_45.c \
     test2013_46.c \
     test2013_49.c \
     test2013_50.c \
     test2013_51.c \
     test2013_52.c \
     test2013_53.c \
     test2013_54.c \
     test2013_55.c \
     test2013_56.c \
     test2013_57.c \
     test2013_58.c \
     test2013_59.c \
     test2013_60.c \
     test2013_61.c \
     test2013_62.c \
     test2013_63.c \
     test2013_64.c \
     test2013_65.c \
     test2013_66.c \
     test2013_67.c \
     test2013_68.c \
     test2013_69.c \
     test2013_70.c \
     test2013_71.c \
     test2013_72.c \
     test2013_73.c \
     test2013_74.c \
     test2013_75.c \
     test2013_76.c \
     test2013_77.c \
     test2013_78.c \
     test2013_79.c \
     test2013_80.c \
     test2013_81.c \
     test2013_82.c \
     test2013_83.c \
     test2013_84.c \
     test2013_85.c \
     test2013_86.c \
     test2013_87.c \
     test2013_88.c \
     test2013_89.c \
     test2013_90.c \
     test2013_91.c \
     test2013_92.c \
     test2013_93.c \
     test2013_94.c \
     test2013_95.c \
     math.c \
     stdio.c \
     test2006_48.c  \
     test2006_132.c \
     test2010_04.c \
     test2010_10.c \
     test2010_12.c \
     test2010_11.c \
     test2010_14.c \
     constants.c

# DQ (3/25/2014): This is a RH5 specific tests (we don't need this test).
# test2012_173.c

# test2012_17.c and test2012_163.c fail because the recursive macro handling for "#define sa_handler union_in_X.sa_handler"
# need to be tunred off when we are able to unparse the original token stream.
FAILING_TESTS = \
   test2012_17.c \
   test2012_163.c 

# These test codes fail this token unparsing test.
#     test_multiline_string_01.c 

# These are identical files but ROSE has to generate backend specific code (e.g. "restrict" should be "__retrict__"). 
#     test2012_19.c 
#     test2012_28.c 
#     test2012_91.c 
#     test2013_17.c 
#     test2013_19.c 
#     test2013_47.c 
#     test2013_48.c
#     complex_01.c 



TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

# DQ (11/11/2014): Note that we need verbose level 1 to internally activate some internal testing that builds the 
# output files: token_leading_* and token_trailing_* which are used in the TEST_Mapping_Source_passed tests.
# DQ (12/1/2013): Use verbose level to set force additional token specific files to be generated (tested below).
# ROSE_FLAGS = -rose:C89 -rose:unparse_tokens
ROSE_FLAGS = -rose:C89 -rose:unparse_tokens -rose:verbose 1
# ROSE_FLAGS = -rose:C89 -rose:unparse_tokens -rose:verbose 0

PASSING_TEST_Source_passed = ${TESTCODES:.c=.c.passed}
TEST_Source_passed = ${ALL_TESTCODES:.c=.c.passed}

testTranslator = ../../testTranslator

# DQ (9/23/2013): Added better control to run tests more cleanly (command-line and any output put into *.passed file, or generates similar *.failed file is testing is unsuccessful).
$(TEST_Source_passed): %.passed: ../../testTranslator $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) CMD="$(testTranslator) $(ROSE_FLAGS) -c $(top_srcdir)/tests/CompileTests/C_tests/$(@:.c.passed=.c)" $(top_srcdir)/scripts/test_exit_status $@
	diff -u $(top_srcdir)/tests/CompileTests/C_tests/$(@:.c.passed=.c) ./rose_$(@:.c.passed=.c)

#------------------------------------------------------------------------------------------------------------------------
# Token Stream Mapping
bin_PROGRAMS += tokenStreamMapping testUnparsingUsingTokenStream

tokenStreamMapping_SOURCES = tokenStreamMapping.C  
tokenStreamMapping_LDADD = $(ROSE_LIBS)

testUnparsingUsingTokenStream_SOURCES = testUnparsingUsingTokenStream.C
testUnparsingUsingTokenStream_LDADD = $(ROSE_LIBS)


PASSING_TEST_Mapping_Source_passed = ${TESTCODES:.c=.c.mapping.passed}
TEST_Mapping_Source_passed = ${ALL_TESTCODES:.c=.c.mapping.passed}

# DQ (9/23/2013): Added better control to run tests more cleanly (command-line and any output put into *.passed file, or generates similar *.failed file is testing is unsuccessful).
$(TEST_Mapping_Source_passed): %.mapping.passed: tokenStreamMapping $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) CMD="./tokenStreamMapping $(ROSE_FLAGS) -c $(top_srcdir)/tests/CompileTests/C_tests/$(@:.c.mapping.passed=.c)" $(top_srcdir)/scripts/test_exit_status $@
	gcc -I$(top_srcdir)/tests/CompileTests/C_tests -c token_leading_$(@:.c.mapping.passed=.c)
	gcc -I$(top_srcdir)/tests/CompileTests/C_tests -c token_trailing_$(@:.c.mapping.passed=.c)

testTokenMapping:
	rm -f test2003_12.c.mapping.passed
	@$(MAKE) test2003_12.c.mapping.passed
	./tokenStreamMapping -rose:C89 -rose:unparse_tokens -c $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c

# Test code using -rose:verbose 2 to trigger output of evaluation files (*.c and *.dot files).
test_unparsingUsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_testUnparsingUsingTokenStream.c 

test_unparsingEmptyFileUsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_test_empty_file.c 

test_unparsingFileWithCR_UsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_test_file_with_CR.c 

test_unparsingFileWithText_UsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_test_file_with_text.c 

#------------------------------------------------------------------------------------------------------------------------

test_binary_after_token_unparsing:
	gcc $(ROSE_FLAGS) -c $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c -o gcc_test2003_12.o
	./tokenStreamMapping $(ROSE_FLAGS) -c $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c
	gcc -c GlobalScopeDeclarationTokenStream.c -o rose_test2003_12.o
	diff $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c GlobalScopeDeclarationTokenStream.c
	diff gcc_test2003_12.o rose_test2003_12.o

#------------------------------------------------------------------------------------------------------------------------
EXTRA_DIST = input_testUnparsingUsingTokenStream.c input_test_empty_file.c input_test_file_with_CR.c input_test_file_with_text.c \
             input_test_02.c


# Additional automake targets
# check-local: $(LIN_TEST_TARGETS)
check-local:
	@$(MAKE) test_unparsingUsingTokens
	@$(MAKE) test_unparsingEmptyFileUsingTokens
	@$(MAKE) test_unparsingFileWithCR_UsingTokens
	@$(MAKE) test_unparsingFileWithText_UsingTokens
	@$(MAKE) $(PASSING_TEST_Mapping_Source_passed)
	@$(MAKE) $(PASSING_TEST_Source_passed)
#	@$(MAKE) $(LIN_TEST_TARGETS)
	@echo "******************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astTokenStreamTests: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************"


clean-local:
	rm -f rose*.c *.o
	rm -f *.passed *.failed *.err
	rm -f *.dot
	rm -f token_leading_*.c
	rm -f token_trailing_*.c


# endif
