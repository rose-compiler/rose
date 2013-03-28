add_subdirectory(gui)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/split-into-tables.tcl
  ${ROSE_BINARY_DIR}/projects/AstEquivalence/
  COPYONLY)

include_directories(${QT_QT3SUPPORT_INCLUDE_DIR})
add_definitions(-DQT3_SUPPORT)


add_executable(astEquivalence astEquivalenceGui.C astEquivalenceMainGui.C)
target_link_libraries(astEquivalence ROSE_DLL qrose ${Boost_LIBRARIES}
  ${QT_LIBRARIES})

add_executable(createTextFiles createTextFiles.C)
target_link_libraries(createTextFiles ROSE_DLL ${Boost_LIBRARIES})

add_executable(runIDA runIDA.C)
target_link_libraries(runIDA ROSE_DLL ${Boost_LIBRARIES})

include_directories(${ROSE_SOURCE_DIR}/src/roseExtensions/sqlite3x)
add_executable(evaluateAllBinaries evaluateAllBinaries.C)
target_link_libraries(evaluateAllBinaries
  ROSE_DLL RoseSQLite3xDatabase ${Boost_LIBRARIES})


