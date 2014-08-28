if(SQLITE3_FOUND)
  add_subdirectory(sqlite3x)
endif()
add_subdirectory(dataStructureTraversal)
add_subdirectory(highLevelGrammar)
add_subdirectory(failSafe)

if(enable-rosehpct)
  add_subdirectory(roseHPCToolkit)
endif()

if(with-roseQt)
  add_subdirectory(qtWidgets)
endif()
