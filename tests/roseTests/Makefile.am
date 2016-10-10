# Temporary makefile for Jenkins
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
if !USING_GCC_3_4_4_BACKEND_COMPILER
if !USING_XTENSA_BACKEND_COMPILER
if !ROSE_USE_INSURE
SUBDIRS += ompLoweringTests programAnalysisTests
endif
endif
endif
endif
