include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/BinaryDisassembly -I/home/panas2/development/ROSE-64bit/NEW_ROSE/src/frontend/BinaryDisassembly/mysql_include

if ROSE_USE_BINARY_ANALYSIS_SUPPORT

if DEV_LIBS
lib_LTLIBRARIES = libroseX86Disassembler.la
else
# This will cause the dynamic library to be built (as libroseObjdumpDisassembler.so)
noinst_LTLIBRARIES = libroseX86Disassembler.la
endif

libroseX86Disassembler_la_DEPENDENCIES =
libroseX86Disassembler_la_SOURCES = x86Disassembler.C x86Disassembler.h

dist_include_HEADERS = x86Disassembler.h

else
# DQ (10/21/2007): build an empty lib file so that ROSE/src/Makefile.am need not be conditional on Binary Analysis.
libroseX86Disassembler_la_SOURCES = dummy_x86Disassembler.C
noinst_LTLIBRARIES = libroseX86Disassembler.la
endif

include_HEADERS = 
