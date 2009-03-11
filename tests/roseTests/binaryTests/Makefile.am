include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = buffer2.bin runTest.C \
             arm-poweroff arm-ctrlaltdel arm-ctrlaltdel.ans \
	     schulz-sample1.so schulz-sample1-p.so schulz-sample2.so schulz-sample2-p.so \
	     runExecFormatsTest testShiftResizeSection.sh

if USE_BINARY_ANALYSIS_TESTS

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification


###############################################################################################################################
# The executables...
###############################################################################################################################

noinst_PROGRAMS = execFormatsTest testElfStrtab testElfByteOrder testElfWordSize testElfSchulz testElfConstruct \
	          testPeConstruct shiftResizeSection

# Parses an executable to produce a dump file (*.dump), an assembly file (rose_*.s), and a new executable created by unparsing
# the AST (*.new). The *.new file is typically identical to the original executable.
execFormatsTest_SOURCES = execFormatsTest.C
execFormatsTest_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

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
	./execFormatsTest $(srcdir)/buffer2.bin
	@echo "See file buffer2.bin.dump for information regarding the executable format"
	@echo "See file rose-buffer2.bin.s for disassembly of instructions"

demo_testElfStrtab: testElfStrtab demo_execFormatsTest
	@echo; echo; echo "Demo for $<"; echo
	./testElfStrtab $(srcdir)/arm-poweroff

demo_testElfByteOrder: testElfByteOrder demo_testElfStrtab
	@echo; echo; echo "Demo for $<"; echo
	./testElfByteOrder $(srcdir)/arm-poweroff

demo_testElfWordSize: testElfWordSize demo_testElfByteOrder
	@echo; echo; echo "Demo for $<"; echo
	./testElfWordSize $(srcdir)/arm-poweroff

demo_testElfSchulz: testElfSchulz demo_testElfWordSize
	@echo; echo; echo "Demo for $<"; echo
	./testElfSchulz $(srcdir)/schulz-sample1.so
	./testElfSchulz $(srcdir)/schulz-sample2.so

demo_testElfConstruct: testElfConstruct demo_testElfSchulz
	@echo; echo; echo "Demo for $<"; echo
	./testElfConstruct

demo_testPeConstruct: testPeConstruct demo_testElfConstruct
	@echo; echo; echo "Demo for $<"; echo
	./testPeConstruct

demo_shiftResizeSection: shiftResizeSection demo_testPeConstruct
	@echo; echo; echo "Demo for $<"; echo
	$(srcdir)/testShiftResizeSection.sh $(srcdir)/arm-ctrlaltdel >arm-ctrlaltdel.ans

###############################################################################################################################
# TESTS
#
# These are pretty much the same as the demos except they compare the new output with expected output and fail if there
# are any differences.
#
# Each passing test creates a *.passed file so that a subsequent "make" doesn't need to rerun the test.
###############################################################################################################################
TEST_PASSED_TARGETS=$(patsubst %,test_%.passed,$(noinst_PROGRAMS))

test_execFormatsTest.passed: execFormatsTest
	: $< not currently tested
	@touch $@

test_testElfStrtab.passed: testElfStrtab
	$(VALGRIND) ./testElfStrtab $(srcdir)/arm-poweroff
	@touch $@

test_testElfByteOrder.passed: testElfByteOrder
	$(VALGRIND) ./testElfByteOrder $(srcdir)/arm-poweroff
	$(VALGRIND) ./execFormatsTest ./arm-poweroff.new
	@if diff -u ./arm-poweroff.new{,.new}; then :; else echo "Files differ; byte order test failed"; exit 1; fi
	@touch $@

test_testElfWordSize.passed: testElfWordSize
	$(VALGRIND) ./testElfWordSize $(srcdir)/arm-poweroff
	$(VALGRIND) ./execFormatsTest ./arm-poweroff.new
	@if diff -u ./arm-poweroff.new{,.new}; then :; else echo "Files differ; word size test failed"; exit 1; fi
	@touch $@

