include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS = addressTakenAnalysis
COUNTEREXAMPLES = counterexamples/ltl2haskell.sh counterexamples/Makefile counterexamples/qc.hs counterexamples/log2csv.awk
EXTRA_DIST = EqualityMaintainer.C Doxyfile $(TESTS) $(COUNTEREXAMPLES)

codethorn_YFLAGS = -p ltl_ # yacc flags for LTL parser

bin_PROGRAMS = codethorn matcher_demo varwatch woodpecker analyterix

#ast_demo matcher_demo iterator_test
CLEANFILES = 

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH) -fopenmp -L$(SPOT_PREFIX)/lib

noinst_PROGRAMS = addressTakenAnalysis 

MYDATE = $(shell date +%Y_%m_%d)

INCLUDES = $(ROSE_INCLUDES) -I$(top_builddir) -I$(SPOT_PREFIX)/include/spot -I$(SPOT_PREFIX)/include

# this line create speparately named object files
codethorn_CXXFLAGS = $(KNOBS) -fopenmp -Wall -O3 -march=native -ftree-vectorize

codethorn_DEPENDENCIES=libspray.la
codethorn_LDADD = -lrose -lspot -lbdd -lspray
codethorn_SOURCES = \
  Analyzer.C                       \
  Analyzer.h                       \
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
  LTLParser.y++                    \
  LanguageRestrictorCollection.C   \
  LanguageRestrictorCollection.h   \
  Miscellaneous.C                  \
  Miscellaneous.h                  \
  SetAlgo.h                        \
  StateRepresentations.C           \
  StateRepresentations.h           \
  Timer.cpp                        \
  Timer.h                          \
  Visualizer.C                     \
  Visualizer.h                     \
  Evaluator.h                      \
  RERS_empty_specialization.C      \
  PropertyValueTable.h \
  PropertyValueTable.C \
  RewriteSystem.C \
  RewriteSystem.h \
  SpotConnection.C \
  SpotConnection.h \
  SpotTgba.C \
  SpotTgba.h \
  SpotState.C \
  SpotState.h \
  SpotSuccIter.C \
  SpotSuccIter.h \
  CounterexampleAnalyzer.C \
  CounterexampleAnalyzer.h \
  RefinementConstraints.C \
  RefinementConstraints.h \
  ArrayElementAccessData.h \
  ArrayElementAccessData.C \
  Specialization.h \
  Specialization.C \
  PragmaHandler.h \
  PragmaHandler.C \
  codethorn.h \
  codethorn.C

addressTakenAnalysis_SOURCES = \
  Timer.cpp \
  Miscellaneous.C \
  CommandLineOptions.C \
  addressTakenAnalysisDriver.C

addressTakenAnalysis_LDADD = -lrose -lspray
addressTakenAnalysis_DEPENDENCIES=libspray.la

