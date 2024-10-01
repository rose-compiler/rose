set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ddisc_union=union")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ddisc_union=union")

########################################################################################################################
# Configuration header files
########################################################################################################################

# The rose_config.h file generated during the configuration process contains CPP symbols that indicate what features are
# available during compilation.  This file cannot be included into end-user source code because it pollutes the global
# namespace.  Therefore, we create a copy called rosePublicConfig.h containing only certain symbols and whose names are
# modified by prepending "ROSE_".

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/rosePublicConfig.h
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/publicConfiguration.pl
    <${CMAKE_BINARY_DIR}/rose_config.h >${CMAKE_BINARY_DIR}/rosePublicConfig.h
  DEPENDS ${CMAKE_BINARY_DIR}/rose_config.h)

add_custom_target(
  generate_rosePublicConfig
  DEPENDS ${CMAKE_BINARY_DIR}/rosePublicConfig.h)

add_executable(rose-feature-tests featureTests.C)
add_dependencies(rose-feature-tests generate_rosePublicConfig)


########################################################################################################################
# ROSETTA
########################################################################################################################

# Header files needed to build ROSETTA
include_directories(
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/src/roseSupport
  ${CMAKE_SOURCE_DIR}/src/roseExtensions/failSafe
  ${CMAKE_SOURCE_DIR}/src/midend/programTransformation/transformationTracking
  ${CMAKE_BINARY_DIR}/src/3rdPartyLibraries/fortran-parser
  ${JAVA_INCLUDE_PATH}
  ${JAVA_INCLUDE_PATH2}
  ${CMAKE_SOURCE_DIR}/src/frontend/ECJ_ROSE_Connection
  ${CMAKE_SOURCE_DIR}/src/midend/astMatching
  ${CMAKE_SOURCE_DIR}/src/util
  ${CMAKE_SOURCE_DIR}/src/frontend
  ${ROSE_INCLUDES})

# Output directory for ROSETTA-generated files
set(ROSETTA_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/frontend/SageIII)

