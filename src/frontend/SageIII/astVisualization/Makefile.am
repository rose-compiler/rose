include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libastVisualization.la
else
noinst_LTLIBRARIES = libastVisualization.la
endif

libastVisualization_la_SOURCES      = astGraph.C AstAttributeDOT.C wholeAST.C

libastVisualization_la_LIBADD       = 
libastVisualization_la_DEPENDENCIES = 

# note that the astGraphTemplate.C is included in the astGraph.h header file
include_HEADERS = AstAttributeDOT.h  astGraph.h astGraphTemplate.C wholeAST.h wholeAST_API.h

