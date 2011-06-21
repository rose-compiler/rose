include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseClangFrontend.so)
noinst_LTLIBRARIES = libroseClangFrontend.la

libroseClangFrontend_la_DEPENDENCIES =
libroseClangFrontend_la_SOURCES = ClangFrontend.C ClangFrontend.h

dist_include_HEADERS = ClangFrontend.h

include_HEADERS =


EXTRA_DIST = CMakeLists.txt
