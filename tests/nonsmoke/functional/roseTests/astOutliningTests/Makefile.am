include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#########################################################################################################################
#						PROGRAMS
#########################################################################################################################

#------------------------------------------------------------------------------------------------------------------------
# outline (lots of tests later in the makefile)
# we install this copy since it is the most tested version, better than the tutorial one
bin_PROGRAMS = outline
outline_SOURCES = outline.cc
outline_CPPFLAGS = $(ROSE_INCLUDES)
outline_LDFLAGS = $(ROSE_RPATHS)
outline_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# injectOutlinePragmas

noinst_PROGRAMS += injectOutlinePragmas
injectOutlinePragmas_SOURCES = RandomStmtSelector.cc injectOutlinePragmas.cc
injectOutlinePragmas_CPPFLAGS = $(ROSE_INCLUDES)
injectOutlinePragmas_LDFLAGS = $(ROSE_RPATHS)
injectOutlinePragmas_LDADD = $(ROSE_LIBS)

TEST_TARGETS += injectOutlinePragmas.passed
injectOutlinePragmas.passed: injectOutlinePragmas
	@$(RTH_RUN) DISABLED="not tested by the original makefile" CMD=false $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# outlineRandom

noinst_PROGRAMS += outlineRandom
outlineRandom_SOURCES = RandomStmtSelector.cc outlineRandom.cc
outlineRandom_CPPFLAGS = $(ROSE_INCLUDES)
outlineRandom_LDFLAGS = $(ROSE_RPATHS)
outlineRandom_LDADD = $(ROSE_LIBS)
EXTRA_DIST += RandomStmtSelector.hh

TEST_TARGETS += outlineRandom.passed
outlineRandom.passed: outlineRandom
	@$(RTH_RUN) DISABLED="not tested by the original makefile" CMD=false $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# outlineSelection (tests below)

noinst_PROGRAMS += outlineSelection
outlineSelection_SOURCES = outlineSelection.cc
outlineSelection_CPPFLAGS = $(ROSE_INCLUDES)
outlineSelection_LDFLAGS = $(ROSE_RPATHS)
outlineSelection_LDADD = $(ROSE_LIBS)

#########################################################################################################################
#						TEST SPECIMENS
#########################################################################################################################

#------------------------------------------------------------------------------------------------------------------------
# Input specimens local to this directory

# Additional tests using #pragma rose_outline, Liao, 12/17/2008

# DQ (6/23/2011): The test code moreTest3.cpp does not output it's outlined function when we get the name qualification correct.
# However, if the name qualification on the friend declaration is wrong, then the outlined function will be output.  I don't
# understand this, but I think it is a bug in the outlining.  This is consistant with the previous version of ROSE before
# adding the new name qualification support.
# DQ (/27/2011): Outlining test moreTest3.cpp fails with level 2 testing for dangling pointer detection.

C_TESTS_REQUIRED_TO_PASS =			\
	 array1.c				\
	 arrayParameter.c			\
	 arrayRef.c \
	 jacobi_rose_outline.c                  \
	 moreTest0.c				\
	 moreTest1.c				\
	 moreTest8.c				\
	 print.c	\
	 variableCloning.c \
	 zeroParameter.c

# This is not ready
#         variables_in_header.c
# template_function1.cpp

CXX_TESTS_REQUIRED_TO_PASS =		\
	array2.cpp				\
	moreTest6.cpp				\
	moreTest7.cpp				\
        charStar.cpp  \
	reference.cpp

# Add a few tool_G's regression tests
# 	test_26_v2_2020.cpp  this is not ready, not sure what happened for preprocessing
CXX2_TESTS_REQUIRED_TO_PASS =                \
 	print.cpp \						 
	test_42_2020.cpp \
	test_20_2019.cpp \
	test_26_v1_2020.cpp \
	charStar.cpp  \
	test_37_2020.cpp \
	ft_cfftz.cpp

# DQ (11/14/2017): This is failing and I need to learn the depth of what is the problem for the outlining tests.
# I think I need to work with Liao on this when he is back from vacation.
#	moreTest2.cpp				
#	moreTest3.cpp				
#	moreTest4.cpp				
#	moreTest5.cpp				

