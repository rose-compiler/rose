include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# This included test programs demonstrating the std::function use and API.
bin_PROGRAMS    = attributeGenerator testFunctionCallBack_01 testFunctionCallBack_02 testFunctionCallBack_03 testFunctionCallBack_04 array_main arrayDSLcompiler

CXX11FLAG = -std=c++11

CPPFLAGS += $(CXX11FLAG)

attributeGenerator_SOURCES = attributeGenerator.C attributeGenerator.h

# Note that dsl_attributes.C is ultimately a generated file.
array_main_SOURCES = array_main.C array.C generated_dsl_attributes.C

arrayDSLcompiler_SOURCES = arrayDSLcompiler.C dsl_detection.C dslSupport.C generated_dsl_attributes.C

dsl_detection_SOURCES = dsl_detection.C


# DQ (1/19/2016): Added test code for std::function support required in DSL recognition and evaluation.
testFunctionCallBack_01_SOURCES = testFunctionCallBack_01.C
testFunctionCallBack_02_SOURCES = testFunctionCallBack_02.C
testFunctionCallBack_03_SOURCES = testFunctionCallBack_03.C
testFunctionCallBack_04_SOURCES = testFunctionCallBack_04.C

# Simple example
# array_SOURCES = array.C


include_HEADERS = 

ATERM_LIB_DIR = $(ATERM_INSTALL_PATH)/lib

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

TESTCODES_REQUIRED_TO_PASS = test2014_01.C

TESTCODE_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Output = $(addprefix rose_,${TESTCODES})
TEST_Output = $(addprefix rose_,${ALL_TESTCODES})

PASSING_REGRESSION_TEST_Output = ${TESTCODES_REQUIRED_TO_PASS:.C=.C.passing}

# We can optionally by-pass the dsl_attributes.C by copying a version from the srcdir.
generated_dsl_attributes.h: $(srcdir)/nongenerated_dsl_attributes.h 
	cp $(srcdir)/nongenerated_dsl_attributes.h generated_dsl_attributes.h

generated_dsl_attributes.C: $(srcdir)/nongenerated_dsl_attributes.C generated_dsl_attributes.h 
	cp $(srcdir)/nongenerated_dsl_attributes.C generated_dsl_attributes.C

# generate the dsl_attributes.C file from the example code implementing the abstraction.
testAttributeGenerator: attributeGenerator
	./attributeGenerator $(srcdir)/array_header.C

testArrayDSLCompiler: arrayDSLcompiler generated_dsl_attributes.C
	./arrayDSLcompiler -c $(srcdir)/array_main.C


testDSLCompiler: dsl_attributes.C


check-local:
	@echo "Tests for DSL Infrastructure examples."
	@echo "*************************************************************************************************"
	@echo "******* ROSE/projects/dsl_infrastructure: make check rule complete (terminated normally) ********"
	@echo "*************************************************************************************************"


EXTRA_DIST = 

CLEANFILES = 

clean-local:
	rm -rf *.o rose_*.C 
	rm -f a.out *.dot 

