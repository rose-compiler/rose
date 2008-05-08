include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I$(builddir) 
INCLUDES2 = $(ROSE_INCLUDES) -I../../

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl

runtimeCheck_SOURCES = runtimeCheck.cpp 
runtimeCheck_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

runtimeCheck_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

TESTCODES = example.C 
TESTCODES2 = example2.C
TESTCODES3 = example3.C

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

EXTRA_DIST = example.C example2.C example3.C

CLEANFILES = *.out example.C *.ti *.pdf *.C

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runtimeCheck $(TESTCODES)
	cp $(srcdir)/example.C .
	cp $(srcdir)/example2.C .
	cp $(srcdir)/example3.C .
	echo $(ROSE_LIBS_WITH_PATH)
	echo $(LIBS_WITH_RPATH)
	echo $(LIBS_WITH_RPATH)
	./runtimeCheck $(TESTCODES) -run -o test_bin
#	$(CXX) -o test_bin rose_$(TESTCODES)
	./test_bin 
	./runtimeCheck $(TESTCODES2) -run -o test_bin2
#	$(CXX) -o test_bin2 rose_$(TESTCODES2)
	./test_bin2 
	./runtimeCheck $(TESTCODES3) -run -c -o test_bin3.o $(INCLUDES2)
	libtool --mode=link $(CXX) -o test_bin3 test_bin3.o $(LDADD)
#	$(CXX) -o test_bin3 rose_$(TESTCODES3) $(INCLUDES2) $(LDADD)
	./test_bin3 example.C -run
#	@echo "*********************************************************************************************************************"
#	@echo "*** ROSE/tests/roseTests/programAnalysisTests/defUseAnalysisTests: make check rule complete (terminated normally) ***"
#	@echo "*********************************************************************************************************************"



clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test* rose_*.C *.dot 

distclean-local:
	rm -rf Templates.DB 

