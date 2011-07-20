include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES=libabstractMemoryObject.la

libabstractMemoryObject_la_SOURCES =\
  memory_object.cpp  memory_object.h

include_HEADERS = \
   memory_object.h
  
clean-local:
	rm -rf Templates.DB ii_files ti_files core

EXTRA_DIST = CMakeLists.txt README  original_design.doc
