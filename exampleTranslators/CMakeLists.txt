if(NOT disable-example-translators-directory)
  include_directories(${ROSE_INCLUDES})
  add_subdirectory(AstCopyReplTester)
  add_subdirectory(defaultTranslator)
  add_subdirectory(documentedExamples)
  add_subdirectory(DOTGenerator)
  add_subdirectory(PDFGenerator)
endif()

