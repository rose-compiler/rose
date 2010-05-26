include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =

libroseDisassemblers_la_SOURCES = \
	Partitioner.C IPDParser.C \
        Disassembler.C DisassemblerArm.C DisassemblerPowerpc.C DisassemblerX86.C \
	Assembler.C AssemblerX86.C AssemblerX86Init.C \
	AssemblerX86Init1.C AssemblerX86Init2.C AssemblerX86Init3.C AssemblerX86Init4.C AssemblerX86Init5.C \
	AssemblerX86Init6.C AssemblerX86Init7.C AssemblerX86Init8.C AssemblerX86Init9.C

include_HEADERS = \
	Partitioner.h \
	Disassembler.h DisassemblerArm.h DisassemblerPowerpc.h DisassemblerX86.h \
	Assembler.h AssemblerX86.h AssemblerX86Init.h 

dist_include_HEADERS =

EXTRA_DIST = CMakeLists.txt