test_testElfSchulz.passed: test_testElfSchulz-1.passed test_testElfSchulz-2.passed
	@touch $@

test_testElfSchulz-1.passed: testElfSchulz
	$(VALGRIND) ./testElfSchulz $(srcdir)/schulz-sample1.so
	$(VALGRIND) ./execFormatsTest ./schulz-sample1.so.new
	$(VALGRIND) ./execFormatsTest $(srcdir)/schulz-sample1-p.so
	@if diff -u schulz-sample1{-p.so,.so.new}.dump; then :; \
	    else echo "Dumps differ; Schulz test-1 failed"; exit 1; fi
	@if diff $(srcdir)/schulz-sample1-p.so schulz-sample1.so.new; then :; \
	    else echo "Libs differ; Schulz test-1 failed"; exit 1; fi
	@touch $@

test_testElfSchulz-2.passed: testElfSchulz
	$(VALGRIND) ./testElfSchulz $(srcdir)/schulz-sample2.so
	$(VALGRIND) ./execFormatsTest ./schulz-sample2.so.new
	$(VALGRIND) ./execFormatsTest $(srcdir)/schulz-sample2-p.so
	@if diff -u schulz-sample2{-p.so,.so.new}.dump; then :; \
	    else echo "Dumps differ; Schulz test-2 failed"; exit 1; fi
	@if diff $(srcdir)/schulz-sample2-p.so schulz-sample2.so.new; then :; \
	    else echo "Libs differ; Schulz test-2 failed"; exit 1; fi
	@touch $@

test_testElfConstruct.passed: testElfConstruct
	$(VALGRIND) ./testElfConstruct
	@touch $@

test_testPeConstruct.passed: testPeConstruct
	$(VALGRIND) ./testPeConstruct
	@touch $@

test_shiftResizeSection.passed: shiftResizeSection
	$(srcdir)/testShiftResizeSection.sh $(srcdir)/arm-ctrlaltdel >arm-ctrlaltdel.ans
	@if diff {$(srcdir),.}/arm-ctrlaltdel.ans; then :; else echo "Answer differs from expected"; exit 1; fi
	@touch $@

###############################################################################################################################
# Miscellaneous....
###############################################################################################################################

# Don't delete things indiscriminately--sometimes developers have other related files that should not be touched!
MOSTLYCLEANFILES = $(TEST_PASSED_TARGETS) test_testElfSchulz-{1,2}.passed \
	a.out arm-ctrlaltdel.ans arm-ctrlaltdel.dump-{1,2,3} arm-ctrlaltdel.{new,new.new} \
	arm-poweroff.{dump,new,new.dump,new.new} rose_arm-ctrlaltdel.{s,new.s} rose_arm-poweroff.{s,new.s} \
	rose_schulz-sample{1,2}-p.so.s rose_schulz-sample{1,2}.so.new.s rose_schulz-sample{1,2}.so.s \
	schulz-sample{1,2}-p.so.{dump,new} schulz-sample{1,2}.so.{dump,new,new.dump,new.new}

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

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

TEST_TRANSLATOR = ./runTest all $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
#$(TEST_Objects): runTest $(TESTCODES) 
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	env SRCDIR="$(srcdir)" $(TEST_TRANSLATOR)

CLEANFILES = unparsed.s callgraph.gml output _binary_tree.dot cfg.dot dfg.dot Templates.DB 

check-local: runTest test1 $(TEST_PASSED_TARGETS)
	./runTest $(srcdir)/buffer2.bin > output
	@echo "*********************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/binaryTests: passed ***"
	@echo "*********************************************************************"

# Don't delete things indiscriminately!
clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) ii_files ti_files DATABASE DATABASE.dot

distclean-local:
	rm -rf Templates.DB 

else

check-local:
	@echo "**********************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/binaryTests: skipped ***"
	@echo "**********************************************************************"

endif
