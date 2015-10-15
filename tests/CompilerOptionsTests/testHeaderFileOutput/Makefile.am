## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/CompileTests/A++Code -I$(srcdir) 
# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/CompilerOptionsTests/A++Code -I$(srcdir) 

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = testHeaderFiles.C 
TESTCODES2 = test2009_515.c

# codes used to isolate bugs (not very interesting)
# testUnparser.C findNullBlockInEDG.C

# Files which don't compile (but used to do so) (this has been isolated as test2001_23.C)
# f_op_all.C i_op_all.C d_op_all.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# Restrict to level 6 testing for the moment
# ROSE_FLAGS = -rose:test 6

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "##############################"
	@echo "Testing use of \"-E\" options:"
	@echo "     a standard compiler option which runs the \"C\" preprocessor (cpp) and pipes output to stdout"
	@echo "##############################"
	@$(MAKE) $(TEST_Objects) rose_test2009_515.c
	@echo "*****************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testHeaderFileOutput: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************************"

# Test the -H option with all of these example files
$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) -H -c $(srcdir)/$(@:.o=.C) > headerList
# test if the input file's path is searched first as an include path
# a conflicting header in another path is provided to catch any possible order problem
# Liao, 5/15/2009
rose_test2009_515.c:$(srcdir)/test2009_515.c
	$(TEST_TRANSLATOR) -c -I$(srcdir)/myinclude $(srcdir)/test2009_515.c

EXTRA_DIST = $(TESTCODES) myinclude $(TESTCODES2) test2009_515.h

distclean-local:
	rm -rf headerList

clean-local:
	rm -f *.o rose_*.C *.sage.C rose_*.c test2001_??.C.show outputFile \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files






