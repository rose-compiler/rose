include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = SemanticVerification libraryIdentification_tests Dwarf_tests Pin_tests

EXTRA_DIST = \
	runExecFormatsTest testShiftResizeSection.sh \
        testSampleBinary.sh testSampleBinaries.conf testFunctionDetection.pl \
	bincfg.h

# Forward declarations
STATIC_TEST_TARGETS=
noinst_PROGRAMS=
MOSTLYCLEANFILES=

if USE_BINARY_ANALYSIS_TESTS

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification

###############################################################################################################################
# The executables and their test cases...
###############################################################################################################################


# Parses an executable to produce a dump file (*.dump), an assembly file (rose_*.s), and a new executable created by unparsing
# the AST (*.new). The *.new file is typically identical to the original executable.
noinst_PROGRAMS += execFormatsTest
execFormatsTest_SOURCES = execFormatsTest.C
execFormatsTest_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += execFormatsTest.passed
EXTRA_DIST += execFormatsTest.conf
execFormatsTest.passed: execFormatsTest.conf execFormatsTest
	@$(RTH_RUN) $< $@


#Test the binary control flow graph that uses the semantic representation to perform an accurate analysis
noinst_PROGRAMS += testBinCFG
testBinCFG_SOURCES = testBinCFG.C
testBinCFG_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testBinCFG.passed
EXTRA_DIST += testBinCFG.conf
MOSTLYCLEANFILES += cfg_x86.dot
testBinCFG.passed: testBinCFG.conf testBinCFG
	@$(RTH_RUN) INPUT=libm-2.3.6.so $< $@


#Test the binary call graph that uses the semantic representation to perform an accurate analysis
noinst_PROGRAMS += testBinCG
testBinCG_SOURCES = testBinCG.C
testBinCG_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testBinCG.C
EXTRA_DIST += testBinCG.conf
MOSTLYCLEANFILES += cfg_x86.dot
testBinCG.passed: testBinCG.conf testBinCG testBinCFG.passed
	@$(RTH_RUN) INPUT=libm-2.3.6.so $< $@


# Tests ELF string table reallocation functions by changing some strings.  At first glance this would appear to be something
# quite easy to do, but it turns out to involve lots of details.
noinst_PROGRAMS += testElfStrtab
testElfStrtab_SOURCES = testElfStrtab.C
testElfStrtab_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testElfStrtab.passed
EXTRA_DIST += testElfStrtab.conf
testElfStrtab.passed: testElfStrtab.conf testElfStrtab
	@$(RTH_RUN) INPUT=arm-poweroff $< $@


# Reads in an ELF executable and changes the byte order from little-endian to big-endian or vice versa and writes out a new
# file. Note that the byte order change affects the ELF file format but not the executable described by that format.
noinst_PROGRAMS += testElfByteOrder
testElfByteOrder_SOURCES = testElfByteOrder.C
testElfByteOrder_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testElfByteOrder.passed
EXTRA_DIST += testElfByteOrder.conf
testElfByteOrder.passed: testElfByteOrder.conf testElfByteOrder testElfStrtab.passed
	@$(RTH_RUN) INPUT=arm-poweroff $< $@


# Reads in an ELF executable and changes the word size from eight to four bytes or vice versa and writes out a new file. Note
# that the word size change affects the ELF file format but not the executable described by that format.
noinst_PROGRAMS += testElfWordSize
testElfWordSize_SOURCES = testElfWordSize.C
testElfWordSize_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testElfWordSize.passed
EXTRA_DIST += testElfWordSize.conf
testElfWordSize.passed: testElfWordSize.conf testElfWordSize testElfStrtab.passed testElfByteOrder.passed
	@$(RTH_RUN) INPUT=arm-poweroff $< $@


