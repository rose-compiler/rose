#-------------------------------------------------------------------------------
add_executable(testSymbolTable testSymbolTable.C)
target_link_libraries(testSymbolTable
  ROSE_DLL EDG ${link_with_libraries})

add_test(
  NAME testSymbolTable_test1
  COMMAND testSymbolTable ${CMAKE_CURRENT_SOURCE_DIR}/input.C
)
