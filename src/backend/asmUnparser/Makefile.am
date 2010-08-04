include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseAsmUnparser.la

libroseAsmUnparser_la_DEPENDENCIES =
libroseAsmUnparser_la_SOURCES = AsmUnparser.C AsmUnparser_compat.C unparseX86Asm.C unparseArmAsm.C unparsePowerpcAsm.C

dist_include_HEADERS = AsmUnparser.h AsmUnparser_compat.h

include_HEADERS = 

EXTRA_DIST = CMakeLists.txt
