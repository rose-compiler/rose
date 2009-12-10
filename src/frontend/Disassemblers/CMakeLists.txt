
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(roseDisassemblers_SRC
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/Partitioner.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/Disassembler.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/DisassemblerArm.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/DisassemblerPowerpc.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/DisassemblerX86.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/Assembler.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init1.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init2.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init3.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init4.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init5.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init6.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init7.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init8.C
   ${CMAKE_SOURCE_DIR}/src/frontend/Disassemblers/AssemblerX86Init9.C
   )

add_library( roseDisassemblers STATIC ${roseDisassemblers_SRC} ) 
add_dependencies( roseDisassemblers ROSETTA  )

########### install files ###############

install(FILES  Partitioner.h  	Disassembler.h DisassemblerArm.h DisassemblerPowerpc.h DisassemblerX86.h  	Assembler.h AssemblerX86.h AssemblerX86Init.h  DESTINATION ${INCLUDE_INSTALL_DIR})
