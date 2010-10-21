include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = \
        abstractHandle \
        astDiagnostics \
        astProcessing \
        astQuery \
        astRewriteMechanism \
        astUtil \
        programAnalysis \
        programTransformation

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   SUBDIRS += binaryAnalyses 
endif

# DQ (7/25/2008):
# This file is deliberately excluded from a distribution so that the
# "make docs" rule (at the top level directory) can query the file to
# determin if this is a distribution.
# EXTRA_DIST = midend.docs

EXTRA_DIST = CMakeLists.txt
