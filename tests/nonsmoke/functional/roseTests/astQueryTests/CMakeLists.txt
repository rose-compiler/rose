include_directories(${CMAKE_BINARY_DIR}/src/roseSupport)

#-------------------------------------------------------------------------------
add_executable(testQuery testQuery.C)
target_link_libraries(testQuery ROSE_DLL EDG ${link_with_libraries})

add_test(
  NAME testQuery_input1.C
  COMMAND testQuery -c ${CMAKE_CURRENT_SOURCE_DIR}/input1.C
)

#-------------------------------------------------------------------------------
add_executable(testQuery2 testQuery2.C)
target_link_libraries(testQuery2 ROSE_DLL EDG ${link_with_libraries})

add_test(
  NAME testQuery2_input1.C
  COMMAND testQuery2 -c ${CMAKE_CURRENT_SOURCE_DIR}/input1.C
)

#-------------------------------------------------------------------------------
add_executable(testQuery3 testQuery3.C)
target_link_libraries(testQuery3 ROSE_DLL EDG ${link_with_libraries})

add_test(
  NAME testQuery3_input1.C
  COMMAND testQuery3 -c ${CMAKE_CURRENT_SOURCE_DIR}/input1.C
)

install(TARGETS testQuery testQuery2 testQuery3 DESTINATION bin)
