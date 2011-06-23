include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(PYTHON_CPPFLAGS)
AM_LDFLAGS = $(PYTHON_LDFLAGS)

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as librosePythonFrontend.so)
noinst_LTLIBRARIES = librosePythonFrontend.la

librosePythonFrontend_la_DEPENDENCIES =
librosePythonFrontend_la_SOURCES = \
	SagePythonInterface.C \
	SagePythonSupport.C \
	PythonFrontend.C

include_HEADERS = \
	PythonFrontend.h \
	SagePythonInterface.h \
	SagePythonSupport.C

sageTranslator_PYTHON = sageTranslator.py
sageTranslatordir = $(libdir)/python

EXTRA_DIST = CMakeLists.txt
