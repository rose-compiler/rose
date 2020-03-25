include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

COUNTEREXAMPLES = counterexamples/ltl2haskell.sh counterexamples/Makefile counterexamples/qc.hs counterexamples/log2csv.awk
EXTRA_DIST = Doxyfile $(TESTS) $(COUNTEREXAMPLES)

bin_PROGRAMS = codethorn woodpecker analyterix addressTakenAnalysis cldemo

# matcher_demo matcher astinfo cldemo
noinst_PROGRAMS = addressTakenAnalysis cldemo
#noinst_PROGRAMS matcher_demo matcher astinfo cldemo

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

lib_LTLIBRARIES = libcodethorn.la libltlthorn.la

##############################################################
# LIBRARY: CODETHORN (CORE)
##############################################################

CODETHORN_SOLO_HEADER_FILES=\
  AnalysisParameters.h \
  CollectionOperators.h \
  CtxAnalysis.h \
  CtxAttribute.h \
  CtxLattice.h \
  CtxTransfer.h \
  EqualityMaintainer.h \
  Evaluator.h \
  ExecutionTrace.h \
  HashFun.h \
  HSetMaintainer.h \
  ReadWriteData.h \
  SetAlgo.h \
  WorkListSeq.h \
  WorkList.h

CODETHORN_HEADER_FILES=\
  AbstractValue.h \
  addressTakenAnalysis.h \
  AliasAnalysis.h \
  AnalysisAbstractionLayer.h \
  AnalysisAstAnnotator.h \
  Analyzer.h \
  ArrayElementAccessData.h \
  AstNodeInfo.h \
  AstTermRepresentation.h \
  BoolLattice.h \
  CallString.h \
  CFAnalysis.h \
  CommandLineOptions.h \
  ConstraintRepresentation.h \
  ContNodeAttribute.h \
  CounterexampleGenerator.h \
  CPAstAttribute.h \
  CppExprEvaluator.h \
  CppStdUtilities.h \
  CTIOLabeler.h \
  CtxCallStrings.h \
  DataDependenceVisualizer.h \
  defUseQuery.h \
  DFAnalysisBase.h \
  DFTransferFunctions.h \
  DotGraphCfgFrontend.h \
  EquivalenceChecking.h \
  EState.h \
  ExprAnalyzer.h \
  FIConstAnalysis.h \
  Flow.h \
  FunctionCallMapping.h \
  FunctionCallMapping2.h \
  FunctionCallTarget.h \
  FunctionId.h \
  FunctionIdMapping.h \
  InputOutput.h \
  InternalChecks.h \
  IntervalAnalysis.h \
  IntervalAstAttribute.h \
  IntervalAstAttributeInterface.h \
  IntervalPropertyStateFactory.h \
  IntervalPropertyState.h \
  IntervalTransferFunctions.h \
  LanguageRestrictorCollection.h \
  LanguageRestrictor.h \
  Lattice.h \
  LoopInfo.h \
  LVAnalysis.h \
  LVAstAttribute.h \
  LVAstAttributeInterface.h \
  LVLattice.h \
  LVTransferFunctions.h \
  Miscellaneous2.h \
  Miscellaneous.h \
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
  PropertyState.h \
  PropertyValueTable.h \
  PState.h \
  RDAnalysis.h \
  RDAstAttribute.h \
  RDLattice.h \
  RDTransferFunctions.h \
  ReachabilityAnalysis.h \
  ReadWriteHistory.h \
  RERS_empty_specialization.h \
  RewriteStatistics.h \
  RewriteSystem.h \
  RWState.h \
  SgTypeSizeMapping.h \
  Solver5.h \
  Solver.h \
  Specialization.h \
  StructureAccessLookup.h \
  SvcompWitness.h \
  TimeMeasurement.h \
  TransitionGraph.h \
  TransitionGraphReducer.h \
  TypeSizeMapping.h \
  UDAstAttribute.h \
  VariableIdUtils.h \
  VariableIdMappingExtended.h \
  VariableValueMonitor.h \
  Visualizer.h

