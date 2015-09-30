#==========================================================================

SUBDIRS = mpich-1.2.7p1

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MPI_ROOT_DIR = $(srcdir)/mpich-1.2.7p1
MPI_INC_DIR = $(MPI_ROOT_DIR)/include
MPI_HDRS = \
	$(MPI_INC_DIR)/mpi.h \
	$(MPI_INC_DIR)/mpidefs.h \
	$(MPI_INC_DIR)/mpi_fortdefs.h \
	$(MPI_INC_DIR)/mpio.h \
	$(MPI_INC_DIR)/mpi_errno.h

# ALTMPI_OUT_DIR = ./include
# ALTMPI_LIB_OUT_DIR = ./lib
ALTMPI_OUT_DIR = include
ALTMPI_LIB_OUT_DIR = lib

# The distribution builds the $(ALTMPI_OUT_DIR) and the $(ALTMPI_LIB_OUT_DIR)
# into the source tree (different from the development bversion of ROSE, so 
# include the directory for that location as well.
AM_CPPFLAGS = -I. -I$(srcdir) -I$(ALTMPI_OUT_DIR) -I$(srcdir)/$(ALTMPI_OUT_DIR)

#==========================================================================

ALTMPI_HDRS_MAIN = $(ALTMPI_OUT_DIR)/mpi.h
ALTMPI_HDRS_OTHERS = \
	$(ALTMPI_OUT_DIR)/mpidefs.h \
	$(ALTMPI_OUT_DIR)/mpi_fortdefs.h \
	$(ALTMPI_OUT_DIR)/mpio.h \
	$(ALTMPI_OUT_DIR)/mpi_errno.h
ALTMPI_HDRS = $(ALTMPI_HDRS_MAIN) $(ALTMPI_HDRS_OTHERS)
ALTMPI_SRCS = $(ALTMPI_LIB_OUT_DIR)/def.c

noinst_LTLIBRARIES = libmpi-alt-defs.la
# libmpi_alt_defs_la_SOURCES = $(ALTMPI_SRCS) $(ALTMPI_HDRS)
libmpi_alt_defs_la_SOURCES = $(ALTMPI_SRCS)

#==========================================================================

$(ALTMPI_HDRS_OTHERS) $(ALTMPI_SRCS): $(ALTMPI_HDRS_MAIN)

$(ALTMPI_HDRS_MAIN): $(MPI_HDRS) $(srcdir)/gen-headers.py
	python $(srcdir)/gen-headers.py \
	  --out-incs=$(ALTMPI_OUT_DIR) \
	  --out-srcs=$(ALTMPI_LIB_OUT_DIR) \
	  $(MPI_ROOT_DIR)

#==========================================================================

EXTRA_DIST = CMakeLists.txt gen-headers.py

clean-local:
	rm -f *.ti a.out rose_*.c
	rm -rf $(ALTMPI_OUT_DIR) $(ALTMPI_LIB_OUT_DIR)

# eof
