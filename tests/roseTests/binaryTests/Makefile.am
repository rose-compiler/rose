include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = SemanticVerification libraryIdentification_tests Dwarf_tests Pin_tests

EXTRA_DIST = \
	runExecFormatsTest testShiftResizeSection.sh \
        testSampleBinary.sh testSampleBinaries.conf testFunctionDetection.pl

# Forward declarations
TEST_TARGETS=
noinst_PROGRAMS=
MOSTLYCLEANFILES=

if USE_BINARY_ANALYSIS_TESTS

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
TEST_SPECIMEN = $(srcdir)/specimen_tests.conf

###############################################################################################################################
# The executables and their test cases...
###############################################################################################################################

# Check boost consistency
noinst_PROGRAMS += testBoost
testBoost_SOURCES = testBoost.C
testBoost_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testBoost.passed
testBoost.passed: testBoost
	./testBoost

# Parses an executable to produce a dump file (*.dump), an assembly file (rose_*.s), and a new executable created by unparsing
# the AST (*.new). The *.new file is typically identical to the original executable.
noinst_PROGRAMS += execFormatsTest
execFormatsTest_SOURCES = execFormatsTest.C
execFormatsTest_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += execFormatsTest.passed
EXTRA_DIST += execFormatsTest.conf
execFormatsTest.passed: execFormatsTest.conf execFormatsTest
	@$(RTH_RUN) $< $@


#Test the binary control flow graph that uses the semantic representation to perform an accurate analysis
noinst_PROGRAMS += testBinCFG
testBinCFG_SOURCES = testBinCFG.C
testBinCFG_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testBinCFG.passed
EXTRA_DIST += testBinCFG.conf
MOSTLYCLEANFILES += cfg_x86.dot
testBinCFG.passed: testBinCFG.conf testBinCFG
	@$(RTH_RUN) INPUT=libm-2.3.6.so $< $@

# Tests for the newer BinaryAnalysis::ControlFlow class.
noinst_PROGRAMS += testControlFlow
testControlFlow_SOURCES = testControlFlow.C
testControlFlow_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testControlFlow-A.passed testControlFlow-B.passed testControlFlow-C.passed testControlFlow-D.passed \
	testControlFlow-E.passed
EXTRA_DIST += testControlFlow.conf testControlFlow-A.dot testControlFlow-B.dot testControlFlow-C.dot testControlFlow-D.dot \
	testControlFlow-E.dot
testControlFlow-A.passed: testControlFlow.conf testControlFlow
	@$(RTH_RUN) CMD=testControlFlow ALGORITHM=A INPUT=buffer2.bin $< $@
testControlFlow-B.passed: testControlFlow.conf testControlFlow
	@$(RTH_RUN) CMD=testControlFlow ALGORITHM=B INPUT=buffer2.bin $< $@
testControlFlow-C.passed: testControlFlow.conf testControlFlow
	@$(RTH_RUN) CMD=testControlFlow ALGORITHM=C INPUT=buffer2.bin $< $@
testControlFlow-D.passed: testControlFlow.conf testControlFlow
	@$(RTH_RUN) CMD=testControlFlow ALGORITHM=D INPUT=buffer2.bin $< $@
testControlFlow-E.passed: testControlFlow.conf testControlFlow
	@$(RTH_RUN) CMD=testControlFlow ALGORITHM=E INPUT=buffer2.bin $< $@

# Test the new binary CFG that points to instructions rather than basic blocks. No test is necessary since this
# is tested by testControlFlow already.  The cfg2.C file serves mostly as a clear example of how to generate an
# instruction-based CFG and write it to a GraphViz file along with some extra info on each vertex.
noinst_PROGRAMS += cfg2
cfg2_SOURCES = cfg2.C
cfg2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

#Test the binary call graph that uses the semantic representation to perform an accurate analysis
noinst_PROGRAMS += testBinCG
testBinCG_SOURCES = testBinCG.C
testBinCG_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testBinCG.C
EXTRA_DIST += testBinCG.conf
MOSTLYCLEANFILES += cfg_x86.dot
testBinCG.passed: testBinCG.conf testBinCG testBinCFG.passed
	@$(RTH_RUN) INPUT=libm-2.3.6.so $< $@

# Tests for the newer BinaryAnalysis::FunctionCall class.
noinst_PROGRAMS += testFunctionCall
testFunctionCall_SOURCES = testFunctionCall.C
testFunctionCall_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testFunctionCall-A.passed testFunctionCall-B.passed
EXTRA_DIST += testFunctionCall.conf testFunctionCall-A.dot testFunctionCall-B.dot
testFunctionCall-A.passed: testFunctionCall.conf testFunctionCall
	@$(RTH_RUN) CMD=testFunctionCall ALGORITHM=A INPUT=buffer2.bin $< $@
testFunctionCall-B.passed: testFunctionCall.conf testFunctionCall
	@$(RTH_RUN) CMD=testFunctionCall ALGORITHM=B INPUT=buffer2.bin $< $@

