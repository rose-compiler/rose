include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I$(builddir) 

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl -lruntimeTransformation

runtimeCheck_SOURCES = runtimeCheck.cpp 
runtimeCheck_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

runtimeCheck_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

TESTCODES = example.C

runtimeCheck.o: runtimeCheck.cpp

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

TEST_TRANSLATOR = ./runtimeCheck all $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
$(TEST_Objects): runtimeCheck $(TESTCODES) 
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	env SRCDIR="$(srcdir)" $(TEST_TRANSLATOR)

EXTRA_DIST = example.C

CLEANFILES = *.out example.C *.ti *.pdf *.C

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runtimeCheck $(TESTCODES)
	cp $(srcdir)/$(TESTCODES) .
	./runtimeCheck $(TESTCODES) 
#	g++ -o test rose_$(TESTCODES)
#	./rose_$(TESTCODES)
#	@echo "*********************************************************************************************************************"
#	@echo "*** ROSE/tests/roseTests/programAnalysisTests/defUseAnalysisTests: make check rule complete (terminated normally) ***"
#	@echo "*********************************************************************************************************************"



clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test* rose_*.C *.dot 

distclean-local:
	rm -rf Templates.DB 