# workaround to intentionally fool policy
faketarget:
	RERS_Problem10_specialization.C
	RERS_Problem1_specialization.C
	RERS_Problem2_specialization.C
	RERS_Problem3_specialization.C
	RERS_Problem4_specialization.C
	RERS_Problem5_specialization.C
	RERS_Problem6_specialization.C
	RERS_Problem7_specialization.C
	RERS_Problem8_specialization.C
	RERS_Problem9_specialization.C
	rers_cpp_forward_decls.C
	RDLattice.h
	RDLattice.C
	WorkList.h
	WorkList.C
	SetAlgo.h
	ast_demo.C
	ShowSeq.h
	iterator_test.C
	WorkListOMP.h
	WorkListOMP.C
	WorkListSeq.C
	DeadCodeEliminationOperators.C 
	spotconnection/ltlverifier.C 
	spotconnection/stg2.c 
	spotconnection/stg2_cpp.C 
	tests/Problem28.c 
	tests/artificialsum1.C 
	tests/artificialsum1.c 
	tests/artificialsum2.C 
	tests/artificialsum3.C 
	tests/artificialsum4.C 
	tests/artificialsum_pointers1.C 
	tests/jacobi.c 
	tests/jacobi_normalized.c 
	tests/jacobi_seq.c 
	tests/logictest1.C 
	tests/rdtest1.C 
	tests/rdtest2.C 
	tests/rdtest3.C 
	tests/rdtest4.C 
	tests/rers13_mini1.C 
	CppExprEvaluator.h
	CppExprEvaluator.C
	AbstractValue.C
	AbstractValue.h
	CPDomain.hpp
	Domain.hpp
	GenericIntervalLattice.h
	IOSequenceGenerator.C
	InfoOperations.C
	ReachabilityResults.C
	ReachabilityResults.h
	cpptests/cpp-test-1.C
	intervalLattice/testIntervalLattice.C
	outdated.cpp
	rtests/array1/checkarrayoperators.cpp
	rtests/linked-list/900_create_insert_delall.C
	rtests/linked-list/901_create_insert_delall.C
	rtests/linked-list/902_create_insert_delall.C
	rtests/linked-list/rose_900_create_insert_delall.C
	rtests/linked-list/rose_900_create_insert_delall_mod2.C
	rtests/linked-list/rose_901_create_insert_delall.C
	rtests/linked-list/rose_902_create_insert_delall.C
	rtests/linked-list/test_2014_09_26_rose_900_create_insert_delall.C
	rtests/linked-list/test_rose_900_create_insert_delall_mod.C
	rtests/ross-event/rose_simple2-event-funcs-modified-2.c
	rtests/ross-event/rose_simple2-event-funcs-modified.c
	rtests/ross-event/runtime.C
	rtests/ross-event/runtime.h
	rtests/ross-event/runtime_macros.h
	rtests/ross-event/simple2-bs.c
	rtests/ross-event/simple2-event-funcs.c
	rtests/ross-event/simple2.c
	rtests/vector1/RevStack.h
	rtests/vector1/rev_rtest3.C
	rtests/vector1/rev_rtest4.C
	rtests/vector1/rev_rtest4.cpp.C
	rtests/vector1/rev_rtest4.cpppure.C
	rtests/vector1/revcheck.C
	rtests/vector1/revtest.C
	rtests/vector1/rose_rev_rtest4.C
	rtests/vector1/rose_rev_rtest4.cpp.C
	rtests/vector1/rose_rev_rtest4.cpppure.C
	rtests/vector1/rose_rev_rtest4.cpppure2.C
	rtests/vector1/rtest1.C
	rtests/vector1/rtest2.C
	runtime/isstacktest.C
	runtime/runtime.C
	runtime/runtime.h
	runtime/runtime_macros.h
	runtime/testruntimesystem.C
	spotconnection/spotdriver.C
	spotconnection/spotdriver.h
	testAbstractValue.C
	tests/Problem1403_mymod.c
	tests/arraytest1.c
	tests/basictest17.C
	tests/cfgtest1.C
	tests/consttest1.C
	tests/consttest2.C
	tests/consttest3.C
	tests/consttest4.C
	tests/csettest1.c
	tests/lvtest1.C
	tests/lvtest2.C
	tests/lvtest3.C
	tests/rers/Problem1T_size5_14.c
	tests/rers/Problem1_mod.c
	tests/rers/Problem1_pmod.c
	tests/rers/inputsequencepack/Problem1_pmod.c
	tests/rev1.C
	tests/cptest10.C
	tests/cptest1.C
	tests/cptest2.C
	tests/cptest3.C
	tests/cptest4.C
	tests/cptest5.C
	tests/cptest6.C
	tests/cptest7.C
	tests/cptest8.C
	tests/cptest9.C
	tests/mailinglist2.C
	tests/mailinglist3.C
	tests/arraytest2.C
	addressTakenAnalysis.h
	addressTakenAnalysis_obsolete/addressTakenAnalysis.h
	addressTakenAnalysis_obsolete/defUseQuery.h
	defUseQuery.h
	Lattice.h
	rtests/ross-event/runtime.h
	runtime/runtime.h
	rtests/ross-event/runtime_macros.h
	runtime/runtime_macros.h
	Timer.h
	VariableIdUtils.h
	addressTakenAnalysis_obsolete/VariableIdUtils.h
	CPDomain.hpp
	LTL.h
	rtests/vector1/RevStack.h 
	tests/rers/Problem1401_opt.c
	tests/rers/Problem1402_opt.c
	tests/rers/Problem1403_opt.c
	tests/varid-test1.C
	tests/arraytest3.C
	tests/basicblocks1.C
	tests/blocks1.C
	tests/interval-test1.C
	tests/interval-test2.C
	tests/polyopt/jacobi-1d-imper.h
	tests/polyopt/jacobi-1d-imper_annot.c
	tests/polyopt/jacobi-2d-imper.h
	tests/polyopt/jacobi-2d-imper_orig_tileonly_size16_tile_8_2_1.c
	tests/polyopt/polybench.h
#      end of fake target

REGRESSION_DATA_DIR=regressiondata

.PHONY: codethorn-dist viz bsps docs test checkdemos

# MS: matcher_demo
matcher_demo_DEPENDENCIES=libspray.la
matcher_demo_LDADD = -lrose -lspray
matcher_demo_SOURCES = Timer.cpp matcher_demo.C
BUILT_SOURCES = 
AM_YFLAGS =
AM_LFLAGS =