# Tests for control flow dominance graphs.
noinst_PROGRAMS += testDominance
testDominance_SOURCES = testDominance.C
testDominance_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testDominance-A.passed testDominance-B.passed testDominance-C.passed testDominance-D.passed
EXTRA_DIST += testDominance.conf testDominance-A.ans testDominance-B.ans testDominance-C.ans testDominance-D.ans
testDominance-A.passed: testDominance.conf testDominance
	@$(RTH_RUN) CMD=testDominance ALGORITHM=A INPUT=buffer2.bin $< $@
testDominance-B.passed: testDominance.conf testDominance
	@$(RTH_RUN) CMD=testDominance ALGORITHM=B INPUT=buffer2.bin $< $@
testDominance-C.passed: testDominance.conf testDominance
	@$(RTH_RUN) CMD=testDominance ALGORITHM=C INPUT=buffer2.bin $< $@
testDominance-D.passed: testDominance.conf testDominance
	@$(RTH_RUN) CMD=testDominance ALGORITHM=D INPUT=buffer2.bin $< $@


# Tests ELF string table reallocation functions by changing some strings.  At first glance this would appear to be something
# quite easy to do, but it turns out to involve lots of details.
noinst_PROGRAMS += testElfStrtab
testElfStrtab_SOURCES = testElfStrtab.C
testElfStrtab_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testElfStrtab.passed
EXTRA_DIST += testElfStrtab.conf
testElfStrtab.passed: testElfStrtab.conf testElfStrtab
	@$(RTH_RUN) INPUT=arm-poweroff $< $@


# Reads in an ELF executable and changes the byte order from little-endian to big-endian or vice versa and writes out a new
# file. Note that the byte order change affects the ELF file format but not the executable described by that format.
noinst_PROGRAMS += testElfByteOrder
testElfByteOrder_SOURCES = testElfByteOrder.C
testElfByteOrder_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testElfByteOrder.passed
EXTRA_DIST += testElfByteOrder.conf
testElfByteOrder.passed: testElfByteOrder.conf testElfByteOrder execFormatsTest testElfStrtab.passed
	@$(RTH_RUN) INPUT=arm-poweroff $< $@


# Reads in an ELF executable and changes the word size from eight to four bytes or vice versa and writes out a new file. Note
# that the word size change affects the ELF file format but not the executable described by that format.
noinst_PROGRAMS += testElfWordSize
testElfWordSize_SOURCES = testElfWordSize.C
testElfWordSize_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testElfWordSize.passed
EXTRA_DIST += testElfWordSize.conf
testElfWordSize.passed: testElfWordSize.conf testElfWordSize execFormatsTest testElfStrtab.passed testElfByteOrder.passed
	@$(RTH_RUN) INPUT=arm-poweroff $< $@


# Demonstrates how to change names of dynamically-linked functions. This example reads an ELF executable dynamically linked to
# the MPI library and changes all calls to PMPI_* functions with calls to XMPI_* functions. See the top of the source file
# for more details.
noinst_PROGRAMS += testElfSchulz
testElfSchulz_SOURCES = testElfSchulz.C
testElfSchulz_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testElfSchulz-1.passed testElfSchulz-2.passed
EXTRA_DIST += testElfSchulz.conf
testElfSchulz-1.passed: testElfSchulz.conf testElfSchulz execFormatsTest
	@$(RTH_RUN) FLAGS='-rose:partitioner_search -leftovers' N=1 $< $@
testElfSchulz-2.passed: testElfSchulz.conf testElfSchulz execFormatsTest
	@$(RTH_RUN) FLAGS='-rose:partitioner_search -leftovers' N=2 $< $@


# Demonstrates how to build an ELF executable from scratch. Lots of comments in the source code. The resulting file is executable
# on an x86 Linux using syscall 1 to exit with a particular value.
noinst_PROGRAMS += testElfConstruct
testElfConstruct_SOURCES = testElfConstruct.C
testElfConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testElfConstruct.passed
EXTRA_DIST += testElfConstruct.conf
MOSTLYCLEANFILES += a.out
testElfConstruct.passed: testElfConstruct.conf testElfConstruct
	@$(RTH_RUN) $< $@


# Demonstrates how to build a PE executable from scratch. This demo is not as complete at the ELF version, but does show how to
# create a file that contains multiple format headers (a DOS header and a PE header).
noinst_PROGRAMS += testPeConstruct
testPeConstruct_SOURCES = testPeConstruct.C
testPeConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testPeConstruct.passed
EXTRA_DIST += testPeConstruct.conf
MOSTLYCLEANFILES += a.out
testPeConstruct.passed: testPeConstruct.conf testPeConstruct
	@$(RTH_RUN) $< $@


