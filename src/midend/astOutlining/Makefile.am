include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastOutliner.la

libastOutliner_la_SOURCES = \
	Check.cc \
	Outliner.cc \
	NameGenerator.cc \
	PragmaInterface.cc \
	ASTtools.cc \
	Copy.cc \
	Jumps.cc \
	PrePostTraversal.cc \
	PreprocessingInfo.cc \
	StmtRewrite.cc \
	This.cc \
	VarSym.cc \
	Case.cc \
	ExtractIfs.cc \
	If.cc \
	IfDirectiveContextFinder.cc \
	IfDirectiveExtractor.cc \
	Block.cc \
	NonLocalDecls.cc \
	PreprocIfs.cc \
	NonLocalControlFlow.cc \
	Preprocess.cc \
	ThisExprs.cc \
	CollectVars.cc \
	GenerateCall.cc \
	GenerateFunc.cc \
	Insert.cc \
	Transform.cc

include_HEADERS = \
	Outliner.hh \
	NameGenerator.hh \
	ASTtools.hh \
	Copy.hh \
	Jumps.hh \
	PrePostTraversal.hh \
	PreprocessingInfo.hh \
	StmtRewrite.hh \
	This.hh \
	VarSym.hh \
	IfDirectiveContextFinder.hh \
	IfDirectiveExtractor.hh \
	If.hh \
        Preprocess.hh \
        Transform.hh

clean-local:
	rm -rf Templates.DB ii_files ti_files core

EXTRA_DIST = README

