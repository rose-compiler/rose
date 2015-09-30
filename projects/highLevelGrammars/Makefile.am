include ../../config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

include_HEADERS = nodeAndEdgeTypes.h AbstractionAttribute.h

bin_PROGRAMS = \
     overloadedOperatorRepresentation \
     highLevelGrammarGenerator \
     exampleTranslator

overloadedOperatorRepresentation_SOURCES = overloadedOperatorRepresentation.C AbstractionAttribute.C

highLevelGrammarGenerator_SOURCES = highLevelGrammarGenerator.C

exampleTranslator_SOURCES = \
     exampleTranslator.C

#    AplusplusAbstractionGeneratedSource.h
#    AplusplusAbstractionGeneratedSource.C

#     AplusplusAbstractionGeneratedSource.h
#     AplusplusAbstractionGeneratedSource.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = $(TESTCODES) simpleA++.h 
CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf
	rm -rf AplusplusAbstractionGeneratedSource.h AplusplusAbstractionGeneratedSource.C

check-local:
	rm -f testCode.o
	$(MAKE) $(TEST_Objects)

TESTCODES = testCode.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS =

APP_INCLUDES = $(top_srcdir)/tests/CompileTests/A++Code

# This rule generates a dependency that forces the "make check" 
# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:

$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN highLevelGrammarGenerator
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./highLevelGrammarGenerator -I$(srcdir) $(ROSE_FLAGS) $(AM_CPPFLAGS) $(NANOS_INCLUDES) -c $(srcdir)/$(@:.o=.C)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)

arrayGrammar_enum.h: parallelSTLQuery $(srcdir)/parallelSTLQuery_testFile.C
	cp $(srcdir)/parallelSTLQuery_testFile.C parallelSTLQuery_testFile.C
	$(VALGRIND) ./highLevelGrammarGenerator -c parallelSTLQuery_testFile.C
	rm -rf rose_parallelSTLQuery_testFile.C


GENERATED_SOURCE = \
   AplusplusAbstractionGeneratedSource.h

# AplusplusAbstractionGeneratedSource.C

GENERATED_SOURCE_DEP = \
   highLevelGrammarGenerator \
   $(GENERATED_SOURCE)

$(GENERATED_SOURCE): highLevelGrammarGenerator $(srcdir)/testCode.C 
	$(VALGRIND) ./highLevelGrammarGenerator -c $(srcdir)/testCode.C

testApp: highLevelGrammarGenerator $(srcdir)/testApp.C FORCE_TEST_CODES_TO_RUN
	$(VALGRIND) ./highLevelGrammarGenerator -I$(APP_INCLUDES) -c $(srcdir)/testApp.C

test: testApp $(GENERATED_SOURCE_DEP) exampleTranslator
	$(VALGRIND) ./exampleTranslator -c $(srcdir)/testCode.C
	rm -rf testCode.o

overtest: overloadedOperatorRepresentation
	$(VALGRIND) ./overloadedOperatorRepresentation -rose:verbose -I$(srcdir) -c $(srcdir)/overloadedOperatorTestCode.C
	rm -rf testCode.o

# Provide a simple interface to the Makefile which outputs the relavant make-rules
rules:
	@echo "Possible Rules for this Makefile:"
	@echo "     testCode.o"
	@echo "     test"











