include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_RTED

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I. -Winvalid-pch 
INCLUDES2 = $(ROSE_INCLUDES) -I../../

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

noinst_PROGRAMS = \
	test

test_SOURCES = test.C
test_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3

run_test:test
	./test$(EXEEXT) $(TEST_CXXFLAGS) $(srcdir)/buffer2.c 


LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl

runtimeCheck_SOURCES = runtimeCheck.cpp \
                       RtedTransformation.cpp \
                       RuntimeSystem.cpp \
                       RtedSymbols.cpp \
                       RtedTransf_array.cpp RtedTransf_prolog.cpp \
                       RtedTransf_support.cpp

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
             index_array.txt \
             DataStructures.h \
             RtedSymbols.h

CLEANFILES = *.out *.ti *.pdf test_runtime* *.txt rose_test_c_E_1_1_a

compile_gcc_file1: build_file1
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
	./runtimeCheck 1 $(rted_path)/C/array_index_out_of_bound/c_E_1_1_a.c -rose:Cxx -c $(INCLUDES) -I$(rted_path)/C/array_index_out_of_bound/ -I$(top_builddir) -I$(srcdir) # $(srcdir)/RuntimeSystem.cpp ; \
        if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi 
# created a new file called rose_c_E_1_1_a.c and rose_rose_E_1_1_a.c	
	cp $(srcdir)/index_array.txt .

check-C-array:
	COUNTER=0 ; \
	if [ -f "result-array.txt" ]; then rm result-array.txt; fi ; \
	cp $(srcdir)/index_array.txt .
	dir=$(rted_path)/C/array_index_out_of_bound/ ; \
	echo "Dir : $$dir " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  binfile=$$(echo "$$file".bin); \
	echo "Filename 2 : $$file2" ; \
	if ! [ -f $$binfile ]; then \
	echo "inserting #define EXITCODE_OK 0 in $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	  if [ -f $$file2 ]; then \
	  echo "inserting #define EXITCODE_OK 0 in $$file2" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	    sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	  fi ; \
	  echo "Script : Running Transformation ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir  ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "deleting #define EXITCODE_OK 0 in $$file2" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file2 ; \
	  fi ; \
	  file_rose=rose_rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "$$binfile exists." ; \
	fi; \
	fi; \
	done 

remember:
	{ \
	job ;
	} & \
	COUNTER=$$(($$COUNTER+1)) ; \
	if [ "$$COUNTER" -ge 1 ] ; then \
	    echo "Running ... " ; \
	    wait ; \
	    COUNTER=0 ; \
	fi ; 

check-C-string:
	if [ -f "result-string.txt" ]; then rm result-string.txt; fi ; \
	cp $(srcdir)/index_array.txt .
	dir=$(rted_path)/C/string_errors/ ; \
	echo "Dir : $$dir " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  binfile=$$(echo "$$file".bin); \
	echo "Filename 2 : $$file2" ; \
	if ! [ -f $$binfile ]; then \
	echo "inserting #define EXITCODE_OK 0 in $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	if [ -f $$file2 ]; then \
	  echo "inserting #define EXITCODE_OK 0 in $$file2" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	    sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	  fi ; \
	  echo "Script : Running Transformation ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$(dir)   ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "deleting #define EXITCODE_OK 0 in $$file2" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file2 ; \
	  fi ; \
	  file_rose=rose_rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.cpp $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "$$binfile exists." ; \
	fi; \
	fi; \
	done 

check-C-array-onefile: compile_gcc_file1
	@echo "Running the new program"
	./rose_test_c_E_1_1_a
	./rose_rose_test_c_E_1_1_a

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runtimeCheck $(TESTCODES) check-C-array-onefile check-C-array check-C-string
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"


clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C rose_*.c *.dot Templates.DB *~ rose_rose* test *.bin *.txt

endif