# Changes the size and/or location of a section in an executable file.
noinst_PROGRAMS += shiftResizeSection
shiftResizeSection_SOURCES = shiftResizeSection.C
shiftResizeSection_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += shiftResizeSection.passed
EXTRA_DIST += shiftResizeSection.conf shiftResizeSection.ans
MOSTLYCLEANFILES += arm-ctrlaltdel.dump-1 arm-ctrlaltdel.dump-2 arm-ctrlaltdel.dump-3
shiftResizeSection.passed: shiftResizeSection.conf shiftResizeSection execFormatsTest
	@$(RTH_RUN) INPUT=arm-ctrlaltdel $< $@

# Check whether the instruction semantics classes can be specialized. This is only a compile test; we never actually
# run the program since the same classes are exercised by other tests.
noinst_PROGRAMS += subSemantics
subSemantics_SOURCES = subSemantics.C
subSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Check whether the dominance classes can be specialized.
noinst_PROGRAMS += subDominance
subDominance_SOURCES = subDominance.C
subDominance_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

# Disassembles an executable according to various command-line switches.
noinst_PROGRAMS += disassemble
disassemble_SOURCES = disassemble.C linux_syscalls.C
disassemble_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
#TEST_TARGETS += disassemble.passed assembler.passed
TEST_TARGETS += disassemble.passed
EXTRA_DIST += disassemble.conf disassemble.ans assembler.conf
disassemble.passed: disassemble.conf disassemble
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@
#assembler.passed: assembler.conf disassemble
#	@$(RTH_RUN) $< $@


# Disassemble a file containing bare instructions
noinst_PROGRAMS += disassembleBuffer
disassembleBuffer_SOURCES = disassembleBuffer.C
disassembleBuffer_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += disassembleBuffer.passed
EXTRA_DIST += disassembleBuffer.conf disassembleBuffer.ans
disassembleBuffer.passed: disassembleBuffer.conf disassembleBuffer
	@$(RTH_RUN) INPUT=buffer2.raw ADDRESS=0x8048310 $< $@


noinst_PROGRAMS += testEtherInsns
testEtherInsns_SOURCES = testEtherInsns.C
testEtherInsns_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testEtherInsns.passed
EXTRA_DIST += testEtherInsns.conf
testEtherInsns.passed: testEtherInsns.conf testEtherInsns
	@$(RTH_RUN) $< $@


# Program to show function boundary information for a binary executable
noinst_PROGRAMS += functionBoundaries
functionBoundaries_SOURCES=functionBoundaries.C
functionBoundaries_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += functionBoundaries_1.passed functionBoundaries_2.passed functionBoundaries_3.passed
EXTRA_DIST += functionBoundaries.conf functionBoundaries_1.ans functionBoundaries_2.ans functionBoundaries_3.ans
MOSTLYCLEANFILES += i686-test1.O0.bin-1.dump i686-test1.O0.bin-2.dump
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
TEST_TARGETS += expandCoreDump.passed
EXTRA_DIST += expandCoreDump.conf
expandCoreDump.passed: expandCoreDump.conf expandCoreDump
	@$(RTH_RUN) $< $@


# Program that encodes/decodes data similar to ROT-13 but acceping binary data
noinst_PROGRAMS += rot13
rot13_SOURCES=rot13.C
TEST_TARGETS += rot13.passed
EXTRA_DIST += rot13.conf
rot13.passed: rot13.conf rot13
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@


# Program to test the assembler (see also "disassemble --assemble")
noinst_PROGRAMS += testAssembler
testAssembler_SOURCES=testAssembler.C
testAssembler_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testAssembler.passed
EXTRA_DIST += testAssembler.conf testAssembler.ans
testAssembler.passed: testAssembler.conf testAssembler
	@$(RTH_RUN) INPUT=buffer2.bin $< $@


# Program to test that we can write and then read an AST for a binary executable
noinst_PROGRAMS += testAstIO
testAstIO_SOURCES=testAstIO.C
testAstIO_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testAstIO.passed
EXTRA_DIST += testAstIO.conf
MOSTLYCLEANFILES += i686-test1.O0.bin.ast
testAstIO.passed: testAstIO.conf testAstIO
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@

# Program to test that SgAsmGenericFile::neuter works across AST-IO.
noinst_PROGRAMS += testAstNeuter
testAstNeuter_SOURCES=testAstNeuter.C
testAstNeuter_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testAstNeuter.passed
EXTRA_DIST += testAstNeuter.conf
MOSTLYCLEANFILES += testAstNeuter.ast
testAstNeuter.passed: testAstNeuter.conf testAstNeuter
	@$(RTH_RUN) INPUT=i686-test1.O0.bin $< $@

# Program to test the RegisterDescriptor and RegisterDictionary classes.
noinst_PROGRAMS += testRegisters
testRegisters_SOURCES=testRegisters.C
testRegisters_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testRegisters.passed
EXTRA_DIST += testRegisters.conf
testRegisters.passed: testRegisters.conf testRegisters
	@$(RTH_RUN) $< $@

