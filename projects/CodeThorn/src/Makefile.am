include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

COUNTEREXAMPLES = counterexamples/ltl2haskell.sh counterexamples/Makefile counterexamples/qc.hs counterexamples/log2csv.awk
EXTRA_DIST = EqualityMaintainer.C Doxyfile $(TESTS) $(COUNTEREXAMPLES)

bin_PROGRAMS = codethorn woodpecker analyterix matcher_demo matcher astinfo cldemo
noinst_PROGRAMS = addressTakenAnalysis matcher_demo matcher astinfo cldemo

CLEANFILES =

if USE_Z3
Z3_INCLUDES = -I$(Z3_INCLUDE_PATH)
Z3_LIB_INCLUDES = -L$(Z3_LIBRARY_PATH)
Z3_LINK = -lz3
else
Z3_INCLUDES =
Z3_LIB_INCLUDES =
Z3_LINK =
endif

if ROSE_WITH_SPOT
AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH) -fopenmp -L$(SPOT_LIBRARY_PATH)
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/Codethorn/src -I$(SPOT_INCLUDE_PATH)/spot -I$(SPOT_INCLUDE_PATH) $(Z3_INCLUDES) -g -rdynamic
else
AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH) -fopenmp $(Z3_LIB_INCLUDES)
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/Codethorn/src $(Z3_INCLUDES) -g -rdynamic
endif

MYDATE = $(shell date +%Y_%m_%d)

lib_LTLIBRARIES = libcodethorn.la

#libcodethorn_la_LDFLAGS= -no-undefined
libcodethorn_la_LDFLAGS=
libcodethorn_la_CXXFLAGS = -fopenmp -Wall -O3
libcodethorn_ladir=.
libcodethorn_la_HEADERS=\
  CounterexampleGenerator.h	   \
  AbstractValue.h \
  AliasAnalysis.h \
  AnalysisAbstractionLayer.h \
  AnalysisAstAnnotator.h \
  AnalysisParameters.h             \
  Analyzer.h                       \
  ArrayElementAccessData.h \
  AstNodeInfo.h \
  AstTermRepresentation.h \
  addressTakenAnalysis.h \
  BoolLattice.h \
  CallString.h \
  CFAnalysis.h \
  CollectionOperators.h            \
  ConstraintRepresentation.h       \
  ContNodeAttribute.h \
  CPAstAttribute.h \
  CTIOLabeler.h                    \
  CtxAnalysis.h \
  CtxCallStrings.h \
  CtxLattice.h \
  CtxTransfer.h \
  DataDependenceVisualizer.h \
  DataRaceDetection.h \
  DFAnalysisBase.h \
  DFTransferFunctions.h \
  DotGraphCfgFrontend.h \
  EqualityMaintainer.h             \
  EquivalenceChecking.h            \
  EState.h           \
  Evaluator.h                      \
  ExecutionTrace.h		   \
  ExprAnalyzer.h                   \
  FIConstAnalysis.h FIConstAnalysis.C \
  FunctionCallMapping.h \
  FunctionCallTarget.h \
  FunctionId.h \
  FunctionIdMapping.h \
  HashFun.h                        \
  HSet.h                           \
  HSetMaintainer.h                 \
  InputOutput.h           \
  InternalChecks.h                 \
  IntervalAnalysis.h \
  IntervalAstAttribute.h \
  IntervalAstAttributeInterface.h \
  IntervalPropertyStateFactory.h \
  IntervalPropertyState.h \
  IntervalTransferFunctions.h \
  LanguageRestrictorCollection.h   \
  LanguageRestrictor.h \
  LoopInfo.h                       \
  LVAnalysis.h \
  LVAstAttribute.h \
  LVAstAttributeInterface.h \
  LVLattice.h \
  LVTransferFunctions.h \
  Miscellaneous2.h \
  Miscellaneous.h                  \
  Normalization.h \
  NormalizationInliner.h \
  NormalizationOp.h \
  NumberIntervalLattice.h \
  PASolver1.h \
  PhiAttribute.h \
  PhiStatement.h \
  PointerAnalysisInterface.h \
  PragmaHandler.h \
  ProgramAbstractionLayer.h \
  ProgramInfo.h \
  ProgramLocationsReport.h \
  PropertyValueTable.h \
  PState.h           \
  RDAnalysis.h \
  RDAstAttribute.h \
  RDTransferFunctions.h \
  ReachabilityAnalysis.h \
  ReadWriteData.h \
  ReadWriteHistory.h \
  RWState.h \
  SetAlgo.h \
  SgTypeSizeMapping.h \
  Solver5.h \
  Solver.h \
  Specialization.h \
  StructureAccessLookup.h \
  SvcompWitness.h		   \
  TimeMeasurement.h \
  TransitionGraph.h                \
  TransitionGraphReducer.h	   \
  TypeSizeMapping.h \
  UDAstAttribute.h                 \
  VariableValueMonitor.h \
  Visualizer.h                     \
  WorkListSeq.h \
  RewriteStatistics.h \
  RewriteSystem.h \
  RERS_empty_specialization.h

