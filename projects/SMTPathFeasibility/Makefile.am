include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


noinst_PROGRAMS =
INCLUDES = $(ROSE_INCLUDES)
TEST_CONFIG = $(top_srcdir)/scripts/test_exit_status
TEST_TARGETS =
EXTRA_DIST =
SMTSRC = $(srcdir)/src
SMTCOMPILE = $(srcdir)/compile
SMTUTIL = $(srcdir)/utils
SMTTESTS = $(srcdir)/tests
SMTTOOL = $(srcdir)/tools
SMTTUTORIAL = $(srcdir)/tutorial
SMTDOCS = $(srcdir)/docs

SMTANDVARCODE = $(srcdir)/utils/handleVariablesForSMTLib.cpp $(srcdir)/utils/getStatementInfoForSMTLib.cpp


#testCollectPaths_SOURCES = $(SMTTOOLS)/testCollectPaths.cpp $(SMTTOOLS)/collectPaths.cpp $(SMTANDVARCODE)
#testCollectPaths_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#testSMTTranslatePathCollector_SOURCES = $(SMTTOOLS)/testSMTTranslatePathCollector.cpp $(SMTTOOLS)/SMTTranslatePathCollector.cpp $(SMTTOOLS)/collectPaths.cpp $(SMTANDVARCODE)
#testSMTTranslatePathCollector_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#writePath_SOURCES = $(SMTUTIL)/writePath.cpp $(SMTTOOLS)/collectPaths.cpp $(SMTANDVARCODE)
#writePath_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#testDriver_SOURCES = $(SMTTESTS)/s_exp_tests/testDriver.cpp $(SMTANDVARCODE)
#testDriver_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
#pathCollection_SOURCES = $(SMTUTIL)/collectPaths.cpp  $(SMTUTIL)/collectPathsEx.cpp
#pathCollection_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#GetRuleQuery_SOURCES = $(SMTUTIL)/evalForRuleQuery.cpp $(SMTUTIL)/collectStatementsLib.cpp $(SMTUTIL)/getDependencyInfo.cpp $(SMTCOMPILE)/GetRuleQuery.cpp
#GetRuleQuery_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#writeStatements_SOURCES = $(SMTANDVARCODE) $(SMTUTIL)/writeStatements.cpp
#writeStatements_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#scopeInfoTest_SOURCES = $(SMTUTIL)/ruleQueryLib.cpp $(SMTTESTS)/utility_tests/scopeInfoTest.cpp
#scopeInfoTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)



#ASTSynth_SOURCES = $(SMTUTIL)/ASTSynth.cpp $(SMTANDVARCODE)
#ASTSynth_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#translateASTZ3_SOURCES = $(SMTUTIL)/translateFromASTLib.cpp $(SMTANDVARCODE) $(SMTUTIL)/translateFromAST.cpp
#translateASTZ3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#testVarFinalState_SOURCES = $(SMTANDVARCODE) $(SMTTESTS)/areUtilitiesFunctional/testVariableFinalStateCalc.cpp

#testVarFinalState_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#testDefUseUtility_SOURCES = $(SMTANDVARCODE) $(SMTTESTS)/areUtilitiesFunctional/testDefUse.cpp
#testDefUseUtility_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#newDFS_SOURCES = $(SMTUTIL)/newDFSSgGraph.cpp $(SMTUTIL)/newDFSLib.cpp
#newDFS_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#newDFSPointToPoint_SOURCES = $(SMTUTIL)/newDFSSgGraphPointToPoint.cpp $(SMTUTIL)/newDFSLib.cpp
#newDFSPointToPoint_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#changeToWhiles_SOURCES = $(SMTUTIL)/changeToWhiles.cpp
#changeToWhiles_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#listStatements_SOURCES = $(SMTUTIL)/listStatements.cpp $(SMTUTIL)/extractPragmaInfo.cpp
#listStatements_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#listStatements_SOURCES =  $(SMTANDVARCODE) $(SMTUTIL)/extractPragmaInfo.cpp $(SMTCOMPILE)/listStatements.cpp