# Tests reading past the end-of-file
noinst_PROGRAMS += testReadPastEOF
testReadPastEOF_SOURCES = testReadPastEOF.C
testReadPastEOF_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += testReadPastEOF.passed
EXTRA_DIST += testReadPastEOF.conf testReadPastEOF.ans
testReadPastEOF.passed: testReadPastEOF.conf testReadPastEOF
	@$(RTH_RUN) $< $@

# Not sure what this does.
if ROSE_USE_SQLITE_DATABASE
noinst_PROGRAMS += testLibraryDb
testLibraryDb_SOURCES = testLibraryDb.C
testLibraryDb_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testLibraryDb.passed
EXTRA_DIST += testLibraryDb.conf
MOSTLYCLEANFILES += test.db
testLibraryDb.passed: testLibraryDb.conf testLibraryDb
	@$(RTH_RUN) $< $@
endif

# Tests reading import sections from PE files
noinst_PROGRAMS += testPeImports
testPeImports_SOURCES = testPeImports.C
testPeImports_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testPeImports.passed
EXTRA_DIST += testPeImports.conf testPeImports.ans
testPeImports.passed: testPeImports.conf testPeImports
	@$(RTH_RUN) INPUT=proxycfg.exe $< $@

# Symbolic semantics test from Cory Cohen that triggers a reference-counting bug.
noinst_PROGRAMS += cory001
cory001_SOURCES = cory001.C
cory001_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += cory001.passed
EXTRA_DIST += cory001.conf
cory001.passed: cory001.conf cory001
	@$(RTH_RUN) CMD=./cory001 $< $@

# Binary CFG test from Cory Cohen that doesn't compile.  This is only a linking test--do not run it.
noinst_PROGRAMS += cory002
cory002_SOURCES = cory002.C
cory002_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# Test symbolic semantics memory read-after-write
noinst_PROGRAMS += testSymReadWrite
testSymReadWrite_SOURCES = testSymReadWrite.C
testSymReadWrite_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testSymReadWrite.passed
EXTRA_DIST += testSymReadWrite.conf testSymReadWrite.ans
testSymReadWrite.passed: testSymReadWrite.conf testSymReadWrite.ans testSymReadWrite
	@$(RTH_RUN) INPUT=memreadwrite $< $@

# Test the WorkList class
noinst_PROGRAMS += testWorkList
testWorkList_SOURCES = testWorkList.C
testWorkList_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testWorkList.passed
testWorkList.passed: $(TEST_EXIT_STATUS) testWorkList
	@$(RTH_RUN) CMD=./testWorkList $< $@

# Test ROSE's Map<T> class
noinst_PROGRAMS += testMap
testMap_SOURCES = testMap.C
testMap_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testMap.passed
testMap.passed: $(TEST_EXIT_STATUS) testMap
	@$(RTH_RUN) CMD=./testMap $< $@

# Test pointer detection
noinst_PROGRAMS += testPointerDetection
testPointerDetection_SOURCES = testPointerDetection.C
testPointerDetection_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
PointerDetectionSpecimenFunctions = \
	simple01 simple02 simple03 simple04 simple05 simple06 simple07 simple08 simple09 simple10 simple11 simple12 \
	simple13 simple14 simple15 simple16 simple17 simple18 simple19 simple20 simple21 simple22 simple23 simple24 \
	simple25 flow01 flow02 flow03 flow04 flow05 flow06 flow07
PointerDetectionTestTargets = $(addprefix tpd_, $(addsuffix .passed, $(PointerDetectionSpecimenFunctions)))
TEST_TARGETS += $(PointerDetectionTestTargets)
EXTRA_DIST += testPointerDetection.conf $(addprefix tpd_, $(addsuffix .ans, $(PointerDetectionSpecimenFunctions)))
$(PointerDetectionTestTargets): tpd_%.passed: testPointerDetection.conf testPointerDetection $(BINARY_SAMPLES)/i386-pointers
	@$(RTH_RUN) FUNC="$$(echo "$@" |sed 's/^tpd_//;s/\.passed$$//')" INPUT=$(BINARY_SAMPLES)/i386-pointers $< $@
.PHONY: testPointerDetection.passed
testPointerDetection.passed: $(PointerDetectionTestTargets)

# Test function call detection
noinst_PROGRAMS += testCallDetection
testCallDetection_SOURCES = testCallDetection.C
testCallDetection_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testCallDetection.passed
EXTRA_DIST += testCallDetection.ans
testCallDetection.passed: $(BINARY_SAMPLES)/i386-fcalls testCallDetection
	@$(RTH_RUN) CMD="./testCallDetection $<" ANS=$(srcdir)/testCallDetection.ans $(top_srcdir)/scripts/test_with_answer $@

