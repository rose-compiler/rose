# This is part of the libcf library.

# This automake file handles the building of cfcheck.

# Ed Hartnett 11/23/06

# $Id: Makefile.am,v 1.3 2009/09/01 16:56:47 ed Exp $

# Find libcf.h here.
AM_CPPFLAGS = -I${top_srcdir}/src
AM_LDFLAGS = 
LDADD =

if USE_NETCDF_DIR
AM_CPPFLAGS += -I@NETCDFDIR@/include
AM_LDFLAGS += -L@NETCDFDIR@/lib
LDADD += -lnetcdf
else
if USE_NETCDF4
LDADD += ../../libsrc4/libnetcdf.la 
AM_CPPFLAGS += -I$(top_srcdir)/../libsrc4
AM_LDFLAGS += -L../../libsrc4
else
LDADD += ../../libsrc/libnetcdf.la 
AM_CPPFLAGS += -I$(top_srcdir)/../libsrc
AM_LDFLAGS += -L../../libsrc
endif # USE_NETCDF4
endif # USE_NETCDF_DIR

if USE_NETCDF4
LDADD += -lhdf5_hl -lhdf5 -lz 
endif

if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4

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

EXTRA_DIST = tst_cfcheck.sh

bin_PROGRAMS = cfcheck
cfcheck_SOURCES = cfcheck.c

TESTS = tst_cfcheck.sh

CLEANFILES = useage.txt