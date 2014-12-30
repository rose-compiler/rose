include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS = addressTakenAnalysis
COUNTEREXAMPLES = counterexamples/ltl2haskell.sh counterexamples/Makefile counterexamples/qc.hs counterexamples/log2csv.awk
EXTRA_DIST = EqualityMaintainer.C Doxyfile $(TESTS) $(COUNTEREXAMPLES)

codethorn_YFLAGS = -p ltl_ # yacc flags for LTL parser

bin_PROGRAMS = codethorn matcher_demo varwatch rdmain woodpecker analyterix reverser
#ast_demo matcher_demo iterator_test
CLEANFILES = 

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH) -fopenmp -L$(SPOT_PREFIX)/lib

noinst_PROGRAMS = addressTakenAnalysis 

MYDATE = $(shell date +%Y_%m_%d)

INCLUDES = $(ROSE_INCLUDES) -I$(top_builddir) -I$(SPOT_PREFIX)/include/spot -I$(SPOT_PREFIX)/include

# in V1.2 there are no KNOBS
KNOBS = 

codethorn_CXXFLAGS = $(KNOBS) -fopenmp -Wall -O3 -march=native -ftree-vectorize
codethorn_LDADD = -lrose -lspot -lbdd
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
  LTLParser.y++                    \
  LanguageRestrictor.C             \
  LanguageRestrictor.h             \
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
  Lattice.h \
  Lattice.C \
  RDLattice.h                      \
  RDLattice.C                      \
  Evaluator.h                      \
  WorkListSeq.h                    \
  AnalysisAbstractionLayer.h       \
  AnalysisAbstractionLayer.C       \
  RDAstAttribute.h         \
  RDAstAttribute.C         \
  RERS_empty_specialization.C      \
  Flow.h \
  Flow.C \
  PropertyValueTable.h \
  PropertyValueTable.C \
  defUseQuery.C \
  defUseQuery.h \
  VariableIdUtils.C \
  VariableIdUtils.h \
  addressTakenAnalysis.C \
  addressTakenAnalysis.h \
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
  ArrayElementAccessData.h \
  ArrayElementAccessData.C \
  codethorn.h \
  codethorn.C

addressTakenAnalysis_SOURCES = \
  Timer.cpp \
  AstTerm.C \
  AnalysisAbstractionLayer.C \
  Miscellaneous.C \
  CommandLineOptions.C \
  addressTakenAnalysis.h \
  addressTakenAnalysis.C \
  defUseQuery.h \
  defUseQuery.C \
  VariableIdUtils.h \
  VariableIdUtils.C \
  addressTakenAnalysisDriver.C

addressTakenAnalysis_LDADD = -lrose

# used only in specialized analyzer
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
#      end of fake target

REGRESSION_DATA_DIR=regressiondata

.PHONY: codethorn-dist viz bsps docs test checkdemos

# MS: matcher_demo
matcher_demo_LDADD = -lrose
matcher_demo_SOURCES = Timer.cpp AstTerm.C matcher_demo.C
BUILT_SOURCES = 
AM_YFLAGS =
AM_LFLAGS =

varwatch_LDADD = -lrose
varwatch_SOURCES = varwatch.C CommandLineOptions.h CommandLineOptions.C

rdmain_LDADD = -lrose
rdmain_SOURCES = CommandLineOptions.h CommandLineOptions.C \
 CFAnalyzer.h CFAnalyzer.C \
  LanguageRestrictor.h LanguageRestrictor.C \
  LanguageRestrictorCollection.h LanguageRestrictorCollection.C \
  DataDependenceVisualizer.h DataDependenceVisualizer.C \
  AnalysisAbstractionLayer.h \
  AnalysisAbstractionLayer.C \
  RDAstAttribute.h         \
  RDAstAttribute.C         \
  UDAstAttribute.h                 \
  UDAstAttribute.C                 \
  AstTerm.h                        \
  AstTerm.C                        \
  Miscellaneous.C                  \
  Miscellaneous.h                  \
  ProgramStats.h                 \
  ProgramStats.C                 \
  Flow.h \
  Flow.C \
  DFAstAttributeConversion.h DFAstAttributeConversion.C \
  defUseQuery.C \
  defUseQuery.h \
  VariableIdUtils.C \
  VariableIdUtils.h \
  Lattice.h \
  Lattice.C \
  PropertyState.h \
  PropertyState.C \
  IntervalAstAttribute.h \
  IntervalAstAttribute.C \
  IntervalPropertyState.h \
  IntervalPropertyState.C \
  IntervalAnalysis.h \
  IntervalAnalysis.C \
  IntervalPropertyStateFactory.h \
  IntervalPropertyStateFactory.C \
  IntervalTransferFunctions.hpp \
  IntervalTransferFunctions.C \
  IntervalAstAttributeInterface.h \
  IntervalAstAttributeInterface.C \
  NumberIntervalLattice.h \
  NumberIntervalLattice.C \
  PASolver1.h \
  PASolver1.C \
  DFTransferFunctions.h \
  DFTransferFunctions.C \
  DFAnalysis2.h \
  DFAnalysis2.C \
  RDLattice.h RDLattice.C \
  RDTransferfunctions.h RDTransferFunctions.C \
  RDAnalysis.h \
  RDAnalysis.C \
  LVLattice.h \
  LVLattice.C \
  LVTransferFunctions.h \
  LVTransferFunctions.C \
  LVAnalysis.h \
  LVAnalysis.C \
  LVAstAttributeInterface.h \
  LVAstAttributeInterface.C \
  LVAstAttribute.h \
  LVAstAttribute.C \
  addressTakenAnalysis.h \
  addressTakenAnalysis.C \
  FIPointerAnalysis.h \
  FIPointerAnalysis.C \
  PointerAnalysisInterface.h \
  PointerAnalysisInterface.C \
  rdmain.C 

