include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libdriverSources = \
   BlockingAnal.C  FusionAnal.C    CopyArrayAnal.C   LoopTransformOptions.C   \
   TransformComputation.C InterchangeAnal.C  TypedFusionImpl.C LoopTransformInterface.C \
   NormalizeCPP.C

# lib_LTLIBRARIES = libdriver.a
# libdriver_a_SOURCES  = $(libdriverSources)

if DEV_LIBS
lib_LTLIBRARIES = libdriver.la
else
noinst_LTLIBRARIES = libdriver.la
endif
libdriver_la_SOURCES = $(libdriverSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  BlockingAnal.h  InterchangeAnal.h  CopyArrayAnal.h  \
                    LoopTransformOptions.h  LoopTransformInterface.h\
                   FusionAnal.h   

