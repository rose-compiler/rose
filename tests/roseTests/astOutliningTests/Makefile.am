# Import definitions common to Outliner submodules.
#include ../Make.inc

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
SUBDIRS = fortranTests
endif

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# Default header file search path
INCLUDES = $(ROSE_INCLUDES)

include $(srcdir)/Makefile-pass.inc


# DQ (!/24/2008): We should not need this!
#        -I$(top_srcdir) -I$(top_builddir)
#       -I$(top_srcdir)/src/midend/astOutlining/src

# Default link flags for executables
OUTLINER_LIBS = $(ROSE_LIBS)

# Useful definitions for testing
#   TEST_DIR = Location of ROSE's basic C++ tests.
#   TEST_INCLUDES = Find header files upon which the test files depend.
TEST_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
TEST_INCLUDES = \
        -I$(TEST_DIR) \
        -I$(top_srcdir)/tests/CompileTests/A++Code
# end of ../Make.in----

# INCLUDES += -I.

noinst_PROGRAMS = \
	outline \
        injectOutlinePragmas \
        outlineRandom \
        outlineSelection

outline_SOURCES = outline.cc
outline_LDFLAGS = $(OUTLINER_LIBS)

injectOutlinePragmas_SOURCES = \
        RandomStmtSelector.cc \
        injectOutlinePragmas.cc
injectOutlinePragmas_LDFLAGS = $(OUTLINER_LIBS)

outlineRandom_SOURCES = \
        RandomStmtSelector.cc \
        outlineRandom.cc
outlineRandom_LDFLAGS = $(OUTLINER_LIBS)

outlineSelection_SOURCES = outlineSelection.cc
outlineSelection_LDFLAGS = $(OUTLINER_LIBS)


# Import a list of C++ test codes expected to pass.
# include $(TEST_DIR)/Makefile-pass.inc
if ROSE_USE_LONG_MAKE_CHECK_RULE
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
else # !ROSE_USE_LONG_MAKE_CHECK_RULE
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT)
endif # ROSE_USE_LONG_MAKE_CHECK_RULE

# Outliner utility and flags to run with the tests.
OUTLINER_BIN = ./outlineSelection$(EXEEXT)
# this flag is used for the long list of CXX test files
TEST_CXXFLAGS = $(TEST_INCLUDES) -rose:verbose 0 -rose:outline:seq 7 -rose:outline:parameter_wrapper 

# Additional tests using #pragma rose_outline, Liao, 12/17/2008
C_TESTCODES_REQUIRED_TO_PASS = \
	 array1.c \
	 moreTest1.c \
	 moreTest0.c \
	 moreTest8.c \
	 arrayParameter.c \
	 zeroParameter.c

CXX_TESTCODES_REQUIRED_TO_PASS = \
	array2.cpp \
	reference.cpp \
	moreTest2.cpp \
	moreTest3.cpp \
	moreTest4.cpp \
	moreTest5.cpp \
	moreTest6.cpp \
	moreTest7.cpp

# DQ (6/23/2011): The test code moreTest3.cpp does not output it's outlined function when we get the name qualification correct.
# However, if the name qualification on the friend declaration is wrong, then the outlined function will be output.  I don't
# understand this, but I think it is a bug in the outlining.  This is consistant with the previous version of ROSE before
# adding the new name qualification support.


# triangle.cpp: TODO AST copy does not support class member across files.
# polygon.cpp: TODO recursively find super classes for a dependent class
TESTCODE_CURRENTLY_FAILING = \
	triangle.cpp \
	polygon.cpp 

ALL_EXTRA_TESTCODE = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# DQ (2/6/2009): The last option should be possible to remove...-rose:skipfinalCompileStep
# this flag is used for the files listed within this file: C_TESTCODES_REQUIRED_TO_PASS and CXX_TESTCODES_REQUIRED_TO_PASS
# TODO remove -rose:skipfinalCompileStep when the command line issue is solved
# OUTLINE_FLAG= --edg:no_warnings -rose:verbose 0 -rose:outline:parameter_wrapper -rose:outline:temp_variable -rose:skipfinalCompileStep
OUTLINE_FLAG= --edg:no_warnings -rose:verbose 0 -rose:outline:parameter_wrapper 
#-rose:outline:temp_variable

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)
PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)
#PASSING_CXX_SEPARATE_FILE_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

EXTERNAL_PASSING_CXX_TEST_Objects = $(TESTCODES_REQUIRED_TO_PASS:.C=.o)

