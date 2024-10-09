if(ENABLE-ADA)
  add_subdirectory(parser)

  set(ADA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

  set(experimental_rose_ada_LIB_SRC
      ada_support.C 
      Ada_to_ROSE_translation.C 
      Ada_to_ROSE.C 
      AdaExpression.C 
      AdaMaker.C 
      AdaStatement.C 
      AdaType.C)

  # Debugging
  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    message(STATUS "ADA SRC DIR: ${ADA_SRC_DIR}")
    message(STATUS "cmake cur binary ${CMAKE_CURRENT_BINARY_DIR}")
    message(STATUS "gnat home : $ENV{GNAT_HOME}")
  endif()

  # Check that these include directories exist before including them to the target
  if(NOT EXISTS ${ADA_SRC_DIR}/parser/asis_adapter/include)
    message(FATAL_ERROR "Trying to include a directory that does not exist"
                        "${ADA_SRC_DIR}/parser/asis_adapter/include")
  endif()
  if(NOT EXISTS ${ADA_SRC_DIR}/parser/a_nodes/include)
    message(FATAL_ERROR "Trying to include a directory that does not exist"
                        "${ADA_SRC_DIR}/parser/a_nodes/include")
  endif()
 
  add_library(experimentalRoseAda OBJECT ${experimental_rose_ada_LIB_SRC})
  target_include_directories(experimentalRoseAda PUBLIC ${ADA_SRC_DIR}/parser/asis_adapter/include ${ADA_SRC_DIR}/parser/a_nodes/include)
  add_dependencies(experimentalRoseAda sage3 asis_adapter)
  target_link_libraries(experimentalRoseAda PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/parser/asis_adapter/lib/libasis_adapter.so) 
endif()
 
