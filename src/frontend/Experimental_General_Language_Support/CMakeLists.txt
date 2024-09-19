if(ENABLE-JOVIAL)
  add_subdirectory(ATerm)

  # Library target for libroseTreeBuilder
  add_library(roseTreeBuilder OBJECT 
    SageTreeBuilder.C
    ModuleBuilder.C
    PosInfo.C
    Tokens.C
  )
 
  # Rosetta has to be built before this to find Cxx_Grammar.h
  add_dependencies(roseTreeBuilder rosetta_generated) 

  target_include_directories(roseTreeBuilder PRIVATE
    ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_Jovial_ROSE_Connection
  )
endif()