# This rule tests the use of outlining in the original file and to a separate file.
$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c outline 
	@echo "***************** Testing C specific outlining...(a classic scheme) *****************"
	./outline$(EXEEXT) -rose:outline:enable_classic -c $<
	@echo "***************** Testing C specific outlining...(in the same file) *****************"
	./outline$(EXEEXT) $(OUTLINE_FLAG) -rose:outline:temp_variable -c $<
	@echo "***************** Testing C specific outlining...(to a separate file) *****************"
	./outline$(EXEEXT) $(OUTLINE_FLAG) -rose:outline:new_file -rose:outline:temp_variable -c $<
	@echo "*************Testing C specific outlining...(to use dlopen and in a separate file) *******"
	./outline$(EXEEXT) $(OUTLINE_FLAG) -rose:outline:use_dlopen -rose:outline:temp_variable -I$(top_srcdir)/projects/autoTuning -rose:outline:exclude_headers -rose:outline:output_path `pwd` -c $<

# This rule tests the use of outlining in the original file and to a separate file.
$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp outline
	@echo "***************** Testing C++ specific outlining...(in the same file) *****************"
	./outline$(EXEEXT) ${OUTLINE_FLAG} -c $<
	@echo "***************** Testing C++ specific outlining...(to a separate file) *****************"
	./outline$(EXEEXT) ${OUTLINE_FLAG} -rose:outline:new_file -rose:outline:temp_variable -c $<
#	@echo "***************** Testing C++ specific outlining...(use dlopen()) *****************"
#	./outline$(EXEEXT) ${OUTLINE_FLAG} -rose:outline:use_dlopen -c $<

# This rule currently only tests outlining to the original file (not a separate file also).
$(EXTERNAL_PASSING_CXX_TEST_Objects): %.o: $(top_srcdir)/tests/CompileTests/Cxx_tests/%.C outline
	./outlineSelection$(EXEEXT) -rose:outline:seq 7 ${OUTLINE_FLAG} $(TEST_INCLUDES) -c $<

test_class_ODR_violation: $(srcdir)/test_class_ODR_violation_main.C $(srcdir)/test_class_ODR_violation_outlined.C
	g++ -c $(srcdir)/test_class_ODR_violation_main.C
	g++ -c $(srcdir)/test_class_ODR_violation_outlined.C
#	Order does not appear to matter in the linking.
	g++ test_class_ODR_violation_main.o test_class_ODR_violation_outlined.o -o test_class_ODR_violation

# additional misc tests: complex transitional dependent declarations: outlined the first for loop
rose_complexStruct.c: outline
	./outline -rose:outline:new_file -rose:outline:temp_variable -rose:outline:exclude_headers -rose:outline:abstract_handle "ForStatement<numbering,1>" -c $(srcdir)/complexStruct.c

# DQ (3/5/2009): Removed use of script to generate the test code cases.
# EXTRA_DIST = build_tests-1pass.sh RandomStmtSelector.hh $(ALL_EXTRA_TESTCODE)
# CLEANFILES = Makefile-1pass LIST.dat

EXTRA_DIST = RandomStmtSelector.hh $(ALL_EXTRA_TESTCODE) complexStruct.c
CLEANFILES = 

# DQ (9/10/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
if !OS_VENDOR_DEBIAN
# DQ (8/29/2008): Added dependences
check-local: outline$(EXEEXT) injectOutlinePragmas outlineRandom outlineSelection
	@echo "Test for ROSE outliner."
	@echo "***************** Testing C specific outlining *******************"
	$(MAKE) $(PASSING_C_TEST_Objects)
	$(MAKE) rose_complexStruct.c
	@echo "***************** Testing C++ specific outlining ******************"
	$(MAKE) $(PASSING_CXX_TEST_Objects)
	$(MAKE) $(EXTERNAL_PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astOutliningTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"
else
check-local:
	@echo "***** SKIPPING TESTS IN AST OUTLINER *****"
endif

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.cpp rose_*.C *.pdf OUT_* *.dot *.*_copy_graph_old_nodes test_class_ODR_violation

# DQ (3/4/2009): Removed use of script to generate list of files to test.
# test -f Makefile-1pass
# $(MAKE) -f Makefile-1pass TEST_DIR="$(TEST_DIR)" OUTLINER_BIN="$(OUTLINER_BIN)" TEST_CXXFLAGS="$(TEST_CXXFLAGS)"
# $(MAKE) one-pass
# one-pass: Makefile-1pass $(OUTLINER_BIN)
#LIST.dat: $(TEST_DIR)/Makefile-pass.inc
#	echo $(TESTCODES_REQUIRED_TO_PASS) > LIST.dat
#Makefile-1pass: ./build_tests-1pass.sh LIST.dat
#	$(srcdir)/build_tests-1pass.sh $(top_srcdir) LIST.dat
# eof
