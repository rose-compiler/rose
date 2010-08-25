include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = CMakeLists.txt DebuggerPtrace.c README SummaryDescription.odp SummaryDescription.pdf

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES=

INCLUDES=$(ROSE_INCLUDES)

dist_include_HEADERS=Debugger.h
noinst_PROGRAMS=verifySemantics traceInstructions

# Program to verify that ROSE's instruction semantics layer functions correctly.
verifySemantics_SOURCES=verifySemantics.C Debugger.C
verifySemantics_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)

# Program to trace instruction execution
traceInstructions_SOURCES=traceInstructions.C Debugger.C
traceInstructions_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