# ROSETTA-generated class definition header files.
# Windows has problems executing commands with even relatively small command-lines, so we have to split this list of files
# into smaller lists. Windows command cannot be longer than 2k characters.
set(ROSETTA_GENERATED_DEFINITIONS_01
  ${ROSETTA_OUTPUT_DIRECTORY}/SgNode.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch32Coprocessor.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch32Instruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch64AtOperand.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch64BarrierOperand.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch64CImmediateOperand.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch64Instruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch64PrefetchOperand.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch64SysMoveOperand.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBasicString.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryAdd.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryAsr.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryConcat.h
)
set(ROSETTA_GENERATED_DEFINITIONS_02
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryDivide.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryLsl.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryLsr.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryMod.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryMsl.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryMultiply.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryPostupdate.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryPreupdate.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinaryRor.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBinarySubtract.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmBlock.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmByteOrder.h
)
set(ROSETTA_GENERATED_DEFINITIONS_03
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssembly.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyOS.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyOSTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyProcessor.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyProcessorTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyRef.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyRefOS.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyRefOSTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyRefProcessor.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyRefProcessorTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyRefTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilAssemblyTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilClassLayout.h
)
set(ROSETTA_GENERATED_DEFINITIONS_04
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilClassLayoutTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilConstant.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilConstantTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilCustomAttribute.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilCustomAttributeTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilDataStream.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilDeclSecurity.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilDeclSecurityTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilEvent.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilEventMap.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilEventMapTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilEventTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilExportedType.h
)
set(ROSETTA_GENERATED_DEFINITIONS_05
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilExportedTypeTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilField.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFieldLayout.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFieldLayoutTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFieldMarshal.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFieldMarshalTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFieldRVA.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFieldRVATable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFieldTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFile.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilFileTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilGenericParamConstraint.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilGenericParamConstraintTable.h
)
set(ROSETTA_GENERATED_DEFINITIONS_06
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilGenericParam.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilGenericParamTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilImplMap.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilImplMapTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilInterfaceImpl.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilInterfaceImplTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilManifestResource.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilManifestResourceTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMemberRef.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMemberRefTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMetadata.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMetadataHeap.h
)
set(ROSETTA_GENERATED_DEFINITIONS_07
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMetadataRoot.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodDef.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodDefTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodImpl.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodImplTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodSemantics.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodSemanticsTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodSpec.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodSpecTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilModule.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilModuleRef.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilModuleRefTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilModuleTable.h
)
set(ROSETTA_GENERATED_DEFINITIONS_08
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilNestedClass.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilNestedClassTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilNode.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilParam.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilParamTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilProperty.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilPropertyMap.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilPropertyMapTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilPropertyTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilStandAloneSig.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilStandAloneSigTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilTypeDef.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilTypeDefTable.h
)
set(ROSETTA_GENERATED_DEFINITIONS_09
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilTypeRef.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilTypeRefTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilTypeSpec.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilTypeSpecTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilUint32Heap.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilUint8Heap.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCliHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCoffStrtab.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCoffSymbol.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCoffSymbolList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCoffSymbolTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCommonSubExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmConstantExpression.h
)
set(ROSETTA_GENERATED_DEFINITIONS_10
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmControlFlagsExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDirectRegisterExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDOSExtendedHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDOSFileHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfAccessDeclaration.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfArrayType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfBaseType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfCatchBlock.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfClassTemplate.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfClassType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfCommonBlock.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfCommonInclusion.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfCompilationUnit.h
)
set(ROSETTA_GENERATED_DEFINITIONS_11
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfCompilationUnitList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfCondition.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfConstant.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfConstruct.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfConstructList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfConstType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfDwarfProcedure.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfEntryPoint.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfEnumerationType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfEnumerator.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfFileType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfFormalParameter.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfFormatLabel.h
)
set(ROSETTA_GENERATED_DEFINITIONS_12
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfFriend.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfFunctionTemplate.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfImportedDeclaration.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfImportedModule.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfImportedUnit.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfInformation.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfInheritance.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfInlinedSubroutine.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfInterfaceType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfLabel.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfLexicalBlock.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfLine.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfLineList.h
)
set(ROSETTA_GENERATED_DEFINITIONS_13
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfMacro.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfMacroList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfMember.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfModule.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfMutableType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfNamelist.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfNamelistItem.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfNamespace.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfPackedType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfPartialUnit.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfPointerType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfPtrToMemberType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfReferenceType.h
)
set(ROSETTA_GENERATED_DEFINITIONS_14
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfRestrictType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfSetType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfSharedType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfStringType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfStructureType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfSubprogram.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfSubrangeType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfSubroutineType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfTemplateTypeParameter.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfTemplateValueParameter.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfThrownType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfTryBlock.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfTypedef.h
)
set(ROSETTA_GENERATED_DEFINITIONS_15
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfUnionType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfUnknownConstruct.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfUnspecifiedParameters.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfUnspecifiedType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfUpcRelaxedType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfUpcSharedType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfUpcStrictType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfVariable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfVariant.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfVariantPart.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfVolatileType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmDwarfWithStmt.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfDynamicEntry.h
)
set(ROSETTA_GENERATED_DEFINITIONS_16
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfDynamicEntryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfDynamicSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfEHFrameEntryCI.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfEHFrameEntryCIList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfEHFrameEntryFD.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfEHFrameEntryFDList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfEHFrameSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfFileHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfNoteEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfNoteEntryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfNoteSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfRelocEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfRelocEntryList.h
)
set(ROSETTA_GENERATED_DEFINITIONS_17
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfRelocSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSectionTableEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSectionTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSegmentTableEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSegmentTableEntryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSegmentTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfStringSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfStrtab.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymbol.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymbolList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymbolSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverDefinedAux.h
)
set(ROSETTA_GENERATED_DEFINITIONS_18
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverDefinedAuxList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverDefinedEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverDefinedEntryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverDefinedSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverEntryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverNeededAux.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverNeededAuxList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverNeededEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverNeededEntryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverNeededSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmElfSymverSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmExecutableFileFormat.h
)
set(ROSETTA_GENERATED_DEFINITIONS_19
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmExprListExp.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmFloatType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmFloatValueExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmFunction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericDLL.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericDLLList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericFile.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericFileList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericFormat.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericHeaderList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericSection.h
)
set(ROSETTA_GENERATED_DEFINITIONS_20
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericSectionList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericString.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericStrtab.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericSymbol.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmGenericSymbolList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmIndirectRegisterExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmInstructionList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmIntegerType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmIntegerValueExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmInterpretation.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmInterpretationList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmAttribute.h
)
set(ROSETTA_GENERATED_DEFINITIONS_21
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmAttributeTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmBootstrapMethod.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmBootstrapMethods.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmClass.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmCodeAttribute.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmConstantPoolEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmConstantPool.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmConstantValue.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmEnclosingMethod.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmExceptionHandler.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmExceptions.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmExceptionTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmField.h
)
set(ROSETTA_GENERATED_DEFINITIONS_22
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmFieldTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmFileHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmInnerClassesEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmInnerClasses.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmLineNumberEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmLineNumberTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmLocalVariableEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmLocalVariableTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmLocalVariableTypeEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmLocalVariableTypeTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmMethod.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmMethodParametersEntry.h
)
set(ROSETTA_GENERATED_DEFINITIONS_23
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmMethodParameters.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmMethodTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmModuleMainClass.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmNestHost.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmNestMembers.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmNode.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmSignature.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmSourceFile.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmStackMapFrame.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmStackMapTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmJvmStackMapVerificationType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLEEntryPoint.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLEEntryTable.h
)
set(ROSETTA_GENERATED_DEFINITIONS_24
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLEFileHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLENameTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLEPageTableEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLEPageTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLERelocTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLESection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLESectionTableEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmLESectionTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmM68kInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmMemoryReferenceExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmMipsInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNEEntryPoint.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNEEntryTable.h
)
set(ROSETTA_GENERATED_DEFINITIONS_25
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNEFileHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNEModuleTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNENameTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNERelocEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNERelocTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNESection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNESectionTableEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNESectionTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNEStringTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNode.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmNullInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmOperandList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEExportDirectory.h
)
set(ROSETTA_GENERATED_DEFINITIONS_26
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEExportEntry.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEExportEntryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEExportSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEFileHeader.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEImportDirectory.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEImportDirectoryList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEImportItem.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEImportItemList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEImportSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPERVASizePair.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPERVASizePairList.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPESection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPESectionTableEntry.h
)
set(ROSETTA_GENERATED_DEFINITIONS_27
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPESectionTable.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPEStringSection.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPowerpcInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmRegisterNames.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmRegisterReferenceExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmRiscOperation.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmScalarType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmStackExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmStatement.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmStaticData.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmStoredString.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmStringStorage.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmSynthesizedDataStructureDeclaration.h
)
set(ROSETTA_GENERATED_DEFINITIONS_28
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmSynthesizedDeclaration.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmSynthesizedFieldDeclaration.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUnaryExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUnaryMinus.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUnaryPlus.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUnaryRrx.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUnarySignedExtend.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUnaryTruncate.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUnaryUnsignedExtend.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmUserInstruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmValueExpression.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmVectorType.h
)
set(ROSETTA_GENERATED_DEFINITIONS_29
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmX86Instruction.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmVoidType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmPointerType.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmAarch64PState.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilExceptionData.h
  ${ROSETTA_OUTPUT_DIRECTORY}/SgAsmCilMethodData.h
)
set(ROSETTA_GENERATED_DEFINITIONS
  ${ROSETTA_GENERATED_DEFINITIONS_01}
  ${ROSETTA_GENERATED_DEFINITIONS_02}
  ${ROSETTA_GENERATED_DEFINITIONS_03}
  ${ROSETTA_GENERATED_DEFINITIONS_04}
  ${ROSETTA_GENERATED_DEFINITIONS_05}
  ${ROSETTA_GENERATED_DEFINITIONS_06}
  ${ROSETTA_GENERATED_DEFINITIONS_07}
  ${ROSETTA_GENERATED_DEFINITIONS_08}
  ${ROSETTA_GENERATED_DEFINITIONS_09}
  ${ROSETTA_GENERATED_DEFINITIONS_10}
  ${ROSETTA_GENERATED_DEFINITIONS_11}
  ${ROSETTA_GENERATED_DEFINITIONS_12}
  ${ROSETTA_GENERATED_DEFINITIONS_13}
  ${ROSETTA_GENERATED_DEFINITIONS_14}
  ${ROSETTA_GENERATED_DEFINITIONS_15}
  ${ROSETTA_GENERATED_DEFINITIONS_16}
  ${ROSETTA_GENERATED_DEFINITIONS_17}
  ${ROSETTA_GENERATED_DEFINITIONS_18}
  ${ROSETTA_GENERATED_DEFINITIONS_19}
  ${ROSETTA_GENERATED_DEFINITIONS_20}
  ${ROSETTA_GENERATED_DEFINITIONS_21}
  ${ROSETTA_GENERATED_DEFINITIONS_22}
  ${ROSETTA_GENERATED_DEFINITIONS_23}
  ${ROSETTA_GENERATED_DEFINITIONS_24}
  ${ROSETTA_GENERATED_DEFINITIONS_25}
  ${ROSETTA_GENERATED_DEFINITIONS_26}
  ${ROSETTA_GENERATED_DEFINITIONS_27}
  ${ROSETTA_GENERATED_DEFINITIONS_28}
  ${ROSETTA_GENERATED_DEFINITIONS_29}
)

