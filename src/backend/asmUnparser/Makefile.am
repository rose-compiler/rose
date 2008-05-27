include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libroseAsmUnparser.la
else
# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseAsmUnparser.la
endif

libroseAsmUnparser_la_DEPENDENCIES =
libroseAsmUnparser_la_SOURCES = unparseX86Asm.C unparseArmAsm.C unparseAsm.C

dist_include_HEADERS = unparseAsm.h

include_HEADERS = 
