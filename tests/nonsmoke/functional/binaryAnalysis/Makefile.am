include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/specimens/binary/SpecimenSets.inc

SUBDIRS = libraryIdentification_tests Dwarf_tests Pin_tests

EXTRA_DIST = \
	runExecFormatsTest testShiftResizeSection.sh \
        testSampleBinary.sh testSampleBinaries.conf testFunctionDetection.pl

# Forward declarations
TEST_TARGETS=
noinst_PROGRAMS=
MOSTLYCLEANFILES=

if USE_BINARY_ANALYSIS_TESTS

# Location of binary specimens
SPECIMEN_DIR = $(nonsmoke_specimens_binary_directory)

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
TEST_WITH_ANSWER = $(top_srcdir)/scripts/test_with_answer


###############################################################################################################################
# Check parsing of symbolic expressions via rose::BinaryAnalysis::SymbolicExprParser
###############################################################################################################################
noinst_PROGRAMS += testSymbolicExprParser
testSymbolicExprParser_SOURCES = testSymbolicExprParser.C
testSymbolicExprParser_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymbolicExprParser.passed

testSymbolicExprParser.passed: $(TEST_WITH_ANSWER) testSymbolicExprParser testSymbolicExprParser.ans
	@$(RTH_RUN)						\
		TITLE="parsing of symbolic expressions [$@]"	\
		CMD="./testSymbolicExprParser"			\
		ANS="$(srcdir)/testSymbolicExprParser.ans"	\
		$< $@


###############################################################################################################################
# Parses an executable to produce a dump file (*.dump), an assembly file (rose_*.s), and a new executable created by unparsing
# the AST (*.new). The *.new file is typically identical to the original executable. This is essentially the same as
# tutorial/identityTranslator and is used by some of the other binary tests in this directory.
###############################################################################################################################

noinst_PROGRAMS += execFormatsTest
execFormatsTest_SOURCES = execFormatsTest.C
execFormatsTest_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)


###############################################################################################################################
# Tests for BinaryAnalysis::ControlFlow class.
###############################################################################################################################
noinst_PROGRAMS += testControlFlow
testControlFlow_SOURCES = testControlFlow.C
testControlFlow_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

testControlFlow_algorithms = A B C D E
testControlFlow_targets = $(addsuffix .passed, $(addprefix testControlFlow-, $(testControlFlow_algorithms)))
testControlFlow_answers = $(testControlFlow_targets:.passed=.ans)

EXTRA_DIST += $(testControlFlow_answers)
TEST_TARGETS += $(testControlFlow_targets)

$(testControlFlow_targets): testControlFlow-%.passed: $(TEST_WITH_ANSWER) testControlFlow testControlFlow-%.dot
	@$(RTH_RUN)							\
		TITLE="AST-based control flow graph, algorithm $* [$@]"	\
		CMD="./testControlFlow $* $(SPECIMEN_DIR)/buffer2.bin"	\
		ANS="$(srcdir)/testControlFlow-$*.dot"			\
		$< $@


###############################################################################################################################
# Test the binary CFG that points to instructions rather than basic blocks. No test is necessary since this
# is tested by testControlFlow already.  The cfg2.C file serves mostly as a clear example of how to generate an
# instruction-based CFG and write it to a GraphViz file along with some extra info on each vertex.
###############################################################################################################################
# FIXME ROSE-475 [Robb P. Matzke 2016-08-31]: This is an example, not a test. Move it to the documentation instead.

noinst_PROGRAMS += cfg2
cfg2_SOURCES = cfg2.C
cfg2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)


###############################################################################################################################
# Tests for the BinaryAnalysis::FunctionCall class.
###############################################################################################################################
noinst_PROGRAMS += testFunctionCall
testFunctionCall_SOURCES = testFunctionCall.C
testFunctionCall_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testFunctionCall-A.passed testFunctionCall-B.passed
EXTRA_DIST += testFunctionCall-A.dot testFunctionCall-B.dot

testFunctionCall-A.passed: $(TEST_WITH_ANSWER) testFunctionCall testFunctionCall-A.dot
	@$(RTH_RUN)							\
		TITLE="function calls, algorithm A [$@]"		\
		CMD="./testFunctionCall A $(SPECIMEN_DIR)/buffer2.bin"	\
		ANS="$(srcdir)/testFunctionCall-A.dot"			\
		$< $@

testFunctionCall-B.passed: $(TEST_WITH_ANSWER) testFunctionCall testFunctionCall-B.dot
	@$(RTH_RUN)							\
		TITLE="function calls, algorithm B [$@]"		\
		CMD="./testFunctionCall B $(SPECIMEN_DIR)/buffer2.bin"	\
		ANS="$(srcdir)/testFunctionCall-B.dot"			\
		$< $@


###############################################################################################################################
# Tests for control flow dominance graphs.
###############################################################################################################################
noinst_PROGRAMS += testDominance
testDominance_SOURCES = testDominance.C
testDominance_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

testDominance_algorithms = A B C D
testDominance_targets = $(addsuffix .passed, $(addprefix testDominance-, $(testDominance_algorithms)))
testDominance_answers = $(testDominance_targets:.passed=.ans)

EXTRA_DIST += $(testDominance_answers)
TEST_TARGETS += $(testDominance_targets)

$(testDominance_targets): testDominance-%.passed: $(TEST_WITH_ANSWER) testDominance testDominance-%.ans
	@$(RTH_RUN)							\
		TITLE="graph dominance, algorithm $* [$@]"		\
		CMD="./testDominance $* $(SPECIMEN_DIR)/buffer2.bin"	\
		ANS="$(srcdir)/testDominance-$*.ans"			\
		$< $@


###############################################################################################################################
# Tests ELF string table reallocation functions by changing some strings.  At first glance this would appear to be something
# quite easy to do, but it turns out to involve lots of details.
###############################################################################################################################
noinst_PROGRAMS += testElfStrtab
testElfStrtab_SOURCES = testElfStrtab.C
testElfStrtab_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testElfStrtab.passed

testElfStrtab.passed: $(TEST_EXIT_STATUS) testElfStrtab
	@$(RTH_RUN)								\
		TITLE="ELF string table allocation [$@]"			\
		CMD="$$(pwd)/testElfStrtab $(SPECIMEN_DIR)/arm-poweroff"	\
		USE_SUBDIR=yes							\
		$< $@


###############################################################################################################################
# Reads in an ELF executable and changes the byte order from little-endian to big-endian or vice versa and writes out a new
# file. Note that the byte order change affects the ELF file format but not the executable described by that format.
###############################################################################################################################
noinst_PROGRAMS += testElfByteOrder
testElfByteOrder_SOURCES = testElfByteOrder.C
testElfByteOrder_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testElfByteOrder.passed
EXTRA_DIST += testElfByteOrder.conf

testElfByteOrder.passed: testElfByteOrder.conf testElfByteOrder execFormatsTest testElfStrtab.passed
	@$(RTH_RUN)						\
		TITLE="modification of byte order [$@]"	\
		INPUT=arm-poweroff				\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"			\
		$< $@


###############################################################################################################################
# Reads in an ELF executable and changes the word size from eight to four bytes or vice versa and writes out a new file. Note
# that the word size change affects the ELF file format but not the executable described by that format.
###############################################################################################################################
noinst_PROGRAMS += testElfWordSize
testElfWordSize_SOURCES = testElfWordSize.C
testElfWordSize_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testElfWordSize.passed
EXTRA_DIST += testElfWordSize.conf

