
include_directories(${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OAWrap ${CMAKE_SOURCE_DIR}/src/midend/programAnalysis ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############

install(FILES  OAWrap.h SAGE2OA.h OAConfig.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES =  $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis/OAWrap -I$(top_srcdir)/src/midend/programAnalysis
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libOAWrapSources = OAWrap.C SAGE2OA.C 
#
## lib_LTLIBRARIES = libOAWrap.a
## libOAWrap_a_SOURCES  = $(libOAWrapSources)
#
#noinst_LTLIBRARIES = libOAWrap.la
#libOAWrap_la_SOURCES = $(libOAWrapSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  OAWrap.h SAGE2OA.h OAConfig.h
#
#
#
#
#
#
#
