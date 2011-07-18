include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../prepostTransformation -I$(srcdir)/../slicing -I$(srcdir)/../computation -I$(srcdir)/../depGraph -I$(srcdir)/../depInfo  -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/astUtil/astInterface

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libdriverSources = \
   BlockingAnal.C  FusionAnal.C   CopyArrayAnal.C  LoopTransformOptions.C   \
   TransformComputation.C InterchangeAnal.C  TypedFusionImpl.C \
   ParallelizeLoop.C LoopTransformInterface.C NormalizeCPP.C

# lib_LTLIBRARIES = libdriver.a
# libdriver_a_SOURCES  = $(libdriverSources)

noinst_LTLIBRARIES = libdriver.la
libdriver_la_SOURCES = $(libdriverSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  BlockingAnal.h  InterchangeAnal.h  CopyArrayAnal.h  \
                    LoopTransformOptions.h  LoopTransformInterface.h\
                   FusionAnal.h ParallelizeLoop.h AutoTuningInterface.h


EXTRA_DIST = CMakeLists.txt
