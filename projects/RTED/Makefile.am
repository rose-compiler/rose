include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_RTED

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I.
INCLUDES2 = $(ROSE_INCLUDES) -I../../

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl

runtimeCheck_SOURCES = runtimeCheck.cpp 

runtimeCheck_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

TESTCODES =  $(srcdir)/example.C 

runtimeCheck.o: runtimeCheck.cpp

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

TEST_TRANSLATOR = ./runtimeCheck all $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
$(TEST_Objects): runtimeCheck $(TESTCODES) 
	@echo "RTED is configured in dir : $(rted_path)"
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	env SRCDIR="$(srcdir)" $(TEST_TRANSLATOR)

EXTRA_DIST = example.C example2.C example4.C example5.C example3.C

CLEANFILES = *.out *.ti *.pdf test_runtime*

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runtimeCheck $(TESTCODES)
	@echo "RTED is configured in dir : $(rted_path)"
	./runtimeCheck $(rted_path) 
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C *.dot Templates.DB

endif
