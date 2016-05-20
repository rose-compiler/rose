## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

C_TESTCODES = \
t0001.c

START_OF_FAILED_TESTS_USING_ROSE = \
t0002.c\
t0003.c\
t0004.c

# DQ (11/11/2012): Added support to build testTranslator if it has not been built yet.
$(TRANSLATOR_EXECUTABLE):
	cd $(top_builddir)/tests/; $(MAKE) testTranslator

# Build the list of object files
TEST_Objects = ${C_TESTCODES:.c=.o}

ROSE_FLAGS = -rose:C --edg:no_warnings -w

TRANSLATOR_EXECUTABLE = $(top_builddir)/tests/testTranslator 
TEST_TRANSLATOR = $(TRANSLATOR_EXECUTABLE) $(ROSE_FLAGS)

$(TEST_Objects): $(C_TESTCODES)
	$(TEST_TRANSLATOR) $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.c)

EXTRA_DIST = $(C_TESTCODES)

clean-local:
	rm -f C/*.o *.o C/rose_*.c rose_*.C rose_*.c rose_*.cc *.sage.C test2001_??.C.show *.dot *.ps \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C core test2005_56
	rm -rf ti_files *.ti *.et

check-local: $(TRANSLATOR_EXECUTABLE) 
	@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C++Code/ElsaTestCases/kandr: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"



