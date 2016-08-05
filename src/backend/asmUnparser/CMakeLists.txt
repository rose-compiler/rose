if (enable-binary-analysis)
add_library(roseAsmUnparser OBJECT
  unparseX86Asm.C
  unparseArmAsm.C
  unparsePowerpcAsm.C
  unparseM68kAsm.C
  unparseMipsAsm.C
  AsmUnparser.C
  AsmUnparser_compat.C
  AsmFunctionIndex.C)
else()
add_library(roseAsmUnparser OBJECT
  dummyAsmUnparser.C
)
endif()
add_dependencies(roseAsmUnparser rosetta_generated)

install(FILES
  AsmUnparser.h AsmUnparser_compat.h AsmFunctionIndex.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
