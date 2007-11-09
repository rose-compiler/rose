noinst_LIBRARIES = libgrato.a

SYNSCANNER = synscanner
SYNPARSER = synparser
FRONTENDSOURCEFILES = $(SYNSCANNER).C $(SYNPARSER).C
IRSOURCEFILES = GrammarIr.C
BACKENDSOURCEFILES = GBackEnd.C GTextBackEnd.C GDotBackEnd.C GCocoBackEnd.C

GRATOHEADERS = GrammarIr.h GBackEnd.h GTextBackEnd.h GTextTransformation.h GDotBackEnd.h GProperty.h GSequenceVisitor.h GCocoBackEnd.h 

noinst_HEADERS = $(GRATOHEADERS) parser_decls.h setalgorithms.h
libgrato_a_SOURCES = $(IRSOURCEFILES) $(BACKENDSOURCEFILES) synparser.y++ synscanner.l++

AM_YFLAGS = -g -d --name-prefix=syn
AM_LFLAGS = -Psyn -olex.yy.c

bin_PROGRAMS = grato
grato_SOURCES = GTextTransformation.C GProperty.C GSequenceVisitor.C grato.C
grato_LDADD = libgrato.a
grato_LDFLAGS = 