testElfWordSize.passed: testElfWordSize.conf testElfWordSize execFormatsTest testElfStrtab.passed testElfByteOrder.passed
	@$(RTH_RUN)					\
		TITLE="modification of word size [$@]"	\
		INPUT=arm-poweroff			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		$< $@


###############################################################################################################################
# Demonstrates how to change names of dynamically-linked functions. This example reads an ELF executable dynamically linked to
# the MPI library and changes all calls to PMPI_* functions with calls to XMPI_* functions. See the top of the source file
# for more details.
###############################################################################################################################
noinst_PROGRAMS += testElfSchulz
testElfSchulz_SOURCES = testElfSchulz.C
testElfSchulz_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testElfSchulz-1.passed testElfSchulz-2.passed
EXTRA_DIST += testElfSchulz.conf

testElfSchulz-1.passed: testElfSchulz.conf testElfSchulz execFormatsTest
	@$(RTH_RUN)							\
		TITLE="rename dynamically linked functions #1 [$@]"	\
		FLAGS='-rose:partitioner_search -leftovers'		\
		N=1							\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"				\
		$< $@
testElfSchulz-2.passed: testElfSchulz.conf testElfSchulz execFormatsTest
	@$(RTH_RUN)							\
		TITLE="rename dynamically linked functions #2 [$@]"	\
		FLAGS='-rose:partitioner_search -leftovers'		\
		N=2							\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"				\
		$< $@


###############################################################################################################################
# Demonstrates how to build an ELF executable from scratch. Lots of comments in the source code. The resulting file is executable
# on an x86 Linux using syscall 1 to exit with a particular value.
###############################################################################################################################
noinst_PROGRAMS += testElfConstruct
testElfConstruct_SOURCES = testElfConstruct.C
testElfConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testElfConstruct.passed

testElfConstruct.passed: $(TEST_EXIT_STATUS) testElfConstruct
	@$(RTH_RUN)						\
		TITLE="build an ELF file from scratch [$@]"	\
		CMD="$$(pwd)/testElfConstruct"			\
		USE_SUBDIR=yes					\
		$< $@


###############################################################################################################################
# Demonstrates how to build a PE executable from scratch. This demo is not as complete at the ELF version, but does show how to
# create a file that contains multiple format headers (a DOS header and a PE header).
###############################################################################################################################
noinst_PROGRAMS += testPeConstruct
testPeConstruct_SOURCES = testPeConstruct.C
testPeConstruct_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testPeConstruct.passed

testPeConstruct.passed: $(TEST_EXIT_STATUS) testPeConstruct
	@$(RTH_RUN)						\
		TITLE="build a PE file from scratch [$@]"	\
		CMD="$$(pwd)/testPeConstruct"			\
		USE_SUBDIR=yes					\
		$< $@


###############################################################################################################################
# Changes the size and/or location of a section in an executable file.
###############################################################################################################################
noinst_PROGRAMS += shiftResizeSection
shiftResizeSection_SOURCES = shiftResizeSection.C
shiftResizeSection_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += shiftResizeSection.passed
EXTRA_DIST += shiftResizeSection.ans

shiftResizeSection.passed: $(TEST_WITH_ANSWER) shiftResizeSection execFormatsTest shiftResizeSection.ans
	@$(RTH_RUN)											\
		TITLE="changing section size and/or location [$@]"					\
		CMD="$(srcdir)/testShiftResizeSection.sh $(SPECIMEN_DIR)/arm-ctrlaltdel $$(pwd)"	\
		ANS="$(srcdir)/shiftResizeSection.ans"							\
		USE_SUBDIR=yes										\
		$< $@


###############################################################################################################################
# Check whether the dominance classes can be specialized.  This is a compile-only test.
###############################################################################################################################
noinst_PROGRAMS += subDominance
subDominance_SOURCES = subDominance.C
subDominance_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)


###############################################################################################################################
# Test ROSE + XEN Ether interface
###############################################################################################################################
noinst_PROGRAMS += testEtherInsns
testEtherInsns_SOURCES = testEtherInsns.C
testEtherInsns_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testEtherInsns.passed

testEtherInsns.passed: $(TEST_EXIT_STATUS) testEtherInsns
	@$(RTH_RUN)									\
		TITLE="interface to XEN Ether [$@]"					\
		DISABLED="cannot be run automatically; requires human interaction"	\
		$< $@


###############################################################################################################################
# Program to show function boundary information for a binary executable
###############################################################################################################################
noinst_PROGRAMS += functionBoundaries
functionBoundaries_SOURCES=functionBoundaries.C
functionBoundaries_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += functionBoundaries_1.passed functionBoundaries_2.passed functionBoundaries_3.passed
EXTRA_DIST += functionBoundaries_1.ans functionBoundaries_2.ans functionBoundaries_3.ans

functionBoundaries_1.passed: $(SPECIMEN_DIR)/i686-test1.O0.bin functionBoundaries functionBoundaries_1.ans
	@$(RTH_RUN)							\
		TITLE="detect function boundaries $(notdir $<) [$@]"	\
		USE_SUBDIR=yes						\
		CMD="$$(pwd)/functionBoundaries $<"			\
		ANS=$(srcdir)/functionBoundaries_1.ans			\
		$(TEST_WITH_ANSWER) $@
functionBoundaries_2.passed: $(SPECIMEN_DIR)/i686-test1.O3.bin functionBoundaries functionBoundaries_2.ans
	@$(RTH_RUN)							\
		TITLE="detect function boundaries $(notdir $<) [$@]"	\
		USE_SUBDIR=yes						\
		CMD="$$(pwd)/functionBoundaries $<"			\
		ANS=$(srcdir)/functionBoundaries_2.ans			\
		$(TEST_WITH_ANSWER) $@
functionBoundaries_3.passed: $(SPECIMEN_DIR)/i686-test1.O3-stripped.bin functionBoundaries functionBoundaries_3.ans
	@$(RTH_RUN)							\
		TITLE="detect function boundaries $(notdir $<) [$@]"	\
		USE_SUBDIR=yes						\
		CMD="$$(pwd)/functionBoundaries $<"			\
		ANS=$(srcdir)/functionBoundaries_3.ans			\
		$(TEST_WITH_ANSWER) $@


###############################################################################################################################
# Program to add read-only data to ELF core dumps.
###############################################################################################################################
noinst_PROGRAMS += expandCoreDump
expandCoreDump_SOURCES=expandCoreDump.C
expandCoreDump_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += expandCoreDump.passed

expandCoreDump.passed: $(TEST_EXIT_STATUS) expandCoreDump
	@$(RTH_RUN)							\
		TITLE="adding read-only data to core dumps [$@]"	\
		DISABLED="not currently tested"				\
		$< $@


###############################################################################################################################
# Program that encodes/decodes data similar to ROT-13 but acceping binary data
###############################################################################################################################
noinst_PROGRAMS += rot13
rot13_SOURCES=rot13.C

TEST_TARGETS += rot13.passed
EXTRA_DIST += rot13.conf

rot13.passed: rot13.conf rot13
	@$(RTH_RUN)				\
		TITLE="binary ROT-13 [$@]"	\
		INPUT=i686-test1.O0.bin		\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"	\
		$< $@


