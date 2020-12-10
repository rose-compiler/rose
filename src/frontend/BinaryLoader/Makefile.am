include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libroseBinaryLoader.la
pkginclude_HEADERS = BinaryLoader.h BinaryLoaderElf.h BinaryLoaderElfObj.h BinaryLoaderPe.h

libroseBinaryLoader_la_DEPENDENCIES =
libroseBinaryLoader_la_SOURCES = BinaryLoader.C BinaryLoaderElf.C BinaryLoaderElfObj.C BinaryLoaderPe.C