# Demonstrates how to change names of dynamically-linked functions. This example reads an ELF executable dynamically linked to
# the MPI library and changes all calls to PMPI_* functions with calls to XMPI_* functions. See the top of the source file
# for more details.
noinst_PROGRAMS += testElfSchulz
testElfSchulz_SOURCES = testElfSchulz.C
testElfSchulz_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testElfSchulz-1.passed testElfSchulz-2.passed
EXTRA_DIST += testElfSchulz.conf
testElfSchulz-1.passed: testElfSchulz.conf testElfSchulz
	@$(RTH_RUN) FLAGS='-rose:partitioner_search -leftovers' N=1 $< $@
testElfSchulz-2.passed: testElfSchulz.conf testElfSchulz
	@$(RTH_RUN) FLAGS='-rose:partitioner_search -leftovers' N=2 $< $@


# Demonstrates how to build an ELF executable from scratch. Lots of comments in the source code. The resulting file is executable
# on an x86 Linux using syscall 1 to exit with a particular value.
noinst_PROGRAMS += testElfConstruct
testElfConstruct_SOURCES = testElfConstruct.C
testElfConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testElfConstruct.passed
EXTRA_DIST += testElfConstruct.conf
MOSTLYCLEANFILES += a.out
testElfConstruct.passed: testElfConstruct.conf testElfConstruct
	@$(RTH_RUN) $< $@


# Demonstrates how to build a PE executable from scratch. This demo is not as complete at the ELF version, but does show how to
# create a file that contains multiple format headers (a DOS header and a PE header).
noinst_PROGRAMS += testPeConstruct
testPeConstruct_SOURCES = testPeConstruct.C
testPeConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testPeConstruct.passed
EXTRA_DIST += testPeConstruct.conf
MOSTLYCLEANFILES += a.out
testPeConstruct.passed: testPeConstruct.conf testPeConstruct
	@$(RTH_RUN) $< $@


# Changes the size and/or location of a section in an executable file.
noinst_PROGRAMS += shiftResizeSection
shiftResizeSection_SOURCES = shiftResizeSection.C
shiftResizeSection_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += shiftResizeSection.passed
EXTRA_DIST += shiftResizeSection.conf shiftResizeSection.ans
MOSTLYCLEANFILES += arm-ctrlaltdel.dump-1 arm-ctrlaltdel.dump-2 arm-ctrlaltdel.dump-3
shiftResizeSection.passed: shiftResizeSection.conf shiftResizeSection
	@$(RTH_RUN) INPUT=arm-ctrlaltdel $< $@


# Finds constant values and propagates them across instructions
noinst_PROGRAMS += findConstants
findConstants_SOURCES = semantics.C
findConstants_CPPFLAGS = -DPOLICY_SELECTOR=1
findConstants_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += findConstants.passed
EXTRA_DIST += semantics.conf findConstants.ans
findConstants.passed: semantics.conf findConstants
	@$(RTH_RUN) CMD=findConstants INPUT=i686-test1.O3.bin $< $@


# Finds constant values and propagates them across instructions using an ABI
noinst_PROGRAMS += findConstantsABI
findConstantsABI_SOURCES = semantics.C
findConstantsABI_CPPFLAGS = -DPOLICY_SELECTOR=2
findConstantsABI_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += findConstantsABI.passed
EXTRA_DIST += semantics.conf findConstantsABI.ans
findConstantsABI.passed: semantics.conf findConstantsABI
	@$(RTH_RUN) CMD=findConstantsABI INPUT=i686-test1.O3.bin $< $@


# Uses VirtualMachineSemantics semantic analysis policy
noinst_PROGRAMS += virtualMachineSemantics
virtualMachineSemantics_SOURCES = semantics.C
virtualMachineSemantics_CPPFLAGS = -DPOLICY_SELECTOR=3
virtualMachineSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += virtualMachineSemantics.passed
EXTRA_DIST += semantics.conf virtualMachineSemantics.ans
virtualMachineSemantics.passed: semantics.conf virtualMachineSemantics
	@$(RTH_RUN) CMD=virtualMachineSemantics INPUT=i686-test1.O3.bin $< $@


