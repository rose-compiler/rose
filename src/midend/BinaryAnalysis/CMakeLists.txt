if(SQLITE3_FOUND_LIB)
  list(APPEND midend_binary_sources
    libraryIdentification/FunctionIdDatabaseInterface.C
    #libraryIdentification/functionIdentification.C
    #libraryIdentification/libraryIdentification_reader.C
    libraryIdentification/libraryIdentification.C)
else()
  list(APPEND midend_binary_sources
    libraryIdentification/dummy.C)
endif()

add_library(midend_binary OBJECT ${midend_binary_sources})
add_dependencies(midend_binary rosetta_generated)

install(FILES
  ether.h
  libraryIdentification/FunctionIdDatabaseInterface.h
  libraryIdentification/FunctionInfo.h
  libraryIdentification/libraryIdentification.h
  libraryIdentification/LibraryInfo.h

  DESTINATION ${INCLUDE_INSTALL_DIR})