# ROSETTA-generated headers files. If one of these headers is no longer generated, "make" will repeatedly run the rule
# to try to generate it.
set(ROSETTA_HEADERS
  ${ROSETTA_OUTPUT_DIRECTORY}/AST_FILE_IO.h
  ${ROSETTA_OUTPUT_DIRECTORY}/AstQueryMemoryPool.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_Grammar.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarDeclarations.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarDowncast.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarStorageClasses.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarSgNodeDefinitions.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarSgAsmNodeDefinitions.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarSgAsmCilNodeDefinitions.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarSgAsmDwarfNodeDefinitions.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarSgAsmJvmNodeDefinitions.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarVariants.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarVisitorSupport.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarMemoryPoolSupport.h
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarTreeTraversalAccessEnums.h
  ${ROSETTA_OUTPUT_DIRECTORY}/StorageClasses.h
  ${ROSETTA_GENERATED_DEFINITIONS}
)

# ROSETTA-generated source files.
set(ROSETTA_SRC
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_Grammar.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarCopyMemberFunctions.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarGetChildIndex.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarMemoryPoolSupport.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarNewAndDeleteOperators.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarNewConstructors.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarProcessDataMemberReferenceToPointers.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarReturnClassHierarchySubTree.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarReturnDataMemberPointers.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarRTI.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarNodeIdSupport.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarTraverseMemoryPool.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarTreeTraversalSuccessorContainer.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarVariantEnumNames.C
  ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_GrammarVisitorSupport.C
  ${ROSETTA_OUTPUT_DIRECTORY}/StorageClasses.C
  ${ROSETTA_OUTPUT_DIRECTORY}/AST_FILE_IO.C)