CODETHORN_SOURCE_FILES=\
  AbstractValue.C \
  addressTakenAnalysis.C \
  AliasAnalysis.C \
  AnalysisAbstractionLayer.C \
  AnalysisAstAnnotator.C \
  Analyzer.C \
  ArrayElementAccessData.C \
  AstNodeInfo.C \
  AstTermRepresentation.C \
  BoolLattice.C \
  CallString.C \
  CFAnalysis.C \
  CommandLineOptions.C \
  ConstraintRepresentation.C \
  ContNodeAttribute.C \
  CounterexampleGenerator.C \
  CPAstAttribute.C \
  CppExprEvaluator.C \
  CppStdUtilities.C \
  CTIOLabeler.C \
  CtxCallStrings.C \
  DataDependenceVisualizer.C \
  defUseQuery.C \
  DFAnalysisBase.C \
  DFTransferFunctions.C \
  DotGraphCfgFrontend.C \
  EquivalenceChecking.C \
  EState.C \
  ExprAnalyzer.C \
  FIConstAnalysis.C \
  Flow.C \
  FunctionCallMapping.C \
  FunctionCallMapping2.C \
  FunctionCallTarget.C \
  FunctionId.C \
  FunctionIdMapping.C \
  InputOutput.C \
  InternalChecks.C \
  IntervalAnalysis.C \
  IntervalAstAttribute.C \
  IntervalAstAttributeInterface.C \
  IntervalPropertyStateFactory.C \
  IntervalPropertyState.C \
  IntervalTransferFunctions.C \
  LanguageRestrictorCollection.C \
  LanguageRestrictor.C \
  Lattice.C \
  LoopInfo.C \
  LVAnalysis.C \
  LVAstAttribute.C \
  LVAstAttributeInterface.C \
  LVLattice.C \
  LVTransferFunctions.C \
  Miscellaneous2.C \
  Miscellaneous.C \
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
  PState.C \
  RDAnalysis.C \
  RDAstAttribute.C \
  RDLattice.C \
  RDTransferFunctions.C \
  ReachabilityAnalysis.C \
  ReadWriteHistory.C \
  RERS_empty_specialization.C \
  RewriteStatistics.C \
  RewriteSystem.C \
  RWState.C \
  SgTypeSizeMapping.C \
  Solver5.C \
  Solver.C \
  Specialization.C \
  StructureAccessLookup.C \
  SvcompWitness.C \
  TimeMeasurement.C \
  TransitionGraph.C \
  TransitionGraphReducer.C \
  TypeSizeMapping.C \
  UDAstAttribute.C \
  VariableIdUtils.C \
  VariableIdMappingExtended.C \
  VariableValueMonitor.C \
  Visualizer.C

#libcodethorn_la_LDFLAGS= -no-undefined
libcodethorn_la_LDFLAGS=
libcodethorn_la_CXXFLAGS = -fopenmp -Wall -O3

# codethorn header files are installed in <installpath>/rose/codethorn
codethornincludedir = $(pkgincludedir)/codethorn
codethorninclude_HEADERS=\
  $(CODETHORN_SOLO_HEADER_FILES) \
  $(CODETHORN_HEADER_FILES) \
  CodeThornLib.h

# codethorn (core) library sources
libcodethorn_la_SOURCES=\
  $(CODETHORN_SOURCE_FILES) \
  CodeThornLib.C

################################################################
# LIBRARY: LTLTHORN/ (for linear temporal logic verification)
################################################################

libltlthorn_la_LDFLAGS=
libltlthorn_la_CXXFLAGS = -fopenmp -Wall -O3

