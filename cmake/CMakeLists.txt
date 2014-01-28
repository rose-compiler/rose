configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/bundle.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake
  @ONLY)
install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake)
