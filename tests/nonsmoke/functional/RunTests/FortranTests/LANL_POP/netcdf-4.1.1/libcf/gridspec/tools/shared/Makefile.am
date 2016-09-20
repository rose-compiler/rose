## This is an automake file, part of Unidata's NetCDF CF Library
## package. Copyright 2009, see the COPYRIGHT file for more
## information.

# This automake file is in charge of building the gridspec
# tools/shared convenience library.

# $Id: Makefile.am,v 1.4 2009/09/01 16:56:46 ed Exp $

noinst_LTLIBRARIES = libtoolsshared.la 
libtoolsshared_la_SOURCES = mpp.c mpp_domain.c mpp_domain.h mpp.h	\
mpp_io.c mpp_io.h tool_util.c tool_util.h create_lonlat_grid.c		\
create_gnomonic_cubic_grid.c create_grid_from_file.c			\
create_conformal_cubic_grid.c fregrid_util.c conserve_interp.c		\
bilinear_interp.c topog.c get_contact.c create_vgrid.c			\
transfer_to_mosaic.c

AM_CPPFLAGS = -I../shared -I../../shared/mosaic -I../make_hgrid	\
-I../fregrid -I../make_coupler_mosaic -I../make_topog		\
-I../make_solo_mosaic -I../make_vgrid -I../river_regrid		\
-I../transfer_to_mosaic_grid

AM_LDFLAGS =
LDADD =

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





