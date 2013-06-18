include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
CODETHORN_SRC_ROOT = $(top_srcdir)/projects/CodeThorn/src/

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

noinst_LTLIBRARIES = libCodeThornAnalysis.la
noinst_PROGRAMS = addressTakenAnalysis

# SA: making a shared library to link with flow insensitive address taken analysis
libCodeThornAnalysis_la_SOURCES = \
	$(CODETHORN_SRC_ROOT)/matcherparser.yy \
	$(CODETHORN_SRC_ROOT)/matcherlexer.ll \
	$(CODETHORN_SRC_ROOT)/Timer.cpp \
	$(CODETHORN_SRC_ROOT)/RoseAst.C \
	$(CODETHORN_SRC_ROOT)/AstTerm.C \
	$(CODETHORN_SRC_ROOT)/AstMatching.C \
	$(CODETHORN_SRC_ROOT)/MatchOperation.C 
	$(CODETHORN_SRC_ROOT)/SgNodeHelper.C \
	$(CODETHORN_SRC_ROOT)/VariableIdMapping.C

INCLUDES = \
	$(ROSE_INCLUDES) \
	-I$(srcdir) \
	-I$(CODETHORN_SRC_ROOT)

#libCodeThornAnalysis_la_CXXFLAGS = $(AM_CXXFLAGS)
BUILT_SOURCES = matcherparser.h
AM_YFLAGS = -d -p matcherparser
AM_LFLAGS = -Pmatcherparser -o$(LEX_OUTPUT_ROOT).c

addressTakenAnalysis_LDADD = -lrose -lCodeThornAnalysis

addressTakenAnalysis_SOURCES = \
	addressTakenAnalysis.C
