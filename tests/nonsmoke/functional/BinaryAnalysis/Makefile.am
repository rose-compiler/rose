include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/specimens/binary/SpecimenSets.inc

SUBDIRS = libraryIdentification_tests Dwarf_tests Pin_tests Concolic

EXTRA_DIST = \
	runExecFormatsTest testShiftResizeSection.sh \
        testSampleBinary.sh testSampleBinaries.conf testFunctionDetection.pl

# Forward declarations
TEST_TARGETS=
noinst_PROGRAMS=
MOSTLYCLEANFILES=
PHONIES=

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

# Location of binary specimens
SPECIMEN_DIR = $(nonsmoke_specimens_binary_directory)

# Corresponding smoke directory
SMOKE_BLDDIR = $(abspath $(top_builddir))/tests/smoke/functional/BinaryAnalysis
SMOKE_SRCDIR = $(abspath $(top_srcdir))/tests/smoke/functional/BinaryAnalysis

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/BinaryAnalysis/libraryIdentification
AM_LDFLAGS = $(ROSE_RPATHS)

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
TEST_WITH_ANSWER = $(top_srcdir)/scripts/test_with_answer

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

# Some tests are disabled for certain combinations of compiler vendor, compiler version, and compiler language. We do that
# by calling this program, which returns a non-empty string describing why something is disabled, or an empty string if
# not disabled. Furthermore, if a test cannot even be compiled for a certain combination, we use conditionalDisable.h in
# the test program itself to cause it to compile an alternative main which prints a message and aborts.
noinst_PROGRAMS += conditionalDisable
conditionalDisable_SOURCES = conditionalDisable.C
conditionalDisable_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

# Some tests are disabled because the ARM AArch64 A64 support is not enabled in ROSE.
noinst_PROGRAMS += isA64Disabled
isA64Disabled_SOURCES = isA64Disabled.C
isA64Disabled_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

###############################################################################################################################
# Check parsing of symbolic expressions via Rose::BinaryAnalysis::SymbolicExprParser
###############################################################################################################################
noinst_PROGRAMS += testSymbolicExprParser
testSymbolicExprParser_SOURCES = testSymbolicExprParser.C
testSymbolicExprParser_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymbolicExprParser.passed

testSymbolicExprParser.passed: $(TEST_WITH_ANSWER) testSymbolicExprParser testSymbolicExprParser.ans conditionalDisable
	@$(RTH_RUN)						\
		TITLE="parsing of symbolic expressions [$@]"	\
		DISABLED="$$(./conditionalDisable)"		\
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

$(testControlFlow_targets): \
testControlFlow-%.passed: $(TEST_WITH_ANSWER) testControlFlow testControlFlow-%.ans conditionalDisable
	@$(RTH_RUN)							\
		TITLE="AST-based control flow graph, algorithm $* [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
		CMD="./testControlFlow $* $(SPECIMEN_DIR)/buffer2.bin"	\
		ANS="$(srcdir)/testControlFlow-$*.ans"			\
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
EXTRA_DIST += testFunctionCall-A.ans testFunctionCall-B.ans

testFunctionCall-A.passed: $(TEST_WITH_ANSWER) testFunctionCall testFunctionCall-A.ans conditionalDisable
	@$(RTH_RUN)							\
		TITLE="function calls, algorithm A [$@]"		\
		DISABLED="$$(./conditionalDisable)"			\
		CMD="./testFunctionCall A $(SPECIMEN_DIR)/buffer2.bin"	\
		ANS="$(srcdir)/testFunctionCall-A.ans"			\
		$< $@

testFunctionCall-B.passed: $(TEST_WITH_ANSWER) testFunctionCall testFunctionCall-B.ans conditionalDisable
	@$(RTH_RUN)							\
		TITLE="function calls, algorithm B [$@]"		\
		DISABLED="$$(./conditionalDisable)"			\
		CMD="./testFunctionCall B $(SPECIMEN_DIR)/buffer2.bin"	\
		ANS="$(srcdir)/testFunctionCall-B.ans"			\
		$< $@


###############################################################################################################################
# Tests ELF string table reallocation functions by changing some strings.  At first glance this would appear to be something
# quite easy to do, but it turns out to involve lots of details.
###############################################################################################################################
noinst_PROGRAMS += testElfStrtab
testElfStrtab_SOURCES = testElfStrtab.C
testElfStrtab_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testElfStrtab.passed

testElfStrtab.passed: $(TEST_EXIT_STATUS) testElfStrtab conditionalDisable
	@$(RTH_RUN)								\
		TITLE="ELF string table allocation [$@]"			\
		DISABLED="$$(./conditionalDisable)"				\
		CMD="$$(pwd)/testElfStrtab $(SPECIMEN_DIR)/i386-poweroff"	\
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

testElfByteOrder.passed: testElfByteOrder.conf testElfByteOrder execFormatsTest testElfStrtab.passed conditionalDisable
	@$(RTH_RUN)					\
		TITLE="modification of byte order [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		INPUT=i386-poweroff			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
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

testElfWordSize.passed: testElfWordSize.conf testElfWordSize execFormatsTest testElfStrtab.passed testElfByteOrder.passed conditionalDisable
	@$(RTH_RUN)					\
		TITLE="modification of word size [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		INPUT=i386-poweroff			\
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

testElfSchulz-1.passed: testElfSchulz.conf testElfSchulz execFormatsTest conditionalDisable
	@$(RTH_RUN)							\
		TITLE="rename dynamically linked functions #1 [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
		N=1							\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"				\
		$< $@
testElfSchulz-2.passed: testElfSchulz.conf testElfSchulz execFormatsTest conditionalDisable
	@$(RTH_RUN)							\
		TITLE="rename dynamically linked functions #2 [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
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

testElfConstruct.passed: $(TEST_EXIT_STATUS) testElfConstruct conditionalDisable
	@$(RTH_RUN)						\
		TITLE="build an ELF file from scratch [$@]"	\
		DISABLED="$$(./conditionalDisable)"		\
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

testPeConstruct.passed: $(TEST_EXIT_STATUS) testPeConstruct conditionalDisable
	@$(RTH_RUN)						\
		TITLE="build a PE file from scratch [$@]"	\
		DISABLED="$$(./conditionalDisable)"		\
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

shiftResizeSection.passed: $(TEST_WITH_ANSWER) shiftResizeSection execFormatsTest shiftResizeSection.ans conditionalDisable
	@$(RTH_RUN)											\
		TITLE="changing section size and/or location [$@]"					\
		DISABLED="$$(./conditionalDisable)"							\
		CMD="$(srcdir)/testShiftResizeSection.sh $(SPECIMEN_DIR)/i386-ctrlaltdel $$(pwd)"	\
		ANS="$(srcdir)/shiftResizeSection.ans"							\
		USE_SUBDIR=yes										\
		$< $@


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

