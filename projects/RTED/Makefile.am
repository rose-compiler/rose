include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = index_array.txt \
             file_read.txt \
	     tests upccrose \
	     upcparam.h upcparam_s.h \
	     extract-actual-error.sed extract-expected-error.sed
# required/upc.h

# Currently, we exclude 32bit and MacOS
#if !OS_VENDOR_REDHAT_32BIT
if !OS_MACOSX

# To keep compile commands tidy, please ONLY put required include flags here.
# tps removed -pthreads from INCLUDES
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I. -Winvalid-pch
TESTSUITEINCL = -I$(srcdir) -I.
UPC_INCLUDES =  -I$(top_srcdir)/config

EXTRACT_EXPECTED_ERROR_SCRIPT=$(srcdir)/extract-expected-error.sed
EXTRACT_EXPECTED_ERROR=sed -n -f $(EXTRACT_EXPECTED_ERROR_SCRIPT)
EXTRACT_ACTUAL_ERROR=sed -n -f $(srcdir)/extract-actual-error.sed


# Qt Debugger is now linked into CppRuntimeSystem
LINK_QT_DEBUGGER=



# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck


LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl
#$(top_builddir)/projects/RTED/CppRuntimeSystem/libCppRuntimeSystem.a

RTS=$(srcdir)/RuntimeSystem.cpp
RTSCXX=RuntimeSystemCxx.o
RTSUPC=RuntimeSystemUpc.o ParallelRTS.o

RTED_CXX = $(LIBTOOL) --tag=CXX --mode=link g++ -g $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS)
RTED_CC = $(LIBTOOL) --tag=CC --mode=link gcc -g $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS)

#$(LIBTOOL) --tag=CXX  --mode=link $(AM_PROG_UPC)
if ROSE_USE_RTED
RTED_UPC =  ./upccrose $(AM_CXXFLAGS) \
	 $(AM_LDFLAGS)
else
RTED_UPC =
endif

RTED_LIB=CppRuntimeSystem/libCppRuntimeSystem.la
RTED_UPC_LIB=CppRuntimeSystem/libUpcRuntimeSystem.la
runtimeCheck_SOURCES = runtimeCheck.cpp \
                       RtedTransformation.cpp ExecuteTransformations.cpp \
                       RtedSymbols.cpp RtedTransf_registerType.cpp \
                       RtedTransf_array.cpp RtedTransf_prolog.cpp \
                       RtedTransf_support.cpp RtedTransf_funccall.cpp \
                       RtedTransf_locals.cpp RtedTransf_scopestmt.cpp \
                       RtedTransf_funcdef.cpp RtedTransf_variable.cpp \
										   RtedTransf_returnStmt.cpp RtedTransf_Upc.cpp \
                       RtedTransf_iofunccall.cpp RtedTransf_operators.cpp \
                       RtedTransf_copyClass.cpp \
		       RtedTransformation.h \
		       RtedSymbols.h \
		       DataStructures.h \
		       RuntimeSystem_ParsingWorkaround.h \
		       RtedVisit_Variable.cpp RtedVisit.h rosez.hpp

# \pp included: RuntimeSystem.h RuntimeSystem.cpp



SUBDIRS = CppRuntimeSystem

# \pp was: runtimeCheck_LDADD =  $(ROSE_LIBS) CppRuntimeSystem/libCppRuntimeSystem.la
runtimeCheck_LDADD =  $(ROSE_LIBS)

runtimeCheck.o: runtimeCheck.cpp

ROSE_FLAGS = -rose:verbose 0

TEST_TRANSLATOR = ./runtimeCheck all $(ROSE_FLAGS)
TESTCODE_INCLUDES =


CLEANFILES = *.out *.ti *.pdf test_runtime* *.txt rose_test_c_E_1_1_a

RTED_C_LOCAL_TEST_DIRS = $(srcdir)/tests/C/panasTests \
			 $(srcdir)/tests/C/memoryleaks \
			 $(srcdir)/tests/C/memoverlap \
			 $(srcdir)/tests/C/pointer \
			 $(srcdir)/tests/C/types

RTED_CPP_LOCAL_TEST_DIRS = 	 $(srcdir)/tests/Cxx/io \
				 $(srcdir)/tests/Cxx/memory \
				 $(srcdir)/tests/Cxx/overloading \
				 $(srcdir)/tests/Cxx/pointer \
				 $(srcdir)/tests/Cxx/simple \
				 $(srcdir)/tests/Cxx/types

#				 $(srcdir)/tests/Cxx/templates # temporarily removed b/c unparsed code
#                                      # cannot be fed into GCC4.3

RTED_UPC_LOCAL_TEST_DIRS = $(srcdir)/tests/UPC/simple/simple

if ROSE_USE_RTED
RTED_C_TEST_DIRS = $(rted_path)/C/array_index_out_of_bound \
		 $(rted_path)/C/string_errors \
		 $(rted_path)/C/subprogram_call_errors \
		 $(rted_path)/C/allocation_deallocation_errors \
		 $(rted_path)/C/input_output_errors \
		 $(rted_path)/C/memory_leaks \
		 $(rted_path)/C/uninitialized_variables \
		 $(rted_path)/C/C99_specific_errors \
		 $(rted_path)/C/pointer_errors \
		 $(RTED_C_LOCAL_TEST_DIRS)

#		 $(rted_path)/C/floating_point_errors \ \pp excluded from tests b/c do not work

RTED_CPP_TEST_DIRS = $(rted_path)/C++/array_index_out_of_bound \
		 $(rted_path)/C++/string_errors \
		 $(rted_path)/C++/allocation_deallocation_errors \
		 $(rted_path)/C++/input_output_errors \
		 $(rted_path)/C++/memory_leaks \
		 $(rted_path)/C++/uninitialized_variables \
		 $(rted_path)/C++/floating_point_errors \
		 $(rted_path)/C++/pointer_errors \
		 $(RTED_CPP_LOCAL_TEST_DIRS)

RTED_UPC_TEST_DIRS = $(RTED_UPC_LOCAL_TEST_DIRS) \
	   $(rted_path)/UPC/C_Outofbnds \
		 $(rted_path)/UPC/D_Outofbnds \
		 $(rted_path)/UPC/G_Uninit \
		 $(rted_path)/UPC/H_Memoryerr \
		 $(rted_path)/UPC/xtra
#~	    $(rted_path)/UPC/E_Outofbnds  arrays as arguments to std function
#~      $(rted_path)/UPC/A_Deadlcks   not our task
#~      $(rted_path)/UPC/B_RacCond    not our task
#~      $(rted_path)/UPC/F_Argmterr   not yet handled
#~      $(rted_path)/UPC/I_Wronorder  not yet handled
#~      $(rted_path)/UPC/J_Nonportbl  not yet handled
#~      $(rted_path)/UPC/K_Warnings   not yet handled


else
RTED_C_TEST_DIRS = 		 $(RTED_C_LOCAL_TEST_DIRS)
RTED_CPP_TEST_DIRS =		 $(RTED_CPP_LOCAL_TEST_DIRS)
RTED_UPC_TEST_DIRS =             $(RTED_UPC_LOCAL_TEST_DIRS)
endif

RTED_TEST_DIRS=$(RTED_C_TEST_DIRS) $(RTED_CPP_TEST_DIRS) $(RTED_UPC_TEST_DIRS)

