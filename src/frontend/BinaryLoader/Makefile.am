include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseExecFormats.so)
noinst_LTLIBRARIES = libroseBinaryLoader.la

libroseBinaryLoader_la_DEPENDENCIES =
libroseBinaryLoader_la_SOURCES = \
   binaryLoader.C\
   BinaryLoaderGeneric.C\
   BinaryLoaderElf.C

include_HEADERS = binaryLoader.h\
	BinaryLoaderGeneric.h\
	BinaryLoaderElf.h

