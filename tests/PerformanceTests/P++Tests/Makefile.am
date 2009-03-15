## Automake options
AUTOMAKE_OPTIONS = foreign no-dependencies
# srcdir           = @srcdir@
# VPATH            = @srcdir@ 

#
# common definitions stored in one location
# include ../../config/Makefile.setup
# include ../../config/Makefile.config.in

#
#location of BenchmarkBase.h and libBenchmarkBase.a
BASE_DIR= $(top_srcdir)/TESTS/PerformanceTests/BenchmarkBase/Base

#
# override libs to get rid of MPI libraries
#
# LIBS = -lc

# AUTOMAKE_OPTIONS = foreign no-dependencies
# ACLOCAL = echo

# EXTRA_DIST=Makefile.user

## Test of overriding libdir and bin dir so that 'make install' puts stuff
## in correct directory
# libdir = .
# bindir = .

#
# set the template repository location to here
#
# CXX_TEMPLATE_REPOSITORY_PATH=.

# These test require a working version of the A++/P++ Array Class library.
# This path must have been specified on the command line to the configure command
# For example: configure --with-AxxPxx=/home/dquinlan/ROSE/A++P++Install
# if (ROSE_PERFORMANCE_TESTS && AXXPXX_SPECIFIC_TESTS)
if ROSE_PERFORMANCE_TESTS
if AXXPXX_SPECIFIC_TESTS
noinst_PROGRAMS = performanceTest

performanceTest_SOURCES = \
	performanceTest.C \
	test_01.C
endif
endif

noinst_HEADERS= \
	test_01.h

AM_CXXFLAGS= -I. -I../BenchmarkBase -I$(BASE_DIR)

LDFLAGS=-L./../BenchmarkBase/Base -L./../src

# DQ (3/14/2009): I think it is OK to overwrite the LDFLAGS value, but let's not.
LDADD += -lBenchmarkBase -lPpp -lPpp_static -lm

## This would be defined to be the value of the _LDADD suffix variables and we want
## to avoid that dependency (since it gets in the way of tests using "make -n")
DEPENDENCIES =

#######################################################
## additional clean rule for removing Templates.DB in
## all directories.  Note that in the EXAMPLES directories
## the $CXX_TEMPLATE_REPOSITORY_PATH is redefined so that
## only the local repository is removed!
#######################################################
clean-local:
	rm -rf rose_*.C
