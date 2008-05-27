include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libroseDisassemblers.la
else
# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la
endif

libroseDisassemblers_la_DEPENDENCIES =
libroseDisassemblers_la_SOURCES = x86Disassembler.C armDisassembler.C putInstructionsIntoBasicBlocks.C disassemblers.h

dist_include_HEADERS = disassemblers.h

include_HEADERS = 