functionBoundaries_1.passed: $(SPECIMEN_DIR)/i686-test1.O0.bin functionBoundaries functionBoundaries_1.ans conditionalDisable
	@$(RTH_RUN)							\
		TITLE="detect function boundaries $(notdir $<) [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
		USE_SUBDIR=yes						\
		CMD="$$(pwd)/functionBoundaries $<"			\
		ANS=$(srcdir)/functionBoundaries_1.ans			\
		$(TEST_WITH_ANSWER) $@
functionBoundaries_2.passed: $(SPECIMEN_DIR)/i686-test1.O3.bin functionBoundaries functionBoundaries_2.ans conditionalDisable
	@$(RTH_RUN)							\
		TITLE="detect function boundaries $(notdir $<) [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
		USE_SUBDIR=yes						\
		CMD="$$(pwd)/functionBoundaries $<"			\
		ANS=$(srcdir)/functionBoundaries_2.ans			\
		$(TEST_WITH_ANSWER) $@
functionBoundaries_3.passed: $(SPECIMEN_DIR)/i686-test1.O3-stripped.bin functionBoundaries functionBoundaries_3.ans conditionalDisable
	@$(RTH_RUN)							\
		TITLE="detect function boundaries $(notdir $<) [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
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

rot13.passed: rot13.conf rot13 conditionalDisable
	@$(RTH_RUN)					\
		TITLE="binary ROT-13 [$@]"		\
		DISABLED="$$(./conditionalDisable)"	\
		INPUT=i686-test1.O0.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		$< $@


###############################################################################################################################
# Program to test the assembler
###############################################################################################################################
noinst_PROGRAMS += testAssembler
testAssembler_SOURCES=testAssembler.C
testAssembler_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testAssembler.passed
EXTRA_DIST += testAssembler.ans

testAssembler.passed: $(TEST_WITH_ANSWER) testAssembler testAssembler.ans conditionalDisable
	@$(RTH_RUN)							\
		TITLE="x86 assembler [$@]"				\
		DISABLED="$$(./conditionalDisable)"			\
		CMD="$$(pwd)/testAssembler $(SPECIMEN_DIR)/buffer2.bin"	\
		USE_SUBDIR=yes						\
		ANS="$(srcdir)/testAssembler.ans"			\
		$< $@


###############################################################################################################################
# Program to test that we can write and then read an AST for a binary executable
###############################################################################################################################
noinst_PROGRAMS += testAstIO
testAstIO_SOURCES=testAstIO.C
testAstIO_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TEST_TARGETS += testAstIO.passed

testAstIO.passed: $(TEST_EXIT_STATUS) testAstIO conditionalDisable
	@$(RTH_RUN)								\
		TITLE="writing and reading binary AST format [$@]"		\
		DISABLED="$$(./conditionalDisable)"				\
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
testAstNeuter.passed: testAstNeuter.conf testAstNeuter conditionalDisable
	@$(RTH_RUN)							\
		TITLE="that neutering works across binary AST IO [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
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

testRegisters.passed: $(TEST_EXIT_STATUS) testRegisters conditionalDisable
	@$(RTH_RUN)							\
		TITLE="register descriptors and dictionaries [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
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

testReadPastEOF.passed: testReadPastEOF.conf testReadPastEOF testReadPastEOF.ans conditionalDisable
	@$(RTH_RUN)					\
		TITLE="reading past end-of-file [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		$< $@


###############################################################################################################################
# Not sure what this does.
###############################################################################################################################
if ROSE_USE_SQLITE_DATABASE

noinst_PROGRAMS += testLibraryDb
testLibraryDb_SOURCES = testLibraryDb.C
testLibraryDb_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testLibraryDb.passed

testLibraryDb.passed: $(TEST_EXIT_STATUS) testLibraryDb conditionalDisable
	@$(RTH_RUN)					\
		TITLE="library identification [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		CMD="$$(pwd)/testLibraryDb"		\
		USE_SUBDIR=yes				\
		$< $@

endif


###############################################################################################################################
# Tests reading import sections from PE files
###############################################################################################################################
noinst_PROGRAMS += testPeImports
testPeImports_SOURCES = testPeImports.C
testPeImports_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testPeImports.passed
EXTRA_DIST += testPeImports.ans

testPeImports.passed: $(TEST_WITH_ANSWER) testPeImports testPeImports.ans conditionalDisable
	@$(RTH_RUN)												\
		TITLE="that PE import tables can be parsed [$@]"						\
		DISABLED="$$(./conditionalDisable)"								\
		CMD="$$(pwd)/testPeImports -rose:read_executable_file_format_only $(SPECIMEN_DIR)/proxycfg.exe"	\
		USE_SUBDIR=yes											\
		ANS="$(srcdir)/testPeImports.ans"								\
		$< $@


###############################################################################################################################
# Binary CFG test from Cory Cohen that doesn't compile.  This is only a linking test--do not run it.
###############################################################################################################################
noinst_PROGRAMS += cory002
cory002_SOURCES = cory002.C
cory002_LDADD = $(ROSE_SEPARATE_LIBS)


###############################################################################################################################
# Test certain RegisterStateGeneric stuff
###############################################################################################################################
noinst_PROGRAMS += cory003
cory003_SOURCES = cory003.C
cory003_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory003.passed

cory003.passed: $(TEST_EXIT_STATUS) cory003 conditionalDisable
	@$(RTH_RUN)					\
		TITLE="generic register states [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=./cory003				\
		$< $@


###############################################################################################################################
# Test register clearing in subclass constructor
###############################################################################################################################
noinst_PROGRAMS += cory004
cory004_SOURCES = cory004.C
cory004_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory004.passed

cory004.passed: $(TEST_EXIT_STATUS) cory004 conditionalDisable
	@$(RTH_RUN)							\
		TITLE="register clearing in subclass container [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
		CMD=./cory004						\
		$< $@


###############################################################################################################################
# Test disassembling between functions
###############################################################################################################################
noinst_PROGRAMS += cory006
cory006_SOURCES = cory006.C
cory006_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory006.passed

cory006.passed: $(TEST_EXIT_STATUS) cory006 conditionalDisable
	@$(RTH_RUN)						\
		TITLE="inter-functon disassembly [$@]"		\
		DISABLED="$$(./conditionalDisable)"		\
		CMD="./cory006 $(SPECIMEN_DIR)//i386-cory006"	\
		$< $@


