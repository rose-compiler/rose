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

  # Create the library experimentalRoseAda 
  add_library(experimentalRoseAda OBJECT ${experimental_rose_ada_LIB_SRC})
  
  # Use headers located here
  target_include_directories(experimentalRoseAda PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/parser/asis_adapter/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/parser/a_nodes/include>
    $<INSTALL_INTERFACE:include/rose>
  )
  
  # Wait until sage3 and asis_adapter are built 
  add_dependencies(experimentalRoseAda sage3 asis_adapter)
  
  # Link this library to asis_adapter 
  target_link_libraries(experimentalRoseAda PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/parser/asis_adapter/lib/libasis_adapter.so) 
  
  # Install these FILES to DESTINATION  when the user runs "make install" 
  install(FILES AdaMaker.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/rose) 
endif()
