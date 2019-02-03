if(NOT enable-internalFrontendDevelopment)
  # I'm not sure whether these are supposed to be installed, but I bet they're not supposed to be installed into the
  # $PREFIX/lib directory.
  #install(FILES  antlr-3.2.jar antlr-3.3-complete.jar DESTINATION ${ROSE_LIB_DIR_NAME})
endif()
