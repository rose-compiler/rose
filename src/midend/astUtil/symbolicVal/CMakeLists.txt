
include_directories(${CMAKE_SOURCE_DIR}/src/util ${CMAKE_SOURCE_DIR}/src/util/support)


########### next target ###############


########### install files ###############

install(FILES SymbolicMultiply.h SymbolicPlus.h BooleanOperators.h UnaryOperators.h
              SymbolicVal.h SymbolicExpr.h SymbolicSelect.h SymbolicBound.h
              DESTINATION ${INCLUDE_INSTALL_DIR})

#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../astInterface -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libsymbolicValSources = \
#   SymbolicBound.C  SymbolicExpr.C  SymbolicFind.C  SymbolicOperator.C \
#   SymbolicUnwrapCond.C SymbolicVal.C 
#
## lib_LTLIBRARIES = libsymbolicVal.a
## libsymbolicVal_a_SOURCES  = $(libsymbolicValSources)
#
#noinst_LTLIBRARIES = libsymbolicVal.la
#libsymbolicVal_la_SOURCES = $(libsymbolicValSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  SymbolicMultiply.h  SymbolicPlus.h    SymbolicVal.h \
#                   SymbolicExpr.h  SymbolicSelect.h  SymbolicBound.h 
#
#
#
#
#
#
#
#
