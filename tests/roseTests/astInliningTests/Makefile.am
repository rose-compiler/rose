include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = \
   inlineEverything

inlineEverything_SOURCES = inlineEverything.C

# inlineEverything_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
inlineEverything_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TESTCODES = \
   pass16.C pass1.C pass2.C pass3.C pass4.C pass5.C pass6.C pass7.C pass8.C pass9.C pass10.C \
   pass11.C pass12.C pass13.C pass14.C pass15.C pass17.C pass18.C pass19.C \
   pass20.C pass21.C

# DQ (12/14/2006): commented out for now
# pass16.C 

# These codes are DESIGNED to fail in the inlining
# fail1.C fail2.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

#TEST_TRANSLATOR = $(top_builddir)/tests/roseTests/astInliningTests/inlineEverything $(ROSE_FLAGS)
TEST_TRANSLATOR = ./inlineEverything $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
$(TEST_Objects): $(TESTCODES) inlineEverything
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	$(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	@echo "Running resulting executable ..."
	./$(@:.o=)

EXTRA_DIST = $(TESTCODES)

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: 
	@$(MAKE) $(TEST_Objects)
	@echo "****************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astInlining: make check rule complete (terminated normally) ***"
	@echo "****************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files pass[1-9] pass1[0-9] pass2[0-9] rose_pass*.C *.dot *.pdf

distclean-local:
	rm -rf Templates.DB 

test1: test1A.C test1B.C
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	DQ(7/13/2004): This is an error since it specifies the output file twice in the vendor compile line
#	$(TEST_TRANSLATOR) -rose:verbose -o test1A.o -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose -c $(srcdir)/test1B.C
	g++ -o test1 rose_test1A.o rose_test1B.o
	@echo "Running resulting executable ..."
	./test1


