add_executable(testMerge testMerge.C)
target_link_libraries(testMerge ROSE_DLL EDG ${link_with_libraries})
install(TARGETS testMerge DESTINATION bin)

add_test(
  NAME testMerge_test1
  COMMAND testMerge -rose:verbose 0 -rose:ast:merge
          -c ${CMAKE_CURRENT_SOURCE_DIR}/mangleTest.C
          ${CMAKE_CURRENT_SOURCE_DIR}/mangleTwo.C
)

add_test(
  NAME testMerge_test2
  COMMAND testMerge -rose:verbose 0
          -c ${CMAKE_CURRENT_SOURCE_DIR}/mangleTest.C
          ${CMAKE_CURRENT_SOURCE_DIR}/mangleTwo.C
          ${CMAKE_CURRENT_SOURCE_DIR}/mangleThree.C
)

add_test(
  NAME testMerge_test3
  COMMAND testMerge -rose:verbose 0 -rose:ast:merge
          -c ${CMAKE_CURRENT_SOURCE_DIR}/mangleTest.C
          ${CMAKE_CURRENT_SOURCE_DIR}/mangleTwo.C
          ${CMAKE_CURRENT_SOURCE_DIR}/mangleThree.C
)
