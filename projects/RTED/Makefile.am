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
RTED_CXX = $(CXXLINK)
RTED_CC = $(LIBTOOL) --tag=CC --mode=link --quiet gcc $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@ 
RTED_LIB=CppRuntimeSystem/libCppRuntimeSystem.la
runtimeCheck_SOURCES = runtimeCheck.cpp \
                       RtedTransformation.cpp \
                       $(RTS) \
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

RTED_ORIGSOURCES_C = ${foreach RTED_TEST_DIR, $(RTED_TEST_DIRS), ${wildcard $(RTED_TEST_DIR)/*[!_s].c} }
RTED_COPYSOURCES_C = ${foreach RTED_TEST_DIR, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${patsubst $(rted_path)/%.c, rted_source/%.c, ${wildcard $(RTED_TEST_DIR)/*.c} } }
RTED_LOCAL_COPYSOURCES_C = ${foreach RTED_TEST_DIR, ${filter $(srcdir)%, $(RTED_TEST_DIRS)}, ${patsubst $(srcdir)/%.c, rted_source/%.c, ${wildcard $(RTED_TEST_DIR)/*.c} } }
RTED_ROSESOURCES_C = ${patsubst %.c, %_rose.c, $(RTED_COPYSOURCES_C) }
RTED_LOCAL_ROSESOURCES_C = ${patsubst %.c, %_rose.c, $(RTED_LOCAL_COPYSOURCES_C) }
RTED_BINFILES_C = ${patsubst $(rted_path)/%.c, rted_source/%.bin, ${filter $(rted_path)%, $(RTED_ORIGSOURCES_C)}}
RTED_LOCAL_BINFILES_C = ${patsubst $(srcdir)/%.c, rted_source/%.bin, ${filter $(srcdir)%, $(RTED_ORIGSOURCES_C)}}
RTED_DEBUGBINFILES_C = ${patsubst $(rted_path)/%.c, rted_source/%_debug.bin, ${filter $(rted_path)%, $(RTED_ORIGSOURCES_C)}}
RTED_LOCAL_DEBUGBINFILES_C = ${patsubst $(srcdir)/%.c, rted_source/%_debug.bin, ${filter $(srcdir)%, $(RTED_ORIGSOURCES_C)}}
RTED_ACTUALOUTPUTFILES_C = ${patsubst $(rted_path)/%.c, rted_source/%.actual-output, $(RTED_ORIGSOURCES_C)}
RTED_EXPECTEDOUTPUTFILES_C = ${patsubst $(rted_path)/%.c, rted_source/%.expected-output, $(RTED_ORIGSOURCES_C)}

RTED_ORIGSOURCES_CPP = ${foreach RTED_TEST_DIR, $(RTED_TEST_DIRS), ${wildcard $(RTED_TEST_DIR)/*[!_s].cpp} }
RTED_COPYSOURCES_CPP = ${foreach RTED_TEST_DIR, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${patsubst $(rted_path)/%, rted_source/%, ${wildcard $(RTED_TEST_DIR)/*.cpp} } }
RTED_LOCAL_COPYSOURCES_CPP = ${foreach RTED_TEST_DIR, ${filter $(srcdir)%, $(RTED_TEST_DIRS)}, ${patsubst $(srcdir)/%, rted_source/%, ${wildcard $(RTED_TEST_DIR)/*.cpp} } }
RTED_ROSESOURCES_CPP = ${patsubst %.cpp, %_rose.cpp, $(RTED_COPYSOURCES_CPP) }
RTED_LOCAL_ROSESOURCES_CPP = ${patsubst %.cpp, %_rose.cpp, $(RTED_LOCAL_COPYSOURCES_CPP) }
RTED_BINFILES_CPP = ${patsubst $(rted_path)/%.cpp, rted_source/%.bin, ${filter $(rted_path)%, $(RTED_ORIGSOURCES_CPP)}}
RTED_LOCAL_BINFILES_CPP = ${patsubst $(srcdir)/%.cpp, rted_source/%.bin, ${filter $(srcdir)%, $(RTED_ORIGSOURCES_CPP)}}
RTED_DEBUGBINFILES_CPP = ${patsubst $(rted_path)/%.cpp, rted_source/%_debug.bin, ${filter $(rted_path)%, $(RTED_ORIGSOURCES_CPP)}}
RTED_LOCAL_DEBUGBINFILES_CPP = ${patsubst $(srcdir)/%.cpp, rted_source/%_debug.bin, ${filter $(srcdir)%, $(RTED_ORIGSOURCES_CPP)}}
RTED_ACTUALOUTPUTFILES_CPP = ${patsubst $(rted_path)/%.cpp, rted_source/%.actual-output, $(RTED_ORIGSOURCES_CPP)}
RTED_EXPECTEDOUTPUTFILES_CPP = ${patsubst $(rted_path)/%.cpp, rted_source/%.expected-output, $(RTED_ORIGSOURCES_CPP)}

RTED_ORIGSOURCES = $(RTED_ORIGSOURCES_CPP) $(RTED_ORIGSOURCES_C)
RTED_COPYSOURCES = $(RTED_COPYSOURCES_CPP) $(RTED_COPYSOURCES_C)
RTED_LOCAL_COPYSOURCES = $(RTED_LOCAL_COPYSOURCES_CPP) $(RTED_LOCAL_COPYSOURCES_C)
RTED_ROSESOURCES = $(RTED_ROSESOURCES_CPP) $(RTED_ROSESOURCES_C)
RTED_LOCAL_ROSESOURCES = $(RTED_LOCAL_ROSESOURCES_CPP) $(RTED_LOCAL_ROSESOURCES_C)
RTED_BINFILES = $(RTED_BINFILES_CPP) $(RTED_BINFILES_C)
RTED_LOCAL_BINFILES = $(RTED_LOCAL_BINFILES_CPP) $(RTED_LOCAL_BINFILES_C)
RTED_DEBUGBINFILES = $(RTED_DEBUGBINFILES_CPP) $(RTED_DEBUGBINFILES_C)
RTED_LOCAL_DEBUGBINFILES = $(RTED_LOCAL_DEBUGBINFILES_CPP) $(RTED_LOCAL_DEBUGBINFILES_C)
RTED_ACTUALOUTPUTFILES = $(RTED_ACTUALOUTPUTFILES_CPP) $(RTED_ACTUALOUTPUTFILES_C)
RTED_EXPECTEDOUTPUTFILES = $(RTED_EXPECTEDOUTPUTFILES_CPP) $(RTED_EXPECTEDOUTPUTFILES_C)

.SECONDARY: $(RTED_BINFILES) $(RTED_DEBUGBINFILES) $(RTED_COPYSOURCES) $(RTED_ROSESOURCES) $(RTED_ACTUALOUTPUTFILES) $(RTED_EXPECTEDOUTPUTFILES)\
	    $(RTED_LOCAL_BINFILES) $(RTED_LOCAL_DEBUGBINFILES) $(RTED_LOCAL_COPYSOURCES) $(RTED_LOCAL_ROSESOURCES)
.PRECIOUS: $(RTED_BINFILES) $(RTED_DEBUGBINFILES) $(RTED_COPYSOURCES) $(RTED_ROSESOURCES) $(RTED_ACTUALOUTPUTFILES) $(RTED_EXPECTEDOUTPUTFILES)\
	   $(RTED_LOCAL_BINFILES) $(RTED_LOCAL_DEBUGBINFILES) $(RTED_LOCAL_COPYSOURCES) $(RTED_LOCAL_ROSESOURCES)

print:
	@echo $(RTED_ORIGSOURCES)

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
			echo -n "    copying input file ... ";\
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

#TODO 1: comment this magic
#	must match %.c{pp}
#	must not match _rose.c{pp}
#	must not match %.bin
rted_source/%.c: rted_source $(rted_path)/%.c
	$(call COPYING_FILE, $@)

rted_source/%.cpp: rted_source $(rted_path)/%.cpp
	$(call COPYING_FILE, $@)

rted_source/%.c: rted_source $(rted_path)/%.cpp
	$(call COPYING_FILE, $@)

rted_source/%.cpp: rted_source $(rted_path)/%.c
	$(call COPYING_FILE, $@)
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create _rose.c and _s_rose.c files
#------------------------------------------------------------------------------

rted_source/%_s_rose.c: rted_source/%_s.c
	@touch $@


rted_source/%_rose.c rted_source/%_rose.cpp: runtimeCheck rted_source/%.c rted_source/%.cpp rted_source/%_s.c rted_source/%_s.cpp $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	@echo -n "    testing if files compiles with gcc (with _s) ... "
	@$(RTED_CC) -g -o $(patsubst %_rose.c, %.bin, $@) $(filter rted_source/%.c, $^) \
		$(srcdir)/$(RTS) $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) \
		$(RTED_LIB)
	@rm $(patsubst %_rose.c, %.bin, $@)
	@echo " done."
	@echo -n "    running transformation ... "
	@./runtimeCheck 2 $(filter rted_source/%.c, $^) \
			  -rose:C_only -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) 2> /dev/null
	@echo " done."
	@echo -n "   adding line information ... "
	@cat rose_$(notdir $(patsubst %_rose.c, %.c, $@ ) ) \
	    | nl -p -b 'a' | sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $@
	@rm rose_$(notdir $(patsubst %_rose.c, %.c, $@ ) )
	@cat rose_$(notdir $(patsubst %_rose.c, %_s.c, $@ ) ) \
	    | nl -p -b 'a' | sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $(patsubst %_rose.c, %_s_rose.c, $@ )
	@rm rose_$(notdir $(patsubst %_rose.c, %_s.c, $@ ) )
	@echo " done."

rted_source/%_rose.c: runtimeCheck rted_source/%.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	@echo -n "    testing if file compiles with gcc ... "
	@$(RTED_CC) -g -o $(patsubst %_rose.c, %.bin, $@) $(filter rted_source/%.c, $^) $(srcdir)/$(RTS) $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) $(RTED_LIB)
	@rm $(patsubst %_rose.c, %.bin, $@)
	@echo " done."
	@echo -n "    running transformation ... "
	@./runtimeCheck 2 $(filter rted_source/%.c, $^) -rose:C_only -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@))  2> /dev/null
	@echo " done." > /dev/null
	@echo -n "   adding line information ... "
	@cat rose_$(notdir $(filter rted_source/%.c, $^)) | nl -p -b 'a' | sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $@
	@rm rose_$(notdir $(filter rted_source/%.c, $^))
	@echo " done."

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create the binary files
#------------------------------------------------------------------------------

#rted_source/%_debug.bin: rted_source/%_rose.c rted_source/%_s_rose.c
#	@touch $@

#rted_source/%_debug.bin: rted_source/%_rose.c
#	@touch $@

rted_source/%_debug.bin: rted_qt rted_source/%_rose.c rted_source/%_s_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented debugger code (with _s) ... "
	@$(RTED_CC) -g -o $@ $(filter rted_source/%.c, $^) \
		     $(srcdir)/$(RTS) $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER) $(RTED_LIB)
	@echo " done."

rted_source/%_debug.bin: rted_qt rted_source/%_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented debugger code ... "
	@$(RTED_CC) -g -o $@ $(filter rted_source/%.c, $^) $(srcdir)/$(RTS) $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER) $(RTED_LIB)
	@echo " done."

rted_source/%.expected-output: rted_source $(EXTRACT_EXPECTED_ERROR_SCRIPT)
	@OUTPUT_SRC='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%.ok_msg, $@))';\
		TEST_FILE='$(strip $(patsubst rted_source/%.expected-output, $(rted_path)/%.c, $@))';\
		if [ -f "$$OUTPUT_SRC" ]; then \
			echo ===`wc -l $$TEST_FILE`=== | $(EXTRACT_EXPECTED_ERROR) - $$OUTPUT_SRC > $@;\
		fi

rted_source/%.bin: rted_source/%_rose.c rted_source/%_s_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented code (with _s) ... "
	@$(RTED_CC) -g -o $@ $(filter rted_source/%.c, $^) \
		     $(srcdir)/$(RTS) $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) \
			 $(RTED_LIB)
	@echo " done."

rted_source/%.bin: rted_source/%_rose.c $(srcdir)/$(RTS) $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented code ... "
	@$(RTED_CC) -g -o $@ $(filter rted_source/%.c, $^) $(srcdir)/$(RTS) $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) $(RTED_LIB)
	@echo " done."

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

run: $(patsubst $(rted_path)/%.c, run/%, $(RTED_ORIGSOURCES) )

run_debug: $(patsubst $(rted_path)/%.c, run_debug/%, $(RTED_ORIGSOURCES) )

run_checkonly: $(patsubst $(rted_path)/%.c, run_checkonly/%, $(RTED_ORIGSOURCES) )

run_log: $(patsubst $(rted_path)/%.c, run_log/%, $(RTED_ORIGSOURCES) )


#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run all tests in a directory listed in RTED_ORIGSOURCES
#------------------------------------------------------------------------------

define RUN_TARGET
$(1): $(2)
	@echo $(1) complete
endef

${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(rted_path)/%, run/%, $(rted_dir)}, ${patsubst $(rted_path)/%.c, run/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(rted_path)/%, run_debug/%, $(rted_dir)}, ${patsubst $(rted_path)/%.c, run_debug/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(rted_path)/%, run_checkonly/%, $(rted_dir)}, ${patsubst $(rted_path)/%.c, run_checkonly/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, ${filter $(rted_path)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(rted_path)/%, run_log/%, $(rted_dir)}, ${patsubst $(rted_path)/%.c, run_log/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, ${filter $(srcdir)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(srcdir)/%, run/%, $(rted_dir)}, ${patsubst $(srcdir)/%.c, run/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, ${filter $(srcdir)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(srcdir)/%, run_debug/%, $(rted_dir)}, ${patsubst $(srcdir)/%.c, run_debug/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, ${filter $(srcdir)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(srcdir)/%, run_checkonly/%, $(rted_dir)}, ${patsubst $(srcdir)/%.c, run_checkonly/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, ${filter $(srcdir)%, $(RTED_TEST_DIRS)}, ${eval ${call RUN_TARGET, ${patsubst $(srcdir)/%, run_log/%, $(rted_dir)}, ${patsubst $(srcdir)/%.c, run_log/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
	
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
