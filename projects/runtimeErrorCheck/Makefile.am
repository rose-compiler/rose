include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I.
INCLUDES2 = $(ROSE_INCLUDES) -I../../

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl

runtimeCheck_SOURCES = runtimeCheck.cpp 

runtimeCheck_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

TESTCODES =  $(srcdir)/example.C 
TESTCODES2 = $(srcdir)/example2.C
if !ROSE_USE_BINARY_SQL
  TESTCODES3 = $(srcdir)/example3.C
endif
TESTCODES4 = $(srcdir)/example4.C
TESTCODES5 = $(srcdir)/example5.C

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

EXTRA_DIST = example.C example2.C example4.C example5.C example3.C

CLEANFILES = *.out *.ti *.pdf test_runtime*

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runtimeCheck $(TESTCODES)
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES) -run -o test_bin
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES) -pdf -o test_bin
if ROSE_USE_OPEN_FORTRAN_PARSER
	./test_bin 
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES2) -run -o test_bin2
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES2) -pdf -o test_bin2
	./test_bin2 
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES4) -run -o test_bin4
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES4) -pdf -o test_bin4
	./test_bin4 
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES5) -run -o test_bin5
	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES5) -pdf -o test_bin5
	./test_bin5 
if !ROSE_USE_BINARY_SQL
#	DQ (12/23/2008): Commented out this example which fails...
#	./runtimeCheck -DCAN_NOT_COMPILE_WITH_ROSE=true $(TESTCODES3) -run -c -o test_bin3.o $(INCLUDES2)
#	$(LIBTOOL) --mode=link $(CXX) -o test_bin3 test_bin3.o $(LDADD)
#	./test_bin3
endif
endif
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C *.dot Templates.DB

