include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_RTED

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I. -Winvalid-pch 
INCLUDES2 = $(ROSE_INCLUDES) -I../../


EXTRACT_EXPECTED_ERROR_SCRIPT=$(srcdir)/extract-expected-error.sed
EXTRACT_EXPECTED_ERROR=sed -n -f $(EXTRACT_EXPECTED_ERROR_SCRIPT)
EXTRACT_ACTUAL_ERROR=sed -n -f $(srcdir)/extract-actual-error.sed


# Qt Debugger is now linked into CppRuntimeSystem
LINK_QT_DEBUGGER=



# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runtimeCheck

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl 

RTS=RuntimeSystem.cpp
RTED_CXX = $(LIBTOOL) --tag=CXX --mode=link g++ $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS)
RTED_CC = $(LIBTOOL) --tag=CC --mode=link gcc $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS)
RTED_LIB=CppRuntimeSystem/libCppRuntimeSystem.la
runtimeCheck_SOURCES = runtimeCheck.cpp \
                       RtedTransformation.cpp \
                       RtedSymbols.cpp RtedTransf_registerType.cpp \
                       RtedTransf_array.cpp RtedTransf_prolog.cpp \
                       RtedTransf_support.cpp RtedTransf_funccall.cpp \
                       RtedTransf_locals.cpp RtedTransf_scopestmt.cpp \
                       RtedTransf_funcdef.cpp RtedTransf_variable.cpp \
                       RtedTransf_iofunccall.cpp



SUBDIRS = CppRuntimeSystem

runtimeCheck_LDADD =  $(ROSE_LIBS) CppRuntimeSystem/libCppRuntimeSystem.la
#if ROSE_WITH_ROSEQT
#	runtimeCheck_LDADD += $(top_builddir)/projects/RTED/rted_qt/libRtedQt.la
#endif

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
	     file_read.txt \
             DataStructures.h \
             RtedSymbols.h

CLEANFILES = *.out *.ti *.pdf test_runtime* *.txt rose_test_c_E_1_1_a

RTED_TEST_DIRS = $(rted_path)/C/array_index_out_of_bound \
		 $(rted_path)/C/string_errors \
		 $(rted_path)/C/subprogram_call_errors \
		 $(rted_path)/C/allocation_deallocation_errors \
		 $(rted_path)/C/input_output_errors \
		 $(rted_path)/C/memory_leaks \
		 $(rted_path)/C/uninitialized_variables \
		 $(rted_path)/C/C99_specific_errors \
		 $(rted_path)/C/floating_point_errors \
		 $(rted_path)/C/pointer_errors \
		 $(rted_path)/C++/array_index_out_of_bound \
		 $(rted_path)/C++/string_errors \
		 $(rted_path)/C++/allocation_deallocation_errors \
		 $(rted_path)/C++/floating_point_errors \
		 $(rted_path)/C++/pointer_errors \
		 $(rted_path)/C++/input_output_errors \
		 $(rted_path)/C++/memory_leaks \
		 $(rted_path)/C++/uninitialized_variables \
		 $(srcdir)/tests/C/memoryleaks \
		 $(srcdir)/tests/C/nomemoryleaks \
		 $(srcdir)/tests/C/panasTests

