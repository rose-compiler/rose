include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/specimens/binary/SpecimenSets.inc

SUBDIRS =

EXTRA_DIST =

# Forward declarations
TEST_TARGETS=
noinst_PROGRAMS=
MOSTLYCLEANFILES=
PHONIES=

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

# Location of binary specimens
#~ SPECIMEN_DIR = $(nonsmoke_specimens_binary_directory)

# Corresponding smoke directory
#~ SMOKE_BLDDIR = $(abspath $(top_builddir))/tests/smoke/functional/BinaryAnalysis
#~ SMOKE_SRCDIR = $(abspath $(top_srcdir))/tests/smoke/functional/BinaryAnalysis

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES) #-I$(top_srcdir)/src/midend/BinaryAnalysis/libraryIdentification
AM_LDFLAGS = $(ROSE_RPATHS)

#~ TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
#~ TEST_WITH_ANSWER = $(top_srcdir)/scripts/test_with_answer

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support
# this, this implementation needs review).  It might be better to prepend the information to the
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif


###############################################################################################################################
# Check parsing of symbolic expressions via Rose::BinaryAnalysis::SymbolicExprParser
###############################################################################################################################

#~ noinst_PROGRAMS += sampleExecutable
#~ testBinaryConcolic_SOURCES = sampleExecutable.C
#~ testBinaryConcolic_LDADD =

AM_YFLAGS = -d -t

noinst_PROGRAMS += crsh
crsh_SOURCES = crsh-parse.ypp crsh-lex.lpp crsh.cpp crsh.hpp ../configDB.h
crsh_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS +=  # testConcolicDB.passed

#~ y.txt: # sample text file
#~ 	echo "HelloWorld" >y.txt

#~ testConcolicDB.passed: $(top_srcdir)/tests/nonsmoke/functional/BinaryAnalysis/Concolic/concolicDB.conf testConcolicDB y.txt # sampleExecutable
#~ 	@$(RTH_RUN) \
#~ 		TITLE="t[$@]" \
#~ 		CMD="./testConcolicDB" \
#~ 		ANS="no" \
#~ 		$< $@

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
