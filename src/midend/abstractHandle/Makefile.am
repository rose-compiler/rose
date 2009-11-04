include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES=libabstractHandle.la

libabstractHandle_la_SOURCES =\
  abstract_handle.cpp roseAdapter.cpp

include_HEADERS = \
  abstract_handle.h roseAdapter.h
  
clean-local:
	rm -rf Templates.DB ii_files ti_files core
# This directory also contains a self-contained example 
# using a simple loop data structure to demonstrate the usage.
# It can be released separated from ROSE
EXTRA_DIST = CMakeLists.txt README testMyLoop.cpp myloop.h loopAdapter.h loopAdapter.cpp \
            makefile-myloop
