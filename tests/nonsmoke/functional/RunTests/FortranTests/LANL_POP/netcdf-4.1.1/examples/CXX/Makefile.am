## This is a automake file, part of Unidata's netCDF package.
# Copyright 2006, see the COPYRIGHT file for more information.  

# This file builds CXX examples.

# $Id: Makefile.am,v 1.20 2009/12/08 20:58:12 dmh Exp $

# We will need to find the C++ header files.
AM_CXXFLAGS = -I$(top_srcdir)/cxx 
AM_LDFLAGS=

# Is the user building netCDF-4?
if USE_NETCDF4

# Find the header file for netcdf-4.
AM_CXXFLAGS += -I$(top_srcdir)/libsrc4

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_CXXFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root for HDF4, use it.
if USE_HDF4_DIR
AM_CXXFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
endif

# If the user specified a root for PNETCDF, use it.
if USE_PNETCDF_DIR
AM_CXXFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

# Since netCDF-4 is being build, link to the netcdf library in
# ../../libsrc4, also link to HDF5 and HDF5 high-level libraries.
LDADD = ../../cxx/libnetcdf_c++.la ../../libsrc4/libnetcdf.la	
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

else # not USE_NETCDF4

# Include the headers from the netCDF-3 C library.
AM_CXXFLAGS += -I$(top_srcdir)/libsrc

LDADD = ../../cxx/libnetcdf_c++.la

# Link the examples to the C++ and C libraries.
LDADD += ../../libsrc/libnetcdf.la

endif # USE_NETCDF4

if BUILD_DAP
AM_CXXFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

# These are the example programs.
TESTPROGRAMS = simple_xy_wr simple_xy_rd sfc_pres_temp_wr	\
sfc_pres_temp_rd pres_temp_4D_wr pres_temp_4D_rd
check_PROGRAMS = $(TESTPROGRAMS)

# Ship also this script, which compares the output of these C++
# examples with the output of the C examples. The output data files
# are identical.
EXTRA_DIST = do_comps.sh

# Here's the source code for the programs.
simple_xy_wr_SOURCES = simple_xy_wr.cpp
simple_xy_rd_SOURCES = simple_xy_rd.cpp
sfc_pres_temp_wr_SOURCES = sfc_pres_temp_wr.cpp
sfc_pres_temp_rd_SOURCES = sfc_pres_temp_rd.cpp
pres_temp_4D_wr_SOURCES = pres_temp_4D_wr.cpp
pres_temp_4D_rd_SOURCES = pres_temp_4D_rd.cpp

# Run all example programs, plus the do_comps.sh script, which checks
# that all the output files are the same as the C example output
# files.
TESTS = $(TESTPROGRAMS) do_comps.sh

# Cleaning up files created during the process.
CLEANFILES = simple_xy.nc sfc_pres_temp.nc pres_temp_4D.nc

