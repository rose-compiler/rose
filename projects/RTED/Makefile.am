include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = index_array.txt \
             file_read.txt \
	     tests upccrose required/upc.h \
	     upcparam.h upcparam_s.h \
	     extract-actual-error.sed extract-expected-error.sed

# Currently, we exclude 32bit and MacOS
#if !OS_VENDOR_REDHAT_32BIT
if !OS_MACOSX

# To keep compile commands tidy, please ONLY put required include flags here.
# tps removed -pthreads from INCLUDES
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I. -Winvalid-pch 
INCLUDES2 = $(ROSE_INCLUDES) -I../../
UPC_INCLUDES =  -I$(srcdir)/required/

EXTRACT_EXPECTED_ERROR_SCRIPT=$(srcdir)/extract-expected-error.sed
EXTRACT_EXPECTED_ERROR=sed -n -f $(EXTRACT_EXPECTED_ERROR_SCRIPT)
EXTRACT_ACTUAL_ERROR=sed -n -f $(srcdir)/extract-actual-error.sed


# Qt Debugger is now linked into CppRuntimeSystem
LINK_QT_DEBUGGER=



# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck


LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl 
#$(top_builddir)/projects/RTED/CppRuntimeSystem/libCppRuntimeSystem.a

RTS=RuntimeSystem.cpp
RTED_CXX = $(LIBTOOL) --tag=CXX --mode=link g++ $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS)
RTED_CC = $(LIBTOOL) --tag=CC --mode=link gcc $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS)

#$(LIBTOOL) --tag=CXX  --mode=link $(AM_PROG_UPC)
if ROSE_USE_RTED
RTED_UPC =  ./upccrose $(AM_CXXFLAGS) \
	 $(AM_LDFLAGS) 
else
RTED_UPC =
endif

RTED_LIB=CppRuntimeSystem/libCppRuntimeSystem.la
runtimeCheck_SOURCES = runtimeCheck.cpp \
                       RtedTransformation.cpp ExecuteTransformations.cpp \
                       RtedSymbols.cpp RtedTransf_registerType.cpp \
                       RtedTransf_array.cpp RtedTransf_prolog.cpp \
                       RtedTransf_support.cpp RtedTransf_funccall.cpp \
                       RtedTransf_locals.cpp RtedTransf_scopestmt.cpp \
                       RtedTransf_funcdef.cpp RtedTransf_variable.cpp \
		       RtedTransf_returnStmt.cpp \
                       RtedTransf_iofunccall.cpp RtedTransf_operators.cpp \
                       RtedTransf_copyClass.cpp \
		       RtedTransformation.h \
		       RtedSymbols.h \
		       DataStructures.h \
		       RuntimeSystem.h RuntimeSystem.cpp RuntimeSystem_ParsingWorkaround.h \
		       RtedVisit_Variable.cpp RtedVisit.h



SUBDIRS = CppRuntimeSystem 

runtimeCheck_LDADD =  $(ROSE_LIBS) CppRuntimeSystem/libCppRuntimeSystem.la

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
				 $(srcdir)/tests/Cxx/templates \
				 $(srcdir)/tests/Cxx/types

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
		 $(rted_path)/C/floating_point_errors \
		 $(rted_path)/C/pointer_errors \
		 $(RTED_C_LOCAL_TEST_DIRS)

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
		 $(rted_path)/UPC/E_Outofbnds \
		$(rted_path)/UPC/A_Deadlcks \
		$(rted_path)/UPC/B_RacCond \
		$(rted_path)/UPC/F_Argmterr \
		$(rted_path)/UPC/G_Uninit \
		$(rted_path)/UPC/H_Memoryerr \
		$(rted_path)/UPC/I_Wronorder \
		$(rted_path)/UPC/J_Nonportbl \
		$(rted_path)/UPC/K_Warnings
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
	@echo -n "    inserting #define EXITCODE_OK 0 ... "
	@sed -i '1i\#define EXITCODE_OK 0' $@
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
	@echo -n "    inserting #define EXITCODE_OK 0 ... "
	@sed -i '1i\#define EXITCODE_OK 0' $@
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
	@echo -n "    inserting #define EXITCODE_OK 0 ... "
	@sed -i '1i\#define EXITCODE_OK 0' $@
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
			echo -n "    inserting define EXITCODE_OK 0 ... ";\
			sed -i '1i\#define EXITCODE_OK 0' $(1);\
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
			echo -n "    inserting define EXITCODE_OK 0 ... ";\
			sed -i '1i\#define EXITCODE_OK 0' $(1);\
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

