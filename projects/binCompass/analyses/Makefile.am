include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)  

LINK = $(CXXLINK)

# DQ (3/14/2009): This is defined in config/Makefile.for.ROSE.includes.and.libs
# LDFLAGS = -module
LDFLAGS += -module

lib_LTLIBRARIES = libPrintAsmFunctions.la libPrintAsmInstruction.la

libPrintAsmFunctions_la_DEPENDENCIES =
libPrintAsmFunctions_la_SOURCES = PrintAsmFunctions.cpp
libPrintAsmInstruction_la_DEPENDENCIES =
libPrintAsmInstruction_la_SOURCES = PrintAsmInstruction.cpp

# Make sure these SAGE specific header files are placed into a distribution
BINARY_DISASSEMBLY_HEADER_FILES = \
   PrintAsmFunctions.h PrintAsmInstruction.h ../binCompassAnalysisInterface.h


include_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)


clean-local:
	rm -f *.dot *.gml *.lo *.la *.s *.o

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o