libcodethorn_la_SOURCES=\
  AbstractValue.C \
  AliasAnalysis.C \
  AnalysisAbstractionLayer.C \
  AnalysisAstAnnotator.C \
  Analyzer.C                       \
  ArrayElementAccessData.C \
  AstNodeInfo.C \
  AstTermRepresentation.C \
  BoolLattice.C \
  CallString.C \
  CFAnalysis.C \
  ConstraintRepresentation.C       \
  ContNodeAttribute.C \
  CounterexampleGenerator.C	   \
  CPAstAttribute.C \
  CppExprEvaluator.C \
  CTIOLabeler.C                    \
  CtxCallStrings.C \
  DataDependenceVisualizer.C \
  DataRaceDetection.C \
  defUseQuery.C \
  DFAnalysisBase.C \
  DFTransferFunctions.C \
  DotGraphCfgFrontend.C \
  EquivalenceChecking.C            \
  EState.C           \
  ExprAnalyzer.C                   \
  FIConstAnalysis.h FIConstAnalysis.C \
  Flow.C \
  FunctionCallMapping.C \
  FunctionCallTarget.C \
  FunctionId.C \
  FunctionIdMapping.C \
  InputOutput.C           \
  InternalChecks.C                 \
  IntervalAnalysis.C \
  IntervalAstAttribute.C \
  IntervalAstAttributeInterface.C \
  IntervalPropertyState.C \
  IntervalPropertyStateFactory.C \
  IntervalTransferFunctions.C \
  LanguageRestrictor.C \
  LanguageRestrictorCollection.C   \
  Lattice.C \
  LoopInfo.C                       \
  LVAnalysis.C \
  LVAstAttribute.C \
  LVAstAttributeInterface.C \
  LVLattice.C \
  LVTransferFunctions.C \
  Miscellaneous2.C \
  Miscellaneous.C                  \
  Normalization.C \
  NormalizationInliner.C \
  NormalizationOp.C \
  NumberIntervalLattice.C \
  PASolver1.C \
  PhiAttribute.C \
  PhiStatement.C \
  PointerAnalysisInterface.C \
  PragmaHandler.C \
  ProgramAbstractionLayer.C \
  ProgramInfo.C \
  ProgramLocationsReport.C \
  PropertyState.C \
  PropertyValueTable.C \
  PState.C           \
  RDAnalysis.C \
  RDAstAttribute.C \
  RDLattice.C \
  RDTransferFunctions.C \
  ReachabilityAnalysis.C \
  ReadWriteHistory.C \
  RWState.C \
  SgTypeSizeMapping.C \
  Solver5.C \
  Solver.C \
  Specialization.C \
  StructureAccessLookup.C \
  SvcompWitness.C		   \
  TransitionGraph.C                \
  TransitionGraphReducer.C	   \
  TypeSizeMapping.C \
  UDAstAttribute.C                 \
  VariableIdUtils.C \
  VariableValueMonitor.C \
  Visualizer.C \
  RewriteStatistics.C \
  RewriteSystem.C \
  TimeMeasurement.C \
  addressTakenAnalysis.C \
  RERS_empty_specialization.C

