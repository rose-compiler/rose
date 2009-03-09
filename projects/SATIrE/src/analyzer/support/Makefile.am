## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(top_srcdir)/src/satire \
	-I$(top_srcdir)/src/analyzer/astaccess  \
	-I$(top_srcdir)/src/analyzer/astaccess/satire \
	-I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
	-I$(top_srcdir)/src/analyzer/astnumbering/satire \
	-I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
	-I$(top_srcdir)/src/analyzer/support/satire \
	-I$(top_srcdir)/src/analyzer/annotation/satire \
	-I$(top_srcdir)/src/termite/satire \
	-I$(top_builddir)/src/termite/satire

if WITH_SWI_PROLOG
  INCLUDES += -I$(SWI_PROLOG_INCLUDE_DIR) 
endif

if WITH_PAG
  INCLUDES += -I$(PAG_INCLUDE_DIR)/pag \
			  -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif


noinst_LTLIBRARIES = libAnalysisSupport.la
nobase_include_HEADERS = \
    satire/satire_analysis_driver.h     satire/satire.h \
	satire/satire_analysis.h            satire/satire_program_representation.h\
	satire/satire_analysis_scheduler.h  satire/satire_utils.h \
	satire/satire_dataflow_analysis.h \
	satire/PagDfiTextPrinter.h          satire/PagDfiCommentAnnotator.h

libAnalysisSupport_la_SOURCES = \
    satire_analysis.C            satire_dataflow_analysis.C \
	satire_analysis_driver.C     satire_program_representation.C \
	satire_analysis_scheduler.C  satire_utils.C