varwatch_DEPENDENCIES=libspray.la
varwatch_LDADD = -lrose -lspray
varwatch_SOURCES = varwatch.C CommandLineOptions.h CommandLineOptions.C

#rdmain_LDADD = -lrose
#rdmain_SOURCES = CommandLineOptions.h CommandLineOptions.C \
  LanguageRestrictorCollection.h LanguageRestrictorCollection.C \
  DataDependenceVisualizer.h DataDependenceVisualizer.C \
  UDAstAttribute.h                 \
  UDAstAttribute.C                 \
  Miscellaneous.C                  \
  Miscellaneous.h                  \
  ProgramStats.h                 \
  ProgramStats.C                 \
  Flow.h \
  Flow.C \
  DFAstAttributeConversion.h DFAstAttributeConversion.C \
  PropertyState.h \
  PropertyState.C \
  FIPointerAnalysis.h \
  FIPointerAnalysis.C \
  rdmain.C 

woodpecker_DEPENDENCIES=libspray.la
woodpecker_LDADD = -lrose -lspray
woodpecker_SOURCES = CommandLineOptions.h CommandLineOptions.C \
 LanguageRestrictorCollection.h LanguageRestrictorCollection.C \
  DataDependenceVisualizer.h DataDependenceVisualizer.C \
  CPAstAttribute.h CPAstAttribute.C \
  UDAstAttribute.h                 \
  UDAstAttribute.C                 \
  Miscellaneous.C                  \
  Miscellaneous.h                  \
  ProgramStats.h                 \
  ProgramStats.C                 \
  FIConstAnalysis.h FIConstAnalysis.C \
  PropertyValueTable.h \
  PropertyValueTable.C \
  Threadification.h \
  Threadification.C \
  TrivialInlining.C                \
  TrivialInlining.h                \
  DeadCodeElimination.C                \
  DeadCodeElimination.h                \
  ReachabilityAnalysis.C \
  ReachabilityAnalysis.h \
  ConversionFunctionsGenerator.h \
  ConversionFunctionsGenerator.C \
  woodpecker.C


#  DFAnalysisBase.C \
#  DFAnalysisBase.h

analyterix_DEPENDENCIES=libspray.la
analyterix_LDADD = -lrose -lspray
analyterix_SOURCES = \
  CPAstAttribute.h CPAstAttribute.C \
  CommandLineOptions.h \
  CommandLineOptions.C \
  DataDependenceVisualizer.C \
  DataDependenceVisualizer.h \
  DFAstAttributeConversion.h DFAstAttributeConversion.C \
  FIConstAnalysis.h FIConstAnalysis.C \
  FIPointerAnalysis.C \
  FIPointerAnalysis.h \
  LanguageRestrictorCollection.C \
  LanguageRestrictorCollection.h \
  Miscellaneous.C \
  Miscellaneous.h \
  ProgramStats.C \
  ProgramStats.h \
  PropertyValueTable.C \
  PropertyValueTable.h \
  RoseRDAnalysis.C \
  RoseRDAnalysis.h \
  TransformationOperators.h \
  TransformationOperators.C \
  UDAstAttribute.C \
  UDAstAttribute.h \
  analyterix.C

# obsolete since integration into ROSE
# explicit rules because of multiple bison parser
#matcherparser.C: $(srcdir)/matcherlexer.ll $(srcdir)/matcherparser.yy
#	$(YACC) $(YFLAGS) -d -p matcherparser $(srcdir)/matcherparser.yy # generates y.tab.h/c
#	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pmatcherparser $(srcdir)/matcherlexer.ll
#	mv y.tab.h matcherparser.h
#	cat lex.yy.c y.tab.c > matcherparser.C
#	rm lex.yy.c y.tab.c

#MS: iterator_test not ingegrated yet
#iterator_test_SOURCES = iterator_test.C ShowSeq.h
#iterator_test_LDADD = -lrose libmatcher.la

#MS: ast_demo not integrated yet
#ast_demo_SOURCES = ast_demo.C Timer.cpp Timer.h  RoseAst.C RoseAst.h
#ast_demo_LFLAGS = -Pmatcher
#ast_demo_YFLAGS = -d -p matcher