codethorn_YFLAGS = -p ltl_ # yacc flags for LTL parser
# this line create speparately named object files
codethorn_CXXFLAGS = -fopenmp -Wall -O3
codethorn_DEPENDENCIES=libcodethorn.la

#codethorn_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn -lspot -lbdd -lz3
if ROSE_WITH_SPOT
codethorn_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lspot -lbdd -lcodethorn $(Z3_LINK)
codethorn_LDFLAGS = -L$(SPOT_LIBRARY_PATH) $(Z3_LIB_INCLUDES)
else
codethorn_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn
endif

codethorndir=.

codethorn_HEADERS = \
  IOAnalyzer.h                     \
  IOSolver.h \
  ReachabilityAnalyzerZ3.h \
  ParallelAutomataGenerator.h \
  LtsminConnection.h		   \
  CounterexampleAnalyzer.h \
  RersCounterexample.h		   \
  SpotConnection.h \
  SpotMiscellaneous.h \
  SpotRenameVisitor.h \
  SpotState.h \
  SpotSuccIter.h \
  SpotTgba.h \
  SSAGenerator.h \
  ParProAnalyzer.h                 \
  ParProEState.h           \
  ParProExplorer.h                 \
  ParProLtlMiner.h                 \
  ParProSpotState.h \
  ParProSpotSuccIter.h \
  ParProSpotTgba.h \
  ParProTransitionGraph.h          \
  Solver8.h \
  Solver10.h \
  Solver11.h \
  Solver12.h \
  PromelaCodeGenerator.h \
  LTL.h                            \
  ReadWriteAnalyzer.h \
  CommandLineOptions.h             \
  codethorn.h

codethorn_SOURCES = \
  IOAnalyzer.C                     \
  IOSolver.C \
  ReachabilityAnalyzerZ3.C \
  ParallelAutomataGenerator.C \
  LtsminConnection.C		   \
  CounterexampleAnalyzer.C \
  RersCounterexample.C		   \
  SpotConnection.C \
  SpotMiscellaneous.C \
  SpotRenameVisitor.C \
  SpotState.C \
  SpotSuccIter.C \
  SpotTgba.C \
  SSAGenerator.C \
  ParProAnalyzer.C                 \
  ParProEState.C           \
  ParProExplorer.C                 \
  ParProLtlMiner.C                 \
  ParProSpotState.C \
  ParProSpotSuccIter.C \
  ParProSpotTgba.C \
  ParProTransitionGraph.C          \
  Solver8.C \
  Solver10.C \
  Solver11.C \
  Solver12.C \
  PromelaCodeGenerator.C \
  LTLParser.y++                    \
  ReadWriteAnalyzer.C \
  CommandLineOptions.C             \
  codethorn.C

addressTakenAnalysis_SOURCES = \
  CommandLineOptions.C \
  addressTakenAnalysisDriver.C

if ROSE_WITH_SPOT
addressTakenAnalysis_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn -lspot -lbdd
addressTakenAnalysis_LDFLAGS = -L$(SPOT_LIBRARY_PATH)
else
addressTakenAnalysis_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
endif
addressTakenAnalysis_CXXFLAGS = -Wall -O3
addressTakenAnalysis_DEPENDENCIES=libcodethorn.la

REGRESSION_DATA_DIR=regressiondata

.PHONY: codethorn-dist viz bsps docs test checkdemos

