include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = SemanticVerification libraryIdentification_tests Dwarf_tests Pin_tests

EXTRA_DIST = runTest.C arm-ctrlaltdel.ans runExecFormatsTest testShiftResizeSection.sh test_findConstants.ans \
		test_disassemble.ans testSampleBinaries.sh testSampleBinaries.conf testFunctionDetection.pl \
		functionBoundaries_1.ans functionBoundaries_2.ans functionBoundaries_3.ans

BINARY_SAMPLES = $(top_srcdir)/binaries/samples

if USE_BINARY_ANALYSIS_TESTS

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification


###############################################################################################################################
# The executables...
###############################################################################################################################

# Each executable will have a test_*.passed target for testing and a demo_* target for interactive demo. The former is the
# name of a file that is created when the test is successful, the later is a phony target.
noinst_PROGRAMS = execFormatsTest testBinCFG testBinCG testElfStrtab testElfByteOrder testElfWordSize testElfSchulz \
		testElfConstruct testPeConstruct shiftResizeSection findConstants disassemble testEtherInsns \
		expandCoreDump functionBoundaries

# Parses an executable to produce a dump file (*.dump), an assembly file (rose_*.s), and a new executable created by unparsing
# the AST (*.new). The *.new file is typically identical to the original executable.
execFormatsTest_SOURCES = execFormatsTest.C
execFormatsTest_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

#Test the binary control flow graph that uses the semantic representation to perform an accurate analysis
testBinCFG_SOURCES = testBinCFG.C
testBinCFG_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

#Test the binary call graph that uses the semantic representation to perform an accurate analysis
testBinCG_SOURCES = testBinCG.C
testBinCG_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)


# Tests ELF string table reallocation functions by changing some strings.  At first glance this would appear to be something
# quite easy to do, but it turns out to involve lots of details.
testElfStrtab_SOURCES = testElfStrtab.C
testElfStrtab_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Reads in an ELF executable and changes the byte order from little-endian to big-endian or vice versa and writes out a new
# file. Note that the byte order change affects the ELF file format but not the executable described by that format.
testElfByteOrder_SOURCES = testElfByteOrder.C
testElfByteOrder_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Reads in an ELF executable and changes the word size from eight to four bytes or vice versa and writes out a new file. Note
# that the word size change affects the ELF file format but not the executable described by that format.
testElfWordSize_SOURCES = testElfWordSize.C
testElfWordSize_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Demonstrates how to change names of dynamically-linked functions. This example reads an ELF executable dynamically linked to
# the MPI library and changes all calls to PMPI_* functions with calls to XMPI_* functions. See the top of the source file
# for more details.
testElfSchulz_SOURCES = testElfSchulz.C
testElfSchulz_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Demonstrates how to build an ELF executable from scratch. Lots of comments in the source code. The resulting file is executable
# on an x86 Linux using syscall 1 to exit with a particular value.
testElfConstruct_SOURCES = testElfConstruct.C
testElfConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Demonstrates how to build a PE executable from scratch. This demo is not as complete at the ELF version, but does show how to
# create a file that contains multiple format headers (a DOS header and a PE header).
testPeConstruct_SOURCES = testPeConstruct.C
testPeConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Changes the size and/or location of a section in an executable file.
shiftResizeSection_SOURCES = shiftResizeSection.C
shiftResizeSection_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Finds constant values and propagates them across instructions
findConstants_SOURCES = findConstants.C
findConstants_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Disassembles an executable according to various command-line switches.
disassemble_SOURCES = disassemble.C
disassemble_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

testEtherInsns_SOURCES = testEtherInsns.C
testEtherInsns_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Program to show function boundary information for a binary executable
functionBoundaries_SOURCES=functionBoundaries.C
functionBoundaries_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Program to add read-only data to ELF core dumps.
expandCoreDump_SOURCES=expandCoreDump.C
expandCoreDump_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

###############################################################################################################################
# DEMOS
#
# These demonstrate various aspects of ROSE's binary executable abilities. They are meant to be run sequentially (regardless
# of make's "-j" switch) and produce output for human consumption.
###############################################################################################################################
DEMO_PHONY_TARGETS=$(patsubst %,demo_%,$(noinst_PROGRAMS))

