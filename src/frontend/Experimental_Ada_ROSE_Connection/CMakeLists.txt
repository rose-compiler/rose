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

  add_library(experimentalRoseAda SHARED ${experimental_rose_ada_LIB_SRC})
  target_include_directories(experimentalRoseAda PUBLIC ${ADA_SRC_DIR}/parser/asis_adapter/include ${ADA_SRC_DIR}/parser/a_nodes/include)
  add_dependencies(experimentalRoseAda sage3 asis_adapter)
  target_link_libraries(experimentalRoseAda PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/parser/asis_adapter/lib/libasis_adapter.so)
endif()