woodpecker_DEPENDENCIES=libcodethorn.la
if ROSE_WITH_SPOT
woodpecker_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn -lspot -lbdd
woodpecker_LDFLAGS = -L$(SPOT_LIBRARY_PATH)
else
woodpecker_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
endif
woodpecker_CXXFLAGS= -fopenmp -Wall -O3
woodpecker_SOURCES = CommandLineOptions.h CommandLineOptions.C \
  LanguageRestrictorCollection.h LanguageRestrictorCollection.C \
  CPAstAttribute.h CPAstAttribute.C \
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
  RewriteStatistics.h \
  RewriteStatistics.C \
  RewriteSystem.h \
  RewriteSystem.C \
  TimeMeasurement.h \
  TimeMeasurement.C \
  woodpecker.C

#  DFAnalysisBase.C \
#  DFAnalysisBase.h

analyterix_DEPENDENCIES=libcodethorn.la
if ROSE_WITH_SPOT
analyterix_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn -lspot -lbdd
analyterix_LDFLAGS = -L$(SPOT_LIBRARY_PATH)
else
analyterix_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
endif
analyterix_SOURCES = \
  CPAstAttribute.h CPAstAttribute.C \
  CommandLineOptions.h \
  CommandLineOptions.C \
  CppStdUtilities.C \
  CppStdUtilities.h \
  DeadCodeAnalysis.C \
  DeadCodeAnalysis.h \
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

roseonlytools: woodpecker analyterix

check-roseonlytools: check-flow-insenstive


# obsolete since integration into ROSE
# explicit rules because of multiple bison parser
#matcherparser.C: $(srcdir)/matcherlexer.ll $(srcdir)/matcherparser.yy
#	$(YACC) $(YFLAGS) -d -p matcherparser $(srcdir)/matcherparser.yy # generates y.tab.h/c
#	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pmatcherparser $(srcdir)/matcherlexer.ll
#	mv y.tab.h matcherparser.h
#	cat lex.yy.c y.tab.c > matcherparser.C
#	rm lex.yy.c y.tab.c

# MS: matcher_demo
matcher_demo_DEPENDENCIES=libcodethorn.la
matcher_demo_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn
matcher_demo__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
matcher_demo_SOURCES = TimeMeasurement.C matcher_demo.C
BUILT_SOURCES =
AM_YFLAGS =
AM_LFLAGS =

matcher_DEPENDENCIES=libcodethorn.la
matcher_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn
matcher__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
matcher_SOURCES = matcher.C

cldemo_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
cldemo__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
cldemo_SOURCES = cldemo.C

#MS: iterator_test not ingegrated yet
#iterator_test_SOURCES = iterator_test.C ShowSeq.h
#iterator_test_LDADD = -lrose libmatcher.la

#MS: ast_demo not integrated yet
#ast_demo_SOURCES = ast_demo.C TimeMeasurement.C TimeMeasurement.h  RoseAst.C RoseAst.h
#ast_demo_LFLAGS = -Pmatcher
#ast_demo_YFLAGS = -d -p matcher

astinfo_DEPENDENCIES=
astinfo_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
astinfo_SOURCES = astinfo.C LineColInfo.C

check-astinfo: astinfo
	@echo "-------------------------------------------------------------"
	./astinfo --classname --edg:no_warnings $(srcdir)/tests/bugs/bug1.C
	@echo "-------------------------------------------------------------"
	./astinfo --linecol --edg:no_warnings $(srcdir)/tests/bugs/bug2.C
	@echo "-------------------------------------------------------------"
	./astinfo --linecol --edg:no_warnings $(srcdir)/tests/bugs/bug3.C
	@echo "-------------------------------------------------------------"

check-matcher-demo:
	./matcher_demo  --edg:no_warnings $(srcdir)/tests/basictest5.C < $(srcdir)/tests/matchexpressions/test1.mat
#	./ast_demo $(srcdir)/tests/basictest5.C

check-matcher:
	./matcher_demo  --edg:no_warnings $(srcdir)/tests/basictest5.C < $(srcdir)/tests/matchexpressions/test1.mat

