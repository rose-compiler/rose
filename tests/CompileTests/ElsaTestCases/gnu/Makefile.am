## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODES = \
d0085.cc

START_OF_FAILED_TESTS_USING_ROSE = \
d0094.cc\
g0003.cc\
g0008.cc\
g0013.cc\
g0018.cc\
g0028.cc\
g0033.cc\
k0005.cc\
t0127.cc\
t0132.cc\
d0078.cc\
d0086.cc\
g0004.cc\
g0014.cc\
g0029.cc\
k0001.cc\
t0124.cc\
d0081.cc\
d0089.cc\
d0092.cc\
d0093.cc\
g0001.cc\
g0002.cc\
g0007.cc\
g0010.cc\
g0011.cc\
g0012.cc\
g0017.cc\
g0022.cc\
g0030.cc\
g0032.cc\
t0129.cc\
t0131.cc\
k0003.cc\
k0004.cc\
bugs/gb0003.cc\
bugs/gb0006.cc\
bugs/gb0012.cc

C_TESTCODES = \
asm01.c

START_OF_C_TESTS_THAT_FAILED_TESTS_USING_ROSE = \
attr01.c\
attr02.c\
c0001.c\
c0002.c\
d0083.c\
dC0001.c\
dC0003.c\
dC0004.c\
dC0005.c\
dC0006.c\
dC0008.c\
dC0009.c\
dC0015.c\
dC0016.c\
dC0017.c\
d0095.c\
t0126.c\
bugs/gb0001.c\
bugs/gb0002.c\
bugs/gb0004.c

# DQ (11/11/2012): Added support to build testTranslator if it has not been built yet.
$(TRANSLATOR_EXECUTABLE):
	cd $(top_builddir)/tests/; $(MAKE) testTranslator

# Build the list of object files
TEST_Objects = ${TESTCODES:.cc=.o}

TEST2_Objects = ${C_TESTCODES:.c=.o}

ROSE_FLAGS = --edg:no_warnings -w

TRANSLATOR_EXECUTABLE = $(top_builddir)/tests/testTranslator 
TEST_TRANSLATOR = $(TRANSLATOR_EXECUTABLE) $(ROSE_FLAGS)

## Include the path to A++ and the transformation specification
# TESTCODE_AM_CPPFLAGS = -I$(top_srcdir)/tests/CompileTests/A++Code -I$(srcdir) 

$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.cc)

$(TEST2_Objects): $(C_TESTCODES)
	$(TEST_TRANSLATOR) -rose:C $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.c)


EXTRA_DIST = $(TESTCODES) $(C_TESTCODES) bugs

clean-local:
	rm -f C/*.o *.o C/rose_*.c rose_*.C rose_*.c rose_*.cc *.sage.C test2001_??.C.show *.dot *.ps \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C core test2005_56
	rm -rf ti_files *.ti *.et

check-local: $(TRANSLATOR_EXECUTABLE) 
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) $(TEST2_Objects)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C++Code/ElsaTestCases/gnu: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"



