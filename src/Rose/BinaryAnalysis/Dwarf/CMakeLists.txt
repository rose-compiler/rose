add_library(roseBinaryAnalysisDwarf
  OBJECT
  Dwarf.C)

add_dependencies(roseBinaryAnalysisDwarf rosetta_generated)

install(FILES
  BasicTypes.h
  Constants.h
  Exception.h

  DESTINATION
  ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Dwarf)
