include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if ROSE_USE_BOOST_WAVE

noinst_LTLIBRARIES = libastTokenStream.la

libastTokenStream_la_SOURCES      = \
     createMap.C  doCompleteMapping.C  linearizeAST.C unparseMacro.C

libastTokenStream_la_LIBADD       = 
libastTokenStream_la_DEPENDENCIES = 

include_HEADERS = \
     createMap.h  doCompleteMapping.h  linearizeAST.h unparseMacro.h

endif
