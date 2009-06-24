include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_RTED

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I. -Winvalid-pch 
INCLUDES2 = $(ROSE_INCLUDES) -I../../



if ROSE_WITH_ROSEQT
    LINK_QT_DEBUGGER=./rted_qt/.libs/libRtedQt.a $(QT_LDFLAGS) -L$(top_builddir)/projects/roseQt/widgets/.libs/ -lRoseQtWidgets -Wl,-rpath,`pwd`/$(top_builddir)/projects/roseQt/widgets/.libs -DROSE_WITH_ROSEQT
else
    LINK_QT_DEBUGGER=
endif

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

noinst_PROGRAMS = test
test_SOURCES = test.C
test_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3
run_test:test
	./test$(EXEEXT) $(TEST_CXXFLAGS) $(srcdir)/buffer2.c 


LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl 

runtimeCheck_SOURCES = runtimeCheck.cpp \
                       RtedTransformation.cpp \
                       RuntimeSystem.c \
                       RtedSymbols.cpp \
                       RtedTransf_array.cpp RtedTransf_prolog.cpp \
                       RtedTransf_support.cpp RtedTransf_funccall.cpp 


SUBDIRS = rted_qt

runtimeCheck_LDADD =  $(ROSE_LIBS)
if ROSE_WITH_ROSEQT
	runtimeCheck_LDADD += $(top_builddir)/projects/RTED/rted_qt/libRtedQt.la
endif

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

# The following runs a test in parallel
# it is currently not used.
check-C-array-parallel:
	COUNTER=0 ; \
	BASE=0; \
	cp $(srcdir)/index_array.txt . ; \
	dir=$(rted_path)/C/array_index_out_of_bound/ ; \
	echo "Dir : $$dir " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	COUNTER=$$(($$COUNTER+1)) ; \
	{ \
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
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
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
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "$$binfile exists." ; \
	fi; \
	fi; \
	} &  \
	pid[$$COUNTER]=$$! ; \
	echo "Adding PID $!  $${pid[$$COUNTER]}" ; \
	if [ $$(( $$COUNTER - $$BASE)) -eq 4 ]; then  \
		for (( i=$$BASE ; i < $$COUNTER ; i++)); do  \
	    	     p=$${pid[i]} ; \
	     	     wait $$p ; \
		     status=$$? ; \
		     echo "job $$i, pid $$p: exit $$status" ; \
		     if [ $$status -ne 127 ]; then  \
	     		if [ $$status -ne 0 ]; then exit 1; fi ;  \
	    	     fi ; \
	  	done ; \
	 BASE=$$(($$COUNTER)) ; \
         fi ; \
	done 



check-C-array:
	if [ -f "result.txt" ]; then rm result.txt; fi ; \
	cp $(srcdir)/index_array.txt . ; \
	dir=$(rted_path)/C/array_index_out_of_bound/ ; \
	echo "Dir : $$dir " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	echo "Current file : $$file " ; \
        file_r=$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	cp $$file $$file_r ; \
	file=$$file_r;\
        binfile=$$(echo "$$file".bin); \
	if ! [ -f $$binfile ]; then \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  if [ -f $$file2 ]; then \
            file2_r=$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	    echo "  cp  $$file2 to $$file2_r" ; \
	    cp $$file2 $$file2_r ; \
	    file2=$$file2_r;\
	    echo "  Filename 2 : $$file2" ; \
	    echo "  inserting #include \"RuntimeSystem.h\" in File2: $$file2" ; \
	    if [ `grep "#include \"RuntimeSystem.h\"" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	      sed -i '1i\#include \"RuntimeSystem.h\"' $$file2 ; \
	    fi ; \
	    echo "  inserting #define EXITCODE_OK 0 in File2: $$file2" ; \
	    if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	      sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	    fi ; \
  	  fi; \
	echo "  inserting #include \"RuntimeSystem.h\" in File: $$file" ; \
	if [ `grep "#include \"RuntimeSystem.h\"" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#include \"RuntimeSystem.h\"' $$file ; \
	fi ; \
	echo "  inserting #define EXITCODE_OK 0 in File: $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	  if [ -f $$file2 ]; then \
	  echo "Script : Running Transformation 2 ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir  ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir)  $(LINK_QT_DEBUGGER)  " ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)  ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "Skipping _s file." ; \
	fi; \
	else \
	  echo "$$binfile exists.\n" ; \
	fi; \
	done 