# Unit tests for use-def (executed created below)
TEST_TARGETS += usedef.passed
EXTRA_DIST += usedef.ans usedef.conf
usedef.passed: usedef.conf symbolicSemantics2 $(BINARY_SAMPLES)/usedef
	@$(RTH_RUN)										\
		USE_SUBDIR=yes									\
		TITLE="use-def unit tests"							\
		CMD="$$(pwd)/symbolicSemantics2 --trace --usedef $(BINARY_SAMPLES)/usedef"	\
		ANS="$(srcdir)/usedef.ans"							\
		$< $@

###############################################################################################################################
# Instruction semantics tests.
#
# Binary instruction semantics are a plug-n-play architecture, so we try to test a number of different combinations, usually
# all compiled from a single source file.  Files with "1" in their name use the older, template-based API while files with
# "2" in their name use the newer run-time plug-n-play API.
###############################################################################################################################

# Null semantics, old API
noinst_PROGRAMS += nullSemantics
nullSemantics_SOURCES = semantics.C
nullSemantics_CPPFLAGS = -DSEMANTIC_DOMAIN=NULL_DOMAIN -DSEMANTIC_API=OLD_API
nullSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += nullSemantics.passed
EXTRA_DIST += semantics.conf nullSemantics.ans
nullSemantics.passed: semantics.conf nullSemantics
	@$(RTH_RUN) CMD=nullSemantics INPUT=i686-test1.O3.bin $< $@

# Null semantics, new API
noinst_PROGRAMS += nullSemantics2
nullSemantics2_SOURCES = semantics.C
nullSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=NULL_DOMAIN -DSEMANTIC_API=NEW_API
nullSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += nullSemantics2.passed
EXTRA_DIST += semantics.conf nullSemantics2.ans
nullSemantics2.passed: semantics.conf nullSemantics2
	@$(RTH_RUN) CMD=nullSemantics2 INPUT=i686-test1.O3.bin $< $@

# Partial symbolic semantics, old API
noinst_PROGRAMS += partialSymbolicSemantics
partialSymbolicSemantics_SOURCES = semantics.C
partialSymbolicSemantics_CPPFLAGS = -DSEMANTIC_DOMAIN=PARTSYM_DOMAIN -DSEMANTIC_API=OLD_API
partialSymbolicSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += partialSymbolicSemantics.passed
EXTRA_DIST += semantics.conf partialSymbolicSemantics.ans
partialSymbolicSemantics.passed: semantics.conf partialSymbolicSemantics
	@$(RTH_RUN) CMD=partialSymbolicSemantics INPUT=i686-test1.O3.bin $< $@

# Partial symbolic semantics, new API
noinst_PROGRAMS += partialSymbolicSemantics2
partialSymbolicSemantics2_SOURCES = semantics.C
partialSymbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=PARTSYM_DOMAIN -DSEMANTIC_API=NEW_API
partialSymbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += partialSymbolicSemantics2.passed
EXTRA_DIST += semantics.conf partialSymbolicSemantics2.ans
partialSymbolicSemantics2.passed: semantics.conf partialSymbolicSemantics2
	@$(RTH_RUN) CMD=partialSymbolicSemantics2 INPUT=i686-test1.O3.bin $< $@

# Interval semantics, old API
noinst_PROGRAMS += intervalSemantics
intervalSemantics_SOURCES = semantics.C
intervalSemantics_CPPFLAGS = -DSEMANTIC_DOMAIN=INTERVAL_DOMAIN -DSEMANTIC_API=OLD_API
intervalSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += intervalSemantics.passed
EXTRA_DIST += semantics.conf intervalSemantics.ans
intervalSemantics.passed: semantics.conf intervalSemantics
	@$(RTH_RUN) CMD=intervalSemantics INPUT=i686-test1.O3.bin $< $@

# Interval semantics, new API
noinst_PROGRAMS += intervalSemantics2
intervalSemantics2_SOURCES = semantics.C
intervalSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=INTERVAL_DOMAIN -DSEMANTIC_API=NEW_API
intervalSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += intervalSemantics2.passed
EXTRA_DIST += semantics.conf intervalSemantics2.ans
intervalSemantics2.passed: semantics.conf intervalSemantics2
	@$(RTH_RUN) CMD=intervalSemantics2 INPUT=i686-test1.O3.bin $< $@

# Symbolic semantics, no SMT solver, old API
noinst_PROGRAMS += symbolicSemantics
symbolicSemantics_SOURCES = semantics.C
symbolicSemantics_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=OLD_API -DSMT_SOLVER=NO_SOLVER
symbolicSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += symbolicSemantics.passed
EXTRA_DIST += semantics.conf symbolicSemantics.ans
symbolicSemantics.passed: semantics.conf symbolicSemantics
	@$(RTH_RUN) CMD=symbolicSemantics INPUT=i686-test1.O3.bin $< $@

# Symbolic semantics, no SMT solver, new API
noinst_PROGRAMS += symbolicSemantics2
symbolicSemantics2_SOURCES = semantics.C
symbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=NEW_API -DSMT_SOLVER=NO_SOLVER
symbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += symbolicSemantics2.passed
EXTRA_DIST += semantics.conf symbolicSemantics2.ans
symbolicSemantics2.passed: semantics.conf symbolicSemantics2
	@$(RTH_RUN) CMD=symbolicSemantics2 INPUT=i686-test1.O3.bin $< $@

