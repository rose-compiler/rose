include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


noinst_PROGRAMS =
INCLUDES = $(ROSE_INCLUDES)
TEST_CONFIG = $(top_srcdir)/scripts/test_exit_status
TEST_TARGETS =
EXTRA_DIST = compile/smtSolvePragmaPath.h src/smtPragmaPathLib.h src/smtQueryLib.h tools/collectPaths.h tools/pointerPredicateTranslationRules.cpp tutorial/findOnePragma.h tutorial/multipleIndividualPragmaCheck.h utils/SMTPragmaBackend.h utils/extractPragmaInfo.h utils/handleVariablesForSMTLib.h utils/pragmaParse.h utils/utilHeader.h
SMTSRC = $(srcdir)/src
SMTCOMPILE = $(srcdir)/compile
SMTUTIL = $(srcdir)/utils
SMTTESTS = $(srcdir)/tests
SMTTOOL = $(srcdir)/tools
SMTTUTORIAL = $(srcdir)/tutorial
SMTDOCS = $(srcdir)/docs

SMTANDVARCODE = $(srcdir)/utils/handleVariablesForSMTLib.cpp $(srcdir)/utils/getStatementInfoForSMTLib.cpp


noinst_PROGRAMS += smtSolvePragmaPath
smtSolvePragmaPath_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTCOMPILE)/smtSolvePragmaPath.cpp $(SMTSRC)/smtQueryLib.cpp $(SMTSRC)/smtPragmaPathLib.cpp

smtSolvePragmaPath_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)



noinst_PROGRAMS+=multipleIndividualPragmaCheck
multipleIndividualPragmaCheck_SOURCES = $(SMTTUTORIAL)/multipleIndividualPragmaCheck.cpp $(SMTUTIL)/pragmaParse.cpp $(SMTUTIL)/SMTPragmaBackend.cpp $(SMTSRC)/smtQueryLib.cpp

multipleIndividualPragmaCheck_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS+=findOnePragma
findOnePragma_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTTUTORIAL)/findOnePragma.cpp $(SMTUTIL)/SMTPragmaBackend.cpp $(SMTSRC)/smtQueryLib.cpp
findOnePragma_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)



C_PASSING_TESTS=
C_FAILING_TESTS=
C_ARRAY_TESTS_PREFIX=$(srcdir)/tests/arrays

C_ARRAY_PASSING_TESTS=\
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess1.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess1.test.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess2.test.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess3.test.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess4.test.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess5.test.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess5.unknowns.test.pragma.c
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess6.test.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess7.test.pragma.c \
        $(C_ARRAY_TESTS_PREFIX)/passing/arrAccess7.unknowns.test.pragma.c

C_ARRAY_FAILING_TESTS=


C_PASSING_TESTS+=$(C_ARRAY_PASSING_TESTS)
C_FAILING_TESTS+=$(C_ARRAY_FAILING_TESTS)



C_IF_TESTS_PREFIX=$(srcdir)/tests/if_statements

C_IF_PASSING_TESTS=\
        $(C_IF_TESTS_PREFIX)/passing/basicIfTests.pragma.c \
        $(C_IF_TESTS_PREFIX)/passing/compound_conditions2.pragma.c \
        $(C_IF_TESTS_PREFIX)/passing/simpleif3.pragma.c \
        $(C_IF_TESTS_PREFIX)/passing/compound_conditionals.c \
        $(C_IF_TESTS_PREFIX)/passing/elseif1.c\
        $(C_IF_TESTS_PREFIX)/passing/justIf.c
C_IF_FAILING_TESTS=

C_PASSING_TESTS+=$(C_IF_PASSING_TESTS)
C_FAILING_TESTS+=$(C_IF_FAILING_TESTS)

C_COMPOUND_ASSIGN_TESTS_PREFIX=$(srcdir)/tests/compoundAssignTests

C_COMPOUND_ASSIGN_PASSING_TESTS=\
	$(C_COMPOUND_ASSIGN_TESTS)/passing/c_assign_test1.c\
	$(C_COMPOUND_ASSIGN_TESTS)/passing/c_assign_test2.c\
	$(C_COMPOUND_ASSIGN_TESTS)/passing/c_assign_test3.c

C_COMPOUND_ASSIGN_FAILING_TESTS=

