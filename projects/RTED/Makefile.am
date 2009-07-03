include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_RTED

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I. -Winvalid-pch 
INCLUDES2 = $(ROSE_INCLUDES) -I../../



if ROSE_WITH_ROSEQT
#    LINK_QT_DEBUGGER=./rted_qt/.libs/libRtedQt.a $(QT_LDFLAGS) $(ROSE_DWARF_LIBS_WITH_PATH)  -L$(top_builddir)/projects/roseQt/widgets/.libs/ -lRoseQtWidgets -Wl,-rpath,`pwd`/$(top_builddir)/projects/roseQt/widgets/.libs -DROSE_WITH_ROSEQT
if ROSE_USE_DWARF
    LINK_QT_DEBUGGER=./rted_qt/.libs/libRtedQt.a $(QT_LDFLAGS) -L$(dwarf_path) -ldwarf -lelf  -L$(top_builddir)/projects/roseQt/widgets/.libs/ -lRoseQtWidgets -Wl,-rpath,`pwd`/$(top_builddir)/projects/roseQt/widgets/.libs -DROSE_WITH_ROSEQT
else
    LINK_QT_DEBUGGER=./rted_qt/.libs/libRtedQt.a $(QT_LDFLAGS) -L$(top_builddir)/projects/roseQt/widgets/.libs/ -lRoseQtWidgets -Wl,-rpath,`pwd`/$(top_builddir)/projects/roseQt/widgets/.libs -DROSE_WITH_ROSEQT
endif
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
                       RtedTransf_support.cpp RtedTransf_funccall.cpp \
                       RtedTransf_locals.cpp RtedTransf_scopestmt.cpp \
                       RtedTransf_funcdef.cpp



SUBDIRS = rted_qt tests CppRuntimeSystem

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
		 $(rted_path)/C/C99_specific_errors

