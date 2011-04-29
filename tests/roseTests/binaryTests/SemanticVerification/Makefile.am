include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = CMakeLists.txt DebuggerPtrace.c README SummaryDescription.odp SummaryDescription.pdf

INCLUDES=$(ROSE_INCLUDES)

dist_include_HEADERS=Debugger.h
noinst_PROGRAMS=

# Program to verify that ROSE's instruction semantics layer functions correctly.
noinst_PROGRAMS += verifySemantics
verifySemantics_SOURCES=verifySemantics.C Debugger.C
verifySemantics_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)


# Program to trace instruction execution
noinst_PROGRAMS += traceInstructions
traceInstructions_SOURCES=traceInstructions.C Debugger.C
traceInstructions_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
