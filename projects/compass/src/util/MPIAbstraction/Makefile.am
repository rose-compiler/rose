#==========================================================================

SUBDIRS = alt-mpi-headers

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../compassSupport \
           $(ROSE_INCLUDES)

AM_LDFLAGS = $(ROSE_RPATHS)

#==========================================================================
# Auxiliary library.

noinst_LTLIBRARIES = libMPIAbstraction.la

libMPIAbstraction_la_SOURCES = Recognizer.C Recognizer.h

#==========================================================================
# Unit test

noinst_PROGRAMS = testMPIAbstraction

# CH (2/22/2010): put $(ROSE_LIBS_WITH_PATH) ahead to avoid the linker to find boost libraries in usr/lib first
# this problem still need to be solved in the future
LDADD = $(ROSE_LIBS_WITH_PATH) \
    libMPIAbstraction.la ../C-API/libC-API.la 

testMPIAbstraction_SOURCES = testMPIAbstraction.C

#==========================================================================

TEST_RUN_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:C_only \
                 -I. -Ialt-mpi-headers/include -I$(srcdir)/testCodes \
                 --mpi-api $(srcdir)/MPI.api

# DQ (3/1/2017): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

test: libMPIAbstraction.la testMPIAbstraction$(EXEEXT) \
	    $(srcdir)/testCodes/is.c \
	    $(srcdir)/testCodes/npbparams.h \
	    $(srcdir)/MPI.api
	@echo "============================================================"
	@echo "Trying to recognize MPI abstractions in 'is.c'..."
	@echo "============================================================"
	$(VALGRIND) ./testMPIAbstraction$(EXEEXT) $(TEST_RUN_FLAGS) -c $(srcdir)/testCodes/is.c
	@echo "============================================================"
	@echo "Compare to a grep-based search (functions only)..."
	@echo "============================================================"
	@grep -E --line-number 'MPI_[^ ]*[ ]*\(' $(srcdir)/testCodes/is.c

check-local:
	echo "Running tests in ROSE/projects/compass/util/MPIAbstraction..."
	@$(MAKE) test
	@echo "*************************************************************************************************"
	@echo "*** ROSE/projects/compass/util/MPIAbstraction: make check rule complete (terminated normally) ***"
	@echo "*************************************************************************************************"


EXTRA_DIST = CMakeLists.txt MPI.api MPI-api.c testCodes

clean-local:
	rm -f *.ti a.out rose_*.c

# eof
