include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_TARGETS =
EXTRA_DIST =

# The test program.
TEST_TRANSLATOR = ../../testTranslator
$(TEST_TRANSLATOR):
	$(MAKE) -C ../.. testTranslator


###############################################################################################################################
# ALL TEST SPECIMENS, REGARDLESS OF WHETHER THEY PASS OR FAIL FOR TESTS IN THIS MAKEFILE.
#
# List of test specimens (inputs to tests that are run in this makefile) come from the global list stored in
# the following include file, which defines variables ROSE__CXX_* (watch out for the double underscore).
include $(top_srcdir)/tests/input_codes/cxx/Makefile_variables

SPECIMENS_all = $(ROSE__CXX_TESTS)

###############################################################################################################################
# SPECIMENS FOR WHICH TESTS IN THIS MAKEFILE FAIL.
#
# The following lists are the file names.  Use a separate variable for each reason-of-failure so its easy to add/remove
# whole categories.  At the end of this section of the makefile there's a SPECIMENS_FAILING that puts them all together
# into a single list.  It is Okay for a specimen to appear in multiple lists if it fails for multiple reasons.
#
# To check whether a test of a specimen FOO.C marked as failing actually fails, you can run "make compilation-FOO.C.passed". To
# get a list of which marked-as-failing specimens pass and which fail, run "make -k check-compilation-failing" and then look
# at the list of *.failed vs. *.passed files (you probably want to do a "make clean" before that).
#
# Note: As of today, 33 of the 91 specimens that are marked as failing actually do pass, but I am not commenting them
#       out (marking them as passing) because the original makefile didn't check them either. [RPM 2013-04-07]

# Tests fail for these specimens on Debian Squeeze with the error "(problem with Boost code in ROSE compiling ROSE)"
# Failing on 2014-04-04 using boost1.47 (RPM)
SPECIMENS_failing_boost_debian_squeeze = \
	test2013_242.C

# Note that these will not compile with GNU 4.7 header files.  It might be a boost issue. The error is always the same
SPECIMENS_failing_boost_gcc47 =			\
	test2013_246.C

# Test failing for switch without case statement (made up example, but we will want it to pass as part of further switch
# statement work later. DQ (9/10/2013)
SPECIMENS_failing_switch =			\
	test2013_254.C

# This is a failing test code for operator->(). DQ (7/31/2013)
SPECIMENS_failing_arrow_operator =		\
	test2013_249.C

# Tests fail for these specimens on OSX. DQ (2/15/2012): 
SPECIMENS_failing_mac_osx =			\
	test2011_06.C				\
	test2004_35.C 

# Tests for these specimens fail with EDG 4.7.  Skip these while we report the problem to EDG and investigate more internally.
# EDG complains: line 13: error: constructor or destructor may not have its address taken.
SPECIMENS_failing_edg47 =
if ROSE_USE_EDG_VERSION_4_7
SPECIMENS_failing_edg47 += test2012_68.C
endif

# This is failing Qt example code (unparsed code is not correct). DQ (7/3/2013)
SPEICMENS_failing_qt =				\
	test2013_245.C

# This is a failing test because we fixup the symbol tables and then compute the name qualification (and with the fixed up
# symbol tables we get the name qualification wrong).  Not clear how to fix this at the moment, it was a made up test that I
# will work on later. DQ (5/23/2013)
SPECIMENS_failing_name_qualification =		\
	test2013_185.C

# Failing regression tests (result of fix for operator vs. non-operator syntax in function calls). DQ (4/21/2013)
SPECIMENS_failing_operator_syntax =		\
	test2013_117.C 

# Failing test codes as a result of name qualification on friend class declaration. DQ (2/16/2013)
SPECIMENS_failing_friend_declaration =		\
	test2013_62.C 

# Specimens that demonstrate expression precedence problems.
SPECIMENS_failing_operator_precedence =		\
	test2013_41.C				\
	test2013_42.C				\
	test2013_43.C

