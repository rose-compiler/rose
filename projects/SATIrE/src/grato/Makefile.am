noinst_LTLIBRARIES = libgrato.la

SYNSCANNER = synscanner
SYNPARSER = synparser
FRONTENDSOURCEFILES = $(SYNSCANNER).C $(SYNPARSER).C
IRSOURCEFILES = GrammarIr.C
BACKENDSOURCEFILES = GBackEnd.C GTextBackEnd.C GDotBackEnd.C GCocoBackEnd.C

GRATOHEADERS = GrammarIr.h GBackEnd.h GTextBackEnd.h GTextTransformation.h GDotBackEnd.h GProperty.h GSequenceVisitor.h GCocoBackEnd.h 

noinst_HEADERS = $(GRATOHEADERS) parser_decls.h setalgorithms.h
libgrato_la_SOURCES = $(IRSOURCEFILES) $(BACKENDSOURCEFILES) synparser.y++ synscanner.l++

AM_YFLAGS = -g -d --name-prefix=syn
AM_LFLAGS = -Psyn -olex.yy.c

DISTCLEANFILES = synparser.c++ synparser.h synscanner.c++ y.vcg

bin_PROGRAMS = grato
grato_SOURCES = GTextTransformation.C GProperty.C GSequenceVisitor.C grato.C
grato_LDADD = libgrato.la
grato_LDFLAGS = 
