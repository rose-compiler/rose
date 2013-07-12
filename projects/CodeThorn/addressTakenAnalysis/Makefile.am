include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
CODETHORN_SRC_ROOT = $(top_srcdir)/projects/CodeThorn/src/

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

noinst_PROGRAMS = addressTakenAnalysis

EXTRA_DISTS = tests

INCLUDES = \
	$(ROSE_INCLUDES) \
	-I$(srcdir) \
	-I$(CODETHORN_SRC_ROOT)

BUILT_SOURCES = matcherparser.h
AM_YFLAGS = -d -p matcherparser
AM_LFLAGS = -Pmatcherparser -o$(LEX_OUTPUT_ROOT).c

addressTakenAnalysis_SOURCES = \
	$(CODETHORN_SRC_ROOT)/matcherparser.yy \
	$(CODETHORN_SRC_ROOT)/matcherlexer.ll \
	$(CODETHORN_SRC_ROOT)/Timer.cpp \
	$(CODETHORN_SRC_ROOT)/RoseAst.C \
	$(CODETHORN_SRC_ROOT)/AstTerm.C \
	$(CODETHORN_SRC_ROOT)/AstMatching.C \
	$(CODETHORN_SRC_ROOT)/MatchOperation.C \
	$(CODETHORN_SRC_ROOT)/SgNodeHelper.C \
	$(CODETHORN_SRC_ROOT)/VariableIdMapping.C \
	$(CODETHORN_SRC_ROOT)/Miscellaneous.C \
	$(CODETHORN_SRC_ROOT)/CommandLineOptions.C \
	addressTakenAnalysis.C \
	defUseQuery.C \
	addressTakenAnalysisDriver.C

addressTakenAnalysis_LDADD = -lrose

ADDRESSTAKENANALYSIS_TESTS = $(ADDRESSTAKENANALYSIS_TESTS_PASSING)

ADDRESSTAKENANALYSIS_TESTS_PASSING = \
	$(srcdir)/tests/test1.cpp \
	$(srcdir)/tests/test2.cpp \
	$(srcdir)/tests/test3.cpp \
	$(srcdir)/tests/test4.cpp \
	$(srcdir)/tests/test5.cpp \
	$(srcdir)/tests/test8.cpp \
	$(srcdir)/tests/test9.cpp

ADDRESSTAKENANALYSIS_TESTS_FAILING = \
	$(srcdir)/tests/test6.cpp \ # has r-value references
	$(srcdir)/tests/test7.cpp

ADDRESSTAKENANALYSIS_CHECKS = ${ADDRESSTAKENANALYSIS_TESTS:.cpp=.cpp-o}

$(ADDRESSTAKENANALYSIS_CHECKS): addressTakenAnalysis $(srcdir)/tests/$(@:.cpp=.cpp-o)
	./addressTakenAnalysis -c --edg:no_warnings $(@:.cpp-o=.cpp)

check-local: $(ADDRESSTAKENANALYSIS_CHECKS)

clean-local:
	rm -f matcher*
