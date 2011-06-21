
noinst_LTLIBRARIES = libpoet.la
libpoet_la_SOURCES=poetAST.cpp codegen.cpp  parse.cpp evalAST.cpp evalOp.cpp driver.cpp poet_yacc.c poet_lex.c timing.cpp AutoTuningInterface.cpp

EXTRA_DIST = poet_yacc.y poet_lex.l ASTfactory.h SymbolTable.h poetAST.h ASTvisitor.h error_config.h poet_config.h ASTeval.h timing.h poet_yacc.h

AM_YFLAGS=-d -v

#Faizur:(07/21/2011): For CMAKE the POET_VERSION is defined in src/CMakeLists.txt
INCLUDES = -DPOET_VERSION="\"1.02.06.10\"" -DPOET_LIB_DIR=\"$(DESTDIR)/$(libdir)/\" -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/driver -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/slicing -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/computation -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/depGraph -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/depInfo -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs

