## This is a automake file, part of Unidata's netCDF package.
# Copyright 2007, see the COPYRIGHT file for more information.

# This file builds the new C++-4 interface.

# $Id: Makefile.am,v 1.16 2010/03/30 14:18:26 ed Exp $

# Point pre-preprocessor to netcdf-4 directory (libsrc4).
AM_CPPFLAGS = -I$(top_srcdir)/libsrc4 
AM_LDFLAGS =

# Link to the netCDF-4 and the HDF5 libraries, and the C++ library.
LDADD = $(top_builddir)/cxx4/libnetcdf_c++4.la $(top_builddir)/libsrc4/libnetcdf.la -lhdf5_hl -lhdf5

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_CPPFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root location for ZLIB, use it.
if USE_ZLIB_DIR
AM_CPPFLAGS += -I@ZLIBDIR@/include
AM_LDFLAGS += -L@ZLIBDIR@/lib
endif

# If the user specified a root location for SZLIB, use it.
if USE_SZLIB_DIR
AM_CPPFLAGS += -I@SZLIBDIR@/include
AM_LDFLAGS += -L@SZLIBDIR@/lib
endif

#SUBDIRS = d

# See comments in libsrc/Makefile.am about the version number.
libnetcdf_c__4_la_LDFLAGS = $(AM_LDFLAGS) -version-info 1:0:0 

# These headers will be installed in the users header directory.
include_HEADERS = netcdf \
                  ncAtt.h   ncCheck.h	    ncDim.h	  ncException.h \
                  ncGroup.h     ncOpaqueType.h  ncVar.h	   ncVlenType.h \
                  ncCompoundType.h  ncEnumType.h  ncFile.h	\
                  ncGroupAtt.h  ncType.h        ncVarAtt.h \
                  ncByte.h ncChar.h ncShort.h ncInt.h ncFloat.h ncDouble.h \
                  ncUbyte.h ncUshort.h ncUint.h ncInt64.h ncUint64.h ncString.h

# This is our output library. 
lib_LTLIBRARIES = libnetcdf_c++4.la

# Library source.
libnetcdf_c__4_la_SOURCES = \
ncAtt.cpp ncCheck.cpp ncCompoundType.cpp ncDim.cpp ncEnumType.cpp \
ncException.cpp ncFile.cpp ncGroup.cpp ncGroupAtt.cpp ncOpaqueType.cpp \
ncType.cpp ncVar.cpp ncVarAtt.cpp  ncVlenType.cpp \
ncByte.cpp ncChar.cpp ncShort.cpp ncInt.cpp ncFloat.cpp ncDouble.cpp \
ncUbyte.cpp ncUshort.cpp ncUint.cpp ncInt64.cpp ncUint64.cpp ncString.cpp


TESTFILES = test_group$(EXEEXT) test_dim$(EXEEXT) test_att$(EXEEXT) test_var$(EXEEXT) test_var2$(EXEEXT) test_type$(EXEEXT)
test_group_SOURCES = test_group.cpp
test_dim_SOURCES = test_dim.cpp
test_att_SOURCES = test_att.cpp test_utilities.h
test_var_SOURCES = test_var.cpp test_utilities.h
test_var2_SOURCES = test_var2.cpp test_utilities.h
test_type_SOURCES = test_type.cpp test_utilities.h

# Build and run these tests.
check_PROGRAMS = $(TESTFILES)
TESTS = $(TESTFILES)

CLEANFILES=firstFile.cdf

# Empty file that gets time-stamped after docs are generated
doxygen_stamp: $(libnetcdf_c__4_la_SOURCES) $(include_HEADERS)
	doxygen
	touch doxygen_stamp