# Tests that where apparently omitted in an older version of this makefile.  The comments were:
#     Comment: TV (08/02/12): Added a list of none previously referenced test codes, code to be sorted
#     Comment: This source files are listed as unused source files and thus violate the unused source file ploicy so let's
#              list them explicitly here
SPECIMENS_failing_not_referenced =		\
	c++trick.C				\
	sgnodee.hpp				\
	test2006_136.h				\
	test2007_113.h				\
	test2007_114.h				\
	test2007_40.h				\
	test2007_95.C				\
	test2008_07.C				\
	test2008_15.C				\
	test2009_23.C				\
	test2009_26.C				\
	test2010_20.C				\
	test2010_21.C				\
	test2010_23.C				\
	test2011_102.C				\
	test2011_103.C				\
	test2011_104.C				\
	test2011_105.C				\
	test2011_106.C				\
	test2011_107.C				\
	test2011_108.C				\
	test2011_109.C				\
	test2011_112.C				\
	test2011_122.C				\
	test2011_123.C				\
	test2011_124.C				\
	test2011_125.C				\
	test2011_126.C				\
	test2011_127.C				\
	test2011_128.C				\
	test2011_129.C				\
	test2011_130.C				\
	test2011_131.C				\
	test2011_132.C				\
	test2011_134.C				\
	test2011_144.C				\
	test2011_145.C				\
	test2011_148.C				\
	test2011_178.C				\
	test2011_196.C				\
	test2011_197.C				\
	test2011_198.C				\
	test2011_199.C				\
	test2011_200.C				\
	test2011_201.C				\
	test2011_202.C				\
	test2011_203.C				\
	test2012_133.C				\
	test2012_134.C				\
	test2012_135.C				\
	test2012_139.C				\
	test2012_140.C				\
	test2012_178.h				\
	test2012_189.C				\
	test2012_35.C				\
	test2012_79.C				\
	test2013_13.C				\
	test2013_14.C				\
	test2013_15.C				\
	test2013_19.C				\
	test2013_24.C				\
	test2013_30.C				\
	test2013_35.C				\
	test2013_41.C				\
	test2013_42.C				\
	test2013_43.C				\
	test2013_45.C				\
	test2013_49.C				\
	test_gnu.C 

# DQ (8/12/2012): This test code appears to demonstrate a loss of the template arguments in the name qualification.
SPECIMENS_failing_template_arguments =		\
	test2012_189.C

# DQ (2/19/2012): These appear to fail only sometimes and were noticed just before going on travel (test with valgrind).
SPECIMENS_failing_statistically =		\
	test2007_62.C				\
	math.C 

# DQ (5/17/2012): These are new tests which I deamed up and which fail.  they likely failed in the older version of EDG
# support, but were not tested there.  Since the current focus is on the previous level of robustness. These are considered to
# be NEW failing tests to work on later (when the previously passing tests are fixed in the newer EDG support).
SPECIMENS_failing_newly =			\
	test2012_86.C

# These were listed in the original makefile without any reasons. The comments are what was in the makefile.
# Comment: Current problem test codes
# Comment: DQ (5/26/2013): Currently failing tests
# Comment: DQ (1/21/2013): Failing test codes:
# Comment: Variation of regression test that fails.
SPECIMENS_failing_miscellaneous =		\
	test2012_252.C				\
	test2013_13.C 				\
	test2013_14.C 				\
	test2013_15.C 				\
	test2013_197.C				\
	test2013_19.C 				\
	test2013_262.C				\
	test2013_263.C				\
	test2013_264.C				\
	test2013_265.C				\
	test2013_266.C				\
	test2013_267.C				\
	test2013_268.C				\
	test2013_269.C 				\
	test2013_30.C 				\
	test2013_35.C 


# The master list of all specimens which cause tests in this makefile to fail.  Do not comment things out here, use the
# individual "SPECIMENS_failing_*" lists above instead in order to keep this makefile from becomming a mess again.
# The names of this list must be unique otherwise make will complain about duplicate rules when we use this list of file
# names to create makefile targets.
SPECIMENS_failing = $(shell echo				\
	$(SPECIMENS_failing_boost_debian_squeeze)		\
	$(SPECIMENS_failing_boost_gcc47)			\
	$(SPECIMENS_failing_switch)				\
	$(SPECIMENS_failing_arrow_operator)			\
	$(SPECIMENS_failing_mac_osx)				\
	$(SPECIMENS_failing_edg47)				\
	$(SPECIMENS_failing_qt)					\
	$(SPECIMENS_failing_name_qualification)			\
	$(SPECIMENS_failing_operator_syntax)			\
	$(SPECIMENS_failing_friend_declaration)			\
	$(SPECIMENS_failing_operator_precedence)		\
	$(SPECIMENS_failing_not_referenced)			\
	$(SPECIMENS_failing_template_arguments)			\
	$(SPECIMENS_failing_statistically)			\
	$(SPEICMENS_failing_newly)				\
	$(SPECIMENS_failing_miscellaneous)			\
		|tr ' ' '\n' |sort |uniq)


