include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (12/7/2004): Commented out since we don't currently pass this test!
SUBDIRS = test

INCLUDES = \
     $(ROSE_INCLUDES) \
     -I$(top_srcdir)/src/util \
     -I$(top_srcdir)/src/util/support \
     -I$(top_srcdir)/src/util/graphs \
     -I$(top_srcdir)/src/midend/astUtil \
     -I$(top_srcdir)/src/midend/astUtil/astInterface \
     -I$(top_srcdir)/src/midend/astUtil/astSupport \
     -I$(top_srcdir)/src/midend/astUtil/symbolicVal \
     -I$(top_srcdir)/src/midend/astUtil/annotation \
     -I$(top_srcdir)/src/midend/programAnalysis \
     -I$(top_srcdir)/src/midend/programAnalysis/CFG \
     -I$(top_srcdir)/src/midend/programAnalysis/dataflowAnalysis \
     -I$(top_srcdir)/src/midend/programAnalysis/valuePropagation \
     -I$(top_srcdir)/src/midend/programAnalysis/CFG \
     -I$(top_srcdir)/src/midend/loopProcessing \
     -I$(top_srcdir)/src/midend/loopProcessing/depInfo

lib_LIBRARIES = libarrayOptimization.a

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libarrayOptimization_a_SOURCES = ArrayAnnot.C  ArrayInterface.C  ArrayRewrite.C 
libarrayOptimization_a_DEPENDENCIES = \
     ArrayAnnot.o  ArrayInterface.o ArrayRewrite.o
libarrayOptimization_a_LIBADD =  \
     ArrayAnnot.o  ArrayInterface.o ArrayRewrite.o


bin_PROGRAMS = ArrayProcessor
ArrayProcessor_SOURCES = ArrayProcessor.C
ArrayProcessor_DEPENDENCIES = libarrayOptimization.a $(ROSE_LIBS_WITH_PATH)

LDADD = libarrayOptimization.a $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
libarrayOptimization.a: $(libarrayOptimization_a_DEPENDENCIES)
	rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libarrayOptimization_a_LIBADD)
	test -f $@ || exit 1

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  ArrayAnnot.h  ArrayInterface.h  ArrayRewrite.h