# Use SymbolicSemantics semantic analysis policy
noinst_PROGRAMS += symbolicSemantics
symbolicSemantics_SOURCES = semantics.C
symbolicSemantics_CPPFLAGS = -DPOLICY_SELECTOR=4 -DSOLVER_SELECTOR=0
symbolicSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += symbolicSemantics.passed
EXTRA_DIST += semantics.conf symbolicSemantics.ans
symbolicSemantics.passed: semantics.conf symbolicSemantics
	@$(RTH_RUN) CMD=symbolicSemantics INPUT=i686-test1.O3.bin $< $@


# Use SymbolicSemantics with YicesSolver policy, invoking "yices" executable. We should be able
# to compile this test even when the "yices" command is not available. However, we won't be able
# to test it if we don't have it.
noinst_PROGRAMS += yicesSemanticsExe
yicesSemanticsExe_SOURCES = semantics.C
yicesSemanticsExe_CPPFLAGS = -DPOLICY_SELECTOR=4 -DSOLVER_SELECTOR=1
yicesSemanticsExe_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsExe.ans
if HAVE_YICES
STATIC_TEST_TARGETS += yicesSemanticsExe.passed
yicesSemanticsExe.passed: semantics.conf yicesSemanticsExe
	@$(RTH_RUN) CMD=yicesSemanticsExe INPUT=i686-test1.O3.bin $< $@
endif


# Use SymbolicSemantics with YicesSolver, linked to Yikes library. We should be able to compile
# this test even when the "yices" library is not available. However, we won't be able to run the
# test if we don't have the library.
noinst_PROGRAMS += yicesSemanticsLib
yicesSemanticsLib_SOURCES = semantics.C
yicesSemanticsLib_CPPFLAGS = -DPOLICY_SELECTOR=4 -DSOLVER_SELECTOR=2
yicesSemanticsLib_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsLib.ans
if HAVE_LIBYICES
STATIC_TEST_TARGETS += yicesSemanticsLib.passed
yicesSemanticsLib.passed: semantics.conf yicesSemanticsLib
	@$(RTH_RUN) CMD=yicesSemanticsLib INPUT=i686-test1.O3.bin $< $@
endif


# Disassembles an executable according to various command-line switches.
noinst_PROGRAMS += disassemble
disassemble_SOURCES = disassemble.C
disassemble_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += disassemble.passed assembler.passed
EXTRA_DIST += disassemble.conf disassemble.ans assembler.conf
disassemble.passed: disassemble.conf disassemble
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@
assembler.passed: assembler.conf disassemble
	@$(RTH_RUN) $< $@


# Disassemble a file containing bare instructions
noinst_PROGRAMS += disassembleBuffer
disassembleBuffer_SOURCES = disassembleBuffer.C
disassembleBuffer_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += disassembleBuffer.passed
EXTRA_DIST += disassembleBuffer.conf disassembleBuffer.ans
disassembleBuffer.passed: disassembleBuffer.conf disassembleBuffer
	@$(RTH_RUN) INPUT=buffer2.raw ADDRESS=0x8048310 $< $@


noinst_PROGRAMS += testEtherInsns
testEtherInsns_SOURCES = testEtherInsns.C
testEtherInsns_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testEtherInsns.passed
EXTRA_DIST += testEtherInsns.conf
testEtherInsns.passed: testEtherInsns.conf testEtherInsns
	@$(RTH_RUN) $< $@


# Program to show function boundary information for a binary executable
noinst_PROGRAMS += functionBoundaries
functionBoundaries_SOURCES=functionBoundaries.C
functionBoundaries_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += functionBoundaries_1.passed functionBoundaries_2.passed functionBoundaries_3.passed
EXTRA_DIST += functionBoundaries.conf functionBoundaries_1.ans functionBoundaries_2.ans functionBoundaries_3.ans
functionBoundaries_1.passed: functionBoundaries.conf functionBoundaries disassemble.passed
	@$(RTH_RUN) INPUT=i686-test1.O0.bin N=1 $< $@
