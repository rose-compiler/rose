include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations
noinst_PROGRAMS=
TEST_TARGETS=
EXTRA_DIST=CMakeLists.txt testProgram.c
MOSTLYCLEANFILES=


if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

# This work requires the SQLite support...
if ROSE_USE_SQLITE_DATABASE

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/BinaryAnalysis/libraryIdentification
AM_LDFLAGS = $(ROSE_RPATHS)

noinst_PROGRAMS += libraryIdentificationTest
libraryIdentificationTest_SOURCES = libraryIdentificationTest.C
libraryIdentificationTest_LDADD = $(ROSE_LIBS_WITH_PATH)

##############################
# Tests
##############################
EXTRA_DIST += libraryIdentificationTest.conf libraryIdentificationTest_obj.conf

TEST_TARGETS += libraryIdentificationTest_1.passed
libraryIdentificationTest_1.passed: libraryIdentificationTest.conf libraryIdentificationTest
	@$(RTH_RUN) ARGS= INPUT=$(top_srcdir)/tests/nonsmoke/specimens/binary/i386-pivot_root $< $@


if ROSE_BUILD_OS_IS_OSX
# These tests are disabled on Mac OS X because that system's "ar" command cannot unpack
# an archive file generated on a Linux machine (such as libTest-amd64.a).  And we cannot
# use a OS X archive containing MachO files because those object files cannot be parsed
# (yet) by ROSE.
else
TEST_TARGETS += libraryIdentificationTest_2.passed
libraryIdentificationTest_2.passed: libraryIdentificationTest_obj.conf libraryIdentificationTest
	@$(RTH_RUN) ARCHIVE=$(top_srcdir)/tests/nonsmoke/specimens/binary/libTest-amd64.a $< $@
endif

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


endif
endif

check-local: $(TEST_TARGETS)