LTLTHORN_SOLO_HEADER_FILES=
LTLTHORN_HEADER_FILES=\
  CounterexampleAnalyzer.h \
  DataRaceDetection.h \
  IOAnalyzer.h \
  IOSolver.h \
  LtsminConnection.h \
  ParProAutomataGenerator.h \
  ParProAutomata.h \
  ParProAnalyzer.h \
  ParProEState.h \
  ParProExplorer.h \
  ParProLtlMiner.h \
  ParProSpotState.h \
  ParProSpotSuccIter.h \
  ParProSpotTgba.h \
  ParProTransitionGraph.h \
  PromelaCodeGenerator.h \
  ReachabilityAnalyzerZ3.h \
  ReadWriteAnalyzer.h \
  RersCounterexample.h \
  Solver10.h \
  Solver11.h \
  Solver12.h \
  Solver8.h \
  SpotConnection.h \
  SpotMiscellaneous.h \
  SpotRenameVisitor.h \
  SpotState.h \
  SpotSuccIter.h \
  SpotTgba.h \
  SSAGenerator.h

LTLTHORN_SOURCE_FILES=\
  CounterexampleAnalyzer.C \
  DataRaceDetection.C \
  IOAnalyzer.C \
  IOSolver.C \
  LtsminConnection.C \
  ParProAutomataGenerator.C \
  ParProAutomata.C \
  ParProAnalyzer.C \
  ParProEState.C \
  ParProExplorer.C \
  ParProLtlMiner.C \
  ParProSpotState.C \
  ParProSpotSuccIter.C \
  ParProSpotTgba.C \
  ParProTransitionGraph.C \
  PromelaCodeGenerator.C \
  ReachabilityAnalyzerZ3.C \
  ReadWriteAnalyzer.C \
  RersCounterexample.C \
  Solver10.C \
  Solver11.C \
  Solver12.C \
  Solver8.C \
  SpotConnection.C \
  SpotMiscellaneous.C \
  SpotRenameVisitor.C \
  SpotState.C \
  SpotSuccIter.C \
  SpotTgba.C \
  SSAGenerator.C

# only informative (not installed)
libltlthorn_xHEADERS = \
  $(LTLTHORN_SOLO_HEADER_FILES) \
  $(LTLTHORN_HEADER_FILES) \
  LTLThornLib.h

# library sources
libltlthorn_la_SOURCES= \
  $(LTLTHORN_SOURCE_FILES) \
  LTLThornLib.C


##############################################################
# TOOL: CODETHORN
##############################################################

# yacc flags for LTL parser
codethorn_YFLAGS = -p ltl_ 

# this line creates speparately named object files
codethorn_CXXFLAGS = -fopenmp -Wall -O3 

# codethorn has most of its code organized in two separate libraries
codethorn_DEPENDENCIES=libcodethorn.la libltlthorn.la

if ROSE_WITH_SPOT
#spot and bdd need to be linked last with gcc 5.4
codethorn_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn -lltlthorn $(Z3_LINK) -lspot -lbdd 
codethorn_LDFLAGS = -L$(SPOT_LIBRARY_PATH) $(Z3_LIB_INCLUDES)
else
codethorn_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn -lltlthorn
endif

codethorndir=.

# only informative
codethorn_xHEADERS = \
  CommandLineOptions.h \
  CodeThornCommandLineOptions.h \
  codethorn.h \
  LTL.h

# codethorn sources in addition to libcodethorn and libltlthorn
codethorn_SOURCES = \
  LTLParser.y++ \
  CodeThornCommandLineOptions.C \
  codethorn.C

##############################################################
# TOOL: LTLTHORN
##############################################################

# TODO


##############################################################
# TOOL: WOODPECKER
##############################################################

woodpecker_DEPENDENCIES=libcodethorn.la
if ROSE_WITH_SPOT
woodpecker_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
woodpecker_LDFLAGS = -L$(SPOT_LIBRARY_PATH)
else
woodpecker_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
endif
woodpecker_CXXFLAGS= -fopenmp -Wall -O3
woodpecker_SOURCES = \
  ProgramStats.h                 \
  ProgramStats.C                 \
  Threadification.h \
  Threadification.C \
  TrivialInlining.C                \
  TrivialInlining.h                \
  DeadCodeElimination.C                \
  DeadCodeElimination.h                \
  ConversionFunctionsGenerator.h \
  ConversionFunctionsGenerator.C \
  woodpecker.C


##############################################################
# TOOL: ANALYTERIX
##############################################################