# Tracing symbolic semantics, no SMT solver, new API
noinst_PROGRAMS += traceSymbolicSemantics2
traceSymbolicSemantics2_SOURCES = semantics.C
traceSymbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=NEW_API -DSMT_SOLVER=NO_SOLVER
traceSymbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += traceSymbolicSemantics2.passed
EXTRA_DIST += semantics.conf traceSymbolicSemantics2.ans
traceSymbolicSemantics2.passed: semantics.conf traceSymbolicSemantics2
	@$(RTH_RUN) CMD=traceSymbolicSemantics2 SWITCHES="--trace --test-subst --no-usedef" INPUT=i686-test1.O3.bin $< $@

# Symbolic semantics, Yices executable, old API
noinst_PROGRAMS += yicesSemanticsExe
yicesSemanticsExe_SOURCES = semantics.C
yicesSemanticsExe_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=OLD_API -DSMT_SOLVER=YICES_EXE
yicesSemanticsExe_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsExe.ans
if ROSE_HAVE_YICES
TEST_TARGETS += yicesSemanticsExe.passed
yicesSemanticsExe.passed: semantics.conf yicesSemanticsExe
	@$(RTH_RUN) CMD=yicesSemanticsExe INPUT=i686-test1.O3.bin $< $@
endif

# Symbolic semantics, Yices executable, new API
noinst_PROGRAMS += yicesSemanticsExe2
yicesSemanticsExe2_SOURCES = semantics.C
yicesSemanticsExe2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=NEW_API -DSMT_SOLVER=YICES_EXE
yicesSemanticsExe2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsExe2.ans
if ROSE_HAVE_YICES
TEST_TARGETS += yicesSemanticsExe2.passed
yicesSemanticsExe2.passed: semantics.conf yicesSemanticsExe2
	@$(RTH_RUN) CMD=yicesSemanticsExe2 INPUT=i686-test1.O3.bin $< $@
endif

# Symbolic semantics, Yices library, old API
noinst_PROGRAMS += yicesSemanticsLib
yicesSemanticsLib_SOURCES = semantics.C
yicesSemanticsLib_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=OLD_API -DSMT_SOLVER=YICES_LIB
yicesSemanticsLib_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsLib.ans
if ROSE_HAVE_LIBYICES
TEST_TARGETS += yicesSemanticsLib.passed
yicesSemanticsLib.passed: semantics.conf yicesSemanticsLib
	@$(RTH_RUN) CMD=yicesSemanticsLib INPUT=i686-test1.O3.bin $< $@
endif

# Symbolic semantics, Yices library, new API
noinst_PROGRAMS += yicesSemanticsLib2
yicesSemanticsLib2_SOURCES = semantics.C
yicesSemanticsLib2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=NEW_API -DSMT_SOLVER=YICES_LIB
yicesSemanticsLib2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsLib2.ans
if ROSE_HAVE_LIBYICES
TEST_TARGETS += yicesSemanticsLib2.passed
yicesSemanticsLib2.passed: semantics.conf yicesSemanticsLib2
	@$(RTH_RUN) CMD=yicesSemanticsLib2 INPUT=i686-test1.O3.bin $< $@
endif

# Multi-domain semantics, old API
noinst_PROGRAMS += multiSemantics
multiSemantics_SOURCES = semantics.C
multiSemantics_CPPFLAGS = -DSEMANTIC_DOMAIN=MULTI_DOMAIN -DSEMANTIC_API=OLD_API
multiSemantics_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += multiSemantics.passed
EXTRA_DIST += semantics.conf multiSemantics.ans
multiSemantics.passed: semantics.conf multiSemantics
	@$(RTH_RUN) CMD=multiSemantics INPUT=i686-test1.O3.bin $< $@

# Multi-domain semantics, new API
noinst_PROGRAMS += multiSemantics2
multiSemantics2_SOURCES = semantics.C
multiSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=MULTI_DOMAIN -DSEMANTIC_API=NEW_API
multiSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += multiSemantics2.passed
EXTRA_DIST += semantics.conf multiSemantics2.ans
multiSemantics2.passed: semantics.conf multiSemantics2
	@$(RTH_RUN) CMD=multiSemantics2 INPUT=i686-test1.O3.bin $< $@

# Finds constant values and propagates them across instructions
noinst_PROGRAMS += findConstants
findConstants_SOURCES = semantics.C
findConstants_CPPFLAGS = -DSEMANTIC_DOMAIN=FINDCONST_DOMAIN -DSEMANTIC_API=OLD_API
findConstants_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += findConstants.passed
EXTRA_DIST += semantics.conf findConstants.ans
findConstants.passed: semantics.conf findConstants
	@$(RTH_RUN) CMD=findConstants INPUT=i686-test1.O3.bin $< $@


