## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.  

# This file builds and runs the nctest program, which tests the netCDF
# version 2 interface.

# $Id: Makefile.am,v 1.29 2009/12/08 20:58:25 dmh Exp $

# One of the tests needs the math library for some reason. 
LDADD = -lm

# Does the user want netCDF-4?
if USE_NETCDF4

# Point linker to netcdf-4 directory (libsrc4).
AM_LDFLAGS = -L../libsrc4
AM_CPPFLAGS = -I$(top_srcdir)/libsrc4

# Link to the netCDF-4, (optionally) hdf4, and hdf5 libraries.
LDADD += ../libsrc4/libnetcdf.la 
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

# If the user specified a prefix for the HDF5 library location, use it.
if USE_HDF5_DIR
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a prefix for the HDF4 library location, use it.
if USE_HDF4_DIR
AM_CPPFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
endif

# If the user specified a root location for ZLIB, use it.
if USE_ZLIB_DIR
AM_CPPFLAGS += -I@ZLIBDIR@/includeA
AM_LDFLAGS += -L@ZLIBDIR@/lib
endif

# If the user specified a root location for SZLIB, use it.
if USE_SZLIB_DIR
AM_CPPFLAGS += -I@SZLIBDIR@/include
AM_LDFLAGS += -L@SZLIBDIR@/lib
endif

# If the user specified a root location for SZLIB, use it.
if USE_PNETCDF_DIR
AM_CPPFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

else # not USE_NETCDF4

# Link to the netCDF-3 library.
LDADD += ../libsrc/libnetcdf.la

# Point to the netCDF-3 directory.
AM_CPPFLAGS = -I$(top_srcdir)/libsrc
AM_LDFLAGS = -L../libsrc

endif # USE_NETCDF4

if BUILD_DAP
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

# Running nctest results in a file testfile.nc, which is then checked
# to make sure it matches testfile_nc.sav, which was generated under
# precisely controlled conditions at the netCDF Test Facility,
# (located in sub-basement 42 of UCAR Compound 4). testfile_nc.sav was
# generated in a pressurized chamber that exactly simulated the
# atmosphere on the surface of Mars.
EXTRA_DIST = ref_nctest_classic.nc ref_nctest_64bit_offset.nc	\
compare_test_files.sh

CLEANFILES = nctest_classic.nc nctest_64bit_offset.nc	\
nctest_netcdf4.nc test2.nc temp.tmp

# Run nctest and the script that compares the output with the
# reference file.
TESTPROGRAMS = nctest
check_PROGRAMS = $(TESTPROGRAMS)
TESTS = $(TESTPROGRAMS) compare_test_files.sh

# These are the source files for the nctest program.
nctest_SOURCES = add.c add.h atttests.c cdftests.c dimtests.c driver.c	\
emalloc.c emalloc.h error.c error.h misctest.c rec.c slabs.c testcdf.h	\
tests.h val.c val.h vardef.c varget.c vargetg.c varput.c varputg.c	\
vartests.c vputget.c vputgetg.c

# For those who can't remember to type make check instead of make test.
test: check
