## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(top_srcdir)/src/satire -I$(top_srcdir)/src/analyzer/astaccess -I$(top_srcdir)/src/analyzer/icfgbuilder/satire -I$(top_srcdir)/src/analyzer/commandlineinterface/satire -I$(top_srcdir)/src/analyzer/astaccess -I$(top_srcdir)/src/analyzer/astaccess/satire -I$(top_srcdir)/src/analyzer/support/satire -I$(top_srcdir)/src/termite/satire -I$(top_srcdir)/src/analyzer/astnumbering/satire -I$(top_srcdir)/src/analyzer/annotation/satire

if WITH_PAG
INCLUDES += -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

if WITH_SWI_PROLOG
INCLUDES += -I$(SWI_PROLOG_INCLUDE_DIR)
endif

noinst_LTLIBRARIES = libCommandLineInterface.la
nobase_include_HEADERS = satire/AnalyzerOptions.h  satire/CommandLineParser.h  satire/set_pag_options.h  satire/attributes

libCommandLineInterface_la_SOURCES = AnalyzerOptions.C  CommandLineParser.C  set_pag_options.C

