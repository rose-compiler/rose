include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =

libroseDisassemblers_la_SOURCES = Disassembler.C DisassemblerX86.C Partitioner.C x86Disassembler.C

include_HEADERS = Disassembler.h DisassemblerX86.h Partitioner.h

dist_include_HEADERS =
