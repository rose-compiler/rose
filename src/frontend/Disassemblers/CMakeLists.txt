
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

#add_library( roseDisassemblers STATIC ${roseDisassemblers_SRC} ) 
#add_dependencies( roseDisassemblers ROSETTA  )

########### install files ###############

install(FILES  Partitioner.h Disassembler.h DisassemblerArm.h DisassemblerPowerpc.h DisassemblerX86.h Assembler.h AssemblerX86.h AssemblerX86Init.h  DESTINATION ${INCLUDE_INSTALL_DIR})
