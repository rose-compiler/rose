if(ENABLE-JOVIAL)

  # Executable target for the Jovial preprocessor
  add_executable(jovial_preprocess
    jovial_preprocess.C
    preprocess.C
  )

  target_include_directories(jovial_preprocess PRIVATE
    ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support
  )

  # Library target for libroseJovial
  add_library(roseJovial STATIC
    jovial_support.C 
    ATermToSageJovialTraversal.C 
    preprocess.C
  )

  target_include_directories(roseJovial PUBLIC
    ${ATERM_ROOT}/include
    ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support
  )

  # Install header and executable files
  install(FILES jovial_support.h DESTINATION include/rose)
  install(TARGETS jovial_preprocess DESTINATION bin)

endif()
