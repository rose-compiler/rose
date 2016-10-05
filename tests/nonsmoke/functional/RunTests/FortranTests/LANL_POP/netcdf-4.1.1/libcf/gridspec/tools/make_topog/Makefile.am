## This is an automake file, part of Unidata's NetCDF CF Library
## package. Copyright 2009, see the COPYRIGHT file for more
## information.

# This automake file is in charge of building the gridspec
# make_topog tool.

# $Id: Makefile.am,v 1.6 2009/09/01 16:56:46 ed Exp $

bin_PROGRAMS = make_topog gs_make_topog
make_topog_SOURCES = make_topog.c  topog.c  topog.h
gs_make_topog_SOURCES = make_topog.c  topog.c  topog.h

# Initalize these.
AM_CPPFLAGS = -I../shared -I../../shared/mosaic
AM_LDFLAGS = -L../shared -L../../shared/mosaic
LDADD = -ltoolsshared -lshared -lnetcdf -lm

if USE_NETCDF_DIR
AM_CPPFLAGS += -I@NETCDFDIR@/include
AM_LDFLAGS += -L@NETCDFDIR@/lib
LDADD += -lnetcdf
else
if USE_NETCDF4
LDADD += ../../../../libsrc4/libnetcdf.la 
AM_CPPFLAGS += -I$(top_srcdir)/../libsrc4
AM_LDFLAGS += -L../../../../libsrc4
else
LDADD += ../../../../libsrc/libnetcdf.la 
AM_CPPFLAGS += -I$(top_srcdir)/../libsrc
AM_LDFLAGS += -L../../../../libsrc
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

TESTS = run_tests
EXTRA_DIST = run_tests.in
CLEANFILES = topog.nc gs_topog.nc topog.cdl gs_topog.cdl
DISTCLEANFILES = run_tests