add_library(roseAsmUnparser OBJECT
  AsmUnparser.C
  AsmUnparser_compat.C)

add_dependencies(roseAsmUnparser rosetta_generated)

install(FILES
    AsmUnparser_compat.h
    AsmUnparser.h

    DESTINATION ${INCLUDE_INSTALL_DIR})
