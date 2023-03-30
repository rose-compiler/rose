add_library(roseBinaryAnalysisDisassembler OBJECT
  Aarch32.C
  Aarch64.C
  Base.C
  BasicTypes.C
  Cil.C
  Exception.C
  Jvm.C
  M68k.C
  Mips.C
  Null.C
  Powerpc.C
  X86.C)

add_dependencies(roseBinaryAnalysisDisassembler rosetta_generated)

install(FILES
  Aarch32.h
  Aarch64.h
  Base.h
  BasicTypes.h
  Cil.h
  Exception.h
  Jvm.h
  M68k.h
  Mips.h
  Null.h
  Powerpc.h
  X86.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Disassembler)
