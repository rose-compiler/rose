include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = CPAnalysis CPTransformation test

INCLUDES = \
     $(ROSE_INCLUDES) -I$(top_srcdir)/src/util  \
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

CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = ContainerTransformer
ContainerTransformer_SOURCES = ContainerTransformer.C
ContainerTransformer_DEPENDENCIES = \
        ContainerTransformer.o \
        $(top_builddir)/projects/containerParallelizer/CPTransformation/libOmpTransformations1.a \
        $(ROSE_LIBS_WITH_PATH)

LDADD = \
     $(top_builddir)/projects/containerParallelizer/CPAnalysis/libCPAnalysis.a \
     $(top_builddir)/projects/containerParallelizer/CPTransformation/libOmpTransformations1.a \
     $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# ROSE_FLAGS = -rose:test 6

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

check-local:
	@echo "*************************************************************************************************"
	@echo "****** ROSE/projects/containerParallelizer: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************"