RTED_ORIGSOURCES = ${foreach RTED_TEST_DIR, $(RTED_TEST_DIRS), ${wildcard $(RTED_TEST_DIR)/*[!_s].c} }
RTED_COPYSOURCES = ${foreach RTED_TEST_DIR, $(RTED_TEST_DIRS), ${patsubst $(rted_path)/%, rted_source/%, ${wildcard $(RTED_TEST_DIR)/*.c} } }
RTED_ROSESOURCES = ${patsubst %.c, %_rose.c, $(RTED_COPYSOURCES) }
RTED_BINFILES = ${patsubst $(rted_path)/%.c, rted_source/%.bin, $(RTED_ORIGSOURCES)}
RTED_DEBUGBINFILES = ${patsubst $(rted_path)/%.c, rted_source/%_debug.bin, $(RTED_ORIGSOURCES)}

.SECONDARY: $(RTED_BINFILES) $(RTED_DEBUGBINFILES) $(RTED_COPYSOURCES) $(RTED_ROSESOURCES)

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
	@mkdir -p $(foreach rted_dir, $(RTED_TEST_DIRS), $(patsubst $(rted_path)/%, rted_source/%, $(rted_dir) ) )

#------------------------------------------------------------------------------
# rule to copy the sources into our current working directory
#------------------------------------------------------------------------------

rted_source/%.c: rted_source $(rted_path)/%.c
	@echo -e Checking $(patsubst $(rted_path)/%.c, %, $(filter $(rted_path)%, $^)) ...
	@echo -n "    copying input file ... "
	@cp $(filter $(rted_path)/%.c, $^) $(dir $@)
	@echo " done."
	@echo -n "    inserting #include \"RuntimeSystem.h\" ... "
	@sed -i '1i\#include \"RuntimeSystem.h\"' $@
	@echo " done."
	@echo -n "    inserting #define EXITCODE_OK 0 ... "
	@sed -i '1i\#define EXITCODE_OK 0' $@
	@echo " done."

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create _rose.c and _s_rose.c files
#------------------------------------------------------------------------------

rted_source/%_s_rose.c: rted_source
	@touch $@

rted_source/%_rose.c: runtimeCheck rted_source/%.c rted_source/%_s.c $(srcdir)/RuntimeSystem.c $(srcdir)/RuntimeSystem.h
	@echo -n "    testing if files compiles with gcc ... "
	@$(CXX) -g -o $(patsubst %_rose.c, %.bin, $@) $(filter rted_source/%.c, $?) \
		$(srcdir)/RuntimeSystem.c $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir)
	@rm $(patsubst %_rose.c, %.bin, $@)
	@echo " done."
	@echo -n "    running transformation ... "
	@./runtimeCheck 2 $(filter rted_source/%.c, $?) \
			  -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@))
	@echo " done."
	@echo -n "   adding line information ... "
	@cat rose_$(notdir $(patsubst %_rose.c, %.c, $@ ) ) \
	    | nl -p -b 'a' | sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $@
	@rm rose_$(notdir $(patsubst %_rose.c, %.c, $@ ) )
	@cat rose_$(notdir $(patsubst %_rose.c, %_s.c, $@ ) ) \
	    | nl -p -b 'a' | sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $(patsubst %_rose.c, %_s_rose.c, $@ )
	@rm rose_$(notdir $(patsubst %_rose.c, %_s.c, $@ ) )
	@echo " done."

rted_source/%_rose.c: runtimeCheck rted_source/%.c $(srcdir)/RuntimeSystem.c $(srcdir)/RuntimeSystem.h
	@echo -n "    testing if file compiles with gcc ... "
#	echo $(filter rted_source/%.c, $?)
#	exit 1
	@$(CXX) -g -o $(patsubst %_rose.c, %.bin, $@) $(filter rted_source/%.c, $?) $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir)
	@rm $(patsubst %_rose.c, %.bin, $@)
	@echo " done."
	@echo -n "    running transformation ... "
	@./runtimeCheck 2 $(filter rted_source/%.c, $?) -rose:Cxx -c $(INCLUDES) -I$(top_builddir) -I$(srcdir) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@))
	@echo " done."
	@echo -n "   adding line information ... "
	@cat rose_$(notdir $(filter rted_source/%.c, $?)) | nl -p -b 'a' | sed -e 's/^ \+\([0-9]\+\)\t\(.*\)x%%x\(.*\)/\2\1\3/; t end; s/^ \+\([0-9]\+\)\t//; : end' > $@
	@rm rose_$(notdir $(filter rted_source/%.c, $?))
	@echo " done."

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to create the binary files
#------------------------------------------------------------------------------

#rted_source/%_debug.bin: rted_source/%_rose.c rted_source/%_s_rose.c
#	@touch $@

#rted_source/%_debug.bin: rted_source/%_rose.c
#	@touch $@

rted_source/%_debug.bin: rted_qt rted_source/%_rose.c rted_source/%_s_rose.c $(srcdir)/RuntimeSystem.c $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented debugger code ... "
	@$(CXX) -g -o $@ $(filter rted_source/%.c, $?) \
		     $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)
	@echo " done."

rted_source/%_debug.bin: rted_qt rted_source/%_rose.c $(srcdir)/RuntimeSystem.c $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented debugger code ... "
	@$(CXX) -g -o $@ $(filter rted_source/%.c, $?) $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir) $(LINK_QT_DEBUGGER)
	@echo " done."

rted_source/%.bin: rted_source/%_rose.c rted_source/%_s_rose.c $(srcdir)/RuntimeSystem.c $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented code ... "
	@$(CXX) -g -o $@ $(filter rted_source/%.c, $?) \
		     $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir)
	@echo " done."

rted_source/%.bin: rted_source/%_rose.c $(srcdir)/RuntimeSystem.c $(srcdir)/RuntimeSystem.h
	@echo -n "   compiling instrumented code ... "
	@$(CXX) -g -o $@ $(filter rted_source/%.c, $?) $(srcdir)/RuntimeSystem.c $(INCLUDES) -I$(dir $(patsubst rted_source/%, $(rted_path)/%, $@)) -I$(top_builddir) -I$(srcdir)
	@echo " done."

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run arbitrary tests from the rted distribution
#------------------------------------------------------------------------------

run_debug/%: rted_source/%_debug.bin
	@echo -n "   running runtime error detection debugger ... "
	@./$(filter rted_source/%_debug.bin, $?)
	@echo " done."

run_checkonly/%: rted_source/%.bin
	@echo -n "   running runtime error detection ... "
	@./$(filter rted_source/%.bin, $?)
	@echo " done."

run/%: rted_source/%.bin rted_source/%_debug.bin
	@echo -n "   running runtime error detection ... "
	@$(patsubst %_debug.bin, ./%.bin, $(filter rted_source/%_debug.bin, $?) ); \
	    if [ $$? -ne 0 ]; \
	    then \
		echo " failed."; \
		echo "   running debugger ... "; \
		$(patsubst %.bin, ./%_debug.bin, $<); \
		exit 1; \
	    fi 
	@echo " done."

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run all tests in the directories listed in RTED_ORIGSOURCES
#------------------------------------------------------------------------------

run: $(patsubst $(rted_path)/%.c, run/%, $(RTED_ORIGSOURCES) )

run_debug: $(patsubst $(rted_path)/%.c, run_debug/%, $(RTED_ORIGSOURCES) )

run_checkonly: $(patsubst $(rted_path)/%.c, run_checkonly/%, $(RTED_ORIGSOURCES) )

#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# rules to run all tests in a directory listed in RTED_ORIGSOURCES
#------------------------------------------------------------------------------

define RUN_TARGET
$(1): $(2)
	@echo $(1) complete
endef

${foreach rted_dir, $(RTED_TEST_DIRS), ${eval ${call RUN_TARGET, ${patsubst $(rted_path)/%, run/%, $(rted_dir)}, ${patsubst $(rted_path)/%.c, run/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, $(RTED_TEST_DIRS), ${eval ${call RUN_TARGET, ${patsubst $(rted_path)/%, run_debug/%, $(rted_dir)}, ${patsubst $(rted_path)/%.c, run_debug/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
${foreach rted_dir, $(RTED_TEST_DIRS), ${eval ${call RUN_TARGET, ${patsubst $(rted_path)/%, run_checkonly/%, $(rted_dir)}, ${patsubst $(rted_path)/%.c, run_checkonly/%, ${wildcard $(rted_dir)/*[!_s].c} } } } }
	
#------------------------------------------------------------------------------

deleteAllbinfiles:
	rm -rf rted_source

c-compile-runtimesystem:
	@echo "Make sure that RuntimeSystem compiles with C"

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: rted_qt runtimeCheck $(TESTCODES) c-compile-runtimesystem  deleteAllbinfiles run_checkonly/C/array_index_out_of_bound run_checkonly/C/string_errors run_checkonly/C/input_output_errors
	@echo "***************************************************************************************"
	@echo "*** ROSE/projects/runtimeErrorCheck: make check rule complete (terminated normally) ***"
	@echo "***************************************************************************************"


clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test_bin* *.o rose_*.C rose_*.c *.dot Templates.DB *~ rose_rose* test *.bin *.txt *.c *.cpp rted_source 

endif
