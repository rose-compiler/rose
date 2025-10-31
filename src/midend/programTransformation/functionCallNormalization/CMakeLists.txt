


########### next target ###############


########### install files ###############

install(FILES  FunctionNormalization.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#libfunctionCallNormalizationSources = \
#   FunctionNormalization.C
#
#noinst_LTLIBRARIES = libfunctionCallNormalization.la
#libfunctionCallNormalization_la_SOURCES = $(libfunctionCallNormalizationSources)
#
#include_HEADERS = FunctionNormalization.h
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
