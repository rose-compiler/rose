include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
CODETHORN_SRC_ROOT = $(top_srcdir)/projects/CodeThorn/src/

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

noinst_PROGRAMS = addressTakenAnalysis

EXTRA_DISTS = tests

INCLUDES = \
	$(ROSE_INCLUDES) \
	-I$(srcdir) \
	-I$(CODETHORN_SRC_ROOT)

# AM_YFLAGS = -d -p matcherparser
# AM_LFLAGS = -Pmatcherparser -o$(LEX_OUTPUT_ROOT).c

addressTakenAnalysis_SOURCES = \
	$(CODETHORN_SRC_ROOT)/Timer.cpp \
	$(CODETHORN_SRC_ROOT)/RoseAst.C \
	$(CODETHORN_SRC_ROOT)/AstTerm.C \
	$(CODETHORN_SRC_ROOT)/AstMatching.C \
	$(CODETHORN_SRC_ROOT)/AstVariableIdInterface.C \
	$(CODETHORN_SRC_ROOT)/MatchOperation.C \
	$(CODETHORN_SRC_ROOT)/SgNodeHelper.C \
	$(CODETHORN_SRC_ROOT)/VariableIdMapping.C \
	$(CODETHORN_SRC_ROOT)/Miscellaneous.C \
	$(CODETHORN_SRC_ROOT)/CommandLineOptions.C \
	matcherparser.C \
	addressTakenAnalysis.C \
	addressTakenAnalysis.h \
	defUseQuery.C \
	defUseQuery.h \
	VariableIdUtils.C \
	VariableIdUtils.h \
	addressTakenAnalysisDriver.C

BUILT_SOURCES = matcherparser.C

addressTakenAnalysis_LDADD = -lrose

ADDRESSTAKENANALYSIS_TESTS = $(ADDRESSTAKENANALYSIS_TESTS_PASSING)

ADDRESSTAKENANALYSIS_TESTS_PASSING = \
	$(srcdir)/tests/test1.cpp \
	$(srcdir)/tests/test2.cpp \
	$(srcdir)/tests/test3.cpp \
	$(srcdir)/tests/test4.cpp \
	$(srcdir)/tests/test5.cpp \
	$(srcdir)/tests/test8.cpp \
	$(srcdir)/tests/test9.cpp \
	$(srcdir)/tests/test10.cpp \
	$(srcdir)/tests/test11.cpp \
	$(srcdir)/tests/test12.cpp \
	$(srcdir)/tests/test14.cpp \
	$(srcdir)/tests/test15.cpp \
	$(srcdir)/tests/test16.cpp \
	$(srcdir)/tests/test18.cpp \
	$(srcdir)/tests/test20.cpp \
	$(srcdir)/tests/test21.cpp \
	$(srcdir)/tests/test22.cpp \
	$(srcdir)/tests/test23.cpp \
	$(srcdir)/tests/test24.cpp

ADDRESSTAKENANALYSIS_TESTS_FAILING = \
	$(srcdir)/tests/test6.cpp \     # has r-value references
	$(srcdir)/tests/test13.cpp \    # not handling SgConditionalExp
	$(srcdir)/tests/test19.cpp \    # not handling SgConditionalExp	
	$(srcdir)/tests/test17.cpp \    # not handling SgThisExp
	$(srcdir)/tests/test7.cpp \
	$(srcdir)/tests/test25.cpp \
	$(srcdir)/tests/test26.cpp \
	$(srcdir)/tests/test27.cpp \
	$(srcdir)/tests/test28.cpp \
	$(srcdir)/tests/test29.cpp \
	$(srcdir)/tests/test30.cpp \
	$(srcdir)/tests/arrow_test.cpp

ADDRESSTAKENANALYSIS_CHECKS = ${ADDRESSTAKENANALYSIS_TESTS:.cpp=.cpp-o}

$(ADDRESSTAKENANALYSIS_CHECKS): addressTakenAnalysis $(srcdir)/tests/$(@:.cpp=.cpp-o)
	./addressTakenAnalysis -c --edg:no_warnings $(@:.cpp-o=.cpp)
# explicit rules because of multiple bison parser
matcherparser.C: $(CODETHORN_SRC_ROOT)/matcherlexer.ll $(CODETHORN_SRC_ROOT)/matcherparser.yy
	$(YACC) $(YFLAGS) -d -p matcherparser $(CODETHORN_SRC_ROOT)/matcherparser.yy # generates y.tab.h/c
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pmatcherparser $(CODETHORN_SRC_ROOT)/matcherlexer.ll
	mv y.tab.h matcherparser.h
	cat lex.yy.c y.tab.c > matcherparser.C
	rm lex.yy.c y.tab.c

check-local: $(ADDRESSTAKENANALYSIS_CHECKS)

clean-local:
	rm -f matcher*
