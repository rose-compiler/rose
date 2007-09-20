## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer -I$(top_srcdir)/src/analyzer/astaccess  -I$(top_srcdir)/src/analyzer/astaccess/satire -I$(top_srcdir)/src/analyzer/icfgbuilder/satire

noinst_LIBRARIES = libIcfgBuilder.a
nobase_include_HEADERS = \
	satire/analysis_info.h \
        satire/cfg_support.h \
	satire/ExprLabeler.h \
	satire/IrCreation.h \
	satire/ProcTraversal.h \
	satire/AstNumberOfNodesStatistics.h \
	satire/CFGTraversal.h \
	satire/ExprTransformer.h \
        satire/testcfg.h

libIcfgBuilder_a_SOURCES = AstNumberOfNodesStatistics.C  cfg_funcs.C  cfg_support.C  CFGTraversal.C  ExprLabeler.C \
                           ExprTransformer.C  IrCreation.C  ProcTraversal.C \
			   testcfg.c




