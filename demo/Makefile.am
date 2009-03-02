include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# These demos require QT to support the QRose work used for the GUI in several ROSE tools
# that are a part of this demo.
if ROSE_USE_QT

INCLUDES = $(ROSE_INCLUDES) 

# There are no executables built as part of this demo (so far)
noinst_PROGRAMS = 

TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = 

# ******************************************************
#              Standard Makefile Content
# ******************************************************

# This rule will run all the demos in sequence, the user should exit each program to 
# proceed to the next part of each demo or the next demo To configure ROSE to run the 
# demo requires some optional support, Expected configure line:
# <path>/configure --with-boost=<path> --with-qt=<path> --with-sqlite3=<path> --with-dwarf=<path> --enable-assembly-semantics
# At present there is only a binary analysis demo, there will be other demos in the future.
demo:
	$(MAKE) demo_binary_analysis


# This is demo specific to binary anslysis support in ROSE.
demo_binary_analysis_A:
	$(MAKE) demo_1
	sleep 10
	$(MAKE) demo_2
	sleep 10
	$(MAKE) demo_3
	sleep 10
	$(MAKE) demo_4
	sleep 10
	$(MAKE) demo_5
	sleep 10
	$(MAKE) demo_6
	sleep 10

demo_binary_analysis_B:
	$(MAKE) demo_7a
	sleep 10
	$(MAKE) demo_7b
	sleep 10
	$(MAKE) demo_8a
	sleep 10
	$(MAKE) demo_8b
	sleep 10
	$(MAKE) demo_9
	sleep 10
	$(MAKE) demo_10
	sleep 10

demo_binary_analysis_C:
	$(MAKE) demo_11
	sleep 10
	$(MAKE) demo_12
	sleep 10
	$(MAKE) demo_13
	sleep 10
	$(MAKE) demo_14
	@echo "End of binary_analysis demos"


# Demo of Async Signal Handling test on binary (similar to Compass checker on source code)
# Note to giving this demo: C^s on 3 second sleep intervals and C^q to continue.
demo_1:
	cd $(top_builddir)/developersScratchSpace/Robb/DynamicLinking; $(MAKE) demo
	@echo "End of demo_1"

# This demo show the handling of different formats (ELF and PE) and different instruction sets (ARM, PowerPC, x86).
demo_2: demo_2_test
	emacs $(top_builddir)/developersScratchSpace/Dan/Disassembler_tests/asm_code_samples_gcc.exe.dump
	@echo "End of demo_2"

demo_2_test:
	cd $(top_builddir)/developersScratchSpace/Dan/Disassembler_tests; $(MAKE) demo_binary_file_format_Linux
	cd $(top_builddir)/developersScratchSpace/Dan/Disassembler_tests; $(MAKE) demo_with_instructions_Linux
	cd $(top_builddir)/developersScratchSpace/Dan/Disassembler_tests; $(MAKE) demo_binary_file_format_PE
	cd $(top_builddir)/developersScratchSpace/Dan/Disassembler_tests; $(MAKE) demo_with_instructions_PE
	cd $(top_builddir)/developersScratchSpace/Dan/Disassembler_tests; $(MAKE) demo_binary_file_format_ARM
	cd $(top_builddir)/developersScratchSpace/Dan/Disassembler_tests; $(MAKE) demo_with_instructions_ARM

# This demo shows the generation of a few different forms of analysis on binaries
demo_3: demo_3_test
	@echo "Run callgraph analysis, control flow analysis, and data flow analysis"
	cd $(top_builddir)/projects/BinQ; $(MAKE) demo_withoutAnalysisRunInBatchMode;
	cd $(top_builddir)/projects/BinQ; zgrviewer callgraph.dot &
	cd $(top_builddir)/projects/BinQ; zgrviewer cfg.dot &
	cd $(top_builddir)/projects/BinQ; zgrviewer dfg.dot &
	@echo "End of demo_3"

demo_3_test:
	cd $(top_builddir)/projects/BinQ; $(MAKE) demo_show_source_code;

# Example of buffer overflow detector
demo_4:
	cd $(top_builddir)/projects/BinQ; $(MAKE) demo_withAnalysisRunInBatchMode
	@echo "End of demo_4"

# Need another demo 2.5) Build a binary and test the forbidden function analysis 

# Something that might require type analysis
demo_5:
	cd $(top_builddir)/projects/BinQ; $(MAKE) demo_rted_example
	@echo "End of demo_5"

# Comparing ROSE vs. IDAPro (on same 32-bit example)
demo_6:
	cd $(top_builddir)/projects/BinQ; $(MAKE) demo_rose_vs_IDAPro
	@echo "End of demo_6"

# Running Compass in ASCII mode (non-stripped binary)
demo_7a:
	cd $(top_builddir)/projects/compass/tools/compass; $(MAKE) demo_small
	@echo "End of demo_7a"

# Running Compass in ASCII mode (stripped binary)
demo_7b:
	cd $(top_builddir)/projects/compass/tools/compass; $(MAKE) demo_linux
	@echo "End of demo_7b"

# Running Compass in GUI mode (on a source code)
demo_8a:
	cd $(top_builddir)/projects/compass/tools/compass; $(MAKE) demo_compassGUI_source
	@echo "End of demo_8a"

