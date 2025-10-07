add_library(SingleStatementToBlockNormalization OBJECT SingleStatementToBlockNormalization.C)
add_dependencies(SingleStatementToBlockNormalization rosetta_generated)

target_include_directories(SingleStatementToBlockNormalization PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)

install(FILES SingleStatementToBlockNormalization.h DESTINATION ${INCLUDE_INSTALL_DIR})
