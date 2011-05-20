## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This automake file generates the Makefile to build netCDF-4. The
# generated makefile is not run unless the user selected to build
# netCDF-4.

# $Id: Makefile.am,v 1.176 2010/04/12 15:34:28 ed Exp $

# Build the netcdf.h file depending on whether parallel I/O is being
# used or not.
#netcdf.h: netcdf_base.h
#	cat @top_srcdir@/libsrc4/netcdf_base.h > netcdf.h

# Cause C preprocessor to search current and parent directory.
AM_CPPFLAGS = -I$(top_srcdir)/libsrc

AM_LDFLAGS = 

# If the user specified a root location for HDF5, use it.
if USE_HDF5_DIR
AM_CPPFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root location for HDF4, use it.
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

# If the user specified a root location for pnetcdf, use it.
if USE_PNETCDF_DIR
AM_CPPFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

# Tell automake that netcdf.h is a header file we want to install.
include_HEADERS = netcdf.h

# This is our output. The big enchilada.
lib_LTLIBRARIES = libnetcdf.la

# Our output library will also include contents of the netcdf3
# convenience library.
libnetcdf_la_LIBADD = ../libsrc/libnetcdf3.la

if BUILD_DAP
libnetcdf_la_LIBADD += ../libncdap4/libncdap4.la
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

# Include netcdf-2 unless v2 has been turned off..
if BUILD_V2
libnetcdf_la_LIBADD += ../libsrc/libnetcdf2.la
TST_V2 = tst_v2 
endif

# Netcdf-4 source.
libnetcdf_la_SOURCES = nc4attr.c nc4internal.c nc4var.c netcdf3.h	\
nc4dim.c nc4internal.h ncfunc.c nc4file.c error.c error.h nc4grp.c	\
nc4type.c nc4hdf.c netcdf4l.h nctolnc4.h rename4.h netcdf.h		\
netcdf_base.h

# If we are not building separate fortran libraries, then include
# their functions in the C library.
if !BUILD_SEPARATE_FORTRAN

# If F77 is built, include its convenience library. If F90 is built,
# it will also be part of this convenience library.
if BUILD_F77
libnetcdf_la_LIBADD += ../fortran/libnetcdff.la
endif

endif # !BUILD_SEPARATE_FORTRAN

# These linker flags specify libtool version info.
libnetcdf_la_LDFLAGS = -version-number 6:0:0

check_PROGRAMS = tst_h_files tst_h_files2 tst_h_atts tst_h_atts2	\
tst_h_atts3 tst_h_vars tst_h_vars2 tst_h_vars3 tst_h_grps		\
tst_h_compounds tst_h_compounds2 tst_h_wrt_cmp tst_h_rd_cmp tst_h_vl	\
tst_h_opaques tst_h_strings tst_h_strings1 tst_h_dimscales		\
tst_h_dimscales1 tst_h_dimscales2 tst_h_dimscales3 tst_h_enums		\
tst_h_vl2 tst_lists tst_dims tst_dims2 tst_dims3 tst_files tst_files4	\
tst_vars tst_varms tst_unlim_vars tst_atts tst_atts2 tst_converts	\
tst_converts2 tst_grps tst_compounds tst_compounds2 tst_compounds3	\
tst_vl tst_opaques tst_strings tst_interops tst_interops4 tst_enums	\
tst_vars2 tst_coords tst_coords2 tst_coords3 tst_vars3 tst_chunks	\
tst_utf8 tst_fills tst_fillbug $(TST_V2) tst_xplatform tst_xplatform2	\
tst_endian_fill

if LARGE_FILE_TESTS
check_PROGRAMS += tst_large
endif

# The parallel program is run from a script.
if TEST_PARALLEL
check_PROGRAMS += tst_h_par
endif

