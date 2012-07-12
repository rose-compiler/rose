## This is a automake file, part of Unidata's netCDF package.
# Copyright 2006, see the COPYRIGHT file for more information.  

# This file builds F77 examples.

# $Id: Makefile.am,v 1.24 2009/12/08 20:58:13 dmh Exp $

# Link in the fortran convenivence library.
LDADD = ../../fortran/libnetcdff.la

# Go to fortran directory for header.
AM_FFLAGS = -I$(top_srcdir)/fortran -I$(top_builddir)/fortran
AM_LDFLAGS =

# These are the example programs.
TESTPROGRAMS = simple_xy_wr simple_xy_rd sfc_pres_temp_wr	\
sfc_pres_temp_rd pres_temp_4D_wr pres_temp_4D_rd

# Is the user building netCDF-4?
if USE_NETCDF4

# If the user specified a root location for HDF4, use it.
if USE_HDF4_DIR
AM_FFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
endif

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_FFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root for HDF5, use it.
if USE_PNETCDF
AM_FFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

# Find netcdf-4 header in libsrc4.
AM_FFLAGS += -I$(top_srcdir)/libsrc4

# Since netCDF-4 is being build, link to the netcdf library in
# ../../libsrc4, also link to HDF5 and HDF5 high-level libraries.
LDADD += ../../libsrc4/libnetcdf.la
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif
LDADD +=  -lhdf5_hl -lhdf5 -lz

if USE_PNETCDF
LDADD += -lpnetcdf
endif

TESTPROGRAMS += simple_xy_nc4_wr simple_xy_nc4_rd

simple_xy_nc4_wr_SOURCES = simple_xy_nc4_wr.f
simple_xy_nc4_rd_SOURCES = simple_xy_nc4_rd.f

else # not USE_NETCDF4

# Without netcdf-4, go to ../../libsrc for the library.
LDADD += ../../libsrc/libnetcdf.la 

endif # USE_NETCDF4

if BUILD_DAP
AM_FFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

check_PROGRAMS = $(TESTPROGRAMS)

EXTRA_DIST = do_comps.sh

# Here's the source code for the programs.
simple_xy_wr_SOURCES = simple_xy_wr.f
simple_xy_rd_SOURCES = simple_xy_rd.f
sfc_pres_temp_wr_SOURCES = sfc_pres_temp_wr.f
sfc_pres_temp_rd_SOURCES = sfc_pres_temp_rd.f
pres_temp_4D_wr_SOURCES = pres_temp_4D_wr.f
pres_temp_4D_rd_SOURCES = pres_temp_4D_rd.f

# Run all test programs, plus the do_comps.sh script, which checks
# that all the output files are the same as the C example output
# files.
TESTS = $(TESTPROGRAMS) do_comps.sh

# Cleaning up files created during the process.
CLEANFILES = simple_xy.nc sfc_pres_temp.nc pres_temp_4D.nc	\
simple_xy_nc4.nc
