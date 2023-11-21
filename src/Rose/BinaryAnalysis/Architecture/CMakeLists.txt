add_library(roseBinaryAnalysisArchitecture OBJECT
  Amd64.C
  ArmAarch32.C
  ArmAarch64.C
  Base.C
  BasicTypes.C
  Cil.C
  Exception.C
  Intel80286.C
  Intel8086.C
  Intel8088.C
  IntelI386.C
  IntelI486.C
  IntelPentium.C
  IntelPentiumii.C
  IntelPentiumiii.C
  IntelPentium4.C
  Jvm.C
  Mips32.C
  Motorola68040.C
  NxpColdfire.C
  Powerpc32.C
  Powerpc64.C
)

add_dependencies(roseBinaryAnalysisArchitecture rosetta_generated)

install(FILES
  Amd64.h
  ArmAarch32.h
  ArmAarch64.h
  Base.h
  BasicTypes.h
  Cil.h
  Exception.h
  Intel80286.h
  Intel8086.h
  Intel8088.h
  IntelI386.h
  IntelI486.h
  IntelPentium.h
  IntelPentiumii.h
  IntelPentiumiii.h
  IntelPentium4.h
  Jvm.h
  Mips32.h
  Motorola68040.h
  NxpColdfire.h
  Powerpc32.h
  Powerpc64.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Architecture)

