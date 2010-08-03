include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_DWARF

INCLUDES = $(ROSE_INCLUDES) 

noinst_PROGRAMS = dwarfReader roseDwarfReader 

dwarfReader_SOURCES     = dwarfReader.C 
dwarfReader_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

roseDwarfReader_SOURCES = roseDwarfReader.C
roseDwarfReader_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

print:
	echo "includes = $(INCLUDES)"

# Collect using: "size /sbin/* | sort -rn"
TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = 

# Use "-g3" to get macro information into Dwarf (default is "-g2").
TEST_FLAGS = -g3 -ggdb
#BINARY_ANALYSIS_FLAGS = -rose:read_executable_file_format_only
testProgram: dwarfReader $(srcdir)/testProgram.C
	g++ $(TEST_FLAGS) -o testProgram $(srcdir)/testProgram.C

test_dwarf: dwarfReader testProgram
	$(VALGRIND) ./dwarfReader -rose:read_executable_file_format_only testProgram

test_rose_dwarf: roseDwarfReader testProgram
	$(VALGRIND) ./roseDwarfReader -rose:read_executable_file_format_only testProgram

test_dwarf_with_instructions: dwarfReader testProgram
	$(VALGRIND) ./dwarfReader testProgram

test_rose_dwarf_large: roseDwarfReader
	$(VALGRIND) ./roseDwarfReader -rose:read_executable_file_format_only $(srcdir)/../Disassembler_tests/raptor2d.BGL.CC.MPI_with_dwarf.ex 

# This is a test of the reader where these is no dwarf information in the binary executable (negative test)
test_no_dwarf_info: dwarfReader testProgram
	$(VALGRIND) ./dwarfReader -rose:read_executable_file_format_only $(srcdir)/../Disassembler_tests/i386-pivot_root

test_rose_dwarf_MachO: roseDwarfReader
	$(VALGRIND) ./roseDwarfReader -rose:read_executable_file_format_only $(srcdir)/../Disassembler_tests/inputCode_dwarfAnalysis_Mach-O


# ******************************************************
#              Standard Makefile Content
# ******************************************************

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/DISASSEMBLER_ROSE_PERFORMANCE_DATA.csv

ROSE_FLAGS = -rose:binary -rose:verbose 0 $(PERFORMANCE_REPORT_OPTION)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES}
TEST_Objects = ${ALL_TESTCODES}

# $(TEST_Objects): preprocessor $(srcdir)/$(@:.o=.C)
#	$(VALGRIND) ./binaryReader $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.c)

$(TEST_Objects): binaryReader
	$(VALGRIND) ./binaryReader $(srcdir)/$@

check-local:
	@echo "Development tests for Dwarf support."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) test_dwarf
	@$(MAKE) test_rose_dwarf
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/developersScratchSpace/Dan/Dwarf_tests: make check rule complete (terminated normally)        ******"
	@echo "***********************************************************************************************************************"
else
check-local:
	@echo "Development tests for Binary Disassembly."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/developersScratchSpace/Dan/Dwarf_tests: skipped due to --disable-binary-analysis-tests    ***"
	@echo "***********************************************************************************************************************"
endif

EXTRA_DIST = CMakeLists.txt testProgram.C

clean-local:
	rm -f *.o rose_*.[s] *.dot *.pdf *~ *.ps *.out *.new *.dump sqlite-database-name.* *.stderr *.stdout testProgram rose_performance_report_lockfile.lock
	rm -rf QMTest

