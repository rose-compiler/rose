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
 
  # Find libasis.so 
  # find_library(ASIS_LIB NAMES asis
  #             PATHS "$ENV{GNAT_HOME}/lib"
  #             NO_DEFAULT_PATH) 
  #if(ASIS_LIB) 
  #  message(STATUS "Found libasis: ${ASIS_LIB}") 
  #else() 
  #  message(FATAL_ERROR "Did not find libasis") 
  #endif() 

  # Find libadalang.so 
  #find_library(ADA_LIB NAMES adalang
  #             PATHS "$ENV{GNAT_HOME}/lib"
  #             NO_DEFAULT_PATH) 
  #if(ADA_LIB) 
  #  message(STATUS "Found libadalang.so: ${ADA_LIB}") 
  #else() 
  #  message(FATAL_ERROR "Did not find libadalang.so") 
  #endif() 
  # Finish checks
  
  add_library(experimentalRoseAda SHARED ${experimental_rose_ada_LIB_SRC})
  target_include_directories(experimentalRoseAda PUBLIC ${ADA_SRC_DIR}/parser/asis_adapter/include ${ADA_SRC_DIR}/parser/a_nodes/include)
  add_dependencies(experimentalRoseAda sage3 asis_adapter)
  target_link_libraries(experimentalRoseAda PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/parser/asis_adapter/lib/libasis_adapter.so) 
  #link_directories(CMAKE_BINARY_DIR/lib) 
  #target_link_libraries(experimentalRoseAda PUBLIC ${ASIS_LIB} ${ADA_LIB} ${CMAKE_CURRENT_BINARY_DIR}/parser/asis_adapter/lib/libasis_adapter.so)
endif()
 