###############################################################################################################################
# Program to test the assembler (see also "disassemble --assemble")
###############################################################################################################################
noinst_PROGRAMS += testAssembler
testAssembler_SOURCES=testAssembler.C
testAssembler_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testAssembler.passed
EXTRA_DIST += testAssembler.ans

testAssembler.passed: $(TEST_WITH_ANSWER) testAssembler testAssembler.ans
	@$(RTH_RUN)												\
		TITLE="x86 assembler [$@]"									\
		CMD="$$(pwd)/testAssembler -rose:partitioner_search -leftovers $(SPECIMEN_DIR)/buffer2.bin"	\
		USE_SUBDIR=yes											\
		ANS="$(srcdir)/testAssembler.ans"								\
		$< $@


###############################################################################################################################
# Program to test that we can write and then read an AST for a binary executable
###############################################################################################################################
noinst_PROGRAMS += testAstIO
testAstIO_SOURCES=testAstIO.C
testAstIO_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testAstIO.passed

testAstIO.passed: $(TEST_EXIT_STATUS) testAstIO
	@$(RTH_RUN)								\
		TITLE="writing and reading binary AST format [$@]"		\
		CMD="$$(pwd)/testAstIO $(SPECIMEN_DIR)/i686-test1.O0.bin"	\
		USE_SUBDIR=yes							\
		$< $@


###############################################################################################################################
# Program to test that SgAsmGenericFile::neuter works across AST-IO.
###############################################################################################################################
noinst_PROGRAMS += testAstNeuter
testAstNeuter_SOURCES=testAstNeuter.C
testAstNeuter_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testAstNeuter.passed
EXTRA_DIST += testAstNeuter.conf

MOSTLYCLEANFILES += testAstNeuter.ast
testAstNeuter.passed: testAstNeuter.conf testAstNeuter
	@$(RTH_RUN)							\
		TITLE="that neutering works across binary AST IO [$@]"	\
		CMD="$$(pwd)/testAstNeuter" INPUT="i686-test1.O0.bin"	\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"				\
		USE_SUBDIR=yes						\
		$< $@


###############################################################################################################################
# Program to test the RegisterDescriptor and RegisterDictionary classes.
###############################################################################################################################
noinst_PROGRAMS += testRegisters
testRegisters_SOURCES=testRegisters.C
testRegisters_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testRegisters.passed

testRegisters.passed: $(TEST_EXIT_STATUS) testRegisters
	@$(RTH_RUN)							\
		TITLE="register descriptors and dictionaries [$@]"	\
		CMD="./testRegisters"					\
		$< $@


###############################################################################################################################
# Tests reading past the end-of-file
###############################################################################################################################
noinst_PROGRAMS += testReadPastEOF
testReadPastEOF_SOURCES = testReadPastEOF.C
testReadPastEOF_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testReadPastEOF.passed
EXTRA_DIST += testReadPastEOF.conf testReadPastEOF.ans

testReadPastEOF.passed: testReadPastEOF.conf testReadPastEOF testReadPastEOF.ans
	@$(RTH_RUN)					\
		TITLE="reading past end-of-file [$@]"	\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		$< $@


###############################################################################################################################
# Not sure what this does.
###############################################################################################################################
if ROSE_USE_SQLITE_DATABASE

noinst_PROGRAMS += testLibraryDb
testLibraryDb_SOURCES = testLibraryDb.C
testLibraryDb_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testLibraryDb.passed

testLibraryDb.passed: $(TEST_EXIT_STATUS) testLibraryDb
	@$(RTH_RUN)					\
		TITLE="library identification [$@]"	\
		CMD="$$(pwd)/testLibraryDb"		\
		USE_SUBDIR=yes				\
		$< $@

endif


###############################################################################################################################
# Tests reading import sections from PE files
###############################################################################################################################
noinst_PROGRAMS += testPeImports
testPeImports_SOURCES = testPeImports.C
testPeImports_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testPeImports.passed
EXTRA_DIST += testPeImports.ans

testPeImports.passed: $(TEST_WITH_ANSWER) testPeImports testPeImports.ans
	@$(RTH_RUN)													\
		TITLE="that PE import tables can be parsed [$@]"							\
		CMD="$$(pwd)/testPeImports -rose:read_executable_file_format_only $(SPECIMEN_DIR)/proxycfg.exe"	\
		USE_SUBDIR=yes												\
		ANS="$(srcdir)/testPeImports.ans"									\
		$< $@


###############################################################################################################################
# Symbolic semantics test from Cory Cohen that triggers a reference-counting bug.
###############################################################################################################################
noinst_PROGRAMS += cory001
cory001_SOURCES = cory001.C
cory001_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory001.passed

cory001.passed: $(TEST_EXIT_STATUS) cory001
	@$(RTH_RUN)						\
		TITLE="reference counting [$@]"			\
		CMD="./cory001 $(SPECIMEN_DIR)//buffer2.bin"	\
		$< $@


###############################################################################################################################
# Binary CFG test from Cory Cohen that doesn't compile.  This is only a linking test--do not run it.
###############################################################################################################################
noinst_PROGRAMS += cory002
cory002_SOURCES = cory002.C
cory002_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


###############################################################################################################################
# Test certain RegisterStateGeneric stuff
###############################################################################################################################
noinst_PROGRAMS += cory003
cory003_SOURCES = cory003.C
cory003_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory003.passed

cory003.passed: $(TEST_EXIT_STATUS) cory003
	@$(RTH_RUN)					\
		TITLE="generic register states [$@]"	\
		CMD=./cory003				\
		$< $@


###############################################################################################################################
# Test register clearing in subclass constructor
###############################################################################################################################
noinst_PROGRAMS += cory004
cory004_SOURCES = cory004.C
cory004_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory004.passed

cory004.passed: $(TEST_EXIT_STATUS) cory004
	@$(RTH_RUN)							\
		TITLE="register clearing in subclass container [$@]"	\
		CMD=./cory004						\
		$< $@


###############################################################################################################################
# Test disassembling between functions
###############################################################################################################################
noinst_PROGRAMS += cory006
cory006_SOURCES = cory006.C
cory006_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory006.passed

cory006.passed: $(TEST_EXIT_STATUS) cory006
	@$(RTH_RUN)						\
		TITLE="inter-functon disassembly [$@]"		\
		CMD="./cory006 $(SPECIMEN_DIR)//i386-cory006"	\
		$< $@


###############################################################################################################################
# Test and demonstrate binary attributes
###############################################################################################################################
noinst_PROGRAMS += cory007
cory007_SOURCES = cory007.C
cory007_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory007.passed

cory007.passed: $(TEST_EXIT_STATUS) cory007
	@$(RTH_RUN)				\
		TITLE="binary attributes [$@]"	\
		CMD="./cory007"			\
		$< $@


###############################################################################################################################
# Test base objects in SgAsmIntegerValueExpression
###############################################################################################################################
noinst_PROGRAMS += cory008
cory008_SOURCES = cory008.C
cory008_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory008.passed
EXTRA_DIST += cory008.ans

cory008.passed: $(TEST_WITH_ANSWER) cory008 cory008.ans
	@$(RTH_RUN)								\
		TITLE="base objects in SgAsmIntegerValueExpression [$@]"	\
		CMD="./cory008 $(SPECIMEN_DIR)/i386-nologin"			\
		ANS=$(srcdir)/cory008.ans					\
		$< $@


