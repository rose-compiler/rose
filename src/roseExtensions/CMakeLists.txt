if(SQLITE3_FOUND)
  add_subdirectory(sqlite3x)
endif()
add_subdirectory(dataStructureTraversal)
add_subdirectory(highLevelGrammar)
add_subdirectory(failSafe)

if(enable-rosehpct)
  find_package(LibXml2)
  if(NOT LIBXML2_FOUND)
    message(ERROR "ROSE_HPCT module requires libxml2")
  endif()
  add_subdirectory(roseHPCToolkit)
endif()

if(with-roseQt)
  add_subdirectory(qtWidgets)
endif()

