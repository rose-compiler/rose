AM_CPPFLAGS = -I$(ROSE_INCLUDE_DIR) \
	-I$(BOOSTDIR)/include \
	-I$(top_builddir)/src/satire \
	-I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
	-I$(top_srcdir)/src/analyzer/astaccess/satire \
	-I$(top_srcdir)/src/analyzer/astnumbering/satire \
	-I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
	-I$(top_srcdir)/src/analyzer/support/satire \
	-I$(top_srcdir)/src/analyzer/annotation/satire \
	-I$(top_srcdir)/src/analyzer/provided/pointsto/satire \
    -I$(top_srcdir)/src/analyzer/provided/aslan/satire \
    -I$(top_srcdir)/src/aral/src/satire \
	-I$(top_srcdir)/src/termite/satire \
	-I$(top_builddir)/src/termite/satire

if WITH_PAG
AM_CPPFLAGS += -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

if WITH_SWI_PROLOG
AM_CPPFLAGS += -I$(SWI_PROLOG_INCLUDE_DIR)
endif

noinst_LTLIBRARIES = libPointsTo.la
nobase_include_HEADERS = satire/pointsto.h satire/o_Location.h
libPointsTo_la_SOURCES = pointsto.C pointsto_implementation.C o_Location.C

EXTRA_DIST = pointsto_implementation.h
