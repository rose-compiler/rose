## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005-2006, see the COPYRIGHT file for more information.

# This file builds and runs the following: 

# The nf_test program, which tests the netCDF fortran 77 interface.

# The ftest program, which tests the netCDF fortran 77 V2
# interface. (Unless V2 is disabled).

# Test programs ftst_vars, ftst_vars2, ftst_vars3, ftst_vars4,
# ftst_vars5, ftst_types, ftst_types2, and ftst_groups, if netCDF-4 is
# enabled.

# The tst_f90 and f90tst_vars programs, which tests the netCDF fortran
# 90 interface. (Unless F90 API is disabled).

# $Id: Makefile.am,v 1.86 2010/01/25 21:01:07 ed Exp $

LDADD =
AM_LDFLAGS = 
AM_CFLAGS =
TESTS =

if BUILD_UTILITIES
# The create_fills shell creates the file fills.nc, needed by later
# tests.
TESTS += create_fills.sh
endif # BUILD_UTILITIES

# nf_test is the main test program. 
check_PROGRAMS = nf_test
nf_test_SOURCES = test_get.m4 test_put.m4 nf_error.F nf_test.F	\
test_read.F test_write.F util.F fortlib.c tests.inc
TESTS += nf_test

# Cleaning up files created during the process.
CLEANFILES = scratch.nc test.nc copy.nc fills.nc
MAINTAINERCLEANFILES = test_get.F test_put.F

# Did the user build the V2 F77 API? If so, run this test.
if BUILD_V2
check_PROGRAMS += tst_f77_v2
tst_f77_v2_SOURCES = tst_f77_v2.F
TESTS += tst_f77_v2
CLEANFILES += tst_f77_v2.nc
endif # BUILD_V2

# If building a separate fortran library, include it so the tests can
# find it.
if BUILD_SEPARATE_FORTRAN
LDADD += ../fortran/libnetcdff.la -lm
# Point pre-processor and linker to directory with C lib.
AM_CPPFLAGS = -I$(NETCDF_C_LIB)/include
AM_LDFLAGS += -L$(NETCDF_C_LIB)/lib
endif

# Some fortran compilers think your files should not have an .f90
# extension! The value of FCFLAGS_f90 is set in the configure script,
# based on the fortran compiler.
AM_FCFLAGS = $(FCFLAGS_f90) -I$(top_srcdir)/fortran -I$(top_builddir)/f90
AM_FFLAGS = -I$(top_srcdir)/fortran

# Is the user building netCDF-4?
if USE_NETCDF4

# Add these netCDF-4 f77 test programs.
check_PROGRAMS += ftst_vars ftst_vars2 ftst_vars3 ftst_vars4 ftst_vars5	\
ftst_vars6 ftst_types ftst_types2 ftst_types3 ftst_groups
ftst_types_SOURCES = ftst_types.F fortlib.c handle_err.F
ftst_types2_SOURCES = ftst_types2.F fortlib.c handle_err.F
ftst_types3_SOURCES = ftst_types3.F fortlib.c handle_err.F
ftst_vars_SOURCES = ftst_vars.F fortlib.c handle_err.F
ftst_vars2_SOURCES = ftst_vars2.F fortlib.c handle_err.F
ftst_vars3_SOURCES = ftst_vars3.F fortlib.c handle_err.F
ftst_vars4_SOURCES = ftst_vars4.F fortlib.c handle_err.F
ftst_vars5_SOURCES = ftst_vars5.F fortlib.c handle_err.F
ftst_vars6_SOURCES = ftst_vars6.F fortlib.c handle_err.F
ftst_groups_SOURCES = ftst_groups.F fortlib.c handle_err.F
TESTS += ftst_vars ftst_vars2 ftst_vars3 ftst_vars4 ftst_vars5	\
ftst_vars6 ftst_types ftst_types2 ftst_types3 ftst_groups
CLEANFILES += ftst_vars.nc ftst_vars2.nc ftst_vars3.nc ftst_vars4.nc	\
ftst_vars5.nc ftst_vars6.nc ftst_types.nc ftst_types2.nc		\
ftst_types3.nc ftst_groups.nc

# Add these netCDF-4 f90 test programs.
if BUILD_F90
check_PROGRAMS += f90tst_vars tst_types tst_types2 f90tst_vars_vlen	\
tst_f90_nc4 f90tst_grps f90tst_fill f90tst_fill2 f90tst_vars3
tst_f90_nc4_SOURCES = tst_f90_nc4.f90
f90tst_vars_SOURCES = f90tst_vars.f90
tst_types_SOURCES = tst_types.f90
tst_types2_SOURCES = tst_types2.f90
f90tst_vars_vlen_SOURCES = f90tst_vars_vlen.f90
f90tst_grps_SOURCES = f90tst_grps.f90
f90tst_fill_SOURCES = f90tst_fill.f90
f90tst_fill2_SOURCES = f90tst_fill2.f90
f90tst_vars3_SOURCES = f90tst_vars3.f90
TESTS += f90tst_vars tst_types tst_types2 f90tst_vars_vlen tst_f90_nc4	\
f90tst_grps f90tst_fill f90tst_fill2 f90tst_vars3
CLEANFILES += f90tst_*.nc tst_types.nc tst_types2.nc tst_f90_nc4.nc

if USE_SZIP
check_PROGRAMS += f90tst_vars2 
TESTS += f90tst_vars2
f90tst_vars2_SOURCES = f90tst_vars2.f90
endif