check-C-string:
	if [ -f "result.txt" ]; then rm result.txt; fi ; \
	cp $(srcdir)/index_array.txt .
	dir=$(rted_path)/C/string_errors/ ; \
	echo "Dir : $$dir " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	echo "Current file : $$file " ; \
        file_r=$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	cp $$file $$file_r ; \
	file=$$file_r;\
        binfile=$$(echo "$$file".bin); \
	if ! [ -f $$binfile ]; then \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  if [ -f $$file2 ]; then \
            file2_r=$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	    echo "  cp  $$file2 to $$file2_r" ; \
	    cp $$file2 $$file2_r ; \
	    file2=$$file2_r;\
	    echo "  Filename 2 : $$file2" ; \
	    echo "  inserting #include \"RuntimeSystem.h\" in File2: $$file2" ; \
	    if [ `grep "#include \"RuntimeSystem.h\"" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	      sed -i '1i\#include \"RuntimeSystem.h\"' $$file2 ; \
	    fi ; \
	    echo "  inserting #define EXITCODE_OK 0 in File2: $$file2" ; \
	    if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	      sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	    fi ; \
  	  fi; \
	echo "  inserting #include \"RuntimeSystem.h\" in File: $$file" ; \
	if [ `grep "#include \"RuntimeSystem.h\"" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#include \"RuntimeSystem.h\"' $$file ; \
	fi ; \
	echo "  inserting #define EXITCODE_OK 0 in File: $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	  if [ -f $$file2 ]; then \
	  echo "Script : Running Transformation 2 ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir  ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir)  $(LINK_QT_DEBUGGER)  " ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)  ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "Skipping _s file." ; \
	fi; \
	else \
	  echo "$$binfile exists.\n" ; \
	fi; \
	done 

check-C-subprogram:
	if [ -f "result.txt" ]; then rm result.txt; fi ; \
	cp $(srcdir)/index_array.txt .
	dir=$(rted_path)/C/subprogram_call_errors/ ; \
	echo "Dir : $$dir " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	echo "Current file : $$file " ; \
        file_r=$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	cp $$file $$file_r ; \
	file=$$file_r;\
        binfile=$$(echo "$$file".bin); \
	if ! [ -f $$binfile ]; then \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  if [ -f $$file2 ]; then \
            file2_r=$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	    echo "  cp  $$file2 to $$file2_r" ; \
	    cp $$file2 $$file2_r ; \
	    file2=$$file2_r;\
	    echo "  Filename 2 : $$file2" ; \
	    echo "  inserting #include \"RuntimeSystem.h\" in File2: $$file2" ; \
	    if [ `grep "#include \"RuntimeSystem.h\"" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	      sed -i '1i\#include \"RuntimeSystem.h\"' $$file2 ; \
	    fi ; \
	    echo "  inserting #define EXITCODE_OK 0 in File2: $$file2" ; \
	    if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	      sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	    fi ; \
  	  fi; \
	echo "  inserting #include \"RuntimeSystem.h\" in File: $$file" ; \
	if [ `grep "#include \"RuntimeSystem.h\"" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#include \"RuntimeSystem.h\"' $$file ; \
	fi ; \
	echo "  inserting #define EXITCODE_OK 0 in File: $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	  if [ -f $$file2 ]; then \
	  echo "Script : Running Transformation 2 ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir  ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir)  $(LINK_QT_DEBUGGER)  " ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)  ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "Skipping _s file." ; \
	fi; \
	else \
	  echo "$$binfile exists.\n" ; \
	fi; \
	done 

check-C-allocation:
	if [ -f "result.txt" ]; then rm result.txt; fi ; \
	cp $(srcdir)/index_array.txt .
	dir=$(rted_path)/C/allocation_deallocation_errors/ ; \
	echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>> Dir : $$dir " ; \
	echo " " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	echo "Current file : $$file " ; \
        file_r=$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	cp $$file $$file_r ; \
	file=$$file_r;\
        binfile=$$(echo "$$file".bin); \
	if ! [ -f $$binfile ]; then \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  if [ -f $$file2 ]; then \
            file2_r=$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	    echo "  cp  $$file2 to $$file2_r" ; \
	    cp $$file2 $$file2_r ; \
	    file2=$$file2_r;\
	    echo "  Filename 2 : $$file2" ; \
	    echo "  inserting #include \"RuntimeSystem.h\" in File2: $$file2" ; \
	    if [ `grep "#include \"RuntimeSystem.h\"" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	      sed -i '1i\#include \"RuntimeSystem.h\"' $$file2 ; \
	    fi ; \
	    echo "  inserting #define EXITCODE_OK 0 in File2: $$file2" ; \
	    if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	      sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	    fi ; \
  	  fi; \
	echo "  inserting #include \"RuntimeSystem.h\" in File: $$file" ; \
	if [ `grep "#include \"RuntimeSystem.h\"" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#include \"RuntimeSystem.h\"' $$file ; \
	fi ; \
	echo "  inserting #define EXITCODE_OK 0 in File: $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	  if [ -f $$file2 ]; then \
	  echo "Script : Running Transformation 2 ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir  ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir)  $(LINK_QT_DEBUGGER)  " ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)  ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "Skipping _s file." ; \
	fi; \
	else \
	  echo "$$binfile exists.\n" ; \
	fi; \
	done 

