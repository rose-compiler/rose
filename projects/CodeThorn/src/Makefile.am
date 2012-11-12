include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = codethorn ast_demo matcher_demo iterator_test
EXTRA_DIST = EqualityMaintainer.C Doxyfile $(TESTS) $(COUNTEREXAMPLES)
BUILT_SOURCES = 
CLEANFILES = 

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

MYDATE = $(shell date +%Y_%m_%d)

INCLUDES = $(ROSE_INCLUDES) -I$(top_builddir)

# in V1.2 there are no KNOBS
KNOBS = 

codethorn_CXXFLAGS = $(KNOBS) -fopenmp -Wall # -O3 -march=native -ftree-vectorize
codethorn_LDADD = -lfl -lrose
codethorn_YFLAGS = -p ltl_ # yacc flags for LTL parser
codethorn_SOURCES = \
  AType.C                          \
  AType.h                          \
  Analyzer.C                       \
  Analyzer.h                       \
  AstTerm.C                        \
  AstTerm.h                        \
  CFAnalyzer.C                     \
  CFAnalyzer.h                     \
  CollectionOperators.h            \
  CommandLineOptions.C             \
  CommandLineOptions.h             \
  ConstraintRepresentation.C       \
  ConstraintRepresentation.h       \
  EqualityMaintainer.h             \
  ExprAnalyzer.C                   \
  ExprAnalyzer.h                   \
  HSet.h                           \
  HSetMaintainer.h                 \
  HashFun.h                        \
  InternalChecks.C                 \
  InternalChecks.h                 \
  LTL.h                            \
  LTLCheckerFixpoint.C             \
  LTLCheckerFixpoint.h             \
  LTLCheckerUnified.C              \
  LTLCheckerUnified.h              \
  LTLParser.y++                    \
  Labeler.C                        \
  Labeler.h                        \
  LanguageRestrictor.C             \
  LanguageRestrictor.h             \
  Miscellaneous.C                  \
  Miscellaneous.h                  \
  RoseAst.C                        \
  RoseAst.h                        \
  SetAlgo.h                        \
  SgNodeHelper.C                   \
  SgNodeHelper.h                   \
  StateRepresentations.C           \
  StateRepresentations.h           \
  Timer.cpp                        \
  Timer.h                          \
  VariableIdMapping.C              \
  VariableIdMapping.h              \
  Visualizer.C                     \
  Visualizer.h                     \
  codethorn.C                      \
  codethorn.h

ast_demo_SOURCES = ast_demo.C Timer.cpp Timer.h  RoseAst.C RoseAst.h AstTerm.C AstTerm.h
ast_demo_LDADD = -lrose libmatcher.la

lib_LTLIBRARIES = libmatcher.la

libmatcher_la_SOURCES = \
  AstMatching.C AstMatching.h matcherlexer.lxx matcherparser.yxx matcherparser_decls.h MatchOperation.C MatchOperation.h

libmatcher_la_LFLAGS = -Pmatcher
libmatcher_la_YFLAGS = -p matcher --defines=matcherparser.h

REGRESSION_DATA_DIR=regressiondata

.PHONY: codethorn-dist viz bsps docs test checkdemos

#MS: not finished yet. Flip comments for the next couple of lines to test it
matcher_demo_LDADD = -lrose libmatcher.la
matcher_demo_SOURCES = matcher_demo.C Timer.cpp RoseAst.C AstTerm.C

iterator_test_SOURCES = iterator_test.C ShowSeq.h
iterator_test_LDADD = -lrose libmatcher.la

check-local:
	./codethorn --internal-checks				 
	@echo ================================================================
	@echo CHECK: all of the following formulae should evaluate to "YES"!
	@echo ================================================================
	./codethorn --edg:no_warnings $(srcdir)/tests/rers_mini1.c    --verify $(srcdir)/tests/rers_mini1.ltl