GENERATE_RTED_ORIGSOURCES = ${foreach RTED_TEST_DIR, $(RTED_TEST_DIRS), ${wildcard $(RTED_TEST_DIR)/*[!_s]${strip $(1)}} }
GENERATE_RTED_COPYSOURCES = ${foreach RTED_TEST_DIR, ${filter ${strip $(2)}%, $(RTED_TEST_DIRS)}, ${patsubst ${strip $(2)}/%${strip $(1)}, rted_source/%${strip $(1)}, ${wildcard $(RTED_TEST_DIR)/*${strip $(1)}} } }
GENERATE_RTED_ROSESOURCES = ${patsubst %${strip $(1)}, %_rose${strip $(1)}, $(2) }
GENERATE_RTED_BINFILES = ${patsubst ${strip $(2)}/%${strip $(1)}, rted_source/%.bin, ${filter ${strip $(2)}%, ${strip $(3)}}}
GENERATE_RTED_DEBUGBINFILES = ${patsubst ${strip $(2)}/%${strip $(1)}, rted_source/%_debug.bin, ${filter ${strip $(2)}%, ${strip $(3)}}}
GENERATE_RTED_OUTPUTFILES = ${patsubst $(rted_path)/%${strip $(1)}, rted_source/%.${strip $(2)}-output, ${strip $(3)}}

RTED_ORIGSOURCES_C   = ${call GENERATE_RTED_ORIGSOURCES, .c}
RTED_ORIGSOURCES_CPP = ${call GENERATE_RTED_ORIGSOURCES, .cpp}
RTED_ORIGSOURCES = $(RTED_ORIGSOURCES_C) $(RTED_ORIGSOURCES_CPP)

RTED_COPYSOURCES_GLOBAL_C = ${call GENERATE_RTED_COPYSOURCES, .c, $(rted_path)}
RTED_COPYSOURCES_LOCAL_C  = ${call GENERATE_RTED_COPYSOURCES, .c, $(srcdir)}
RTED_COPYSOURCES_GLOBAL_CPP = ${call GENERATE_RTED_COPYSOURCES, .cpp, $(rted_path)}
RTED_COPYSOURCES_LOCAL_CPP  = ${call GENERATE_RTED_COPYSOURCES, .cpp, $(srcdir)}
RTED_COPYSOURCES_GLOBAL = $(RTED_COPYSOURCES_GLOBAL_C) $(RTED_COPYSOURCES_GLOBAL_CPP)
RTED_COPYSOURCES_LOCAL = $(RTED_COPYSOURCES_LOCAL_C) $(RTED_COPYSOURCES_LOCAL_CPP)
RTED_COPYSOURCES_C = $(RTED_COPYSOURCES_LOCAL_C) $(RTED_COPYSOURCES_GLOBAL_C)
RTED_COPYSOURCES_CPP = $(RTED_COPYSOURCES_LOCAL_CPP) $(RTED_COPYSOURCES_GLOBAL_CPP)
RTED_COPYSOURCES = $(RTED_COPYSOURCES_GLOBAL) $(RTED_COPYSOURCES_LOCAL)

RTED_ROSESOURCES  = ${call GENERATE_RTED_ROSESOURCES, .c, $(RTED_COPYSOURCES_C)}
RTED_ROSESOURCES += ${call GENERATE_RTED_ROSESOURCES, .cpp, $(RTED_COPYSOURCES_CPP)}

RTED_BINFILES  = ${call GENERATE_RTED_BINFILES, .c, $(rted_path), $(RTED_ORIGSOURCES_C)}
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .c, $(srcdir), $(RTED_ORIGSOURCES_C)}
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .cpp, $(rted_path), $(RTED_ORIGSOURCES_CPP)}
RTED_BINFILES += ${call GENERATE_RTED_BINFILES, .cpp, $(srcdir), $(RTED_ORIGSOURCES_CPP)}

RTED_DEBUGBINFILES  = ${call GENERATE_RTED_DEBUGBINFILES, .c, $(rted_path), $(RTED_ORIGSOURCES_C)}
RTED_DEBUGBINFILES += ${call GENERATE_RTED_DEBUGBINFILES, .c, $(srcdir), $(RTED_ORIGSOURCES_C)}
RTED_DEBUGBINFILES += ${call GENERATE_RTED_DEBUGBINFILES, .cpp, $(rted_path), $(RTED_ORIGSOURCES_CPP)}
RTED_DEBUGBINFILES += ${call GENERATE_RTED_DEBUGBINFILES, .cpp, $(srcdir), $(RTED_ORIGSOURCES_CPP)}

RTED_OUTPUTFILES  = ${call GENERATE_RTED_OUTPUTFILES, .c, actual, $(RTED_ORIGSOURCES_C)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .c, expected, $(RTED_ORIGSOURCES_C)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .cpp, actual, $(RTED_ORIGSOURCES_CPP)}
RTED_OUTPUTFILES += ${call GENERATE_RTED_OUTPUTFILES, .cpp, expected, $(RTED_ORIGSOURCES_CPP)}

.SECONDARY: $(RTED_BINFILES) $(RTED_DEBUGBINFILES) $(RTED_COPYSOURCES) $(RTED_ROSESOURCES) $(RTED_OUTPUTFILES)
.PRECIOUS: $(RTED_BINFILES) $(RTED_DEBUGBINFILES) $(RTED_COPYSOURCES) $(RTED_ROSESOURCES) $(RTED_OUTPUTFILES)

print:
	@echo $(RTED_ROSESOURCES)

check-C-array: run/C/array_index_out_of_bound

check-C-string: run/C/string_errors

check-C-subprogram: run/C/subprogram_call_errors

check-C-allocation: run/C/allocation_deallocation_errors

check-C-inputoutput: run/C/input_output_errors

check-C-memoryleaks: run/C/memory_leaks

check-C-uninitvar: run/C/uninitialized_variables

check-C-C99specific: run/C/C99_specific_errors

rted_source:
	@cp -f $(srcdir)/index_array.txt .
	@cp -f $(srcdir)/file_read.txt .
	@mkdir -p $(foreach rted_dir,$(filter $(rted_path)%, $(RTED_TEST_DIRS) ), $(patsubst $(rted_path)/%, rted_source/%, $(rted_dir) ) )
	@mkdir -p $(foreach local_test_dir,$(filter $(srcdir)%, $(RTED_TEST_DIRS) ), $(patsubst $(srcdir)/%, rted_source/%, $(local_test_dir) ) )

#------------------------------------------------------------------------------
# rule to copy the sources into our current working directory
#------------------------------------------------------------------------------

#testcases in $(top_srcdir)/projects/RTED/tests
rted_source/tests/%.c: rted_source $(top_srcdir)/projects/RTED/tests/%.c
	@echo -e Checking $(patsubst $(top_srcdir)/projects/RTED/tests/%.c, %, $(filter $(top_srcdir)/%, $^)) ...
	@echo -n "    copying input file ... "
	@cp $(filter $(top_srcdir)/%.c, $^) $(dir $@)
	@echo " done."
	@echo -n "    inserting #include \"RuntimeSystem.h\" ... "
	@sed -i '1i\#include \"RuntimeSystem.h\"' $@
	@echo " done."
	@echo -n "    inserting #define EXITCODE_OK 0 ... "
	@sed -i '1i\#define EXITCODE_OK 0' $@
	@echo " done."

#testcases in rted distribution

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

rted_source/%.c: rted_source $(rted_path)/%.c
	$(call COPYING_FILE, $@)

rted_source/%.cpp: rted_source $(rted_path)/%.cpp
	$(call COPYING_FILE, $@)

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create _rose.c and _s_rose.c files
#------------------------------------------------------------------------------

rted_source/%_s_rose.c: rted_source/%_s.c
	@touch $@

rted_source/%_s_rose.cpp: rted_source/%_s.cpp
	@touch $@

# parameters:
#  1: file extension (.c, .cpp, .f, ... )
#  2: compile command
#  3: rose option, for example: C_Only
TRANSFORMING_FILES = @echo -n "    testing if files compiles with gcc (with _s) ... ";

TRANSFORMING_FILES += $(2) \
		      -g \
		      -o $(patsubst %_rose$(strip $(1)), %.bin, $@) \
		      $(filter rted_source/%$(strip $(1)), $^) \
		      $(INCLUDES) \
		      -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) \
		      -I$(top_builddir) -I$(srcdir) $(RTED_LIB);

TRANSFORMING_FILES += if [ $$? -ne 0 ]; then exit 1; fi;

TRANSFORMING_FILES += rm $(patsubst %_rose$(strip $(1)), %.bin, $@);
TRANSFORMING_FILES += echo " done.";
TRANSFORMING_FILES += echo -n "    running transformation ... ";

TRANSFORMING_FILES += ./runtimeCheck 2 $(filter rted_source/%$(strip $(1)), $^) \
				       -rose:$(strip $(3)) \
				       -c \
				       $(INCLUDES) \
				       -I$(top_builddir) \
				       -I$(srcdir) \
				       -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@));
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

rted_source/%_rose.c: runtimeCheck rted_source/%.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .c, $(RTED_CC), C_Only)

rted_source/%_rose.cpp: runtimeCheck rted_source/%.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call TRANSFORMING_FILES, .cpp, $(RTED_CXX), Cxx)

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create the binary files
#------------------------------------------------------------------------------

#rted_source/%_debug.bin: rted_source/%_rose.c rted_source/%_s_rose.c
#	@touch $@

#rted_source/%_debug.bin: rted_source/%_rose.c
#	@touch $@

# parameters:
#  1: file extension (.c, .cpp, .f, ... )
#  2: compile command
#  3: additional parameters, like linking against debugger ...
COMPILING_RUNTIME_SYSTEM = @echo -n "   compiling instrumented debugger code (with _s) ... "; \
			   $(2) -g -o $@ $(filter rted_source/%$(strip $(1)), $^) \
				      $(srcdir)/$(RTS) \
				      $(INCLUDES) \
				      -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) \
				      -I$(top_builddir) \
				      -I$(srcdir) \
				      $(3) $(RTED_LIB); \
			   if [ $$? -ne 0 ]; then exit 1; fi; \
			   echo " done."

rted_source/%_debug.bin: rted_qt rted_source/%_rose.c rted_source/%_s_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC), $(LINK_QT_DEBUGGER))

rted_source/%_debug.bin: rted_qt rted_source/%_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC), $(LINK_QT_DEBUGGER))

rted_source/%.bin: rted_source/%_rose.c rted_source/%_s_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC))

rted_source/%.bin: rted_source/%_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .c, $(RTED_CC))

rted_source/%_debug.bin: rted_qt rted_source/%_rose.cpp rted_source/%_s_rose.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX), $(LINK_QT_DEBUGGER))

rted_source/%_debug.bin: rted_qt rted_source/%_rose.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX), $(LINK_QT_DEBUGGER))

rted_source/%.bin: rted_source/%_rose.cpp rted_source/%_s_rose.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX))

rted_source/%.bin: rted_source/%_rose.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	$(call COMPILING_RUNTIME_SYSTEM, .cpp, $(RTED_CXX))

# TODO: make this aware of different file extensions ...

# parameters:
#  1: file extension (.c, .cpp, .f, ... )
GENERATE_EXPECTED_OUTPUT = @OUTPUT_SRC='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%.ok_msg, $@))';\
			   TEST_FILE='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%$(strip $(1)), $@))';\
			   if [ -f "$$OUTPUT_SRC" ]; then \
				echo ===`wc -l $$TEST_FILE`=== | $(EXTRACT_EXPECTED_ERROR) - $$OUTPUT_SRC > $@;\
			   fi

rted_source/C/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .c )

rted_source/tests/C/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .c )

rted_source/C++/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .cpp )

rted_source/tests/C++/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	$(call GENERATE_EXPECTED_OUTPUT, .cpp )

#@OUTPUT_SRC='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%.ok_msg, $@))';\
#TEST_FILE='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%, $@))';\
#if [ -f "$$OUTPUT_SRC" ]; then \
#echo ===`wc -l $$TEST_FILE`=== | $(EXTRACT_EXPECTED_ERROR) - $$OUTPUT_SRC > $@;\
#fi
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# rules to run arbitrary tests from the rted distribution
#------------------------------------------------------------------------------

run_debug/%: rted_source/%_debug.bin rted_source/%.expected-output 
	@echo -n "   running runtime error detection debugger ... "
	@./$(filter rted_source/%_debug.bin, $?)
	@echo " done."

run_checkonly/%: rted_source/%.bin rted_source/%.expected-output 
	@echo -n "   running runtime error detection ... "
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
				echo -e '\n\n';\
				echo 'Expected error did not match actual error found';\
				echo ' (the error may be in the extraction scripts -- see the actual files that were diff-d';\
				echo "  $(filter %.expected-output, $?) $$ACTUAL_OUTPUT)";\
				echo -e '\n========== Expected Error ==========';\
				cat $(patsubst rted_source/%.expected-output, $(rted_path)/%.ok_msg, $(filter %.expected-output, $?));\
				echo -e '\n========== Actual   Error ==========';\
				cat $$RESULT;\
				exit 1;\
			fi;\
		else\
			./$(filter rted_source/%.bin, $?);\
		fi;
	@echo " done."

run/%: rted_source/%.bin rted_source/%_debug.bin rted_source/%.expected-output 
	@echo -n "   running runtime error detection ... "
	@$(patsubst %_debug.bin, ./%.bin, $(filter rted_source/%_debug.bin, $^) ); \
	    if [ $$? -ne 0 ]; \
	    then \
		echo " failed."; \
		echo "   running debugger ... "; \
		$(patsubst %.bin, ./%_debug.bin, $^); \
		exit 1; \
	    fi 
	@echo " done."
       
run_log/%: rted_source/%.bin rted_source/%.expected-output 
	@echo -n "   running runtime error detection ... "
	@$(filter rted_source/%.bin, $^); \
	if [ $$? -ne 0 ]; \
	then \
		echo " $@ failed" >> test_log.txt;\
	else \
	echo " $@ passed" >> test_log.txt;\
	fi 
	@echo " done."  

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run all tests in the directories listed in RTED_ORIGSOURCES
#------------------------------------------------------------------------------

RTED_ORIGSOURCES_GLOBAL_C = ${filter $(rted_path)%, $(RTED_ORIGSOURCES_C)}
RTED_ORIGSOURCES_LOCAL_C = ${filter $(srcdir)%, $(RTED_ORIGSOURCES_C)}
RTED_ORIGSOURCES_GLOBAL_CPP = ${filter $(rted_path)%, $(RTED_ORIGSOURCES_CPP)}
RTED_ORIGSOURCES_LOCAL_CPP = ${filter $(srcdir)%, $(RTED_ORIGSOURCES_CPP)}

GENERATE_RTED_RUN_TARGET = ${patsubst ${strip $(4)}/%${strip $(1)}, ${strip $(2)}/%, $(3)}
RUN_TARGETS_GLOBAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run, $(RTED_ORIGSOURCES_GLOBAL_C), $(rted_path)}
RUN_TARGETS_LOCAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run, $(RTED_ORIGSOURCES_LOCAL_C), $(srcdir)}
RUN_TARGETS_GLOBAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run, $(RTED_ORIGSOURCES_GLOBAL_CPP), $(rted_path)}
RUN_TARGETS_LOCAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run, $(RTED_ORIGSOURCES_LOCAL_CPP), $(srcdir)}
RUN_DEBUG_TARGETS_GLOBAL_C  = ${call GENERATE_RTED_RUN_TARGET, .c, run_debug, $(RTED_ORIGSOURCES_GLOBAL_C), $(rted_path)}
RUN_DEBUG_TARGETS_LOCAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run_debug, $(RTED_ORIGSOURCES_LOCAL_C), $(srcdir)}
RUN_DEBUG_TARGETS_GLOBAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_debug, $(RTED_ORIGSOURCES_GLOBAL_CPP), $(rted_path)}
RUN_DEBUG_TARGETS_LOCAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_debug, $(RTED_ORIGSOURCES_LOCAL_CPP), $(srcdir)}
RUN_CHECKONLY_TARGETS_GLOBAL_C  = ${call GENERATE_RTED_RUN_TARGET, .c, run_checkonly, $(RTED_ORIGSOURCES_GLOBAL_C), $(rted_path)}
RUN_CHECKONLY_TARGETS_LOCAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run_checkonly, $(RTED_ORIGSOURCES_LOCAL_C), $(srcdir)}
RUN_CHECKONLY_TARGETS_GLOBAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_checkonly, $(RTED_ORIGSOURCES_GLOBAL_CPP), $(rted_path)}
RUN_CHECKONLY_TARGETS_LOCAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_checkonly, $(RTED_ORIGSOURCES_LOCAL_CPP), $(srcdir)}
RUN_LOG_TARGETS_GLOBAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run_log, $(RTED_ORIGSOURCES_GLOBAL_C), $(rted_path)}
RUN_LOG_TARGETS_LOCAL_C = ${call GENERATE_RTED_RUN_TARGET, .c, run_log, $(RTED_ORIGSOURCES_LOCAL_C), $(srcdir)}
RUN_LOG_TARGETS_GLOBAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_log, $(RTED_ORIGSOURCES_GLOBAL_CPP), $(rted_path)}
RUN_LOG_TARGETS_LOCAL_CPP = ${call GENERATE_RTED_RUN_TARGET, .cpp, run_log, $(RTED_ORIGSOURCES_LOCAL_CPP), $(srcdir)}

run: $(RUN_TARGETS_GLOBAL_C) $(RUN_TARGETS_LOCAL_C) $(RUN_TARGETS_GLOBAL_CPP) $(RUN_TARGETS_LOCAL_CPP)

run_debug: $(RUN_DEBUG_TARGETS_GLOBAL_C) $(RUN_DEBUG_TARGETS_LOCAL_C) $(RUN_DEBUG_TARGETS_GLOBAL_CPP) $(RUN_DEBUG_TARGETS_LOCAL_CPP)

run_checkonly: $(RUN_CHECKONLY_TARGETS_GLOBAL_C) $(RUN_CHECKONLY_TARGETS_LOCAL_C) $(RUN_CHECKONLY_TARGETS_GLOBAL_CPP) $(RUN_CHECKONLY_TARGETS_LOCAL_CPP)

run_log: $(RUN_LOG_TARGETS_GLOBAL_C) $(RUN_LOG_TARGETS_LOCAL_C) $(RUN_LOG_TARGETS_GLOBAL_CPP) $(RUN_LOG_TARGETS_LOCAL_CPP)

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run all tests in a directory listed in RTED_ORIGSOURCES
#------------------------------------------------------------------------------

define RUN_TARGET
$(1): $(filter $(1)%, $(RUN_TARGETS_GLOBAL_C))
	@echo $(1) complete
endef

define RUN_CHECKONLY_TARGET
$(1): $(filter $(1)%, $(RUN_CHECKONLY_TARGETS_GLOBAL_C))
	@echo $(1) complete
endef

define RUN_DEBUG_TARGET
$(1): $(filter $(1)%, $(RUN_DEBUG_TARGETS_GLOBAL_C))
	@echo $(1) complete
endef

define RUN_LOG_TARGET
$(1): $(filter $(1)%, $(RUN_LOG_TARGETS_GLOBAL_C))
	@echo $(1) complete
endef


print_t:
	@echo $(RUN_CHECKONLY_TARGETS_GLOBAL_C)

# OPTIMIZE: eval/call is slow (make nothing ~1.1s -> 2.3s with run, check,
# 	log, debug)
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(rted_path)%, run%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_CHECKONLY_TARGET, ${patsubst $(rted_path)%, run_checkonly%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_LOG_TARGET, ${patsubst $(rted_path)%, run_log%, $(rted_dir)}}}}
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_DEBUG_TARGET, ${patsubst $(rted_path)%, run_debug%, $(rted_dir)}}}}
#${foreach rted_dir, ${filter $(srcdir)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(srcdir)%, %, $(rted_dir)}}}}

#------------------------------------------------------------------------------

deleteAllbinfiles:
	rm -rf rted_source

c-compile-runtimesystem:
	@echo "Make sure that RuntimeSystem compiles with C"

check-local: rted_qt runtimeCheck $(TESTCODES) c-compile-runtimesystem run_checkonly/C/string_errors run_checkonly/C/allocation_deallocation_errors run_checkonly/C/input_output_errors
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"


clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C rose_*.c *.dot Templates.DB *~ rose_rose* test *.bin *.txt *.c *.cpp rted_source 

endif