CHECK_DEFAULT_PASSING=check-codethorn-internal check-domain check-normalization check-equivalence check-svcomp-witness check-par-cfg

CHECK_DEFAULT_FAILING=check-data-races check-deadcode

#CHECK_WITH_SPOT_ONLY=check-ltl check-ltl-driven
CHECK_WITH_SPOT_ONLY_PASSING=check-ltl check-ltl-driven-reset-analyzer

CHECK_WITH_SPOT_PASSING=$(CHECK_DEFAULT_PASSING) $(CHECK_WITH_SPOT_ONLY_PASSING)

if ROSE_WITH_SPOT
check-local: $(CHECK_WITH_SPOT_PASSING)
else
check-local: $(CHECK_DEFAULT_PASSING)
endif

check-default-failing: $(CHECK_DEFAULT_FAILING)

check-svcomp:
	@echo ================================================================
	@echo RUNNING VERIFICATION ERROR TESTS
	@echo ================================================================
	@./codethorn $(srcdir)/tests/svcomp-test2.c --edg:no_warnings
	@./codethorn $(srcdir)/tests/svcomp-test4.c --max-transitions=10 --edg:no_warnings
	@./codethorn $(srcdir)/tests/svcomp-test5.c --edg:no_warnings

check-codethorn-internal:
	@echo ================================================================
	@echo RUNNING CODETHORN INTERNAL CHECKS
	@echo ================================================================
	@./codethorn --internal-checks

check-domain:
	$(srcdir)/scripts/runDomainTests $(srcdir)/tests

check-domain-old:
	@echo ================================================================
	@echo DOMAIN TESTS
	@echo ================================================================
	@xxx
	@./codethorn --edg:no_warnings $(srcdir)/tests/domaintest1.C
	@./codethorn --edg:no_warnings $(srcdir)/tests/domaintest2.C
	@./codethorn --edg:no_warnings $(srcdir)/tests/struct1.C
	@./codethorn --edg:no_warnings $(srcdir)/tests/struct2.C
	@./codethorn --edg:no_warnings --stg-trace-file=trace.txt $(srcdir)/tests/struct3.C
	@./codethorn $(srcdir)/tests/intertest14.C --viz && dot -Tpdf transitiongraph1.dot -otransitiongraph1.pdf

check-data-races:
	@echo ================================================================
	@echo RUNNING DATA RACE VERIFICATION TESTS
	@echo ================================================================
	@./codethorn --data-race=yes  $(srcdir)/tests/datarace/anti_dep_1.yes.c --max-time=5 --solver=12
	@./codethorn --data-race=yes  $(srcdir)/tests/datarace/inner_only_1.no.c --max-time=5 --solver=12


#check-flow-insensitive: check-analyterix check-const-analysis
check-flow-insensitive: check-const-analysis

check-analyterix:
	@echo ================================================================
	@echo RUNNING STATIC PROGRAM ANALYSIS TESTS
	@echo ================================================================
	@$(srcdir)/scripts/runAnalyterixTests.sh "$(srcdir)" "$(top_builddir)/projects/CodeThorn/src" "$(clean-only)" "$(skip-analyterix)" # Run analyterix tests

check-const-analysis:
	@echo ================================================================
	@echo RUNNING CONST ANALYSIS CHECK
	@echo ================================================================
	@./woodpecker --edg:no_warnings --csv-const-result=tmp.const.csv $(srcdir)/tests/Problem1401_opt.pp.c
	@diff tmp.const.csv $(srcdir)/tests/Problem1401_opt.pp.const.csv
	@rm tmp.const.csv


