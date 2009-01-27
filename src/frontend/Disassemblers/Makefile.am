include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =
libroseDisassemblers_la_SOURCES = x86Disassembler.C armDisassembler.C powerpcDisassembler.C putInstructionsIntoBasicBlocks.C disassemblers.h disassemblerCommon.C detectFunctionStarts.C

dist_include_HEADERS =

include_HEADERS =  disassemblers.h
