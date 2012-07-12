## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.
# This file builds the C++ interface.

# $Id: Makefile.am,v 1.49 2009/12/08 20:58:10 dmh Exp $

# Here's what the hand-crafted Makefile did to build cxx:
# c++ -c -g -O2 -I../libsrc -I.   -Df2cFortran netcdf.cpp
# c++ -c -g -O2 -I../libsrc -I.   -Df2cFortran ncvalues.cpp
# ar cru libnetcdf_c++.a netcdf.o ncvalues.o
# ranlib libnetcdf_c++.a
# c++ -c -g -O2 -I../libsrc -I.   -Df2cFortran nctst.cpp
# c++ -o nctst -g -O2  nctst.o libnetcdf_c++.a -L../libsrc -lnetcdf 

AM_CPPFLAGS =
AM_LDFLAGS =

CLEANFILES = nctst_classic.nc nctst_64bit_offset.nc nctst_netcdf4.nc	\
nctst_netcdf4_classic.nc nctst_classic.cdl nctst_64bit_offset.cdl	\
tst_failure.nc

EXTRA_DIST = ref_nctst.cdl ref_nctst_64bit_offset.cdl			\
ref_nctst_netcdf4.cdl ref_nctst_netcdf4_classic.cdl run_nc_tests.sh	\
run_nc4_tests.sh

# nctst creates a simple data file in each format, then uses ncdump to
# create CDL files, which are compared with some reference files
# shipped with the code. tst_failure is a program that is intended to
# fail.
TESTFILES = nctst$(EXEEXT) tst_failure$(EXEEXT)
nctst_SOURCES = nctst.cpp
tst_failure_SOURCES = tst_failure.cpp
XFAIL_TESTS = tst_failure$(EXEEXT)

# Build and run these tests.
check_PROGRAMS = $(TESTFILES)
TESTS = $(TESTFILES)

# If the utilities have been built, run these extra tests.
if BUILD_UTILITIES
TESTS += run_nc_tests.sh
endif

# If we're building netCDF-4, we need to include netcdf-4 headers, and
# also link to hdf5 libraries.
if USE_NETCDF4

# Point pre-preprocessor to netcdf-4 directory (libsrc4).
AM_CPPFLAGS += -I$(top_srcdir)/libsrc4 

# The C++ library depends on the C library.
libnetcdf_c___la_LIBADD = $(top_builddir)/libsrc4/libnetcdf.la

# Link to the netCDF-4, (maybe) the hdf4, the HDF5, and the C++ libraries.
LDADD = $(top_builddir)/cxx/libnetcdf_c++.la $(top_builddir)/libsrc4/libnetcdf.la
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

# If the user specified a root location for HDF4, use it.
if USE_HDF4_DIR
AM_CPPFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
endif

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

# If the user specified a root location for PNETCDF, use it.
if USE_PNETCDF_DIR
AM_CPPFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

# NetCDF_4 testing generates some extra cleaning work (so do my kids!)
CLEANFILES += nctst_netcdf4.cdl nctst_netcdf4_classic.cdl

# Run extra netcdf-4 tests, but only if utilities are built.
if BUILD_UTILITIES
TESTS += run_nc4_tests.sh
endif

# See comments in libsrc/Makefile.am about the version number.
libnetcdf_c___la_LDFLAGS = $(AM_LDFLAGS) -version-info 5:0:0 

else # not USE_NETCDF4

# Point pre-processor to netcdf-3 directory (libsrc).
AM_CPPFLAGS += -I$(top_srcdir)/libsrc

LDADD = $(top_builddir)/cxx/libnetcdf_c++.la 

# Link to the netCDF-3 library and the C++ library.
LDADD += $(top_builddir)/libsrc/libnetcdf.la 
# The C++ library depends on the C library.
libnetcdf_c___la_LIBADD = $(top_builddir)/libsrc/libnetcdf.la

# See comments in libsrc/Makefile.am about the version number.
libnetcdf_c___la_LDFLAGS = $(AM_LDFLAGS) -version-info 4:0:0 

endif # USE_NETCDF4

if BUILD_DAP
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

# These headers will be installed in the users header directory.
include_HEADERS = netcdfcpp.h ncvalues.h netcdf.hh

# This is our output library. 
lib_LTLIBRARIES = libnetcdf_c++.la

# Library source.
libnetcdf_c___la_SOURCES = netcdf.cpp ncvalues.cpp

test: check