###############################################################################################################################
# Test symbolic semantics memory read-after-write
###############################################################################################################################
noinst_PROGRAMS += testSymReadWrite
testSymReadWrite_SOURCES = testSymReadWrite.C
testSymReadWrite_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymReadWrite.passed
EXTRA_DIST += testSymReadWrite.conf testSymReadWrite.ans

testSymReadWrite.passed: testSymReadWrite.conf testSymReadWrite.ans testSymReadWrite
	@$(RTH_RUN)							\
		TITLE="symbolic semantic domain read/write [$@]"	\
		INPUT=memreadwrite					\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"				\
		$< $@


###############################################################################################################################
# Test the WorkList class
###############################################################################################################################
noinst_PROGRAMS += testWorkList
testWorkList_SOURCES = testWorkList.C
testWorkList_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testWorkList.passed

testWorkList.passed: $(TEST_EXIT_STATUS) testWorkList
	@$(RTH_RUN)					\
		TITLE="work list unit tests [$@]"	\
		CMD=./testWorkList			\
		$< $@


###############################################################################################################################
# Test ROSE's Map<T> class
###############################################################################################################################
noinst_PROGRAMS += testMap
testMap_SOURCES = testMap.C
testMap_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testMap.passed

testMap.passed: $(TEST_EXIT_STATUS) testMap
	@$(RTH_RUN)							\
		TITLE="custom Map class unit tests (deprecated) [$@]"	\
		CMD=./testMap						\
		$< $@


###############################################################################################################################
# Test pointer detection
###############################################################################################################################
noinst_PROGRAMS += testPointerDetection
testPointerDetection_SOURCES = testPointerDetection.C
testPointerDetection_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

PointerDetectionSpecimen = $(SPECIMEN_DIR)/i386-pointers
PointerDetectionSpecimenFunctions =											\
	simple01 simple02 simple03 simple04 simple05 simple06 simple07 simple08 simple09 simple10 simple11 simple12	\
	simple13 simple14 simple15 simple16 simple17 simple18 simple19 simple20 simple21 simple22 simple23 simple24	\
	simple25 flow01 flow02 flow03 flow04 flow05 flow06 flow07
# This is not currently working: it finds the local variable used as a pointer, but not the L-values used to assign
# to the local variable.
PointerDetectionSpecimenFunctionsNonWorking =	\
	flow07
PointerDetectionSwitches =											\
	$(patsubst %,--function=%,										\
	    $(filter-out $(PointerDetectionSpecimenFunctionsNonWorking),$(PointerDetectionSpecimenFunctions)))

EXTRA_DIST += testPointerDetection.ans
TEST_TARGETS += testPointerDetection.passed

testPointerDetection.passed: $(TEST_WITH_ANSWER) testPointerDetection testPointerDetection.ans
	@$(RTH_RUN)											\
	    TITLE="pointer detection [$@]"								\
	    CMD="./testPointerDetection $(PointerDetectionSwitches) $(PointerDetectionSpecimen)"	\
	    ANS=$(srcdir)/testPointerDetection.ans							\
	    $< $@


###############################################################################################################################
# Test function call detection
###############################################################################################################################
noinst_PROGRAMS += testCallDetection
testCallDetection_SOURCES = testCallDetection.C
testCallDetection_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testCallDetection.passed
EXTRA_DIST += testCallDetection.ans

testCallDetection.passed: $(TEST_WITH_ANSWER) $(SPECIMEN_DIR)/i386-fcalls testCallDetection testCallDetection.ans
	@$(RTH_RUN)								\
		TITLE="function call detection [$@]"				\
		CMD="$$(pwd)/testCallDetection $(SPECIMEN_DIR)/i386-fcalls"	\
		USE_SUBDIR=yes							\
		ANS=$(srcdir)/testCallDetection.ans				\
		$< $@


###############################################################################################################################
# Test analysis to see whether a function returns a value
###############################################################################################################################
noinst_PROGRAMS += testReturnsValue
testReturnsValue_SOURCES = testReturnsValue.C
testReturnsValue_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testReturnsValue.passed

testReturnsValue.passed: $(SPECIMEN_DIR)/buffer2.bin testReturnsValue $(TEST_EXIT_STATUS)
	@$(RTH_RUN)						\
		TITLE="function-returns-value analysis [$@]"	\
		CMD="./testReturnsValue $<"			\
		$(TEST_EXIT_STATUS) $@


###############################################################################################################################
# Unit tests for use-def (executed created below)
###############################################################################################################################
# Uses "symbolicSemantics2" executable compiled in another test.

TEST_TARGETS += usedef.passed
EXTRA_DIST += usedef.ans usedef.conf

usedef.passed: usedef.conf symbolicSemantics2 $(SPECIMEN_DIR)/usedef
	@$(RTH_RUN)											\
		USE_SUBDIR=yes										\
		TITLE="use-def unit tests"								\
		CMD="$$(pwd)/symbolicSemantics2 --trace --usedef $(abspath $(SPECIMEN_DIR)/usedef)"	\
		ANS="$(srcdir)/usedef.ans"								\
		$< $@


###############################################################################################################################
# Test BitPattern<> class template
###############################################################################################################################
noinst_PROGRAMS += testBitPattern
testBitPattern_SOURCES = testBitPattern.C
testBitPattern_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testBitPattern.passed

testBitPattern.passed: $(TEST_EXIT_STATUS) testBitPattern
	@$(RTH_RUN)					\
		TITLE="bit pattern matcher [$@]"	\
		CMD="./testBitPattern"			\
		$< $@


###############################################################################################################################
# Random number generator tests
###############################################################################################################################
noinst_PROGRAMS += testRNG
testRNG_SOURCES = testRNG.C
testRNG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testRNG.passed

testRNG.passed: $(TEST_EXIT_STATUS) testRNG
	@$(RTH_RUN)								\
		TITLE="pseudo random number generator (deprecated) [$@]"	\
		CMD="./testRNG"							\
		$< $@


###############################################################################################################################
# Symbolic expression simplifications
###############################################################################################################################
noinst_PROGRAMS += testSymbolicSimplification
testSymbolicSimplification_SOURCES = testSymbolicSimplification.C
testSymbolicSimplification_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymbolicSimplification.passed
EXTRA_DIST += testSymbolicSimplification.ans

testSymbolicSimplification.passed: $(TEST_WITH_ANSWER) testSymbolicSimplification testSymbolicSimplification.ans
	@$(RTH_RUN)						\
		TITLE="symbolic simplification [$@]"		\
		CMD="./testSymbolicSimplification"		\
		ANS="$(srcdir)/testSymbolicSimplification.ans"	\
		$< $@


###############################################################################################################################
# Symbolic expression user-defined flags
###############################################################################################################################
noinst_PROGRAMS += testSymbolicFlags
testSymbolicFlags_SOURCES = testSymbolicFlags.C
testSymbolicFlags_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymbolicFlags.passed
EXTRA_DIST += testSymbolicFlags.ans

testSymbolicFlags.passed: $(TEST_WITH_ANSWER) testSymbolicFlags testSymbolicFlags.ans
	@$(RTH_RUN)					\
		TITLE="symbolic flags [$@]"		\
		CMD="./testSymbolicFlags"		\
		ANS="$(srcdir)/testSymbolicFlags.ans"	\
		$< $@


