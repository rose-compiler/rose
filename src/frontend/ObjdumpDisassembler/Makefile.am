include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/BinaryDisassembly

if ROSE_USE_BINARY_ANALYSIS_SUPPORT

if DEV_LIBS
lib_LTLIBRARIES = libroseObjdumpDisassembler.la
else
# This will cause the dynamic library to be built (as libroseObjdumpDisassembler.so)
noinst_LTLIBRARIES = libroseObjdumpDisassembler.la
endif

#noinst_PROGRAMS = objdumpTest
#objdumpTest_SOURCES = objdumpTest.C 
#objdumpTest_LDADD = -L../BinaryDisassembly/ -lbinaryFrontend -lrose -lrt -lfl

libroseObjdumpDisassembler_la_DEPENDENCIES =
libroseObjdumpDisassembler_la_SOURCES = objdumpToRoseBinaryAst.C 

else
# DQ (10/21/2007): build an empty lib file so that ROSE/src/Makefile.am need not be conditional on Binary Analysis.
noinst_LTLIBRARIES = libroseObjdumpDisassembler.la
endif

include_HEADERS = objdumpToRoseBinaryAst.h

dist_libexec_SCRIPTS = asmToRoseAst.tcl

# objdumpTest: objdumpTest.C objdumpToRoseBinaryAst.C
#	g++ -g -o objdumpTest objdumpTest.C objdumpToRoseBinaryAst.C -I/export/0/tmp.willcock2/dec4/install/include -L/export/0/tmp.willcock2/dec4/install/lib -lrose -lrt -lfl -lbinarydisassembly -lrose /usr/lib/mysql/libmysqlclient_r.so.14

