include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =      $(ROSE_INCLUDES) -I$(top_srcdir)/src/util  \
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
     -I$(top_srcdir)/projects/containerParallelizer/CPAnalysis \
     -I$(top_srcdir)/projects/containerParallelizer/CPTransformation \
     -I$(top_srcdir)/projects/containerParallelizer

lib_LIBRARIES = libCPAnalysis.a

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libCPAnalysis_a_SOURCES      = TestParallelLoop.C 
libCPAnalysis_a_DEPENDENCIES = TestParallelLoop.o
libCPAnalysis_a_LIBADD       = TestParallelLoop.o

# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
libCPAnalysis.a: $(libCPAnalysis_a_DEPENDENCIES)
	rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libCPAnalysis_a_LIBADD)
	test -f $@ || exit 1

bin_PROGRAMS = ContainerAnalyzer
ContainerAnalyzer_SOURCES = ContainerAnalyzer.C
ContainerAnalyzer_DEPENDENCIES = \
     ContainerAnalyzer.o \
     libCPAnalysis.a \
     $(ROSE_LIBS_WITH_PATH)
LDADD = libCPAnalysis.a $(LIBS_WITH_RPATH) $(ROSE_LIBS)

ROSE_FLAGS =

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  TestParallelLoop.h  

