add_library(roseAsmUnparser OBJECT
  AsmUnparser.C
  AsmUnparser_compat.C
  AsmFunctionIndex.C
  BinaryEdgeArrows.C
  BinarySerialIo.C
  BinaryUnparserArm.C
  BinaryUnparserBase.C
  BinaryUnparserM68k.C
  BinaryUnparserMips.C
  BinaryUnparserPowerpc.C
  BinaryUnparserX86.C
  unparseA64Asm.C
  unparseM68kAsm.C
  unparseMipsAsm.C
  unparsePowerpcAsm.C
  unparseX86Asm.C)

add_dependencies(roseAsmUnparser rosetta_generated)

install(FILES
    AsmUnparser.h
    AsmUnparser_compat.h
    AsmFunctionIndex.h
    BinaryEdgeArrows.h
    BinarySerialIo.h
    BinaryUnparser.h
    BinaryUnparserArm.h
    BinaryUnparserBase.h
    BinaryUnparserM68k.h
    BinaryUnparserMips.h
    BinaryUnparserPowerpc.h
    BinaryUnparserX86.h
    DESTINATION ${INCLUDE_INSTALL_DIR})