# Finds constant values and propagates them across instructions using an ABI
noinst_PROGRAMS += findConstantsABI
findConstantsABI_SOURCES = semantics.C
findConstantsABI_CPPFLAGS = -DSEMANTIC_DOMAIN=FINDCONSTABI_DOMAIN -DSEMANTIC_API=OLD_API
findConstantsABI_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += findConstantsABI.passed
EXTRA_DIST += semantics.conf findConstantsABI.ans
findConstantsABI.passed: semantics.conf findConstantsABI
	@$(RTH_RUN) CMD=findConstantsABI INPUT=i686-test1.O3.bin $< $@

# Demo how to subclass something from SymbolicSemantics
noinst_PROGRAMS += semanticsSubclassing
semanticsSubclassing_SOURCES = semanticsSubclassing.C
semanticsSubclassing_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += semanticsSubclassing.passed
semanticsSubclassing.passed: $(TEST_EXIT_STATUS) semanticsSubclassing
	@$(RTH_RUN) CMD=./semanticsSubclassing $< $@



###############################################################################################################################
# Instruction semantics speed tests.  These aren't actually run automatically, we just compile them to make sure they
# compile.  To run them, just run the executable with one argument: the name of a binary file.  The test just starts
# executing instructions at the entry address until it reaches a branch whose condition is not known, at which time the
# test repeats.  It does this for one minute and then prints out the execution rate.
###############################################################################################################################

# Tests speed of null instruction semantics with and without using templates
noinst_PROGRAMS += nullSemanticsSpeed1 nullSemanticsSpeed2
nullSemanticsSpeed1_SOURCES = semanticsSpeed.C
nullSemanticsSpeed1_CPPFLAGS = -DSEMANTIC_DOMAIN=NULL_DOMAIN -DSEMANTIC_API=OLD_API
nullSemanticsSpeed1_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
nullSemanticsSpeed2_SOURCES = semanticsSpeed.C
nullSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=NULL_DOMAIN -DSEMANTIC_API=NEW_API
nullSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of partial symbolic instruction semantics with and without using templates
noinst_PROGRAMS += partialSymbolicSemanticsSpeed1 partialSymbolicSemanticsSpeed2
partialSymbolicSemanticsSpeed1_SOURCES = semanticsSpeed.C
partialSymbolicSemanticsSpeed1_CPPFLAGS = -DSEMANTIC_DOMAIN=PARTSYM_DOMAIN -DSEMANTIC_API=OLD_API
partialSymbolicSemanticsSpeed1_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
partialSymbolicSemanticsSpeed2_SOURCES = semanticsSpeed.C
partialSymbolicSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=PARTSYM_DOMAIN -DSEMANTIC_API=NEW_API
partialSymbolicSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of symbolic semantics with and without using templates
noinst_PROGRAMS += symbolicSemanticsSpeed1 symbolicSemanticsSpeed2
symbolicSemanticsSpeed1_SOURCES = semanticsSpeed.C
symbolicSemanticsSpeed1_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=OLD_API
symbolicSemanticsSpeed1_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
symbolicSemanticsSpeed2_SOURCES = semanticsSpeed.C
symbolicSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSEMANTIC_API=NEW_API
symbolicSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of interval semantics with and without using templates
noinst_PROGRAMS += intervalSemanticsSpeed1 intervalSemanticsSpeed2
intervalSemanticsSpeed1_SOURCES = semanticsSpeed.C
intervalSemanticsSpeed1_CPPFLAGS = -DSEMANTIC_DOMAIN=INTERVAL_DOMAIN -DSEMANTIC_API=OLD_API
intervalSemanticsSpeed1_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
intervalSemanticsSpeed2_SOURCES = semanticsSpeed.C
intervalSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=INTERVAL_DOMAIN -DSEMANTIC_API=NEW_API
intervalSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of multi-domain semantics with and without using templates
noinst_PROGRAMS += multiSemanticsSpeed1 multiSemanticsSpeed2
multiSemanticsSpeed1_SOURCES = semanticsSpeed.C
multiSemanticsSpeed1_CPPFLAGS = -DSEMANTIC_DOMAIN=MULTI_DOMAIN -DSEMANTIC_API=OLD_API
multiSemanticsSpeed1_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
multiSemanticsSpeed2_SOURCES = semanticsSpeed.C
multiSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=MULTI_DOMAIN -DSEMANTIC_API=NEW_API
multiSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)


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

demo_partialSymbolicSemantics: partialSymbolicSemantics demo_findConstantsABI
	@echo; echo; echo "Demo for $<"; echo
	./partialSymbolicSemantics $(BINARY_SAMPLES)/i686-test1.O3.bin

demo_symbolicSemantics: symbolicSemantics demo_partialSymbolicSemantics
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

demo_testReadPastEOF: testReadPastEOF
	: no demo yet for $<

###############################################################################################################################
# Test various things for all our sample binaries