GENERATE_RTED_ORIGSOURCES = ${foreach RTED_TEST_DIR, $(RTED_TEST_DIRS), $(patsubst $(RTED_TEST_DIR)/%_s$(strip $(1)), , ${wildcard $(RTED_TEST_DIR)/*${strip $(1)}}) }
GENERATE_RTED_COPYSOURCES = ${foreach RTED_TEST_DIR, ${filter ${strip $(2)}%, $(RTED_TEST_DIRS)}, ${patsubst ${strip $(2)}/%${strip $(1)}, rted_source/%${strip $(1)}, ${wildcard $(RTED_TEST_DIR)/*${strip $(1)}} } }
GENERATE_RTED_ROSESOURCES = ${patsubst %${strip $(1)}, %_rose${strip $(1)}, $(2) }
GENERATE_RTED_BINFILES = ${patsubst ${strip $(2)}/%${strip $(1)}, rted_source/%.bin, ${filter ${strip $(2)}%, ${strip $(3)}}}
if ROSE_USE_RTED
GENERATE_RTED_OUTPUTFILES = ${patsubst $(rted_path)/%${strip $(1)}, rted_source/%.${strip $(2)}-output, ${strip $(3)}}
endif

RTED_ORIGSOURCES_C   = ${call GENERATE_RTED_ORIGSOURCES, .c}
RTED_ORIGSOURCES_CPP = ${call GENERATE_RTED_ORIGSOURCES, .cpp}
RTED_ORIGSOURCES_UPC = ${call GENERATE_RTED_ORIGSOURCES, .upc}
RTED_ORIGSOURCES = $(RTED_ORIGSOURCES_C) $(RTED_ORIGSOURCES_CPP) $(RTED_ORIGSOURCES_UPC)

if ROSE_USE_RTED
RTED_COPYSOURCES_GLOBAL_C = ${call GENERATE_RTED_COPYSOURCES, .c, $(rted_path)}
RTED_COPYSOURCES_GLOBAL_CPP = ${call GENERATE_RTED_COPYSOURCES, .cpp, $(rted_path)}
RTED_COPYSOURCES_GLOBAL_UPC = ${call GENERATE_RTED_COPYSOURCES, .upc, $(rted_path)}
RTED_COPYSOURCES_LOCAL_UPC  = ${call GENERATE_RTED_COPYSOURCES, .upc, $(srcdir)}
endif

RTED_COPYSOURCES_LOCAL_C  = ${call GENERATE_RTED_COPYSOURCES, .c, $(srcdir)}
RTED_COPYSOURCES_LOCAL_CPP  = ${call GENERATE_RTED_COPYSOURCES, .cpp, $(srcdir)}

RTED_COPYSOURCES_GLOBAL = $(RTED_COPYSOURCES_GLOBAL_C) $(RTED_COPYSOURCES_GLOBAL_CPP) $(RTED_COPYSOURCES_GLOBAL_UPC)
RTED_COPYSOURCES_LOCAL = $(RTED_COPYSOURCES_LOCAL_C) $(RTED_COPYSOURCES_LOCAL_CPP) $(RTED_COPYSOURCES_LOCAL_UPC)

RTED_COPYSOURCES_C = $(RTED_COPYSOURCES_LOCAL_C) $(RTED_COPYSOURCES_GLOBAL_C)
RTED_COPYSOURCES_CPP = $(RTED_COPYSOURCES_LOCAL_CPP) $(RTED_COPYSOURCES_GLOBAL_CPP)
RTED_COPYSOURCES_UPC = $(RTED_COPYSOURCES_LOCAL_UPC) $(RTED_COPYSOURCES_GLOBAL_UPC)
RTED_COPYSOURCES = $(RTED_COPYSOURCES_GLOBAL) $(RTED_COPYSOURCES_LOCAL)

RTED_ROSESOURCES  = ${call GENERATE_RTED_ROSESOURCES, .c, $(RTED_COPYSOURCES_C)}
RTED_ROSESOURCES += ${call GENERATE_RTED_ROSESOURCES, .cpp, $(RTED_COPYSOURCES_CPP)}
RTED_ROSESOURCES += ${call GENERATE_RTED_ROSESOURCES, .upc, $(RTED_COPYSOURCES_UPC)}

RTED_BINFILES = ${call GENERATE_RTED_BINFILES, .c, $(srcdir), $(RTED_ORIGSOURCES_C)}
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .cpp, $(srcdir), $(RTED_ORIGSOURCES_CPP)}
if ROSE_USE_RTED
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .c, $(rted_path), $(RTED_ORIGSOURCES_C)}
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .cpp, $(rted_path), $(RTED_ORIGSOURCES_CPP)}
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .upc, $(rted_path), $(RTED_ORIGSOURCES_UPC)}
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .upc, $(srcdir), $(RTED_ORIGSOURCES_UPC)}
endif

RTED_OUTPUTFILES  = ${call GENERATE_RTED_OUTPUTFILES, .c, actual, $(RTED_ORIGSOURCES_C)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .c, expected, $(RTED_ORIGSOURCES_C)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .cpp, actual, $(RTED_ORIGSOURCES_CPP)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .cpp, expected, $(RTED_ORIGSOURCES_CPP)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .upc, actual, $(RTED_ORIGSOURCES_UPC)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .upc, expected, $(RTED_ORIGSOURCES_UPC)}

.SECONDARY: $(RTED_BINFILES)$(RTED_COPYSOURCES) $(RTED_ROSESOURCES) $(RTED_OUTPUTFILES)
.PRECIOUS: $(RTED_BINFILES) $(RTED_COPYSOURCES) $(RTED_ROSESOURCES) $(RTED_OUTPUTFILES)

print:
	@echo $(RTED_TEST_DIRS)

if ROSE_USE_RTED
check-C-array: run/C/array_index_out_of_bound

check-C-string: run/C/string_errors

check-C-subprogram: run/C/subprogram_call_errors

check-C-allocation: run/C/allocation_deallocation_errors

check-C-inputoutput: run/C/input_output_errors

check-C-memoryleaks: run/C/memory_leaks

check-C-uninitvar: run/C/uninitialized_variables

check-C-C99specific: run/C/C99_specific_errors
endif

if ROSE_USE_RTED
rted_source:
	@echo "Copying all files .... "
	@cp -f $(srcdir)/index_array.txt .
	@cp -f $(srcdir)/file_read.txt .
	@cp -f $(srcdir)/upcparam.h .
	@cp -f $(srcdir)/upcparam_s.h .
	@cp -f $(srcdir)/upccrose .
	@mkdir -p $(foreach rted_dir,$(filter $(rted_path)%, $(RTED_TEST_DIRS) ), $(patsubst $(rted_path)/%, rted_source/%, $(rted_dir) ) )
	@mkdir -p $(foreach local_test_dir,$(filter $(srcdir)%, $(RTED_TEST_DIRS) ), $(patsubst $(srcdir)/%, rted_source/%, $(local_test_dir) ) )
else
rted_source:
	@echo "Copying all files .... "
	@mkdir -p $(foreach local_test_dir,$(filter $(srcdir)%, $(RTED_TEST_DIRS) ), $(patsubst $(srcdir)/%, rted_source/%, $(local_test_dir) ) )
endif

#------------------------------------------------------------------------------
# rule to copy the sources into our current working directory
#------------------------------------------------------------------------------

#testcases in $(top_srcdir)/projects/RTED/tests
rted_source/tests/%.c: rted_source $(top_srcdir)/projects/RTED/tests/%.c
	@echo -e Checking $(patsubst $(top_srcdir)/projects/RTED/tests/%.c, %, $(filter $(top_srcdir)/%, $^)) ...
	@echo -n "    copying input file (.c) ... "
	@cp $(filter $(top_srcdir)/%.c, $^) $(dir $@)
	@echo " done."
	@echo -n "    inserting #include \"RuntimeSystem.h\" ... "
	@sed -i '1i\#include \"RuntimeSystem.h\"' $@
	@echo " done."

#testcases in $(top_srcdir)/projects/RTED/tests
rted_source/tests/%.cpp: rted_source $(top_srcdir)/projects/RTED/tests/%.cpp
	@echo -e Checking $(patsubst $(top_srcdir)/projects/RTED/tests/%.cpp, %, $(filter $(top_srcdir)/%, $^)) ...
	@echo -n "    copying input file (.cpp) ... "
	@cp $(filter $(top_srcdir)/%.cpp, $^) $(dir $@)
	@echo " done."
	@echo -n "    inserting #include \"RuntimeSystem.h\" ... "
	@sed -i '1i\#include \"RuntimeSystem.h\"' $@
	@echo " done."

#testcases in $(top_srcdir)/projects/RTED/tests
rted_source/tests/%.upc: rted_source $(top_srcdir)/projects/RTED/tests/%.upc
	@echo -e Checking $(patsubst $(top_srcdir)/projects/RTED/tests/%.upc, %, $(filter $(top_srcdir)/%, $^)) ...
	@echo -n "    copying input file (.upc) ... $(filter $(top_srcdir)/%.upc, $^) $(dir $@) "
	@cp $(filter $(top_srcdir)/%.upc, $^) $(dir $@)
	@echo " done."
	@echo -n "    inserting #include \"RuntimeSystem.h\" ... "
	@sed -i '1i\#include \"RuntimeSystem.h\"' $@
	@echo " done."

#testcases in rted distribution

if ROSE_USE_RTED
COPYING_FILE = @SOURCE_FILE='$(strip $(patsubst rted_source/%, $(rted_path)/%, $(1)))';\
		if [ -f $$SOURCE_FILE ]; then\
			echo -n "    copying input file $(1)... ";\
			cp $$SOURCE_FILE $(dir $(1));\
			echo " done.";\
			echo -n "    inserting include \"RuntimeSystem.h\" ... ";\
			sed -i '1i\#include \"RuntimeSystem.h\"' $(1);\
			echo " done.";\
		else\
			echo -n "    not copying fake dependency $(1) ... ";\
		fi

# TODO 2: Remove this once the assert( sym != null ) issue is fixed.
COPYING_FILE_WITH_WORKAROUND = @SOURCE_FILE='$(strip $(patsubst rted_source/%, $(rted_path)/%, $(1)))';\
		if [ -f $$SOURCE_FILE ]; then\
			echo -n "    copying input file $(1)... ";\
			cp $$SOURCE_FILE $(dir $(1));\
			echo " done.";\
			echo -n "    inserting include \"RuntimeSystem_ParsingWorkaround.h\" ... ";\
			sed -i '1i\#include \"RuntimeSystem_ParsingWorkaround.h\"' $(1);\
			echo " done.";\
		else\
			echo -n "    not copying fake dependency $(1) ... ";\
		fi
endif

if ROSE_USE_RTED
rted_source/%_s.c: rted_source $(rted_path)/%_s.c
	$(call COPYING_FILE, $@)

rted_source/%_s.cpp: rted_source $(rted_path)/%_s.cpp
	$(call COPYING_FILE, $@)

rted_source/%_s.upc: rted_source $(rted_path)/%_s.upc
	$(call COPYING_FILE, $@)


rted_source/%.c: rted_source $(rted_path)/%.c
	$(call COPYING_FILE, $@)

rted_source/%.cpp: rted_source $(rted_path)/%.cpp
	$(call COPYING_FILE_WITH_WORKAROUND, $@)

rted_source/%.upc: rted_source $(rted_path)/%.upc
	$(call COPYING_FILE, $@)
endif

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create _rose.c and _s_rose.c files
#------------------------------------------------------------------------------

rted_source/%_s_rose.c: rted_source/%_s.c
	@touch $@

rted_source/%_s_rose.cpp: rted_source/%_s.cpp
	@touch $@

rted_source/%_s_rose.upc: rted_source/%_s.upc
	@touch $@


# parameters:
#  1: file extension (.c, .cpp, .f, ... )
#  2: compile command
#  3: rose option, for example: C_Only
TRANSFORMING_FILES = @echo -n "    testing if files compiles with gcc (with _s) ... $(patsubst %_rose$(strip $(1)), %.bin, $@)";

#TRANSFORMING_FILES += echo -n "$(2) -g -o $(patsubst %_rose$(strip $(1)), %.bin, $@) \
#		      $(filter rted_source/%$(strip $(1)), $^) \
#		      -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) \
#		      -I$(dir $(patsubst rted_source/%, $(srcdir)/%, $@)) \
#		      -I$(top_builddir) -I$(srcdir) $(RTED_LIB) ";

TRANSFORMING_FILES += $(2) \
		      -g \
		      -o $(patsubst %_rose$(strip $(1)), %.bin, $@) \
		      $(filter rted_source/%$(strip $(1)), $^)
if ROSE_USE_RTED
TRANSFORMING_FILES +=  -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@))
endif
TRANSFORMING_FILES +=  -I$(dir $(patsubst rted_source/%, $(srcdir)/%, $@)) \
		      -I$(top_builddir) -I$(srcdir) $(RTED_LIB) > $@.compile.out 2> $@.compile.err\
			  || { cat $@.compile.err; false; };


TRANSFORMING_FILES += if [ $$? -ne 0 ]; then exit 1; fi;

TRANSFORMING_FILES += rm $(patsubst %_rose$(strip $(1)), %.bin, $@);
TRANSFORMING_FILES += echo " done.";
TRANSFORMING_FILES += echo -n "    running transformation ... $(patsubst %_rose$(strip $(1)), %.bin, $@)";

TRANSFORMING_FILES += ./runtimeCheck $(filter rted_source/%$(strip $(1)), $^) \
				       -rose:$(strip $(3)) \
				       -c \
				       $(TESTSUITEINCL) \
				       -I$(top_builddir) \
				       -I$(srcdir)
if ROSE_USE_RTED
TRANSFORMING_FILES +=  -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@))
endif
TRANSFORMING_FILES +=  -I$(dir $(patsubst rted_source/%, $(srcdir)/%, $@)) \
					   > $@.runtimeCheck.out 2> $@.runtimeCheck.err\
					   || { cat $@.runtimeCheck.err; false; };
TRANSFORMING_FILES += if [ $$? -ne 0 ]; then exit 1; fi;

TRANSFORMING_FILES += echo " done.";

#was:TRANSFORMING_FILES += echo -n "   adding line information ... ";
#was:TRANSFORMING_FILES += cat rose_$(notdir $(patsubst %_rose$(strip $(1)), %$(strip $(1)), $@ ) ) | \
#was:			  nl -p -b 'a' | \
#was:			  sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $@;
# \pp \todo remove the cat and write previous output already to $@
TRANSFORMING_FILES += cat rose_$(notdir $(patsubst %_rose$(strip $(1)), %$(strip $(1)), $@ ) ) > $@;

TRANSFORMING_FILES += rm rose_$(notdir $(patsubst %_rose$(strip $(1)), %$(strip $(1)), $@ ) );

# \pp \todo modify along the line of the previous modifications
TRANSFORMING_FILES += SUPPORT_FILE='rose_$(notdir $(patsubst %_rose$(strip $(1)), %_s$(strip $(1)), $@ ) )'; \
		      if [ -f $$SUPPORT_FILE ]; \
		      then \
			cat $$SUPPORT_FILE | \
			    nl -p -b 'a' | \
			    sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $(patsubst %_rose$(strip $(1)), %_s_rose$(strip $(1)), $@ ); \
		      fi;

TRANSFORMING_FILES += rm -f rose_$(notdir $(patsubst %_rose$(strip $(1)), %_s$(strip $(1)), $@ ) );
TRANSFORMING_FILES += echo " done.";

rted_source/%_rose.c: runtimeCheck rted_source/%.c rted_source/%_s.c $(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .c, $(RTED_CC), C_Only)

rted_source/%_rose.cpp: runtimeCheck rted_source/%.cpp rted_source/%_s.cpp $(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .cpp, $(RTED_CXX), Cxx)

rted_source/%_rose.upc: runtimeCheck rted_source/%.upc rted_source/%_s.upc $(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .upc, $(RTED_UPC), UPC -DWITH_UPC $(UPC_INCLUDES))


rted_source/%_rose.c: runtimeCheck rted_source/%.c $(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .c, $(RTED_CC), C_Only)

rted_source/%_rose.cpp: runtimeCheck rted_source/%.cpp $(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .cpp, $(RTED_CXX), Cxx)

rted_source/%_rose.upc: runtimeCheck rted_source/%.upc $(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .upc, $(RTED_UPC), UPC -DWITH_UPC $(UPC_INCLUDES))

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create the binary files
#------------------------------------------------------------------------------

# parameters:
#  1: file extension (.c, .cpp, .f, ... )
#  2: compile command
#  3: additional parameters, like linking against debugger ...
#COMPILING_RUNTIME_SYSTEM =  @echo -n "$(2) -g -o $@ $(filter rted_source/%$(strip $(1)), $^) \
#				      $(srcdir)/$(RTS) \
#				      $(INCLUDES) \
#				      -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) \
#	 		              -I$(dir $(patsubst rted_source/%, $(srcdir)/%, $@)) \
#				      -I$(top_builddir) \
#				      -I$(srcdir) \
#				      $(3) $(RTED_LIB)";
COMPILING_RUNTIME_SYSTEM = @echo " !>>>>>  compiling instrumented debugger code (with _s) ... $(filter rted_source/%$(strip $(1)), $^)"; \
			   $(2) -g -o $@ $(filter rted_source/%$(strip $(1)), $^) \
				 $(TESTSUITEINCL)
if ROSE_USE_RTED
COMPILING_RUNTIME_SYSTEM +=    -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@))
endif
COMPILING_RUNTIME_SYSTEM +=    -I$(dir $(patsubst rted_source/%, $(srcdir)/%, $@)) \
				      -I$(top_builddir) \
				      -I$(srcdir) \
				      $(3) $(4) $(5) \
					    > $@.compile.out 2> $@.compile.err || { cat $@.compile.err; false; }; \
			   if [ $$? -ne 0 ]; then exit 1; fi; \
			   echo -n " done.";

RuntimeSystemCxx.o: $(srcdir)/RuntimeSystem.cpp $(srcdir)/RuntimeSystem.h
	$(CXX) -g -Wall -Wextra -o $@ -c $<

RuntimeSystemUpc.o: $(srcdir)/RuntimeSystem.cpp $(srcdir)/RuntimeSystem.h
	$(CXX) -g -Wall -Wextra -DWITH_UPC=1 -o $@ -c $<

ParallelRTS.o: $(srcdir)/ParallelRTS.upc $(srcdir)/ParallelRTS.h
	$(UPC) -g -Wall -Wextra -DIN_TARGET_LIBS -DWITH_UPC=1 -o $@ -c $<

rted_source/%.bin: rted_source/%_rose.c rted_source/%_s_rose.c $(RTSCXX) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC), $(RTED_LIB), $(RTSCXX))

rted_source/%.bin: rted_source/%_rose.c $(RTSCXX) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC), $(RTED_LIB), $(RTSCXX))

rted_source/%.bin: rted_source/%_rose.cpp rted_source/%_s_rose.cpp $(RTSCXX) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX), $(RTED_LIB), $(RTSCXX))

rted_source/%.bin: rted_source/%_rose.cpp $(RTSCXX) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX), $(RTED_LIB), $(RTSCXX))

rted_source/%.bin: rted_source/%_rose.upc rted_source/%_s_rose.upc $(RTSUPC) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .upc, $(RTED_UPC), $(RTED_UPC_LIB), $(RTSUPC))

rted_source/%.bin: rted_source/%_rose.upc $(RTSUPC) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .upc, $(RTED_UPC), $(RTED_UPC_LIB), $(RTSUPC))

# TODO: make this aware of different file extensions ...

# parameters:
#  1: file extension (.c, .cpp, .f, ... )
#  2: location to search ( $(srcdir), $(rted_path), ... )
GENERATE_EXPECTED_OUTPUT = @OUTPUT_SRC='$(strip $(patsubst rted_source/%.expected-output, $(strip $(2))/%.ok_msg, $@))';\
			   TEST_FILE='$(strip $(patsubst rted_source/%.expected-output, $(strip $(2))/%$(strip $(1)), $@))';\
			   if [ -f "$$OUTPUT_SRC" ]; then \
				echo ===`wc -l $$TEST_FILE`=== | $(EXTRACT_EXPECTED_ERROR) - $$OUTPUT_SRC | head -n 1 > $@;\
			   fi
if ROSE_USE_RTED
rted_source/C/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .c, $(rted_path))

rted_source/C++/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .cpp, $(rted_path))

rted_source/UPC/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .upc, $(rted_path))
endif

rted_source/tests/C/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .c, $(srcdir) )

rted_source/tests/Cxx/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .cpp, $(srcdir))

rted_source/tests/UPC/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .upc, $(srcdir))

#@OUTPUT_SRC='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%.ok_msg, $@))';\
#TEST_FILE='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%, $@))';\
#if [ -f "$$OUTPUT_SRC" ]; then \
#echo ===`wc -l $$TEST_FILE`=== | $(EXTRACT_EXPECTED_ERROR) - $$OUTPUT_SRC > $@;\
#fi
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# rules to run arbitrary tests from the rted distribution
#------------------------------------------------------------------------------

run/%: rted_source/%.bin rted_source/%.expected-output
	@echo -n "   running runtime error detection ... $(filter %.bin, $?)"
	@OUTPUT_SRC='$(strip $(filter %.expected-output, $?))';\
		if [ -f "$$OUTPUT_SRC" ]; then\
			RESULT='$(strip $(patsubst %.bin, %.result.txt, $(filter rted_source/%.bin, $?)))';\
			export LD_LIBRARY_PATH=$(top_builddir)/projects/RTED/CppRuntimeSystem/.libs/:${LD_LIBRARY_PATH};\
			./$(filter rted_source/%.bin, $?) 2> $$RESULT;\
			if [ $$? -ne 0 ];then cat $$RESULT && echo "Test exited unexpectedly with non-zero status" && exit 1; fi;\
			ACTUAL_OUTPUT='./$(strip $(patsubst %.bin, %.actual-output, $(filter rted_source/%.bin, $?)))';\
			if [ ! -f $$RESULT ]; then echo "no result produced!" && exit 1; fi; \
			$(EXTRACT_ACTUAL_ERROR) $$RESULT > $$ACTUAL_OUTPUT;\
			diff --ignore-all-space --ignore-case -q $$ACTUAL_OUTPUT $(filter %.expected-output, $?) > /dev/null;\
			if [ $$? -ne 0 ]; then\
				echo -e '\n\n';\
				echo 'Expected error did not match actual error found';\
				echo ' (the error may be in the extraction scripts -- see the actual files that were diff-d';\
				echo "  $(filter %.expected-output, $?) $$ACTUAL_OUTPUT)";\
				echo -e '\n----------- Expected Error ------------';\
				echo "(remember to subtract 1 from actual line numbers (for non-instrumented source) to compensate for added lines)";\
				RTED_OKMSG="$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%.ok_msg, $(filter %.expected-output, $?)))";\
				LOCAL_OKMSG="$(strip $(patsubst rted_source/%.expected-output, $(srcdir)/%.ok_msg, $(filter %.expected-output, $?)))";\
				if [ -f "$$RTED_OKMSG" ]; then cat $$RTED_OKMSG;\
				elif [ -f "$$LOCAL_OKMSG" ]; then cat $$LOCAL_OKMSG;\
				fi;\
				echo -e '\n----------- Actual   Error ------------';\
				cat $$RESULT;\
				exit 1;\
			fi;\
		else\
			./$(filter rted_source/%.bin, $?);\
		fi;
	@echo " done."

# was: upcrun -quiet -n 4 ./$(filter rted_source/%.bin, $?) 2> $$RESULT;
#			./$(filter rted_source/%.bin, $?) -n 4 2> $$RESULT;

runupc/%: rted_source/%.bin rted_source/%.expected-output
	echo "   running (UPC) runtime error detection ... $(filter %.bin, $?) -n 4"
	OUTPUT_SRC='$(strip $(filter %.expected-output, $?))';\
		if [ -f "$$OUTPUT_SRC" ]; then\
			RESULT='$(strip $(patsubst %.bin, %.result.txt, $(filter rted_source/%.bin, $?)))';\
			./$(filter rted_source/%.bin, $?) -n 4 2> $$RESULT;\
			if [ $$? -ne 0 ];then cat $$RESULT && echo "Test exited unexpectedly with non-zero status" && exit 1; fi;\
			ACTUAL_OUTPUT='./$(strip $(patsubst %.bin, %.actual-output, $(filter rted_source/%.bin, $?)))';\
			if [ ! -f $$RESULT ]; then echo "no result produced!" && exit 1; fi; \
			$(EXTRACT_ACTUAL_ERROR) $$RESULT > $$ACTUAL_OUTPUT;\
			diff --ignore-all-space --ignore-case -q $$ACTUAL_OUTPUT $(filter %.expected-output, $?) > /dev/null;\
			if [ $$? -ne 0 ]; then\
				echo -e '\n\n';\
				echo 'Expected error did not match actual error found';\
				echo ' (the error may be in the extraction scripts -- see the actual files that were diff-d';\
				echo "  $(filter %.expected-output, $?) $$ACTUAL_OUTPUT)";\
				echo -e '\n----------- Expected Error ------------';\
				echo "(remember to subtract 1 from actual line numbers (for non-instrumented source) to compensate for added lines)";\
	        		RTED_OKMSG="$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%.ok_msg, $(filter %.expected-output, $?)))";\
			        LOCAL_OKMSG="$(strip $(patsubst rted_source/%.expected-output, $(srcdir)/%.ok_msg, $(filter %.expected-output, $?)))";\
				if [ -f "$$RTED_OKMSG" ]; then cat $$RTED_OKMSG;\
				elif [ -f "$$LOCAL_OKMSG" ]; then cat $$LOCAL_OKMSG;\
				fi;\
				echo -e '\n----------- Actual   Error ------------';\
				cat $$RESULT;\
				echo "$<" >>badlist.txt; \
			else\
			  echo "$<" >>goodlist.txt; \
			fi;\
		else\
			./$(filter rted_source/%.bin, $?) -n 4;\
		fi;
	@echo " done."

run_log/%: rted_source/%.bin rted_source/%.expected-output
	@echo "   logging results for runtime error detection ... $(filter %.bin, $?)"
	@OUTPUT_SRC='$(strip $(filter %.expected-output, $?))';\
		if [ -f "$$OUTPUT_SRC" ]; then\
			RESULT='$(strip $(patsubst %.bin, %.result.txt, $(filter rted_source/%.bin, $?)))';\
			./$(filter rted_source/%.bin, $?) 2> $$RESULT;\
			if [ $$? -ne 0 ];then cat $$RESULT && echo "Test exited unexpectedly with non-zero status" && exit 1; fi;\
			ACTUAL_OUTPUT='./$(strip $(patsubst %.bin, %.actual-output, $(filter rted_source/%.bin, $?)))';\
			if [ ! -f $$RESULT ]; then echo "no result produced!" && exit 1; fi; \
			$(EXTRACT_ACTUAL_ERROR) $$RESULT > $$ACTUAL_OUTPUT;\
			diff --ignore-all-space --ignore-case -q $$ACTUAL_OUTPUT $(filter %.expected-output, $?) > /dev/null;\
			if [ $$? -ne 0 ]; then\
				echo " $@ failed" >> test_log.txt;\
			else \
				echo " $@ passed" >> test_log.txt;\
			fi;\
		else\
			./$(filter rted_source/%.bin, $?);\
		fi;
	@echo " done."

# was:			upcrun -n 4 ./$(filter rted_source/%.bin, $?) 2> $$RESULT;

runupc_log/%: rted_source/%.bin rted_source/%.expected-output
	@echo "   logging results for runtime error detection ... $(filter %.bin, $?)"
	@OUTPUT_SRC='$(strip $(filter %.expected-output, $?))';\
		if [ -f "$$OUTPUT_SRC" ]; then\
			RESULT='$(strip $(patsubst %.bin, %.result.txt, $(filter rted_source/%.bin, $?)))';\
			./$(filter rted_source/%.bin, $?) -n 4 2> $$RESULT;\
			if [ $$? -ne 0 ];then cat $$RESULT && echo "Test exited unexpectedly with non-zero status" && exit 1; fi;\
			ACTUAL_OUTPUT='./$(strip $(patsubst %.bin, %.actual-output, $(filter rted_source/%.bin, $?)))';\
			if [ ! -f $$RESULT ]; then echo "no result produced!" && exit 1; fi; \
			$(EXTRACT_ACTUAL_ERROR) $$RESULT > $$ACTUAL_OUTPUT;\
			diff --ignore-all-space --ignore-case -q $$ACTUAL_OUTPUT $(filter %.expected-output, $?) > /dev/null;\
			if [ $$? -ne 0 ]; then\
				echo " $@ failed" >> test_log.txt;\
			else \
				echo " $@ passed" >> test_log.txt;\
			fi;\
		else\
			./$(filter rted_source/%.bin, $?);\
		fi;
	@echo " done."

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run all tests in the directories listed in RTED_ORIGSOURCES
#------------------------------------------------------------------------------

if ROSE_USE_RTED
RTED_ORIGSOURCES_GLOBAL_C = ${filter $(rted_path)%, $(RTED_ORIGSOURCES_C)}
RTED_ORIGSOURCES_GLOBAL_CPP = ${filter $(rted_path)%, $(RTED_ORIGSOURCES_CPP)}
RTED_ORIGSOURCES_GLOBAL_UPC = ${filter $(rted_path)%, $(RTED_ORIGSOURCES_UPC)}
RTED_ORIGSOURCES_LOCAL_UPC = ${filter $(srcdir)%, $(RTED_ORIGSOURCES_UPC)}
endif
RTED_ORIGSOURCES_LOCAL_C = ${filter $(srcdir)%, $(RTED_ORIGSOURCES_C)}
RTED_ORIGSOURCES_LOCAL_CPP = ${filter $(srcdir)%, $(RTED_ORIGSOURCES_CPP)}


GENERATE_RTED_RUN_TARGET = ${patsubst ${strip $(4)}/%${strip $(1)}, ${strip $(2)}/%, $(3)}
if ROSE_USE_RTED
RUN_TARGETS_GLOBAL_C  = ${call GENERATE_RTED_RUN_TARGET, .c, run, $(RTED_ORIGSOURCES_GLOBAL_C), $(rted_path)}
RUN_TARGETS_GLOBAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run, $(RTED_ORIGSOURCES_GLOBAL_CPP), $(rted_path)}
endif
RUN_TARGETS_LOCAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run, $(RTED_ORIGSOURCES_LOCAL_C), $(srcdir)}
RUN_TARGETS_LOCAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run, $(RTED_ORIGSOURCES_LOCAL_CPP), $(srcdir)}

if ROSE_USE_RTED
RUN_TARGETS_GLOBAL_UPC = ${call GENERATE_RTED_RUN_TARGET, .upc, runupc, $(RTED_ORIGSOURCES_GLOBAL_UPC), $(rted_path)}
endif
RUN_TARGETS_LOCAL_UPC = ${call GENERATE_RTED_RUN_TARGET, .upc, runupc, $(RTED_ORIGSOURCES_LOCAL_UPC), $(srcdir)}

if ROSE_USE_RTED
RUN_LOG_TARGETS_GLOBAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run_log, $(RTED_ORIGSOURCES_GLOBAL_C), $(rted_path)}
RUN_LOG_TARGETS_GLOBAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_log, $(RTED_ORIGSOURCES_GLOBAL_CPP), $(rted_path)}
RUN_LOG_TARGETS_GLOBAL_UPC = ${call GENERATE_RTED_RUN_TARGET, .upc, runupc_log, $(RTED_ORIGSOURCES_GLOBAL_UPC), $(rted_path)}
RUN_LOG_TARGETS_LOCAL_UPC = ${call GENERATE_RTED_RUN_TARGET, .upc, runupc_log, $(RTED_ORIGSOURCES_LOCAL_UPC), $(srcdir)}
endif
RUN_LOG_TARGETS_LOCAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run_log, $(RTED_ORIGSOURCES_LOCAL_C), $(srcdir)}
RUN_LOG_TARGETS_LOCAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_log, $(RTED_ORIGSOURCES_LOCAL_CPP), $(srcdir)}


run: $(RUN_TARGETS_GLOBAL_C) $(RUN_TARGETS_LOCAL_C) $(RUN_TARGETS_GLOBAL_CPP) $(RUN_TARGETS_LOCAL_CPP)

runupc:  $(RUN_TARGETS_GLOBAL_UPC) $(RUN_TARGETS_LOCAL_UPC)

run_log: $(RUN_LOG_TARGETS_GLOBAL_C) $(RUN_LOG_TARGETS_LOCAL_C) $(RUN_LOG_TARGETS_GLOBAL_CPP) $(RUN_LOG_TARGETS_LOCAL_CPP)

runupc_log: $(RUN_LOG_TARGETS_GLOBAL_UPC) $(RUN_LOG_TARGETS_LOCAL_UPC)

run_log/c:		$(RUN_LOG_TARGETS_GLOBAL_C)

run_log/cpp:	$(RUN_LOG_TARGETS_GLOBAL_CPP)

#run_log/upc:	$(RUN_LOG_TARGETS_GLOBAL_UPC)

test:
	@echo $(RTED_ORIGSOURCES_GLOBAL_CPP)

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run all tests in a directory listed in RTED_ORIGSOURCES
#------------------------------------------------------------------------------

define RUN_TARGET_C
$(1): $(filter $(1)%, $(RUN_TARGETS_GLOBAL_C)) $(filter $(1)%, $(RUN_TARGETS_LOCAL_C))
	@echo $(1) complete
endef

define RUN_LOG_TARGET_C
$(1): $(filter $(1)%, $(RUN_LOG_TARGETS_GLOBAL_C))
	@echo $(1) complete
endef

define RUN_TARGET_CPP
$(1): $(filter $(1)%, $(RUN_TARGETS_GLOBAL_CPP)) $(filter $(1)%, $(RUN_TARGETS_LOCAL_CPP))
	@echo $(1) complete
endef

define RUN_LOG_TARGET_CPP
$(1): $(filter $(1)%, $(RUN_LOG_TARGETS_GLOBAL_CPP))
	@echo $(1) complete
endef

define RUN_TARGET_UPC
$(1): $(filter $(1)%, $(RUN_TARGETS_GLOBAL_UPC)) $(filter $(1)%, $(RUN_TARGETS_LOCAL_UPC))
	@echo $(1) complete
endef

define RUN_LOG_TARGET_UPC
$(1): $(filter $(1)%, $(RUN_LOG_TARGETS_GLOBAL_UPC))
	@echo $(1) complete
endef

if ROSE_USE_RTED
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_C_TEST_DIRS)}, ${eval ${call RUN_TARGET_C, ${patsubst $(rted_path)%, run%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_CPP_TEST_DIRS)}, ${eval ${call RUN_TARGET_CPP, ${patsubst $(rted_path)%, run%, $(rted_dir)}}}}

${foreach rted_dir, ${filter $(rted_path)%, $(RTED_UPC_TEST_DIRS)}, ${eval ${call RUN_TARGET_UPC, ${patsubst $(rted_path)%, runupc%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(srcdir)%, $(RTED_UPC_TEST_DIRS)}, ${eval ${call RUN_TARGET_UPC, ${patsubst $(srcdir)%, runupc%, $(rted_dir)}}}}
endif
${foreach rted_dir, ${filter $(srcdir)%, $(RTED_C_TEST_DIRS)}, ${eval ${call RUN_TARGET_C, ${patsubst $(srcdir)%, run%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(srcdir)%, $(RTED_CPP_TEST_DIRS)}, ${eval ${call RUN_TARGET_CPP, ${patsubst $(srcdir)%, run%, $(rted_dir)}}}}

if ROSE_USE_RTED
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_C_TEST_DIRS)}, ${eval ${call RUN_LOG_TARGET_C, ${patsubst $(rted_path)%, run_log%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_CPP_TEST_DIRS)}, ${eval ${call RUN_LOG_TARGET_CPP, ${patsubst $(rted_path)%, run_log%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_UPC_TEST_DIRS)}, ${eval ${call RUN_LOG_TARGET_UPC, ${patsubst $(rted_path)%, runupc_log%, $(rted_dir)}}}}
endif

#------------------------------------------------------------------------------




#------------------------------------------------------------------------------
# Tests that are known to fail
#------------------------------------------------------------------------------

# upc:c_G_4_a test RTED does not catch invalid assignment
#                  and RTED/UPC does not catch negative numbers passed as affinity expr
# upc:c_G_2_g ptr reads are not tested when casted
# upc:c_G_6_i ptr reads are not tested when evaluated as binary operations
# upc:c_G_6_i ptr reads are not tested when evaluated as binary operations
# upc:c_H_5_b helper file (_s) declares its own variable; RTED adds the
#             CreateVariable call to the first function
# upc:c_H_5_d see upc:c_H_5_b
# upc:c_H_5_f see upc:c_H_5_b


# TODO 2: Handle these tests
TESTS_KNOWN_TO_FAIL = run/C/subprogram_call_errors/c_C_1_3_f \
					  run/C++/allocation_deallocation_errors/C_G_2_1_g_d \
					  run/C++/allocation_deallocation_errors/C_G_2_1_h_ld \
					  run/C++/allocation_deallocation_errors/C_G_2_3_a_d \
					  run/C++/allocation_deallocation_errors/C_G_2_4_a_d \
					  run/C++/allocation_deallocation_errors/C_G_4_1_o_f \
					  run/C++/array_index_out_of_bound/C_E_1_7_a_o \
					  run/C++/array_index_out_of_bound/C_E_1_7_i_f \
					  run/C++/array_index_out_of_bound/C_E_1_7_i_sh \
					  run/C++/memory_leaks/C_I_2_2_a_d \
					  run/C++/memory_leaks/C_I_2_5_a_d \
					  run/C++/memory_leaks/C_I_2_6_a_d \
					  run/C++/memory_leaks/C_I_2_6_b_o \
					  run/C++/pointer_errors/C_J_1_4_a_d \
					  run/C++/pointer_errors/C_J_2_1_a_d \
					  run/C++/uninitialized_variables/C_A_1_1_c_uc \
					  run/C++/uninitialized_variables/C_A_1_2_a_d \
					  run/C++/uninitialized_variables/C_A_1_2_a_i \
					  run/C++/uninitialized_variables/C_A_1_3_a_d \
					  run/C++/uninitialized_variables/C_A_1_3_a_i \
					  run/C++/uninitialized_variables/C_A_2_1_b_uc \
					  run/C++/uninitialized_variables/C_A_3_1_c_uc \
					  run/C++/uninitialized_variables/C_A_3_2_c_uc \
					  run/C++/input_output_errors/C_H_1_2_a_fs \
					  run/C++/input_output_errors/C_H_1_2_a_ifs \
					  run/C++/input_output_errors/C_H_1_3_a_fs \
					  run/C++/input_output_errors/C_H_2_2_a_fs \
					  run/C++/input_output_errors/C_H_2_2_a_ofs \
					  run/C++/input_output_errors/C_H_2_3_a_fs \
					  run/C++/input_output_errors/C_H_3_3_a_fs \
					  run/C++/input_output_errors/C_H_3_3_a_ifs \
					  run/C++/input_output_errors/C_H_3_4_a_fs \
					  run/C++/input_output_errors/C_H_3_4_a_ofs \
					  run/tests/C/memoryleaks/assignment_scope_switch \
					  run/tests/C/memoryleaks/assignment_to_locals_then_globals_then_local \
					  run/tests/C/memoryleaks/assignment_to_fn_malloc_novar \
					  run/tests/C/memoverlap/heap_array_in_struct \
					  run/tests/C/memoverlap/heap_array_in_struct_in_array \
					  run/tests/C/pointer/arraytest \
					  run/tests/Cxx/overloading/array_index \
					  run/tests/C/types/union \
					  run/tests/Cxx/pointer/arrow_star \
					  run/tests/Cxx/pointer/C_F_3_3_a_i \
					  run/tests/Cxx/pointer/dot_star \
					  run/tests/Cxx/pointer/nulltest \
					  run/tests/UPC/simple/test \
					  run/tests/UPC/simple/simple \
					  run/tests/Cxx/pointer/arrow_varref2 \
					  run/C/uninitialized_variables/c_A_6_a_in \
					  run/C/uninitialized_variables/c_A_6_b_in \
					  run/C/uninitialized_variables/c_A_6_c_in \
					  run/C/uninitialized_variables/c_A_6_d_in \
					  run/C/uninitialized_variables/c_A_6_e_in \
					  run/C++/uninitialized_variables/c_A_6_a_in \
					  run/C++/uninitialized_variables/c_A_6_b_in \
					  run/C++/uninitialized_variables/c_A_6_c_in \
					  run/C++/uninitialized_variables/c_A_6_d_in \
					  run/C++/uninitialized_variables/c_A_6_e_in \
					  runupc/UPC/G_Uninit/c_G_4_a \
					  runupc/UPC/G_Uninit/c_G_4_c \
            runupc/UPC/G_Uninit/c_G_2_g_D \
            runupc/UPC/G_Uninit/c_G_2_g_I \
            runupc/UPC/G_Uninit/c_G_2_g_L \
            runupc/UPC/G_Uninit/c_G_2_g_F \
            runupc/UPC/G_Uninit/c_G_2_g_S \
            runupc/UPC/G_Uninit/c_G_2_h_D \
            runupc/UPC/G_Uninit/c_G_2_h_I \
            runupc/UPC/G_Uninit/c_G_2_h_L \
            runupc/UPC/G_Uninit/c_G_2_h_F \
            runupc/UPC/G_Uninit/c_G_2_h_S \
            runupc/UPC/G_Uninit/c_G_6_i \
            runupc/UPC/G_Uninit/c_G_6_j \
            runupc/UPC/H_Memoryerr/c_H_5_b \
            runupc/UPC/H_Memoryerr/c_H_5_d \
            runupc/UPC/H_Memoryerr/c_H_5_f \
					  TheEnd_KNOWN_TO_FAIL

# These tests do not compile
#   most likely because a function's result value is cast
#   to shared (as in "int shared * shared", where a stack value -the pointer-
#   cannot be shared)
TEST_HAS_COMPILE_ERROR = runupc/UPC/G_Uninit/c_G_1_b_D \
                         runupc/UPC/G_Uninit/c_G_1_b_I \
                         runupc/UPC/G_Uninit/c_G_1_b_L \
                         runupc/UPC/G_Uninit/c_G_1_b_S \
                         runupc/UPC/G_Uninit/c_G_1_b_F \
                         runupc/UPC/G_Uninit/c_G_1_e_D \
                         runupc/UPC/G_Uninit/c_G_1_e_I \
                         runupc/UPC/G_Uninit/c_G_1_e_L \
                         runupc/UPC/G_Uninit/c_G_1_e_S \
                         runupc/UPC/G_Uninit/c_G_1_e_F \
                         runupc/UPC/G_Uninit/c_G_1_f_D \
                         runupc/UPC/G_Uninit/c_G_1_f_I \
                         runupc/UPC/G_Uninit/c_G_1_f_L \
                         runupc/UPC/G_Uninit/c_G_1_f_S \
                         runupc/UPC/G_Uninit/c_G_1_f_F \
                         runupc/UPC/G_Uninit/c_G_3_a_D \
                         runupc/UPC/G_Uninit/c_G_3_a_I \
                         runupc/UPC/G_Uninit/c_G_3_a_L \
                         runupc/UPC/G_Uninit/c_G_3_a_S \
                         runupc/UPC/G_Uninit/c_G_3_a_F \
                         runupc/UPC/G_Uninit/c_G_3_b_D \
                         runupc/UPC/G_Uninit/c_G_3_b_I \
                         runupc/UPC/G_Uninit/c_G_3_b_L \
                         runupc/UPC/G_Uninit/c_G_3_b_S \
                         runupc/UPC/G_Uninit/c_G_3_b_F \
                         runupc/UPC/G_Uninit/c_G_3_c_D \
                         runupc/UPC/G_Uninit/c_G_3_c_I \
                         runupc/UPC/G_Uninit/c_G_3_c_L \
                         runupc/UPC/G_Uninit/c_G_3_c_S \
                         runupc/UPC/G_Uninit/c_G_3_c_F \
                         runupc/UPC/G_Uninit/c_G_3_d_D \
                         runupc/UPC/G_Uninit/c_G_3_d_I \
                         runupc/UPC/G_Uninit/c_G_3_d_L \
                         runupc/UPC/G_Uninit/c_G_3_d_S \
                         runupc/UPC/G_Uninit/c_G_3_d_F \
                         runupc/UPC/G_Uninit/c_G_3_e_D \
                         runupc/UPC/G_Uninit/c_G_3_e_I \
                         runupc/UPC/G_Uninit/c_G_3_e_L \
                         runupc/UPC/G_Uninit/c_G_3_e_S \
                         runupc/UPC/G_Uninit/c_G_3_e_F \
                         runupc/UPC/G_Uninit/c_G_3_f_D \
                         runupc/UPC/G_Uninit/c_G_3_f_I \
                         runupc/UPC/G_Uninit/c_G_3_f_L \
                         runupc/UPC/G_Uninit/c_G_3_f_S \
                         runupc/UPC/G_Uninit/c_G_3_f_F \
                         runupc/UPC/G_Uninit/c_G_6_a \
                         runupc/UPC/C_Outofbnds/c_C_10_2_c_F \
                         runupc/UPC/C_Outofbnds/c_C_10_2_c_I \
                         runupc/UPC/C_Outofbnds/c_C_10_2_c_L \
                         runupc/UPC/C_Outofbnds/c_C_10_2_c_D \
                         runupc/UPC/C_Outofbnds/c_C_10_2_c_S \
                         runupc/UPC/C_Outofbnds/c_C_10_2_d_F \
                         runupc/UPC/C_Outofbnds/c_C_10_2_d_I \
                         runupc/UPC/C_Outofbnds/c_C_10_2_d_L \
                         runupc/UPC/C_Outofbnds/c_C_10_2_d_D \
                         runupc/UPC/C_Outofbnds/c_C_10_2_d_S \
                         runupc/UPC/C_Outofbnds/c_C_10_4_c_F \
                         runupc/UPC/C_Outofbnds/c_C_10_4_c_I \
                         runupc/UPC/C_Outofbnds/c_C_10_4_c_L \
                         runupc/UPC/C_Outofbnds/c_C_10_4_c_D \
                         runupc/UPC/C_Outofbnds/c_C_10_4_c_S \
                         runupc/UPC/C_Outofbnds/c_C_10_4_d_F \
                         runupc/UPC/C_Outofbnds/c_C_10_4_d_I \
                         runupc/UPC/C_Outofbnds/c_C_10_4_d_L \
                         runupc/UPC/C_Outofbnds/c_C_10_4_d_D \
                         runupc/UPC/C_Outofbnds/c_C_10_4_d_S \
                         runupc/UPC/C_Outofbnds/c_C_11_2_c_F \
                         runupc/UPC/C_Outofbnds/c_C_11_2_c_I \
                         runupc/UPC/C_Outofbnds/c_C_11_2_c_L \
                         runupc/UPC/C_Outofbnds/c_C_11_2_c_D \
                         runupc/UPC/C_Outofbnds/c_C_11_2_c_S \
                         runupc/UPC/C_Outofbnds/c_C_11_2_d_F \
                         runupc/UPC/C_Outofbnds/c_C_11_2_d_I \
                         runupc/UPC/C_Outofbnds/c_C_11_2_d_L \
                         runupc/UPC/C_Outofbnds/c_C_11_2_d_D \
                         runupc/UPC/C_Outofbnds/c_C_11_2_d_S \
                         runupc/UPC/C_Outofbnds/c_C_11_4_c_F \
                         runupc/UPC/C_Outofbnds/c_C_11_4_c_I \
                         runupc/UPC/C_Outofbnds/c_C_11_4_c_L \
                         runupc/UPC/C_Outofbnds/c_C_11_4_c_D \
                         runupc/UPC/C_Outofbnds/c_C_11_4_c_S \
                         runupc/UPC/C_Outofbnds/c_C_11_4_d_F \
                         runupc/UPC/C_Outofbnds/c_C_11_4_d_I \
                         runupc/UPC/C_Outofbnds/c_C_11_4_d_L \
                         runupc/UPC/C_Outofbnds/c_C_11_4_d_D \
                         runupc/UPC/C_Outofbnds/c_C_11_4_d_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_a_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_a_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_a_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_a_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_a_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_b_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_b_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_b_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_b_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_b_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_c_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_c_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_c_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_c_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_c_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_d_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_d_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_d_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_d_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_d_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_e_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_e_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_e_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_e_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_e_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_f_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_f_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_f_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_f_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_f_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_g_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_g_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_g_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_g_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_g_S \
                         runupc/UPC/C_Outofbnds/c_C_6_1_h_F \
                         runupc/UPC/C_Outofbnds/c_C_6_1_h_I \
                         runupc/UPC/C_Outofbnds/c_C_6_1_h_L \
                         runupc/UPC/C_Outofbnds/c_C_6_1_h_D \
                         runupc/UPC/C_Outofbnds/c_C_6_1_h_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_a_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_a_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_a_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_a_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_a_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_b_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_b_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_b_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_b_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_b_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_c_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_c_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_c_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_c_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_c_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_d_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_d_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_d_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_d_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_d_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_e_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_e_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_e_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_e_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_e_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_f_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_f_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_f_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_f_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_f_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_g_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_g_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_g_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_g_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_g_S \
                         runupc/UPC/C_Outofbnds/c_C_6_2_h_F \
                         runupc/UPC/C_Outofbnds/c_C_6_2_h_I \
                         runupc/UPC/C_Outofbnds/c_C_6_2_h_L \
                         runupc/UPC/C_Outofbnds/c_C_6_2_h_D \
                         runupc/UPC/C_Outofbnds/c_C_6_2_h_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_a_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_a_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_a_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_a_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_a_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_b_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_b_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_b_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_b_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_b_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_c_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_c_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_c_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_c_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_c_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_d_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_d_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_d_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_d_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_d_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_e_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_e_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_e_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_e_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_e_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_f_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_f_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_f_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_f_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_f_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_g_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_g_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_g_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_g_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_g_S \
                         runupc/UPC/D_Outofbnds/c_D_3_1_h_F \
                         runupc/UPC/D_Outofbnds/c_D_3_1_h_I \
                         runupc/UPC/D_Outofbnds/c_D_3_1_h_L \
                         runupc/UPC/D_Outofbnds/c_D_3_1_h_D \
                         runupc/UPC/D_Outofbnds/c_D_3_1_h_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_a_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_a_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_a_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_a_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_a_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_b_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_b_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_b_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_b_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_b_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_c_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_c_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_c_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_c_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_c_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_d_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_d_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_d_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_d_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_d_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_e_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_e_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_e_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_e_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_e_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_f_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_f_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_f_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_f_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_f_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_g_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_g_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_g_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_g_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_g_S \
                         runupc/UPC/D_Outofbnds/c_D_3_2_h_F \
                         runupc/UPC/D_Outofbnds/c_D_3_2_h_I \
                         runupc/UPC/D_Outofbnds/c_D_3_2_h_L \
                         runupc/UPC/D_Outofbnds/c_D_3_2_h_D \
                         runupc/UPC/D_Outofbnds/c_D_3_2_h_S \
                         runupc/UPC/H_Memoryerr/c_H_2_b \
                         runupc/UPC/H_Memoryerr/c_H_2_d \
                         runupc/UPC/H_Memoryerr/c_H_3_b \
                         runupc/UPC/H_Memoryerr/c_H_3_d \
                         runupc/UPC/H_Memoryerr/c_H_4_c \
                         runupc/UPC/H_Memoryerr/c_H_5_c \
                         runupc/UPC/H_Memoryerr/c_H_5_e \
						             runupc/UPC/H_Memoryerr/c_H_6_c \
						             runupc/UPC/H_Memoryerr/c_H_6_e \
						             runupc/UPC/H_Memoryerr/c_H_7_e \
						             runupc/UPC/H_Memoryerr/c_H_7_f \
                         TheEnd_COMPILE_ERROR

# Test bugs prevent us from finding expected errors
# BUG Description
# c_C_10_2_a_L  (F/D) unexpected out of bounds access (allocates int, means other type[L,F,D])
# c_C_10_2_b_L  same
# c_C_10_4_a_L  same
#               note, the S version does not have a bug, b/c the array is allocated
#               in terms of int, thus no out of bounds access occurs.
# c_C_10_4_b_L  same
# c_C_11_2_a_L  same
# c_C_11_2_b_L  same
# c_C_11_4_a_L  same (see c_C_10_4_a_S)
# c_C_11_4_b_L  same
# c_H_2_c       forward declares a function, but calls that function differently in the call
# c_H_1_d       wrong line number for expected errors
# c_H_7_b       memory is allocated using upc_global_alloc, but released with free (C library function)
TEST_HAS_BUG = runupc/UPC/C_Outofbnds/c_C_10_2_a_F \
               runupc/UPC/C_Outofbnds/c_C_10_2_a_L \
               runupc/UPC/C_Outofbnds/c_C_10_2_a_D \
               runupc/UPC/C_Outofbnds/c_C_10_2_b_F \
               runupc/UPC/C_Outofbnds/c_C_10_2_b_L \
               runupc/UPC/C_Outofbnds/c_C_10_2_b_D \
							 runupc/UPC/C_Outofbnds/c_C_10_4_a_F \
               runupc/UPC/C_Outofbnds/c_C_10_4_a_L \
               runupc/UPC/C_Outofbnds/c_C_10_4_a_D \
               runupc/UPC/C_Outofbnds/c_C_10_4_a_S \
               runupc/UPC/C_Outofbnds/c_C_10_4_b_F \
               runupc/UPC/C_Outofbnds/c_C_10_4_b_L \
               runupc/UPC/C_Outofbnds/c_C_10_4_b_D \
               runupc/UPC/C_Outofbnds/c_C_11_2_a_F \
               runupc/UPC/C_Outofbnds/c_C_11_2_a_L \
               runupc/UPC/C_Outofbnds/c_C_11_2_a_D \
               runupc/UPC/C_Outofbnds/c_C_11_2_b_F \
               runupc/UPC/C_Outofbnds/c_C_11_2_b_L \
               runupc/UPC/C_Outofbnds/c_C_11_2_b_D \
               runupc/UPC/C_Outofbnds/c_C_11_4_a_F \
               runupc/UPC/C_Outofbnds/c_C_11_4_a_L \
               runupc/UPC/C_Outofbnds/c_C_11_4_a_D \
               runupc/UPC/C_Outofbnds/c_C_11_4_a_S \
               runupc/UPC/C_Outofbnds/c_C_11_4_b_F \
               runupc/UPC/C_Outofbnds/c_C_11_4_b_L \
               runupc/UPC/C_Outofbnds/c_C_11_4_b_D \
               runupc/UPC/H_Memoryerr/c_H_2_c \
               runupc/UPC/H_Memoryerr/c_H_1_d \
               runupc/UPC/H_Memoryerr/c_H_7_a \
               runupc/UPC/H_Memoryerr/c_H_7_b \
               TheEnd_TEST_BUGS


# Late bugs do not prevent us from finding expected errors (the unexpected bug
# occurs after the expected one).
TEST_HAS_LATE_BUG = runupc/UPC/C_Outofbnds/c_C_10_1_c_F \
                    runupc/UPC/C_Outofbnds/c_C_10_1_c_I \
                    runupc/UPC/C_Outofbnds/c_C_10_1_c_L \
                    runupc/UPC/C_Outofbnds/c_C_10_1_c_D \
                    runupc/UPC/C_Outofbnds/c_C_10_1_c_S \
                    runupc/UPC/C_Outofbnds/c_C_10_1_d_F \
                    runupc/UPC/C_Outofbnds/c_C_10_1_d_I \
                    runupc/UPC/C_Outofbnds/c_C_10_1_d_L \
                    runupc/UPC/C_Outofbnds/c_C_10_1_d_D \
                    runupc/UPC/C_Outofbnds/c_C_10_1_d_S \
                    runupc/UPC/C_Outofbnds/c_C_10_3_c_F \
                    runupc/UPC/C_Outofbnds/c_C_10_3_c_I \
                    runupc/UPC/C_Outofbnds/c_C_10_3_c_L \
                    runupc/UPC/C_Outofbnds/c_C_10_3_c_D \
                    runupc/UPC/C_Outofbnds/c_C_10_3_c_S \
                    runupc/UPC/C_Outofbnds/c_C_10_3_d_F \
                    runupc/UPC/C_Outofbnds/c_C_10_3_d_I \
                    runupc/UPC/C_Outofbnds/c_C_10_3_d_L \
                    runupc/UPC/C_Outofbnds/c_C_10_3_d_D \
                    runupc/UPC/C_Outofbnds/c_C_10_3_d_S \
                    TheEnd_LATE_TEST_BUGS


$(TEST_HAS_BUG):
	@echo This test fails due to a bug in the test code: $@

$(TESTS_KNOWN_TO_FAIL):
	@echo This test is known to fail, not running it now: $@

$(TEST_HAS_COMPILE_ERROR):
	@echo This test does not compile, not running it now: $@


run/tests/C/panasTests/%:
	@echo ignoring test: $@


#------------------------------------------------------------------------------

deleteAllbinfiles:
	rm -rf rted_source


checkfast:	runtimeCheck  \
				$(patsubst $(srcdir)/%, run/%, $(RTED_C_LOCAL_TEST_DIRS)) \
				$(patsubst $(srcdir)/%, run/%, $(RTED_CPP_LOCAL_TEST_DIRS)) \
				$(patsubst $(srcdir)/%, run/%, $(RTED_UPC_LOCAL_TEST_DIRS))
	@echo "---------------------------------------------------------------------------------------"
	@echo "--- ROSE/projects/RTED: checkfast passed                                            ---"
	@echo "---------------------------------------------------------------------------------------"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C rose_*.c *.dot Templates.DB *~ rose_rose* test *.bin *.txt *.c *.cpp rted_source

if ROSE_USE_RTED
# run/C/floating_point_errors
check-local: runtimeCheck  checkfast \
			run/C/allocation_deallocation_errors \
				run/C/array_index_out_of_bound \
				run/C/C99_specific_errors \
				run/C/input_output_errors \
				run/C/memory_leaks \
				run/C/pointer_errors \
				run/C/string_errors \
				run/C/subprogram_call_errors \
				run/C/uninitialized_variables \
				run/C++/allocation_deallocation_errors \
				run/C++/array_index_out_of_bound \
				run/C++/memory_leaks \
				run/C++/pointer_errors \
				run/C++/string_errors \
				run/C++/uninitialized_variables
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"
else
# DQ (11/10/2009): Comment out, with permission from Thomas, the RTED test until we
# can explore the RedHat version 5 problem, 32-bit problem, and Mac OSX problem.
# check-local: runtimeCheck checkfast
check-local:
	@echo "\n RTED short tests not run (Commented out temporarily):"
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/runtimeErrorCheck: make check not run here: configure --enable-rted=<RTED Path> ******"
	@echo "***********************************************************************************************************"
endif


endif
#endif
