
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  PDFGeneration.h AttributeMechanism.h DOTGeneration.h DOTRepresentation.h     Translator.h DOTSubgraphRepresentation.h DOTGraphInterface.h TreeTraversal.h     DOTRepresentationImpl.h DOTGenerationImpl.h     DOTSubgraphRepresentationImpl.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libmstlSources = \
#   PDFGeneration.C DOTGraphInterface.C
#
## lib_LIBRARIES = libmstl.a
## libmstl_a_SOURCES  = $(libmstlSources)
#
#noinst_LTLIBRARIES = libmstl.la
#libmstl_la_SOURCES = $(libmstlSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
## Some source files are included so that templates can be included
#include_HEADERS = \
#   PDFGeneration.h AttributeMechanism.h DOTGeneration.h DOTRepresentation.h \
#   Translator.h DOTSubgraphRepresentation.h DOTGraphInterface.h TreeTraversal.h \
#   DOTRepresentationImpl.h DOTGenerationImpl.h \
#   DOTSubgraphRepresentationImpl.h
#
#EXTRA_DIST = MSTL.docs
#
#
#
#
#
#
#
