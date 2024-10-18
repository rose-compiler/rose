add_library(roseAstNodesBinaryAnalysis OBJECT
  SgAsmAarch32Instruction.C
  SgAsmAarch32Instruction.C
  SgAsmAarch64Instruction.C
  SgAsmBlock.C
  SgAsmCilInstruction.C
  SgAsmControlFlagsExpression.C
  SgAsmExecutableFileFormat.C
  SgAsmExpression.C
  SgAsmFloatType.C
  SgAsmFloatValueExpression.C
  SgAsmFunction.C
  SgAsmGenericHeader.C
  SgAsmInstruction.C
  SgAsmIntegerType.C
  SgAsmIntegerValueExpression.C
  SgAsmInterpretation.C
  SgAsmJvmInstruction.C
  SgAsmM68kInstruction.C
  SgAsmMipsInstruction.C
  SgAsmNullInstruction.C
  SgAsmOperandList.C
  SgAsmPEExportEntry.C
  SgAsmPointerType.C
  SgAsmPowerpcInstruction.C
  SgAsmScalarType.C
  SgAsmStackExpression.C
  SgAsmStaticData.C
  SgAsmType.C
  SgAsmUserInstruction.C
  SgAsmValueExpression.C
  SgAsmVectorType.C
  SgAsmVoidType.C
  SgAsmX86Instruction.C)

add_dependencies(roseAstNodesBinaryAnalysis rosetta_generated)
