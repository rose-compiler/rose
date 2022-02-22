list(APPEND midend_binary_sources
    libraryIdentification/libraryIdentification.C)

add_library(midend_binary OBJECT ${midend_binary_sources})
add_dependencies(midend_binary rosetta_generated)

install(FILES
  ether.h
  libraryIdentification/FunctionInfo.h
  libraryIdentification/libraryIdentification.h
  libraryIdentification/LibraryInfo.h

  DESTINATION ${INCLUDE_INSTALL_DIR})
