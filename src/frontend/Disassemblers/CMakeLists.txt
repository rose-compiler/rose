add_library(roseDisassemblers OBJECT Assembler.C AssemblerX86.C
  AssemblerX86Init1.C AssemblerX86Init2.C AssemblerX86Init3.C
  AssemblerX86Init4.C AssemblerX86Init5.C AssemblerX86Init6.C
  AssemblerX86Init7.C AssemblerX86Init8.C AssemblerX86Init9.C
  AssemblerX86Init.C x86InstructionProperties.C)


add_dependencies(roseDisassemblers rosetta_generated)

########### install files ###############

install(
  FILES
    Assembler.h BitPattern.h AssemblerX86.h AssemblerX86Init.h
    x86InstructionProperties.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
