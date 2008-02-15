## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/CompileTests/A++Code -I$(srcdir) 
# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/CompilerOptionsTests/A++Code -I$(srcdir) 

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = 

# codes used to isolate bugs (not very interesting)
# testUnparser.C findNullBlockInEDG.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# Restrict to level 6 testing for the moment
# ROSE_FLAGS = -rose:test 6

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

testFilenames:
	$(srcdir)/test_filenames.pl

testLongFilenames:
	$(srcdir)/test_filenames_looong_torture.pl

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#########################################"
	@echo "Testing use of filenames skipped for now!"
	@echo "#########################################"
#	@$(MAKE) testFilenames
#	@$(MAKE) testLongFilenames
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testFileNamesAndExtensions: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************"

# Test the -H option with all of these example files
$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) -H -c $(srcdir)/$(@:.o=.C) > headerList

EXTRA_DIST = README test_filenames_looong_torture.pl test_filenames.pl 

# DQ (1/15/2007): We will likely generate these file names directly 
# instead of representingthem explicitly in the source tree.
# fileExtensions fileNames

distclean-local:
	rm -rf headerList

clean-local:
	rm -f *.o *.C rose_*.C *.sage.C test2001_??.C.show outputFile \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files






