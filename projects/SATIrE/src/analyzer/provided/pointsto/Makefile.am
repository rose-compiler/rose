INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(BOOSTDIR)/include -I$(top_srcdir)/src/satire -I$(top_srcdir)/src/analyzer/icfgbuilder/satire -I$(top_srcdir)/src/analyzer/astaccess/satire -I$(top_srcdir)/src/analyzer/astnumbering/satire -I$(top_srcdir)/src/analyzer/commandlineinterface/satire -I$(top_srcdir)/src/analyzer/support/satire -I$(top_srcdir)/src/analyzer/annotation/satire -I$(top_srcdir)/src/analyzer/provided/pointsto/satire

if WITH_PAG
INCLUDES += -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

noinst_LIBRARIES = libPointsTo.a
nobase_include_HEADERS = satire/pointsto.h satire/o_Location.h
libPointsTo_a_SOURCES = pointsto.C pointsto_implementation.C o_Location.C

EXTRA_DIST = pointsto_implementation.h
