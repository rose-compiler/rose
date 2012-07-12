# This is a automake file, part of Unidata's netCDF package.
# Copyright 2006, see the COPYRIGHT file for more information.

# This file builds the C examples.

# $Id: Makefile.am,v 1.22 2009/12/08 20:58:11 dmh Exp $

LDADD = -lm
AM_CPPFLAGS = -I$(top_srcdir) 

# These are the netCDF-3 examples. 
TESTPROGRAMS = simple_xy_wr simple_xy_rd sfc_pres_temp_wr	\
sfc_pres_temp_rd pres_temp_4D_wr pres_temp_4D_rd

# To build netcdf-4, or not to build netcdf-4, that is the question...
if USE_NETCDF4

# These are the extra netCDF-4 examples. 
TESTPROGRAMS += simple_nc4_wr simple_nc4_rd simple_xy_nc4_wr	\
simple_xy_nc4_rd

# Link to the netCDF-4, (optionally) HDF4, and the HDF5 libraries.
LDADD += ../../libsrc4/libnetcdf.la 
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

# Point pre-processor and linker to netcdf-4 directory (libsrc4).
AM_CPPFLAGS += -I$(top_srcdir)/libsrc4
AM_LDFLAGS = -L../../libsrc4

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

if USE_PNETCDF_DIR
AM_CPPFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

else # not USE_NETCDF4

# Link to the netCDF-3 library.
LDADD += ../../libsrc/libnetcdf.la

# Point pre-processor and linker to netcdf-3 directory (libsrc).
AM_CPPFLAGS += -I$(top_srcdir)/libsrc
AM_LDFLAGS = -L../../libsrc

endif #USE_NETCDF4

if BUILD_DAP
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

check_PROGRAMS = $(TESTPROGRAMS)
TESTS = $(TESTPROGRAMS)

# These files are created by the tests.
CLEANFILES = sfc_pres_temp.nc simple_xy.nc pres_temp_4D.nc	\
simple_nc4.nc simple_xy_nc4.nc

