## Process this file with automake to create Makefile.in.

AM_CPPFLAGS = -I$(ROSE_INCLUDE_DIR) \
    -I$(BOOST_INCLUDE_DIR) \
    -I$(top_builddir)/src/satire \
    -I$(top_srcdir)/src/analyzer/astaccess  \
    -I$(top_srcdir)/src/analyzer/astaccess/satire \
    -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
    -I$(top_srcdir)/src/analyzer/astnumbering/satire \
    -I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
    -I$(top_srcdir)/src/analyzer/support/satire \
    -I$(top_srcdir)/src/analyzer/annotation/satire \
    -I$(top_srcdir)/src/analyzer/provided/pointsto/satire \
    -I$(top_srcdir)/src/analyzer/provided/aslan/satire \
    -I$(top_srcdir)/src/termite/satire \
    -I$(top_builddir)/src/termite/satire \
    -I$(top_srcdir)/src/aral/src/satire

bin_PROGRAMS = aral aralexample1
lib_LTLIBRARIES = libaral.la
nobase_include_HEADERS = satire/aralir.h satire/aralparser_decls.h

aral_SOURCES = aral.C
aral_LDADD = libaral.la

aralexample1_SOURCES = aralexample1.C
aralexample1_LDADD = libaral.la

BUILT_SOURCES = aralparser.h

AM_YFLAGS = -d -p aral
AM_LFLAGS = -Paral
LEX_OUTPUT_ROOT = lex.aral

libaral_la_SOURCES = aralscanner.lxx aralparser.yxx aralir.C