demos: demos-local

demos-local: $(DEMO_PHONY_TARGETS)

demo_execFormatsTest: execFormatsTest
	@echo; echo; echo "Demo for $<"; echo
	./execFormatsTest $(BINARY_SAMPLES)/buffer2.bin
	@echo "See file buffer2.bin.dump for information regarding the executable format"
	@echo "See file rose-buffer2.bin.s for disassembly of instructions"

demo_testElfStrtab: testElfStrtab demo_execFormatsTest
	@echo; echo; echo "Demo for $<"; echo
	./testElfStrtab $(BINARY_SAMPLES)/arm-poweroff

demo_testElfByteOrder: testElfByteOrder demo_testElfStrtab
	@echo; echo; echo "Demo for $<"; echo
	./testElfByteOrder $(BINARY_SAMPLES)/arm-poweroff

demo_testElfWordSize: testElfWordSize demo_testElfByteOrder
	@echo; echo; echo "Demo for $<"; echo
	./testElfWordSize $(BINARY_SAMPLES)/arm-poweroff

demo_testElfSchulz: testElfSchulz demo_testElfWordSize
	@echo; echo; echo "Demo for $<"; echo
	./testElfSchulz $(BINARY_SAMPLES)/schulz-sample1.so
	./testElfSchulz $(BINARY_SAMPLES)/schulz-sample2.so

demo_testElfConstruct: testElfConstruct demo_testElfSchulz
	@echo; echo; echo "Demo for $<"; echo
	./testElfConstruct

demo_testPeConstruct: testPeConstruct demo_testElfConstruct
	@echo; echo; echo "Demo for $<"; echo
	./testPeConstruct

demo_shiftResizeSection: shiftResizeSection demo_testPeConstruct
	@echo; echo; echo "Demo for $<"; echo
	$(srcdir)/testShiftResizeSection.sh $(BINARY_SAMPLES)/arm-ctrlaltdel >arm-ctrlaltdel.ans

demo_findConstants: findConstants demo_shiftResizeSection
	@echo; echo; echo "Demo for $<"; echo
	./findConstants $(BINARY_SAMPLES)/i686-test1.O3.bin

demo_disassemble: disassemble demo_findConstants
	@echo; echo; echo "Demo for $<"; echo
	./disassemble -rose:disassembler_search following,immediate,words,allbytes,unused,nonexe,deadend,unknown $(BINARY_SAMPLES)/i686-test1.O0.bin
	@echo "output has been dumped to i686-test1.O0.bin.dump"

demo_testBinCFG: testBinCFG demo_disassemble
	: no demo yet for testBinCFG

demo_expandCoreDump: expandCoreDump demo_testBinCFG
	: no demo yet for expandCoreDump

demo_functionBoundaries: functionBoundaries demo_exapndCoreDump
	@echo; echo; echo "Demo for $<";
	@echo "The functionBoundaries tool prints information about what binary functions were detected and why."
	./functionBoundaries $(BINARY_SAMPLES)/i686-test1.O3.bin

###############################################################################################################################
# TESTS
#
# These are pretty much the same as the demos except they compare the new output with expected output and fail if there
# are any differences.
#
# Each passing test creates a *.passed file so that a subsequent "make" doesn't need to rerun the test.
#
# Some of the targets depend on other tests because ROSE should not execute concurrently in the same working directory for
# specimen binaries having the same names due to ROSE creating files in the CWD based on the base name of the specimen.
###############################################################################################################################
TEST_PASSED_TARGETS=$(patsubst %,test_%.passed,$(noinst_PROGRAMS)) test_assembler.passed

test_execFormatsTest.passed: execFormatsTest
	: $< not currently tested
	@touch $@

test_testBinCFG.passed: testBinCFG
	$(VALGRIND) ./testBinCFG $(BINARY_SAMPLES)/libm-2.3.6.so
	@touch $@

test_testBinCG.passed: testBinCG
	$(VALGRIND) ./testBinCG $(BINARY_SAMPLES)/libm-2.3.6.so
	@touch $@

test_testElfStrtab.passed: testElfStrtab
	$(VALGRIND) ./testElfStrtab $(BINARY_SAMPLES)/arm-poweroff
	@touch $@

