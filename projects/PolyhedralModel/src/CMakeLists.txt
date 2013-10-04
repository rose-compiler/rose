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

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/maths
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/system
  ${ROSE_SOURCE_DIR}/projects/pragmaParsing
  ${with-ppl}/include)

add_library(PolyModelSystem system/Utils.cpp)

add_library(PolyModelMaths
  maths/Exception-maths.cpp maths/PolyhedronExploration.cpp maths/Quast.cpp
  maths/biLinear.cpp maths/Farkas.cpp)
target_link_libraries(PolyModelMaths PolyModelSystem ${libppl})

add_library(PolyModelCommon
  common/Exception.cpp common/ScheduleSpace.cpp)
target_link_libraries(PolyModelCommon PolyModelMaths)

add_library(PolyModelRose
  rose/rose-utils.cpp rose/Exception-rose.cpp rose/Variable.cpp
  rose/Parser.cpp rose/Quast-rose.cpp)

add_library(PolyModelRoseKernel rose-kernel/PolyhedralKernel.cpp)
target_link_libraries(PolyModelRoseKernel PolyModelRose)

add_library(PolyModelRosePragma
  rose-pragma/PolyhedricPragmaParser.cpp rose-pragma/Exception-rose-pragma.cpp
  rose-pragma/PolyhedricPragma.cpp)
target_link_libraries(PolyModelRosePragma PolyModelRose)

add_library(PolyModelRoseMaxCover rose-max-cover/TDBUtraversal.cpp)
add_library(PolyModelScopLib scoplib/Exception-ScopLib.cpp)

add_library(PolyModel libPolyModelDummyFile.cpp)
target_link_libraries(PolyModel
  PolyModelCommon PolyModelRosePragma PolyModelScopLib)
