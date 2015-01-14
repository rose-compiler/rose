### Generate armInstructionEnumPrinter.C ###
FILE(READ armInstructionEnum.h contents)

file( REMOVE ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp )
file( WRITE  ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp "\#include \"armInstructionEnum.h\"\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp "\#include <string>\n" )

# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
#
STRING(REGEX REPLACE ";" "\\\\;" contents "${contents}")
STRING(REGEX REPLACE "\n" ";" contents "${contents}")
#STRING(REGEX REPLACE ", *//.*;" ".*,,;" contents "${contents}")

set(found_first FALSE)

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp "static const char* armInstructionStrings[] = {\n" )
foreach(v ${contents})
  if(v MATCHES "arm_unknown_instruction.*" )
    set(found_first TRUE)
  endif()

  if( found_first )
    string( REGEX REPLACE ", *//.*" "" v2 ${v}  )
    string( REGEX MATCH "[a-zA-Z0-9_]+" v3 ${v2}  )
    set(v4 "\"${v3}\",")
    string( REGEX REPLACE "_record" "." v4 ${v4}  )
    file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp "${v4}\n" )
  endif()
  if( v MATCHES ".*arm_last_instruction.*" )
    break()
  endif()
endforeach()

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp "\"\"};\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp "std::string toString(ArmInstructionKind k) {static const int s = sizeof(armInstructionStrings) / sizeof(*armInstructionStrings); if ((int)k < 0 || (int)k >= s) return \"\"; else return armInstructionStrings[(int)k];}\n" )

execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C.tmp
          ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/armInstructionEnumPrinter.C
)


### Generate powerpcInstructionEnumPrinter.C ###

FILE(READ powerpcInstructionEnum.h contents)

file( REMOVE ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp )
file( WRITE  ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp "\#include \"powerpcInstructionEnum.h\"\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp "\#include <string>\n" )

# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
#
STRING(REGEX REPLACE ";" "\\\\;" contents "${contents}")
STRING(REGEX REPLACE "\n" ";" contents "${contents}")
#STRING(REGEX REPLACE ", *//.*;" ".*,,;" contents "${contents}")

set(found_first FALSE)

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp "static const char* powerpcInstructionStrings[] = {\n" )
foreach(v ${contents})
  if(v MATCHES "powerpc_unknown_instruction.*" )
    set(found_first TRUE)
  endif()

  if( found_first )
    string( REGEX REPLACE ", *//.*" "" v2 ${v}  )
    string( REGEX MATCH "[a-zA-Z0-9_]+" v3 ${v2}  )
    set(v4 "\"${v3}\",")
    string( REGEX REPLACE "_record" "." v4 ${v4}  )
    file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp "${v4}\n" )
  endif()
  if( v MATCHES ".*powerpc_last_instruction.*" )
    break()
  endif()
endforeach()

file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp "\"\"};\n" )
file( APPEND ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp "std::string toString(PowerpcInstructionKind k) {static const int s = sizeof(powerpcInstructionStrings) / sizeof(*powerpcInstructionStrings); if ((int)k < 0 || (int)k >= s) return \"\"; else return powerpcInstructionStrings[(int)k];}\n" )

execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C.tmp
          ${ROSE_TOP_BINARY_DIR}/src/frontend/BinaryDisassembly/powerpcInstructionEnumPrinter.C
)


### Generate the library ###
add_library(binaryFrontend OBJECT
  RoseBin_support.cpp instructionDispatch.cpp x86InstructionProperties.h
  x86InstructionProperties.C armInstructionEnum.h powerpcInstructionEnum.h
  powerpcInstructionProperties.h powerpcInstructionProperties.C readTicl.C
  RoseBin_file.cpp RoseFile.cpp RoseBin_buildTree.cpp
  RoseBin_IDAPRO_buildTree.cpp AST_BIN_Traversal.C
  ${CMAKE_CURRENT_BINARY_DIR}/armInstructionEnumPrinter.C
  ${CMAKE_CURRENT_BINARY_DIR}/powerpcInstructionEnumPrinter.C
)
add_dependencies(binaryFrontend rosetta_generated)


########### install files ###############

install(FILES
    armInstructionEnum.h AST_BIN_Traversal.h MyAstAttribute.h
    powerpcInstructionEnum.h powerpcInstructionProperties.h readTicl.h
    RoseBin_abstract.h RoseBin_buildTree.h RoseBin_file.h
    RoseBin_IDAPRO_branchGraph.h RoseBin_IDAPRO_buildTree.h
    RoseBin_IDAPRO_callGraph.h RoseBin_IDAPRO_exprTree.h
    RoseBin_IDAPRO_substTree.h RoseBin_support.h RoseFile.h
    RoseObj.h x86InstructionProperties.h
  DESTINATION include)

