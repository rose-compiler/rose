## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODES = \
12.3.2c.cc\
13.1a.cc\
13.1d.cc\
3.4.3a.cc\
7.3.1.2b.cc\
7.3.3d.cc\
7.3.4b.cc

# DQ (6/22/2011): Failing test copied to Cxx_tests (for further testing).
# 7.3.3a.cc

# DQ (12/5/2006): Many of these tests are expected failures 
# (e.g. 13.1g.cc, but it only fails for g++ 3.4.x, and passes for g++ 3.3.x)
START_OF_FAILED_TESTS_USING_ROSE = \
7.3.4e.cc\
8.2.7b.cc\
12.6.2.2a.cc\
13.1b.cc\
13.1e.cc\
13.1g.cc\
13.2a.cc\
7.3.3b.cc\
7.3.3k.cc\
8.2.1.cc\
8.3.3.2.cc\
12.6.2.2b.cc\
13.1c.cc\
13.1f.cc\
13.3.1.1.2.cc\
7.1.3b.cc\
7.3.2.cc\
7.3.3c.cc\
7.3.3f.cc\
7.3.4d.cc\
8.2.7a.cc

# Build the list of object files
TEST_Objects = ${TESTCODES:.cc=.o}

ROSE_FLAGS = --edg:no_warnings -w

TRANSLATOR_EXECUTABLE = $(top_builddir)/tests/testTranslator 
TEST_TRANSLATOR = $(TRANSLATOR_EXECUTABLE) $(ROSE_FLAGS)

$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) $(INCLUDES) -c $(srcdir)/$(@:.o=.cc)

EXTRA_DIST = $(TESTCODES)

clean-local:
	rm -f C/*.o *.o C/rose_*.c rose_*.C rose_*.c rose_*.cc *.sage.C test2001_??.C.show *.dot *.ps \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C core test2005_56
	rm -rf ti_files *.ti *.et

check-local: $(TRANSLATOR_EXECUTABLE) 
	@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C++Code/ElsaTestCases/std: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"



