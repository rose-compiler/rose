#==========================================================================

SUBDIRS = alt-mpi-headers

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../compassSupport \
           $(ROSE_INCLUDES)

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
#LDADD = libMPIAbstraction.la ../C-API/libC-API.la \
#  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

testMPIAbstraction_SOURCES = testMPIAbstraction.C

#==========================================================================

TEST_RUN_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:C_only \
                 -I. -Ialt-mpi-headers/include -I$(srcdir)/testCodes \
                 --mpi-api $(srcdir)/MPI.api

test: libMPIAbstraction.la testMPIAbstraction$(EXEEXT) \
	    $(srcdir)/testCodes/is.c \
	    $(srcdir)/testCodes/npbparams.h \
	    $(srcdir)/MPI.api
	@echo "============================================================"
	@echo "Trying to recognize MPI abstractions in 'is.c'..."
	@echo "============================================================"
	./testMPIAbstraction$(EXEEXT) $(TEST_RUN_FLAGS) -c $(srcdir)/testCodes/is.c
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