pkglib_LTLIBRARIES = libspray.la
libspray_la_LDFLAGS= -no-undefined
libspray_la_LDFLAGS=
libspray_la_SOURCES=\
 AliasAnalysis.h \
 AnalysisAbstractionLayer.h \
 AnalysisAstAnnotator.h \
 AstTerm.h \
 AType.h \
 CFAnalysis.h \
 DFAnalysisBase.h \
 DFTransferFunctions.h \
 IntervalAnalysis.h \
 IntervalAstAttribute.h \
 IntervalAstAttributeInterface.h \
 IntervalPropertyState.h \
 IntervalPropertyStateFactory.h \
 IntervalTransferFunctions.h \
 LVAnalysis.h \
 LVAstAttribute.h \
 LVAstAttributeInterface.h \
 LVLattice.h \
 LVTransferFunctions.h \
 LanguageRestrictor.h \
 Miscellaneous2.h \
 NumberIntervalLattice.h \
 PASolver1.h \
 PointerAnalysisInterface.h \
 RDAnalysis.h \
 RDAstAttribute.h \
 RDTransferFunctions.h \
 WorkListSeq.h \
 AnalysisAstAnnotator.C \
 AliasAnalysis.C \
 AstTerm.C \
 AType.C \
 CFAnalysis.C \
 CppExprEvaluator.C \
 DFAnalysisBase.C \
 DFTransferFunctions.C \
 Flow.C \
 IntervalAnalysis.C \
 IntervalAstAttribute.C \
 IntervalPropertyStateFactory.C \
 IntervalPropertyState.C \
 IntervalTransferFunctions.C \
 IntervalAstAttributeInterface.C \
 LanguageRestrictor.C \
 Lattice.C \
 LVAnalysis.C \
 LVAstAttribute.C \
 LVAstAttributeInterface.C \
 LVLattice.C \
 LVTransferFunctions.C \
 Miscellaneous2.C \
 NumberIntervalLattice.C \
 PASolver1.C \
 PointerAnalysisInterface.C \
 PropertyState.C \
 RDAnalysis.C \
 RDAstAttribute.C \
 RDLattice.C \
 RDTransferFunctions.C \
 VariableIdUtils.C \
 AnalysisAbstractionLayer.C \
 addressTakenAnalysis.C \
 defUseQuery.C

check-local:
	./codethorn --internal-checks				 
	@echo ================================================================
	@echo RUNNING LTL VERIFICATION TESTS
	@echo ================================================================
	rm -f *.consistent
	$(srcdir)/scripts/runLtlRegressionTests $(srcdir) . #run RERS examples in several versions and compare to previous results
	$(srcdir)/scripts/run_tests_cegpra $(srcdir) . #run CEGPRA tests

checkdemos: 
	./matcher_demo $(srcdir)/tests/basictest5.C < $(srcdir)/tests/matchexpressions/test1.mat
	./ast_demo $(srcdir)/tests/basictest5.C

# MS: 1-6 is reasonable for V1.2
RERS=$(patsubst %,Problem%.log, $(shell seq 6))
rers: $(RERS)

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
	rm -f *.jpg
	rm -f viz/*
	rm -f bsps/*
	rm -f *.consistent Problem*.[0-9].csv
	rm -f codethorn-LTLParser.c++

distclean-local: clean
	rm -f *.tgz
	rm -f *~
	rm -rf ../docs/doxygen
	rm -f codethornref

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

	./codethorn --rersmode=yes $(srcdir)/tests/rers/Problem1.c
	cp $(srcdir)/tests/rers/Problem1.c bsps
	dot -Tjpg transitiongraph2.dot -oviz/transitiongraph2.jpg
	cp viz/transitiongraph2.jpg bsps/rers1_transitiongraph2.jpg

codethorn-dist:
	tar cvzf codethorn_$(MYDATE).tgz *.C *cpp *.h *.lxx *.yxx tests/*.C tests/*.c Makefile*

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
    tests/funccalltest2a.c \
    tests/funccalltest2b.C \
    tests/funccalltest1a.c \
    tests/funccalltest1b.C \
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
    tests/rers_mini8.c \
    tests/rers_mini9.c \
    tests/rers_mini9.ltl \
    tests/test_dowhile1.c \
    tests/test_dowhile2.c \
    tests/test_for1.c \
    tests/test_for2.c \
    tests/test_for3.c \
    tests/rers_mini10.c \
    tests/rers_mini11.c \
    tests/rers_mini12.c \
    tests/rers_mini13.c \
    tests/rers_mini14.c \
    tests/rers_mini15.c \
    tests/test_for4.c \
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
    tests/rers/properties9.txt \
    tests/rers/Problem28_mod.c \
    tests/rers/Problem28_opt.C \
    tests/rers/Problem901-solutions.csv \
    tests/rers/Problem901-solutions.ltl.csv \
    tests/rers/Problem901.c \
    tests/gototest1.C \
    tests/gototest2.C \
    tests/gototest3.C \
    tests/switchtest1.C \
    tests/switchtest2.C \
    tests/switchtest3.C \
    examples/example-invocations.sh 

