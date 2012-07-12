## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This file builds and runs the nc_test program, which tests the
# netCDF-3 API for all formats.

# $Id: Makefile.am,v 1.32 2009/12/05 20:46:29 dmh Exp $

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/ncdump
LDADD =

# Link to the netCDF-4 and the HDF5 libraries.

if BUILD_DAP

if USE_NETCDF4

LDADD += ../libsrc4/libnetcdf.la

if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4

LDADD += -lhdf5_hl -lhdf5  -lz

if USE_PNETCDF
LDADD += -lpnetcdf
endif

# Point pre-processor and linker to netcdf-4 directory (libsrc4).
AM_CPPFLAGS += -I$(top_srcdir)/libsrc4
AM_LDFLAGS = -L../libsrc4

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_CPPFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a prefix for the HDF4 library location, use it.
if USE_HDF4_DIR
AM_CPPFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
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

if USE_PNETCDF_DIR
AM_CPPFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

else # ! USE_NETCDF4

AM_CPPFLAGS += -I$(top_srcdir)/libsrc

LDADD += ${top_builddir}/libsrc/libnetcdf.la

endif # USE_NETCDF4

endif # BUILD_DAP

# Set up the tests; do the .sh first, then .c
check_PROGRAMS = 
TESTS = 
TESTS += tst_ncdap3.sh

if USE_NETCDF4
TESTS += tst_ncdap4.sh
endif # USE_NETCDF4

# remote tests are optional
# because the server may be down
if ENABLE_DAP_REMOTE_TESTS
TESTS += tst_remote3.sh
if ENABLE_DAP_LONG_TESTS
TESTS += tst_longremote3.sh
endif

if USE_NETCDF4
TESTS += tst_remote4.sh
if ENABLE_DAP_LONG_TESTS
TESTS += tst_longremote4.sh
endif
endif

endif #REMOTE_TESTS

# Do the programmatic tests last; all are part of the remote test set
if ENABLE_DAP_REMOTE_TESTS

# Conversion tests
test_cvt3_SOURCES = test_cvt.c
test_cvt4_SOURCES = test_cvt.c
test_varm3_SOURCES = test_varm3.c
check_PROGRAMS += test_cvt3 test_varm3 test_cvt4

TESTS += test_cvt3 test_varm3

if USE_NETCDF4
TESTS += test_cvt4
endif

endif ENABLE_DAP_REMOTE_TESTS

# Need to add subdirs
SUBDIRS = testdata3 expected3 expected4 expectremote3 expectremote4

EXTRA_DIST = tst_ncdap3.sh tst_ncdap4.sh \
             tst_remote3.sh tst_remote4.sh \
             tst_longremote3.sh tst_longremote4.sh \
             tst_ncdap.sh tst_ncdap_shared.sh tst_remote.sh

CLEANFILES = test_varm3 test_cvt3 results/*.dmp results/*.das results/*.dds datadds*

if USE_NETCDF4
CLEANFILES += test_cvt4
endif

test: check