C_PASSING_TESTS+=$(C_COMPOUND_ASSIGN_PASSING_TESTS)
C_FAILING_TESTS+=$(C_COMPOUND_ASSIGN_FAILING_TESTS)

C_COMPOUND_CONDITIONALS_TESTS_PREFIX=$(srcdir)/tests/compoundConditionals

C_COMPOUND_CONDITIONALS_PASSING_TESTS=$(C_COMPOUND_CONDITIONALS_TESTS_PREFIX)/passing/t1.c
C_COMPOUND_CONDITIONALS_FAILING_TESTS=

C_PASSING_TESTS+=$(C_COMPOUND_CONDITIONALS_PASSING_TESTS)
C_FAILING_TESTS+=$(C_COMPOUND_CONDITIONALS_FAILING_TESTS)

C_ALGORITHMS_TESTS_PREFIX=$(srcdir)/tests/algorithms
C_ALGORITHMS_PASSING_TESTS= \
C_ALGORITHMS_FAILING_TESTS=$(C_ALGORITHMS_TESTS_PREFIX)/failing/bisection.c

C_PASSING_TESTS+=$(C_ALGORITHMS_PASSING_TESTS)
C_FAILING_TESTS+=$(C_ALGORITHMS_FAILING_TESTS)

C_FINAL_VALUES_TESTS_PREFIX=$(srcdir)/tests/final_values_tests
C_FINAL_VALUES_PASSING_TESTS=
C_FINAL_VALUES_FAILING_TESTS=$(C_FINAL_VALUES_TESTS_PREFIX)/failing/finalCheck1.c
C_PASSING_TESTS+=$(C_FINAL_VALUES_PASSING_TESTS)
C_FAILING_TESTS+=$(C_FINAL_VALUES_FAILING_TESTS)

C_INTERPROCEDURAL_TESTS_PREFIX=$(srcdir)/tests/interprocedural_tests
C_INTERPROCEDURAL_PASSING_TESTS=
C_INTERPROCEDURAL_FAILING_TESTS=$(C_INTERPROCEDURAL_TESTS_PREFIX)/failing/tf1.c

C_PASSING_TESTS+=$(C_INTERPROCEDURAL_PASSING_TESTS)
C_FAILING_TESTS+=$(C_INTERPROCEDURAL_FAILING_TESTS)
C_LOOP_TESTS_PREFIX=$(srcdir)/tests/loop_tests
C_LOOP_PASSING_TESTS= \
	$(C_LOOP_TESTS_PREFIX)/passing/while1.c\
	$(C_LOOP_TESTS_PREFIX)/passing/whileTest4.c

C_LOOP_FAILING_TESTS=\
	$(C_LOOP_TESTS_PREFIX)/failing/mWhiles1.c\
	$(C_LOOP_TESTS_PREFIX)/failing/whileTest1.c\
	$(C_LOOP_TESTS_PREFIX)/failing/whileTest2.c\
	$(C_LOOP_TESTS_PREFIX)/failing/whileTest3.c

C_PASSING_TESTS+=$(C_LOOP_PASSING_TESTS)
C_FAILING_TESTS+=$(C_LOOP_FAILING_TESTS)

C_NUMERICAL_ANALYSIS_PREFIX=$(srcdir)/tests/numerical_analysis
C_NUMERICAL_ANALYSIS_PASSING_TESTS=
C_NUMERICAL_ANALYSIS_FAILING_TESTS=\
	$(C_NUMERICAL_ANALYSIS_PREFIX)/failing/fixedpoint.c \
	$(C_NUMERICAL_ANALYSIS_PREFIX)/failing/MandelbrotExampleNormalized.c
C_PASSING_TESTS+=$(C_NUMERICAL_ANALYSIS_PASSING_TESTS)
C_FAILING_TESTS+=$(C_NUMERICAL_ANALYSIS_FAILING_TESTS)

C_NUMERIC_OPERATORS_PREFIX=$(srcdir)/tests/numeric_operators
C_NUMERIC_OPERATORS_TESTS=$(C_NUMERIC_OPERATORS_PREFIX)/passing/numericalComparisonBinaryOps.pragma.c
C_NUMERIC_OPERATORS_TESTS_FAILING=


C_PASSING_TESTS+=$(C_IF_PASSING_TESTS)

