
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(alt-mpi-headers)


########### next target ###############


########### next target ###############

set(testMPIAbstraction_SRCS testMPIAbstraction.C)

kde4_add_executable(testMPIAbstraction ${testMPIAbstraction_SRCS})

target_link_libraries(testMPIAbstraction ${KDE4_KDECORE_LIBS})


########### install files ###############




#original Makefile.am contents follow:

##==========================================================================
#
#SUBDIRS = alt-mpi-headers
#
#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../compassSupport \
#           $(ROSE_INCLUDES)
#
##==========================================================================
## Auxiliary library.
#
#noinst_LTLIBRARIES = libMPIAbstraction.la
#
#libMPIAbstraction_la_SOURCES = Recognizer.C Recognizer.h
#
##==========================================================================
## Unit test
#
#noinst_PROGRAMS = testMPIAbstraction
#
#LDADD = libMPIAbstraction.la ../C-API/libC-API.la \
#  $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#testMPIAbstraction_SOURCES = testMPIAbstraction.C
#
##==========================================================================
#
#TEST_RUN_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:C_only \
#                 -I. -Ialt-mpi-headers/include -I$(srcdir)/testCodes \
#                 --mpi-api $(srcdir)/MPI.api
#
#test: libMPIAbstraction.la testMPIAbstraction$(EXEEXT) \
#	    $(srcdir)/testCodes/is.c \
#	    $(srcdir)/testCodes/npbparams.h \
#	    $(srcdir)/MPI.api
#	@echo "============================================================"
#	@echo "Trying to recognize MPI abstractions in 'is.c'..."
#	@echo "============================================================"
#	./testMPIAbstraction$(EXEEXT) $(TEST_RUN_FLAGS) \
#	  -c $(srcdir)/testCodes/is.c
#	@echo "============================================================"
#	@echo "Compare to a grep-based search (functions only)..."
#	@echo "============================================================"
#	@grep -E --line-number 'MPI_[^ ]*[ ]*\(' $(srcdir)/testCodes/is.c
#
#check-local:
#	@$(MAKE) test
#	@echo "*********************************************************************************************************************"
#	@echo "*** ROSE/projects/compass/util/C-API: make check rule complete (terminated normally) ***"
#	@echo "*********************************************************************************************************************"
#
#EXTRA_DIST = MPI.api MPI-api.c testCodes
#
#clean-local:
#	rm -f *.ti a.out rose_*.c
#
## eof
