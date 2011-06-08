# Copyright 2008, 2009 University Corporation for Atmospheric Research
#
# This file is part of the UDUNITS-2 package.  See the file LICENSE
# in the top-level source-directory of the package for copying and
# redistribution conditions.
#
## Process this file with automake to produce Makefile.in
SUBDIRS	= xmlFailures xmlSuccesses
lib_LTLIBRARIES = libudunits2.la
libudunits2_la_SOURCES = unitcore.c converter.c formatter.c \
    idToUnitMap.c idToUnitMap.h \
    unitToIdMap.c unitToIdMap.h \
    unitAndId.c unitAndId.h \
    systemMap.c systemMap.h \
    prefix.c \
    parser.y \
    status.c \
    xml.c \
    error.c \
    ut_free_system.c
BUILT_SOURCES = parser.c scanner.c
pkgdata_DATA = \
    udunits2.xml \
    udunits2-accepted.xml \
    udunits2-base.xml \
    udunits2-common.xml \
    udunits2-derived.xml \
    udunits2-prefixes.xml
info_TEXINFOS = udunits2lib.texi
#html_DATA = udunits2lib.html
#pdf_DATA = udunits2lib.pdf
AM_MAKEINFOFLAGS = -I ..
AM_MAKEINFOHTMLFLAGS = --no-split -I ..
AM_CPPFLAGS    = -DDEFAULT_UDUNITS2_XML_PATH='"$(pkgdatadir)/udunits2.xml"'

lex_prefix = ut
LEX_OUTPUT_ROOT = lex.$(lex_prefix)
LFLAGS = -d -P$(lex_prefix)

LDADD = libudunits2.la @LIBS_CUNIT@ -lm @LIBS_COVERAGE@
libudunits2_la_LIBADD = ../expat/libexpat.la
libudunits2_la_CFLAGS = @CFLAGS_COVERAGE@
include_HEADERS = udunits2.h converter.h
EXTRA_DIST = \
    scanner.l \
    scanner.c \
    udunits2.xml \
    udunits2-accepted.xml \
    udunits2-base.xml \
    udunits2-common.xml \
    udunits2-derived.xml \
    udunits2-prefixes.xml \
    udunits2lib.html \
    udunits2lib.pdf
AM_YFLAGS = -t -p ut

if HAVE_CUNIT
check_PROGRAMS = testUnits
testUnits_CPPFLAGS = @CPPFLAGS_CUNIT@
TESTS_ENVIRONMENT = UDUNITS2_XML_PATH='$(srcdir)/udunits2.xml'
TESTS = testUnits
endif

if ENABLE_UDUNITS_1
libudunits2_la_SOURCES += udunits-1.c
include_HEADERS += udunits.h
testUnits_1_CPPFLAGS = $(AM_CPPFLAGS) @CPPFLAGS_CUNIT@
if HAVE_CUNIT
check_PROGRAMS += testUnits-1
TESTS += testUnits-1
endif
endif

DISTCLEANFILES	= *.log
MOSTLYCLEANFILES = lint.log *.ln *.i core core.[0-9]* *.gcov *.gcda *.gcno
TAGS_FILES = parser.c ../expat/*.c ../expat/*.h

LINTFLAGS	= -u
LINT		= lint

SUFFIXES = .c .l

.c.ln:
	$(LINT.c) $(libudunits2_la_CPPFLAGS) $(CPPFLAGS) -c $<

.c.i:
	$(CPP) $(libudunits2_la_CPPFLAGS) $(CPPFLAGS) $< >$@

.c.gcov:
	gcov -o .libs/libudunits2_la-$<.o $<.c

if HAVE_CUNIT
debug:		testUnits
	UDUNITS2_XML_PATH='$(srcdir)/udunits2.xml' \
	    libtool --mode=execute gdb testUnits
endif

parser.o: scanner.c
