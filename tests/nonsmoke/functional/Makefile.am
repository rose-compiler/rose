include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

# Tests for utility functions within ROSE
SUBDIRS += Utility

# Tests for the rose::BinaryAnalysis namespace
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
if !ROSE_USE_INSURE
   SUBDIRS += BinaryAnalysis
endif
endif