###############################################################################################################################
# Test and demonstrate binary attributes
###############################################################################################################################
noinst_PROGRAMS += cory007
cory007_SOURCES = cory007.C
cory007_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory007.passed

cory007.passed: $(TEST_EXIT_STATUS) cory007 conditionalDisable
	@$(RTH_RUN)					\
		TITLE="binary attributes [$@]"		\
		DISABLED="$$(./conditionalDisable)"	\
		CMD="./cory007"				\
		$< $@


###############################################################################################################################
# Test base objects in SgAsmIntegerValueExpression
###############################################################################################################################
noinst_PROGRAMS += cory008
cory008_SOURCES = cory008.C
cory008_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory008.passed
EXTRA_DIST += cory008.ans

cory008.passed: $(TEST_WITH_ANSWER) cory008 cory008.ans conditionalDisable
	@$(RTH_RUN)								\
		TITLE="base objects in SgAsmIntegerValueExpression [$@]"	\
		DISABLED="$$(./conditionalDisable)"				\
		CMD="./cory008 $(SPECIMEN_DIR)/i386-nologin"			\
		ANS=$(srcdir)/cory008.ans					\
		$< $@


###############################################################################################################################
# Test symbolic structural comparison of integer constants
###############################################################################################################################
noinst_PROGRAMS += cory009
cory009_SOURCES = cory009.C
cory009_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += cory009.passed

cory009.passed: $(TEST_EXIT_STATUS) cory009 conditionalDisable
	@$(RTH_RUN)									\
		TITLE="test symbolic structural comparison of integer constants [$@]"	\
		DISABLED="$$(./conditionalDisable)"					\
		CMD="./cory009"								\
		$< $@


###############################################################################################################################
# Test symbolic semantics memory read-after-write
###############################################################################################################################
noinst_PROGRAMS += testSymReadWrite
testSymReadWrite_SOURCES = testSymReadWrite.C
testSymReadWrite_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymReadWrite.passed
EXTRA_DIST += testSymReadWrite.conf testSymReadWrite.ans

testSymReadWrite.passed: testSymReadWrite.conf testSymReadWrite.ans testSymReadWrite conditionalDisable
	@$(RTH_RUN)							\
		TITLE="symbolic semantic domain read/write [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
		INPUT=memreadwrite					\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"				\
		$< $@


###############################################################################################################################
# Test the WorkList class
###############################################################################################################################
noinst_PROGRAMS += testWorkList
testWorkList_SOURCES = testWorkList.C
testWorkList_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testWorkList.passed

testWorkList.passed: $(TEST_EXIT_STATUS) testWorkList conditionalDisable
	@$(RTH_RUN)					\
		TITLE="work list unit tests [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=./testWorkList			\
		$< $@


###############################################################################################################################
# Test ROSE's Map<T> class
###############################################################################################################################

if !USING_INTEL_COMPILER

# DQ (11/17/2017): The testMap.C code causes an internal compiler error for the Intel v16 compiler.

noinst_PROGRAMS += testMap
testMap_SOURCES = testMap.C
testMap_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testMap.passed

testMap.passed: $(TEST_EXIT_STATUS) testMap conditionalDisable
	@$(RTH_RUN)							\
		TITLE="custom Map class unit tests (deprecated) [$@]"	\
		DISABLED="$$(./conditionalDisable)"			\
		CMD=./testMap						\
		$< $@
endif

###############################################################################################################################
# Test pointer detection
###############################################################################################################################
noinst_PROGRAMS += testPointerDetection
testPointerDetection_SOURCES = testPointerDetection.C
testPointerDetection_LDADD = $(ROSE_SEPARATE_LIBS)

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

testPointerDetection.passed: $(TEST_WITH_ANSWER) testPointerDetection testPointerDetection.ans conditionalDisable
	@$(RTH_RUN)											\
	    TITLE="pointer detection [$@]"								\
	    DISABLED="$$(./conditionalDisable)"								\
	    CMD="./testPointerDetection $(PointerDetectionSwitches) $(PointerDetectionSpecimen)"	\
	    ANS=$(srcdir)/testPointerDetection.ans							\
	    $< $@


###############################################################################################################################
# Test function call detection
###############################################################################################################################
noinst_PROGRAMS += testCallDetection
testCallDetection_SOURCES = testCallDetection.C
testCallDetection_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testCallDetection.passed
EXTRA_DIST += testCallDetection.ans

testCallDetection.passed: $(TEST_WITH_ANSWER) $(SPECIMEN_DIR)/i386-fcalls testCallDetection testCallDetection.ans conditionalDisable
	@$(RTH_RUN)								\
		TITLE="function call detection [$@]"				\
		DISABLED="$$(./conditionalDisable)"				\
		CMD="$$(pwd)/testCallDetection $(SPECIMEN_DIR)/i386-fcalls"	\
		USE_SUBDIR=yes							\
		ANS=$(srcdir)/testCallDetection.ans				\
		$< $@


###############################################################################################################################
# Test analysis to see whether a function returns a value
###############################################################################################################################
noinst_PROGRAMS += testReturnsValue
testReturnsValue_SOURCES = testReturnsValue.C
testReturnsValue_CPPFLAGS = -DROSE_BINARY_TEST_DISABLED='"deprecation"' $(AM_CPPFLAGS)
testReturnsValue_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testReturnsValue.passed

testReturnsValue.passed: $(SPECIMEN_DIR)/buffer2.bin testReturnsValue $(TEST_EXIT_STATUS) conditionalDisable
	@$(RTH_RUN)						\
		TITLE="function-returns-value analysis [$@]"	\
		DISABLED="deprecated $$(./conditionalDisable)"	\
		CMD="./testReturnsValue $<"			\
		$(TEST_EXIT_STATUS) $@


###############################################################################################################################
# Unit tests for use-def (executed created below)
###############################################################################################################################
# Uses "symbolicSemantics2" executable compiled in another test.

TEST_TARGETS += usedef.passed
EXTRA_DIST += usedef.ans usedef.conf

usedef.passed: usedef.conf symbolicSemantics2 $(SPECIMEN_DIR)/usedef conditionalDisable
	@$(RTH_RUN)											\
		USE_SUBDIR=yes										\
		DISABLED="$$(./conditionalDisable)"							\
		TITLE="use-def unit tests"								\
		CMD="$$(pwd)/symbolicSemantics2 --trace --usedef $(abspath $(SPECIMEN_DIR)/usedef)"	\
		ANS="$(srcdir)/usedef.ans"								\
		$< $@


###############################################################################################################################
# Test BitPattern<> class template
###############################################################################################################################
noinst_PROGRAMS += testBitPattern
testBitPattern_SOURCES = testBitPattern.C
testBitPattern_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testBitPattern.passed

