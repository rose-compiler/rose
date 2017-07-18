
noinst_LTLIBRARIES = libpoet.la
libpoet_la_SOURCES=poet_AST.cpp codegen.cpp  parse.cpp evalOp.cpp driver.cpp poet_yacc.c poet_lex.c timing.cpp AutoTuningInterface.cpp eval.cpp eval_ext.cpp tokenize.cpp 

EXTRA_DIST = poet_yacc.y poet_lex.l poet_ASTfactory.h poet_SymbolTable.h poet_AST.h poet_ASTvisitor.h poet_error.h poet_config.h poet_ASTeval.h poet_ASTinterface.h timing.h poet_yacc.h lib/ExpStmt.incl lib/Loops.incl lib/omp.incl lib/utils.incl

AM_YFLAGS=-d -v

#Faizur:(07/21/2011): For CMAKE the POET_VERSION is defined in src/CMakeLists.txt
INCLUDES = -DPOET_VERSION="\"1.03.13\"" -DPOET_LIB_DIR=\"$(DESTDIR)/$(libdir)/\" -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/driver -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/slicing -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/computation -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/depGraph -I$(top_srcdir)/src/midend/programTransformation/loopProcessing/depInfo -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/frontend/SageIII

