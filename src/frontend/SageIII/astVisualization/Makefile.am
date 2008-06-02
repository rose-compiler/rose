include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libastVisualization.la
else
noinst_LTLIBRARIES = libastVisualization.la
endif

libastVisualization_la_SOURCES      = astGraph.C AstAttributeDOT.C wholeAST.C

libastVisualization_la_LIBADD       = 
libastVisualization_la_DEPENDENCIES = 

include_HEADERS = AstAttributeDOT.h  astGraph.h astGraphTemplateImpl.h wholeAST.h wholeAST_API.h

