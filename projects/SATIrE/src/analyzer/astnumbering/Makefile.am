## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(BOOST_INCLUDE_DIR) -I$(top_srcdir)/src/satire -I$(top_srcdir)/src/analyzer/astnumbering -I$(top_srcdir)/src/analyzer/astnumbering/satire -I$(top_srcdir)/src/analyzer/commandlineinterface/satire

noinst_LTLIBRARIES = libAstNumbering.la
nobase_include_HEADERS = satire/EqualityTraversal.h

libAstNumbering_la_SOURCES = EqualityTraversal.C

EXTRA_DIST = ValueEqualityCases ValueHashCases ValueStringCases



