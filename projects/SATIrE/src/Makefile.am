## Process this file with automake to create Makefile.in.

SUBDIRS = grato pig analyzer termite

#libsatire_a_LIBADD = icfgbuilder/libIcfgBuilder.a commandlineinterface/libCommandLineInterface.a astaccess/libAstAccess.a

# GB (2008-05-16): Added a header file to wrap <rose.h>
nobase_include_HEADERS = satire/satire_rose.h

#libsatire_a_SOURCES =

#lib_LIBRARIES = libsatire.a

#libsatire.a: analyzer/icfgbuilder/libIcfgBuilder.a  analyzer/commandlineinterface/libCommandLineInterface.a  analyzer/astaccess/libAstAccess.a  analyzer/astnumbering/libAstNumbering.a  analyzer/support/libAnalysisSupport.a  analyzer/provided/pointsto/libPointsTo.a
#	sh $(top_srcdir)/src/buildsatirelib

lib_LTLIBRARIES = libsatire.la

libsatire_la_SOURCES = satire.c
libsatire_la_LIBADD = analyzer/icfgbuilder/libIcfgBuilder.la \
	              analyzer/commandlineinterface/libCommandLineInterface.la \
                      analyzer/astaccess/libAstAccess.la \
                      analyzer/astnumbering/libAstNumbering.la \
                      analyzer/support/libAnalysisSupport.la \
                      analyzer/provided/pointsto/libPointsTo.la

EXTRA_DIST = buildsatirelib