# Running Compass in GUI mode (on a binary; note output sent to terminal at present)
demo_8b:
	cd $(top_builddir)/projects/compass/tools/compass; $(MAKE) demo_compassGUI_binary
	@echo "End of demo_8b"

# This test the construction of a new checker for Compass (to be submitted to Compass).
# Note that this demo removes a checker directory (relative to the source tree) and then rebuilds it.
# To make this safer we do it in two steps to avoid using "-r" with rm.
demo_9:
	@echo "Remove existing checker: $(top_srcdir)/../CompassCheckers/demoBufferOverflowCheckerTest"
	mkdir -p $(top_srcdir)/../CompassCheckers/demoBufferOverflowCheckerTest
	rm -f $(top_srcdir)/../CompassCheckers/demoBufferOverflowCheckerTest/*
	rmdir $(top_srcdir)/../CompassCheckers/demoBufferOverflowCheckerTest
	cd $(top_srcdir)/../CompassCheckers; $(top_srcdir)/projects/compass/src/compass_scripts/gen_checker.sh demo buffer overflow checker test
	@echo "Built checker directory $(top_srcdir)/../CompassCheckers/demoBufferOverflowCheckerTest"
	@echo "End of demo_9"

# The demo shows the translation of a binary into a low level representation to support general 
# instrcution set independent analysis.  Also demonstrated disassemble and semantic analysis of 
# PowerPC instruction set.  The semantic analysis of binaries is supported for x86 and PowerPC
# (but not yet for ARM).
demo_10:
	cd $(top_builddir)/projects/assemblyToSourceAst; $(MAKE) ppc-check
	@echo "End of demo_10"

# Demonstrate edit graph work to support binary comparision on binary file format (related to binary diff on instructions)
demo_11a:
	cd $(top_builddir)/developersScratchSpace/Dan/astEquivalence_tests; $(MAKE) demoSourceEquivalence
	cd $(top_builddir)/developersScratchSpace/Dan/astEquivalence_tests; $(MAKE) demoBinaryEquivalence
	@echo "End of demo_11a"

# Demonstrate AST diff using BinQ on binary instructions (only on the instructions)
demo_11b:
	cd $(top_builddir)/projects/BinQ; $(MAKE) demo_diff
	@echo "End of demo_11b"

# Using Compass with large projects
demo_12:
	cd $(top_builddir)/projects/compass/tools/compass/gui; $(MAKE) demo
	@echo "End of demo_12"

# Static binary rewriting
demo_13:
	cd $(top_builddir)/developersScratchSpace/Dan/translator_tests; $(MAKE) demo
	@echo "End of demo_13"

# Template for more rules...
# demo_15:
#	cd $(top_builddir)/; $(MAKE) xxx
#	@echo "End of demo_15"








testProgram_1: $(srcdir)/testProgram.c
	gcc -g -o testProgram_1 $(srcdir)/testProgram.c

testProgram_2: $(srcdir)/testProgram.c
	gcc -o testProgram_2 $(srcdir)/testProgram.c

# Test of astEquivalence on Binary Executable
testBinaryEquivalence: astDiff testProgram_1 testProgram_2
	./astDiff -rose:read_executable_file_format_only testProgram_1 testProgram_2

# Test of astEquivalence on Source Code
testSourceEquivalence: astDiff $(srcdir)/testProgram.c $(srcdir)/testProgram_alt.c
	cat $(srcdir)/testProgram.c $(srcdir)/testProgram_alt.c
	./astDiff $(srcdir)/testProgram.c $(srcdir)/testProgram_alt.c

testBoost: shortestPathBoostExample
	./shortestPathBoostExample

# This demo generates the edit graph for two similar source code files
# and displays the generated visualization of the graph using zgrviewer.
demoSourceEquivalence: testSourceEquivalence
	zgrviewer output.dot &

# This demo generates the edit graph for two similar binary files (one
# compiled with debug and the other without), and displays a pre-generated
# image of a visualization of the a quadrant of the whole graph (takes 
# 20+ minutes to layout in dot so the image has been pre-computed).
demoBinaryEquivalence: testBinaryEquivalence
	eog $(srcdir)/astEquivalenceGraphForBinaryCode3.png &



# ******************************************************
#              Standard Makefile Content
# ******************************************************

check-local:
	@echo "ROSE demo."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "Testing demos..." 
	make demo_2_test
	make demo_3_test
	make demo_10
#	make demo_13
	@echo "****************************************************************************"
	@echo "******** ROSE/demo: make check rule complete (terminated normally) *********"
	@echo "****************************************************************************"
else
check-local:
	@echo "\nROSE demo requires Qt configure option (at least), Demo NOT run.  To run these reconfigure ROSE (see manual)"
	@echo "***********************************************************************************************************************************************************"
	@echo "**************************************************************** make check not run here: DEMO ************************************************************"
	@echo "*** Expected configure line: <...>/configure --with-boost=<path> --with-qt=<path> --with-sqlite3=<path> --with-dwarf=<path> --enable-assembly-semantics ***"
	@echo "***********************************************************************************************************************************************************"
endif

EXTRA_DIST = demo.txt

# There should be no temporary files generated as part of the demo (in this directory).
clean-local:
	rm -f *.o 

