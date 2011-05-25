include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS = hcpragma testParserBuilder buildFromString

hcpragma_SOURCES = hcpragma.C hcpragma.h

testParserBuilder_SOURCES = testParserBuilder.cpp

buildFromString_SOURCES = buildFromString.C

C_TESTCODES_REQUIRED_TO_PASS = parser_input.c

TESTCODE_CURRENTLY_FAILING = 

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave
# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c hcpragma
	./hcpragma $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

generatedCodeExamples = \
   rose_inputbuildFromString.C


TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3
rose_inputbuildFromString.C:buildFromString
	./buildFromString$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildFromString.C

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

check-local:
	@echo "Test for AstFromString using a program parsing pragmas."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(generatedCodeExamples) 
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/pragmaParsing: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) README \
        inputbuildFromString.C

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest
