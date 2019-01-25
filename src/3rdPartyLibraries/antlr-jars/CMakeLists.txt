if(NOT enable-internalFrontendDevelopment)
  install(FILES  antlr-3.2.jar antlr-3.3-complete.jar
    DESTINATION ${ROSE_LIB_DIR_NAME})
endif()