testBitPattern.passed: $(TEST_EXIT_STATUS) testBitPattern conditionalDisable
	@$(RTH_RUN)					\
		TITLE="bit pattern matcher [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		CMD="./testBitPattern"			\
		$< $@


###############################################################################################################################
# Random number generator tests
###############################################################################################################################
noinst_PROGRAMS += testRNG
testRNG_SOURCES = testRNG.C
testRNG_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testRNG.passed

testRNG.passed: $(TEST_EXIT_STATUS) testRNG conditionalDisable
	@$(RTH_RUN)								\
		TITLE="pseudo random number generator (deprecated) [$@]"	\
		DISABLED="$$(./conditionalDisable)"				\
		CMD="./testRNG"							\
		$< $@


###############################################################################################################################
# Symbolic expression simplifications
###############################################################################################################################
noinst_PROGRAMS += testSymbolicSimplification
testSymbolicSimplification_SOURCES = testSymbolicSimplification.C
testSymbolicSimplification_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymbolicSimplification.passed
EXTRA_DIST += testSymbolicSimplification.ans

testSymbolicSimplification.passed: $(TEST_WITH_ANSWER) testSymbolicSimplification testSymbolicSimplification.ans conditionalDisable
	@$(RTH_RUN)						\
		TITLE="symbolic simplification [$@]"		\
		DISABLED="$$(./conditionalDisable)"		\
		CMD="./testSymbolicSimplification"		\
		ANS="$(srcdir)/testSymbolicSimplification.ans"	\
		$< $@


###############################################################################################################################
# Symbolic expression user-defined flags
###############################################################################################################################
noinst_PROGRAMS += testSymbolicFlags
testSymbolicFlags_SOURCES = testSymbolicFlags.C
testSymbolicFlags_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymbolicFlags.passed
EXTRA_DIST += testSymbolicFlags.ans

testSymbolicFlags.passed: $(TEST_WITH_ANSWER) testSymbolicFlags testSymbolicFlags.ans conditionalDisable
	@$(RTH_RUN)					\
		TITLE="symbolic flags [$@]"		\
		DISABLED="$$(./conditionalDisable)"	\
		CMD="./testSymbolicFlags"		\
		ANS="$(srcdir)/testSymbolicFlags.ans"	\
		$< $@


###############################################################################################################################
# Instruction semantics verification.
###############################################################################################################################
noinst_PROGRAMS += verifySemantics
verifySemantics_SOURCES = verifySemantics.C
verifySemantics_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += verifySemantics.passed

verifySemantics.passed: $(TEST_EXIT_STATUS) verifySemantics
	@$(RTH_RUN)							\
		TITLE="instruction semantics verification"		\
		DISABLED="not portable enough to run automatically"	\
		CMD=false						\
		$< $@


###############################################################################################################################
# Disassembly tests using Rose::BinaryAnalysis::Partitioner2
###############################################################################################################################

# Disassembles executables using Partitioner2
# Disassembly of executable files (DOS, ELF, PE) of various architectures (amd64, Arm, Mips, M68k, PowerPC, x86)
# + MIPS specimens are currently failing a FIXME assertion in makeShadowRegister()
# + PowerPC specimens have lots of "XL-Form xoOpcode = 36 not handled!" and similar errors
# + The pocketputty.exe pocketputty2.exe puttytel.exe are not good unit tests because they're too large; they take too
#   long when extensive debugging is enabled on heavily loaded or slow machines.

$(SMOKE_BLDDIR)/testPartitioner2: $(SMOKE_SRCDIR)/testPartitioner2.C
	$(MAKE) -C $(SMOKE_BLDDIR) testPartitioner2

# Be careful where the comma is located!
testPartitioner2_specimens = $(filter-out		\
	$(nonsmoke_specimens_binary_any_any_mips)	\
	$(nonsmoke_specimens_binary_any_any_ppc)	\
	$(nonsmoke_specimens_binary_any_any_arm)	\
	$(nonsmoke_specimens_binary_large),		\
	$(nonsmoke_specimens_binary_any_exe_any))

testPartitioner2_test_targets = $(addprefix testPartitioner2_, $(addsuffix .passed, $(testPartitioner2_specimens)))
testPartitioner2_answers = $(testPartitioner2_test_targets:.passed=.ans)

EXTRA_DIST += $(testPartitioner2_answers)
TEST_TARGETS += $(testPartitioner2_test_targets)
TEST_TARGETS += testPartitioner2_buffer2.bin.passed

$(testPartitioner2_test_targets): \
testPartitioner2_%.passed: $(SPECIMEN_DIR)/% $(SMOKE_BLDDIR)/testPartitioner2 testPartitioner2_%.ans conditionalDisable
	@$(RTH_RUN)													\
		TITLE="testPartitioner2 $(notdir $<) [$@]"								\
		DISABLED="$$(./conditionalDisable)"									\
		USE_SUBDIR=yes												\
		CMD="$(SMOKE_BLDDIR)/testPartitioner2 $(if $(findstring exefmt,$<), --no-inter-function-calls) $<"	\
		ANS="$(srcdir)/testPartitioner2_$*.ans"									\
		$(top_srcdir)/scripts/test_with_answer $@

# Another set of partitioner tests, but using the map-based semantic memory
testP2map_test_targets = $(addprefix testP2map_, $(addsuffix .passed, $(testPartitioner2_specimens)))
testP2map_answers = $(testPartitioner2_test_target:.passed=.ans)

EXTRA_DIST += $(testP2map_answers)
TEST_TARGETS += $(testP2map_test_targets)

$(testP2map_test_targets): \
testP2map_%.passed: $(SPECIMEN_DIR)/% $(SMOKE_BLDDIR)/testPartitioner2 conditionalDisable
	@$(RTH_RUN)													\
		TITLE="testPartitioner2 map $(notdir $<) [$@]"								\
		DISABLED="$$(./conditionalDisable)"									\
		USE_SUBDIR=yes												\
		CMD="$(SMOKE_BLDDIR)/testPartitioner2 $(if $(findstring exefmt,$<), --no-inter-function-calls) --semantic-memory=map --use-semantics $<"	\
		ANS="$(srcdir)/testP2map_$*.ans"									\
		$(top_srcdir)/scripts/test_with_answer $@



PHONIES += check-testPartitioner2
check-testPartitioner2: $(testPartitioner2_test_targets) $(testP2map_test_targets)


###############################################################################################################################
# CFG dominance tests
###############################################################################################################################

noinst_PROGRAMS += testDominance
testDominance_SOURCES = testDominance.C
testDominance_CPPFLAGS = -I$(top_srcdir)
testDominance_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

