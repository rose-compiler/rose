## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

C_TESTCODES = \
dC0010.c\
dC0011.c\
dC0013.c\
dC0017.c\
dC0018.c

# These fail as a result of the new constant folding support, I think
START_OF_FAILED_CONSTANT_FOLDING_TESTS_USING_ROSE = \
d0124b.c\
dC0019.c

START_OF_FAILED_TESTS_USING_ROSE = \
dC0020.c\
dC0021.c\
dC0022.c\
dC0023.c\
dC0024.c\
dC0025.c\
dC0026.c\
dC0027.c\
dC0028.c\
dC0029.c\
dC0030.c\
dC0031.c\
dC0032.c\
k0001.c\
k0002.c\
k0003.c\
k0003a.c\
k0004.c\
k0005.c\
k0006a.c\
k0006.c\
k0007.c\
k0008.c\
k0009.c\
k0010.c\
k0011.c\
t0001.c\
t0002.c\
t0003.c\
t0005.c\
t0006.c\
t0007.c\
t0008.c\
t0009.c\
t0010.c\
t0011.c\
t0012.c\
t0014.c\
t0015.c\
t0016.c\
t0017.c\
t0018.c\
t0019.c\
t0020.c\
t0021.c\
t0023.c\
t0024.c\
t0025.c\
t0026.c

# DQ (11/11/2012): Added support to build testTranslator if it has not been built yet.
$(TRANSLATOR_EXECUTABLE):
	cd $(top_builddir)/tests/; $(MAKE) testTranslator

TEST_Objects = ${C_TESTCODES:.c=.o}

ROSE_FLAGS = -rose:C --edg:no_warnings -w

TRANSLATOR_EXECUTABLE = $(top_builddir)/tests/testTranslator 
TEST_TRANSLATOR = $(TRANSLATOR_EXECUTABLE) $(ROSE_FLAGS)

$(TEST_Objects): $(C_TESTCODES)
	$(TEST_TRANSLATOR) $(AM_CPPFLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.c)

EXTRA_DIST = $(C_TESTCODES)

clean-local:
	rm -f C/*.o *.o C/rose_*.c rose_*.C rose_*.c rose_*.cc *.sage.C test2001_??.C.show *.dot *.ps \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C core test2005_56
	rm -rf ti_files *.ti *.et

check-local: $(TRANSLATOR_EXECUTABLE) 
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) $(TEST2_Objects)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C++Code/ElsaTestCases/ctests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"



