
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  omp_lowering.h libgomp_g.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#noinst_LTLIBRARIES = libompLowering.la
#libompLowering_la_SOURCES = omp_lowering.cpp
#
#include_HEADERS = omp_lowering.h libgomp_g.h
#
#EXTRA_DIST = README
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
