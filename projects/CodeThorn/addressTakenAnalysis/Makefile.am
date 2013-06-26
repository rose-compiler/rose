include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
CODETHORN_SRC_ROOT = $(top_srcdir)/projects/CodeThorn/src/

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

noinst_PROGRAMS = addressTakenAnalysis

INCLUDES = \
	$(ROSE_INCLUDES) \
	-I$(srcdir) \
	-I$(CODETHORN_SRC_ROOT)

BUILT_SOURCES = matcherparser.h
AM_YFLAGS = -d -p matcherparser
AM_LFLAGS = -Pmatcherparser -o$(LEX_OUTPUT_ROOT).c

addressTakenAnalysis_SOURCES = \
	$(CODETHORN_SRC_ROOT)/matcherparser.yy \
	$(CODETHORN_SRC_ROOT)/matcherlexer.ll \
	$(CODETHORN_SRC_ROOT)/Timer.cpp \
	$(CODETHORN_SRC_ROOT)/RoseAst.C \
	$(CODETHORN_SRC_ROOT)/AstTerm.C \
	$(CODETHORN_SRC_ROOT)/AstMatching.C \
	$(CODETHORN_SRC_ROOT)/MatchOperation.C \
	$(CODETHORN_SRC_ROOT)/SgNodeHelper.C \
	$(CODETHORN_SRC_ROOT)/VariableIdMapping.C \
	$(CODETHORN_SRC_ROOT)/Miscellaneous.C \
	$(CODETHORN_SRC_ROOT)/CommandLineOptions.C \
	addressTakenAnalysis.C

addressTakenAnalysis_LDADD = -lrose

clean-local:
	rm -f matcher*
