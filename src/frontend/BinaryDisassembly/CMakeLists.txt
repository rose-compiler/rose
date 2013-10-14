
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### next target ###############

#BEGIN ***************** GENERATING armInstructionEnumPrinter.C *******************
FILE(READ armInstructionEnum.h contents)

file( REMOVE ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C )
file( WRITE  ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C "\#include \"armInstructionEnum.h\"\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C "\#include <string>\n" )

# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
#
STRING(REGEX REPLACE ";" "\\\\;" contents "${contents}")
STRING(REGEX REPLACE "\n" ";" contents "${contents}")
#STRING(REGEX REPLACE ", *//.*;" ".*,,;" contents "${contents}")

set(found_first FALSE)

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C "static const char* armInstructionStrings[] = {\n" )
foreach(v ${contents})
  if(v MATCHES "arm_unknown_instruction.*" )
    set(found_first TRUE)
  endif()

  if( found_first )
    string( REGEX REPLACE ", *//.*" "" v2 ${v}  )
    string( REGEX MATCH "[a-zA-Z0-9_]+" v3 ${v2}  )
    set(v4 "\"${v3}\",")
    string( REGEX REPLACE "_record" "." v4 ${v4}  )
    file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C "${v4}\n" )
  endif()
  if( v MATCHES ".*arm_last_instruction.*" )
    break()
  endif()
endforeach()

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C "\"\"};\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C "std::string toString(ArmInstructionKind k) {static const int s = sizeof(armInstructionStrings) / sizeof(*armInstructionStrings); if ((int)k < 0 || (int)k >= s) return \"\"; else return armInstructionStrings[(int)k];}\n" )

#END ***************** GENERATING armInstructionEnumPrinter.C *******************

#***************** GENERATING powerpcInstructionEnumPrinter.C *******************

FILE(READ powerpcInstructionEnum.h contents)

file( REMOVE ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C )
file( WRITE  ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C "\#include \"powerpcInstructionEnum.h\"\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C "\#include <string>\n" )

# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
#
STRING(REGEX REPLACE ";" "\\\\;" contents "${contents}")
STRING(REGEX REPLACE "\n" ";" contents "${contents}")
#STRING(REGEX REPLACE ", *//.*;" ".*,,;" contents "${contents}")

set(found_first FALSE)

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C "static const char* powerpcInstructionStrings[] = {\n" )
foreach(v ${contents})
  if(v MATCHES "powerpc_unknown_instruction.*" )
    set(found_first TRUE)
  endif()

  if( found_first )
    string( REGEX REPLACE ", *//.*" "" v2 ${v}  )
    string( REGEX MATCH "[a-zA-Z0-9_]+" v3 ${v2}  )
    set(v4 "\"${v3}\",")
    string( REGEX REPLACE "_record" "." v4 ${v4}  )
    file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C "${v4}\n" )
  endif()
  if( v MATCHES ".*powerpc_last_instruction.*" )
    break()
  endif()
endforeach()

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C "\"\"};\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C "std::string toString(PowerpcInstructionKind k) {static const int s = sizeof(powerpcInstructionStrings) / sizeof(*powerpcInstructionStrings); if ((int)k < 0 || (int)k >= s) return \"\"; else return powerpcInstructionStrings[(int)k];}\n" )

#END ***************** GENERATING powerpcInstructionEnumPrinter.C *******************


########### install files ###############

#install(FILES  $(BINARY_DISASSEMBLY_HEADER_FILES) DESTINATION ${INCLUDE_INSTALL_DIR})