checkdemos: 
	./matcher_demo $(srcdir)/tests/basictest5.C < $(srcdir)/tests/matchexpressions/test1.mat
	./ast_demo $(srcdir)/tests/basictest5.C

test:
	./codethorn --edg:no_warnings $(srcdir)/tests/rers/Problem1.c \
	               --verify $(srcdir)/tests/rers/properties1.txt \
	               --csv-assert Problem1-assert.csv \
	               --csv-ltl Problem1-ltl.csv

# MS: 1-6 is reasonable for V1.2
RERS=$(patsubst %,Problem%.log, $(shell seq 6))
rers: $(RERS)

# parallel tests with make -j<N> test
%.log: $(srcdir)/tests/rers/%.c $(TOOLNAME3) Makefile
	./codethorn --edg:no_warnings --colors=no $< \
	     --verify $(patsubst $(srcdir)/tests/rers/Problem%.c,$(srcdir)/tests/rers/properties%.txt,$<) \
	     --csv-ltl $(patsubst $(srcdir)/tests/rers/Problem%.c,Problem%-ltl.csv,$<) \
	     --csv-assert $(patsubst $(srcdir)/tests/rers/Problem%.c,Problem%-assert.csv,$<) >$@


validate:
	cd regressiondata && python validate.py --log Problem1.log --csv rers_Problem1_ltl_csv.txt

docs:
	doxygen Doxyfile

viz: transitiongraph1.dot transitiongraph2.dot cfg.dot ast.dot
	dot -Tps transitiongraph1.dot -oviz/transitiongraph1.ps
	dot -Tps transitiongraph2.dot -oviz/transitiongraph2.ps
	dot -Tps cfg.dot -oviz/cfg.ps
	dot -Gordering=out -Tps ast.dot -oviz/ast.ps
vizjpg: transitiongraph1.dot transitiongraph2.dot cfg.dot ast.dot
	dot -Tjpg transitiongraph1.dot -oviz/transitiongraph1.jpg
	dot -Tjpg transitiongraph2.dot -oviz/transitiongraph2.jpg
	dot -Tjpg cfg.dot -oviz/cfg.jpg
	dot -Gordering=out -Tjpg ast.dot -oviz/ast.jpg

