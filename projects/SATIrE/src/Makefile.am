## Process this file with automake to create Makefile.in.

SUBDIRS = grato pig analyzer termite

nobase_include_HEADERS = satire/satire_rose.h

lib_LTLIBRARIES = libsatire.la

libsatire_la_SOURCES = libsatire.c
libsatire_la_LIBADD = analyzer/icfgbuilder/libIcfgBuilder.la \
	              analyzer/commandlineinterface/libCommandLineInterface.la \
                      analyzer/astaccess/libAstAccess.la \
                      analyzer/astnumbering/libAstNumbering.la \
                      analyzer/support/libAnalysisSupport.la \
                      analyzer/provided/pointsto/libPointsTo.la

INCLUDES = -I$(top_srcdir)/src/satire \
       -I$(top_srcdir)/src/termite/satire \
       -I$(top_builddir)/src/termite/satire \
       -I$(top_srcdir)/src/analyzer/annotation/satire \
       -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
       -I$(top_srcdir)/src/analyzer/astaccess \
       -I$(top_srcdir)/src/analyzer/astaccess/satire \
       -I$(top_srcdir)/src/analyzer/astnumbering/satire \
       -I$(top_srcdir)/src/analyzer/annotation/satire \
       -I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
       -I$(top_srcdir)/src/analyzer/provided/pointsto/satire \
       -I$(top_srcdir)/src/analyzer/support \
       -I$(top_srcdir)/src/analyzer/support/satire \
       -I$(ROSE_INCLUDE_DIR) \
       -I$(BOOST_INCLUDE_DIR)

if WITH_PAG
INCLUDES += -I$(PAG_INCLUDE_DIR)/pag \
            -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

if WITH_SWI_PROLOG
INCLUDES += -I$(SWI_PROLOG_INCLUDE_DIR)
endif

binprograms_ldadd = \
	libsatire.la \
	analyzer/provided/dataflow/libsatiredfa.la \
	analyzer/astaccess/satire/libsyntree.la \
	termite/libtermsupport.la \
	analyzer/astaccess/libpagdatatypes.la \
	-lrose
if WITH_PAG
binprograms_ldadd += -lpag
endif
if WITH_SWI_PROLOG
binprograms_ldadd += $(SWI_PROLOG_LIBS) $(SWI_PROLOG_LDADD)
endif
binprograms_ldflags = \
    -L$(ROSE_LIB_DIR) -Wl,-rpath $(ROSE_LIB_DIR) \
    -L$(BOOST_LIB_DIR) -Wl,-rpath $(BOOST_LIB_DIR)
if WITH_PAG
binprograms_ldflags += -L$(PAG_LIB_DIR) -Wl,-rpath $(PAG_LIB_DIR)
endif

bin_PROGRAMS = satire_driver
satire_driver_SOURCES = $(top_srcdir)/src/analyzer/support/satire_driver.C
satire_driver_LDADD = $(binprograms_ldadd)
satire_driver_LDFLAGS = $(binprograms_ldflags)

if WITH_PAG
bin_PROGRAMS += ais_annotator
ais_annotator_SOURCES = \
			$(top_srcdir)/src/analyzer/provided/ais_annotator/ais_annotator.C
ais_annotator_LDADD = $(binprograms_ldadd)
ais_annotator_LDFLAGS = $(binprograms_ldflags)
endif
