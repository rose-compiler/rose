include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)  

LINK = $(CXXLINK)

LDFLAGS = -module

lib_LTLIBRARIES = libInterruptAnalysis.la libBufferOverflow.la

libInterruptAnalysis_la_DEPENDENCIES =
libInterruptAnalysis_la_SOURCES = InterruptAnalysis.cpp 
libBufferOverflow_la_DEPENDENCIES =
libBufferOverflow_la_SOURCES = BufferOverflow.cpp

# Make sure these SAGE specific header files are placed into a distribution
BINARY_DISASSEMBLY_HEADER_FILES = \
   InterruptAnalysis.h ../GraphAnalysisInterface.h BufferOverflow.h


include_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)


clean-local:
	rm -f *.dot *.gml *.lo *.la *.s *.o

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o