###############################################################################################################################
# SPECIMENS THAT MUST PASS TESTS IN THIS MAKEFILE REGARDLESS OF WHETHER THEY'RE MARKED AS FAILING ABOVE.
#
# The original makefile confusingly started with a list of all specimens, then removed specimens that were marked as being
# problematic, and then added tests again in a long list.
#
# DONT ADD MORE STUFF HERE!!! Only delete stuff from the SEPCIMEN_failing_* lists above.
#
SPECIMENS_required_to_pass =			\
	lulesh.C				\
	luleshTALC.C				\
	test2013_253.C				\
	test2013_255.C				\
	test2013_256.C				\
	test2013_257.C				\
	test2013_258.C				\
	test2013_259.C				\
	test2013_260.C				\
	test2013_261.C				\
	test2013_271.C				\
	test2013_272.C				\
	test2013_273.C				\
	test2013_274.C				\
	test2013_275.C				\
	test2013_276.C				\
	test2013_277.C				\
	test2013_278.C				\
	test2013_279.C				\
	test2013_280.C				\
	test2013_281.C				\
	test2013_282.C				\
	test2013_283.C				\
	test2013_284.C				\
	test2013_285.C				\
	test2013_286.C				\
	test2013_287.C				\
	test2013_288.C				\
	test2013_289.C				\
	test2013_290.C				\
	test2013_291.C				\
	test2013_292.C				\
	test2013_293.C				\
	test2013_294.C				\
	test2013_295.C				\
	test2013_296.C				\
	test2014_01.C				\
	test2014_02.C				\
	test2014_03.C				\
	test2014_04.C				\
	test2014_05.C				\
	test2014_06.C

###############################################################################################################################
# UNREFERENCED LISTS
#
# The following lists were present in the original makefile but were unused by that makefile.  We're keeping them for
# historical purposes only.

# DQ (8/6/2012): These are the previously failing test with the older version of ROSE
PASSING_PREVIOUSLY_FAILING_TESTS =											\
	certSignalExample.C longFile.C test2005_151.C test2005_200.C test2005_204.C test2005_69.C test2005_85.C 	\
	test2006_09.C test2006_127.C test2006_15.C test2006_167.C test2006_168.C test2006_169.C test2006_172.C 		\
	test2006_174.C test2006_175.C test2006_176.C test2006_180.C test2006_182.C test2006_186.C test2006_188.C 	\
	test2006_27.C test2006_64.C test2007_100.C test2007_104.C test2007_106.C test2007_107.C test2007_110.C 		\
	test2007_111.C test2007_114.C test2007_116.C test2007_117.C test2007_118.C test2007_119.C test2007_120.C 	\
	test2007_121.C test2007_122.C test2007_126.C test2007_127.C test2007_129.C test2007_131.C test2007_132.C 	\
	test2007_134.C test2007_136.C test2007_143.C test2007_145.C test2007_148.C test2007_149.C test2007_156.C 	\
	test2007_168.C test2007_174.C test2007_176.C test2007_18.C test2007_19.C test2007_20.C test2007_22.C 		\
	test2007_27.C test2007_31.C test2007_32.C test2007_33.C test2007_34.C test2007_36.C test2007_37.C 		\
	test2007_39.C test2007_45.C test2007_51.C test2007_52.C test2007_56.C test2007_58.C test2007_61.C 		\
	test2007_63.C test2007_67.C test2007_68.C test2007_71.C test2007_73.C test2007_74.C test2007_76.C 		\
	test2007_80.C test2007_86.C test2007_87.C test2007_88.C test2007_89.C test2007_91.C test2007_96.C 		\
	test2007_97.C test2007_99.C test2009_07.C test2009_13.C test2009_22.C test2009_33.C test2009_34.C 		\
	test2009_38.C test2009_44.C test2009_46.C test2009_47.C test2010_07.C test2010_09.C test2010_10.C 		\
	test2010_11.C test2010_15.C 