testDominance_specimens = $(testPartitioner2_specimens)
testDominance_test_targets = $(patsubst %, testDominance_%.passed, $(testDominance_specimens))
testDominance_answers = $(testDominance_test_targets:.passed=.ans)

EXTRA_DIST += $(testDominance_answers)
TEST_TARGETS += $(testDominance_test_targets)

$(testDominance_test_targets): testDominance_%.passed: $(SPECIMEN_DIR)/% testDominance testDominance_%.ans conditionalDisable
	@$(RTH_RUN)						\
		TITLE="graph dominance $(notdir $<) [$@]"	\
		DISABLED="$$(./conditionalDisable)"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testDominance $<"			\
		ANS="$(srcdir)/testDominance_$*.ans"		\
		$(top_srcdir)/scripts/test_with_answer $@

PHONIES += check-testDominance
check-testDominance: $(testDominance_test_targets)


###############################################################################################################################
# Instruction semantics tests.
#
# Binary instruction semantics are a plug-n-play architecture, so we try to test a number of different combinations, usually
# all compiled from a single source file.  Files with "1" in their name use the older, template-based API while files with
# "2" in their name use the newer run-time plug-n-play API.
###############################################################################################################################

# Convenience target to run all these semantics tests. This target is not run automatically by "make check" (which instead
# depends on each of the targets individually rather than just this one).
PHONIES += check-semantics
#       API2 (the newer stuff)               Extra tests
check-semantics:								\
        nullSemantics2-x86.passed						\
        partialSymbolicSemantics2-x86.passed					\
        intervalSemantics2-x86.passed						\
        symbolicSemantics2-x86.passed        traceSymbolicSemantics2-x86.passed	\
        yicesSemanticsExe2-x86.passed						\
        yicesSemanticsLib2-x86.passed						\
	z3SemanticsExe2-x86.passed						\
	z3SemanticsLib2-x86.passed						\
        semanticsSubclassing.passed

# TOO1 (3/24/2015): Failing jenkins-release GCC 4.2.4; removing temporarily until fixed
#multiSemantics2.passed

# Null semantics
noinst_PROGRAMS += nullSemantics2
nullSemantics2_SOURCES = semantics.C
nullSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=NULL_DOMAIN
nullSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += nullSemantics2-x86.passed
EXTRA_DIST += semantics.conf nullSemantics2-x86.ans
nullSemantics2-x86.passed: semantics.conf nullSemantics2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=nullSemantics2			\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@

# Partial symbolic semantics
noinst_PROGRAMS += partialSymbolicSemantics2
partialSymbolicSemantics2_SOURCES = semantics.C
partialSymbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=PARTSYM_DOMAIN
partialSymbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += partialSymbolicSemantics2-x86.passed
EXTRA_DIST += semantics.conf partialSymbolicSemantics2-x86.ans
partialSymbolicSemantics2-x86.passed: semantics.conf partialSymbolicSemantics2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=partialSymbolicSemantics2		\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@

# Interval semantics
noinst_PROGRAMS += intervalSemantics2
intervalSemantics2_SOURCES = semantics.C
intervalSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=INTERVAL_DOMAIN
intervalSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += intervalSemantics2-x86.passed
EXTRA_DIST += semantics.conf intervalSemantics2-x86.ans
intervalSemantics2-x86.passed: semantics.conf intervalSemantics2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=intervalSemantics2			\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@

# Symbolic semantics, no SMT solver
noinst_PROGRAMS += symbolicSemantics2
symbolicSemantics2_SOURCES = semantics.C
symbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=NO_SOLVER
symbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += symbolicSemantics2-x86.passed
EXTRA_DIST += semantics.conf symbolicSemantics2-x86.ans
symbolicSemantics2-x86.passed: semantics.conf symbolicSemantics2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=symbolicSemantics2			\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@

# Tracing symbolic semantics, no SMT solver
noinst_PROGRAMS += traceSymbolicSemantics2
traceSymbolicSemantics2_SOURCES = semantics.C
traceSymbolicSemantics2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=NO_SOLVER
traceSymbolicSemantics2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
TEST_TARGETS += traceSymbolicSemantics2-x86.passed
EXTRA_DIST += semantics.conf traceSymbolicSemantics2-x86.ans
traceSymbolicSemantics2-x86.passed: semantics.conf traceSymbolicSemantics2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=traceSymbolicSemantics2		\
		SWITCHES="--trace --no-usedef"		\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@

# Symbolic semantics, Yices executable
noinst_PROGRAMS += yicesSemanticsExe2
yicesSemanticsExe2_SOURCES = semantics.C
yicesSemanticsExe2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=YICES_EXE
yicesSemanticsExe2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsExe2-x86.ans
if ROSE_HAVE_YICES
TEST_TARGETS += yicesSemanticsExe2-x86.passed
yicesSemanticsExe2-x86.passed: semantics.conf yicesSemanticsExe2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=yicesSemanticsExe2			\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@
endif

# Symbolic semantics, Yices library
noinst_PROGRAMS += yicesSemanticsLib2
yicesSemanticsLib2_SOURCES = semantics.C
yicesSemanticsLib2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=YICES_LIB
yicesSemanticsLib2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf yicesSemanticsLib2-x86.ans
if ROSE_HAVE_LIBYICES
TEST_TARGETS += yicesSemanticsLib2-x86.passed
yicesSemanticsLib2-x86.passed: semantics.conf yicesSemanticsLib2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=yicesSemanticsLib2			\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@
endif

# Symbolic semantics, Z3 executable
noinst_PROGRAMS += z3SemanticsExe2
z3SemanticsExe2_SOURCES = semantics.C
z3SemanticsExe2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=Z3_EXE
z3SemanticsExe2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf z3SemanticsExe2-x86.ans
if ROSE_HAVE_Z3
TEST_TARGETS += z3SemanticsExe2-x86.passed
z3SemanticsExe2-x86.passed: semantics.conf z3SemanticsExe2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=z3SemanticsExe2			\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@
endif