functionBoundaries_2.passed: functionBoundaries.conf functionBoundaries findConstants.passed findConstantsABI.passed
	@$(RTH_RUN) INPUT=i686-test1.O3.bin N=2 $< $@
functionBoundaries_3.passed: functionBoundaries.conf functionBoundaries
	@$(RTH_RUN) INPUT=i686-test1.O3-stripped.bin N=3 $< $@


# Program to add read-only data to ELF core dumps.
noinst_PROGRAMS += expandCoreDump
expandCoreDump_SOURCES=expandCoreDump.C
expandCoreDump_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += expandCoreDump.passed
EXTRA_DIST += expandCoreDump.conf
expandCoreDump.passed: expandCoreDump.conf expandCoreDump
	@$(RTH_RUN) $< $@


# Program that encodes/decodes data similar to ROT-13 but acceping binary data
noinst_PROGRAMS += rot13
rot13_SOURCES=rot13.C
STATIC_TEST_TARGETS += rot13.passed
EXTRA_DIST += rot13.conf
rot13.passed: rot13.conf rot13
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@


# Program to test the assembler (see also "disassemble --assemble")
noinst_PROGRAMS += testAssembler
testAssembler_SOURCES=testAssembler.C
testAssembler_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testAssembler.passed
EXTRA_DIST += testAssembler.conf testAssembler.ans
testAssembler.passed: testAssembler.conf testAssembler
	@$(RTH_RUN) INPUT=buffer2.bin $< $@


# Program to test that we can write and then read an AST for a binary executable
noinst_PROGRAMS += testAstIO
testAstIO_SOURCES=testAstIO.C
testAstIO_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testAstIO.passed
EXTRA_DIST += testAstIO.conf
testAstIO.passed: testAstIO.conf testAstIO
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@

# Program to test that SgAsmGenericFile::neuter works across AST-IO.
noinst_PROGRAMS += testAstNeuter
testAstNeuter_SOURCES=testAstNeuter.C
testAstNeuter_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testAstNeuter.passed
EXTRA_DIST += testAstNeuter.conf
MOSTLYCLEANFILES += testAstNeuter.ast
testAstNeuter.passed: testAstNeuter.conf testAstNeuter
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@

# Program to test the RegisterDescriptor and RegisterDictionary classes.
noinst_PROGRAMS += testRegisters
testRegisters_SOURCES=testRegisters.C
testRegisters_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
STATIC_TEST_TARGETS += testRegisters.passed
EXTRA_DIST += testRegisters.conf
testRegisters.passed: testRegisters.conf testRegisters
	@$(RTH_RUN) $< $@

# Not sure what this does.
if ROSE_USE_SQLITE_DATABASE
noinst_PROGRAMS += testLibraryDb
testLibraryDb_SOURCES = testLibraryDb.C
testLibraryDb_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
STATIC_TEST_TARGETS += testLibraryDb.passed
EXTRA_DIST += testLibraryDb.conf
MOSTLYCLEANFILES += test.db
testLibraryDb.passed: testLibraryDb.conf testLibraryDb
	@$(RTH_RUN) $< $@
endif

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

demo_findConstantsABI: findConstantsABI demo_findConstants
	@echo; echo; echo "Demo for $<"; echo
	./findConstantsABI $(BINARY_SAMPLES)/i686-test1.O3.bin

demo_virtualMachineSemantics: virtualMachineSemantics demo_findConstantsABI
	@echo; echo; echo "Demo for $<"; echo
	./virtualMachineSemantics $(BINARY_SAMPLES)/i686-test1.O3.bin

demo_symbolicSemantics: symbolicSemantics demo_virtualMachineSemantics
	: no demo yet for symbolicSemantics

demo_yicesSemanticsExe: yicesSemanticsExe demo_symbolicSemantics
	: no demo yet for yicesSemanticsExe

demo_yicesSemanticsLib: yicesSemanticsLib demo_yicesSemanticsLib
	: no demo yet for yicesSemanticsLib