# DQ (8/6/2012): This is the list of test code that were failing before the move to EDG 4.3 (failing with the use of EDG 3.3)
# DQ (8/6/2012): Original count is 183 failing tests that have never passed previously.
NEVER_PASING_TESTCODE_CURRENTLY_FAILING = 										\
	lexPhase2003_02.C test2004_25.C test2005_69.C test2005_85.C test2005_115.C test2005_151.C test2005_188.C 	\
	test2005_200.C test2005_201.C test2005_204.C test2005_205.C test2006_09.C test2006_11.C test2006_15.C 		\
	test2006_21.C test2006_24.C test2006_27.C test2006_33.C test2006_35.C test2006_36.C test2006_49.C 		\
	test2006_60.C test2006_64.C test2006_127.C test2006_128.C test2006_130.C test2006_131.C test2006_138.C 		\
	test2006_167.C test2006_168.C test2006_169.C test2006_170.C test2006_171.C test2006_172.C test2006_173.C 	\
	test2006_174.C test2006_175.C test2006_176.C test2006_180.C test2006_182.C test2006_184.C test2006_186.C 	\
	test2006_187.C test2006_188.C test2007_18.C test2007_19.C test2007_20.C test2007_22.C test2007_27.C 		\
	test2007_30.C test2007_31.C test2007_32.C test2007_33.C test2007_34.C test2007_35.C test2007_36.C 		\
	test2007_37.C test2007_39.C test2007_42.C test2007_45.C test2007_51.C test2007_52.C test2007_54.C 		\
	test2007_56.C test2007_58.C test2007_60.C test2007_61.C test2007_63.C test2007_64.C test2007_65.C 		\
	test2007_66.C test2007_67.C test2007_68.C test2007_69.C test2007_71.C test2007_73.C test2007_74.C 		\
	test2007_76.C test2007_79.C test2007_80.C test2007_86.C test2007_87.C test2007_88.C test2007_89.C 		\
	test2007_91.C test2007_92.C test2007_93.C test2007_96.C test2007_97.C test2007_99.C test2007_100.C 		\
	test2007_101.C test2007_104.C test2007_105.C test2007_106.C test2007_107.C test2007_108.C test2007_109.C 	\
	test2007_110.C test2007_111.C test2007_112.C test2007_113.C test2007_114.C test2007_115.C test2007_116.C 	\
	test2007_117.C test2007_118.C test2007_119.C test2007_120.C test2007_121.C test2007_122.C test2007_123.C 	\
	test2007_124.C test2007_125.C test2007_126.C test2007_127.C test2007_129.C test2007_131.C test2007_132.C 	\
	test2007_134.C test2007_136.C test2007_138.C test2007_139.C test2007_143.C test2007_144.C test2007_145.C 	\
	test2007_146.C test2007_148.C test2007_149.C test2007_153.C test2007_156.C test2007_168.C test2007_174.C 	\
	test2007_175.C test2007_176.C test2009_07.C test2009_13.C test2009_17.C test2009_21.C test2009_22.C 		\
	test2009_29.C test2009_32.C test2009_33.C test2009_34.C test2009_35.C test2009_36.C test2009_38.C 		\
	test2009_43.C test2009_44.C test2009_45.C test2009_46.C test2009_47.C test2010_01.C test2010_02.C 		\
	test2010_07.C test2010_08.C test2010_09.C test2010_10.C test2010_11.C test2010_12.C test2010_15.C 		\
	test2010_16.C test2010_17.C test2010_22.C test2010_27.C test2011_01.C test2011_02.C test2011_05.C 		\
	test2011_06.C test2011_07.C test2011_08.C test2011_09.C test2011_10.C test2011_11.C test2011_12.C 		\
	test2011_13.C test2011_14.C test2011_15.C test2011_16.C test2011_17.C test2011_18.C longFile.C 			\
	certSignalExample.C test2010_26.C 

###############################################################################################################################
# Make sure that all the test codes end up in the distribution

EXTRA_DIST += $(SPECIMENS_all) $(SPECIMENS_required_to_pass)

###############################################################################################################################
# A test that tries to compile each specimen.
# Only the specimens that don't fail are tested by "make check".  If you want to test specimen foo.C that's marked as failing
# then run "make compile-foo.C.passed"

# The input specimens to which this test applies. Modify specimen lists above rather than changing this variable (or if
# you change this, at least adjust the documentation above, especially for SPECIMENS_required_to_pass).
COMPILATION_TEST_SPECIMENS =					\
	$(filter-out $(SPECIMENS_failing), $(SPECIMENS_all))	\
	$(SPECIMENS_required_to_pass)

COMPILATION_TEST_SPECIMENS_FAILING = $(SPECIMENS_failing)

# The makefile targets for this test.  We define one target per (test,specimen) pair so that the test can run across
# all the specimens in parallel.
COMPILATION_TEST_TARGETS = $(addprefix compile-, $(addsuffix .passed, $(COMPILATION_TEST_SPECIMENS)))
COMPILATION_TEST_TARGETS_FAILING = $(addprefix compile-, $(addsuffix .passed, $(COMPILATION_TEST_SPECIMENS_FAILING)))
TEST_TARGETS += $(COMPILATION_TEST_TARGETS)

# Command-line switches for running this test on each of the specimens.
# DQ (3/10/2013): Add the Boost path so that the case where Boost is not installed in /usr/include is covered.
COMPILATION_TEST_FLAGS =			\
	--edg:no_warnings 			\
	-w 					\
	-rose:verbose 0 			\
	-rose:skip_unparse_asm_commands		\
	$(ROSE_BOOST_NORMAL_INCLUDE_PATH)	\
	-DBOOST_REGEX_MATCH_EXTRA		\
	-I$(srcdir)

