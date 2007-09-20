## Process this file with automake to create Makefile.in.

SUBDIRS = grato pig analyzer termite

#libsatire_a_LIBADD = icfgbuilder/libIcfgBuilder.a commandlineinterface/libCommandLineInterface.a astaccess/libAstAccess.a

libsatire_a_SOURCES =

lib_LIBRARIES = libsatire.a

libsatire.a: analyzer/icfgbuilder/libIcfgBuilder.a  analyzer/commandlineinterface/libCommandLineInterface.a  analyzer/astaccess/libAstAccess.a
	sh $(top_srcdir)/src/buildsatirelib

EXTRA_DIST = buildsatirelib