# Tell cmake that these files are generated by ROSETTA and will only be available at compile time
set_source_files_properties(${ROSETTA_SRC} ${ROSETTA_HEADERS} PROPERTIES GENERATED 1)

if(MSVC)
  set_source_files_properties(
    ${ROSETTA_OUTPUT_DIRECTORY}/StorageClasses.C
    ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_Grammar.C
    ${ROSETTA_OUTPUT_DIRECTORY}/Cxx_VisitorSupport.C
    PROPERTIES COMPILE_FLAGS /bigobj)
endif()

########################################################################################################################
# Subdirs for the ROSE library
########################################################################################################################

# Build in the subdirectories that make up the ROSE source code
add_subdirectory(util)
add_subdirectory(Sawyer)
add_subdirectory(Rosebud)
add_subdirectory(ROSETTA)
add_subdirectory(roseSupport)
add_subdirectory(3rdPartyLibraries)
add_subdirectory(roseIndependentSupport)

add_definitions(-DROSE_DLL_EXPORTS)

add_subdirectory(roseExtensions)
add_subdirectory(generated)
add_subdirectory(frontend)
add_subdirectory(midend)
add_subdirectory(backend)
add_subdirectory(Rose)
add_subdirectory(AstNodes)

########################################################################################################################
# The ROSE library
########################################################################################################################

set(rose_LIB_OBJS   
  util_main
  util_commandlineProcessing
  util_sawyer
  util_stringSupport
  util_support
  util_graphs
  unparser
  sage3
  virtualCFG
  astFromString
  roseSupport
  roseDisassemblers
  roseBinaryFormats
  roseAsmUnparser
  astFixup
  midend
  abstractHandle
  abstractLayer
  astDiagnostics
  astProcessing
  astMatching
  midend_util
  midend_binary
  midend_pt
  ExtractFunctionArgumentsNormalization
  SingleStatementToBlockNormalization
  sageInterface
  astTokenStream
  astHiddenTypeAndDeclarationLists
  astVisualization
  includeDirectivesProcessing
  astPostProcessing
  failsafe
  trans_tracking
  astRewriteMechanism
  roseAstNodesExpression
  roseNamespace
  roseBinaryAnalysis
  roseBinaryAnalysisArchitecture
  roseBinaryAnalysisByteCode
  roseBinaryAnalysisCallingConvention
  roseBinaryAnalysisConcolic
  roseBinaryAnalysisConcolicCallback
  roseBinaryAnalysisConcolicI386Linux
  roseBinaryAnalysisConcolicM68kSystem
  roseBinaryAnalysisDebugger
  roseBinaryAnalysisDisassembler
  roseBinaryAnalysisDwarf
  roseBinaryAnalysisInstructionSemantics
  roseBinaryAnalysisInstructionSemanticsBaseSemantics
  roseBinaryAnalysisModelChecker
  roseBinaryAnalysisPartitioner2
  roseBinaryAnalysisUnparser
  roseColor
  roseCommandLine
  roseDiagnostics
  roseFileSystem
  roseSarif
  roseStringUtility
  RoseAST
  roseCodegen
  roseGeneratedBinaryAnalysis
  roseGeneratedJovial
  mstl
  libSrcGeneratedRoseSarif)

