include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastVisualization.la

libastVisualization_la_SOURCES      = \
   astGraph.C AstAttributeDOT.C wholeAST.C

#   colorFilesTraversal.C  colorMemoryPoolTraversal.C \
#   customAstDOTGeneration.C  customAstDOTGenerationData.C \
#   customMemoryPoolDOTGeneration.C  customMemoryPoolDOTGenerationData.C

libastVisualization_la_LIBADD       = 
libastVisualization_la_DEPENDENCIES = 

include_HEADERS = AstAttributeDOT.h  astGraph.h astGraphTemplateImpl.h wholeAST.h wholeAST_API.h

