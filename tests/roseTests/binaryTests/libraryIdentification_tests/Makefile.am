include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations
noinst_PROGRAMS=
TEST_TARGETS=
EXTRA_DIST=CMakeLists.txt testProgram.c
MOSTLYCLEANFILES=


if USE_BINARY_ANALYSIS_TESTS

# This work requires the SQLite support...
if ROSE_USE_SQLITE_DATABASE

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification

noinst_PROGRAMS += libraryIdentificationTest
libraryIdentificationTest_SOURCES = libraryIdentificationTest.C
libraryIdentificationTest_LDADD = $(ROSE_LIBS_WITH_PATH)

# Tests
TEST_TARGETS += libraryIdentificationTest_1.passed \
                libraryIdentificationTest_2.passed \
                libraryIdentificationTest_3.passed \
                libraryIdentificationTest_4.passed
EXTRA_DIST += libraryIdentificationTest.conf libraryIdentificationTest_obj.conf

libraryIdentificationTest_1.passed: libraryIdentificationTest.conf libraryIdentificationTest
	@$(TH) ARGS= INPUT=$(top_srcdir)/binaries/samples/i386-pivot_root $< $@
libraryIdentificationTest_2.passed: libraryIdentificationTest_obj.conf libraryIdentificationTest
	@$(TH) ARCHIVE=$(top_builddir)/src/midend/abstractHandle/.libs/libabstractHandle.a $< $@
libraryIdentificationTest_3.passed: libraryIdentificationTest.conf libraryIdentificationTest
	@$(TH) ARGS='-rose:verbose 0 -rose:read_executable_file_format_only' \
		INPUT=$(top_builddir)/src/midend/abstractHandle/.libs/libabstractHandle.a $< $@
libraryIdentificationTest_4.passed: libraryIdentificationTest.conf libraryIdentificationTest
	@$(TH) ARGS='-rose:verbose 0' \
		INPUT=$(top_builddir)/src/midend/abstractHandle/.libs/libabstractHandle.a $< $@

# Conditionally link to SSL support since they disable use of Java (and so Fortran support in ROSE).
if ROSE_USE_SSL_SUPPORT
  SSL_LIBS = -lssl -lcrypto
else
  SSL_LIBS =
endif

MOSTLYCLEANFILES += \
	$(TEST_TARGETS) $(patsubst %.passed, %.failed, $(TEST_TARGETS)) \
	*.dump *.new *.dot rose_*.s \
	object_names.txt testLibraryIdentification.db

check-local: $(TEST_TARGETS)



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


else
check-local:
	@echo "  TESTING (disabled: all tests disabled here; no SQLite)"
endif

else

# Using --disable-binary-analysis-tests
check-local:
endif