# Symbolic semantics, Z3 library
noinst_PROGRAMS += z3SemanticsLib2
z3SemanticsLib2_SOURCES = semantics.C
z3SemanticsLib2_CPPFLAGS = -DSEMANTIC_DOMAIN=SYMBOLIC_DOMAIN -DSMT_SOLVER=Z3_LIB
z3SemanticsLib2_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
EXTRA_DIST += semantics.conf z3SemanticsLib2-x86.ans
if ROSE_HAVE_LIBZ3
TEST_TARGETS += z3SemanticsLib2-x86.passed
z3SemanticsLib2-x86.passed: semantics.conf z3SemanticsLib2 conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=z3SemanticsLib2			\
		INPUT=i686-test1.O3.bin			\
		SPECIMEN_DIR="$(SPECIMEN_DIR)"		\
		ANSWER=$(srcdir)/$(@:.passed=.ans)	\
		$< $@
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
semanticsSubclassing.passed: $(TEST_EXIT_STATUS) semanticsSubclassing conditionalDisable
	@$(RTH_RUN)					\
		DISABLED="$$(./conditionalDisable)"	\
		CMD=./semanticsSubclassing		\
		$< $@


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
# Only x86 32-bit is fully supported
llvmTranscoder_Specimens =					\
        $(filter-out						\
	        pentium-xmm					\
		$(nonsmoke_specimens_binary_large),		\
		$(nonsmoke_specimens_binary_any_exe_x86))

llvmTranscoder_TestTargets = $(addprefix llvm-xcode-, $(addsuffix .passed, $(llvmTranscoder_Specimens)))
TEST_TARGETS += $(llvmTranscoder_TestTargets)
EXTRA_DIST += llvmTranscoder.conf

$(llvmTranscoder_TestTargets): llvm-xcode-%.passed: $(SPECIMEN_DIR)/% llvmTranscoder llvmTranscoder.conf conditionalDisable
	@$(RTH_RUN)					\
		TITLE="LLVM transcoder for $* [$@]"	\
		DISABLED="$$(./conditionalDisable)"	\
		SPECIMEN="$(abspath $<)"		\
		LLVM_VERSION="$(llvmVersion)"		\
		$(srcdir)/llvmTranscoder.conf $@

PHONIES += check-llvm-xcode
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

$(llvmAnalysis_TestTargets): llvm-analysis-%.passed: $(llvmAnalysis_SpecimenDir)/%.c llvmTranscoder conditionalDisable
	@$(RTH_RUN)											\
		TITLE="LLVM analysis for $(notdir $<) [$@]"						\
		DISABLED="$$(./conditionalDisable)"							\
		USE_SUBDIR=yes										\
		SPECIMEN="$<"										\
		OPT_PASSES="$(shell perl -ne '/OPT:\s*(.*?)\s*(\*\/\s*)?$$/ and print qq{$$1\n}' $<)"	\
		DISASSEMBLER=$(abspath disassemble)							\
		TRANSCODER="$(abspath llvmTranscoder) --llvm=$(llvmVersion)"				\
		$(srcdir)/llvmAnalysis.conf $@

PHONIES += check-llvm-analysis
check-llvm-analysis: $(llvmAnalysis_TestTargets)

#-------------- all LLVM-specific tests

PHONIES += check-llvm
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

$(taintedFlow_TestTargets): taint_%.passed: $(SPECIMEN_DIR)/% taintedFlow taintedFlow.conf conditionalDisable
	@$(RTH_RUN)												\
		TITLE="Tainted flow for $* [$@]"								\
		DISABLED="$$(./conditionalDisable)"								\
		USE_SUBDIR=yes											\
		CMD="$(abspath ./taintedFlow) --approx=under --blocks --names='^f[0-9]' -- $(abspath $<)"	\
		ANS="$(abs_srcdir)/taint_$*.ans"								\
		$(abs_srcdir)/taintedFlow.conf $@

PHONIES += check-taint
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

$(testNoop_TestTargets): noop_%.passed: $(SPECIMEN_DIR)/% testNoop conditionalDisable
	@$(RTH_RUN)								\
		TITLE="testNoop $* [$@]"					\
		DISABLED="$$(./conditionalDisable)"				\
		USE_SUBDIR=yes							\
		CMD="$(abspath ./testNoop) $(testNoop_Switches) $(abspath $<)"	\
		ANS="$(abs_srcdir)/noop_$*.ans"					\
		$(top_srcdir)/scripts/test_with_answer $@

PHONIES += check-noop
check-noop: $(testNoop_TestTargets)


###############################################################################################################################
# Static semantics tests
###############################################################################################################################

noinst_PROGRAMS += testStaticSemantics
testStaticSemantics_SOURCES = testStaticSemantics.C
testStaticSemantics_LDADD = $(ROSE_SEPARATE_LIBS)

# We lack semantics for some architectures, so don't try to test them (the result would be an std::runtime_error with the
# message "no instruction semantics for architecture".  Also, we skip pocketputty2.exe because it takes too long with lots
# of debugging is enabled.
testStaticSemantics_specimens =					\
	$(filter-out						\
		$(nonsmoke_specimens_binary_any_any_mips)	\
		$(nonsmoke_specimens_binary_any_any_ppc)	\
		$(nonsmoke_specimens_binary_any_any_arm)	\
		$(nonsmoke_specimens_large)			\
		,$(nonsmoke_specimens_binary_any_exe_any))

testStaticSemantics_test_targets = $(addprefix testStaticSemantics_, $(addsuffix .passed, $(testStaticSemantics_specimens)))
TEST_TARGETS += $(testStaticSemantics_test_targets)

$(testStaticSemantics_test_targets): testStaticSemantics_%.passed: $(SPECIMEN_DIR)/% testStaticSemantics conditionalDisable
	@$(RTH_RUN)						\
		TITLE="testStaticSemantics $(notdir $<) [$@]"	\
		DISABLED="$$(./conditionalDisable)"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testStaticSemantics $<"		\
		$(top_srcdir)/scripts/test_exit_status $@

PHONIES += check-testStaticSemantics
check-testStaticSemantics: $(testStaticSemantics_test_targets)


###############################################################################################################################
# Data-flow tests
###############################################################################################################################

noinst_PROGRAMS += testLazyInitialStates
testLazyInitialStates_SOURCES = testLazyInitialStates.C
testLazyInitialStates_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testLazyInitialStates.passed

testLazyInitialStates.passed: $(SPECIMEN_DIR)/i386-initialState testLazyInitialStates conditionalDisable
	@$(RTH_RUN)									\
		TITLE="lazy symbolic states [$@]"					\
		DISABLED="$$(./conditionalDisable)"					\
		USE_SUBDIR=yes								\
		CMD="$$(pwd)/testLazyInitialStates --isa=i386 --function-at=0 map:0=rx::$<"	\
		$(top_srcdir)/scripts/test_exit_status $@


###############################################################################################################################
# Test various things for all our sample binaries
###############################################################################################################################

#--------------------------------------------------------------------------------
#FIXME: all the ARM specimens are failing, and have been. This might be due to poor disassembly. [Robb Matzke 2013-08-30]
astio_Specimens =						\
	$(filter-out						\
		$(nonsmoke_specimens_binary_any_any_arm)	\
		$(nonsmoke_specimens_binary_large),		\
		$(nonsmoke_specimens_binary_any_exe_any))

