
bin_PROGRAMS = pcg
pcg_SOURCES=  poet_AST.cpp main.cpp codegen.cpp parse.cpp tokenize.cpp eval.cpp eval_ext.cpp evalOp.cpp driver.cpp timing.cpp poet_yacc.y poet_lex.l 

noinst_LIBRARIES = libpoet.a
libpoet_a_SOURCES=poet_AST.cpp codegen.cpp parse.cpp eval.cpp eval_ext.cpp evalOp.cpp driver.cpp tokenize.cpp timing.cpp poet_yacc.c poet_lex.c

EXTRA_DIST = poet_ASTfactory.h poet_SymbolTable.h poet_AST.h poet_ASTinterface.h poet_ASTvisitor.h poet_error.h poet_config.h poet_ASTeval.h timing.h

files = $(EXTRA_DIST) $(pcg_SOURCES)

AM_YFLAGS=-d -v
AM_CPPFLAGS = -DPOET_LIB_DIR=\"$(abs_top_srcdir)/lib\" -DPOET_DEST_DIR=\"$(DESTDIR)/$(libdir)/\"
INCLUDES = -DVERSION="\"1.03.06\"" -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/driver -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/slicing -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/computation -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/depGraph -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/depInfo -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs

include  $(top_srcdir)/src/3rdPartyLibraries/POET/Make.inc
rosedir= $(rose_srcdir)/src/3rdPartyLibraries/POET