#listStatements_SOURCES = $(SMTUTIL)/listStatLib.cpp $(SMTUTIL)/getDependencyInfo.cpp $(SMTUTIL)/extractPragmaInfo.cpp $(SMTCOMPILE)/listStatements.cpp
#listStatements_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
#pragmaToPragma_SOURCES = $(SMTUTIL)/pragmaToPragma.cpp
#pragmaToPragma_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#writeVars_SOURCES = $(SMTUTIL)/writeVars.cpp
#writeVars_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#writeAnnotatedPreSMT_SOURCES = $(SMTUTIL)/writeAnnotatedPreSMT.cpp $(SMTSRC)/smtQueryLibUnbound.cpp
#writeAnnotatedPreSMT_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#writeVarsForStatements_SOURCES = $(SMTUTIL)/writeVarsForStatements.cpp $(SMTSRC)/smtQueryLib.cpp
#writeVarsForStatements_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#writeVarsForFunctions_SOURCES = $(SMTUTIL)/writeVarsForFunctions.cpp
#writeVarsForFunctions_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#extractPragmaInfo_SOURCES = $(SMTUTIL)/extractPragmaInfo.cpp
#extractPragmaInfo_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += smtSolvePragmaPath
smtSolvePragmaPath_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTCOMPILE)/smtSolvePragmaPath.cpp $(SMTSRC)/smtQueryLib.cpp $(SMTSRC)/smtPragmaPathLib.cpp

smtSolvePragmaPath_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#noinst_PROGRAMS+=checkPragmaParse
#checkPragmaParse_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTUTIL)/checkPragmaParse.cpp

#checkPragmaParse_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS+=multipleIndividualPragmaCheck
multipleIndividualPragmaCheck_SOURCES = $(SMTTUTORIAL)/multipleIndividualPragmaCheck.cpp $(SMTUTIL)/pragmaParse.cpp $(SMTUTIL)/SMTPragmaBackend.cpp $(SMTSRC)/smtQueryLib.cpp

multipleIndividualPragmaCheck_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS+=findOnePragma
findOnePragma_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTTUTORIAL)/findOnePragma.cpp $(SMTUTIL)/SMTPragmaBackend.cpp $(SMTSRC)/smtQueryLib.cpp
findOnePragma_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#INCLUDES = $(ROSE_INCLUDES)
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_REQ_TESTS=

C_ARRAY_TESTS_PREFIX=$(srcdir)/tests/arrays/passing
C_IF_TESTS_PREFIX=$(srcdir)/tests/if_statements/passing

C_ARRAY_TESTS=\
	$(C_ARRAY_TESTS_PREFIX)/arrAccess1.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess1.test.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess2.test.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess3.test.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess4.test.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess5.test.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess5.unknowns.test.pragma.c
	$(C_ARRAY_TESTS_PREFIX)/arrAccess6.test.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess7.test.pragma.c \
	$(C_ARRAY_TESTS_PREFIX)/arrAccess7.unknowns.test.pragma.c

C_REQ_TESTS+=$(C_ARRAY_TESTS)




C_IF_TESTS=\
	$(C_IF_TESTS_PREFIX)/basicIfTests.pragma.c \
	$(C_IF_TESTS_PREFIX)/compound_conditions2.pragma.c \
	$(C_IF_TESTS_PREFIX)/simpleif3.pragma.c \
	$(C_IF_TESTS_PREFIX)/compound_conditionals.c 

C_REQ_TESTS+=$(C_IF_TESTS)

C_RESULT_QUERY_TESTS_PREFIX=$(srcdir)/tests/ResultQueryTests

C_RESULT_QUERY_TESTS=\
	$(C_RESULT_QUERY_TESTS_PREFIX)/test2.c\
	$(C_RESULT_QUERY_TESTS_PREFIX)/test3.c\
	$(C_RESULT_QUERY_TESTS_PREFIX)/test4.c
Z3_RESULT_QUERY_TESTS=\
	$(C_RESULT_QUERY_TESTS_PREFIX)/test2.c.RQ.smt2\
	$(C_RESULT_QUERY_TESTS_PREFIX)/test3.c.RQ.smt2\
	$(C_RESULT_QUERY_TESTS_PREFIX)/test4.c.RQ.smt2

