include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as librosePHPFrontend.so)
noinst_LTLIBRARIES = librosePHPFrontend.la

librosePHPFrontend_la_DEPENDENCIES =
librosePHPFrontend_la_SOURCES = PHPFrontend.C PHPFrontend.h

dist_pkginclude_HEADERS = PHPFrontend.h

pkginclude_HEADERS = 


EXTRA_DIST = CMakeLists.txt
