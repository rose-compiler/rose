
include_directories(${CMAKE_SOURCE_DIR}/src/util ${CMAKE_SOURCE_DIR}/src/util/support)


########### next target ###############


########### install files ###############

install(FILES  AstInterface.h  CPPAstInterface.h AstInterface_ROSE.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libastInterfaceSources = AstInterface.C CPPAstInterface.C
#
## lib_LTLIBRARIES = libastInterface.a
## libastInterface_a_SOURCES  = $(libastInterfaceSources)
#
#noinst_LTLIBRARIES = libastInterface.la
#libastInterface_la_SOURCES = $(libastInterfaceSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  AstInterface.h  CPPAstInterface.h AstInterface_ROSE.h
#
#
#
#
#
#
#
#
