include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libimplicitCodeGeneration.la
else
noinst_LTLIBRARIES = libimplicitCodeGeneration.la
endif

libimplicitCodeGeneration_la_SOURCES = analysisUtils.C defaultFunctionGenerator.C destructorCallAnnotator.C shortCircuitingTransformation.C

include_HEADERS = analysisUtils.h defaultFunctionGenerator.h destructorCallAnnotator.h shortCircuitingTransformation.h

EXTRA_DIST = README