demo_disassemble: disassemble demo_yicesSemanticsLib
	@echo; echo; echo "Demo for $<"; echo
	./disassemble -rose:disassembler_search following,immediate,words,allbytes,unused,nonexe,deadend,unknown $(BINARY_SAMPLES)/i686-test1.O0.bin
	@echo "output has been dumped to i686-test1.O0.bin.dump"

demo_disassembleBuffer: disassembleBuffer demo_disassemble
	@echo; echo; echo "Demo for $<"; echo
	./disassembleBuffer $(BINARY_SAMPLES)/buffer2.raw 0x8048310

demo_testAssembler: testAssembler demo_disassembleBuffer
	@echo; echo; echo "Demo for $<"; echo
	./testAssembler $(BINARY_SAMPLES)/buffer2.bin

demo_testBinCFG: testBinCFG demo_testAssembler
	: no demo yet for testBinCFG

demo_expandCoreDump: expandCoreDump demo_testBinCFG
	: no demo yet for expandCoreDump

demo_functionBoundaries: functionBoundaries demo_exapndCoreDump
	@echo; echo; echo "Demo for $<";
	@echo "The functionBoundaries tool prints information about what binary functions were detected and why."
	./functionBoundaries $(BINARY_SAMPLES)/i686-test1.O3.bin

demo_rot13: rot13
	: no demo yet for rot13

demo_testAstIO: testAstIO
	: no demo yet for $<

demo_testAstNeuter: testAstNeuter
	: no demo yet for $<

demo_testRegisters: testRegisters
	: no demo yet for $<


###############################################################################################################################
# DYNAMICALLY-GENERATED TESTS
#
# Most testing rules appear next to the rules for generating the test executable.  Dynamically-generated tests appear here.
#
# Dynamicly-generated tests start here and their definitions are inserted below by the "build" script.  We make the
# dynamic tests depend on the static test because we're not sure what output files will be generated and we don't want
# the output of dynamic tests causing havoc for the static tests.  The test_sample_binaries.result (along with a whole
# slew of other targets) is created by the "build" script and placed in the generated Makefile.in.
#
# The rules in this section of the makefile are generated by the "build" script based on the selection of binary samples
# (usually from the binaries/samples directory) available at that time.  We do it this way for a number of reasons:
#
#  1. Some of the samples have licenses that restrict their distribution.
#  2. Some developers work with malicious software samples that we don't wish to distribute
#  3. ROSE generates file names in the current working directory based on basenames of samples. If the sample directories
#     contain files with duplicate basenames then the tests will fail if those to files happen to be tested in parallel. These
#     dependencies must be represented in the makefile rules.
#
# See scripts/sample_binary_rules.pl for more documentation.
###############################################################################################################################

# Yes, use recursive make. See comment above.
dynamic_tests: $(STATIC_TEST_TARGETS)
	@$(MAKE) test_sample_binaries.result


# All sample binary tests have these additional dependencies
SAMPLE_TEST_DEPS = execFormatsTest disassemble

INSERT_SAMPLE_RULES_HERE:



###############################################################################################################################
# Miscellaneous....
###############################################################################################################################

# Don't delete things indiscriminately--sometimes developers have other related files that should not be touched!
MOSTLYCLEANFILES += $(STATIC_TEST_TARGETS) $(patsubst %.passed, %.failed, $(STATIC_TEST_TARGETS))

# DQ (10/19/2010): This rule fails when ROSE is configure using --without-java, why is this?
# if !ROSE_USE_INTERNAL_JAVA_SUPPORT
# mostlyclean-binarysamples is defined by scripts/sample_binary_rules.pl invoked from "build"
mostlyclean-local: mostlyclean-binarysamples
# endif

if ROSE_BUILD_OS_IS_CYGWIN
# DQ (3/21/2009): These tests don't appear to run under Cygwin.
check-local:
	@echo "  TESTING (disabled: all tests disabled for Cygwin)"
else
check-local: $(STATIC_TEST_TARGETS) dynamic_tests
	$(RTH_STATS)
endif

else
check-local:
endif
