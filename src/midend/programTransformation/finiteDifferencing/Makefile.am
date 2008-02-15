include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I${top_srcdir}/src/midend/programAnalysis/CFG 

libfiniteDifferencingSources = \
   finiteDifferencing.C patternRewrite.C

if DEV_LIBS
lib_LTLIBRARIES = libfiniteDifferencing.la
else
noinst_LTLIBRARIES = libfiniteDifferencing.la
endif
libfiniteDifferencing_la_SOURCES = $(libfiniteDifferencingSources)

include_HEADERS = finiteDifferencing.h patternRewrite.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core