TRANSFORMING_FILES += ./runtimeCheck 2 $(filter rted_source/%$(strip $(1)), $^) \
				       -rose:$(strip $(3)) \
				       -c \
				       $(INCLUDES) \
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
TRANSFORMING_FILES += echo -n "   adding line information ... ";
TRANSFORMING_FILES += cat rose_$(notdir $(patsubst %_rose$(strip $(1)), %$(strip $(1)), $@ ) ) | \
			  nl -p -b 'a' | \
			  sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $@;

TRANSFORMING_FILES += rm rose_$(notdir $(patsubst %_rose$(strip $(1)), %$(strip $(1)), $@ ) );

TRANSFORMING_FILES += SUPPORT_FILE='rose_$(notdir $(patsubst %_rose$(strip $(1)), %_s$(strip $(1)), $@ ) )'; \
		      if [ -f $$SUPPORT_FILE ]; \
		      then \
			cat $$SUPPORT_FILE | \
			    nl -p -b 'a' | \
			    sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $(patsubst %_rose$(strip $(1)), %_s_rose$(strip $(1)), $@ ); \
		      fi;

TRANSFORMING_FILES += rm -f rose_$(notdir $(patsubst %_rose$(strip $(1)), %_s$(strip $(1)), $@ ) );
TRANSFORMING_FILES += echo " done.";

rted_source/%_rose.c: runtimeCheck rted_source/%.c rted_source/%_s.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .c, $(RTED_CC), C_Only)

rted_source/%_rose.cpp: runtimeCheck rted_source/%.cpp rted_source/%_s.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .cpp, $(RTED_CXX), Cxx)

rted_source/%_rose.upc: runtimeCheck rted_source/%.upc rted_source/%_s.upc $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h 
	$(call TRANSFORMING_FILES, .upc, $(RTED_UPC), UPC $(UPC_INCLUDES))


rted_source/%_rose.c: runtimeCheck rted_source/%.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .c, $(RTED_CC), C_Only)

rted_source/%_rose.cpp: runtimeCheck rted_source/%.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .cpp, $(RTED_CXX), Cxx)

rted_source/%_rose.upc: runtimeCheck rted_source/%.upc $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h 
	$(call TRANSFORMING_FILES, .upc, $(RTED_UPC), UPC $(UPC_INCLUDES))

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
				      $(srcdir)/$(RTS) \
				      $(INCLUDES) 
if ROSE_USE_RTED
COMPILING_RUNTIME_SYSTEM +=    -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) 
endif
COMPILING_RUNTIME_SYSTEM +=    -I$(dir $(patsubst rted_source/%, $(srcdir)/%, $@)) \
				      -I$(top_builddir) \
				      -I$(srcdir) \
				      $(3) $(RTED_LIB) > $@.compile.out 2> $@.compile.err \
					  || { cat $@.compile.err; false; }; \
			   if [ $$? -ne 0 ]; then exit 1; fi; \
			   echo -n " done.";

rted_source/%.bin: rted_source/%_rose.c rted_source/%_s_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC))

rted_source/%.bin: rted_source/%_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC))

rted_source/%.bin: rted_source/%_rose.cpp rted_source/%_s_rose.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX))

rted_source/%.bin: rted_source/%_rose.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX))

rted_source/%.bin: rted_source/%_rose.upc rted_source/%_s_rose.upc $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .upc, $(RTED_UPC))

rted_source/%.bin: rted_source/%_rose.upc $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .upc, $(RTED_UPC))

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
				echo "(remember to subtract 2 from actual line numbers (for non-instrumented source) to compensate for added lines)";\
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

runupc/%: rted_source/%.bin rted_source/%.expected-output
	@echo -n "   running (UPC) runtime error detection ... upcrun -n 4 $(filter %.bin, $?)"
	@OUTPUT_SRC = '$(strip $(filter %.expected-output, $?))';\
		if [ -f "$$OUTPUT_SRC" ]; then\
			RESULT='$(strip $(patsubst %.bin, %.result.txt, $(filter rted_source/%.bin, $?)))';\
			upcrun -quiet -n 4 ./$(filter rted_source/%.bin, $?)  2> $$RESULT;\
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
				echo "(remember to subtract 2 from actual line numbers (for non-instrumented source) to compensate for added lines)";\
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

runupc_log/%: rted_source/%.bin rted_source/%.expected-output 
	@echo "   logging results for runtime error detection ... $(filter %.bin, $?)"
	@OUTPUT_SRC='$(strip $(filter %.expected-output, $?))';\
		if [ -f "$$OUTPUT_SRC" ]; then\
			RESULT='$(strip $(patsubst %.bin, %.result.txt, $(filter rted_source/%.bin, $?)))';\
			upcrun -n 4 ./$(filter rted_source/%.bin, $?) 2> $$RESULT;\
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
					  run/C++/uninitialized_variables/c_A_6_e_in


$(TESTS_KNOWN_TO_FAIL):
	@echo This test is known to fail, not running it now: $@

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