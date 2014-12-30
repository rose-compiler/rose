include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_PROGRAMS = multipleIndividualPragmaCheck findOnePragma smtSolvePragmaPath pragmaToPragma checkPragmaParse

INCLUDES = $(ROSE_INCLUDES)
TEST_TARGETS =
EXTRA_DIST =
SMTSRC = $(srcdir)/src
SMTCOMPILE = $(srcdir)/compile
SMTUTIL = $(srcdir)/utils
SMTTESTS = $(srcdir)/tests
SMTTUTORIAL = $(srcdir)/tutorial
SMTDOCS = $(srcdir)/docs

pragmaToPragma_SOURCES = $(SMTUTIL)/pragmaToPragma.cpp
pragmaToPragma_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


smtSolvePragmaPath_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTCOMPILE)/smtSolvePragmaPath.cpp $(SMTSRC)/smtQueryLib.cpp $(SMTSRC)/smtPragmaPathLib.cpp

smtSolvePragmaPath_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

checkPragmaParse_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTUTIL)/checkPragmaParse.cpp

checkPragmaParse_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


multipleIndividualPragmaCheck_SOURCES = $(SMTTUTORIAL)/multipleIndividualPragmaCheck.cpp $(SMTUTIL)/pragmaParse.cpp $(SMTUTIL)/SMTPragmaBackend.cpp $(SMTSRC)/smtQueryLib.cpp

multipleIndividualPragmaCheck_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


findOnePragma_SOURCES = $(SMTUTIL)/pragmaParse.cpp $(SMTTUTORIAL)/findOnePragma.cpp $(SMTUTIL)/SMTPragmaBackend.cpp $(SMTSRC)/smtQueryLib.cpp
findOnePragma_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

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



#if z3 runs through a path that is not satisfiable, get-model will cause an error
#this is correct, but the error will cause test failure even though its result is correct


C_IF_TESTS=\
	$(C_IF_TESTS_PREFIX)/basicIfTests.pragma.c \
	$(C_IF_TESTS_PREFIX)/compound_conditions2.pragma.c \
	$(C_IF_TESTS_PREFIX)/simpleif3.pragma.c \
	$(C_IF_TESTS_PREFIX)/compound_conditionals.c 

C_REQ_TESTS+=$(C_IF_TESTS)

smtSolvePragmaPath_SPECIMENS = $(C_REQ_TESTS)
smtSolvePragmaPath_TEST_TARGETS = $(smtSolvePragmaPath_SPECIMENS)
$(smtSolvePragmaPath_TEST_TARGETS): smtSolvePragmaPath
	./smtSolvePragmaPath $@;	
TEST_TARGETS+=$(smtSolvePragmaPath_TEST_TARGETS)

.PHONY: check-smtSolvePragmaPath
check-smtSolvePragmaPath: $(smtSolvePragmaPath_TEST_TARGETS)
check-local: $(TEST_TARGETS)
	rm -f *.dot
clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f *.dot
	rm -f *.o
	rm -f *.smt2