astio_Targets = $(addprefix astio_, $(addsuffix .passed, $(astio_Specimens)))

TEST_TARGETS += $(astio_Targets)

$(astio_Targets): astio_%.passed: $(SPECIMEN_DIR)/% testAstIO conditionalDisable
	@$(RTH_RUN)							\
		TITLE="binary AST IO for $(notdir $<) [$@]"		\
		DISABLED="$$(./conditionalDisable)"			\
		CMD="$$(pwd)/testAstIO -rose:binary $(abspath $<)"	\
		USE_SUBDIR=yes						\
		$(TEST_EXIT_STATUS) $@

PHONIES += check_astio
check_astio: $(astio_Targets)

#--------------------------------------------------------------------------------
PHONIES += check_specimens
check_specimens: check_parse_container check_disassemble check_unparse check_assemble check_astio

#############################################################################################
# Test disassembling random input data for various architectures.
#############################################################################################

noinst_PROGRAMS += testRandomInput
testRandomInput_SOURCES = testRandomInput.C
testRandomInput_LDADD = $(ROSE_SEPARATE_LIBS)

testRandomInput_ISAs = amd64 coldfire i386 m68040 mips-be mips-le ppc32-be ppc32-le ppc64-be ppc64-le
testRandomInput_Targets = $(addprefix tri_, $(addsuffix .passed, $(testRandomInput_ISAs)))
TEST_TARGETS += $(testRandomInput_Targets) tri_a64.passed

$(testRandomInput_Targets): tri_%.passed: testRandomInput conditionalDisable
	@$(RTH_RUN)							\
		TITLE="disassemble random $* input [$@]"		\
		DISABLED="$$(./conditionalDisable)"			\
		CMD="$$(pwd)/testRandomInput --size=50000 --isa=$*"	\
		$(TEST_EXIT_STATUS) $@

tri_a64.passed: testRandomInput conditionalDisable isA64Disabled
	@$(RTH_RUN)							\
		TITLE="disassemble random $* input [$@]"		\
		DISABLED="$$(./conditionalDisable)$$(./isA64Disabled)"	\
		CMD="$$(pwd)/testRandomInput --size=50000 --isa=a64"	\
		$(TEST_EXIT_STATUS) $@


PHONIES += check-random-input
check-random-input: $(testRandomInput_Targets) tri_a64.passed

#############################################################################################
# Serialization of IR nodes related to binary analysis
#############################################################################################

# DQ (2/6/2017): This fails to to compile with GNU 4.8.4 and Boost 1.60.
# noinst_PROGRAMS += testSageSerialization
# testSageSerialization_SOURCES = testSageSerialization.C
# testSageSerialization_LDADD = $(ROSE_SEPARATE_LIBS)

# TEST_TARGETS += testSageSerialization.passed

# testSageSerialization.passed: testSageSerialization conditionalDisable
# 	@$(RTH_RUN)								\
#		TITLE="serialization of Sage binary analysis nodes [$@]"	\
#		DISABLED="$$(./conditionalDisable)"				\
#		CMD="$$(pwd)/testSageSerialization"				\
#		$(top_srcdir)/scripts/test_exit_status $@



#############################################################################################
# PDF File Generation for Binaries
#############################################################################################

# DQ (3/26/2017): Added PDF test for ROSE evaluation (forces testing of parts of ROSE for lcov)
test_pdf_translator = $(abspath ../testPDFGeneration)
test_pdf_small_specimen = $(top_srcdir)/tests/nonsmoke/specimens/binary/i386-pop
test_pdf_large_specimen = $(top_srcdir)/tests/nonsmoke/specimens/binary/pocketputty.exe

$(test_pdf_translator):
	$(MAKE) -C $(dir $(test_pdf_translator)) $(notdir $(test_pdf_translator))

TEST_TARGETS += test_small_pdf_generation.passed test_large_pdf_generation.passed

test_small_pdf_generation.passed: $(test_pdf_small_specimen) $(test_pdf_translator) conditionalDisable
	@$(RTH_RUN)								\
		TITLE="generation of small PDF file [$@]"			\
		DISABLED="$$(./conditionalDisable)"				\
		USE_SUBDIR=yes							\
		CMD="$(test_pdf_translator) -rose:verbose 0 $< && ls -l *.pdf"	\
		$(TEST_EXIT_STATUS) $@

# Matzke (2017-05-08): Disabled because it can take >900 seconds when debugging is enabled, and the
# functionality is already mostly tested by test_small_pdf_generation.passed
test_large_pdf_generation.passed: $(test_pdf_large_specimen) $(test_pdf_translator) conditionalDisable
	@$(RTH_RUN)								\
		TITLE="generation of large PDF file [$@]"			\
		DISABLED="takes too long"					\
		USE_SUBDIR=yes							\
		CMD="$(test_pdf_translator) -rose:verbose 0 $< && ls -l *.pdf"	\
		$(TEST_EXIT_STATUS) $@

###############################################################################################################################
# Symbolic substitution in huge expressions with many common subexpressions
###############################################################################################################################

noinst_PROGRAMS += testSymbolicSubstitution
testSymbolicSubstitution_SOURCES = testSymbolicSubstitution.C
testSymbolicSubstitution_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSymbolicSubstitution.passed

testSymbolicSubstitution.passed: testSymbolicSubstitution conditionalDisable
	@$(RTH_RUN)					\
	    TITLE="symbolic substitution"		\
	    DISABLED="$$(./conditionalDisable)"		\
	    USE_SUBDIR=yes				\
	    CMD="$$(pwd)/testSymbolicSubstitution"	\
	    $(top_srcdir)/scripts/test_exit_status $@

###############################################################################################################################
# Z3 solver with wide constants
################################################################################################################################

noinst_PROGRAMS += testSmtWideConstant
testSmtWideConstant_SOURCES = testSmtWideConstant.C
testSmtWideConstant_LDADD = $(ROSE_SEPARATE_LIBS)

if ROSE_HAVE_Z3
TEST_TARGETS += testSmtWideConstant-z3exe.passed
testSmtWideConstant-z3exe.passed: $(top_srcdir)/scripts/test_exit_status testSmtWideConstant conditionalDisable
	@$(RTH_RUN)						\
		TITLE="SMT wide constant z3-exe [$@]"		\
		DISABLED="$$(./conditionalDisable)"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testSmtWideConstant z3-exe"	\
		$< $@
endif