# This is a netCDF-4 V2 test program.
if BUILD_V2
check_PROGRAMS += ftst_v2
ftst_v2_SOURCES = ftst_v2.F 
TESTS += ftst_v2
CLEANFILES += ftst_v2.nc
endif # BUILD_V2

# This is the netCDF-4 F90 large file test.
if LARGE_FILE_TESTS
check_PROGRAMS += tst_flarge
tst_flarge_SOURCES = tst_flarge.f90
TESTS += tst_flarge
CLEANFILES += tst_flarge.nc
endif # LARGE_FILE_TESTS

# This is an f90 benchmark.
if BUILD_BENCHMARKS
check_PROGRAMS += tst_io
tst_io_SOURCES = tst_io.f90
TESTS += tst_io
CLEANFILES += tst_io1.nc tst_io2.nc tst_io3.nc tst_io4.nc tst_io5.nc	\
tst_io6.nc tst_io7.nc tst_io8.nc tst_io9.nc tst_io10.nc tst_io11.nc
endif #BUILD_BENCHMARKS

# Test parallel I/O.
if TEST_PARALLEL
check_PROGRAMS += f90tst_parallel f90tst_parallel2 f90tst_parallel3	
#f90tst_parallel_fill
f90tst_parallel_SOURCES = f90tst_parallel.f90
f90tst_parallel2_SOURCES = f90tst_parallel2.f90
f90tst_parallel3_SOURCES = f90tst_parallel3.f90
#f90tst_parallel_fill_SOURCES = f90tst_parallel_fill.f90
TESTS += run_f90_par_test.sh
CLEANFILES += f90tst_parallel*.nc
endif # TEST_PARALLEL

endif #BUILD_F90

# Test parallel I/O for F77.
if TEST_F77
if TEST_PARALLEL
check_PROGRAMS += ftst_parallel
ftst_parallel_SOURCES = ftst_parallel.F
TESTS += run_f77_par_test.sh
CLEANFILES += ftst_parallel.nc
endif # TEST_PARALLEL
endif # TEST_F77

# If the user specified a prefix for the HDF4 library location, use it.
if USE_HDF4_DIR
AM_FFLAGS += -I@HDF4DIR@/include
AM_FCFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
endif

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_FFLAGS += -I@HDF5DIR@/include
AM_FCFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root location for ZLIB, use it.
if USE_ZLIB_DIR
AM_FFLAGS += -I@ZLIBDIR@/include
AM_FCFLAGS += -I@ZLIBDIR@/include
AM_LDFLAGS += -L@ZLIBDIR@/lib
endif

# If the user specified a root location for SZLIB, use it.
if USE_SZLIB_DIR
AM_FFLAGS += -I@SZLIBDIR@/include
AM_FCFLAGS += -I@SZLIBDIR@/include
AM_LDFLAGS += -L@SZLIBDIR@/lib
endif

# If the user specified a root location for pnetcdf, use it.
if USE_PNETCDF_DIR
AM_FFLAGS += -I@PNETCDFDIR@/include
AM_FCFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

if BUILD_C

# Find header file in libsrc4 directory.
AM_FFLAGS += -I$(top_srcdir)/libsrc4
AM_FCFLAGS += -I$(top_srcdir)/libsrc4

# Include the C library in the link.
LDADD += ../libsrc4/libnetcdf.la 

else

AM_FFLAGS += -I@NETCDF_C_LIB@/include
AM_FCFLAGS += -I@NETCDF_C_LIB@/include
AM_LDFLAGS += -L@NETCDF_C_LIB@/lib
LDADD += -lnetcdf

endif # BUILD_C

# Link to the netCDF-4, (optionally) HDF4, and the HDF5 libraries.
LDADD += ../libsrc4/libnetcdf.la 
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

else # not USE_NETCDF4

if BUILD_C

# Find header file in libsrc directory.
AM_FFLAGS += -I$(top_srcdir)/libsrc -I$(top_srcdir)/fortran
AM_FCFLAGS += -I$(top_srcdir)/libsrc -I$(top_srcdir)/fortran 

# Include the C library in the link.
LDADD += ../libsrc/libnetcdf.la

else # not BUILD_C

AM_FFLAGS += -I@NETCDF_C_LIB@/include
AM_FCFLAGS += -I@NETCDF_C_LIB@/include
AM_LDFLAGS += -L@NETCDF_C_LIB@/lib
LDADD += -lnetcdf

endif # BUILD_C

endif # USE_NETCDF4

if BUILD_DAP
AM_CFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

if BUILD_C
# This is the fortran v2 test. It depends on the utilities being built
# to generate it's input file.
if BUILD_V2
if BUILD_UTILITIES
check_PROGRAMS += ftest
ftest_SOURCES = ftest.F fortlib.c 
TESTS += ftest
CLEANFILES += test.nc
endif # BUILD_UTILITIES
endif # BUILD_V2
endif #BUILD_C

# This is the netCDF-3 F90 test.
if BUILD_F90
AM_FFLAGS += @MOD_FLAG@../f90
AM_FCFLAGS += @MOD_FLAG@../f90
check_PROGRAMS += tst_f90
tst_f90_SOURCES = tst_f90.f90
TESTS += tst_f90
CLEANFILES += tst_f90.nc
endif

# Tell make how to turn .m4 files into .F files.
.m4.F:
	m4 $(M4FLAGS) $< >$@

# test_get.F and test_put.f need to be distributed, so that the user
# need not have m4. fills.nc is used by test program ftest.
EXTRA_DIST = test_get.F test_put.F fills.cdl create_fills.sh	\
run_f90_par_test.sh run_f77_par_test.sh

test: check