woodpecker_LDADD = -lrose
woodpecker_SOURCES = CommandLineOptions.h CommandLineOptions.C CFAnalyzer.h CFAnalyzer.C LanguageRestrictor.h LanguageRestrictor.C LanguageRestrictorCollection.h LanguageRestrictorCollection.C DataDependenceVisualizer.h DataDependenceVisualizer.C \
  AnalysisAbstractionLayer.h       \
  AnalysisAbstractionLayer.C       \
  AType.h                          \
  AType.C                          \
  CPAstAttribute.h CPAstAttribute.C \
  UDAstAttribute.h                 \
  UDAstAttribute.C                 \
  AstTerm.h                        \
  AstTerm.C                        \
  Miscellaneous.C                  \
  Miscellaneous.h                  \
  ProgramStats.h                 \
  ProgramStats.C                 \
  Flow.h \
  Flow.C \
  FIConstAnalysis.h FIConstAnalysis.C \
  defUseQuery.C \
  defUseQuery.h \
  VariableIdUtils.C \
  VariableIdUtils.h \
  addressTakenAnalysis.C \
  addressTakenAnalysis.h \
  PropertyValueTable.h \
  PropertyValueTable.C \
  TrivialInlining.C                \
  TrivialInlining.h                \
  DeadCodeElimination.C                \
  DeadCodeElimination.h                \
  ReachabilityAnalysis.C \
  ReachabilityAnalysis.h \
  woodpecker.C


analyterix_LDADD = -lrose
analyterix_SOURCES = \
  CommandLineOptions.h CommandLineOptions.C \
  DFAstAttributeConversion.h DFAstAttributeConversion.C \
  AnalysisAbstractionLayer.h       \
  AnalysisAbstractionLayer.C       \
  CFAnalyzer.h \
  CFAnalyzer.C \
  DFAnalysis2.h \
  DFAnalysis2.C \
  PASolver1.h \
  PASolver1.C \
  PointerAnalysisInterface.h \
  PointerAnalysisInterface.C \
  RDLattice.h \
  RDLattice.C \
  RDAnalysis.h \
  RDAnalysis.C \
  DFTransferFunctions.h \
  DFTransferFunctions.C \
  RDTransferFunctions.h \
  RDTransferFunctions.C \
  FIConstAnalysis.h FIConstAnalysis.C \
  LanguageRestrictor.h \
  LanguageRestrictor.C \
  LanguageRestrictorCollection.h \
  LanguageRestrictorCollection.C \
  DataDependenceVisualizer.h \
  DataDependenceVisualizer.C \
  AType.h \
  AType.C \
  RDAstAttribute.h \
  RDAstAttribute.C \
  UDAstAttribute.h \
  UDAstAttribute.C \
  CPAstAttribute.h CPAstAttribute.C \
  AstTerm.h \
  AstTerm.C \
  Miscellaneous.C \
  Miscellaneous.h \
  ProgramStats.h \
  ProgramStats.C \
  Flow.h \
  Flow.C \
  defUseQuery.C \
  defUseQuery.h \
  VariableIdUtils.C \
  VariableIdUtils.h \
  addressTakenAnalysis.C \
  addressTakenAnalysis.h \
  PropertyValueTable.h \
  PropertyValueTable.C \
  Lattice.h \
  Lattice.C \
  PropertyState.h \
  PropertyState.C \
  analyterix.C

reverser_LDADD = -lrose
reverser_SOURCES = \
  LanguageRestrictor.h \
  LanguageRestrictor.C \
  LanguageRestrictorCollection.h \
  LanguageRestrictorCollection.C \
  AstTerm.h \
  AstTerm.C \
  reverser.C

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
#ast_demo_SOURCES = ast_demo.C Timer.cpp Timer.h  RoseAst.C RoseAst.h AstTerm.C AstTerm.h
#ast_demo_LFLAGS = -Pmatcher
#ast_demo_YFLAGS = -d -p matcher


check-local:
	./codethorn --internal-checks				 
	@echo ================================================================
	@echo RUNNING LTL VERIFICATION TESTS
	@echo ================================================================
	rm -f *.consistent
	$(srcdir)/scripts/runLtlRegressionTests $(srcdir) . #run RERS examples in several versions and compare to previous results

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
    tests/rers/Problem901.c

# MS: does not exist
#    tests/test.ltl \
# xxtest.C # MS: intentionally here as comment

