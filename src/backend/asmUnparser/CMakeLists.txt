add_library(roseAsmUnparser OBJECT
  AsmFunctionIndex.C
  AsmUnparser.C
  AsmUnparser_compat.C
  BinaryEdgeArrows.C
  BinarySerialIo.C
  BinaryUnparserAarch32.C
  BinaryUnparserAarch64.C
  BinaryUnparserBase.C
  BinaryUnparserM68k.C
  BinaryUnparserMips.C
  BinaryUnparserNull.C
  BinaryUnparserPowerpc.C
  BinaryUnparserX86.C
  unparseAarch32Asm.C
  unparseAarch64Asm.C
  unparseM68kAsm.C
  unparseMipsAsm.C
  unparsePowerpcAsm.C
  unparseX86Asm.C)

add_dependencies(roseAsmUnparser rosetta_generated)

install(FILES
    AsmFunctionIndex.h
    AsmUnparser_compat.h
    AsmUnparser.h
    BinaryEdgeArrows.h
    BinarySerialIo.h
    BinaryUnparserAarch32.h
    BinaryUnparserAarch64.h
    BinaryUnparserBase.h
    BinaryUnparser.h
    BinaryUnparserM68k.h
    BinaryUnparserMips.h
    BinaryUnparserNull.h
    BinaryUnparserPowerpc.h
    BinaryUnparserX86.h
    DESTINATION ${INCLUDE_INSTALL_DIR})