###############################################################################################################################
# Instruction semantics verification.
###############################################################################################################################
noinst_PROGRAMS += verifySemantics
verifySemantics_SOURCES = verifySemantics.C
verifySemantics_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += verifySemantics.passed

verifySemantics.passed: $(TEST_EXIT_STATUS) verifySemantics
	@$(RTH_RUN)							\
		TITLE="instruction semantics verification"		\
		DISABLED="not portable enough to run automatically"	\
		CMD=false						\
		$< $@


###############################################################################################################################
# Disassembly tests using rose::BinaryAnalysis::Partitioner2
###############################################################################################################################

# Disassembles executables using Partitioner2
# Disassembly of executable files (DOS, ELF, PE) of various architectures (amd64, Arm, Mips, M68k, PowerPC, x86)
# + MIPS specimens are currently failing a FIXME assertion in makeShadowRegister()
# + PowerPC specimens have lots of "XL-Form xoOpcode = 36 not handled!" and similar errors
# + The pocketputty.exe pocketputty2.exe puttytel.exe are not good unit tests because they're too large; they take too
#   long when extensive debugging is enabled on heavily loaded or slow machines.
noinst_PROGRAMS += testPartitioner2
testPartitioner2_SOURCES = testPartitioner2.C
testPartitioner2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# Be careful where the comma is located!
testPartitioner2_specimens = $(filter-out		\
	$(nonsmoke_specimens_binary_any_any_mips)	\
	$(nonsmoke_specimens_binary_any_any_ppc)	\
	$(nonsmoke_specimens_binary_large),		\
	$(nonsmoke_specimens_binary_any_exe_any))

testPartitioner2_test_targets = $(addprefix testPartitioner2_, $(addsuffix .passed, $(testPartitioner2_specimens)))
testPartitioner2_answers = $(testPartitioner2_test_targets:.passed=.ans)

EXTRA_DIST += $(testPartitioner2_answers)
TEST_TARGETS += $(testPartitioner2_test_targets)
TEST_TARGETS += testPartitioner2_buffer2.bin.passed

#BEGIN TEMPORARY [RPM 2015-02-19]
#$(testPartitioner2_answers): testPartitioner2_%.ans:
#	./testPartitioner2 $(testPartitioner2_directory)/$* >$@
#	mv $@ $(srcdir)/$@
#END TEMPORARY [RPM 2015-02-19]

$(testPartitioner2_test_targets): testPartitioner2_%.passed: $(SPECIMEN_DIR)/% testPartitioner2 testPartitioner2_%.ans
	@$(RTH_RUN)												\
		TITLE="testPartitioner2 $(notdir $<) [$@]"							\
		USE_SUBDIR=yes											\
		CMD="$$(pwd)/testPartitioner2 $(if $(findstring exefmt,$<), --no-inter-function-calls) $<"	\
		ANS="$(srcdir)/testPartitioner2_$*.ans"							\
		$(top_srcdir)/scripts/test_with_answer $@

.PHONY: check-testPartitioner2
check-testPartitioner2: $(testPartitioner2_test_targets)


###############################################################################################################################
# Disassembly tests using the old partitioner (deprecated)
###############################################################################################################################

noinst_PROGRAMS += disassemble
disassemble_SOURCES = disassemble.C linux_syscalls.C
disassemble_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Disassembly of executable files (DOS, ELF, PE) of various architectures (amd64, Arm, Mips, M68k, PowerPC, x86)
# MIPS specimens are currently failing a FIXME assertion in makeShadowRegister()
# PowerPC specimens have lots of "XL-Form xoOpcode = 36 not handled!" and similar errors
# Be careful where the comma is located!
disassemble1_specimens =					\
	$(filter-out						\
		$(nonsmoke_specimens_binary_any_any_mips)	\
		$(nonsmoke_specimens_binary_any_any_ppc)	\
		$(nonsmoke_specimens_binary_large),		\
		$(nonsmoke_specimens_binary_any_exe_any))
disassemble1_test_targets = $(addprefix disassemble1_, $(addsuffix .passed, $(disassemble1_specimens)))
disassemble1_answers = $(disassemble1_test_targets:.passed=.ans)

EXTRA_DIST += $(disassemble1_answers) disassemble.conf
TEST_TARGETS += $(disassemble1_test_targets)

$(disassemble1_test_targets): disassemble1_%.passed: $(SPECIMEN_DIR)/% disassemble disassemble.conf disassemble1_%.ans
	@$(RTH_RUN)						\
		TITLE="disassemble $(notdir $<) [$@]"		\
		USE_SUBDIR=yes					\
		EXE="$$(pwd)/disassemble"			\
		SWITCHES=					\
		PSEARCH="-rose:partitioner_search -leftovers"	\
		ARGS="$<"					\
		$(srcdir)/disassemble.conf $@

# Disassembly of raw x86 buffers (these should probably have position independent code)
disassemble2_specimens = $(filter-out $(nonsmoke_specimens_binary_large), $(raw_insns_x86_specimens))
disassemble2_test_targets = $(addprefix disassemble2_, $(addsuffix .passed, $(disassemble2_specimens)))
disassemble2_answers = $(disassemble2_test_targets:.passed=.ans)

EXTRA_DIST += $(disassemble2_answers)
TEST_TARGETS += $(disassemble2_test_targets)

disassemble2_buffer2.raw.passed: $(SPECIMEN_DIR)/buffer2.raw disassemble disassemble.conf disassemble2_buffer2.raw.ans
	@$(RTH_RUN)													\
		TITLE="disassemble $(notdir $<) [$@]"									\
		USE_SUBDIR=yes												\
		EXE="$$(pwd)/disassemble"										\
		SWITCHES="--disassemble --linear --raw=0x08048310"							\
		DSEARCH="-rose:disassembler_search following,immediate,words,-allbytes,unused,nonexe,deadend,-unknown"	\
		ARGS="$< 0x08048310"											\
		$(srcdir)/disassemble.conf $@

disassemble2_vm-final.raw.passed: $(SPECIMEN_DIR)/vm-final.raw disassemble disassemble.conf disassemble2_vm-final.raw.ans
	@$(RTH_RUN)													\
		DISABLED="This takes forever to run"									\
		TITLE="disassemble $(notdir $<) [$@]"									\
		USE_SUBDIR=yes												\
		EXE="$$(pwd)/disassemble"										\
		SWITCHES="--disassemble --linear --raw=0x00c01000"							\
		DSEARCH="-rose:disassembler_search following,immediate,words,-allbytes,unused,nonexe,deadend,-unknown"	\
		ARGS="$< 0x00c00000"											\
		$(srcdir)/disassemble.conf $@

disassemble2_other_targets = $(filter-out	\
	disassemble2_buffer2.raw.passed		\
	disassemble2_vm-final.raw.passed	\
	,$(disassemble2_test_targets))

$(disassemble2_other_targets): disassemble2_%.passed: $(SPECIMEN_DIR)/% disassemble
	@$(RTH_RUN)						\
		DISABLED="unknown map and entry address"	\
		TITLE="disassemble $(notdir $<) [$@]"		\
		CMD=false					\
		$(srcdir)/disassemble.conf $@