# cannot run in parallel with testElfStrtab
test_testElfByteOrder.passed: testElfByteOrder test_testElfStrtab.passed
	$(VALGRIND) ./testElfByteOrder $(BINARY_SAMPLES)/arm-poweroff
	$(VALGRIND) ./execFormatsTest ./arm-poweroff.new
	@if diff -u ./arm-poweroff.new{,.new}; then :; else echo "Files differ; byte order test failed"; exit 1; fi
	@touch $@

# cannot run in parallel with testElfStrtab or testElfByteOrder
test_testElfWordSize.passed: testElfWordSize test_testElfStrtab.passed test_testElfByteOrder.passed
	$(VALGRIND) ./testElfWordSize -rose:read_executable_file_format_only $(BINARY_SAMPLES)/arm-poweroff
	$(VALGRIND) ./execFormatsTest -rose:read_executable_file_format_only ./arm-poweroff.new
	@if diff -u ./arm-poweroff.new{,.new}; then								\
		: ok;												\
	else													\
		sed -n '0,/^Section/p' <arm-poweroff.dump;							\
		hexdump -Cv arm-poweroff.new >arm-poweroff.new.hexump;						\
		hexdump -Cv arm-poweroff.new.new >arm-poweroff.new.new.hexump;					\
		diff -u arm-poweroff.new{,.new}.hexdump |head -n100;						\
		rm -f arm-poweroff.new{,.new}.hexdump;								\
		echo "Files differ; word size test failed";							\
		exit 1;												\
	fi
	@touch $@

test_testElfSchulz.passed: test_testElfSchulz-1.passed test_testElfSchulz-2.passed
	@touch $@

test_testElfSchulz-1.passed: testElfSchulz
	$(VALGRIND) ./testElfSchulz $(BINARY_SAMPLES)/schulz-sample1.so
	$(VALGRIND) ./execFormatsTest ./schulz-sample1.so.new
	$(VALGRIND) ./execFormatsTest $(BINARY_SAMPLES)/schulz-sample1-p.so
	@if diff -u schulz-sample1{-p.so,.so.new}.dump; then :; \
	    else echo "Dumps differ; Schulz test-1 failed"; exit 1; fi
	@if diff $(BINARY_SAMPLES)/schulz-sample1-p.so schulz-sample1.so.new; then :; \
	    else echo "Libs differ; Schulz test-1 failed"; exit 1; fi
	@touch $@

test_testElfSchulz-2.passed: testElfSchulz
	$(VALGRIND) ./testElfSchulz $(BINARY_SAMPLES)/schulz-sample2.so
	$(VALGRIND) ./execFormatsTest ./schulz-sample2.so.new
	$(VALGRIND) ./execFormatsTest $(BINARY_SAMPLES)/schulz-sample2-p.so
	@if diff -u schulz-sample2{-p.so,.so.new}.dump; then :; \
	    else echo "Dumps differ; Schulz test-2 failed"; exit 1; fi
	@if diff $(BINARY_SAMPLES)/schulz-sample2-p.so schulz-sample2.so.new; then :; \
	    else echo "Libs differ; Schulz test-2 failed"; exit 1; fi
	@touch $@

test_so_file:
	$(VALGRIND) ./execFormatsTest $(BINARY_SAMPLES)/libm-2.3.6.so

test_testElfConstruct.passed: testElfConstruct
	$(VALGRIND) ./testElfConstruct
	@touch $@

test_testPeConstruct.passed: testPeConstruct
	$(VALGRIND) ./testPeConstruct
	@touch $@

test_shiftResizeSection.passed: shiftResizeSection
	$(srcdir)/testShiftResizeSection.sh $(BINARY_SAMPLES)/arm-ctrlaltdel >arm-ctrlaltdel.ans
	@if diff -u {$(srcdir),.}/arm-ctrlaltdel.ans; then :; else echo "Answer differs from expected"; exit 1; fi
	@touch $@