set(rose_LIB_SRCS
  dummyCppFileForLibrose.C
  ${ROSETTA_SRC})

if(ENABLE-JAVA)
  list(APPEND rose_LIB_OBJS roseJava)
endif()

if(ENABLE-JOVIAL)
  list(APPEND rose_LIB_OBJS ${ATERM_LIBRARY})
  list(APPEND rose_LIB_OBJS roseATerm)
  list(APPEND rose_LIB_OBJS roseTreeBuilder)
  list(APPEND rose_LIB_OBJS roseJovial)
endif()

if (ENABLE-PHP)
  list(APPEND rose_LIB_OBJS phpFrontend)
endif()

if(NOT ENABLE-INTERNALFRONTENDDEVELOPMENT)
  list(APPEND rose_LIB_OBJS
    midend_pa
    midend_loopproc)
endif()

if(ENABLE-ROSEHPCT)
  list(APPEND rose_LIB_OBJS roseExtensions)
endif()

# TODO Conditional either EDG or CLang or nothing
if(ENABLE-CPP)
 list(APPEND rose_LIB_OBJS RoseCxxFrontendEDG)
endif()

set(rose_LIB_SRC_FILES dummyCppFileForLibrose.C)

#foreach(_object IN LISTS rose_LIB_OBJS)
#  list(APPEND rose_LIB_SRCS $<TARGET_OBJECTS:${_object}>)
#  get_target_property(_sources ${_object} SOURCES)
#  get_target_property(_sources_dir ${_object} SOURCE_DIR)
#  foreach(_file IN LISTS _sources)
#    list(APPEND rose_LIB_SRC_FILES ${_sources_dir}/${_file})
#  endforeach()
#endforeach()

#list(LENGTH rose_LIB_SRC_FILES list_length)

########################################################################################################################
# The ROSE library itself (librose.so, librose.a, librose.dll)
########################################################################################################################
add_library(ROSE_DLL SHARED ${rose_LIB_SRCS})
add_dependencies(ROSE_DLL roseUtil)

set_target_properties(ROSE_DLL PROPERTIES OUTPUT_NAME "rose")

target_link_libraries(ROSE_DLL ${link_with_libraries} ${rose_LIB_OBJS})

set_target_properties(ROSE_DLL PROPERTIES
  VERSION 1.0.0
  SOVERSION 1
  DEFINE_SYMBOL ROSE_DLL_EXPORTS)

install(TARGETS ROSE_DLL DESTINATION ${ROSE_LIB_DIR_NAME})
install(
  FILES
    featureTests.h RoseFirst.h rose.h roseInternal.h rose_msvc.h msvc_stdint.h
    ${CMAKE_BINARY_DIR}/rosePublicConfig.h
  DESTINATION ${INCLUDE_INSTALL_DIR})

# Install the parse table for the Jovial front-end
if(ENABLE-JOVIAL)
  install(FILES ${CMAKE_SOURCE_DIR}/src/3rdPartyLibraries/jovial-parser/share/rose/Jovial.tbl DESTINATION share/rose)
endif()

#cmake_host_system_information(RESULT Ncpu QUERY NUMBER_OF_PHYSICAL_CORES)
#blt_add_code_checks(PREFIX "rosecheck"
#                    SOURCES ${rose_LIB_SRC_FILES}
#                    CPPCHECK_FLAGS "-j${Ncpu}")

########################################################################################################################
# Dependencies of the ROSE library. librose depends on additional libraries.
########################################################################################################################

set(rose_library_dir_list "")
set(lib_dir "")
macro(add_library_dependency lib)
  get_filename_component(lib_dir ${lib} DIRECTORY)
  list(APPEND rose_library_dir_list ${lib_dir})
endmacro()

# Z3 has an executable component and a development (library and headers) component that can be installed independently
# of each other. Therfore Z3_FOUND_EXE says whether the executable component was found, and Z3_FOUND_LIB says whether
# the development component was found; Z3_FOUND is set if either component is found.
if(Z3_FOUND_LIB)
  target_link_libraries(ROSE_DLL ${Z3_LIBRARY})
  add_library_dependency(${Z3_LIBRARY})
endif()

# YAML-cpp is for parsing YAML and JSON files.
if(ENABLE-YAML AND YAMLCPP_FOUND)
  target_link_libraries(ROSE_DLL ${YAMLCPP_LIBRARY})
  add_library_dependency(${Z3_LIBRARY})
endif()

# Dlib is a C++ support library. ROSE uses matrices and some graph algorithms currently.
if(DLIB_FOUND)
  target_link_libraries(ROSE_DLL ${DLIB_LIBRARY})
  add_library_dependency(${DLIB_LIBRARY})
endif()