check-C-inputoutput:
	if [ -f "result.txt" ]; then rm result.txt; fi ; \
	cp $(srcdir)/index_array.txt .
	cp $(srcdir)/file_read.txt .	
	dir=$(rted_path)/C/input_output_errors/ ; \
	echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>> Dir : $$dir " ; \
	echo " " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	echo "Current file : $$file " ; \
        file_r=$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	cp $$file $$file_r ; \
	file=$$file_r;\
        binfile=$$(echo "$$file".bin); \
	if ! [ -f $$binfile ]; then \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  if [ -f $$file2 ]; then \
            file2_r=$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	    echo "  cp  $$file2 to $$file2_r" ; \
	    cp $$file2 $$file2_r ; \
	    file2=$$file2_r;\
	    echo "  Filename 2 : $$file2" ; \
	    echo "  inserting #include \"RuntimeSystem.h\" in File2: $$file2" ; \
	    if [ `grep "#include \"RuntimeSystem.h\"" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	      sed -i '1i\#include \"RuntimeSystem.h\"' $$file2 ; \
	    fi ; \
	    echo "  inserting #define EXITCODE_OK 0 in File2: $$file2" ; \
	    if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	      sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	    fi ; \
  	  fi; \
	echo "  inserting #include \"RuntimeSystem.h\" in File: $$file" ; \
	if [ `grep "#include \"RuntimeSystem.h\"" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#include \"RuntimeSystem.h\"' $$file ; \
	fi ; \
	echo "  inserting #define EXITCODE_OK 0 in File: $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	  if [ -f $$file2 ]; then \
	  echo "Script : Running Transformation 2 ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir  ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir)  $(LINK_QT_DEBUGGER)  " ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)  ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "Skipping _s file." ; \
	fi; \
	else \
	  echo "$$binfile exists.\n" ; \
	fi; \
	done 

check-C-memoryleaks:
	if [ -f "result.txt" ]; then rm result.txt; fi ; \
	cp $(srcdir)/index_array.txt .
	dir=$(rted_path)/C/memory_leaks/ ; \
	echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>> Dir : $$dir " ; \
	echo " " ; \
	for file in $$(find $$dir -name "*.c" | sort); do \
	  echo "Found file : $$file" ; \
	done ; \
	echo "Found files : `find $$dir -name "*.c" | wc` " ; \
	for file in $$(find $$dir -name "*.c"| sort); do \
	echo "Current file : $$file " ; \
        file_r=$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	cp $$file $$file_r ; \
	file=$$file_r;\
        binfile=$$(echo "$$file".bin); \
	if ! [ -f $$binfile ]; then \
	if !  [[ "$$file" =~ "_s.c" ]]  ; then \
	  file2=$$(echo $$file | sed -e 's/..$$//'  ); \
	  file2=$$(echo "$$file2"_s.c); \
	  if [ -f $$file2 ]; then \
            file2_r=$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	    echo "  cp  $$file2 to $$file2_r" ; \
	    cp $$file2 $$file2_r ; \
	    file2=$$file2_r;\
	    echo "  Filename 2 : $$file2" ; \
	    echo "  inserting #include \"RuntimeSystem.h\" in File2: $$file2" ; \
	    if [ `grep "#include \"RuntimeSystem.h\"" $$file2 | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	      sed -i '1i\#include \"RuntimeSystem.h\"' $$file2 ; \
	    fi ; \
	    echo "  inserting #define EXITCODE_OK 0 in File2: $$file2" ; \
	    if [ `grep "#define EXITCODE_OK 0" $$file2 | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	      sed -i '1i\#define EXITCODE_OK 0' $$file2 ; \
	    fi ; \
  	  fi; \
	echo "  inserting #include \"RuntimeSystem.h\" in File: $$file" ; \
	if [ `grep "#include \"RuntimeSystem.h\"" $$file | wc | awk '{print $$1}' ` -eq 0 ] ; then \
	  sed -i '1i\#include \"RuntimeSystem.h\"' $$file ; \
	fi ; \
	echo "  inserting #define EXITCODE_OK 0 in File: $$file" ; \
	if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$2}' ` -eq 0 ] ; then \
	  sed -i '1i\#define EXITCODE_OK 0' $$file ; \
	fi ; \
	  if [ -f $$file2 ]; then \
	  echo "Script : Running Transformation 2 ... $$file $$file2 -I$(srcdir) " ; \
	  ./runtimeCheck 2 $$file $$file2 -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir  ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file2_rose=rose_$$(echo $$file2 | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 2f:  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir)  $(LINK_QT_DEBUGGER)  " ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $$file2_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)  ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $$file2 $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	else \
	  echo "Script : Running Transformation ... $$file -I$(srcdir)" ; \
	  ./runtimeCheck 1 $$file -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$$dir ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  echo "removing #define EXITCODE_OK 0 in $$file" ; \
	  if [ `grep "#define EXITCODE_OK 0" $$file | wc | awk '{print $$1}' ` -eq 1 ] ; then \
	    sed -i '1d' $$file ; \
	  fi ; \
	  file_rose=rose_$$(echo $$file | sed -e 's/^\(.*\/\)*//'  ); \
	  file_rose_bin=$$(echo "$$file_rose".bin); \
	  echo "Compiling 1f:  $(CXX) -g -o $$file_rose_bin $$file_rose" ;\
	  $(CXX) -g -o $$file_rose_bin $$file_rose $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	  echo "Executing created file $$file_rose_bin" ; \
	  ./$$file_rose_bin ; \
	  if [ $$? -eq 0 ] ; then echo "Worked." ; else echo "Exit" ; exit 1 ; fi ; \
	  file_bin=$$(echo "$$file".bin); \
	  $(CXX) -g -o $$file_bin $$file $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$$dir -I$(top_builddir) -I$(srcdir) ; \
	fi; \
	else \
	  echo "Skipping _s file." ; \
	fi; \
	else \
	  echo "$$binfile exists.\n" ; \
	fi; \
	done 