# This test is *very* sensitive to changes to the Disassembler and Partitioner classes because they and this test all use
# the findConstants instruction semantics. If the Disassembler or Partitioner is even slightly changed, it could make a
# different number of calls to the instruction semantics, which changes the sequence number used to generate constant
# names. Therefore, when comparing the test output with the answer, we will change all constant names from "v[0-9]+" to
# "vXXX".  Any problems in the semantic analysis should still be caught since they would presumably affect lots of
# other output too.  The test is still sensitive to minor formatting changes in unparseInstruction().
FIND_CONSTANTS_DIFF=diff <(sed "s/v[0-9][0-9]*/vXXX/g" test_findConstants.ans) \
                         <(sed "s/v[0-9][0-9]*/vXXX/g" $(srcdir)/test_findConstants.ans)
test_findConstants.passed: findConstants
	$(VALGRIND) ./findConstants $(BINARY_SAMPLES)/i686-test1.O3.bin >test_findConstants.ans
	@bash -c 'if $(FIND_CONSTANTS_DIFF); then \
          echo "*** $@: Result (test_findConstants.ans) differs from expected answer in $(srcdir)"; \
	  echo "    (You should compare the files manually to verify the result, then copy it to"; \
	  echo "     $(srcdir) if it looks OK.) Tail end of the difference is:"; \
	  $(FIND_CONSTANTS_DIFF) |head -n100 |sed "s/^/findConstants> /"; \
	  exit 1 ; \
	fi'
	@touch $@

test_disassemble.passed: disassemble
	$(VALGRIND) ./disassemble --quiet -rose:disassembler_search following,immediate,words,-allbytes,unused,nonexe,deadend,-unknown $(BINARY_SAMPLES)/i686-test1.O0.bin
	@mv i686-test1.O0.bin.dump test_disassemble.ans
#	Commenting this out on only RedHat 32-bit systems is not enough (also fails on Max OSX; for the same reason).
if !OS_VENDOR_REDHAT_32BIT
#	@if diff -u {$(srcdir),.}/test_disassemble.ans; then :; else echo "Answer differs from expected"; exit 1; fi
endif
	@touch $@

# disassemble will exit non-zero if the assembly doesn't result in an encoding identical to the original
# We're using the buffer2.bin from projects/BinQ for now because it's 64-bit; the one in our own srcdir is 32-bit and
# the assembler isn't completely working yet for 32-bit. [RPM 2009-07-21]
test_assembler.passed: disassemble
	$(VALGRIND) ./disassemble --quiet --reassemble --show-bad $(top_srcdir)/projects/BinQ/buffer2.bin
	@touch $@

# This tests lots of sample binaries. We cannot run it concurrently with the other tests because some of the output
# names could conflict with the other tests (we don't know what files are in the sample binaries directory).
# NOTE: When we get too many tests to reasonably run serially, we can split them into multiple make rules, where each
# rule either runs a single subdirectory of samples and/or each rule runs different tests by referencing different
# configuration files. Don't forget to update the mostlyclean-local rule as well.
test_samples.passed: disassemble execFormatsTest $(srcdir)/testSampleBinaries.sh $(srcdir)/testSampleBinaries.conf
	@if $(srcdir)/testSampleBinaries.sh $(BINARY_SAMPLES) $(srcdir)/testSampleBinaries.conf 99>&1 >test_samples.out 2>&1; \
	then \
	   touch $@; \
	else \
	   echo "Full output is in `pwd`/test_samples.out"; \
	   false; \
	fi

test_testEtherInsns.passed: testEtherInsns
	@echo "testEtherInsns cannot be run automatically--it requires human interaction.     (SKIPPED)"

test_expandCoreDump.passed: expandCoreDump
	@echo "expandCoreDump is not currently tested."

# Tests for detecting the locations of functions, written this way for parallelism.
test_functionBoundaries_1.passed: functionBoundaries $(BINARY_SAMPLES)/i686-test1.O0.bin test_disassemble.passed
	./functionBoundaries $(BINARY_SAMPLES)/i686-test1.O0.bin >functionBoundaries_1.ans
	diff -u {$(srcdir),.}/functionBoundaries_1.ans
	touch $@
test_functionBoundaries_2.passed: functionBoundaries $(BINARY_SAMPLES)/i686-test1.O3.bin test_findConstants.passed
	./functionBoundaries $(BINARY_SAMPLES)/i686-test1.O3.bin >functionBoundaries_2.ans
	diff -u {$(srcdir),.}/functionBoundaries_2.ans
	touch $@
