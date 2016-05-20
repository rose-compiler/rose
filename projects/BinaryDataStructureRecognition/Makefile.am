include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AM_CXXFLAGS = $(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS) ${SQLITE3_CFLAGS}

# AM_LDFLAGS  = $(ROSE_LDFLAGS) $(ROSE_LIB) $(BOOST_LDFLAGS)   $(BOOST_PROGRAM_OPTIONS_LIB) \
#              $(BOOST_REGEX_LIB) $(BOOST_UNIT_TEST_FRAMEWORK_LIB) $(BOOST_FILESYSTEM_LIB) $(SQLITE3_LDFLAGS)

# AM_LDFLAGS += -Wl,-rpath -Wl,@prefix@/lib @prefix@/lib/librose.so

# AM_CPPFLAGS = -I@prefix@/include $(ROSE_INCLUDES)
AM_CPPFLAGS = $(ROSE_INCLUDES) 

noinst_PROGRAMS = astStructRecognition

# shortestPathBoostExample

astStructRecognition_SOURCES = \
   astStructRecognition.C structureRecognitionSupport.C variableInitialization.C \
   virtualTableLoad.C sectionBoundaryTraversal.C generateSourceCodeForDataStructures.C \
   functionAnalysis.C scopeAnalysis.C

astStructRecognition_LDADD   = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

TESTCODES_REQUIRED_TO_PASS = testProgram_1.C testProgram_2.C

TESTCODE_CURRENTLY_FAILING = 

include_HEADERS = \
   sectionBoundaryTraversal.h structureRecognitionSupport.h variableInitialization.h \
   virtualTableLoad.h generateSourceCodeForDataStructures.h astStructRecognition.h \
   functionAnalysis.h binaryAnalysisAttributes.h scopeAnalysis.h

# ******************************************************
#              Standard Makefile Content
# ******************************************************

#testProgram_1: $(srcdir)/testProgram_1.C
#	g++ -g -o testProgram_1 $(srcdir)/testProgram_1.C

testProgram32_1: $(srcdir)/testProgram_1.C
	g++ -m32 -o testProgram32_1 $(srcdir)/testProgram_1.C

# Test of astEquivalence on Binary Executable
readBinaryTestProgram: astStructRecognition testProgram_1
#	./astStructRecognition -rose:read_executable_file_format_only testProgram_1 testProgram_2
	./astStructRecognition -rose:skipfinalCompileStep testProgram_1

# Skip the link step using the backend compiler on the generated code since it will not have a "main()" function.
readBinaryTestProgram32: astStructRecognition testProgram32_1
	./astStructRecognition -rose:verbose 0 -c -rose:read_instructions_only testProgram32_1

test: readBinaryTestProgram32

# This demo generates the edit graph for two similar source code files
# and displays the generated visualization of the graph using zgrviewer.
demoDetectStructures: astStructRecognition
	cp output.dot outputSource.dot
	zgrviewer outputSource.dot &

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

ROSE_FLAGS = -rose:verbose 2 $(PERFORMANCE_REPORT_OPTION)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# PASSING_TEST_Objects = ${TESTCODES}
# TEST_Objects = ${ALL_TESTCODES}
PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

$(TEST_Objects): astStructRecognition
	g++ -m32 -o $(@:.o=) $(srcdir)/$(@:.o=.C)
	$(VALGRIND) ./astStructRecognition -rose:verbose 0 -c -rose:read_instructions_only $(@:.o=)

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/CompilerTests/binaryDataStructureRecognition_tests
	cp $(srcdir)/*.c $(top_srcdir)/tests/CompilerTests/binaryDataStructureRecognition_tests

check-local:
	@echo "Dan Quinlan's development tests for Binary Data-Structure Recognition."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
if OS_64BIT
	@$(MAKE) $(PASSING_TEST_Objects)
else
	@echo "****** ROSE/projects/BinaryDataStructureRecognition: Skipping these tests for 32-bit systems ..."
endif
	@echo "*****************************************************************************************************************"
	@echo "****** ROSE/projects/BinaryDataStructureRecognition: make check rule complete (terminated normally)        ******"
	@echo "*****************************************************************************************************************"

EXTRA_DIST =

clean-local:
	rm -f *.o rose_*.[s] rose_*.? *.dot *.png *.pdf *~ *.ps *.out *.new *.dump sqlite-database-name.* *.stderr *.stdout *.exe testProgram rose_performance_report_lockfile.lock
	rm -f testProgram_1 testProgram32_1 ${ALL_TESTCODES:.C=} class_design_*.C
	rm -rf QMTest

