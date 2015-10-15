include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)  

LINK = $(CXXLINK)

# DQ (3/14/2009): This is defined in config/Makefile.for.ROSE.includes.and.libs
# LDFLAGS = -module
LDFLAGS += -module

lib_LTLIBRARIES = libInterruptAnalysis.la libBufferOverflow.la libCycleDetection.la

libInterruptAnalysis_la_DEPENDENCIES =
libInterruptAnalysis_la_SOURCES = InterruptAnalysis.cpp 
libBufferOverflow_la_DEPENDENCIES =
libBufferOverflow_la_SOURCES = BufferOverflow.cpp
libCycleDetection_la_DEPENDENCIES =
libCycleDetection_la_SOURCES = CycleDetection.cpp

# Make sure these SAGE specific header files are placed into a distribution
BINARY_DISASSEMBLY_HEADER_FILES = \
   InterruptAnalysis.h ../GraphAnalysisInterface.h BufferOverflow.h CycleDetection.h


include_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)


clean-local:
	rm -f *.dot *.gml *.lo *.la *.s *.o

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o
