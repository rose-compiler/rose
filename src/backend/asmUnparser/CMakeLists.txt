add_library(roseAsmUnparser OBJECT
  unparseX86Asm.C
  unparseArmAsm.C
  unparsePowerpcAsm.C
  unparseM68kAsm.C
  unparseMipsAsm.C
  AsmUnparser.C
  AsmUnparser_compat.C
  AsmFunctionIndex.C)
add_dependencies(roseAsmUnparser rosetta_generated generate_stringify)

install(FILES
  AsmUnparser.h AsmUnparser_compat.h AsmFunctionIndex.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