# A convenient target for just testing the disassembler
.PHONY: check-disassemble
check-disassemble: $(disassemble1_test_targets) $(disassemble2_test_targets)


###############################################################################################################################
# Instruction semantics tests.
#
# Binary instruction semantics are a plug-n-play architecture, so we try to test a number of different combinations, usually
# all compiled from a single source file.  Files with "1" in their name use the older, template-based API while files with
# "2" in their name use the newer run-time plug-n-play API.
###############################################################################################################################

# Convenience target to run all these semantics tests. This target is not run automatically by "make check" (which instead
# depends on each of the targets individually rather than just this one).
.PHONY: check-semantics
#       API1 (the old stuff)            API2 (the newer stuff)           Extra tests
check-semantics:											\
        nullSemantics.passed            nullSemantics2.passed						\
        partialSymbolicSemantics.passed partialSymbolicSemantics2.passed				\
        intervalSemantics.passed        intervalSemantics2.passed					\
        symbolicSemantics.passed        symbolicSemantics2.passed        traceSymbolicSemantics2.passed	\
        yicesSemanticsExe.passed        yicesSemanticsExe2.passed					\
        yicesSemanticsLib.passed        yicesSemanticsLib2.passed					\
        multiSemantics.passed										\
        findConstants.passed										\
        findConstantsABI.passed										\
                                        semanticsSubclassing.passed     
# TOO1 (3/24/2015): Failing jenkins-release GCC 4.2.4; removing temporarily until fixed
#multiSemantics2.passed

# Null semantics
noinst_PROGRAMS += nullSemantics2
nullSemantics2_SOURCES = semantics.C
nullSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=NULL_DOMAIN
nullSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += nullSemantics2.passed
EXTRA_DIST += semantics.conf nullSemantics2.ans
nullSemantics2.passed: semantics.conf nullSemantics2
	@$(RTH_RUN) CMD=nullSemantics2 INPUT=i686-test1.O3.bin SPECIMEN_DIR="$(SPECIMEN_DIR)" $< $@

# Partial symbolic semantics
noinst_PROGRAMS += partialSymbolicSemantics2
partialSymbolicSemantics2_SOURCES = semantics.C
partialSymbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=PARTSYM_DOMAIN
partialSymbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += partialSymbolicSemantics2.passed
EXTRA_DIST += semantics.conf partialSymbolicSemantics2.ans
partialSymbolicSemantics2.passed: semantics.conf partialSymbolicSemantics2
	@$(RTH_RUN) CMD=partialSymbolicSemantics2 INPUT=i686-test1.O3.bin SPECIMEN_DIR="$(SPECIMEN_DIR)" $< $@

# Interval semantics
noinst_PROGRAMS += intervalSemantics2
intervalSemantics2_SOURCES = semantics.C
intervalSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=INTERVAL_DOMAIN
intervalSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += intervalSemantics2.passed
EXTRA_DIST += semantics.conf intervalSemantics2.ans
intervalSemantics2.passed: semantics.conf intervalSemantics2
	@$(RTH_RUN) CMD=intervalSemantics2 INPUT=i686-test1.O3.bin SPECIMEN_DIR="$(SPECIMEN_DIR)" $< $@

# Symbolic semantics, no SMT solver
noinst_PROGRAMS += symbolicSemantics2
symbolicSemantics2_SOURCES = semantics.C
symbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=NO_SOLVER
symbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += symbolicSemantics2.passed
EXTRA_DIST += semantics.conf symbolicSemantics2.ans
symbolicSemantics2.passed: semantics.conf symbolicSemantics2
	@$(RTH_RUN) CMD=symbolicSemantics2 INPUT=i686-test1.O3.bin SPECIMEN_DIR="$(SPECIMEN_DIR)" $< $@

# Tracing symbolic semantics, no SMT solver
noinst_PROGRAMS += traceSymbolicSemantics2
traceSymbolicSemantics2_SOURCES = semantics.C
traceSymbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=NO_SOLVER
traceSymbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += traceSymbolicSemantics2.passed
EXTRA_DIST += semantics.conf traceSymbolicSemantics2.ans
traceSymbolicSemantics2.passed: semantics.conf traceSymbolicSemantics2
	@$(RTH_RUN)				\
		CMD=traceSymbolicSemantics2	\
		SWITCHES="--trace --no-usedef"	\
		INPUT=i686-test1.O3.bin		\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"	\
		$< $@

# Symbolic semantics, Yices executable
noinst_PROGRAMS += yicesSemanticsExe2
yicesSemanticsExe2_SOURCES = semantics.C
yicesSemanticsExe2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=YICES_EXE
yicesSemanticsExe2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsExe2.ans
if ROSE_HAVE_YICES
TEST_TARGETS += yicesSemanticsExe2.passed
yicesSemanticsExe2.passed: semantics.conf yicesSemanticsExe2
	@$(RTH_RUN) CMD=yicesSemanticsExe2 INPUT=i686-test1.O3.bin SPECIMEN_DIR="$(SPECIMEN_DIR)" $< $@
endif

# Symbolic semantics, Yices library
noinst_PROGRAMS += yicesSemanticsLib2
yicesSemanticsLib2_SOURCES = semantics.C
yicesSemanticsLib2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=YICES_LIB
yicesSemanticsLib2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsLib2.ans
if ROSE_HAVE_LIBYICES
TEST_TARGETS += yicesSemanticsLib2.passed
yicesSemanticsLib2.passed: semantics.conf yicesSemanticsLib2
	@$(RTH_RUN) CMD=yicesSemanticsLib2 INPUT=i686-test1.O3.bin SPECIMEN_DIR="$(SPECIMEN_DIR)" $< $@
endif

# Multi-domain semantics, new API
# TOO1 (3/24/2015): Failing jenkins-release GCC 4.2.4; removing temporarily until fixed
#noinst_PROGRAMS += multiSemantics2
#multiSemantics2_SOURCES = semantics.C
#multiSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=MULTI_DOMAIN
#multiSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
#TEST_TARGETS += multiSemantics2.passed
#EXTRA_DIST += semantics.conf multiSemantics2.ans
#multiSemantics2.passed: semantics.conf multiSemantics2
#	@$(RTH_RUN) CMD=multiSemantics2 INPUT=i686-test1.O3.bin SPECIMEN_DIR="$(SPECIMEN_DIR)" $< $@

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
noinst_PROGRAMS += nullSemanticsSpeed2
nullSemanticsSpeed2_SOURCES = semanticsSpeed.C
nullSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=NULL_DOMAIN
nullSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of partial symbolic instruction semantics with and without using templates
noinst_PROGRAMS += partialSymbolicSemanticsSpeed2
partialSymbolicSemanticsSpeed2_SOURCES = semanticsSpeed.C
partialSymbolicSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=PARTSYM_DOMAIN
partialSymbolicSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of symbolic semantics with and without using templates
noinst_PROGRAMS += symbolicSemanticsSpeed2
symbolicSemanticsSpeed2_SOURCES = semanticsSpeed.C
symbolicSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN
symbolicSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of interval semantics with and without using templates
noinst_PROGRAMS += intervalSemanticsSpeed2
intervalSemanticsSpeed2_SOURCES = semanticsSpeed.C
intervalSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=INTERVAL_DOMAIN
intervalSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Tests speed of multi-domain semantics with and without using templates
noinst_PROGRAMS += multiSemanticsSpeed2
multiSemanticsSpeed2_SOURCES = semanticsSpeed.C
multiSemanticsSpeed2_CPPFLAGS = -DSEMANTIC_DOMAIN=MULTI_DOMAIN
multiSemanticsSpeed2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)


