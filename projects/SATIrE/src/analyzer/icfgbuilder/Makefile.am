## Process this file with automake to create Makefile.in.

AM_CPPFLAGS =

# Black magic: There is a header file naming conflict between ROSE and PAG,
# both have a "general.h" header. SATIrE won't build with some versions of
# PAG unless the PAG headers have priority in this directory.
if WITH_PAG
AM_CPPFLAGS += -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

if WITH_SWI_PROLOG
AM_CPPFLAGS += -I$(SWI_PROLOG_INCLUDE_DIR)
endif

AM_CPPFLAGS += -I$(ROSE_INCLUDE_DIR) \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(top_builddir)/src/satire \
	-I$(top_srcdir)/src/analyzer/astaccess \
	-I$(top_srcdir)/src/analyzer/astaccess/satire \
	-I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
	-I$(top_srcdir)/src/analyzer/astnumbering/satire \
	-I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
	-I$(top_srcdir)/src/analyzer/provided/pointsto/satire \
    -I$(top_srcdir)/src/analyzer/provided/aslan/satire \
	-I$(top_srcdir)/src/analyzer/support/satire \
	-I$(top_srcdir)/src/analyzer/annotation/satire \
    -I$(top_srcdir)/src/aral/src/satire \
	-I$(top_srcdir)/src/termite/satire \
	-I$(top_builddir)/src/termite/satire

noinst_LTLIBRARIES = libIcfgBuilder.la
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

libIcfgBuilder_la_SOURCES = \
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
PAG_PATH = $(PAG_INCLUDE_DIR)/..

CFGOBJS = $(addsuffix _cfg.lo, $(shell cat $(PAG_PATH)/share/pag/cfg/files))

CFG = $(addsuffix _cfg.lo, $(shell cat $(PAG_PATH)/share/pag/cfg/files))

libIcfgBuilder_la_LIBADD = $(CFGOBJS)

AM_CFLAGS = -Wno-unused

%_cfg.c: $(PAG_PATH)/share/pag/cfg/%.c
	cp $< $@

endif
