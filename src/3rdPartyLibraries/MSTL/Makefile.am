include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
libmstlSources = \
   DOTGraphInterface.C
else
libmstlSources = \
   PDFGeneration.C DOTGraphInterface.C
endif

# lib_LIBRARIES = libmstl.a
# libmstl_a_SOURCES  = $(libmstlSources)

noinst_LTLIBRARIES = libmstl.la
libmstl_la_SOURCES = $(libmstlSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

# Some source files are included so that templates can be included
pkginclude_HEADERS = \
   PDFGeneration.h DOTGeneration.h DOTRepresentation.h \
   Translator.h DOTSubgraphRepresentation.h DOTGraphInterface.h TreeTraversal.h \
   DOTRepresentationImpl.h DOTGenerationImpl.h \
   DOTSubgraphRepresentationImpl.h

EXTRA_DIST = CMakeLists.txt MSTL.docs







