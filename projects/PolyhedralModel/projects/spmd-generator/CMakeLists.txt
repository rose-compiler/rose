add_library(SPMDizeCommon
  common/spmd-generator.cpp common/spmd-tree.cpp common/spmd-driver.cpp
  common/comm-analysis.cpp common/placement.cpp common/compute-system.cpp
  common/array-analysis.cpp)
target_link_libraries(SPMDizeCommon ToolBoxes)

add_library(SPMDizePolyDriver
  polydriver/polyhedral-driver.cpp polydriver/polyhedral-placement.cpp
  polydriver/polyhedral-array-analysis.cpp polydriver/polyhedral-utils.cpp)
target_link_libraries(SPMDizePolyDriver PolyModel PolyModelRose)

add_library(PMDizeOpenCLgen opencl-generator/opencl-generator.cpp)

add_library(ToolBoxes
  toolboxes/rose-toolbox.cpp toolboxes/algebra-container.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/rose/Variable.cpp)

add_library(ComputeSystem
  compute-systems/array-system.cpp compute-systems/gpu-system.cpp
  compute-systems/compute-node.cpp compute-systems/cluster-system.cpp
  compute-systems/memory.cpp)

add_executable(identity dummy-test/identity.cpp)
target_link_libraries(identity
  SPMDizeCommon SPMDizePolyDriver ${libppl} ROSE_DLL ${Boost_LIBRARIES})

add_executable(single_gpu
  single-gpu/single-gpu.cpp single-gpu/depth-placement.cpp
  single-gpu/test.cpp)
target_link_libraries(single_gpu
  SPMDizeCommon SPMDizePolyDriver ComputeSystem SPMDizeOpenCLgen ${libppl}
  ROSE_DLL ${Boost_LIBRARIES})