TESTS = tst_h_files tst_h_files2 tst_h_atts tst_h_atts3 tst_h_vars	\
tst_h_vars2 tst_h_vars3 tst_h_grps tst_h_compounds tst_h_compounds2	\
tst_h_wrt_cmp tst_h_rd_cmp tst_h_vl tst_h_opaques tst_h_strings		\
tst_h_strings1 tst_h_dimscales tst_h_dimscales1 tst_h_dimscales2	\
tst_h_dimscales3 tst_h_enums tst_lists tst_dims tst_dims2 tst_dims3	\
tst_files tst_files4 tst_vars tst_varms tst_unlim_vars tst_atts		\
tst_atts2 tst_converts tst_converts2 tst_grps tst_compounds		\
tst_compounds2 tst_compounds3 tst_vl tst_h_vl2 tst_opaques tst_strings	\
tst_interops tst_interops4 tst_enums tst_vars2 tst_vars3 tst_chunks	\
tst_coords tst_coords2 tst_coords3 tst_utf8 tst_fills tst_fillbug	\
$(TST_V2) tst_xplatform tst_xplatform2 tst_h_atts2 tst_endian_fill

if USE_HDF4
check_PROGRAMS += tst_interops2 
TESTS += tst_interops2
if USE_HDF4_FILE_TESTS
check_PROGRAMS += tst_interops3
TESTS += run_get_hdf4_files.sh tst_interops3
endif # USE_HDF4_FILE_TESTS
tst_interops2_LDADD = ${lib_LTLIBRARIES} -lmfhdf -ldf -ljpeg -lhdf5_hl -lhdf5 -lz 
endif # USE_HDF4

EXTRA_DIST = ref_tst_compounds.nc ref_tst_h_compounds.h5	\
ref_tst_h_compounds2.h5 run_par_tests.sh run_valgrind_tests.sh	\
run_hdf4_valgrind_tests.sh ref_tst_xplatform2_1.nc		\
ref_tst_xplatform2_2.nc ref_tst_dims.nc run_get_hdf4_files.sh	\
ref_tst_interops4.nc

# (The above does not include tst_h_converts, which won't work until I
# sort out the MAX_INT problem. - Ed)

# These linker flags will be applied to all test program compiles.
LDADD = ${lib_LTLIBRARIES}

if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
libnetcdf_la_LIBADD += -lmfhdf -ldf 
endif # USE_HDF4

LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
libnetcdf_la_LIBADD += -lpnetcdf
endif

# Always need to link to hdf5 and zlib.
libnetcdf_la_LIBADD += -lhdf5_hl -lhdf5 

# This header is used by all test programs.
noinst_HEADERS = nc_tests.h

# This script will run all the parallel test programs.
if TEST_PARALLEL
TESTS += run_par_tests.sh
endif

# Add this if --enable-benchmarks was used.
if BUILD_BENCHMARKS
TESTS += tst_files2 tst_files3 tst_ar5 tst_h_files3
check_PROGRAMS += tst_files2 tst_files3 tst_ar5 tst_h_files3
endif # BUILD_BENCHMARKS

# This will run a bunch of the test programs with valgrind, the memory
# checking tool. (Valgrind must be present for this to work.)
if USE_VALGRIND_TESTS
TESTS += run_valgrind_tests.sh
if USE_HDF4
TESTS += run_hdf4_valgrind_tests.sh
endif # USE_HDF4
endif # USE_VALGRIND_TESTS

# Clean up. Each test program creates one data file, with the same
# name as the test program, and either a .h5 or .nc, for "pure" HDF5
# files and netCDF-4 files.
CLEANFILES = tst_nc_converts.nc tst_h_*.h5 tst_*.nc tst_interops.h5	\
tst_interops2.h4 tst_interops4.nc AMSR_E_* MYD29.A2* 

DISTCLEANFILES = netcdf.h

if USE_HDF4_FILE_TESTS
DISTCLEANFILES += AMSR_E_L2_Rain_V10_200905312326_A.hdf	\
AMSR_E_L3_DailyLand_V06_20020619.hdf			\
MYD29.A2009152.0000.005.2009153124331.hdf		\
MYD29.A2002185.0000.005.2007160150627.hdf		\
MOD29.A2000055.0005.005.2006267200024.hdf
endif # HDF4_FILE_TESTS

test: check
