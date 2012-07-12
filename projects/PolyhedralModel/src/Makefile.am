
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_LTLIBRARIES = \
  libPolyModelCommon.la \
  libPolyModelMaths.la \
  libPolyModelSystem.la \
  libPolyModelRose.la \
  libPolyModelScopLib.la

#  libPolyModelRoseFromPolyDesc.la

lib_LTLIBRARIES = \
  libPolyModel.la \
  libPolyModelRosePragma.la \
  libPolyModelRoseKernel.la \
  libPolyModelRoseMaxCover.la

libPolyModel_la_SOURCES         = libPolyModelDummyFile.cpp
libPolyModel_la_LIBADD          = libPolyModelCommon.la libPolyModelRosePragma.la libPolyModelScopLib.la
libPolyModel_la_includedir      = $(includedir)/polyhedral-model/utils
libPolyModel_la_include_HEADERS = utils/ChainedList.hpp

CXXFLAGS=-g

# COMMON directory

libPolyModelCommon_la_SOURCES         = common/Exception.cpp common/ScheduleSpace.cpp
libPolyModelCommon_la_CXXFLAGS        = $(CXXFLAGS) -I$(PPL_PATH)/include/
libPolyModelCommon_la_LIBADD          = libPolyModelMaths.la
libPolyModelCommon_la_includedir      = $(includedir)/polyhedral-model/common
libPolyModelCommon_la_include_HEADERS = \
  common/Exception.hpp common/Exception.tpp \
  common/PolyhedricContainer.hpp common/PolyhedricContainer.tpp \
  common/PolyhedricDependency.hpp common/PolyhedricDependency.tpp \
  common/Schedule.hpp common/Schedule.tpp \
  common/ScopTree.hpp common/ScopTree.tpp \
  common/ScheduleSpace.hpp common/ScheduleSpace.tpp \
  common/Utils.hpp

# MATHS directory

libPolyModelMaths_la_SOURCES         = maths/Exception-maths.cpp maths/PolyhedronExploration.cpp maths/Quast.cpp maths/biLinear.cpp maths/Farkas.cpp
libPolyModelMaths_la_CXXFLAGS        = $(CXXFLAGS) -I$(PPL_PATH)/include/
libPolyModelMaths_la_LIBADD          = libPolyModelSystem.la -L$(PPL_PATH)/lib/ -lppl
libPolyModelMaths_la_includedir      = $(includedir)/polyhedral-model/maths
libPolyModelMaths_la_include_HEADERS = \
  maths/Exception-maths.hpp \
  maths/PPLUtils.hpp \
  maths/Farkas.hpp \
  maths/biLinear.hpp \
  maths/PolyhedronExploration.hpp \
  maths/Quast.hpp maths/Quast.tpp

# SYSTEM directory

libPolyModelSystem_la_SOURCES         = system/Utils.cpp
libPolyModelSystem_la_CXXFLAGS        = $(CXXFLAGS) 
libPolyModelSystem_la_LIBADD          = 
libPolyModelSystem_la_includedir      = $(includedir)/polyhedral-model/system
libPolyModelSystem_la_include_HEADERS = system/Utils.hpp

# ROSE directory

libPolyModelRose_la_SOURCES         = rose/rose-utils.cpp rose/Exception-rose.cpp rose/Variable.cpp rose/Parser.cpp rose/Quast-rose.cpp
libPolyModelRose_la_CXXFLAGS        = $(CXXFLAGS) \
  -I$(PPL_PATH)/include/ \
  $(ROSE_INCLUDES)
libPolyModelRose_la_LIBADD          =
libPolyModelRose_la_includedir      = $(includedir)/polyhedral-model/rose
libPolyModelRose_la_include_HEADERS = \
  rose/rose-utils.hpp \
  rose/Exception-rose.hpp \
  rose/Variable.hpp \
  rose/Attribute.hpp \
  rose/Attribute.tpp \
  rose/Parser.hpp \
  rose/Parser.tpp \
  rose/CodeGeneration.hpp \
  rose/CodeGeneration.tpp

# ROSE-PRAGMA directory

libPolyModelRosePragma_la_SOURCES         = rose-pragma/PolyhedricPragmaParser.cpp rose-pragma/Exception-rose-pragma.cpp rose-pragma/PolyhedricPragma.cpp
libPolyModelRosePragma_la_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir)/../../pragmaParsing \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
if ROSE_USE_CLOOG
  libPolyModelRosePragma_la_CXXFLAGS     += -DCLOOG_INT_GMP -I$(CLOOG_PATH)/include