C_RESULT_QUERY_TESTS_PREFIX=$(srcdir)/tests/ResultQueryTests

C_RESULT_QUERY_PASSING_TESTS=\
	$(C_RESULT_QUERY_TESTS_PREFIX)/passing/test2.c\
	$(C_RESULT_QUERY_TESTS_PREFIX)/passing/test3.c\
	$(C_RESULT_QUERY_TESTS_PREFIX)/passing/test4.c

C_RESULT_QUERY_FAILING_TESTS=$(C_RESULT_QUERY_TESTS_PREFIX)/failing/testWithConditionalPragma.c

C_PASSING_TESTS+=$(C_RESULT_QUERY_PASSING_TESTS)
C_FAILING_TESTS+=$(C_RESULT_QUERY_FAILING_TESTS)

C_OPERATOR_CHECK_PREFIX=$(srcdir)/tests/operator_check
C_OPERATOR_CHECK_PASSING_TESTS=
C_OPERATOR_CHECK_FAILING_TESTS=\
	$(C_OPERATOR_CHECK_TESTS_PREFIX)/failing/test1.c\
	$(C_OPERATOR_CHECK_TESTS_PREFIX)/failing/test2.c\
	$(C_OPERATOR_CHECK_TESTS_PREFIX)/failing/test3.c\
	$(C_OPERATOR_CHECK_TESTS_PREFIX)/failing/test4.c

C_PASSING_TESTS+=$(C_OPERATOR_CHECK_PASSING_TESTS)
C_FAILING_TESTS+=$(C_OPERATOR_CHECK_FAILING_TESTS)

C_RECURSIVE_FUNCTIONS_PREFIX=$(srcdir)/tests/recursive_functions
C_RECURSIVE_FUNCTIONS_PASSING_TESTS=
C_RECURSIVE_FUNCTIONS_FAILING_TESTS=$(C_RECURSIVE_FUNCTIONS_PREFIX)/failing/sum.c

C_PASSING_TESTS+=$(C_RECURSIVE_FUNCTIONS_PASSING_TESTS)
C_FAILING_TESTS+=$(C_RECURSIVE_FUNCTIONS_FAILING_TESTS)


C_TYPES_PREFIX=$(srcdir)/tests/types
C_TYPES_PASSING_TESTS=
C_TYPES_FAILING_TESTS=$(C_TYPES_PREFIX)/failing/type_test.c

C_PASSING_TESTS+=$(C_TYPES_PASSING_TESTS)
C_FAILING_TESTS+=$(C_TYPES_FAILING_TESTS)

C_UNARY_OPS_PREFIX=$(srcdir)/tests/unaryOpsTests
C_UNARY_OPS_PASSING_TESTS=\
	$(C_UNARY_OPS_PREFIX)/passing/MinusMinusPrefix.c\
	$(C_UNARY_OPS_PREFIX)/passing/PlusPlusPrefix.c

C_UNARY_OPS_FAILING_TESTS=\
	$(C_UNARY_OPS_PREFIX)/failing/MinusMinusPostfix.c\
	$(C_UNARY_OPS_PREFIX)/failing/PlusPlusPostfix.c

C_PASSING_TESTS+=$(C_UNARY_OPS_PASSING_TESTS)
C_FAILING_TESTS+=$(C_UNARY_OPS_FAILING_TESTS)


#Z3_RESULT_QUERY_TESTS=\
#	$(C_RESULT_QUERY_TESTS_PREFIX)/test2.c.RQ.smt2\
#	$(C_RESULT_QUERY_TESTS_PREFIX)/test3.c.RQ.smt2\
#	$(C_RESULT_QUERY_TESTS_PREFIX)/test4.c.RQ.smt2

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

analyzeBasic_TEST_TARGETS=$(C_PASSING_TESTS)

$(analyzeBasic_TEST_TARGETS): ap_%.passed: % $(TEST_CONFIG)
	@$(RTH_RUN) CMD="./analyzePath -edg:w -c $<" $(TEST_CONFIG) $@
	head --lines=-3 $@ > $@.smt2
.PHONY: check-analyzePath check-analyzeBasic
check-analyzePath: $(analyzePath_TEST_TARGETS)
check-analyzeBasic: $(analyzeBasic_TEST_TARGETS)

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



	


#smtSolvePragmaPath_SPECIMENS = $(C_PASSING_TESTS)
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