# Makefile target that will run only these compilation tests and not all the tests defined in this makefile.
.phony: check-compilation check-compilation-failing
check-compilation: $(COMPILATION_TEST_TARGETS)
check-compilation-failing: $(COMPILATION_TEST_TARGETS_FAILING)

# The targets for running this compilation test on each of the input specimens.
# I believe that the ROSE_USE_INSURE was not being used; the rule would not have worked as written. [RPM 2014-04-04]
$(COMPILATION_TEST_TARGETS) $(COMPILATION_TEST_TARGETS_FAILING): compile-%.passed: % $(TEST_TRANSLATOR)
if !ROSE_USE_INSURE
	@$(RTH_RUN)											\
		TITLE="$(notdir $(TEST_TRANSLATOR)) $* [$@]"						\
		USE_SUBDIR=yes										\
		CMD="$(abspath $(TEST_TRANSLATOR)) $(COMPILATION_TEST_FLAGS) -c $(srcdir)/$*"		\
		$(top_srcdir)/scripts/test_exit_status $@
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
	$(VALGRIND) $(TEST_TRANSLATOR) $(COMPILATION_TEST_FLAGS) -c $(srcdir)/$*
endif

###############################################################################################################################
# ONE-OFF MISCELLANEOUS TESTS THAT ARE PART OF "make check"

# Some of these test codes reference A++ header files as part of their tests
# Include the path to A++ and the transformation specification
APP_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skip_unparse_asm_commands

# DQ (9/13/2013): Added new test for -rose:unparse_includes (which has a bug in how it unparses C++ style comments
# and should maybe not be unparsing declarations that are front-end specific (e.g. builtin function declarations).
TEST_TARGETS += test_unparse_includes_option.passed
test_unparse_includes_option_flags =								\
	$(ROSE_FLAGS) -rose:unparse_includes -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES)	\
	-c $(srcdir)/test2013_257.C -o test2013_257_unparse_headers.o
test_unparse_includes_option.passed: $(TEST_TRANSLATOR)
	@$(RTH_RUN)										\
		USE_SUBDIR=yes									\
		CMD="$(abspath $(TEST_TRANSLATOR)) $(test_unparse_includes_option_flags)"	\
		$(top_srcdir)/scripts/test_exit_status $@

# DQ (9/16/2013): Test linking the resulting translated file. Phil Miller reported an error and the issue
# is that va_start() and va_end() must be unparsed to be __builtin_va_start() and __builtin_va_end() (at least for GNU).
TEST_TARGETS += test_unparse_includes_option_with_link.passed
test_unparse_includes_option_with_link_flags =							\
	$(ROSE_FLAGS) -rose:unparse_includes -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES)	\
	$(srcdir)/test2013_260.C -o test2013_260_unparse_headers
test_unparse_includes_option_with_link.passed: $(TEST_TRANSLATOR)
	@$(RTH_RUN)											\
		USE_SUBDIR=yes										\
		CMD="$(abspath $(TEST_TRANSLATOR)) $(test_unparse_includes_option_with_link_flags)"	\
		$(top_srcdir)/scripts/test_exit_status $@

# DQ (9/17/2013): Added autoconf style link test.
TEST_TARGETS += test_common_configure_test_with_link_part_1.passed
test_with_link_part_1_flags = \
	$(ROSE_FLAGS) -I$(srcdir) conftest.C -o conftest_configure_test_part_1
test_common_configure_test_with_link_part_1.passed: $(TEST_TRANSLATOR)
	@$(RTH_RUN)												\
		USE_SUBDIR=yes											\
		CMD="cp $(srcdir)/conftest.C . && $(abspath $(TEST_TRANSLATOR)) $(test_with_link_part_1_flags)"	\
		$(top_srcdir)/scripts/test_exit_status $@

# DQ (9/22/2013): Added autoconf style link test.
TEST_TARGETS += test_common_configure_test_with_link_part_2.passed
test_with_link_part_2_flags = \
	$(ROSE_FLAGS) -I$(srcdir) conftest.C -o conftest_configure_test_part_2
test_common_configure_test_with_link_part_2.passed: $(TEST_TRANSLATOR)
	@$(RTH_RUN)														\
		USE_SUBDIR=yes													\
		CMD="cp $(srcdir)/conftest_2.C conftest.C && $(abspath $(TEST_TRANSLATOR)) $(test_with_link_part_2_flags)"	\
		$(top_srcdir)/scripts/test_exit_status $@