check-par-cfg:
	@echo ================================================================
	@echo RUNNING PARALLEL CFG CHECK
	@echo ================================================================
	@echo 01
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-01-SimpleOmpParallelFor.c
	@echo 02
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-02-OnlyOmpParallel.c
	@echo 03
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-03-OmpParallelOmpForLoop.c
	@echo 04
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-04-OmpParLoopTrailStmt.c
	@echo 05
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-05-LoopOmpParallelFor.c
	@echo 06
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-06-OmpParallelNestedForLoops.c
	@echo 07
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-07-ParallelOmpForNestedOmpParallelFor.c
	@echo 08
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-08-ParallelForParallelFor.c
	@echo 09
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-09-ParallelConsecutiveForLoops.c
	@echo 10
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-10-ParallelForContinueStmt.c
	@echo 11
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-11-ParallelForSeqStmtsParallelFor.c
	@echo 12
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-12-SeqForParForNestedParFor.c
	@echo 13
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-13-ParForNestedParForNestedParFor.c
	@echo 14
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-14-NestedFollowingNestedPar.c
	@echo 15
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-15-ParConsecutiveNoWaitFor.c
	@echo 20
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-20-ParallelForCallFuncParallelFor.c
	@echo 30
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-30-ParSectionsForLoop.c
	@echo 31
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-31-ParSectionsAssignment.c
	@echo 32
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-32-ParSectionsAssignments.c
	@echo 33
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-33-ParSectionsParForLoop.c
	@echo 34
	@./codethorn --omp-ast=yes $(srcdir)/tests/ompcfg/Par-34-ParSectionsNoWait.c



# outdated tests (but still passing)
check-ltl-old: check-ltl-rers-different-versions

# failing tests due to changes in domain
#check-ltl-failing: check-ltl-rers-array

# current tests all passing
check-ltl: check-ltl-start check-ltl-rers-arithmetic check-ltl-rers-timeout check-ltl-rers-loop-aware-sync check-ltl-rers-topify check-ltl-cegpra check-ltl-rers-array

check-ltl-start:
	@echo ================================================================
	@echo RUNNING LTL VERIFICATION TESTS
	@echo ================================================================
	@rm -f *.consistent

check-ltl-rers-different-versions:
	@$(srcdir)/scripts/runRersTestsDifferentVersion $(srcdir) $(top_builddir)/projects/CodeThorn/src #run RERS Problem1 from 2012 in several versions and compare to previous results

check-ltl-rers-arithmetic:
	@$(srcdir)/scripts/runRersArithmeticProblemTest $(srcdir) $(top_builddir)/projects/CodeThorn/src #test the parallel analysis of RERS Problem1402 (4 threads, problem features arithmetic)

check-ltl-rers-array:
	@$(srcdir)/scripts/runRersArrayProblemTest $(srcdir) $(top_builddir)/projects/CodeThorn/src #test the analysis of RERS Problem1403 (which features arrays)

check-ltl-rers-topify:
	@$(srcdir)/scripts/runRersTopifyTest $(srcdir) $(top_builddir)/projects/CodeThorn/src #test the analysis of RERS Problem1403, abstracting from global variables after 10,000 transitions

# SKIPPED
check-ltl-rers-memory-bound:
	@$(srcdir)/scripts/runRersMemoryBoundTest $(srcdir) $(top_builddir)/projects/CodeThorn/src #test the analysis of RERS Problem1603 while using up to 1GB of RAM up to and including the STG computation (4 threads)

check-ltl-rers-timeout:
	@$(srcdir)/scripts/runRersTimeoutTest $(srcdir) $(top_builddir)/projects/CodeThorn/src #test the analysis of RERS Problem1402 and terminate after 5 seconds (4 threads)
check-ltl-rers-loop-aware-sync:
	@$(srcdir)/scripts/runRersLoopAwareSyncTest $(srcdir) $(top_builddir)/projects/CodeThorn/src #one test for the loop-aware-sync analysis (solver 12, ISoLA'16 paper)
check-ltl-cegpra:
	@$(srcdir)/scripts/run_tests_cegpra $(srcdir) $(top_builddir)/projects/CodeThorn/src #run CEGPRA tests

