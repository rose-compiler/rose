if(ENABLE_EXAMPLE_TRANSLATORS_DIRECTORY)
  include_directories(${ROSE_INCLUDES})
  add_subdirectory(DOTGenerator)
  add_subdirectory(AstCopyReplTester)
  add_subdirectory(defaultTranslator)
  add_subdirectory(documentedExamples)
  add_subdirectory(PDFGenerator)
endif()