noinst_PROGRAMS += get_type
get_type_SOURCES = $(SMTUTIL)/get_type.cpp
get_type_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += translateArrayPtrsAndStructs
translateArrayPtrsAndStructs_SOURCES = $(SMTUTIL)/translateArrayPtrsAndStructs.cpp $(SMTANDVARCODE)
translateArrayPtrsAndStructs_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += get_struct
get_struct_SOURCES = $(SMTUTIL)/get_struct.cpp
get_struct_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += get_function
get_function_SOURCES = $(SMTUTIL)/get_function.cpp
get_function_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += replacePlusPlusMinusMinus
replacePlusPlusMinusMinus_SOURCES = $(SMTUTIL)/replacePlusPlusMinusMinus.cpp
replacePlusPlusMinusMinus_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += analyzePath
analyzePath_SOURCES = $(SMTTOOL)/analyzePath.cpp $(SMTTOOL)/analyzePathUtils.cpp $(SMTTOOL)/collectPaths.cpp $(SMTUTIL)/extractPragmaInfo.cpp $(SMTUTIL)/initializeExpressions.cpp $(SMTANDVARCODE)
analyzePath_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
analyzePath_SPECIMENS = test2.c test3.c test4.c
analyzePath_TEST_TARGETS = $(addprefix ap_, $(addsuffix .passed, $(analyzePath_SPECIMENS)))
$(analyzePath_TEST_TARGETS): ap_%.passed: % $(TEST_CONFIG) analyzePath
	@$(RTH_RUN) CMD="./analyzePath -edg:w -c $<" $(TEST_CONFIG) $@
	head --lines=-3 $@ > $@.smt2;
if USE_Z3
z3_SPECIMENS = ap_test2.c.passed.smt2 ap_test3.c.passed.smt2 ap_test4.c.passed.smt2
z3_TEST_TARGETS = $(addsuffix .result, $(z3_SPECIMENS))
$(z3_TEST_TARGETS): %.result: % $(TEST_CONFIG)
	@$(RTH_RUN) CMD="z3 -smt2 $<" $(TEST_CONFIG) $@
endif


.PHONY: check-analyzePath
check-analyzePath: $(analyzePath_TEST_TARGETS)

EXTRA_DIST+=$(analyzePath_SPECIMENS)
TEST_TARGETS+=$(analyzePath_TEST_TARGETS) 
if USE_Z3
EXTRA_DIST+=$(z3_SPECIMENS)
TEST_TARGETS+=$(z3_TEST_TARGETS)
endif

noinst_PROGRAMS += replaceExpressions
replaceExpressions_SOURCES = $(SMTUTIL)/replaceExpressions.cpp
replaceExpressions_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 


noinst_PROGRAMS += replaceExpressionsAndSimplifyExpressions
replaceExpressionsAndSimplifyExpressions_SOURCES = $(SMTUTIL)/replaceExpressionsAndSimplifyExpressions.cpp
replaceExpressionsAndSimplifyExpressions_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)




#analyzePath_SPECIMENS = $(C_RESULT_QUERY_TESTS)
#analyzePath_TEST_TARGETS=$(analyzePath_SPECIMENS)
#$(analyzePath_TEST_TARGETS): analyzePath
#	./analyzePath $@ > $@.RQ.smt2;
#
#TEST_TARGETS+=$(analyzePath_TEST_TARGETS)
#
#if USE_Z3
#z3_SPECIMENS=$(Z3_RESULT_QUERY_TESTS)
#z3_TEST_TARGETS=$(z3_SPECIMENS)
#$(z3_TEST_TARGETS): z3
#	z3 -smt2 $@ > $@.output;
#
#TEST_TARGETS+=$(z3_TEST_TARGETS)
#endif



	


#smtSolvePragmaPath_SPECIMENS = $(C_REQ_TESTS)
#smtSolvePragmaPath_TEST_TARGETS = $(smtSolvePragmaPath_SPECIMENS)
#$(smtSolvePragmaPath_TEST_TARGETS): smtSolvePragmaPath
#	./smtSolvePragmaPath $@ >> pragmaPathResults;
#	./listStatements $@;
#	./translateASTZ3 $@ >> translationResults;
#TEST_TARGETS+=$(smtSolvePragmaPath_TEST_TARGETS)

#.PHONY: check-smtSolvePragmaPath
#check-local: $(TEST_TARGETS)

if USE_Z3
check-local: $(TEST_TARGETS)
	echo "using z3!";
else
check-local: $(TEST_TARGETS)
	echo "not using z3!";
endif
clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f *.dot
	rm -f *.o
	rm -f *.smt2
	rm -f *.result
	rm -f rose_test*.c
	rm -f ap_test*.passed