###############################################################################################################################
# LLVM tests
###############################################################################################################################

noinst_PROGRAMS += llvmTranscoder
llvmTranscoder_SOURCES = llvmTranscoder.C
llvmTranscoder_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

#-------------- basic test to make sure we can transcode each pre-compiled 32-bit x86 specimen

# The "llvm-as" tool might not be installed, in which case $(llvmVersion) will be an empty string.
llvmVersion = $(shell llvm-as --version 2>&1 |sed -nr '/LLVM version/ s/.*version ([0-9]+\.[0-9]+\.[0-9]+).*/\1/p')

# 128-bit registers are not supported, so don't try to test them (pentium-xxm)
# Test takes too long when compiled with debugging (puttytel.exe)
llvmTranscoder_Specimens =						\
	$(filter-out pentium-xmm $(large_specimens),			\
		$(elf_exe_x86_specimens) $(pe_exe_x86_specimens))
llvmTranscoder_TestTargets = $(addprefix llvm-xcode-, $(addsuffix .passed, $(llvmTranscoder_Specimens)))
TEST_TARGETS += $(llvmTranscoder_TestTargets)
EXTRA_DIST += llvmTranscoder.conf

$(llvmTranscoder_TestTargets): llvm-xcode-%.passed: $(SPECIMEN_DIR)/% llvmTranscoder llvmTranscoder.conf
	@$(RTH_RUN)					\
		TITLE="LLVM transcoder for $* [$@]"	\
		SPECIMEN="$(abspath $<)"		\
		LLVM_VERSION="$(llvmVersion)"		\
		$(srcdir)/llvmTranscoder.conf $@

.PHONY: check-llvm-xcode
check-llvm-xcode: $(llvmTranscoder_TestTargets)

#--------------- tests that run LLVM analysis passes using LLVM's "opt" command

llvmAnalysis_SpecimenDir = $(srcdir)/llvmAnalysisSpecimens
llvmAnalysis_Specimens =			\
	basicaa.c				\
	basiccg.c				\
	count-aa.c				\
	domfrontier.c				\
	domtree.c				\
	dot-callgraph.c				\
	dot-cfg-only.c				\
	dot-cfg.c				\
	globalsmodref-aa.c			\
	instcount.c				\
	intervals.c				\
	iv-users.c				\
	libcall-aa.c				\
	loops.c					\
	memdep.c				\
	postdomtree.c				\
	print-callgraph-sccs.c			\
	print-cfg-sccs.c			\
	print-dom-info.c			\
	print-used-types.c			\
	scalar-evolution.c

# This appears to not be present for llvm-3.5.0, so don't try to test it.
#	postdomfrontier.c

llvmAnalysis_TestTargets = $(addprefix llvm-analysis-, $(addsuffix .passed, $(basename $(llvmAnalysis_Specimens))))
EXTRA_DIST += $(addprefix $(llvmAnalysis_SpecimenDir), $(llvmAnalysis_Specimens))
EXTRA_DIST += llvmAnalysis.conf

# Do not run these tests automatically by "make check" because LLVM is not always installed.  Therefore, we
# also need to mention the outputs in MOSTLYCLEANFILES.
#TEST_TARGETS += $(llvmAnalysis_TestTargets)
MOSTLYCLEANFILES += $(llvmAnalysis_TestTargets) $(llvmAnalysis_TestTargets:.passed=.failed)

$(llvmAnalysis_TestTargets): llvm-analysis-%.passed: $(llvmAnalysis_SpecimenDir)/%.c llvmTranscoder disassemble
	@$(RTH_RUN)											\
		TITLE="LLVM analysis for $(notdir $<) [$@]"						\
		USE_SUBDIR=yes										\
		SPECIMEN="$<"										\
		OPT_PASSES="$(shell perl -ne '/OPT:\s*(.*?)\s*(\*\/\s*)?$$/ and print qq{$$1\n}' $<)"	\
		DISASSEMBLER=$(abspath disassemble)							\
		TRANSCODER="$(abspath llvmTranscoder) --llvm=$(llvmVersion)"				\
		$(srcdir)/llvmAnalysis.conf $@

.PHONY: check-llvm-analysis
check-llvm-analysis: $(llvmAnalysis_TestTargets)

#-------------- all LLVM-specific tests

.PHONY: check-llvm
check-llvm: $(llvmTranscoder_TestTargets) $(llvmAnalysis_TestTargets)


###############################################################################################################################
# Binary tainted flow analysis
###############################################################################################################################

noinst_PROGRAMS += taintedFlow
taintedFlow_SOURCES = taintedFlow.C
taintedFlow_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

taintedFlow_Specimens = i386-taintflow1
taintedFlow_TestTargets = $(addprefix taint_, $(addsuffix .passed, $(taintedFlow_Specimens)))
taintedFlow_TestAnswers = $(addprefix taint_, $(addsuffix .ans, $(taintedFlow_Specimens)))

TEST_TARGETS += $(taintedFlow_TestTargets)
EXTRA_DIST += $(taintedFlow_TestAnswers) taintedFlow.conf

$(taintedFlow_TestTargets): taint_%.passed: $(SPECIMEN_DIR)/% taintedFlow taintedFlow.conf
	@$(RTH_RUN) \
		TITLE="Tainted flow for $* [$@]" \
		USE_SUBDIR=yes \
		CMD="$(abspath ./taintedFlow) --approx=under --blocks --names='^f[0-9]' -- $(abspath $<)" \
		ANS="$(abs_srcdir)/taint_$*.ans" \
		$(abs_srcdir)/taintedFlow.conf $@

.PHONY: check-taint
check-taint: $(taintedFlow_TestTargets)


###############################################################################################################################
# No-op analysis tests
###############################################################################################################################

noinst_PROGRAMS += testNoop
testNoop_SOURCES = testNoop.C
testNoop_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

testNoop_Specimens = i386-noop
testNoop_TestTargets = $(addprefix noop_, $(addsuffix .passed, $(testNoop_Specimens)))
testNoop_TestAnswers = $(addprefix noop_, $(addsuffix .ans, $(testNoop_Specimens)))
testNoop_Switches = --stack=0xdddd0001 --use-semantics --no-find-dead-code --no-intra-function-code

TEST_TARGETS += $(testNoop_TestTargets)
EXTRA_DIST += $(testNoop_TestAnswers)

$(testNoop_TestTargets): noop_%.passed: $(SPECIMEN_DIR)/% testNoop
	@$(RTH_RUN)								\
		TITLE="testNoop $* [$@]"					\
		USE_SUBDIR=yes							\
		CMD="$(abspath ./testNoop) $(testNoop_Switches) $(abspath $<)"	\
		ANS="$(abs_srcdir)/noop_$*.ans"					\
		$(top_srcdir)/scripts/test_with_answer $@

.PHONY: check-noop
check-noop: $(testNoop_TestTargets)


###############################################################################################################################
# Static semantics tests
###############################################################################################################################

