## This is an automake file, part of Unidata's NetCDF CF Library package.
# Copyright 2006, see the COPYRIGHT file for more information.

# THis automake file is in charge of building the src directory.

# $Id: Makefile.am,v 1.8 2010/04/01 19:57:33 ed Exp $

# Tell automake that libcf.h is a header file we want to install.
include_HEADERS = libcf.h

# Test programs will need to link to this library (libcf) and also
# netcdf.
LDADD = ${lib_LTLIBRARIES}  ../../ncdump/nctime.o

# Initalize these.
AM_CPPFLAGS=
AM_LDFLAGS=

# This is our output, the NetCDF CF Library.
lib_LTLIBRARIES = libcf.la

check_PROGRAMS = tst_coords tst_files tst_vars tst_cvars tst_vert	\
tst_subsets

#if BUILD_F90
#lib_LTLIBRARIES += libcff.la 
#libcff_la_SOURCES = cffcvars.f90
#check_PROGRAMS += tst_f03_file
#tst_f03_file_SOURCES = tst_f03_file.f90
#LDADD += libcf.la 
#nodist_include_HEADERS = libcf.mod
#if UPPER_CASE_MOD
#LIBCF.mod: cffcvars.o
#BUILT_SOURCES = LIBCF.mod
#else
#libcf.mod: cffcvars.o
#BUILT_SOURCES = libcf.mod
#endif
# if USE_NETCDF_DIR
# AM_FCFLAGS = @MOD_FLAG@@NETCDFDIR@/include
# else
# AM_FCFLAGS = @MOD_FLAG@../../f90
# endif
# endif # BUILD_F90

# Link to the netCDF-4, (optionally) HDF4, and the HDF5 libraries.
if USE_NETCDF_DIR
LDADD += -lnetcdf
AM_CPPFLAGS += -I@NETCDFDIR@/include
AM_LDFLAGS += -L@NETCDFDIR@/lib
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

if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4

if USE_NETCDF4
LDADD += -lhdf5_hl -lhdf5 -lz 
endif

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

# Add the gridspec functions.
if BUILD_GRIDSPEC
#libcf_la_LIBADD = ../gridspec/shared/mosaic/libshared.la 
#libcf_la_LIBADD += ../gridspec/tools/shared/libtoolsshared.la 
endif 

# These linker flags specify libtool version info.
libcf_la_LDFLAGS = -version-info 0:0:0

# This is the coordinate systems stuff.
libcf_la_SOURCES = nccoord.c libcf.h libcf_int.h cffiles.c cfvars.c	\
cfcvars.c cfsubsets.c cferror.c cferror.h cdTimeConv.c cdmsint.h	\
cdms.h cddrs.h drscdf.h cdunif.h

# These files are created by the tests.
CLEANFILES = libcf.mod tst_coords_classic.nc tst_coords_64bit.nc	\
tst_coords_netcdf4_classic.nc tst_coords_netcdf4.nc tst_vars.nc		\
tst_files.nc tst_cvars.nc tst_vert_sigma.nc tst_vert_hybrid_sigma.nc	\
tst_subsets_simple_sigma.nc tst_subsets_nw.nc tst_subsets_sw.nc		\
tst_subsets_se.nc tst_subsets_ne.nc tst_f03_file.nc

tst_coords_SOURCES = tst_coords.c nc_tests.h
tst_files_SOURCES = tst_files.c nc_tests.h
tst_vars_SOURCES = tst_vars.c nc_tests.h
tst_cvars_SOURCES = tst_cvars.c nc_tests.h
tst_vert_SOURCES = tst_vert.c nc_tests.h
tst_subsets_SOURCES = tst_subsets.c nc_tests.h

if EXTRA_DATA_TESTS
check_PROGRAMS += tst_example_data
tst_example_data_SOURCES = tst_example_data.c nc_tests.h
endif # EXTRA_DATA_TESTS

# Rule to build Fortran 2003 code.
#cfcvars.o:cfcvsars.f90
#	$(FC) -c $(AM_FCFLAGS) $(FCFLAGS) -c $? -o $@

# tst_f03_file.o:tst_f03_file.f90
# 	$(FC) -c $(AM_FCFLAGS) $(FCFLAGS) -c $? -o $@

TESTS = $(check_PROGRAMS)



