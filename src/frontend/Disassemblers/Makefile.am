include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =

libroseDisassemblers_la_SOURCES = 					\
	Assembler.C AssemblerX86.C AssemblerX86Init.C			\
	AssemblerX86Init1.C AssemblerX86Init2.C AssemblerX86Init3.C	\
	AssemblerX86Init4.C AssemblerX86Init5.C AssemblerX86Init6.C	\
	AssemblerX86Init7.C AssemblerX86Init8.C AssemblerX86Init9.C	\
	x86InstructionProperties.C

pkginclude_HEADERS = BitPattern.h		\
	Assembler.h AssemblerX86.h		\
	AssemblerX86Init.h			\
	x86InstructionProperties.h

EXTRA_DIST = CMakeLists.txt dummyDisassembler.C