include $(BINARY_SAMPLES)/Specimens.inc

#--------------------------------------------------------------------------------
parse_Specimens = $(Executable_Specimens)
parse_Targets = $(addprefix parse_, $(addsuffix .passed, $(parse_Specimens)))
TEST_TARGETS += $(parse_Targets)

$(parse_Targets): parse_%.passed: $(BINARY_SAMPLES)/% disassemble
	@$(RTH_RUN) CMD="`pwd`/disassemble -rose:binary --skip-disassemble" SPECIMEN="$<" $(TEST_SPECIMEN) $@

.PHONY: check_parse_container
check_parse_container: $(parse_Targets)

#--------------------------------------------------------------------------------
#FIXME: MIPS parser chokes on unimplemented DisassemblerMips::makeShadowRegister() [Robb Matzke 2013-08-30]
disassemble_Specimens = $(filter-out $(MIPS_Specimens), $(Executable_Specimens))
disassemble_Targets = $(addprefix disassemble_, $(addsuffix .passed, $(disassemble_Specimens)))
TEST_TARGETS += $(disassemble_Targets)

$(disassemble_Targets): disassemble_%.passed: $(BINARY_SAMPLES)/% disassemble
	@$(RTH_RUN) CMD="`pwd`/disassemble -rose:binary --quiet" SPECIMEN="$<" $(TEST_SPECIMEN) $@

.PHONY: check_disassemble
check_disassemble: $(disassemble_Targets)

#--------------------------------------------------------------------------------
#FIXME: MIPS parser chokes on unimplemented DisassemblerMips::makeShadowRegister() [Robb Matzke 2013-08-30]
unparse_Specimens = $(filter-out $(MIPS_Specimens), $(Executable_Specimens))
unparse_Targets = $(addprefix unparse_, $(addsuffix .passed, $(unparse_Specimens)))
TEST_TARGETS += $(unparse_Targets)

$(unparse_Targets): unparse_%.passed: $(BINARY_SAMPLES)/% disassemble
	@$(RTH_RUN) CMD="`pwd`/disassemble -rose:binary" SPECIMEN="$<" $(TEST_SPECIMEN) $@

.PHONY: check_unparse
check_unparse: $(unparse_Targets)

#--------------------------------------------------------------------------------
#FIXME: all of the x86 executables have at least one thing that doesn't assemble [Robb Matzke 2013-08-30]
#assemble_Specimens = $(filter $(x86_Specimens), $(Executable_Specimens))
assemble_Targets = $(addprefix assemble_, $(addsuffix .passed, $(assemble_Specimens)))
TEST_TARGETS += $(assemble_Targets)

$(assemble_Targets): assemble_%.passed: $(BINARY_SAMPLES)/% disassemble
	@$(RTH_RUN) CMD="`pwd`/disassemble -rose:binary --quiet --reassemble --show-bad" SPECIMEN="$<" $(TEST_SPECIMEN) $@

.PHONY: check_assemble
check_assemble: $(assemble_Targets)

#--------------------------------------------------------------------------------
#FIXME: all the ARM specimens are failing, and have been. This might be due to poor disassembly. [Robb Matzke 2013-08-30]
astio_Specimens = $(filter-out $(ARM_Specimens), $(Executable_Specimens))
astio_Targets = $(addprefix astio_, $(addsuffix .passed, $(astio_Specimens)))
TEST_TARGETS += $(astio_Targets)

$(astio_Targets): astio_%.passed: $(BINARY_SAMPLES)/% testAstIO
	@$(RTH_RUN) CMD="`pwd`/testAstIO -rose:binary" SPECIMEN="$<" $(TEST_SPECIMEN) $@

.PHONY: check_astio
check_astio: $(astio_Targets)

#--------------------------------------------------------------------------------
.PHONY: check_specimens
check_specimens: check_parse_container check_disassemble check_unparse check_assemble check_astio

###############################################################################################################################
# Miscellaneous....
###############################################################################################################################

# Don't delete things indiscriminately--sometimes developers have other related files that should not be touched!
# Rather than use one big MOSTLYCLEANFILES, split things into smaller parts so we don't get command lines that are too big.
clean-local:
	rm -f $(addsuffix .dump, $(All_Specimens))
	rm -f $(addsuffix .new, $(All_Specimens))
	rm -f $(addsuffix .new.dump, $(All_Specimens))
	rm -f $(addsuffix .new.new, $(All_Specimens))
	rm -f $(addprefix rose_, $(addsuffix .s, $(All_Specimens)))
	rm -f $(addprefix rose_, $(addsuffix .new.s, $(All_Specimens)))
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)

if ROSE_BUILD_OS_IS_CYGWIN
# DQ (3/21/2009): These tests don't appear to run under Cygwin.
check-local:
	@echo "  TESTING (disabled: all tests disabled for Cygwin)"
else
check-local: $(TEST_TARGETS)
	$(RTH_STATS)
endif

else
check-local:
endif
