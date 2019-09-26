if(SQLITE3_FOUND_LIB)
  add_library(RoseSQLite3xDatabase
    sqlite3x_command.C sqlite3x_exception.C sqlite3x_transaction.C
    sqlite3x_connection.C sqlite3x_reader.C)

  # Must link to roseUtil in order to have access to ROSE_AUTOMAKE_* strings
  target_link_libraries(RoseSQLite3xDatabase ${SQLITE3_LIBRARY} ${link_with_libraries})

  install(FILES sqlite3x.h DESTINATION ${INCLUDE_INSTALL_DIR})
endif()