# Capstone is used to disassemble ARM.
if(CAPSTONE_FOUND)
  target_link_libraries(ROSE_DLL ${CAPSTONE_LIBRARY})
  add_library_dependency(${CAPSTONE_LIBRARY})
endif()

# If readline was found.
if(ENABLE-READLINE AND READLINE_FOUND)
  target_link_libraries(ROSE_DLL ${READLINE_LIBRARY})
  add_library_dependency(${READLINE_LIBRARY})
endif()

# The GNU GCrypt library is used for computing a wide variety of hashes. It needs the GPG-Error library.
if(GCRYPT_FOUND AND GPGERROR_FOUND)
  target_link_libraries(ROSE_DLL ${GCRYPT_LIBRARY})
  add_library_dependency(${GCRYPT_LIBRARY})
  target_link_libraries(ROSE_DLL ${GPGERROR_LIBRARY})
  add_library_dependency(${GPGERROR_LIBRARY})
endif()

# Zlib is a compression library and is needed by Boost. Make sure you compiled Boost with this same zlib.
if(ZLIB_FOUND)
  target_link_libraries(ROSE_DLL ${ZLIB_LIBRARY})
  add_library_dependency(${ZLIB_LIBRARY})
endif()

# If POET is enabled in ROSE, we need to link in some additional dependencies.
if(ENABLE-POET)
  set(LINK_FILES poet ${CMAKE_DL_LIBS})
  add_dependencies(ROSE_DLL poet)
else()
  set(LINK_FILES ${CMAKE_DL_LIBS})
endif()

# ???
if(NOT BINARY_EDG)
  add_definitions(-DUSE_FAKE_EDG)
endif()


# Depending on which language analysis (frontend) features are enabled in ROSE, we may need some additional libraries.
if(ENABLE-FORTRAN)
  list(APPEND LINK_FILES roseFortran)
endif()

if (ENABLE-JAVA)
  list(APPEND LINK_FILES ${JAVA_JVM_LIBRARY})
endif()

if(ENABLE-PYTHON)
  list(APPEND LINK_FILES unparsePython rosePythonFrontend)
endif()

if(ENABLE-ROSEHPCT)
  list(APPEND LINK_FILES ${LIBXML2_LIBRARIES})
endif()

if(ENABLE-ADA)
  list(APPEND LINK_FILES experimentalRoseAda)
endif()

set(ROSE_TARGET_LINK ${LINK_FILES})
if (WIN32)
  target_link_libraries(ROSE_DLL ${ROSE_TARGET_LINK} shlwapi.lib Ws2_32.lib)
else ()
  target_link_libraries(ROSE_DLL ${ROSE_TARGET_LINK} )
endif ()

# If the C preprocessor is enabled, ROSE has some additional depenendencies
if(ENABLE-CPP)
  add_dependencies(ROSE_DLL OMPPARSER ROSE_PREPROCESSOR generate_rosePublicConfig)
endif()

# SQLite3 has an executable component and a development (library and headers) component that can be installed
# independently of each other. Therfore SQLITE3_FOUND_EXE says whether the executable component was found, and
# SQLITE3_FOUND_LIB says whether the development component was found; SQLITE3_FOUND is set if either component is found.
if(SQLITE3_FOUND_LIB)
  # CORY: Maybe you can help. When sqlite is enabled, I get errors from cmake (when run in a clean build directory) that
  #       say "Cannot find source file /home/matzke/junk/_build-cmake/src/util/CMakeFiles/util_main.dir/rose_paths.C.o;
  #       Tried extensions .c .C .c++ .cc .cpp .cxx .m .M .mm .h .hh .h++ .hm .hpp .hxx .in .txx". I'm not sure why
  #       cmake things "rose_paths.C.o" should be a source file. The $ROSE/src/util/CMakeLists.txt has the CMake
  #       commands for creating rose_paths.C and rose_paths.h, and is the only CMakeLists.txt file that mentions
  #       "rose_paths". The error goes away if I comment out the following line (but then of course there's linking
  #       problems later).
  target_link_libraries(ROSE_DLL RoseSQLite3xDatabase)
endif()

########################################################################################################################
# testSharedRoseLib -- a basic test to make sure the ROSE library works
########################################################################################################################

remove_definitions(-DROSE_DLL_EXPORTS)
add_executable(testSharedRoseLib testRoseLib.C)
target_link_libraries(testSharedRoseLib ROSE_DLL ${link_with_libraries})

########################################################################################################################
# The rose-config tool (Deprecated; use the rose-config.cfg file directly instead)
########################################################################################################################

