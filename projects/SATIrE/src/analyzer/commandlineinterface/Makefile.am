## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer -I$(top_srcdir)/src/analyzer/astaccess -I$(top_srcdir)/src/analyzer/icfgbuilder/satire -I$(top_srcdir)/src/analyzer/commandlineinterface/satire -I$(top_srcdir)/src/analyzer/astaccess -I$(top_srcdir)/src/analyzer/astaccess/satire

noinst_LIBRARIES = libCommandLineInterface.a
nobase_include_HEADERS = satire/AnalyzerOptions.h  satire/CommandLineParser.h  satire/set_pag_options.h  satire/attributes

libCommandLineInterface_a_SOURCES = AnalyzerOptions.C  CommandLineParser.C  set_pag_options.C

