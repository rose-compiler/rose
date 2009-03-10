## Process this file with automake to create Makefile.in.

SUBDIRS = grato pig analyzer termite

nobase_include_HEADERS = satire/satire_rose.h

lib_LTLIBRARIES = libsatire.la

libsatire_la_SOURCES = satire.c
libsatire_la_LIBADD = analyzer/icfgbuilder/libIcfgBuilder.la \
	              analyzer/commandlineinterface/libCommandLineInterface.la \
                      analyzer/astaccess/libAstAccess.la \
                      analyzer/astnumbering/libAstNumbering.la \
                      analyzer/support/libAnalysisSupport.la \
                      analyzer/provided/pointsto/libPointsTo.la

EXTRA_DIST = buildsatirelib