deleteAllbinfiles:
	@echo "Deleting all generated bin files in RTED directory"
	if [ -f "result.txt" ]; then rm result.txt; fi ; \
	dir=$(rted_path)/C/array_index_out_of_bound/ ; \
	if [ "" == "`find $$dir -name "*.bin"`" ]; then \
	   echo "No bin files exist in $$dir" ; \
	else \
	   echo "Deleting bin files in $$dir" ; \
	   echo "rm $${dir}*.bin" ; \
	   rm $${dir}*.bin ; \
	fi; \
	dir=$(rted_path)/C/string_errors/ ; \
	if [ "" == "`find $$dir -name "*.bin"`" ]; then \
	   echo "No bin files exist in $$dir" ; \
	else \
	   echo "Deleting bin files in $$dir" ; \
	   echo "rm $${dir}*.bin" ; \
	   rm $${dir}*.bin ; \
	fi;
	dir=$(rted_path)/C/subprogram_call_errors/ ; \
	if [ "" == "`find $$dir -name "*.bin"`" ]; then \
	   echo "No bin files exist in $$dir" ; \
	else \
	   echo "Deleting bin files in $$dir" ; \
	   echo "rm $${dir}*.bin" ; \
	   rm $${dir}*.bin ; \
	fi;
	dir=$(rted_path)/C/allocation_deallocation_errors/ ; \
	if [ "" == "`find $$dir -name "*.bin"`" ]; then \
	   echo "No bin files exist in $$dir" ; \
	else \
	   echo "Deleting bin files in $$dir" ; \
	   echo "rm $${dir}*.bin" ; \
	   rm $${dir}*.bin ; \
	fi;
	dir=$(rted_path)/C/input_output_errors/ ; \
	if [ "" == "`find $$dir -name "*.bin"`" ]; then \
	   echo "No bin files exist in $$dir" ; \
	else \
	   echo "Deleting bin files in $$dir" ; \
	   echo "rm $${dir}*.bin" ; \
	   rm $${dir}*.bin ; \
	fi;
	dir=$(rted_path)/C/memory_leaks/ ; \
	if [ "" == "`find $$dir -name "*.bin"`" ]; then \
	   echo "No bin files exist in $$dir" ; \
	else \
	   echo "Deleting bin files in $$dir" ; \
	   echo "rm $${dir}*.bin" ; \
	   rm $${dir}*.bin ; \
	fi;

c-compile-runtimesystem:
	@echo "Make sure that RuntimeSystem compiles with C"
	$(CC) -Wall -g -I$(srcdir) $(srcdir)/test_runtime.c $(srcdir)/RuntimeSystem.c -o runtime_test.bin
	@echo "Cleaning all .bin testcodes"
	rm *.bin

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: rted_qt runtimeCheck $(TESTCODES) c-compile-runtimesystem  deleteAllbinfiles check-C-array check-C-string check-C-inputoutput
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"


clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C rose_*.c *.dot Templates.DB *~ rose_rose* test *.bin *.txt *.c *.cpp *.bin

endif