check-arrays:
	./codethorn $(srcdir)/tests/pointer/test48.cpp --explicit-arrays=yes --viz=yes && dot -Tpdf transitiongraph1.dot -otransitiongraph1.pdf

check-array-bounds: ./codethorn
	./codethorn --explicit-arrays=yes $(srcdir)/tests/arraybounds1.C

# seg-faults with --reset-analyzer=no. Works with --reset-analyzer=yes
check-ltl-driven:
	./codethorn $(srcdir)/tests/rers/Problem1401_opt.c --rersmode=yes --with-counterexamples=yes --counterexamples-with-output=yes --input-values="{1,2,3,4,5}" --ltl-in-alphabet="{1,2,3,4,5}" --ltl-out-alphabet="{18,19,20,21,22,23,24,25,26}" --check-ltl=$(srcdir)/tests/rers/constraints-RERS14-5.txt  --display-diff=100000 --ltl-driven --reset-analyzer=no

check-ltl-driven-reset-analyzer:
	./codethorn $(srcdir)/tests/rers/Problem1401_opt.c --rersmode=yes --with-counterexamples=yes --counterexamples-with-output=yes --input-values="{1,2,3,4,5}" --ltl-in-alphabet="{1,2,3,4,5}" --ltl-out-alphabet="{18,19,20,21,22,23,24,25,26}" --check-ltl=$(srcdir)/tests/rers/constraints-RERS14-5.txt  --display-diff=100000 --ltl-driven --reset-analyzer=yes

check-svcomp-witness:
	./codethorn $(srcdir)/tests/svcomp/eca-rers2012/Problem01_label15_false-unreach-call.c --svcomp-mode --input-values="{1,2,3,4,5,6}" --witness-file=toBeImplemented.witness --with-counterexamples

check-equivalence:
	@rm -f tmp.nsdump
	@echo ================================================================
	@echo RUNNING UPDATE SEQUENCE VERIFICATION TESTS
	@echo ================================================================
	@./codethorn --edg:no_warnings $(srcdir)/tests/jacobi-1d-imper_mod.c --dump-non-sorted=tmp.nsdump --rule-commutative-sort=no
#	@diff tmp.nsdump $(srcdir)/tests/jacobi-1d-imper_mod.c.nsdump
	@cat tmp.nsdump
	@rm -f tmp.nsdump

check-commandline-options: ./codethorn
	@echo ================================================================
	@echo RUNNING COMMAND LINE OPTION TESTS
	@echo ================================================================
	@./codethorn --edg:no_warnings --cl-options="a bbb cc" $(srcdir)/tests/commandlineoptions1.C --viz
	@dot -Tpdf transitiongraph1.dot -otransitiongraph1.pdf

check-deadcode:
	$(srcdir)/scripts/runDeadCodeTests $(srcdir)/tests

check-normalization:
	$(srcdir)/scripts/runNormalizationTests $(srcdir)/tests

check-dev:
	./codethorn $(srcdir)/tests/DOM007_sequence_arrays.C --stg-trace-file=trace.txt --log-level=trace
	@echo "Generated trace.txt for DOM007_sequence_arrays.C."
check-dev2:
	./codethorn $(srcdir)/tests/DOM008_sequence_structs.C --stg-trace-file=trace.txt --log-level=trace
	@echo "Generated trace.txt for DOM008_sequence_structs.C."

# MS: 1-6 is reasonable for V1.2
RERS=$(patsubst %,Problem%.log, $(shell seq 6))
rers: $(RERS)

docs:
	cd "$(srcdir)" && doxygen

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
	rm -f tmp.nsdump
	rm -f rose_Problem1401_opt.pp.c
	rm -f Problem1401_assert_results_forcedtop_1000_n_bf.csv
	rm -f transitiongraph1.pdf
	rm -f transitiongraph2.pdf
	rm -f rose_NORM*.[Cc]

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

demo:
	$(srcdir)/demodir/rundemo.sh $(srcdir)/demodir $(top_builddir)/projects/CodeThorn