if ROSE_HAVE_LIBZ3
TEST_TARGETS += testSmtWideConstant-z3lib.passed
testSmtWideConstant-z3lib.passed: $(top_srcdir)/scripts/test_exit_status testSmtWideConstant conditionalDisable
	@$(RTH_RUN)						\
		TITLE="SMT wide constant z3-lib [$@]"		\
		DISABLED="$$(./conditionalDisable)"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testSmtWideConstant z3-lib"	\
		$< $@
endif

########################################################################################################################
# Test RegisterStateGeneric's peekRegister method
########################################################################################################################

noinst_PROGRAMS += testPeekRegister
testPeekRegister_SOURCES = testPeekRegister.C
testPeekRegister_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testPeekRegister.passed
testPeekRegister.passed: $(top_srcdir)/scripts/test_exit_status testPeekRegister conditionalDisable
	@$(RTH_RUN)						\
		TITLE="RegisterStateGeneric::peekRegister [$@]"	\
		DISABLED="$$(./conditionalDisable)"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testPeekRegister"			\
		$< $@

########################################################################################################################
# Test P2 data blocks
########################################################################################################################

noinst_PROGRAMS += testDataBlockOwnership
testDataBlockOwnership_SOURCES = testDataBlockOwnership.C
testDataBlockOwnership_LDADD = $(ROSE_SEPARATE_LIBS)
testDataBlockOwnership_specimen = $(top_srcdir)/tests/nonsmoke/specimens/binary/x86-64-nologin

TEST_TARGETS += testDataBlockOwnership.passed
testDataBlockOwnership.passed: $(top_srcdir)/scripts/test_exit_status testDataBlockOwnership $(testDataBlockOwnership_specimen)
	@$(RTH_RUN)									\
		TITLE="test data block ownership [$@]"					\
		DISABLED="$$(./conditionalDisable)"					\
		USE_SUBDIR=yes								\
		CMD="$$(pwd)/testDataBlockOwnership $(testDataBlockOwnership_specimen)"	\
		$< $@

########################################################################################################################
# Test S-Records
########################################################################################################################

noinst_PROGRAMS += testSRecords
testSRecords_SOURCES = testSRecords.C
testSRecords_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testSRecords.passed
testSRecords.passed: $(top_srcdir)/scripts/test_exit_status testSRecords
	@$(RTH_RUN)						\
		TITLE="test S-Record parsing/unparsing [$@]"	\
		DISABLED="$$(./conditionalDisable)"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testSRecords"			\
		$< $@

########################################################################################################################
# Test disassembler
########################################################################################################################

noinst_PROGRAMS += testDisassembler
testDisassembler_SOURCES = testDisassembler.C
testDisassembler_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS +=					\
	testDisassembler-ppc32.passed		\
	testDisassembler-ppc64a.passed		\
	testDisassembler-ppc64b.passed		\
	testDisassembler-a64.passed

# PowerPC-32 decoding PowerPC-32 instructions
testDisassembler_ppc32_input = $(srcdir)/testDisassembler-ppc32.txt
testDisassembler_ppc32_answer = $(srcdir)/testDisassembler-ppc32.ans
testDisassembler-ppc32.passed: $(top_srcdir)/scripts/test_with_answer testDisassembler $(testDisassembler_ppc32_input)
	@$(RTH_RUN)										\
		TITLE="test disassembler PowerPC-32 [$@]"					\
		DISABLED="$$(./conditionalDisable)"						\
		USE_SUBDIR=no									\
		CMD="$$(pwd)/testDisassembler --isa=ppc32-be $(testDisassembler_ppc32_input)"	\
		ANS="$(testDisassembler_ppc32_answer)"						\
		$< $@

# PowerPC-64 decoding PowerPC-32 instructions
testDisassembler_ppc64a_input = $(srcdir)/testDisassembler-ppc32.txt
testDisassembler_ppc64a_answer = $(srcdir)/testDisassembler-ppc64a.ans
testDisassembler-ppc64a.passed: $(top_srcdir)/scripts/test_with_answer testDisassembler $(testDisassembler_ppc64a_input)
	@$(RTH_RUN)										\
		TITLE="test disassembler PowerPC-64 [$@]"					\
		DISABLED="$$(./conditionalDisable)"						\
		USE_SUBDIR=no									\
		CMD="$$(pwd)/testDisassembler --isa=ppc64-be $(testDisassembler_ppc64a_input)"	\
		ANS="$(testDisassembler_ppc64a_answer)"						\
		$< $@

# PowerPC-64 decoding PowerPC-64 instructions
testDisassembler_ppc64b_input = $(srcdir)/testDisassembler-ppc64.txt
testDisassembler_ppc64b_answer = $(srcdir)/testDisassembler-ppc64b.ans
testDisassembler-ppc64b.passed: $(top_srcdir)/scripts/test_with_answer testDisassembler $(testDisassembler_ppc64b_input)
	@$(RTH_RUN)										\
		TITLE="test disassembler PowerPC-64 [$@]"					\
		DISABLED="$$(./conditionalDisable)"						\
		USE_SUBDIR=no									\
		CMD="$$(pwd)/testDisassembler --isa=ppc64-be $(testDisassembler_ppc64b_input)"	\
		ANS="$(testDisassembler_ppc64b_answer)"						\
		$< $@

# ARM AArch64 A64
testDisassembler_a64_input = $(srcdir)/testDisassembler-a64.txt
testDisassembler_a64_answer = $(srcdir)/testDisassembler-a64.ans
testDisassembler-a64.passed: $(top_srcdir)/scripts/test_with_answer testDisassembler isA64Disabled $(testDisassembler_a64_input)
	@$(RTH_RUN)										\
		TITLE="test disassembler ARM AArch64 A64 [$@]"					\
		DISABLED="$$(./isA64Disabled)"							\
		USE_SUBDIR=no									\
		CMD="$$(pwd)/testDisassembler --isa=a64 $(testDisassembler_a64_input)"		\
		ANS="$(testDisassembler_a64_answer)"						\
		$< $@



###############################################################################################################################
# Standard boilerplate
###############################################################################################################################

if ROSE_BUILD_OS_IS_CYGWIN
# DQ (3/21/2009): These tests don't appear to run under Cygwin.
check-local:
	@echo "  TESTING (disabled: all tests disabled for Cygwin)"
else
# Binary analysis is enabled and supported
check-local: $(TEST_TARGETS)
endif
else
# Binary analysis is not enabled
check-local:
	@echo "  TESTING (disabled: binary frontend is not enabled)"
endif

# Don't delete things indiscriminately--sometimes developers have other related files that should not be touched!
# Rather than use one big MOSTLYCLEANFILES, split things into smaller parts so we don't get command lines that are too big.
clean-local:
	rm -f $(MOSTLYCLEANFILES)

.PHONY: $(PHONIES)
