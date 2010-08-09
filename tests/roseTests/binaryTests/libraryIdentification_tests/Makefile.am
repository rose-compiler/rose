include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


if USE_BINARY_ANALYSIS_TESTS

# This work requires the SQLite support...
if ROSE_USE_SQLITE_DATABASE

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification

noinst_PROGRAMS = libraryIdentificationTest

libraryIdentificationTest_SOURCES = libraryIdentificationTest.C

# Conditionally link to SSL support since they disable use of Java (and so Fortran support in ROSE).
if ROSE_USE_SSL_SUPPORT
  SSL_LIBS = -lssl -lcrypto
else
  SSL_LIBS =
endif

# libraryIdentificationTest_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS) $(SSL_LIBS)
libraryIdentificationTest_LDADD = $(ROSE_LIBS_WITH_PATH)

print:
	echo "includes = $(INCLUDES)"

TESTCODES_REQUIRED_TO_PASS =

TESTCODE_CURRENTLY_FAILING = 

test_file_format_elf: libraryIdentificationTest
#	$(VALGRIND) ./libraryIdentificationTest -rose:read_executable_file_format_only $(srcdir)/i386-pivot_root
	rm -f testLibraryIdentification.db
	$(VALGRIND) ./libraryIdentificationTest $(top_srcdir)/binaries/samples/i386-pivot_root

# Test *.o files
test_object_files: libraryIdentificationTest
	mkdir -p tmp_objects; cd tmp_objects; ar -vox $(PWD)/../../../src/midend/abstractHandle/.libs/libabstractHandle.a
	$(VALGRIND) ./libraryIdentificationTest -rose:binary -rose:verbose 0 tmp_objects/roseAdapter.o

# Test a *.a file with ROSE...
test_library_archive: libraryIdentificationTest
	$(VALGRIND) ./libraryIdentificationTest -rose:verbose 0 -rose:read_executable_file_format_only $(top_builddir)/src/midend/abstractHandle/.libs/libabstractHandle.a

test_library_archive_with_instructions: libraryIdentificationTest
	$(VALGRIND) ./libraryIdentificationTest -rose:verbose 0 $(top_builddir)/src/midend/abstractHandle/.libs/libabstractHandle.a

# ****************************************************
#                     Demo Rules
# ****************************************************

demo_with_instructions_Linux:
	./libraryIdentificationTest testProgram
	cp testProgram.dot testProgram_file_format_and_instructions.dot
	zgrviewer testProgram_file_format_and_instructions.dot &

# ******************************************************
#              Standard Makefile Content
# ******************************************************

testProgram_1: $(srcdir)/testProgram.c
	gcc -g -o testProgram_1 $(srcdir)/testProgram.c

testProgram_2: $(srcdir)/testProgram.c
	gcc -o testProgram_2 $(srcdir)/testProgram.c

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

$(TEST_Objects): libraryIdentificationTest
	$(VALGRIND) ./libraryIdentificationTest $(srcdir)/$@

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/CompilerTests/libraryIdentification_tests
	cp $(srcdir)/*.c $(top_srcdir)/tests/CompilerTests/libraryIdentification_tests

check-local:
	@echo "Development tests for library identification in binaries."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) test_file_format_elf
	@echo "**************************************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/binaryTests/libraryIdentification_tests: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************************************************"
else

# Need SQLite
check-local:
	@echo "Development tests for library identification in binaries."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@echo "*********************************************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/binaryTests/libraryIdentification_tests: configure using --with-sqlite3=<path>/sqlite3-install ******"
	@echo "*********************************************************************************************************************************"
endif

else

# Using --disable-binary-analysis-tests
check-local:
	@echo "Dan Quinlan's development tests for library identification in binaries."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "**************************************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/binaryTests/libraryIdentification_tests: skipped due to --disable-binary-analysis-tests    ***"
	@echo "**************************************************************************************************************************"
endif

EXTRA_DIST = CMakeLists.txt testProgram.c

clean-local:
	rm -f *.o rose_*.[s] *.dot *.pdf *~ *.ps *.out *.new *.dump object_names.txt *.db sqlite-database-name.* *.stderr *.stdout testProgram rose_performance_report_lockfile.lock 
	rm -rf QMTest
	rm -rf tmp_objects

