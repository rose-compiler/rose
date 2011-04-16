include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = hcpragma
hcpragma_SOURCES = hcpragma.C hcpragma.h AstFromString.cpp AstFromString.h

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = parser_input.c

TESTCODE_CURRENTLY_FAILING = 

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave
# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c hcpragma
	./hcpragma $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

check-local:
	@echo "Test for AstFromString using a program parsing pragmas."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/pragmaParsing: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) README parser_input.c

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest


