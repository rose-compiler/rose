include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## The Rewrite Library should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

INCLUDES = $(ROSE_INCLUDES)

# Separate out the template files to make clear which sources are templates
# the hope was that this would be useful to generate the TemplateSourceFiles
TemplateFiles = \
     rewriteMidLevelInterfaceTemplates.C \
     prefixGeneration.C \
     ASTFragmentCollectorTraversal.C \
     rewriteSynthesizedAttributeTemplates.C \
     rewriteTreeTraversal.C \
     rewriteASTFragementStringTemplates.C \
     nodeCollectionTemplates.C \
     rewriteDebuggingSupportTemplates.C

# Force specific object files (directly associated with executables; with same name) to
# be recompiled when the template files are modified.
TemplateSourceFiles = \
     $(srcdir)/rewriteMidLevelInterfaceTemplates.C \
     $(srcdir)/prefixGeneration.C \
     $(srcdir)/ASTFragmentCollectorTraversal.C \
     $(srcdir)/rewriteSynthesizedAttributeTemplates.C \
     $(srcdir)/rewriteTreeTraversal.C \
     $(srcdir)/rewriteASTFragementStringTemplates.C \
     $(srcdir)/nodeCollectionTemplates.C \
     $(srcdir)/rewriteDebuggingSupportTemplates.C

librewriteSources = \
     nodeCollection.C \
     rewriteMidLevelInterface.C \
     rewriteHighLevelInterface.C \
     rewriteSynthesizedAttribute.C \
     rewriteASTFragementString.C \
     rewriteAccumulatedAttribute.C \
     rewriteTypeDeclaration.C \
     rewriteDebuggingSupport.C \
     rewriteLowLevelInterface.C \
     $(TemplateFiles)

# lib_LTLIBRARIES = librewrite.a
# librewrite_a_SOURCES  = $(librewriteSources)
# librewrite_a_DEPENDENCIES  = $(librewrite_a_SOURCES)

if DEV_LIBS
lib_LTLIBRARIES = librewrite.la
else
noinst_LTLIBRARIES = librewrite.la
endif
librewrite_la_SOURCES = $(librewriteSources)
librewrite_la_DEPENDENCIES = $(librewrite_la_SOURCES)

# Header files we want to have installed
# (note that rewrite.h includes the rewriteTreeTraversal.C 
# and rewriteSynthesizedAttributeTemplates.C because of how
# templates are used).
include_HEADERS = \
     rewrite.h \
     lowLevelRewriteInterface.h \
     midLevelRewriteInterface.h \
     highLevelRewriteInterface.h \
     $(TemplateFiles)

clean-local:
	rm -rf Templates.DB ii_files ti_files core *.dot *.pdf
	rm -rf rose_inputProgram*.C \
          rose_transformation_*.C *.C.pdf

EXTRA_DIST = ASTRewrite.docs


























