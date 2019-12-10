include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=OpNet

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)
AM_CPPFLAGS = \
  $(ROSE_INCLUDES)
AM_CXXFLAGS= \
  -I$(top_srcdir)/projects/typeforge/include \
  -g -Wall -O3 -march=native -ftree-vectorize

lib_LTLIBRARIES= \
  libTypeforge.la \
  libTypeforge-transform.la \
  libTypeforge-cast.la \
  libTypeforge-utils.la

libTypeforge_la_SOURCES= \
  SpecFrontEnd.cpp \
  ToolConfig.cpp \
  CommandList.cpp \
  Analysis.cpp \
  commons.cpp

libTypeforge_transform_la_SOURCES= \
  TFTypeTransformer.cpp \
  TFTransformation.cpp

libTypeforge_cast_la_SOURCES= \
  CastStats.cpp

libTypeforge_utils_la_SOURCES= \
  CommandLineOptions.cpp \
  CppStdUtilities.cpp

