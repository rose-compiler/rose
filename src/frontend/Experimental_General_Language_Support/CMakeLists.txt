if(ENABLE-JOVIAL)
  add_subdirectory(ATerm)

  # Library target for libroseTreeBuilder
  add_library(roseTreeBuilder STATIC 
    SageTreeBuilder.C
    ModuleBuilder.C
    PosInfo.C
    Tokens.C
  )

  target_include_directories(roseTreeBuilder PRIVATE
    ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_Jovial_ROSE_Connection
  )
endif()