test_functionBoundaries_3.passed: functionBoundaries $(BINARY_SAMPLES)/i686-test1.O3-stripped.bin
	./functionBoundaries $(BINARY_SAMPLES)/i686-test1.O3-stripped.bin >functionBoundaries_3.ans
	diff -u {$(srcdir),.}/functionBoundaries_3.ans
	touch $@
test_functionBoundaries.passed: test_functionBoundaries_1.passed test_functionBoundaries_2.passed test_functionBoundaries_3.passed
	touch $@

###############################################################################################################################
# Miscellaneous....
###############################################################################################################################

# Don't delete things indiscriminately--sometimes developers have other related files that should not be touched!
MOSTLYCLEANFILES = \
        $(TEST_PASSED_TARGETS) test_testElfSchulz-{1,2}.passed \
	a.out arm-ctrlaltdel.ans arm-ctrlaltdel.dump-{1,2,3} arm-ctrlaltdel.new.new \
	arm-poweroff.{new.dump,new.new} rose_arm-ctrlaltdel.new.s rose_arm-poweroff.new.s \
	rose_schulz-sample{1,2}.so.new.s schulz-sample{1,2}.so.{new.dump,new.new} test_findConstants.ans \
	test_disassemble.ans test_samples.{passed,out} \
	test_functionBoundaries_{1,2,3}.passed functionBoundaries_{1,2,3}.ans

mostlyclean-local:
	$(srcdir)/testSampleBinaries.sh --cleanup $(BINARY_SAMPLES) $(srcdir)/testSampleBinaries.conf


###############################################################################################################################
# The older part of this makefile...
###############################################################################################################################

## Don't use the repository in ../src
#CXX_TEMPLATE_REPOSITORY_PATH = .

if ROSE_USE_SQLITE_DATABASE

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runTest testLibraryDb

testLibraryDb_SOURCES = testLibraryDb.C
# testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
testLibraryDb_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

test1: testLibraryDb
	./testLibraryDb
	rm test.db
else

bin_PROGRAMS  = runTest
test1:
	: $@ is only for ROSE_USE_SQLITE_DATABASE

endif
runTest_SOURCES = runTest.C

# testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
runTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#TESTCODES = buffer2.bin

# Build the list of object files
#TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -g -rose:verbose 0
# ROSE_FLAGS = 

TEST_TRANSLATOR = ./runTest all $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
#$(TEST_Objects): runTest $(TESTCODES) 
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	env SRCDIR="$(srcdir)" $(TEST_TRANSLATOR)

CLEANFILES = unparsed.s callgraph.gml output _binary_tree.dot cfg.dot dfg.dot Templates.DB 

# DQ (3/21/2009): These tests don't appear to run under Cygwin.
if ROSE_BUILD_OS_IS_CYGWIN
check-local:
	@echo "*****************************************************************************************"
	@echo "*** ROSE/tests/roseTests/binaryTests: (commented out for Cygwin) ***"
	@echo "*****************************************************************************************"
else

# DQ (4/8/2009): I have removed the requirement to run rule: test_so_file (added by Thomas)
# I have left the rule in place so that we can have it availalbe for testing (but it only 
# works if BinQ is built which is not the default configuration).
# check-local: runTest test1 $(TEST_PASSED_TARGETS) test_so_file
# Caution: runTest generates the same output files as some $(TEST_PASSED_TARGETS) [RPM 2009-07-21]
check-local: runTest test1 $(TEST_PASSED_TARGETS)
	./runTest $(BINARY_SAMPLES)/buffer2.bin > output
	$(MAKE) test_samples.passed
	@echo "*********************************************************************"
	@echo "*** ROSE/tests/roseTests/binaryTests: passed ***"
	@echo "*********************************************************************"
endif

# Don't delete things indiscriminately!
clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) ii_files ti_files DATABASE DATABASE.dot cfg_x86.dot  cg_x86.dot

distclean-local:
	rm -rf Templates.DB 

else

check-local:
	@echo "**********************************************************************"
	@echo "*** ROSE/tests/roseTests/binaryTests: skipped ***"
	@echo "**********************************************************************"

endif
