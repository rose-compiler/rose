project(ROSEPLUGIN)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules"  ${CMAKE_MODULE_PATH})
include(FindROSE)
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED)
find_package(ROSE REQUIRED)
find_package(Boost REQUIRED)

#set(BUILD_SHARED_LIBS OFF CACHE BOOL "Default to building a static mondo-lib")
#if (NOT BUILD_SHARED_LIBS)
#  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#else()
#  if (CMAKE_CXX_VISIBILITY_PRESET STREQUAL "hidden")
#    message(FATAL_ERROR "CMAKE_CXX_VISIBILITY_PRESET=hidden is incompatible \
#                         with BUILD_SHARED_LIBS.")
#  endif()
#endif()

set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 0)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_CMAKE_DIR}"
  )

# import LLVM CMake functions
include(AddLLVM)

if( NOT MSVC ) # MSVC mangles symbols differently, and
               # RoseConnectionClangPlugin.export contains C++ symbols.
  if( NOT LLVM_REQUIRES_RTTI )
    if( NOT LLVM_REQUIRES_EH )
      set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/RoseConnectionClangPlugin.exports)
    endif()
  endif()
endif()

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS} ${ROSE_ROOT}/include ${CLANG_INCLUDE_DIRS} ${ROSE_ROOT}/include/rose ${Boost_INCLUDE_DIRS})
add_library(RoseConnectionClangPlugin MODULE RoseConnectionClangPlugin.cpp)
if (APPLE)
   target_link_libraries(RoseConnectionClangPlugin clangAST clangBasic clangFrontend rose -L${ROSE_ROOT}/lib)
else ()
   target_link_libraries(RoseConnectionClangPlugin  rose -L${ROSE_ROOT}/lib)
endif ()
#target_link_libraries(RoseConnectionClangPlugin clangAST clangBasic clangFrontend)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(RoseConnectionClangPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()


add_library(RoseConnectionLLVMPassPlugin MODULE RoseConnectionLLVMPassPlugin.cpp)
target_compile_features(RoseConnectionLLVMPassPlugin PRIVATE cxx_range_for cxx_auto_type)
target_link_libraries(RoseConnectionLLVMPassPlugin rose -L${ROSE_ROOT}/lib)

if(APPLE)
    set_target_properties(RoseConnectionLLVMPassPlugin PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)


