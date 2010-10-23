
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

# Use this as a way to turn off this optional use to build smaller files
# so that we test this unter Linux before worring about use within Windows.
# tps (01/04/10) : Added name for if condition
message ( STATUS ">>>>>>>>>>>>>>>>>>>>>> smallerGeneratedFiles : " ${enable-smaller-generated-files} )
if ( enable-smaller-generated-files  )

########### next target ###############
set( libgeneratedIR_SRC
SgAccessModifier.C                     SgAsmDwarfRestrictType.C            SgAsmLESection.C                           SgCatchOptionStmt.C                   SgFunctionParameterList.C               SgOmpClause.C                                SgTemplateInstantiationMemberFunctionDecl.C 
SgActualArgumentExpression.C           SgAsmDwarfSetType.C                 SgAsmLESectionTable.C                      SgCatchStatementSeq.C                 SgFunctionParameterTypeList.C           SgOmpCollapseClause.C                        SgTemplateParameter.C 
SgAddOp.C                              SgAsmDwarfSharedType.C              SgAsmLESectionTableEntry.C                 SgCharVal.C                           SgFunctionRefExp.C                      SgOmpCopyinClause.C                          SgTemplateParameterList.C 
SgAddressOfOp.C                        SgAsmDwarfStringType.C              SgAsmMemoryReferenceExpression.C           SgClassDeclaration.C                  SgFunctionSymbol.C                      SgOmpCopyprivateClause.C                     SgTemplateSymbol.C 
SgAggregateInitializer.C               SgAsmDwarfStructureType.C           SgAsmNEEntryPoint.C                        SgClassDecl_attr.C                    SgFunctionType.C                        SgOmpCriticalStatement.C                     SgTemplateType.C 
SgAliasSymbol.C                        SgAsmDwarfSubprogram.C              SgAsmNEEntryTable.C                        SgClassDefinition.C                   SgFunctionTypeSymbol.C                  SgOmpDefaultClause.C                         SgThisExp.C 
SgAllocateStatement.C                  SgAsmDwarfSubrangeType.C            SgAsmNEFileHeader.C                        SgClassNameRefExp.C                   SgFunctionTypeTable.C                   SgOmpDoStatement.C                           SgThrowOp.C 
SgAndAssignOp.C                        SgAsmDwarfSubroutineType.C          SgAsmNEModuleTable.C                       SgClassSymbol.C                       SgGlobal.C                              SgOmpExpressionClause.C                      SgToken.C 
SgAndOp.C                              SgAsmDwarfTemplateTypeParameter.C   SgAsmNENameTable.C                         SgClassType.C                         SgGotoStatement.C                       SgOmpFirstprivateClause.C                    SgTryStmt.C 
SgArithmeticIfStatement.C              SgAsmDwarfTemplateValueParameter.C  SgAsmNERelocEntry.C                        SgClinkageDeclarationStatement.C      SgGraph.C                               SgOmpFlushStatement.C                        SgTypeBool.C 
SgArrayType.C                          SgAsmDwarfThrownType.C              SgAsmNERelocTable.C                        SgClinkageEndStatement.C              SgGraphEdge.C                           SgOmpForStatement.C                          SgType.C 
SgArrowExp.C                           SgAsmDwarfTryBlock.C                SgAsmNESection.C                           SgClinkageStartStatement.C            SgGraphEdgeList.C                       SgOmpIfClause.C                              SgTypeChar.C 
SgArrowStarOp.C                        SgAsmDwarfTypedef.C                 SgAsmNESectionTable.C                      SgCloseStatement.C                    SgGraphNode.C                           SgOmpLastprivateClause.C                     SgTypeComplex.C 
SgAsmArmInstruction.C                  SgAsmDwarfUnionType.C               SgAsmNESectionTableEntry.C                 SgColonShapeExp.C                     SgGraphNodeList.C                       SgOmpMasterStatement.C                       SgTypeDefault.C 
SgAsmArmRegisterReferenceExpression.C  SgAsmDwarfUnknownConstruct.C        SgAsmNEStringTable.C                       SgCommaOpExp.C                        SgGreaterOrEqualOp.C                    SgOmpNowaitClause.C                          SgTypedefDeclaration.C 
SgAsmBasicString.C                     SgAsmDwarfUnspecifiedParameters.C   SgAsmNode.C                                SgCommonBlock.C                       SgGreaterThanOp.C                       SgOmpNumThreadsClause.C                      SgTypedefSeq.C 
SgAsmBinaryAdd.C                       SgAsmDwarfUnspecifiedType.C         SgAsmOp.C                                  SgCommonBlockObject.C                 SgIdentDirectiveStatement.C             SgOmpOrderedClause.C                         SgTypedefSymbol.C 
SgAsmBinaryAddPostupdate.C             SgAsmDwarfUpcRelaxedType.C          SgAsmOperandList.C                         SgCommonSymbol.C                      SgIfdefDirectiveStatement.C             SgOmpOrderedStatement.C                      SgTypedefType.C 
SgAsmBinaryAddPreupdate.C              SgAsmDwarfUpcSharedType.C           SgAsmPEExportDirectory.C                   SgComplexVal.C                        SgIfDirectiveStatement.C                SgOmpParallelStatement.C                     SgTypeDouble.C 
SgAsmBinaryAsr.C                       SgAsmDwarfUpcStrictType.C           SgAsmPEExportEntry.C                       SgComputedGotoStatement.C             SgIfndefDirectiveStatement.C            SgOmpPrivateClause.C                         SgTypeEllipse.C 
SgAsmBinaryDivide.C                    SgAsmDwarfVariable.C                SgAsmPEExportEntryList.C                   SgConcatenationOp.C                   SgIfStmt.C                              SgOmpReductionClause.C                       SgTypeFloat.C 
SgAsmBinaryExpression.C                SgAsmDwarfVariant.C                 SgAsmPEExportSection.C                     SgConditionalExp.C                    SgImagPartOp.C                          SgOmpScheduleClause.C                        SgTypeGlobalVoid.C 
SgAsmBinaryLsl.C                       SgAsmDwarfVariantPart.C             SgAsmPEFileHeader.C                        SgConjugateOp.C                       SgImplicitStatement.C                   SgOmpSectionsStatement.C                     SgTypeIdOp.C 
SgAsmBinaryLsr.C                       SgAsmDwarfVolatileType.C            SgAsmPEImportDirectory.C                   SgConstructorInitializer.C            SgImpliedDo.C                           SgOmpSectionStatement.C                      SgTypeImaginary.C 
SgAsmBinaryMod.C                       SgAsmDwarfWithStmt.C                SgAsmPEImportDirectoryList.C               SgConstVolatileModifier.C             SgImportStatement.C                     SgOmpSharedClause.C                          SgTypeInt.C 
SgAsmBinaryMultiply.C                  SgAsmElfDynamicEntry.C              SgAsmPEImportHNTEntry.C                    SgContainsStatement.C                 SgIncidenceDirectedGraph.C              SgOmpSingleStatement.C                       SgTypeLong.C 
SgAsmBinaryRor.C                       SgAsmElfDynamicEntryList.C          SgAsmPEImportHNTEntryList.C                SgContinueStmt.C                      SgIncidenceUndirectedGraph.C            SgOmpTaskStatement.C                         SgTypeLongDouble.C 
SgAsmBinarySubtract.C                  SgAsmElfDynamicSection.C            SgAsmPEImportILTEntry.C                    SgC_PreprocessorDirectiveStatement.C  SgIncludeDirectiveStatement.C           SgOmpTaskwaitStatement.C                     SgTypeLongLong.C 
SgAsmBinarySubtractPostupdate.C        SgAsmElfEHFrameEntryCI.C            SgAsmPEImportILTEntryList.C                SgCtorInitializerList.C               SgIncludeNextDirectiveStatement.C       SgOmpThreadprivateStatement.C                SgTypeModifier.C 
SgAsmBinarySubtractPreupdate.C         SgAsmElfEHFrameEntryCIList.C        SgAsmPEImportLookupTable.C                 SgDataStatementGroup.C                SgInitializedName.C                     SgOmpUntiedClause.C                          SgTypeShort.C 
SgAsmBlock.C                           SgAsmElfEHFrameEntryFD.C            SgAsmPEImportSection.C                     SgDataStatementObject.C               SgInitializer.C                         SgOmpVariablesClause.C                       SgTypeSignedChar.C 
SgAsmByteValueExpression.C             SgAsmElfEHFrameEntryFDList.C        SgAsmPERVASizePair.C                       SgDataStatementValue.C                SgInquireStatement.C                    SgOmpWorkshareStatement.C                    SgTypeSignedInt.C 
SgAsmCoffStrtab.C                      SgAsmElfEHFrameSection.C            SgAsmPERVASizePairList.C                   SgDeadIfDirectiveStatement.C          SgIntegerDivideOp.C                     SgOpenStatement.C                            SgTypeSignedLong.C 
SgAsmCoffSymbol.C                      SgAsmElfFileHeader.C                SgAsmPESection.C                           SgDeallocateStatement.C               SgInterfaceBody.C                       SgOptions.C                                  SgTypeSignedLongLong.C 
SgAsmCoffSymbolList.C                  SgAsmElfNoteEntry.C                 SgAsmPESectionTable.C                      SgDeclarationModifier.C               SgInterfaceStatement.C                  SgOrOp.C                                     SgTypeSignedShort.C 
SgAsmCoffSymbolTable.C                 SgAsmElfNoteEntryList.C             SgAsmPESectionTableEntry.C                 SgDeclarationStatement.C              SgInterfaceSymbol.C                     SgParameterStatement.C                       SgTypeString.C 
SgAsmCommonSubExpression.C             SgAsmElfNoteSection.C               SgAsmPEStringSection.C                     SgDefaultOptionStmt.C                 SgIntKeyedBidirectionalGraph.C          SgPartialFunctionModifierType.C              SgTypeUnknown.C 
SgAsmControlFlagsExpression.C          SgAsmElfRelocEntry.C                SgAsmPowerpcInstruction.C                  SgDefaultSymbol.C                     SgIntrinsicSymbol.C                     SgPartialFunctionType.C                      SgTypeUnsignedChar.C 
SgAsmDataStructureDeclaration.C        SgAsmElfRelocEntryList.C            SgAsmPowerpcRegisterReferenceExpression.C  SgDefineDirectiveStatement.C          SgIntVal.C                              SgPlusAssignOp.C                             SgTypeUnsignedInt.C 
SgAsmDeclaration.C                     SgAsmElfRelocSection.C              SgAsmQuadWordValueExpression.C             SgDeleteExp.C                         SgIOItemExpression.C                    SgPlusPlusOp.C                               SgTypeUnsignedLong.C 
SgAsmDOSExtendedHeader.C               SgAsmElfSection.C                   SgAsmRegisterReferenceExpression.C         SgDerivedTypeStatement.C              SgIorAssignOp.C                         SgPntrArrRefExp.C                            SgTypeUnsignedLongLong.C 
SgAsmDOSFileHeader.C                   SgAsmElfSectionTable.C              SgAsmSingleFloatValueExpression.C          SgDesignatedInitializer.C             SgIOStatement.C                         SgPointerAssignOp.C                          SgTypeUnsignedShort.C 
SgAsmDoubleFloatValueExpression.C      SgAsmElfSectionTableEntry.C         SgAsmStatement.C                           SgDimensionObject.C                   SgLabelRefExp.C                         SgPointerDerefExp.C                          SgTypeVoid.C 
SgAsmDoubleWordValueExpression.C       SgAsmElfSegmentTable.C              SgAsmStmt.C                                SgDirectedGraphEdge.C                 SgLabelStatement.C                      SgPointerMemberType.C                        SgTypeWchar.C 
SgAsmDwarfAccessDeclaration.C          SgAsmElfSegmentTableEntry.C         SgAsmStoredString.C                        SgDirectory.C                         SgLabelSymbol.C                         SgPointerType.C                              SgUnaryAddOp.C 
SgAsmDwarfArrayType.C                  SgAsmElfSegmentTableEntryList.C     SgAsmStringStorage.C                       SgDirectoryList.C                     SgLessOrEqualOp.C                       SgPragma.C                                   SgUnaryOp.C 
SgAsmDwarfBaseType.C                   SgAsmElfStringSection.C             SgAsmType128bitFloat.C                     SgDivAssignOp.C                       SgLessThanOp.C                          SgPragmaDeclaration.C                        SgUndefDirectiveStatement.C 
SgAsmDwarfCatchBlock.C                 SgAsmElfStrtab.C                    SgAsmType80bitFloat.C                      SgDivideOp.C                          SgLineDirectiveStatement.C              SgPrintStatement.C                           SgUndirectedGraphEdge.C 
SgAsmDwarfClassTemplate.C              SgAsmElfSymbol.C                    SgAsmTypeByte.C                            SgDotExp.C                            SgLinemarkerDirectiveStatement.C        SgProcedureHeaderStatement.C                 SgUnknownArrayOrFunctionReference.C 
SgAsmDwarfClassType.C                  SgAsmElfSymbolList.C                SgAsmType.C                                SgDotStarOp.C                         SgLinkageModifier.C                     SgProgramHeaderStatement.C                   SgUnknownFile.C 
SgAsmDwarfCommonBlock.C                SgAsmElfSymbolSection.C             SgAsmTypeDoubleFloat.C                     SgDoubleVal.C                         SgLocatedNode.C                         SgProject.C                                  SgUnparse_Info.C 
SgAsmDwarfCommonInclusion.C            SgAsmElfSymverDefinedAux.C          SgAsmTypeDoubleQuadWord.C                  SgDoWhileStmt.C                       SgLocatedNodeSupport.C                  SgPseudoDestructorRefExp.C                   SgUnsignedCharVal.C 
SgAsmDwarfCompilationUnit.C            SgAsmElfSymverDefinedAuxList.C      SgAsmTypeDoubleWord.C                      SgElaboratedTypeModifier.C            SgLongDoubleVal.C                       SgQualifiedName.C                            SgUnsignedIntVal.C 
SgAsmDwarfCompilationUnitList.C        SgAsmElfSymverDefinedEntry.C        SgAsmTypeQuadWord.C                        SgElseDirectiveStatement.C            SgLongIntVal.C                          SgQualifiedNameType.C                        SgUnsignedLongLongIntVal.C 
SgAsmDwarfCondition.C                  SgAsmElfSymverDefinedEntryList.C    SgAsmTypeSingleFloat.C                     SgElseifDirectiveStatement.C          SgLongLongIntVal.C                      SgReadStatement.C                            SgUnsignedLongVal.C 
SgAsmDwarfConstant.C                   SgAsmElfSymverDefinedSection.C      SgAsmTypeVector.C                          SgElseWhereStatement.C                SgLshiftAssignOp.C                      SgRealPartOp.C                               SgUnsignedShortVal.C 
SgAsmDwarfConstruct.C                  SgAsmElfSymverEntry.C               SgAsmTypeWord.C                            SgEmptyDirectiveStatement.C           SgLshiftOp.C                            SgReferenceType.C                            SgUPC_AccessModifier.C 
SgAsmDwarfConstructList.C              SgAsmElfSymverEntryList.C           SgAsmUnaryArmSpecialRegisterList.C         SgEndfileStatement.C                  SgMemberFunctionDeclaration.C           SgRefExp.C                                   SgUpcBarrierStatement.C 
SgAsmDwarfConstType.C                  SgAsmElfSymverNeededAux.C
SgAsmUnaryExpression.C                     SgEndifDirectiveStatement.C
SgMemberFunctionRefExp.C                SgRenamePair.C
SgUpcBlocksizeofExpression.C 
SgAsmDwarfDwarfProcedure.C             SgAsmElfSymverNeededAuxList.C
SgAsmUnaryMinus.C                          SgEntryStatement.C
SgMemberFunctionSymbol.C                SgRenameSymbol.C
SgUpcElemsizeofExpression.C 
SgAsmDwarfEntryPoint.C                 SgAsmElfSymverNeededEntry.C         SgAsmUnaryPlus.C                           SgEnumDeclaration.C                   SgMemberFunctionType.C                  SgReturnStmt.C                               SgUpcFenceStatement.C 
SgAsmDwarfEnumerationType.C            SgAsmElfSymverNeededEntryList.C     SgAsmUnaryRrx.C                            SgEnumFieldSymbol.C                   SgMinusAssignOp.C                       SgRewindStatement.C                          SgUpcForAllStatement.C 
SgAsmDwarfEnumerator.C                 SgAsmElfSymverNeededSection.C
SgAsmValueExpression.C                     SgEnumSymbol.C
SgMinusMinusOp.C                        SgRshiftAssignOp.C
SgUpcLocalsizeofExpression.C 
SgAsmDwarfFileType.C                   SgAsmElfSymverSection.C             SgAsmVectorValueExpression.C               SgEnumType.C                          SgMinusOp.C                             SgRshiftOp.C                                 SgUpcMythread.C 
SgAsmDwarfFormalParameter.C            SgAsmExecutableFileFormat.C         SgAsmWordValueExpression.C                 SgEnumVal.C                           SgModAssignOp.C                         SgScopeOp.C                                  SgUpcNotifyStatement.C 
SgAsmDwarfFormatLabel.C                SgAsmExpression.C                   SgAsmx86Instruction.C                      SgEqualityOp.C                        SgModifier.C                            SgScopeStatement.C                           SgUpcThreads.C 
SgAsmDwarfFriend.C                     SgAsmExprListExp.C                  SgAsmx86RegisterReferenceExpression.C      SgEquivalenceStatement.C              SgModifierNodes.C                       SgSequenceStatement.C                        SgUpcWaitStatement.C 
SgAsmDwarfFunctionTemplate.C           SgAsmFieldDeclaration.C             SgAssignedGotoStatement.C                  SgErrorDirectiveStatement.C           SgModifierType.C                        SgShortVal.C                                 SgUserDefinedBinaryOp.C 
SgAsmDwarfImportedDeclaration.C        SgAsmFunctionDeclaration.C          SgAssignInitializer.C                      SgExponentiationOp.C                  SgModOp.C                               SgSizeOfOp.C                                 SgUserDefinedUnaryOp.C 
SgAsmDwarfImportedModule.C             SgAsmGenericDLL.C                   SgAssignOp.C                               SgExpression.C                        SgModuleStatement.C                     SgSourceFile.C                               SgUseStatement.C 
SgAsmDwarfImportedUnit.C               SgAsmGenericDLLList.C               SgAssignStatement.C                        SgExpressionRoot.C                    SgModuleSymbol.C                        SgSpawnStmt.C                                SgUsingDeclarationStatement.C 
SgAsmDwarfInformation.C                SgAsmGenericFile.C                  SgAssociateStatement.C                     SgExprListExp.C                       SgMultAssignOp.C                        SgSpecialFunctionModifier.C                  SgUsingDirectiveStatement.C 
SgAsmDwarfInheritance.C                SgAsmGenericFileList.C              SgAsteriskShapeExp.C                       SgExprStatement.C                     SgMultiplyOp.C                          SgStatement.C                                SgValueExp.C 
SgAsmDwarfInlinedSubroutine.C          SgAsmGenericFormat.C                SgAttribute.C                              SgFile.C                              SgName.C                                SgStatementExpression.C                      SgVarArgCopyOp.C 
SgAsmDwarfInterfaceType.C              SgAsmGenericHeader.C                SgAttributeSpecificationStatement.C        Sg_File_Info.C                        SgNamedType.C                           SgStatementFunctionStatement.C               SgVarArgEndOp.C 
SgAsmDwarfLabel.C                      SgAsmGenericHeaderList.C            SgBackspaceStatement.C                     SgFileList.C                          SgNameGroup.C                           SgStopOrPauseStatement.C                     SgVarArgOp.C 
SgAsmDwarfLexicalBlock.C               SgAsmGenericSection.C               SgBaseClass.C                              SgFloatVal.C                          SgNamelistStatement.C                   SgStorageModifier.C                          SgVarArgStartOneOperandOp.C 
SgAsmDwarfLine.C                       SgAsmGenericSectionList.C           SgBaseClassModifier.C                      SgFlushStatement.C                    SgNamespaceAliasDeclarationStatement.C  SgStringKeyedBidirectionalGraph.C            SgVarArgStartOp.C 
SgAsmDwarfLineList.C                   SgAsmGenericString.C                SgBasicBlock.C                             SgForAllStatement.C                   SgNamespaceDeclarationStatement.C       SgStringVal.C                                SgVariableDeclaration.C 
SgAsmDwarfMacro.C                      SgAsmGenericStrtab.C                SgBidirectionalGraph.C                     SgForInitStatement.C                  SgNamespaceDefinitionStatement.C        SgSubscriptExpression.C                      SgVariableDefinition.C 
SgAsmDwarfMacroList.C                  SgAsmGenericSymbol.C                SgBinaryComposite.C                        SgFormatItem.C                        SgNamespaceSymbol.C                     SgSubtractOp.C                               SgVariableSymbol.C 
SgAsmDwarfMember.C                     SgAsmGenericSymbolList.C            SgBinaryOp.C                               SgFormatItemList.C                    SgNewExp.C                              SgSupport.C                                  SgVariantExpression.C 
SgAsmDwarfModule.C                     SgAsmInstruction.C                  SgBitAndOp.C                               SgFormatStatement.C                   SgNode.C                                SgSwitchStatement.C                          SgVariantStatement.C 
SgAsmDwarfMutableType.C                SgAsmInterpretation.C               SgBitAttribute.C                           SgForStatement.C                      SgNotEqualOp.C                          SgSymbol.C                                   SgVarRefExp.C 
SgAsmDwarfNamelist.C                   SgAsmInterpretationList.C           SgBitComplementOp.C                        SgFortranDo.C                         SgNotOp.C                               SgSymbolTable.C                              SgWaitStatement.C 
SgAsmDwarfNamelistItem.C               SgAsmLEEntryPoint.C                 SgBitOrOp.C                                SgFortranIncludeLine.C                SgNullExpression.C                      SgTemplateArgument.C                         SgWarningDirectiveStatement.C 
SgAsmDwarfNamespace.C                  SgAsmLEEntryTable.C                 SgBitXorOp.C                               SgFortranNonblockedDo.C               SgNullifyStatement.C                    SgTemplateArgumentList.C                     SgWcharVal.C 
SgAsmDwarfPackedType.C                 SgAsmLEFileHeader.C                 SgBlockDataStatement.C                     SgFuncDecl_attr.C                     SgNullStatement.C                       SgTemplateDeclaration.C                      SgWhereStatement.C 
SgAsmDwarfPartialUnit.C                SgAsmLENameTable.C                  SgBoolValExp.C                             SgFunctionCallExp.C                   SgOmpAtomicStatement.C                  SgTemplateInstantiationDecl.C                SgWhileStmt.C 
SgAsmDwarfPointerType.C                SgAsmLEPageTable.C                  SgBreakStmt.C                              SgFunctionDeclaration.C               SgOmpBarrierStatement.C                 SgTemplateInstantiationDefn.C                SgWriteStatement.C 
SgAsmDwarfPtrToMemberType.C            SgAsmLEPageTableEntry.C             SgCaseOptionStmt.C                         SgFunctionDefinition.C                SgOmpBodyStatement.C                    SgTemplateInstantiationDirectiveStatement.C  SgXorAssignOp.C 
SgAsmDwarfReferenceType.C              SgAsmLERelocTable.C                 SgCastExp.C                                SgFunctionModifier.C                  SgOmpClauseBodyStatement.C              SgTemplateInstantiationFunctionDecl.C

   )


