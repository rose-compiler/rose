add_library(roseBinaryAnalysisUnparser OBJECT
  EdgeArrows.C
  Aarch32.C
  Aarch64.C
  Base.C
  Cil.C
  Jvm.C
  M68k.C
  Mips.C
  Null.C
  Powerpc.C
  X86.C)

add_dependencies(roseBinaryAnalysisUnparser rosetta_generated)

install(FILES
  EdgeArrows.h
  Aarch32.h
  Aarch64.h
  Base.h
  Cil.h
  Jvm.h
  M68k.h
  Mips.h
  Null.h
  Powerpc.h
  Settings.h
  X86.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Unparser)
