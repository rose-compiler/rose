AM_CPPFLAGS = \
	-I$(top_builddir)/src/satire \
	-I$(top_srcdir)/src/analyzer/icfgbuilder \
	-I$(top_srcdir)/src/grato

noinst_PROGRAMS = pig

pig_SOURCES = generate.h    Rule.h    spec.h   yystype.h \
              generate.c++  Rule.c++  spec.c++ pigparser.y++ pigscanner.l++ 

# dependency for parallel builds: yacc must run before lex
pigscanner.l++: pigparser.c++

SYNOBJS = ../grato/libgrato.la

pig_LDADD = $(SYNOBJS)

DISTCLEANFILES = pigparser.c++ pigparser.h pigscanner.c++ y.vcg

AM_YFLAGS = -g -d --name-prefix=pig
AM_LFLAGS = -Ppig -olex.yy.c

$(SYNOBJS):
	$(MAKE) -C ../grato

