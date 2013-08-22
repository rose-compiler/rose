include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
CODETHORN_SRC_ROOT = $(top_srcdir)/projects/CodeThorn/src/

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

noinst_PROGRAMS = addressTakenAnalysis 
#arrRefExpLhsTypeCheck

#arrRefExpLhsTypeCheck_SOURCES = arrRefExpLhsTypeCheck.C
#arrRefExpLhsTypeCheck_LDADD = -lrose

EXTRA_DISTS = tests

INCLUDES = \
	$(ROSE_INCLUDES) \
	-I$(srcdir) \
	-I$(CODETHORN_SRC_ROOT)

addressTakenAnalysis_SOURCES = \
	$(CODETHORN_SRC_ROOT)/Timer.cpp \
	$(CODETHORN_SRC_ROOT)/AstTerm.C \
	$(CODETHORN_SRC_ROOT)/AnalysisAbstractionLayer.C \
	$(CODETHORN_SRC_ROOT)/SgNodeHelper.C \
	$(CODETHORN_SRC_ROOT)/VariableIdMapping.C \
	$(CODETHORN_SRC_ROOT)/Miscellaneous.C \
	$(CODETHORN_SRC_ROOT)/CommandLineOptions.C \
	addressTakenAnalysis.C \
	addressTakenAnalysis.h \
	defUseQuery.C \
	defUseQuery.h \
	VariableIdUtils.C \
	VariableIdUtils.h \
	addressTakenAnalysisDriver.C

addressTakenAnalysis_LDADD = -lrose

ADDRESSTAKENANALYSIS_TESTS = $(ADDRESSTAKENANALYSIS_TESTS_PASSING)

ADDRESSTAKENANALYSIS_TESTS_PASSING = \
	$(srcdir)/tests/test1.cpp \
	$(srcdir)/tests/test2.cpp \
	$(srcdir)/tests/test3.cpp \
	$(srcdir)/tests/test4.cpp \
	$(srcdir)/tests/test5.cpp \
	$(srcdir)/tests/test6.cpp \
	$(srcdir)/tests/test8.cpp \
	$(srcdir)/tests/test9.cpp \
	$(srcdir)/tests/test10.cpp \
	$(srcdir)/tests/test11.cpp \
	$(srcdir)/tests/test12.cpp \
	$(srcdir)/tests/test13.cpp \
	$(srcdir)/tests/test14.cpp \
	$(srcdir)/tests/test15.cpp \
	$(srcdir)/tests/test16.cpp \
	$(srcdir)/tests/test17.cpp \
	$(srcdir)/tests/test18.cpp \
	$(srcdir)/tests/test19.cpp \
	$(srcdir)/tests/test20.cpp \
	$(srcdir)/tests/test21.cpp \
	$(srcdir)/tests/test22.cpp \
	$(srcdir)/tests/test23.cpp \
	$(srcdir)/tests/test24.cpp \
	$(srcdir)/tests/test25.cpp \
	$(srcdir)/tests/test27.cpp \
	$(srcdir)/tests/test28.cpp \
	$(srcdir)/tests/test29.cpp \
	$(srcdir)/tests/test30.cpp \
	$(srcdir)/tests/test31.cpp \
	$(srcdir)/tests/test33.cpp \
	$(srcdir)/tests/test34.cpp \
	$(srcdir)/tests/test35.cpp \
	$(srcdir)/tests/test36.cpp \
	$(srcdir)/tests/test37.cpp \
	$(srcdir)/tests/test38.cpp \
	$(srcdir)/tests/test39.cpp \
	$(srcdir)/tests/test41.c


ADDRESSTAKENANALYSIS_TESTS_FAILING = \
	$(srcdir)/tests/test7.cpp \ # has r-value references
	$(srcdir)/tests/test26.cpp \ # comma op
	$(srcdir)/tests/test40.cpp \ # comma op
	$(srcdir)/tests/test32.cpp \
	$(srcdir)/tests/arrow_test.cpp

ADDRESSTAKENANALYSIS_CHECKS = ${ADDRESSTAKENANALYSIS_TESTS:.cpp=.cpp-o}

$(ADDRESSTAKENANALYSIS_CHECKS): addressTakenAnalysis $(srcdir)/tests/$(@:.cpp=.cpp-o)
	./addressTakenAnalysis -c --edg:no_warnings $(@:.cpp-o=.cpp)

check-local: $(ADDRESSTAKENANALYSIS_CHECKS)

# Files that represent header files from ROSE that we can now compilie with ROSE.
#TESTCODES_REQUIRED_TO_PASS = \
testRoseHeaders_00.C \
testRoseHeaders_01.C \
testRoseHeaders_02.C \
testRoseHeaders_04.C \
testRoseHeaders_05.C \
testRoseHeaders_06.C \
testRoseHeaders_07.C \
testRoseHeaders_08.C


#testRoseHeaders_03.C

#TEST_OBJECTS = $(TESTCODES_REQUIRED_TO_PASS:.C=.o)
#ROSE_EXAMPLE_TESTS_DIR=$(top_srcdir)/tests/CompileTests/RoseExample_tests

#ROSE_FLAGS = --edg:no_warnings -w --edg:restrict

#$(TEST_OBJECTS): arrRefExpLhsTypeCheck $(ROSE_EXAMPLE_TESTS_DIR)/$(@:.o=.C)
#	./arrRefExpLhsTypeCheck $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(ROSE_EXAMPLE_TESTS_DIR)/$(@:.o=.C)

#check-arrRefExpLhsTypeCheck: $(TEST_OBJECTS)

