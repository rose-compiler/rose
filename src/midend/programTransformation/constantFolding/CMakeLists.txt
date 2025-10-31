


########### next target ###############


########### install files ###############

install(FILES  constantFolding.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#libconstantFoldingSources = \
#   constantFolding.C
#
#noinst_LTLIBRARIES = libconstantFolding.la
#libconstantFolding_la_SOURCES = $(libconstantFoldingSources)
#
#include_HEADERS = constantFolding.h
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