endif
libPolyModelRosePragma_la_LIBADD          = \
  libPolyModelRose.la
if ROSE_USE_CLOOG
  libPolyModelRosePragma_la_LIBADD       += -L$(CLOOG_PATH)/lib -lcloog-isl
endif
libPolyModelRosePragma_la_includedir      = $(includedir)/polyhedral-model/rose-pragma
libPolyModelRosePragma_la_include_HEADERS = \
  rose-pragma/Exception-rose-pragma.hpp \
  rose-pragma/PolyhedricPragmaParser.hpp \
  rose-pragma/PolyhedricPragma.hpp

# ROSE-KERNEL directory

libPolyModelRoseKernel_la_SOURCES         = rose-kernel/PolyhedralKernel.cpp
libPolyModelRoseKernel_la_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir)/../../pragmaParsing \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
libPolyModelRoseKernel_la_LIBADD          = \
  libPolyModelRose.la
libPolyModelRoseKernel_la_includedir      = $(includedir)/polyhedral-model/rose-kernel
libPolyModelRoseKernel_la_include_HEADERS = \
  rose-kernel/PolyhedralKernel.hpp

# ROSE-MAX-COVER directory

libPolyModelRoseMaxCover_la_SOURCES         = rose-max-cover/TDBUtraversal.cpp
libPolyModelRoseMaxCover_la_CXXFLAGS        =  $(CXXFLAGS) \
  -I$(PPL_PATH)/include/ \
  $(ROSE_INCLUDES)
libPolyModelRoseMaxCover_la_LIBADD          = 
libPolyModelRoseMaxCover_la_includedir      = $(includedir)/polyhedral-model/rose-max-cover
libPolyModelRoseMaxCover_la_include_HEADERS = \
  rose-max-cover/TDBUtraversal.hpp \
  maths/Expression.hpp \
  maths/Expression.tpp

# ROSE-FROM-POLY-DESC directory

#libPolyModelRoseFromPolyDesc_la_SOURCES         = 
#libPolyModelRoseFromPolyDesc_la_CXXFLAGS        =  $(CXXFLAGS) \
#  -I$(PPL_PATH)/include/ \
#  $(ROSE_INCLUDES)
#if ROSE_USE_CLOOG
#  libPolyModelRoseFromPolyDesc_la_CXXFLAGS     += -DCLOOG_INT_GMP -I$(CLOOG_PATH)/include
#endif
#libPolyModelRoseFromPolyDesc_la_LIBADD          = 
#if ROSE_USE_CLOOG
#  libPolyModelRoseFromPolyDesc_la_LIBADD       += -L$(CLOOG_PATH)/lib -lcloog-isl
#endif
#libPolyModelRoseFromPolyDesc_la_includedir      = $(includedir)/polyhedral-model/rose-from-poly-desc
#libPolyModelRoseFromPolyDesc_la_include_HEADERS = 

# SCOPLIB directory

libPolyModelScopLib_la_SOURCES         = scoplib/Exception-ScopLib.cpp
libPolyModelScopLib_la_CXXFLAGS        = $(CXXFLAGS) -I scoplib/ -I common/ -I maths/ -I$(PPL_PATH)/include/
libPolyModelScopLib_la_LIBADD          = 
libPolyModelScopLib_la_includedir      = $(includedir)/polyhedral-model/scoplib
libPolyModelScopLib_la_include_HEADERS = \
  scoplib/Exception-ScopLib.hpp \
  scoplib/Cloog.hpp \
  scoplib/Cloog.tpp \
  scoplib/ScopLib.hpp \
  scoplib/ScopLib.tpp \
  scoplib/Candl.hpp \
  scoplib/Candl.tpp

include_header = \
$(libPolyModel_la_include_HEADERS) \
$(libPolyModelCommon_la_include_HEADERS) \
$(libPolyModelSystem_la_include_HEADERS) \
$(libPolyModelRose_la_include_HEADERS) \
$(libPolyModelRosePragma_la_include_HEADERS) \
$(libPolyModelRoseMaxCover_la_include_HEADERS) \
$(libPolyModelScopLib_la_include_HEADER)

../../pragmaParsing/libStringParser.la:
	make -C ../../pragmaParsing libStringParser.la
