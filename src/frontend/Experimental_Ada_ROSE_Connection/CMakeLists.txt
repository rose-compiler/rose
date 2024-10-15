# Build experimentalRoseAda library and link it to  libasis_adapter
if(ENABLE-ADA)
  # Process an additional CMakeLists.txt in this directory 
  add_subdirectory(parser)
 
  # Define source files for experimentalRoseAda object library
  set(experimental_rose_ada_LIB_SRC
      ada_support.C 
      Ada_to_ROSE_translation.C 
      Ada_to_ROSE.C 
      AdaExpression.C 
      AdaMaker.C 
      AdaStatement.C 
      AdaType.C)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    message(STATUS "Environment vairable GNAT_HOME: $ENV{GNAT_HOME}")
  endif()

  # Check that these directories exist before including header files from them when building the target  
  set(ADA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

  if(NOT EXISTS ${ADA_SRC_DIR}/parser/asis_adapter/include)
    message(FATAL_ERROR "Trying to include a directory that does not exist"
                        "${ADA_SRC_DIR}/parser/asis_adapter/include")
  endif()
  if(NOT EXISTS ${ADA_SRC_DIR}/parser/a_nodes/include)
    message(FATAL_ERROR "Trying to include a directory that does not exist"
                        "${ADA_SRC_DIR}/parser/a_nodes/include")
  endif()

  # Create the library experimentalRoseAda 
  add_library(experimentalRoseAda OBJECT ${experimental_rose_ada_LIB_SRC})
  
  # Use headers located here 
  target_include_directories(experimentalRoseAda PUBLIC ${ADA_SRC_DIR}/parser/asis_adapter/include ${ADA_SRC_DIR}/parser/a_nodes/include)
  
  # Wait until sage3 and asis_adapter are built 
  add_dependencies(experimentalRoseAda sage3 asis_adapter)
  
  # Link this library to asis_adapter 
  target_link_libraries(experimentalRoseAda PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/parser/asis_adapter/lib/libasis_adapter.so) 
  
  # Install these FILES to DESTINATION  when the user runs "make install" 
  install(FILES AdaMaker.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/rose) 
endif()
