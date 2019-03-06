if(PYTHONINTERP_FOUND)
  include_directories(${ROSE_SOURCE_DIR}/projects/DatalogAnalysis/include)

  add_subdirectory(relationTranslatorGenerator)
  add_subdirectory(src)
endif()
