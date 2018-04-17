## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This automake file generates the Makefile to build netCDF-4. The
# generated makefile is not run unless the user selected to build
# netCDF-4.

# $Id: Makefile.am,v 1.8 2009/12/05 20:46:28 dmh Exp $

AM_CPPFLAGS = -I$(top_srcdir)/libncdap3 -I$(top_srcdir)/libncdap3/oc \
	      -I$(top_srcdir)/libsrc4 \
	      -I$(top_srcdir)/libsrc

AM_CPPFLAGS += -I@HDF5DIR@/include -I@ZLIBDIR@/include
AM_LDFLAGS = -L@HDF5DIR@/lib -L@ZLIBDIR@/lib

# If the user specified a prefix for the HDF4 library location, use it.
if USE_HDF4_DIR
AM_CPPFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
endif

if USE_ZLIB_DIR
AM_CPPFLAGS += -I@ZLIBDIR@/include
AM_LDFLAGS += -L@ZLIBDIR@/lib
endif

if USE_SZLIB_DIR
AM_CPPFLAGS += -I@SZLIBDIR@/include
AM_LDFLAGS += -L@SZLIBDIR@/lib
endif

if USE_PNETCDF_DIR
AM_CPPFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

CURL_CFLAGS = @CURL_CFLAGS@

# DRNO Sources
SRC=\
ncdap4.c \
cdf4.c \
getvara4.c \
constraints4.c \
dispatch4.c

HDRS=\
ncdap4.h \
constraints4.h \
dispatch4.h \
dapdispatch4.h

if BUILD_DAP

if USE_NETCDF4

# create convenience library
libncdap4_la_SOURCES = $(SRC) $(HDRS)
libncdap4_la_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
AM_LDFLAGS += $(CURL_LIBS)

noinst_LTLIBRARIES = libncdap4.la

#noinst_PROGRAMS = ncd
#ncd_SOURCES = ncdump.c vardata.c dumplib.c indent.c nctime.c ncdump.h vardata.h dumplib.h indent.h isnan.h nctime.h cdl.h
#ncd_LDADD = libnetcdf.la -lm -lhdf5_hl -lhdf5

endif # USE_NETCDF4
endif # BUILD_DAP

# Clean up.
CLEANFILES =
DISTCLEANFILES =

test: check


