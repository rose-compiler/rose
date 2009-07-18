include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =

libroseDisassemblers_la_SOURCES = \
	Partitioner.C \
        Disassembler.C DisassemblerArm.C DisassemblerPowerpc.C DisassemblerX86.C \
	Assembler.C AssemblerX86.C AssemblerX86Init.C

include_HEADERS = \
	Partitioner.h \
	Disassembler.h DisassemblerArm.h DisassemblerPowerpc.h DisassemblerX86.h \
	Assembler.h AssemblerX86.h AssemblerX86Init.h 

dist_include_HEADERS =
