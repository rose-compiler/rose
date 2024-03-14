add_library(roseDisassemblers OBJECT Assembler.C AssemblerX86.C
  AssemblerX86Init1.C AssemblerX86Init2.C AssemblerX86Init3.C
  AssemblerX86Init4.C AssemblerX86Init5.C AssemblerX86Init6.C
  AssemblerX86Init7.C AssemblerX86Init8.C AssemblerX86Init9.C
  AssemblerX86Init.C
  SgAsmAarch32Instruction.C SgAsmStackExpression.C SgAsmStaticData.C SgAsmValueExpression.C
  SgAsmAarch64Instruction.C SgAsmBlock.C SgAsmExecutableFileFormat.C
  SgAsmExpression.C SgAsmFloatValueExpression.C SgAsmFunction.C
  SgAsmInstruction.C SgAsmAarch32Instruction.C SgAsmControlFlagsExpression.C
  SgAsmIntegerValueExpression.C SgAsmInterpretation.C SgAsmType.C
  SgAsmM68kInstruction.C SgAsmCilInstruction.C SgAsmJvmInstruction.C
  SgAsmMipsInstruction.C SgAsmNullInstruction.C SgAsmPEExportEntry.C
  SgAsmPowerpcInstruction.C SgAsmUserInstruction.C SgAsmX86Instruction.C
  x86InstructionProperties.C SgAsmOperandList.C SgAsmGenericHeader.C)

add_dependencies(roseDisassemblers rosetta_generated)

########### install files ###############

install(
  FILES
    Assembler.h AssemblerX86.h AssemblerX86Init.h
    x86InstructionProperties.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