add_definitions(-DLIBDIR="${CMAKE_INSTALL_PREFIX}/${ROSE_LIB_DIR_NAME}") #LIB_INSTALL_DIR ?
add_executable(rose-config rose-config.C)
target_link_libraries(rose-config ROSE_DLL ${link_with_libraries})
install(TARGETS rose-config DESTINATION bin)

########################################################################################################################
# The rose-compiler tool
########################################################################################################################

add_executable(rose-compiler rose-compiler.C)
target_link_libraries(rose-compiler ROSE_DLL ${link_with_libraries})
install(TARGETS rose-compiler DESTINATION bin)


########################################################################################################################
# rose-config.cfg -- text file that contains various settings for user-level makefiles.
########################################################################################################################

#Build LDFLAGS for ROSE tools
set(rose_config_installed_cppflags "-I${CMAKE_INSTALL_PREFIX}/include/rose -std=c++14")

list(APPEND rose_library_dir_list "${CMAKE_INSTALL_PREFIX}/${ROSE_LIB_DIR_NAME}")
list(APPEND rose_library_dir_list ${Boost_LIBRARY_DIRS})
if(NOT ${ATERM_LIBRARY} STREQUAL "ATERM_LIBRARY-NOTFOUND")
  if(NOT ${ATERM_LIBRARY} STREQUAL "")
    add_library_dependency(${ATERM_LIBRARY})
    set(rose_config_installed_cppflags "${rose_config_installed_cppflags} -I${ATERM_LIBRARY_DIR}/../include")
  endif()
endif()

# Add quadmath to the libraries reported by rose-config if we did in fact link librose to it
# see ${link_with_libraries} in the parent CMakeLists.txt file for what librose is being 
# linked to 
if(NOT ${QUADMATH_LIBRARY} STREQUAL "QUADMATH_LIBRARY-NOTFOUND")
  if(NOT ${QUADMATH_LIBRARY} STREQUAL "")
    add_library_dependency(${QUADMATH_LIBRARY})
  endif()
endif()

foreach(X IN LISTS rose_library_dir_list)
  set(rose_config_installed_ldflags "${rose_config_installed_ldflags} -L${X}")
endforeach()

# Add -pthread if Threads_FOUND or Boost::thread is found to avoid rose-tool segfaults 
if(Threads_FOUND OR "${Boost_LIBRARIES}" MATCHES "Boost::thread")
  set(rose_config_installed_cppflags "${rose_config_installed_cppflags} -pthread")
endif()

# Fix double slashes in rose-config
if("${Boost_INCLUDE_DIRS}" MATCHES "^/")
  # this var starts with a slash so we dont append another one after the -I 
  set(rose_config_installed_cppflags "${rose_config_installed_cppflags} -I${Boost_INCLUDE_DIRS}")
else()
  set(rose_config_installed_cppflags "${rose_config_installed_cppflags} -I/${Boost_INCLUDE_DIRS}")
endif() 

# There must be a cleaner approach to report this to rose-config, but for now  
set(rose_library_list "rose")
set(rose_library_list "${rose_library_list};${Boost_LIBRARIES}")
set(rose_library_list "${rose_library_list};m")
if(NOT ${QUADMATH_LIBRARY} STREQUAL "QUADMATH_LIBRARY-NOTFOUND")
  if(NOT ${QUADMATH_LIBRARY} STREQUAL "")
    set(rose_library_list "${rose_library_list};${QUADMATH_LIBRARY}")
  endif()
endif() 

# Set macro for EDG 6 and 6.5
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Move this to src/frontend/CxxFrontend/EDG/EDG_6.5/src/CMakeLists.txt and edg_6.0
  set(ROSE_USE_EDG_QUAD_FLOAT 1)
endif() 

# Add -pthread if Threads_FOUND or Boost::thread found to avoid rose-tool segfaults 
if(Threads_FOUND OR "${Boost_LIBRARIES}" MATCHES "Boost::thread")
  set(rose_config_installed_ldflags "${rose_config_installed_ldflags} -pthread")
endif()

foreach(X IN LISTS rose_library_list)
  # Dereference ${X} with "" to fix Windows CI/CD error 
  get_filename_component(Y "${X}" NAME_WE)
  
  # If Y is empty, use X directly
  if(Y STREQUAL "") 
    set(Y "${X}")
  endif() 

  # Handle non-path library names appropriately
  string(REGEX REPLACE "^-l" "" Y "${Y}")  # Remove '-l' prefix if present
  string(REGEX REPLACE "^lib" "" Y "${Y}") # Remove 'lib' prefix if present
  string(REGEX REPLACE "^Boost::" "boost_" Y "${Y}") # Convert boost modules into ld-recognizeable libraries
  
  # Avoid treating empty strings as library names
  if(NOT Y STREQUAL "")
    set(rose_config_installed_ldflags "${rose_config_installed_ldflags} -l${Y}")
  endif()
