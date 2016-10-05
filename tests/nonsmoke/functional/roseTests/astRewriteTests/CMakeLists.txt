if (NOT CYGWIN)
  set(TAU_INCLUDES "-I${CMAKE_SOURCE_DIR}/tutorial/TAU_INCLUDE_DIR")

  #-----------------------------------------------------------------------------
  add_executable(testIncludeDirectiveInsertion
    testIncludeDirectiveInsertion.C)
  target_link_libraries(testIncludeDirectiveInsertion
    ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME IncludeDirectiveInsertion
    COMMAND testIncludeDirectiveInsertion -rose:verbose 0
      "-I${CMAKE_CURRENT_SOURCE_DIR}" "${TAU_INCLUDES}"
      "${CMAKE_CURRENT_SOURCE_DIR}/inputProgram6.C"
  )
endif()

# the rest of the tests in Makefile.am appeared to be disabled.