FORTRAN_TESTS_REQUIRED_TO_PASS =		\
	array-variable.f			\
	array.f					\
	assignment.f				\
	assignmentmain.f			\
	assignmentmain2.f			\
	blockdo.f90				\
	contains.f90				\
	dimension.f				\
	doloop.f				\
	doloop3.f90				\
	doloop4.f90				\
	doloop_nested.f90			\
	format.f				\
	function.f				\
	function2.f				\
	hello.f					\
	integer.f				\
	intent.f90				\
	lengthlimit.f				\
	loan.f					\
	moduleParameter.f90			\
	parameter.f				\
	parameter2.f				\
	stop.f90				\
	stop1.f90				\
	subroutine.f90 				\
	ugoto.f					\
	write.f

C_AND_CXX_TESTS_REQUIRED_TO_PASS = $(C_TESTS_REQUIRED_TO_PASS) $(CXX_TESTS_REQUIRED_TO_PASS)
EXTRA_DIST += $(C_AND_CXX_TESTS_REQUIRED_TO_PASS) $(FORTRAN_TESTS_REQUIRED_TO_PASS)

# arithmeticIf.f, bug 166 refer to labels 
# column72.f, not present in original makefile [RPM 2013-09-28]
# column80.f, not present in original makefile [RPM 2013-09-28]
# contains1.f90, subprogram in module not supported
# continue.f, not present in original makefile [RPM 2013-09-28]
# doloop2.f , bug 178, shared continue of two loops
# doloop5.f90 bug 183, lack of end for do loops
# format2.f, bug 180, not my problem, test it again when Dan's ready
# implicit.f90, not present in original makefile [RPM 2013-09-28]
# module.f90, similar to the above
# module1.f90, supluous END
# module3.f90, contains in module not supported
# moduleSubprogram.f90 , similar to above
# polygon.cpp: TODO recursively find super classes for a dependent class
# power.f, END SUBROUTINE problem
# print.f, bug 170
# read.f, need label symbol for format field
# read2.f , bug 179, openfortran parser's problem, won't fix
# real8.f bug 168, won't fix
# stop2.f90, comments are attached before a hidden function definition node??
# subroutine1.f, not present in original makefile [RPM 2013-09-28]
# triangle.cpp: TODO AST copy does not support class member across files.
# useModule1.f90 , USE not supported
TESTCODE_CURRENTLY_FAILING =			\
	arithmeticIf.f				\
	contains1.f90				\
	doloop2.f				\
	doloop5.f90				\
	format2.f				\
	module.f90				\
	module1.f90				\
	module3.f90				\
	moduleSubprogram.f90			\
	polygon.cpp				\
	power.f					\
	print.f					\
	read.f					\
	read2.f					\
	real8.f					\
	stop2.f90				\
	triangle.cpp				\
	useModule1.f90

EXTRA_DIST += $(TESTCODE_CURRENTLY_FAILING)

# Print the names of the currently failing tests to remind someone to look at them
failing_test_targets = $(addprefix failing_, $(addsuffix .passed, $(TESTCODE_CURRENTLY_FAILING)))
TEST_TARGETS += $(failing_test_targets)
$(failing_test_targets): failing_%.passed: %
	@$(RTH_RUN) \
		TITLE="$(notdir $<)" \
		DISABLED="this specimen is commented out in the makefile" \
		CMD=false \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# Input specimens external to our directory

include $(srcdir)/Makefile-pass.inc

