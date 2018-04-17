add_executable(identityTranslator identityTranslator.C)
target_link_libraries(identityTranslator ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS identityTranslator DESTINATION bin)