# DQ (9/22/2013): Added autoconf style link test.
TEST_TARGETS += test_common_configure_test_with_link_part_3.passed
test_with_link_part_3_flags = \
	$(ROSE_FLAGS) -I$(srcdir) conftest.C -o conftest_configure_test_part_3
test_common_configure_test_with_link_part_3.passed: $(TEST_TRANSLATOR)
	@$(RTH_RUN)														\
		USE_SUBDIR=yes													\
		DISABLED="I think this fails on later versions of GNU (e.g. gnu 4.4.5). (DQ 9/22/2013)"				\
		CMD="cp $(srcdir)/conftest_3.C conftest.C && $(abspath $(TEST_TRANSLATOR)) $(test_with_link_part_3_flags)"	\
		$(top_srcdir)/scripts/test_exit_status $@

TEST_TARGETS += test2013_09.C.passed
test2013_09.C.passed: $(srcdir)/test2013_09.C $(TEST_TRANSLATOR)
	@$(RTH_RUN)												\
		TITLE="Running boost specific test [$@]"							\
		USE_SUBDIR=yes											\
		CMD="$(abspath $(TEST_TRANSLATOR)) $(ROSE_FLAGS) -I$(top_builddir) $(APP_INCLUDES) -c $<"	\
		$(top_srcdir)/scripts/test_exit_status $@


TEST_TARGETS += test2013_235.C.passed
test2013_235.C.passed: $(srcdir)/test2013_235.C $(TEST_TRANSLATOR)
	@$(RTH_RUN)												\
		TITLE="Running boost specific test [$@]"							\
		USE_SUBDIR=yes											\
		CMD="$(abspath $(TEST_TRANSLATOR)) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $<"	\
		$(top_srcdir)/scripts/test_exit_status $@

TEST_TARGETS += test2013_236.C.passed
test2013_236.C.passed: $(srcdir)/test2013_236.C $(TEST_TRANSLATOR)
	@$(RTH_RUN)												\
		TITLE="Running boost specific test [$@]"							\
		USE_SUBDIR=yes											\
		CMD="$(abspath $(TEST_TRANSLATOR)) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $<"	\
		$(top_srcdir)/scripts/test_exit_status $@

###############################################################################################################################
# THE FOLLOWING TESTS WERE NOT ORIGINALLY RUN BY "make check" AND MAY OR MAY NOT ACTUALLY WORK. 

ALL_TESTCODES = $(SPECIMENS_all) $(SPECIMENS_required_to_pass)
TEST_Sources = ${ALL_TESTCODES:.C=.C.passed}
TESTCODE_INCLUDES = $(ROSE_BOOST_NORMAL_INCLUDE_PATH) -DBOOST_REGEX_MATCH_EXTRA

# DQ (9/18/2011): Added testing for new option to use folded constants or the original express trees.
TEST_Objects_FoldedConstants = ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS_FOLDED_CONSTANTS:.C=.foldedConstants-o}
TEST_Objects_UnfoldedConstants = ${ALL_TESTCODES:.C=.unfoldedConstants-o}

# VALGRIND_OPTIONS = -v --num-callers=12 --leak-check=yes --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# DQ (9/18/2011): This is testing the option to process the AST to use the constant folded values (contained in the AST).
$(TEST_Objects_FoldedConstants): ../../testTranslatorFoldedConstants
	$(VALGRIND) ../../testTranslatorFoldedConstants $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.foldedConstants-o=.C) && touch $@
#	$(VALGRIND) ../../testFrontEnd $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

# DQ (9/18/2011): This is the equivalent to the default using testTranslator (so not critical to run this).
$(TEST_Objects_UnfoldedConstants): ../../testTranslatorUnfoldedConstants
	$(VALGRIND) ../../testTranslatorUnfoldedConstants $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.unfoldedConstants-o=.C) && touch $@

# DQ (4/6/2010): Added test for parsing only (tested here for C++ code)
testParser:../../testParser
	$(VALGRIND) ../../testParser $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/test2001_01.C

# This requires a special makefile rule
test2005_56.o: $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -c $(srcdir)/test2005_56.C -o test2005_56.o

test2010_01.o: $(TEST_TRANSLATOR)
	@echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) $(ROSE_INCLUDES) -c $(srcdir)/test2010_01.C -o test2010_01.o

test2010_02.o: $(TEST_TRANSLATOR)
	@echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_02.C -o test2010_02.o

test2010_07.o: $(TEST_TRANSLATOR)
	@echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_07.C -o test2010_07.o
#	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) -I$(top_srcdir)/src -I$(top_srcdir)/src/util -c $(srcdir)/test2010_07.C -o test2010_07.o

