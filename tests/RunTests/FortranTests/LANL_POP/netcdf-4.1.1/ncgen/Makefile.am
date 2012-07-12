## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.
# This file builds and runs the ncgen program.

# $Id: Makefile.am,v 1.48 2010/04/04 19:39:42 dmh Exp $

# Build ncgen from the listed sources.
bin_PROGRAMS = ncgen

ncgen_SOURCES=main.c cdata.c bindata.c genchar.c cvt.c data.c debug.c escapes.c \
genc.c genbin.c generr.c genlib.c getfill.c odom.c offsets.c semantics.c \
ncgentab.c \
dump.c util.c bytebuffer.c list.c \
data.h debug.h generr.h genlib.h includes.h ncgen.h odom.h offsets.h \
dump.h util.h bytebuffer.h list.h \
genf77.c f77data.c \
genj.c jdata.c \
nciter.h nciter.c \
ConvertUTF.c ConvertUTF.h

# This is the man page.
man_MANS = ncgen.1

# These files all need to be distributed.
EXTRA_DIST = ncgen.y ncgenyy.c ncgen.l $(man_MANS) internals.html

EXTRA_DIST += run_tests.sh run_nc4_tests.sh c0.cdl

# This shell script causes ncgen to build a classic and a 64-bit
# offset file from a cdl file shipped with the distribution.
TESTS = run_tests.sh 
CLEANFILES = c0.nc c0_64.nc

AM_LDFLAGS =
AM_CPPFLAGS =

if USE_NETCDF4
# If netcdf4 is built, point at it's directory. 
AM_CPPFLAGS += -I.. -I../libsrc4 -I$(top_srcdir)/libsrc4

# Link to netcdf-4 library, and the HDF5 libraries.
LDADD = ../libsrc4/libnetcdf.la
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

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

# This shell script causes ncgen to build a netcdf-4 and netcdf-4
# classic file from a cdl file shipped with the distribution.
TESTS += run_nc4_tests.sh
# The nc4 tests produce these files.
CLEANFILES += c0_4.nc c0_4c.nc

else
# ! USE_NETCDF4

AM_CPPFLAGS += -I.. -I../libsrc -I$(top_srcdir)/libsrc

LDADD = ../libsrc/libnetcdf.la

endif

LDADD += -lm

if BUILD_DAP
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

test: check

# These rule are used if someone wants to rebuild ncgenyy.c or ncgentab.c
# Otherwise never invoked, but records how to do it.
# BTW: note that renaming is essential because otherwise
# autoconf will forcibly delete files of the name *.tab.*
ncgenyy.c: ncgen.l
	flex -Pncg -8 ncgen.l
	rm -f ncgenyy.c
	mv lex.ncg.c ncgenyy.c

ncgentab.c: ncgen.y ncgenyy.c
	bison -pncg -t ncgen.y
	rm -f ncgentab.c
	mv ncgen.tab.c ncgentab.c