noinst_PROGRAMS += testStaticSemantics
testStaticSemantics_SOURCES = testStaticSemantics.C
testStaticSemantics_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# We lack semantics for some architectures, so don't try to test them (the result would be an std::runtime_error with the
# message "no instruction semantics for architecture".  Also, we skip pocketputty2.exe because it takes too long with lots
# of debugging is enabled.
testStaticSemantics_specimens = $(filter-out								\
	$(any_any_mips_specimens) $(any_any_ppc_specimens) $(any_any_arm_specimens) $(large_specimens)	\
	,$(any_exe_any_specimens))
testStaticSemantics_test_targets = $(addprefix testStaticSemantics_, $(addsuffix .passed, $(testStaticSemantics_specimens)))
TEST_TARGETS += $(testStaticSemantics_test_targets)

$(testStaticSemantics_test_targets): testStaticSemantics_%.passed: $(SPECIMEN_DIR)/% testStaticSemantics
	@$(RTH_RUN)						\
		TITLE="testStaticSemantics $(notdir $<) [$@]"	\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testStaticSemantics $<"		\
		$(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-testStaticSemantics
check-testStaticSemantics: $(testStaticSemantics_test_targets)


###############################################################################################################################
# Data-flow tests
###############################################################################################################################

noinst_PROGRAMS += testLazyInitialStates
testLazyInitialStates_SOURCES = testLazyInitialStates.C
testLazyInitialStates_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testLazyInitialStates.passed

testLazyInitialStates.passed: $(SPECIMEN_DIR)/i386-initialState testLazyInitialStates
	@$(RTH_RUN)									\
		TITLE="lazy symbolic states [$@]"					\
		USE_SUBDIR=yes								\
		CMD="$$(pwd)/testLazyInitialStates --isa=i386 --start=0 map:0=rx::$<"	\
		$(top_srcdir)/scripts/test_exit_status $@


###############################################################################################################################
# Test various things for all our sample binaries
###############################################################################################################################
# These all depend on the deprecated "disassemble" tool.

#--------------------------------------------------------------------------------
parse_Specimens = $(filter-out $(large_specimens), $(any_exe_any_specimens))
parse_Targets = $(addprefix parse_, $(addsuffix .passed, $(parse_Specimens)))

TEST_TARGETS += $(parse_Targets)

$(parse_Targets): parse_%.passed: $(SPECIMEN_DIR)/% disassemble
	@$(RTH_RUN)					\
		TITLE="parse $(notdir $<) [$@]"		\
		CMD="$$(pwd)/disassemble $(abspath $<)"	\
		USE_SUBDIR=yes				\
		$(TEST_EXIT_STATUS) $@

.PHONY: check_parse_container
check_parse_container: $(parse_Targets)

#--------------------------------------------------------------------------------
#FIXME: MIPS parser chokes on unimplemented DisassemblerMips::makeShadowRegister() [Robb Matzke 2013-08-30]
disassemble_Specimens = $(filter-out $(any_any_mips_specimens) $(large_specimens), $(any_exe_any_specimens))
disassemble_Targets = $(addprefix disassemble_, $(addsuffix .passed, $(disassemble_Specimens)))

TEST_TARGETS += $(disassemble_Targets)

$(disassemble_Targets): disassemble_%.passed: $(SPECIMEN_DIR)/% disassemble
	@$(RTH_RUN)						\
		TITLE="disassemble $(notdir $<) [$@]"		\
		CMD="$$(pwd)/disassemble --quiet $(abspath $<)"	\
		USE_SUBDIR=yes					\
		$(TEST_EXIT_STATUS) $@

.PHONY: check_disassemble
check_disassemble: $(disassemble_Targets)

#--------------------------------------------------------------------------------
#FIXME: MIPS parser chokes on unimplemented DisassemblerMips::makeShadowRegister() [Robb Matzke 2013-08-30]
unparse_Specimens = $(filter-out $(any_any_mips_specimens) $(large_specimens), $(any_exe_any_specimens))
unparse_Targets = $(addprefix unparse_, $(addsuffix .passed, $(unparse_Specimens)))

TEST_TARGETS += $(unparse_Targets)

$(unparse_Targets): unparse_%.passed: $(SPECIMEN_DIR)/% disassemble
	@$(RTH_RUN)					\
		TITLE="unparse $(notdir $<) [$@]"	\
		CMD="$$(pwd)/disassemble $(abspath $<)"	\
		USE_SUBDIR=yes				\
		$(TEST_EXIT_STATUS) $@

.PHONY: check_unparse
check_unparse: $(unparse_Targets)

#--------------------------------------------------------------------------------
#FIXME: all of the x86 executables have at least one thing that doesn't assemble [Robb Matzke 2013-08-30]
#assemble_Specimens = $(any_exe_x86_specimens)
assemble_Targets = $(addprefix assemble_, $(addsuffix .passed, $(assemble_Specimens)))

TEST_TARGETS += $(assemble_Targets)

$(assemble_Targets): assemble_%.passed: $(SPECIMEN_DIR)/% disassemble
	@$(RTH_RUN)									\
		TITLE="assemble $(notdir $<) [$@]"					\
		CMD="$$(pwd)/disassemble --quiet --reassemble --show-bad $(abspath $<)"	\
		USE_SUBDIR=yes								\
		$(TEST_EXIT_STATUS) $@

.PHONY: check_assemble
check_assemble: $(assemble_Targets)

#--------------------------------------------------------------------------------
#FIXME: all the ARM specimens are failing, and have been. This might be due to poor disassembly. [Robb Matzke 2013-08-30]
astio_Specimens = $(filter-out $(any_any_arm_specimens) $(large_specimens), $(any_exe_any_specimens))
astio_Targets = $(addprefix astio_, $(addsuffix .passed, $(astio_Specimens)))

TEST_TARGETS += $(astio_Targets)

$(astio_Targets): astio_%.passed: $(SPECIMEN_DIR)/% testAstIO
	@$(RTH_RUN)							\
		TITLE="binary AST IO for $(notdir $<) [$@]"		\
		CMD="$$(pwd)/testAstIO -rose:binary $(abspath $<)"	\
		USE_SUBDIR=yes						\
		$(TEST_EXIT_STATUS) $@

.PHONY: check_astio
check_astio: $(astio_Targets)

#--------------------------------------------------------------------------------
.PHONY: check_specimens
check_specimens: check_parse_container check_disassemble check_unparse check_assemble check_astio


###############################################################################################################################
# Standard boilerplate
###############################################################################################################################

if ROSE_BUILD_OS_IS_CYGWIN
# DQ (3/21/2009): These tests don't appear to run under Cygwin.
check-nonsmoke-local:
	@echo "  TESTING (disabled: all tests disabled for Cygwin)"
else
# Binary analysis is enabled and supported
check-nonsmoke-local: $(TEST_TARGETS)

endif
else
# Binary analysis is not enabled
check-nonsmoke-local:
	@echo "  TESTING (disabled: binary frontend is not enabled)"
endif


check-smoke-local:

# FIXME: check-local temporarily runs non-smoke tests as well until Jenkins and the matrix are fixed [Matzke, 2016-09-07]
#check-local: check-smoke-local
check-local: check-smoke-local check-nonsmoke-local

check-all-local: check-smoke-local check-nonsmoke-local

# Don't delete things indiscriminately--sometimes developers have other related files that should not be touched!
# Rather than use one big MOSTLYCLEANFILES, split things into smaller parts so we don't get command lines that are too big.
clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)