message ( STATUS ">>>>>>>>>>>>>>>>>>>>>> CxxGrammarMetaProgram_SRCS : " ${CxxGrammarMetaProgram_SRCS} )
get_target_property (META_EXE_LOC CxxGrammarMetaProgram LOCATION) 


# define a custom command to call the code generator and generate source/header files
# Generate the edg-sage connection using ROSETTA
add_custom_command(
   OUTPUT ${libgeneratedIR_SRC}
#  depend on the code generator and input files
   DEPENDS  CxxGrammarMetaProgram ${GRAMMAR_SOURCE_FILES}
#  We need to copy the input files  to the build tree since 
#  the code generator will output the generated source files into the same directory as where the input files locate
   COMMAND
      ${CMAKE_COMMAND}  -E make_directory  ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/Grammar
   COMMAND
      ${CMAKE_COMMAND}  -E make_directory  ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
   COMMAND
       ${CMAKE_COMMAND}  -E copy_directory  ${ROSE_TOP_SRC_DIR}/src/ROSETTA/Grammar/ ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/Grammar/
#   create this directory is needed since a few generated files will be put there        
   COMMAND ${META_EXE_LOC}
      ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/
)



add_library( libgeneratedIR STATIC ${libgeneratedIR_SRC} )
add_dependencies( libgeneratedIR CxxGrammarMetaProgram)

########### install files ###############

# Need to define a mechanism to collect a list of all files (later).
install(FILES  SgAccessModifier.h SgAsmDwarfRestrictType.h DESTINATION ${INCLUDE_INSTALL_DIR})



endif ( enable-smaller-generated-files )
