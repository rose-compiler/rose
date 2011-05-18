# This is part of the netCDF package.
# Copyright 2005 University Corporation for Atmospheric Research/Unidata
# See COPYRIGHT file for conditions of use.
# 
# This entire directory will be skipped, unless the configure script
# is run with --enable-netcdf-4.
#
# $Id: Makefile.am,v 1.64 2010/01/11 19:27:11 ed Exp $

if USE_NETCDF4

# Set up flags to find libsrc4 version of libnetcdf.a and netcdf.h.
AM_CPPFLAGS = -I$(srcdir) -I$(top_srcdir)/libsrc4 
LDADD = ../libsrc4/libnetcdf.la
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

AM_LDFLAGS =

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_CPPFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root for HDF4, use it.
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

if BUILD_DAP
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

# Our test programs and sources...
#check_PROGRAMS = tst_nc_test_file cdm_sea_soundings
check_PROGRAMS = cdm_sea_soundings
#tst_nc_test_file_SOURCES = tst_nc_test_file.c tests.h
cdm_sea_soundings_SOURCES = cdm_sea_soundings.c tests.h
TESTS = cdm_sea_soundings

if LARGE_FILE_TESTS
tst_large_SOURCES = tst_large.c tests.h
check_PROGRAMS += tst_large
TESTS += tst_large
endif # LARGE_FILE_TESTS

if BUILD_BENCHMARKS
bm_file_SOURCES = bm_file.c tests.h
tst_create_files_SOURCES = tst_create_files.c tests.h
tst_ar4_SOURCES = tst_ar4.c tests.h
tst_chunks_SOURCES = tst_chunks.c
check_PROGRAMS += tst_create_files bm_file tst_chunks tst_ar4	\
tst_ar4_3d tst_ar4_4d
TESTS += tst_ar4_3d tst_create_files run_bm_test1.sh run_bm_elena.sh	\
run_bm_test2.sh run_tst_chunks.sh run_bm_ar4.sh
if TEST_PARALLEL
TESTS += run_par_bm_test.sh
endif # TEST_PARALLEL
benchmarks: check
	./run_bm_radar_2D.sh	
	./run_bm_radar_2D_compression1.sh	
	./run_bm.sh
	./run_tst_chunks.sh
	./run_bm_ar4.sh
endif # BUILD_BENCHMARKS

CLEANFILES = cdm_sea_soundings.nc tst_large.nc bm_chunking.nc		\
bm_radar.nc bm_radar1.nc radar_3d_compression_test.txt			\
radar_3d_compression.txt radar_2d_compression.txt			\
radar_3d_chunking.txt tst_floats_1D.nc tst_floats_1D.cdl		\
floats_1D_3.nc floats_1D.cdl tst_floats2_*.nc tst_ints2_*.nc		\
tst_shorts2_*.nc tst_elena_*.nc tst_simple*.nc tst_floats2_*.cdl	\
tst_ints2_*.cdl tst_shorts2_*.cdl tst_elena_*.cdl tst_simple*.cdl	\
tst_chunks.nc tst_chunks.cdl pr_A1.* tauu_A1.* usi_01.* thetau_01.*

# This are extra tests that will only be run if netcdf-4 is configured
# with --enable-parallel-tests.
if TEST_PARALLEL
check_PROGRAMS += tst_mpi_parallel tst_parallel tst_parallel3	\
tst_parallel4 tst_nc4perf
CLEANFILES += tst_mpi_parallel.bin tst_parallel.nc tst_parallel3.nc	\
tst_parallel4.nc tst_nc4perf.nc tst_parallel2.nc
TESTS += run_par_test.sh
if USE_PNETCDF
check_PROGRAMS += tst_parallel2
TESTS += run_pnetcdf_test.sh
endif
endif

EXTRA_DIST = run_par_test.sh run_bm.sh run_bm_test1.sh run_bm_test2.sh	\
run_bm_radar_2D.sh run_bm_radar_2D_compression1.sh run_par_bm_test.sh	\
run_bm_elena.sh run_par_bm_radar_2D.sh run_bm_radar_2D_endianness1.sh   \
run_tst_chunks.sh ref_chunks1.cdl ref_chunks2.cdl run_pnetcdf_test.sh

endif # BUILD_NETCDF4