clean-local:
	rm -f *.dot
	rm -f *.ps
	rm -f viz/*
	rm -f bsps/*

distclean-local: clean
	rm -f *.tgz
	rm -f *~
	rm -rf ../docs/doxygen
	rm -f $(TOOLNAME3)ref

bsps: codethorn
	./codethorn $(srcdir)/tests/basictest10f.C && make viz && make vizjpg
	cp $(srcdir)/tests/basictest10f.C bsps
	cp viz/cfg.ps bsps/basictest10f_cfg.ps
	cp viz/transitiongraph1.ps bsps/basictest10f_transitiongraph1.ps
	cp viz/transitiongraph2.ps bsps/basictest10f_transitiongraph2.ps
	cp viz/ast.jpg bsps/basictest10f_ast.jpg
	cp viz/cfg.jpg bsps/basictest10f_cfg.jpg
	cp viz/transitiongraph1.jpg bsps/basictest10f_transitiongraph1.jpg
	cp viz/transitiongraph2.jpg bsps/basictest10f_transitiongraph2.jpg
	ps2pdf bsps/basictest10f_transitiongraph2.ps bsps/basictest10f_transitiongraph2.pdf
	./codethorn $(srcdir)/tests/basictest15.C && make viz && make vizjpg
	cp $(srcdir)/tests/basictest15.C bsps
	cp viz/cfg.ps bsps/basictest15_cfg.ps
	cp viz/transitiongraph1.ps bsps/basictest15_transitiongraph1.ps
	cp viz/transitiongraph2.ps bsps/basictest15_transitiongraph2.ps
	cp viz/ast.jpg bsps/basictest15_ast.jpg
	cp viz/cfg.jpg bsps/basictest15_cfg.jpg
	cp viz/transitiongraph1.jpg bsps/basictest15_transitiongraph1.jpg
	cp viz/transitiongraph2.jpg bsps/basictest15_transitiongraph2.jpg
	ps2pdf bsps/basictest15_transitiongraph2.ps bsps/basictest15_transitiongraph2.pdf

	./codethorn $(srcdir)/tests/rers/Problem1.c
	cp $(srcdir)/tests/rers/Problem1.c bsps
	dot -Tjpg transitiongraph2.dot -oviz/transitiongraph2.jpg
	cp viz/transitiongraph2.jpg bsps/rers1_transitiongraph2.jpg

codethorn-dist:
	tar cvzf codethorn_$(MYDATE).tgz *.C *cpp *.h *.lxx *.yxx tests/*.C tests/*.c Makefile*


asserttest: $(TOOLNAME3)
	./codethorn --edg:no_warnings $(srcdir)/tests/asserttest2.c --csv-assert assert_csv.txt

regressiontest: $(TOOLNAME3)
	@./codethorn --csv-assert tmp_rers_mini1_assert_csv.txt --csv-ltl tmp_rers_mini1_ltl_csv.txt --verify $(srcdir)/tests/rers_mini1.ltl --viz=yes $(srcdir)/tests/rers_mini1.c
	@echo "-----------------------"
	@sort -u tmp_rers_mini1_assert_csv.txt > tmp2_rers_mini1_assert_csv.txt 
	@diff tmp2_rers_mini1_assert_csv.txt $(REGRESSION_DATA_DIR)/rers_mini1_assert_csv.txt
	@echo "Regression assert test PASSED."
	@diff tmp_rers_mini1_ltl_csv.txt $(REGRESSION_DATA_DIR)/rers_mini1_ltl_csv.txt
	@echo "Regression LTL test PASSED."
	@sort -u transitiongraph1.dat > rg1.dat
	@sort -u $(REGRESSION_DATA_DIR)/rers_mini1_transitiongraph1.dat > rg2.dat
	@diff rg1.dat rg2.dat
	@echo "Regression transition graph test PASSED."
	@echo "-----------------------"
	@echo "All regression tests PASSED."
	@echo "-----------------------"

# use with caution
generateregressiondata:
	./codethorn --csv-assert $(REGRESSION_DATA_DIR)/rers_mini1_assert_csv.txt --csv-ltl $(REGRESSION_DATA_DIR)/rers_mini1_ltl_csv.txt --verify $(srcdir)/tests/rers_mini1.ltl --viz=yes $(srcdir)/tests/rers_mini1.c 
	sort -u transitiongraph1.dat > tmp_transitiongraph1.dat
	cp tmp_transitiongraph1.dat $(REGRESSION_DATA_DIR)/rers_mini1_transitiongraph1.dat

COUNTEREXAMPLES = counterexamples/ltl2haskell.sh counterexamples/Makefile counterexamples/qc.hs counterexamples/log2csv.awk

CODETHORN_TESTS= \
    tests/asserttest1.c \
    tests/asserttest2.c \
    tests/basictest0.C \
    tests/basictest10a.C \
    tests/basictest10b.C \
    tests/basictest10.C \
    tests/basictest10c.C \
    tests/basictest10d.C \
    tests/basictest10e.C \
    tests/basictest10f.C \
    tests/basictest11.C \
    tests/basictest12.C \
    tests/basictest13.C \
    tests/basictest14.C \
    tests/basictest15b.C \
    tests/basictest15.C \
    tests/basictest15c.C \
    tests/basictest15d.C \
    tests/basictest16.C \
    tests/basictest1.C \
    tests/basictest2.C \
    tests/basictest3.C \
    tests/basictest4.C \
    tests/basictest5.C \
    tests/basictest6.C \
    tests/basictest7.C \
    tests/basictest8.C \
    tests/basictest9.C \
    tests/basictest_globalvars.C \
    tests/checkp006if \
    tests/funccalltest2.c \
    tests/funccalltest2.C \
    tests/funccalltest.c \
    tests/funccalltest.C \
    tests/inputtest1.c \
    tests/inputtest2.c \
    tests/intertest10.C \
    tests/intertest11.C \
    tests/intertest12.C \
    tests/intertest13.C \
    tests/intertest14.C \
    tests/intertest16.C \
    tests/intertest1.C \
    tests/intertest2.C \
    tests/intertest3.C \
    tests/intertest4.C \
    tests/intertest5.C \
    tests/intertest6.C \
    tests/intertest7.C \
    tests/intertest8.C \
    tests/iotest1.c \
    tests/logandcheck1.c \
    tests/numloop1.c \
    tests/numloop2.c \
    tests/p001.c \
    tests/p001-transitiongraph1.dat \
    tests/p002.c \
    tests/p003.c \
    tests/p004.c \
    tests/p005.c \
    tests/p005-transitiongraph1.dat \
    tests/p006.c \
    tests/p006if.c \
    tests/p006if.ltl \
    tests/p006loop2.c \
    tests/p006loop.c \
    tests/p007.c \
    tests/p007-transitiongraph1.dat \
    tests/p008.c \
    tests/p008-transitiongraph1.dat \
    tests/p009.c \
    tests/p009.ltl \
    tests/properties007.txt \
    tests/properties008.txt \
    tests/equalitytest1.c \
    tests/rers_mini1.c \
    tests/rers_mini1.ltl \
    tests/rers_mini2.c \
    tests/rers_mini3.c \
    tests/rers_mini4.c \
    tests/rers_mini5b.c \
    tests/rers_mini5.c \
    tests/rers_mini6.c \
    tests/rers_mini7.c \
    tests/test_dowhile1.c \
    tests/test_dowhile2.c \
    tests/test_for1.c \
    tests/test_for2.c \
    tests/test_for3.c \
     \
    tests/matchexpressions/test1.mat \
    tests/matchexpressions/test2.mat \
    tests/matchexpressions/test3.mat \
    tests/matchexpressions/test4.mat \
    tests/matchexpressions/test5.mat \
    tests/matchexpressions/test6.mat \
     \
    tests/rers/Problem10.c \
    tests/rers/Problem10-solutions.txt \
    tests/rers/Problem11.c \
    tests/rers/Problem11-solutions.txt \
    tests/rers/Problem12.c \
    tests/rers/Problem13.c \
    tests/rers/Problem14.c \
    tests/rers/Problem15.c \
    tests/rers/Problem16.c \
    tests/rers/Problem17.c \
    tests/rers/Problem18.c \
    tests/rers/Problem19.c \
    tests/rers/Problem1.c \
    tests/rers/Problem2.c \
    tests/rers/Problem3.c \
    tests/rers/Problem4.c \
    tests/rers/Problem5.c \
    tests/rers/Problem6.c \
    tests/rers/Problem7.c \
    tests/rers/Problem8.c \
    tests/rers/Problem9.c \
    tests/rers/properties10.txt \
    tests/rers/properties11.txt \
    tests/rers/properties12.txt \
    tests/rers/properties13.txt \
    tests/rers/properties14.txt \
    tests/rers/properties15.txt \
    tests/rers/properties16.txt \
    tests/rers/properties17.txt \
    tests/rers/properties18.txt \
    tests/rers/properties19.txt \
    tests/rers/properties1.txt \
    tests/rers/properties2.txt \
    tests/rers/properties3.txt \
    tests/rers/properties4.txt \
    tests/rers/properties5.txt \
    tests/rers/properties6.txt \
    tests/rers/properties7.txt \
    tests/rers/properties8.txt \
    tests/rers/properties9.txt

# MS: does not exist
#    tests/test.ltl \
# xxtest.C # MS: intentionally here as comment