analyterix_DEPENDENCIES=libcodethorn.la
if ROSE_WITH_SPOT
analyterix_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
analyterix_LDFLAGS = -L$(SPOT_LIBRARY_PATH)
else
analyterix_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
endif
analyterix_CXXFLAGS = -fopenmp -Wall -O3
analyterix_SOURCES = \
  DeadCodeAnalysis.C \
  DeadCodeAnalysis.h \
  DFAstAttributeConversion.h DFAstAttributeConversion.C \
  FIPointerAnalysis.C \
  FIPointerAnalysis.h \
  ProgramStats.C \
  ProgramStats.h \
  RoseRDAnalysis.C \
  RoseRDAnalysis.h \
  TransformationOperators.h \
  TransformationOperators.C \
  analyterix.C

roseonlytools: woodpecker analyterix

check-roseonlytools: check-flow-insenstive


##############################################################
# TOOL: ADDRESSTAKENANALYSIS (for testing only)
##############################################################

if ROSE_WITH_SPOT
addressTakenAnalysis_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn -lboost_program_options
addressTakenAnalysis_LDFLAGS = -L$(SPOT_LIBRARY_PATH)
else
addressTakenAnalysis_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)  -lcodethorn
endif
addressTakenAnalysis_CXXFLAGS = -fopenmp -Wall -O3
addressTakenAnalysis_DEPENDENCIES=libcodethorn.la

REGRESSION_DATA_DIR=regressiondata

addressTakenAnalysis_SOURCES = \
  addressTakenAnalysisDriver.C

.PHONY: codethorn-dist viz bsps test checkdemos

# obsolete since integration into ROSE
# explicit rules because of multiple bison parser
#matcherparser.C: $(srcdir)/matcherlexer.ll $(srcdir)/matcherparser.yy
#	$(YACC) $(YFLAGS) -d -p matcherparser $(srcdir)/matcherparser.yy # generates y.tab.h/c
#	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pmatcherparser $(srcdir)/matcherlexer.ll
#	mv y.tab.h matcherparser.h
#	cat lex.yy.c y.tab.c > matcherparser.C
#	rm lex.yy.c y.tab.c

# MS: matcher_demo
#matcher_demo_DEPENDENCIES=libcodethorn.la
#matcher_demo_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn
#matcher_demo__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
#matcher_demo_SOURCES = matcher_demo.C
#BUILT_SOURCES =
#AM_YFLAGS =
#AM_LFLAGS =

#matcher_DEPENDENCIES=libcodethorn.la
#matcher_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lcodethorn
#matcher__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
#matcher_SOURCES = matcher.C

cldemo_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
cldemo__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
cldemo_SOURCES = cldemo.C

#MS: iterator_test not ingegrated yet
#iterator_test_SOURCES = iterator_test.C ShowSeq.h
#iterator_test_LDADD = -lrose libmatcher.la

#astinfo_DEPENDENCIES=
#astinfo_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#astinfo_SOURCES = astinfo.C LineColInfo.C

#MS: ast_demo not integrated yet
#ast_demo_SOURCES = ast_demo.C TimeMeasurement.C TimeMeasurement.h  RoseAst.C RoseAst.h
#ast_demo_LFLAGS = -Pmatcher
#ast_demo_YFLAGS = -d -p matcher

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
	@./codethorn $(srcdir)/tests/svcomp/svcomp-test1.c
	@./codethorn $(srcdir)/tests/svcomp/svcomp-test2.c
	@./codethorn $(srcdir)/tests/svcomp/svcomp-test3.c
	@./codethorn $(srcdir)/tests/svcomp/svcomp-test4.c
	@./codethorn $(srcdir)/tests/svcomp/svcomp-test5.c
	@./codethorn $(srcdir)/tests/svcomp/svcomp-test6.c
	@./codethorn $(srcdir)/tests/svcomp/svcomp-test7.c

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
	@echo
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
	$(srcdir)/scripts/runParCfgTests $(srcdir)/tests/ompcfg

# outdated tests (but still passing)
check-ltl-old: check-ltl-rers-different-versions

# failing tests
#check-failing: 