test2010_08.o: $(TEST_TRANSLATOR)
	@echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_08.C -o test2010_08.o

cpp_test2010_07: $(TEST_TRANSLATOR)
	@echo "Running boost specific test."
	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_07.C



../../testTranslatorFoldedConstants:
	cd ../..; $(MAKE) testTranslatorFoldedConstants

../../testTranslatorUnfoldedConstants:
	cd ../..; $(MAKE) testTranslatorUnfoldedConstants


CURRENT_DIRECTORY = `pwd`
QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

$(QMTEST_Objects): $(TEST_TRANSLATOR)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/$(TEST_TRANSLATOR) NULL $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# This must be placed after the more general rule to work properly!
test2005_56.qmt: $(TEST_TRANSLATOR)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/$(TEST_TRANSLATOR) NULL $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# DQ (6/18/2008): All of these should be present...
# confdefs.h     test2001_11.h   test2001_18B.h  test2003_12.h  test2003_25.h   test2005_154.h  test2005_159.h  test2005_26.h  test2005_88.h    test2006_187b.h  test2006_42.h   test2007_113.h  test2007_137.h   test2007_167.h  test2007_42.h  test2007_97.h
# foo.h          test2001_15.h   test2003_08.h   test2003_14.h  test2004_40.h   test2005_157.h  test2005_15.h   test2005_75.h  test2006_136.h   test2006_33.h    test2007_08.h   test2007_114.h  test2007_160a.h  test2007_40.h   test2007_47.h
# test2001_05.h  test2001_18A.h  test2003_09.h   test2003_24.h  test2005_133.h  test2005_158.h  test2005_160.h  test2005_81.h  test2006_187a.h  test2006_36.h    test2007_101.h  test2007_136.h  test2007_160.h   test2007_41.h   test2007_48.h

# This was a bug specific to the normalization of source file names to absolute paths
# DQ (12/11/2009): At the moment it is an failing test because "thisIsAnError" will be 
# interpreted to be a file.
testUsingBadCommandline: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -DthisIsOK -D thisIsAnError -rose:verbose 0 roseTestProgram.C

# This was a bug specific to the normalization of source file names to absolute paths
testBuildUsingNoPath: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -rose:verbose 0 roseTestProgram.C

# This was a bug specific to the normalization of source file names to absolute paths
testBuildUsingRelativePath: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -rose:verbose 0 ../Cxx_tests/roseTestProgram.C

# This was always worked but is the alternative to the normalization of source file names to absolute paths
testBuildUsingAbsolutePath: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -rose:verbose 0 `pwd`/roseTestProgram.C

# DQ (10/23/2010): Added MPI tests that I can at least run locally to debug when Hudson MPI tests fail.
MPI_LOCATION = /home/dquinlan/ROSE/ROSE_TESTS/MPI_header_tests/MPI_header_tests_64bit
test_MPI_LAM:
	$(TEST_TRANSLATOR) -rose:verbose 0 -I$(MPI_LOCATION)/LAM_MPI/include -c $(MPI_LOCATION)/tests/input.C

test_multiple_files: $(srcdir)/test2012_246a.C $(srcdir)/test2012_246b.C
	../../testTranslator -rose:verbose 1 -c $(srcdir)/test2012_246a.C $(srcdir)/test2012_246b.C 

# DQ (7/24/2011): These test fail the AST IO but pass the compilation tests.  Debug these for AST File I/O separately.
failingAST_IO_tests: test2011_113.o test2011_114.o test2011_116.o

print_testcodes:
	echo $(TESTCODES)

# Added performance test for ROSE evaluation:
performance_timing:
	../../testGraphGeneration --edg:no_warnings -w -rose:verbose 3 -rose:skip_unparse_asm_commands -rose:skipAstConsistancyTests -rose:skip_commentsAndDirectives -rose:skip_unparse -I$(srcdir) -c $(srcdir)/testPerformance.C

# DQ (11/19/2012): HPCToolKit use.  This makes some significant assumptions on how we have setup HPCToolKit.
hpctoolkit:
	hpcrun -e WALLCLOCK -- ../../testTranslator --edg:no_warnings -w -rose:verbose 0 -rose:skip_unparse_asm_commands -rose:skipAstConsistancyTests -rose:skip_commentsAndDirectives -rose:skip_unparse -I/home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/Cxx_tests -c /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/Cxx_tests/testPerformance.C
#	hpcprof hpctoolkit-lt-testTranslator-measurements/lt-testTranslator-000000-000-0f802c45-7612-0.hpcrun
	hpcprof hpctoolkit-lt-testTranslator-measurements/lt-testTranslator-000000-000-0f802c45-3676-0.hpcrun
