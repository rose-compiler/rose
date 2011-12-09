include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = test

INCLUDES = \
     $(ROSE_INCLUDES) 
#     -I$(top_srcdir)/src/util \
#     -I$(top_srcdir)/src/util/support \
#     -I$(top_srcdir)/src/util/graphs \
#     -I$(top_srcdir)/src/midend/astUtil \
#     -I$(top_srcdir)/src/midend/astUtil/astInterface \
#     -I$(top_srcdir)/src/midend/astUtil/astSupport \
#     -I$(top_srcdir)/src/midend/astUtil/symbolicVal \
#     -I$(top_srcdir)/src/midend/astUtil/annotation \
#     -I$(top_srcdir)/src/midend/programAnalysis \
#     -I$(top_srcdir)/src/midend/programAnalysis/CFG \
#     -I$(top_srcdir)/src/midend/programAnalysis/valuePropagation \
#     -I$(top_srcdir)/src/midend/programAnalysis/CFG \
#     -I$(top_srcdir)/src/midend/loopProcessing/driver \
#     -I$(top_srcdir)/src/midend/loopProcessing/depInfo

#lib_LIBRARIES = libarrayOptimization.a

## The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .

#libarrayOptimization_a_SOURCES = ArrayAnnot.C  ArrayInterface.C  ArrayRewrite.C 

bin_PROGRAMS = ArrayProcessor
ArrayProcessor_SOURCES = ArrayProcessor.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#LDADD = libarrayOptimization.a $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C

distclean-local:
	rm -rf Templates.DB

#include_HEADERS =  ArrayAnnot.h  ArrayInterface.h  ArrayRewrite.h