# failing tests due to changes in domain
#check-ltl-failing: check-ltl-rers-array

# current tests all passing
check-ltl: check-ltl-start check-ltl-rers-timeout check-ltl-rers-arithmetic check-ltl-rers-loop-aware-sync check-ltl-rers-topify check-ltl-cegpra check-ltl-rers-array

check-ltl-start:
	@echo ================================================================
	@echo RUNNING LTL VERIFICATION TESTS
	@echo ================================================================
	@rm -f *.consistent

#run RERS Problem1 from 2012 in several versions and compare to previous results
check-ltl-rers-different-versions:
	@$(srcdir)/scripts/runRersTestsDifferentVersion $(srcdir) $(top_builddir)/projects/CodeThorn/src

#test the parallel analysis of RERS Problem1402 (4 threads, problem features arithmetic)
check-ltl-rers-arithmetic:
	@$(srcdir)/scripts/runRersArithmeticProblemTest $(srcdir) $(top_builddir)/projects/CodeThorn/src 

#test the analysis of RERS Problem1403 (which features arrays)
check-ltl-rers-array:
	@$(srcdir)/scripts/runRersArrayProblemTest $(srcdir) $(top_builddir)/projects/CodeThorn/src

#test the analysis of RERS Problem1403, abstracting from global variables after 10,000 transitions
check-ltl-rers-topify:
	@$(srcdir)/scripts/runRersTopifyTest $(srcdir) $(top_builddir)/projects/CodeThorn/src

# SKIPPED
#test the analysis of RERS Problem1603 while using up to 1GB of RAM up to and including the STG computation (4 threads)
check-ltl-rers-memory-bound:
	@$(srcdir)/scripts/runRersMemoryBoundTest $(srcdir) $(top_builddir)/projects/CodeThorn/src

#test the analysis of RERS Problem1402 and terminate after 5 seconds (4 threads)
check-ltl-rers-timeout:
	@$(srcdir)/scripts/runRersTimeoutTest $(srcdir) $(top_builddir)/projects/CodeThorn/src

#one test for the loop-aware-sync analysis (solver 12, ISoLA'16 paper)
check-ltl-rers-loop-aware-sync:
	@$(srcdir)/scripts/runRersLoopAwareSyncTest $(srcdir) $(top_builddir)/projects/CodeThorn/src

#run CEGPRA tests
check-ltl-cegpra:
	@$(srcdir)/scripts/run_tests_cegpra $(srcdir) $(top_builddir)/projects/CodeThorn/src

# not used
check-arrays:
	./codethorn $(srcdir)/tests/pointer/test48.cpp --explicit-arrays=yes --viz=yes && dot -Tpdf transitiongraph1.dot -otransitiongraph1.pdf

# not used
check-array-bounds: ./codethorn
	./codethorn --explicit-arrays=yes $(srcdir)/tests/arraybounds1.C

# seg-faults with --reset-analyzer=no. Works with --reset-analyzer=yes (see below)
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
	@./codethorn --edg:no_warnings --normalize-all=off $(srcdir)/tests/jacobi-1d-imper_mod.c --dump-non-sorted=tmp.nsdump --rule-commutative-sort=no --abstraction-mode=0
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

# MS: 1-6 is reasonable for V1.2
RERS=$(patsubst %,Problem%.log, $(shell seq 6))
rers: $(RERS)

docs:
	cd "$(srcdir)" && doxygen

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
	rm -f LTLParser.c++
	rm -f *.cfg.dot # runParCfgTests
	rm -f a.out
	rm -rf ../docs/doxygen
	rm -f *.imout
	rm -f *.exe
	rm -f *.imout.check
	rm -Rf make_check_log_*

distclean-local: clean
	rm -f *.tgz
	rm -f *~
	rm -f codethornref

codethorn-dist:
	tar cvzf codethorn_$(MYDATE).tgz *.C *cpp *.h *.lxx *.yxx tests/*.C tests/*.c Makefile*

demo:
	$(srcdir)/demodir/rundemo.sh $(srcdir)/demodir $(top_builddir)/projects/CodeThorn