#	hpcviewer /home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.2.4-dq-edg4x-rc-debug_mode_off/tests/CompileTests/Cxx_tests/hpctoolkit-lt-testTranslator-database-11205 &



#------------------------------------------------------------------------------
#                           Performance Timing Tests
#------------------------------------------------------------------------------
## Purpose
#
#  Enforce a time limit constraint on testcode compilation.
#
### Make Targets
#
#  Note: A testcode's object files are removed before running the individual
#  steps in these make targets.
#
#### Default
#
#  $ make <testcode>.timed
#
#    Compiles <testcode> with a $TIMEOUT limit
#
#    E.g.  $ make test2005_35.timed
#
#  $ make check-performance-timing
#
#    Compiles all testcodes with a $TIMEOUT limit
#
#### Threshold
#
#  $ make <testcode>.timed_with_threshold
#
#    Compiles <testcode> with g++ and then uses the g++ compile time
#    as a $TIMEOUT threshold to compile <testcode> with ROSE.
#
#    E.g.  $ make test2005_35.timed_with_threshold
#
#  $ make check-performance-timing-with-threshold
#
#    Compiles all testcodes with a $TIMEOUT limit calculated from
#    the g++ compile time threshold.
#
### Environment Variables
#
#  TIMEOUT              Maximum time limit for the compilation of a <testcode>
#  TIMEOUT_TOLERANCE    $gnu_time * $TIMEOUT_TOLERANCE (threshold tests only)
#------------------------------------------------------------------------------

# DQ (6/30/2013): Increased this to support compiling Boost example tests. We 
# need to rethink how we want to handl this. I suggest restricting this to a 
# few tests instead of each test. Compiling with optimization also makes about
# a factor of four difference for test2013_241.C (boost example test code).
# Since Jenkins testing is currently done without optimization we need the 
# larger timeout value just to handle the newer boost test codes.
# TIMEOUT            ?= 60  # Default timeout value
TIMEOUT            ?= 420  # Default timeout value
TIMEOUT_TOLERANCE  ?= 20  # Default timeout tolerance value for threshold
                          # tests = $gnu_time * $TIMEOUT_TOLERANCE

check-performance-timing:
	@$(MAKE) $(TEST_Sources:.C.passed=.timed)

check-performance-timing-with-threshold:
	@$(MAKE) $(TEST_Sources:.C.passed=.timed_with_threshold)

$(TEST_Sources:.C.passed=.timed):
	rm -f $(@:.timed=.C.passed) &&                                                 \
	$(top_srcdir)/scripts/timeout.sh $(TIMEOUT) $(MAKE) $(@:.timed=.C.passed) ||   \
			(                                                                          \
				test $$? -eq 124 &&                                                      \
				echo "[ERROR] '$@' compilation timed out after '$(TIMEOUT)' seconds" &&  \
				exit 1                                                                   \
			)

# 1. Time and Compile testcode with g++
# 2. Compile with ROSE, using TIMEOUT relative to time results from g++
#
# Uses $(TESTCODES_REQUIRED_TO_PASS)
#
$(TEST_Sources:.C.passed=.timed_with_threshold):
	rm -f $(@:.timed_with_threshold=.C.passed) &&                                            \
	SECONDS=0 &&                                                                             \
			$(MAKE) TEST_TRANSLATOR=g++ ROSE_FLAGS=  $(@:.timed_with_threshold=.C.passed) &&     \
	gnu_time="$$((($$SECONDS + 1)))" &&                                                      \
	echo "[INFO] '$@' compiled with GNU in '$$gnu_time' seconds" &&                          \
	rm -f $(@:.timed_with_threshold=.C.passed) &&                                            \
	TIMEOUT="$$((($$gnu_time * $(TIMEOUT_TOLERANCE))))" &&                                   \
	echo "[INFO] '$@' performance test is set to timeout after '$$TIMEOUT' seconds" &&       \
	$(MAKE) TIMEOUT="$$TIMEOUT" $(@:.timed_with_threshold=.timed)


###############################################################################################################################
# Automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
	rm -f $(COMPILATION_TEST_TARGETS_FAILING) $(COMPILATION_TEST_TARGETS_FAILING:.passed=.failed)

clean-local-old-stuff-unused:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot *.ti
	rm -f *.foldedConstants-o
	rm -f *.unfoldedConstants-o
	rm -rf QMTest
	rm -f *.int.c
	rm -f test2013_260_unparse_headers conftest_configure_test*
	rm -f conftest.C
	rm -f *.passed *.failed *.err
