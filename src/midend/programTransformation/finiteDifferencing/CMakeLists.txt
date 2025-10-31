


########### next target ###############


########### install files ###############

install(FILES  finiteDifferencing.h patternRewrite.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES) -I${top_srcdir}/src/midend/programAnalysis/CFG 
#
#libfiniteDifferencingSources = \
#   finiteDifferencing.C patternRewrite.C
#
#noinst_LTLIBRARIES = libfiniteDifferencing.la
#libfiniteDifferencing_la_SOURCES = $(libfiniteDifferencingSources)
#
#include_HEADERS = finiteDifferencing.h patternRewrite.h
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
