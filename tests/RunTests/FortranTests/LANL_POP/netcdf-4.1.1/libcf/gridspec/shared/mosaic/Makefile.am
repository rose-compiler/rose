## This is an automake file, part of Unidata's NetCDF CF Library
## package. Copyright 2009, see the COPYRIGHT file for more
## information.

# This automake file is in charge of building the gridspec
# tools/shared convenience library.

# $Id: Makefile.am,v 1.3 2009/07/10 12:51:13 ed Exp $

noinst_LTLIBRARIES = libshared.la 
libshared_la_SOURCES = constant.h create_xgrid.c create_xgrid.h	\
gradient_c2l.c gradient_c2l.h interp.c interp.h mosaic_util.c	\
mosaic_util.h read_mosaic.c read_mosaic.h

AM_CPPFLAGS = 
AM_LDFLAGS =
LDADD = -lm

# Does the user want to build netCDF-4?
if USE_NETCDF4

# Link to the netCDF-4, (optionally) HDF4, and the HDF5 libraries.
LDADD += ../../../../libsrc4/libnetcdf.la 
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

# Point pre-processor and linker to netcdf-4 directory (libsrc4).
AM_CPPFLAGS += -I$(top_srcdir)/../libsrc4
AM_LDFLAGS += -L../../../../libsrc4

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

else # not USE_NETCDF4

# Link to the netCDF-3 library.
LDADD += ../../../../libsrc/libnetcdf.la

# Point pre-processor and linker to netcdf-3 directory (libsrc).
AM_CPPFLAGS += -I$(top_srcdir)/../libsrc
AM_LDFLAGS += -L../../../../libsrc

endif #USE_NETCDF4