TEST_DIR = $(abs_top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests
TEST_INCLUDES = -I$(TEST_DIR) -I$(abs_top_srcdir)/tests/nonsmoke/functional/CompileTests/A++Code -I$(abs_top_srcdir)/tests/nonsmoke/functional/roseTests/astOutliningTests 
TEST_CXXFLAGS = $(TEST_INCLUDES) -rose:verbose 0 -rose:outline:seq 7 -rose:outline:parameter_wrapper 

if ROSE_USE_LONG_MAKE_CHECK_RULE
EXTERNAL_CXX_TESTS_REQUIRED_TO_PASS = $(EXAMPLE_CXX_TESTS_REQUIRED_TO_PASS)
else
EXTERNAL_CXX_TESTS_REQUIRED_TO_PASS = $(EXAMPLE_CXX_TESTS_REQUIRED_TO_PASS_SHORT)
endif

#------------------------------------------------------------------------------------------------------------------------
# OpenMP input specimens external to our directory
OpenMP_C_TEST_REQUIRED_TO_PASS = \
				 jacobi.c


OpenMP_TEST_DIR = $(abs_top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests
OpenMP_TEST_INCLUDES = -I$(OpenMP_TEST_DIR)

#########################################################################################################################
#						TESTS
# These are separated into distinct rules because they run different commands and/or command-line switches; it is no
# longer necessary to separate these based on the language of the specimen (C/C++/F77/F90/etc).
#########################################################################################################################

#------------------------------------------------------------------------------------------------------------------------
# Default outlining flags

# DQ (2/6/2009): The last option should be possible to remove...-rose:skipfinalCompileStep
# this flag is used for the files listed within this file: C_TESTS_REQUIRED_TO_PASS and CXX_TESTS_REQUIRED_TO_PASS
# TODO remove -rose:skipfinalCompileStep when the command line issue is solved
#-rose:outline:temp_variable
OUTLINE_FLAGS= --edg:no_warnings -rose:verbose 0 -rose:outline:parameter_wrapper -rose:detect_dangling_pointers 1

#------------------------------------------------------------------------------------------------------------------------
# Test outlining using the classic scheme, using tests within the current folder

FORTRAN_RESULTS = 
classic_test_targets = $(addprefix classic_, $(addsuffix .passed, $(C_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(classic_test_targets)

$(classic_test_targets): classic_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outline classic $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) -rose:outline:enable_classic -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# Using tests  from OpenMP test folder, turning on the select_omp_loop option
omp_c_classic_test_targets = $(addprefix classic_, $(addsuffix .passed, $(OpenMP_C_TEST_REQUIRED_TO_PASS)))
TEST_TARGETS += $(omp_c_classic_test_targets)

$(omp_c_classic_test_targets): classic_%.passed: $(OpenMP_TEST_DIR)/% outline
	@$(RTH_RUN) \
		TITLE="outline classic $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(OpenMP_TEST_INCLUDES) -rose:outline:select_omp_loop -rose:outline:enable_classic -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@


#------------------------------------------------------------------------------------------------------------------------
# Test outlining in the same file, using tests within the current folder

inplace_test_targets = $(addprefix inplace_, $(addsuffix .passed, $(C_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(inplace_test_targets)

$(inplace_test_targets): inplace_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outline in-place $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) -rose:outline:temp_variable -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# Test outlining to a separate file

tofile_test_targets  = $(addprefix tofile_,  $(addsuffix .passed, $(C_AND_CXX_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(tofile_test_targets)

$(tofile_test_targets): tofile_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outline new-file $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) -rose:outline:new_file -rose:outline:temp_variable -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# shortcut to test new_file

test_newfile: $(tofile_test_targets)

#------------------------------------------------------------------------------------------------------------------------
# Test outlining to a separate file using dlopen

dlopen_test_targets  = $(addprefix dlopen_,  $(addsuffix .passed, $(C_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(dlopen_test_targets)

dlopen_test_flags =				\
	-rose:outline:use_dlopen		\
	-I$(abs_top_srcdir)/projects/autoTuning	\
	-rose:outline:output_path .

$(dlopen_test_targets): dlopen_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outine dlopen $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) $(dlopen_test_flags) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@
# Liao, 2019/1/31
# test the new option  -rose:outline:copy_orig_file , which copies input file into a lib source file and append all outlined functions to the single lib source file. This eliminates the need to copy dependent headers and prototypes into the lib source file. 
dlopen2_test_targets  = $(addprefix dlopen2_,  $(addsuffix .passed, $(C_TESTS_REQUIRED_TO_PASS)))
dlopen2_test_targets  += $(addprefix dlopen2_,  $(addsuffix .passed, $(CXX_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(dlopen2_test_targets)
dlopen2_test_flags =				\
	-rose:outline:use_dlopen		\
	-rose:outline:copy_orig_file		\
	-I$(abs_top_srcdir)/projects/autoTuning	\
	-rose:outline:output_path .

$(dlopen2_test_targets): dlopen2_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outine dlopen2 $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) $(dlopen2_test_flags) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# Liao, 2020//25
# test the new option  -rose:outline:copy_orig_file , which copies input file into a lib source file and append all outlined functions to the single lib source file. This eliminates the need to copy dependent headers and prototypes into the lib source file. 
# combined with -rose:unparseHeaderFiles -rose:unparseHeaderFilesRootFolder
dlopen3_test_targets = $(addprefix dlopen3_,  $(addsuffix .passed, $(CXX2_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(dlopen3_test_targets)
dlopen3_test_flags =				\
	-rose:outline:use_dlopen		\
	-rose:outline:copy_orig_file		\
	-I$(abs_top_srcdir)/projects/autoTuning	\
	-rose:outline:exclude_headers -rose:unparseHeaderFiles -rose:unparseHeaderFilesRootFolder unparseHeaderRoot_test_20_2019

$(dlopen3_test_targets): dlopen3_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outine dlopen3 $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) $(dlopen3_test_flags) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# shortcut to test the dlopen feature
test_dlopen: $(dlopen_test_targets) $(dlopen2_test_targets) $(dlopen3_test_targets)
test_dlopen3: $(dlopen3_test_targets)


#------------------------------------------------------------------------------------------------------------------------
# Test outlining with default $(OUTLINE_FLAGS) switches

#default_test_targets = $(addprefix default_, $(addsuffix .passed, $(CXX_TESTS_REQUIRED_TO_PASS)))
default_test_targets = $(addprefix default_, $(addsuffix .passed, $(C_AND_CXX_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(default_test_targets)

$(default_test_targets): default_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outline default $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) $(OUTLINE_FLAGS) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

test_default:$(default_test_targets)

#------------------------------------------------------------------------------------------------------------------------
# Outlining tests that don't use any outline-specific switches

noswitch_test_args = -rose:verbose 0 -ffixed-line-length-none
noswitch_test_targets = $(addprefix noswitch_, $(addsuffix .passed, $(FORTRAN_TESTS_REQUIRED_TO_PASS)))

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
TEST_TARGETS += $(noswitch_test_targets)
FORTRAN_RESULTS += $(noswitch_test_targets)
endif

$(noswitch_test_targets): noswitch_%.passed: % outline
	@$(RTH_RUN) \
		TITLE="outline $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline$(EXEEXT) $(TEST_INCLUDES) $(noswitch_test_args) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#########################################################################################################################
# outlineSelection: 
# Tests using the -rose:outline:seq 7 switch 
#------------------------------------------------------------------------------------------------------------------------

# This is to test the external C++ programs only
seq7a_test_flags = -rose:outline:seq 7 $(OUTLINE_FLAGS) $(TEST_INCLUDES)
seq7a_test_targets = $(addprefix seq7a_, $(addsuffix .passed, $(EXTERNAL_CXX_TESTS_REQUIRED_TO_PASS)))
TEST_TARGETS += $(seq7a_test_targets)

$(seq7a_test_targets): seq7a_%.passed: $(TEST_DIR)/% outlineSelection
	@$(RTH_RUN) \
		TITLE="outlineSelection seq7 $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outlineSelection$(EXEEXT) $(TEST_INCLUDES) $(seq7a_test_flags) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# Tests for outlineSelection using -rose:outline:seq 7 -ffixed-line-length-none on  local specimens
# This rule uses fortran tests within the current test directory

seq7b_test_flags = -rose:verbose 0 -rose:outline:seq 7 -ffixed-line-length-none
seq7b_test_targets = $(addprefix seq7b_, $(addsuffix .passed, $(FORTRAN_TESTS_REQUIRED_TO_PASS)))

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
TEST_TARGETS += $(seq7b_test_targets)
FORTRAN_RESULTS += $(seq7b_test_targets)
endif

$(seq7b_test_targets): seq7b_%.passed: % outlineSelection
	@$(RTH_RUN) \
		TITLE="outlineSelection seq7 $(notdir $<) [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outlineSelection$(EXEEXT) $(TEST_INCLUDES) $(seq7b_test_flags) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# complex transitional dependent declarations: outlined the first for loop

complexStruct_flags = \
	-rose:outline:new_file \
	-rose:outline:temp_variable \
	-rose:outline:exclude_headers \
	-rose:outline:abstract_handle 'ForStatement<numbering,1>'

TEST_TARGETS += complexStruct.passed
complexStruct.passed: complexStruct.c outline
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/outline $(TEST_INCLUDES) $(complexStruct_flags) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += complexStruct.c

#########################################################################################################################
#				OTHER TARGETS NOT USED DIRECTLY IN THIS MAKEFILE
#########################################################################################################################

test_class_ODR_violation: $(srcdir)/test_class_ODR_violation_main.C $(srcdir)/test_class_ODR_violation_outlined.C
	g++ -c $(srcdir)/test_class_ODR_violation_main.C
	g++ -c $(srcdir)/test_class_ODR_violation_outlined.C
#	Order does not appear to matter in the linking.
	g++ test_class_ODR_violation_main.o test_class_ODR_violation_outlined.o -o test_class_ODR_violation

#########################################################################################################################
#					AUTOMAKE BOILERPLATE
#########################################################################################################################
# a shortcut to test Fortran only
test-fortran: $(FORTRAN_RESULTS)

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -rf unparseHeaderRoot_test_20_2019
