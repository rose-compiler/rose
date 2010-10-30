include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libroseBinaryLoader.la
include_HEADERS = BinaryLoader.h BinaryLoaderElf.h BinaryLoaderElfObj.h BinaryLoaderPe.h

libroseBinaryLoader_la_DEPENDENCIES =

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libroseBinaryLoader_la_SOURCES = BinaryLoader.C BinaryLoaderElf.C BinaryLoaderElfObj.C BinaryLoaderPe.C
else
   libroseBinaryLoader_la_SOURCES = 
endif

