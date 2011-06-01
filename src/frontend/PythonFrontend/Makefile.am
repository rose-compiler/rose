include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as librosePythonFrontend.so)
noinst_LTLIBRARIES = librosePythonFrontend.la

librosePythonFrontend_la_DEPENDENCIES =
librosePythonFrontend_la_SOURCES = PythonFrontend.C PythonFrontend.h

dist_include_HEADERS = PythonFrontend.h

include_HEADERS = 


EXTRA_DIST = CMakeLists.txt
