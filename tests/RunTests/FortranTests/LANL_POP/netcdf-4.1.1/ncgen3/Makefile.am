# This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.
# This file builds and runs the ncgen program.

# $Id: Makefile.am,v 1.45 2010/03/31 18:18:39 dmh Exp $

# Build ncgen from the listed sources.
bin_PROGRAMS = ncgen3
ncgen3_SOURCES = main.c load.c escapes.c getfill.c init.c	\
genlib.c generic.h ncgen.h genlib.h ncgentab.h ncgentab.c

man_MANS = ncgen3.1

# These files all need to be distributed.
EXTRA_DIST = ncgen.y ncgenyy.c ncgen.l c0.cdl run_tests.sh run_nc4_tests.sh \
             $(man_MANS)

# This shell script causes ncgen3 to build a classic and a 64-bit
# offset file from a cdl file shipped with the distribution.
TESTS = run_tests.sh 

# The tests produce these files.
CLEANFILES = c0.nc c0_64.nc

AM_LDFLAGS =
AM_CPPFLAGS =

if USE_NETCDF4

# This shell script causes ncgen to build a netcdf-4 and netcdf-4
# classic file from a cdl file shipped with the distribution.
TESTS += run_nc4_tests.sh

# The nc4 tests produce these files.
CLEANFILES += c0_4.nc c0_4c.nc

# If netcdf4 is built, point at it's directory. 
AM_CPPFLAGS += -I.. -I../libsrc4 -I$(top_srcdir)/libsrc4

# Link to the netCDF-4, (optionally) HDF4, and the HDF5 libraries.
LDADD = ../libsrc4/libnetcdf.la

if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

# If the user specified a prefix for the HDF4 library location, use it.
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

# Look for netcdf.h in libsrc.
AM_CPPFLAGS += -I.. -I../libsrc -I$(top_srcdir)/libsrc

# Link to the netcdf-3 library.
LDADD = ../libsrc/libnetcdf.la

endif # USE_NETCDF4

LDADD += -lm

if BUILD_DAP
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

# Here is the sequence of tests that need to be run:
# ./ncgen3 -b -o c0.nc c0.cdl
# ../ncdump/ncdump -n c1 c0.nc > c1.cdl
# *** ncgen3 -b test successful ***
# ./ncgen3 -c -o ctest0.nc c0.cdl > ctest.c && \
# cc -o ctest -g  -I../libsrc -I.   -Df2cFortran ctest.c ../libsrc/libnetcdf.a  && \
# ./ctest	&& \
# ../ncdump/ncdump -n c1 ctest0.nc > ctest1.cdl
# *** ncgen3 -c test successful ***

test: check

# This is used if someone wants to rebuild ncgenyy.c or ncgentab.c
# Otherwise never invoked, but records how to do it. Don't forget to
# manually insert #include <config.h> in ncgenyy.c!
ncgenyy.c: ncgen.l
	flex -Pncg -8 ncgen.l
	mv lex.ncg.c ncgenyy.c

ncgentab.c ncgentab.h: ncgen.y
	bison -pncg -d ncgen.y
	mv ncgen.tab.c ncgentab.c
	mv ncgen.tab.h ncgentab.h
