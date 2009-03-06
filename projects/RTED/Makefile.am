include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_RTED

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I.
INCLUDES2 = $(ROSE_INCLUDES) -I../../

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl

runtimeCheck_SOURCES = runtimeCheck.cpp \
                       RtedTransformation.cpp \
                       RuntimeSystem.cpp 

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

EXTRA_DIST = example.C \
             RtedTransformation.h \
             RuntimeSystem.h \
             index_array.txt 

CLEANFILES = *.out *.ti *.pdf test_runtime* *.txt rose_test_c_E_1_1_a

compile_gcc_file1:
	@echo "Compiling new program with g++"
# make sure rose_c_E_1_1_a.c and rose_rose_E_1_1_a.c compile
	$(CXX) $(INCLUDES) -I$(rted_path)/C/array_index_out_of_bound/ -I$(top_builddir) -I$(srcdir) rose_c_E_1_1_a.c $(srcdir)/RuntimeSystem.cpp -o rose_test_c_E_1_1_a 
	$(CXX) $(INCLUDES) -I$(rted_path)/C/array_index_out_of_bound/ -I$(top_builddir) -I$(srcdir) rose_rose_c_E_1_1_a.c $(srcdir)/RuntimeSystem.cpp  -o rose_rose_test_c_E_1_1_a 

modifyTestCode:
	@echo "inserting #define EXITCODE_OK 0 in test code"
	if [ `grep "#define EXITCODE_OK 0" $(rted_path)/C/array_index_out_of_bound/c_E_1_1_a.c | wc | awk '{print $$1}' ` -eq 0 ] ; then \
  	sed -i '1i\#define EXITCODE_OK 0' $(rted_path)/C/array_index_out_of_bound/c_E_1_1_a.c ; \
	fi 

build_file1: modifyTestCode
	@echo "RTED is configured in dir : $(rted_path)"
	@echo "Transforming original test program ... "
# This breaks right now because of the backend!
	./runtimeCheck -rtedpath $(rted_path) -path /C/array_index_out_of_bound/ -file c_E_1_1_a.c -rose:Cxx $(INCLUDES) -I$(top_builddir) -I$(srcdir) # $(srcdir)/RuntimeSystem.cpp
# created a new file called rose_c_E_1_1_a.c and rose_rose_E_1_1_a.c	
	cp $(srcdir)/index_array.txt .

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runtimeCheck $(TESTCODES) build_file1 compile_gcc_file1
	@echo "Running the new program"
	./rose_test_c_E_1_1_a
	./rose_rose_test_c_E_1_1_a
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C rose_*.c *.dot Templates.DB

endif
