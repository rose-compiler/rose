## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(BOOST_INCLUDE_DIR) -I$(top_srcdir)/src/satire -I$(top_srcdir)/src/analyzer/astaccess  -I$(top_srcdir)/src/analyzer/astaccess/satire -I$(top_srcdir)/src/analyzer/icfgbuilder/satire -I$(top_srcdir)/src/analyzer/astnumbering/satire -I$(top_srcdir)/src/analyzer/commandlineinterface/satire -I$(top_srcdir)/src/analyzer/provided/pointsto/satire -I$(top_srcdir)/src/analyzer/support/satire -I$(top_srcdir)/src/analyzer/annotation/satire

if WITH_PAG
INCLUDES += -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

noinst_LIBRARIES = libIcfgBuilder.a
nobase_include_HEADERS = \
	satire/analysis_info.h \
        satire/cfg_support.h \
	satire/kfg_types.h \
	satire/ExprLabeler.h \
	satire/IrCreation.h \
	satire/ProcTraversal.h \
	satire/AstNumberOfNodesStatistics.h \
	satire/CFGTraversal.h \
	satire/ExprTransformer.h \
	satire/Context.h \
        satire/testcfg.h

libIcfgBuilder_a_SOURCES = \
	AstNumberOfNodesStatistics.C \
	cfg_funcs.C \
	cfg_support.C \
	CFGTraversal.C \
	ExprLabeler.C \
	ExprTransformer.C \
	Context.C \
	IrCreation.C \
	ProcTraversal.C \
	testcfg.c

if WITH_PAG
libIcfgBuilder_a_LIBADD = $(CFGOBJS)

PAG_PATH = $(PAG_INCLUDE_DIR)/..

CFGOBJS = $(addsuffix _cfg.o, $(shell cat $(PAG_PATH)/share/pag/cfg/files))

%_cfg.o: $(PAG_PATH)/share/pag/cfg/%.c
	$(COMPILE) -Wno-unused -c $< -o $@
endif
