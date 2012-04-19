
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CXXFLAGS=-g

noinst_LTLIBRARIES = \
  libToolBoxes.la

lib_LTLIBRARIES = \
  libSPMDizeCommon.la

libSPMDizeCommon_la_SOURCES = \
  common/spmd-generator.cpp \
  common/spmd-tree.cpp \
  common/spmd-driver.cpp \
  common/comm-analysis.cpp \
  common/placement.cpp \
  common/compute-system.cpp \
  common/array-analysis.cpp
libSPMDizeCommon_la_CXXFLAGS = \
  $(CXXFLAGS) \
  $(ROSE_INCLUDES) \
  -I$(srcdir)/../../src/
libSPMDizeCommon_la_LIBADD = \
  libToolBoxes.la
libSPMDizeCommon_la_includedir = $(includedir)/SPMDize/
libSPMDizeCommon_la_include_HEADERS = \
  common/spmd-generator.hpp \
  common/spmd-tree.hpp \
  common/spmd-driver.hpp \
  common/comm-analysis.hpp

libToolBoxes_la_SOURCES = \
  toolboxes/rose-toolbox.cpp \
  toolboxes/algebra-container.cpp \
  $(srcdir)/../../src/rose/Variable.cpp
libToolBoxes_la_CXXFLAGS = \
  $(CXXFLAGS) \
  $(ROSE_INCLUDES) \
  -I$(srcdir)/../../src/
  
