## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This automake file generates the Makefile to build netCDF-4. The
# generated makefile is not run unless the user selected to build
# netCDF-4.

# $Id: Makefile.am,v 1.31 2010/04/10 20:56:32 dmh Exp $

SUBDIRS=oc

if USE_NETCDF4
AM_CPPFLAGS = -I$(top_srcdir)/libsrc4
AM_CPPFLAGS += -I$(top_srcdir)/libsrc
else
AM_CPPFLAGS = -I$(top_srcdir)/libsrc
endif

AM_LDFLAGS = 

CURL_CFLAGS = @CURL_CFLAGS@

# DRNO Sources
SRC=\
ncdap3.c \
constraints3.c \
getvara3.c \
common34.c \
dapcvt.c \
dapalign.c \
dapodom.c \
ncdaperr.c \
cdf3.c \
dapdispatch3.c \
dispatch3.c \
dapdump.c \
dapdebug.c \
daputil.c \
dapattr3.c \
cetab.c \
ceparse.c \
celex.c \
nchashmap.c nclist.c ncbytes.c

HDRS=\
constraints3.h \
ncdap.h \
ncdap3.h \
daprename.h \
dapalign.h \
dapodom.h \
getvara.h \
dapnc.h \
daputil.h \
dispatch3.h \
dispatchdefs3.h \
dapdispatch3.h \
dapdebug.h \
dapdump.h \
netcdf3l.h \
ceparselex.h \
cetab.h \
nchashmap.h nclist.h ncbytes.h

if BUILD_DAP

# Build convenience library

noinst_LTLIBRARIES = libncdap3.la
libncdap3_la_SOURCES = $(SRC) $(HDRS)
libncdap3_la_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
libncdap3_la_LIBADD =
AM_LDFLAGS += $(CURL_LIBS)

if INTERNAL_OCLIB
  AM_CPPFLAGS += -I${top_srcdir}/libncdap3/oc
  libncdap3_la_LIBADD += ./oc/liboc.la
else
  AM_LDFLAGS += -L@OCLIBDIR@/lib
  libncdap3_la_LIBADD += -loc
endif # INTERNAL_OCLIB

endif # BUILD_DAP

# Clean up. 
CLEANFILES =
DISTCLEANFILES =

# These rule are used if someone wants to rebuild the grammar files.
# Otherwise never invoked, but records how to do it.
# BTW: note that renaming is essential because otherwise
# autoconf will forcibly delete files of the name *.tab.*
cetab.c cetab.h: oc/ce.y
	bison -d -t -p ce oc/ce.y
	rm -f cetab.c cetab.h
	mv ce.tab.c cetab.c
	mv ce.tab.h cetab.h

test: check

