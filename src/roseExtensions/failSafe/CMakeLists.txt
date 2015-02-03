add_library(failsafe OBJECT failSafePragma.C)
add_dependencies(failsafe rosetta_generated)

install(FILES failSafePragma.h DESTINATION ${INCLUDE_INSTALL_DIR})
