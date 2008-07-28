include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ROSE Outlinging sources

# include ../Make.inc

SUBDIRS = ASTtools CPreproc Preprocess Transform

noinst_LTLIBRARIES = libliaOutliner.la

libliaOutlinerdir = $(liaOutlinerlibdir)

libliaOutliner_la_SOURCES = \
	Check.cc \
	LiaOutliner.cc \
	NameGenerator.cc \
	PragmaInterface.cc

libliaOutliner_la_LIBADD = \
	ASTtools/libASTtools.la \
	CPreproc/libCPreproc.la \
	Preprocess/libPreprocess.la \
	Transform/libTransform.la

liaOutlinerinc_HEADERS = \
	LiaOutliner.hh \
	NameGenerator.hh

# eof
