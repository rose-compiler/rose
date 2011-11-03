include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libimplicitCodeGeneration.la

libimplicitCodeGeneration_la_SOURCES = analysisUtils.C defaultFunctionGenerator.C destructorCallAnnotator.C shortCircuitingTransformation.C

include_HEADERS = analysisUtils.h defaultFunctionGenerator.h destructorCallAnnotator.h shortCircuitingTransformation.h

EXTRA_DIST = CMakeLists.txt README
