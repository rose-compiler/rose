
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CXXFLAGS=-g

noinst_PROGRAMS = \
  identity \
  single-gpu

noinst_LTLIBRARIES = \
  libToolBoxes.la \
  libComputeSystem.la

lib_LTLIBRARIES = \
  libSPMDizeCommon.la \
  libSPMDizePolyDriver.la \
  libSPMDizeOpenCLgen.la

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
  common/comm-analysis.hpp \
  common/array-analysis.hpp \
  common/compute-system.hpp \
  common/placement.hpp

libSPMDizePolyDriver_la_SOURCES = \
  polydriver/polyhedral-driver.cpp \
  polydriver/polyhedral-placement.cpp \
  polydriver/polyhedral-array-analysis.cpp \
  polydriver/polyhedral-utils.cpp
libSPMDizePolyDriver_la_CXXFLAGS = \
  $(CXXFLAGS) \
  $(ROSE_INCLUDES) \
  -I$(srcdir)/../../src/ \
  -I$(PPL_PATH)/include
libSPMDizePolyDriver_la_LIBADD = \
  ../../src/libPolyModel.la \
  ../../src/libPolyModelRose.la
libSPMDizePolyDriver_la_includedir = $(includedir)/SPMDize/
libSPMDizePolyDriver_la_include_HEADERS = \
  polydriver/polyhedral-driver.hpp \
  polydriver/polyhedral-array-analysis.hpp \
  polydriver/polyhedral-placement.hpp \
  polydriver/polyhedral-utils.hpp

libSPMDizeOpenCLgen_la_SOURCES = \
  opencl-generator/opencl-generator.cpp
libSPMDizeOpenCLgen_la_CXXFLAGS = \
  $(CXXFLAGS) \
  $(ROSE_INCLUDES) \
  -I$(srcdir)/../../src/
libSPMDizeOpenCLgen_la_includedir = $(includedir)/SPMDize/
libSPMDizeOpenCLgen_la_include_HEADERS = \
  opencl-generator/opencl-generator.hpp

libToolBoxes_la_SOURCES = \
  toolboxes/rose-toolbox.cpp \
  toolboxes/algebra-container.cpp \
  $(srcdir)/../../src/rose/Variable.cpp
libToolBoxes_la_CXXFLAGS = \
  $(CXXFLAGS) \
  $(ROSE_INCLUDES) \
  -I$(srcdir)/../../src/
libToolBoxes_la_includedir = $(includedir)/SPMDize/
libToolBoxes_la_include_HEADERS = \
  toolboxes/algebra-container.hpp \
  toolboxes/rose-toolbox.hpp

libComputeSystem_la_SOURCES = \
  compute-systems/array-system.cpp \
  compute-systems/gpu-system.cpp \
  compute-systems/compute-node.cpp \
  compute-systems/cluster-system.cpp \
  compute-systems/memory.cpp
libComputeSystem_la_includedir = $(includedir)/SPMDize/
libComputeSystem_la_include_HEADERS = \
  compute-systems/array-system.hpp \
  compute-systems/cluster-system.hpp \
  compute-systems/compute-node.hpp \
  compute-systems/gpu-system.hpp \
  compute-systems/memory.hpp

identity_SOURCES = \
  dummy-test/identity.cpp
identity_CXXFLAGS = \
  $(CXXFLAGS) \
  $(ROSE_INCLUDES) \
  -I$(srcdir)/../../src/ \
  -I$(PPL_PATH)/include
identity_LDADD = \
  libSPMDizeCommon.la \
  libSPMDizePolyDriver.la \
  -L$(PPL_PATH)/lib -lppl -lgmp \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

single_gpu_SOURCES = \
  single-gpu/single-gpu.cpp \
  single-gpu/depth-placement.cpp \
  single-gpu/test.cpp
single_gpu_CXXFLAGS = \
  $(CXXFLAGS) \
  $(ROSE_INCLUDES) \
  -I$(srcdir)/../../src/ \
  -I$(PPL_PATH)/include
single_gpu_LDADD = \
  libSPMDizeCommon.la \
  libSPMDizePolyDriver.la \
  libComputeSystem.la \
  libSPMDizeOpenCLgen.la \
  -L$(PPL_PATH)/lib -lppl -lgmp \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)
single_gpu_includedir = $(includedir)/SPMDize/
single_gpu_include_HEADERS = \
  single-gpu/depth-placement.hpp \
  single-gpu/single-gpu.hpp

../../src/libPolyModelRose.la:
	make -C  ../../src/ libPolyModelRose.la

../../src/libPolyModel.la:
	make -C  ../../src/ libPolyModel.la