endforeach()

 # Add -ldl before -lm if HAVE_LIBDL
 if(HAVE_LIBDL)
   string(REPLACE " -lm " " -ldl -lm " rose_config_installed_ldflags "${rose_config_installed_ldflags}")
 endif()

#Need to update config ldflags with the following potential values
   #     $(JAVA_JVM_LINK) \
   #     $(SQLITE_DATABASE_LIBS) $(RT_LIBS) \
   #     $(ROSE_YICES_LIBS_WITH_PATH) $(ROSE_Z3_LIBS_WITH_PATH) \
   #     $(ROSE_PHP_LIBS_WITH_PATH) $(ROSE_ELF_LIBS_WITH_PATH) \
   #     $(ROSE_DWARF_LIBS_WITH_PATH) $(ROSE_QT_LIBS_WITH_PATH) \
   #     $(ROSE_SSL_LIBS) $(ROSE_ETHER_LIBS) \
   #     $(ROSE_INTEL_COMPILER_MATH_LIBS) $(ROSE_ATERM_LIBS) \
   #     $(ROSE_FLANG_LIBS) $(ROSE_COBOL_PT_LIBS_WITH_PATH) \
   #     $(ROSE_YAMLCPP_LIBS_WITH_PATH) $(ROSE_LIBMAGIC_LIBS_WITH_PATH) \
   #     $(ROSE_DLIB_LIBS_WITH_PATH) $(ROSE_GCRYPT_LIBS_WITH_PATH) \
   #     $(ROSE_CAPSTONE_LIBS_WITH_PATH) -lm $(am__append_18) \
   #     $(am__append_19)

# Function for rose-config    
function(resolve_compiler_names)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(cc_name "gcc" PARENT_SCOPE)
        set(cxx_name "g++" PARENT_SCOPE)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        set(cc_name "clang" PARENT_SCOPE)
        set(cxx_name "clang++" PARENT_SCOPE)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        set(cc_name "icc" PARENT_SCOPE)
        set(cxx_name "icpc" PARENT_SCOPE)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "XL")
        set(cc_name "xlc" PARENT_SCOPE)
        set(cxx_name "xlC" PARENT_SCOPE)
    endif()
endfunction()

# Usage example
resolve_compiler_names()

# Use cc_name if set, otherwise default to BACKEND_C_COMPILER_NAME_WITHOUT_PATH
if(cc_name)
    set(ACTUAL_CC ${cc_name})
else()
    set(ACTUAL_CC ${BACKEND_C_COMPILER_NAME_WITHOUT_PATH})
endif()

# Use cxx_name if set, otherwise default to BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH
if(cxx_name)
    set(ACTUAL_CXX ${cxx_name})
else()
    set(ACTUAL_CXX ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH})
endif()

set(ROSE_LIBDIRS "")
set(ROSE_LINK_RPATHS "")

list(REMOVE_DUPLICATES rose_library_dir_list)
foreach(X IN LISTS rose_library_dir_list)
  set(ROSE_LIBDIRS "${ROSE_LIBDIRS} ${X}")
  set(ROSE_LINK_RPATHS "${ROSE_LINK_RPATHS} -Wl,-rpath ${X}")
endforeach()

if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
  message(STATUS "rose_library_dir_list: ${rose_library_dir_list}")
  message(STATUS "ROSE_LIBDIRS: ${ROSE_LIBDIRS}")
  message(STATUS "ROSE_LINK_RPATHS: ${ROSE_LINK_RPATHS}")
endif() 

file(REMOVE ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg)
file(WRITE  ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "# Things that the 'rose-config' tool can report\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_CC           = ${ACTUAL_CC}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_CCPATH       = ${CMAKE_C_COMPILER}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_CXX          = ${ACTUAL_CXX}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_CXX_PATH     = ${CMAKE_CXX_COMPILER}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_CPPFLAGS     = ${rose_config_installed_cppflags}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_CFLAGS       = ${CMAKE_C_FLAGS}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_CXXFLAGS     = ${CMAKE_CXX_FLAGS}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_LDFLAGS      = ${rose_config_installed_ldflags}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_LIBDIRS      = ${ROSE_LIBDIRS}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_PREFIX       = ${CMAKE_INSTALL_PREFIX}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "# Additional items useful to include in Makefiles\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_RPATHS       = ${ROSE_LIBDIRS}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_LINK_RPATHS  = ${ROSE_LINK_RPATHS}\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/rose-config.cfg "ROSE_VERSION      = ${ROSE_PACKAGE_VERSION}\n")
install(FILES ${CMAKE_BINARY_DIR}/src/rose-config.cfg DESTINATION ${ROSE_LIB_DIR_NAME})
