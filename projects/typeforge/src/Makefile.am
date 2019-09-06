include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = 

bin_PROGRAMS = typeforge
noinst_PROGRAMS = 

CLEANFILES = 

typeforge_YFLAGS = -p ltl_ # yacc flags for specification parser (not used yet)
typeforge_CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
typeforge_DEPENDENCIES=

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/typeforge/src -g -rdynamic

typeforge_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
typeforge__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
typeforge_SOURCES = \
  typeforge.C \
  CommandLineOptions.C \
  CastStats.C \
  CastTransformer.C \
  TFTypeTransformer.C \
  CastGraphVis.C \
  CppStdUtilities.C \
  TFTransformation.C \
  SpecFrontEnd.C \
  ToolConfig.cpp \
  CommandList.C \
  Analysis.C \
  OperandNetwork.C \
  OperandData.C \
  TypeChain.C

