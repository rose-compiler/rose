include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# This directory support the separation of the generated files used for the ROSE IR
# into smaller files (to avoid a few large 300K line files).
SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES)

EXTRA_DIST = CMakeLists.txt 

# Use a makefile include meachanism.  However I can't make this work!
# include $(top_builddir)/src/frontend/SageIII/GENERATED_CODE_DIRECTORY_Cxx_Grammar/generatedFileList.inc
# GENERATED_SOURCE = SgAddOp.C

if ROSE_USE_SMALLER_GENERATED_FILES

haveRosettaGeneratedSource:
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
	touch haveRosettaGeneratedSource

# Force ROSETTA to be run to generate the ROSE IR source code.
force-haveRosettaGeneratedSource:
	-rm *.h *.C
	$(MAKE) haveRosettaGeneratedSource

$(GENERATED_SOURCE): haveRosettaGeneratedSource

noinst_LTLIBRARIES = libgeneratedIR.la
nodist_libgeneratedIR_la_SOURCES = $(GENERATED_SOURCE)
# nodist_libgeneratedIR_la_SOURCES = $(GENERATED_SOURCE)
endif

# DQ (10/18/2007): Added AST_FILE_IO.h StorageClasses.h
# Removed sla.h (redundant with location in src/util/commandlineProcessing/sla.h)
# include_HEADERS = *.h

test:
	echo "Force ROSETTA to regenerate the source code for the ROSE IR."
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)

CLEANFILES = haveRosettaGeneratedSource

clean-local:
#	rm -rf libgeneratedIR.la
	rm -rf *.h *.C *.loT

printFiles:
	echo "GENERATED_SOURCE = $(GENERATED_SOURCE)"

printObjFiles:
	echo "GENERATED_OBJ = $(GENERATED_OBJ)"

distclean-local:


# DQ (12/29/2009): It seems that I am forced to express the list of source files explicitly.
# So it seems that this list will have to be modified if any new IR nodes are added.
SHORT_GENERATED_SOURCE = \
SgAccessModifier.C

GENERATED_SOURCE = \
SgAccessModifier.C                     SgAsmDwarfRestrictType.C            SgAsmLESection.C                           SgCatchOptionStmt.C                   SgFunctionParameterList.C               SgOmpClause.C                                SgTemplateInstantiationMemberFunctionDecl.C \
SgActualArgumentExpression.C           SgAsmDwarfSetType.C                 SgAsmLESectionTable.C                      SgCatchStatementSeq.C                 SgFunctionParameterTypeList.C           SgOmpCollapseClause.C                        SgTemplateParameter.C \
SgAddOp.C                              SgAsmDwarfSharedType.C              SgAsmLESectionTableEntry.C                 SgCharVal.C                           SgFunctionRefExp.C                      SgOmpCopyinClause.C                          SgTemplateParameterList.C \
SgAddressOfOp.C                        SgAsmDwarfStringType.C              SgAsmMemoryReferenceExpression.C           SgClassDeclaration.C                  SgFunctionSymbol.C                      SgOmpCopyprivateClause.C                     SgTemplateSymbol.C \
SgAggregateInitializer.C               SgAsmDwarfStructureType.C           SgAsmNEEntryPoint.C                        SgClassDecl_attr.C                    SgFunctionType.C                        SgOmpCriticalStatement.C                     SgTemplateType.C \
SgAliasSymbol.C                        SgAsmDwarfSubprogram.C              SgAsmNEEntryTable.C                        SgClassDefinition.C                   SgFunctionTypeSymbol.C                  SgOmpDefaultClause.C                         SgThisExp.C \
SgAllocateStatement.C                  SgAsmDwarfSubrangeType.C            SgAsmNEFileHeader.C                        SgClassNameRefExp.C                   SgFunctionTypeTable.C                   SgOmpDoStatement.C                           SgThrowOp.C \
SgAndAssignOp.C                        SgAsmDwarfSubroutineType.C          SgAsmNEModuleTable.C                       SgClassSymbol.C                       SgGlobal.C                              SgOmpExpressionClause.C                      SgToken.C \
SgAndOp.C                              SgAsmDwarfTemplateTypeParameter.C   SgAsmNENameTable.C                         SgClassType.C                         SgGotoStatement.C                       SgOmpFirstprivateClause.C                    SgTryStmt.C \
SgArithmeticIfStatement.C              SgAsmDwarfTemplateValueParameter.C  SgAsmNERelocEntry.C                        SgClinkageDeclarationStatement.C      SgGraph.C                               SgOmpFlushStatement.C                        SgTypeBool.C \
SgArrayType.C                          SgAsmDwarfThrownType.C              SgAsmNERelocTable.C                        SgClinkageEndStatement.C              SgGraphEdge.C                           SgOmpForStatement.C                          SgType.C \
SgArrowExp.C                           SgAsmDwarfTryBlock.C                SgAsmNESection.C                           SgClinkageStartStatement.C            SgGraphEdgeList.C                       SgOmpIfClause.C                              SgTypeChar.C \
SgArrowStarOp.C                        SgAsmDwarfTypedef.C                 SgAsmNESectionTable.C                      SgCloseStatement.C                    SgGraphNode.C                           SgOmpLastprivateClause.C                     SgTypeComplex.C \
SgAsmArmInstruction.C                  SgAsmDwarfUnionType.C               SgAsmNESectionTableEntry.C                 SgColonShapeExp.C                     SgGraphNodeList.C                       SgOmpMasterStatement.C                       SgTypeDefault.C \
SgAsmArmRegisterReferenceExpression.C  SgAsmDwarfUnknownConstruct.C        SgAsmNEStringTable.C                       SgCommaOpExp.C                        SgGreaterOrEqualOp.C                    SgOmpNowaitClause.C                          SgTypedefDeclaration.C \
SgAsmBasicString.C                     SgAsmDwarfUnspecifiedParameters.C   SgAsmNode.C                                SgCommonBlock.C                       SgGreaterThanOp.C                       SgOmpNumThreadsClause.C                      SgTypedefSeq.C \
SgAsmBinaryAdd.C                       SgAsmDwarfUnspecifiedType.C         SgAsmOp.C                                  SgCommonBlockObject.C                 SgIdentDirectiveStatement.C             SgOmpOrderedClause.C                         SgTypedefSymbol.C \
SgAsmBinaryAddPostupdate.C             SgAsmDwarfUpcRelaxedType.C          SgAsmOperandList.C                         SgCommonSymbol.C                      SgIfdefDirectiveStatement.C             SgOmpOrderedStatement.C                      SgTypedefType.C \
SgAsmBinaryAddPreupdate.C              SgAsmDwarfUpcSharedType.C           SgAsmPEExportDirectory.C                   SgComplexVal.C                        SgIfDirectiveStatement.C                SgOmpParallelStatement.C                     SgTypeDouble.C \
SgAsmBinaryAsr.C                       SgAsmDwarfUpcStrictType.C           SgAsmPEExportEntry.C                       SgComputedGotoStatement.C             SgIfndefDirectiveStatement.C            SgOmpPrivateClause.C                         SgTypeEllipse.C \
SgAsmBinaryDivide.C                    SgAsmDwarfVariable.C                SgAsmPEExportEntryList.C                   SgConcatenationOp.C                   SgIfStmt.C                              SgOmpReductionClause.C                       SgTypeFloat.C \
SgAsmBinaryExpression.C                SgAsmDwarfVariant.C                 SgAsmPEExportSection.C                     SgConditionalExp.C                    SgImagPartOp.C                          SgOmpScheduleClause.C                        SgTypeGlobalVoid.C \
SgAsmBinaryLsl.C                       SgAsmDwarfVariantPart.C             SgAsmPEFileHeader.C                        SgConjugateOp.C                       SgImplicitStatement.C                   SgOmpSectionsStatement.C                     SgTypeIdOp.C \
SgAsmBinaryLsr.C                       SgAsmDwarfVolatileType.C            SgAsmPEImportDirectory.C                   SgConstructorInitializer.C            SgImpliedDo.C                           SgOmpSectionStatement.C                      SgTypeImaginary.C \
SgAsmBinaryMod.C                       SgAsmDwarfWithStmt.C                SgAsmPEImportDirectoryList.C               SgConstVolatileModifier.C             SgImportStatement.C                     SgOmpSharedClause.C                          SgTypeInt.C \
SgAsmBinaryMultiply.C                  SgAsmElfDynamicEntry.C              SgAsmPEImportHNTEntry.C                    SgContainsStatement.C                 SgIncidenceDirectedGraph.C              SgOmpSingleStatement.C                       SgTypeLong.C \
SgAsmBinaryRor.C                       SgAsmElfDynamicEntryList.C          SgAsmPEImportHNTEntryList.C                SgContinueStmt.C                      SgIncidenceUndirectedGraph.C            SgOmpTaskStatement.C                         SgTypeLongDouble.C \
SgAsmBinarySubtract.C                  SgAsmElfDynamicSection.C            SgAsmPEImportILTEntry.C                    SgC_PreprocessorDirectiveStatement.C  SgIncludeDirectiveStatement.C           SgOmpTaskwaitStatement.C                     SgTypeLongLong.C \
SgAsmBinarySubtractPostupdate.C        SgAsmElfEHFrameEntryCI.C            SgAsmPEImportILTEntryList.C                SgCtorInitializerList.C               SgIncludeNextDirectiveStatement.C       SgOmpThreadprivateStatement.C                SgTypeModifier.C \
SgAsmBinarySubtractPreupdate.C         SgAsmElfEHFrameEntryCIList.C        SgAsmPEImportLookupTable.C                 SgDataStatementGroup.C                SgInitializedName.C                     SgOmpUntiedClause.C                          SgTypeShort.C \
SgAsmBlock.C                           SgAsmElfEHFrameEntryFD.C            SgAsmPEImportSection.C                     SgDataStatementObject.C               SgInitializer.C                         SgOmpVariablesClause.C                       SgTypeSignedChar.C \
SgAsmByteValueExpression.C             SgAsmElfEHFrameEntryFDList.C        SgAsmPERVASizePair.C                       SgDataStatementValue.C                SgInquireStatement.C                    SgOmpWorkshareStatement.C                    SgTypeSignedInt.C \
SgAsmCoffStrtab.C                      SgAsmElfEHFrameSection.C            SgAsmPERVASizePairList.C                   SgDeadIfDirectiveStatement.C          SgIntegerDivideOp.C                     SgOpenStatement.C                            SgTypeSignedLong.C \
SgAsmCoffSymbol.C                      SgAsmElfFileHeader.C                SgAsmPESection.C                           SgDeallocateStatement.C               SgInterfaceBody.C                       SgOptions.C                                  SgTypeSignedLongLong.C \
SgAsmCoffSymbolList.C                  SgAsmElfNoteEntry.C                 SgAsmPESectionTable.C                      SgDeclarationModifier.C               SgInterfaceStatement.C                  SgOrOp.C                                     SgTypeSignedShort.C \
SgAsmCoffSymbolTable.C                 SgAsmElfNoteEntryList.C             SgAsmPESectionTableEntry.C                 SgDeclarationStatement.C              SgInterfaceSymbol.C                     SgParameterStatement.C                       SgTypeString.C \
SgAsmCommonSubExpression.C             SgAsmElfNoteSection.C               SgAsmPEStringSection.C                     SgDefaultOptionStmt.C                 SgIntKeyedBidirectionalGraph.C          SgPartialFunctionModifierType.C              SgTypeUnknown.C \
SgAsmControlFlagsExpression.C          SgAsmElfRelocEntry.C                SgAsmPowerpcInstruction.C                  SgDefaultSymbol.C                     SgIntrinsicSymbol.C                     SgPartialFunctionType.C                      SgTypeUnsignedChar.C \
SgAsmDataStructureDeclaration.C        SgAsmElfRelocEntryList.C            SgAsmPowerpcRegisterReferenceExpression.C  SgDefineDirectiveStatement.C          SgIntVal.C                              SgPlusAssignOp.C                             SgTypeUnsignedInt.C \
SgAsmDeclaration.C                     SgAsmElfRelocSection.C              SgAsmQuadWordValueExpression.C             SgDeleteExp.C                         SgIOItemExpression.C                    SgPlusPlusOp.C                               SgTypeUnsignedLong.C \
SgAsmDOSExtendedHeader.C               SgAsmElfSection.C                   SgAsmRegisterReferenceExpression.C         SgDerivedTypeStatement.C              SgIorAssignOp.C                         SgPntrArrRefExp.C                            SgTypeUnsignedLongLong.C \
SgAsmDOSFileHeader.C                   SgAsmElfSectionTable.C              SgAsmSingleFloatValueExpression.C          SgDesignatedInitializer.C             SgIOStatement.C                         SgPointerAssignOp.C                          SgTypeUnsignedShort.C \
SgAsmDoubleFloatValueExpression.C      SgAsmElfSectionTableEntry.C         SgAsmStatement.C                           SgDimensionObject.C                   SgLabelRefExp.C                         SgPointerDerefExp.C                          SgTypeVoid.C \
SgAsmDoubleWordValueExpression.C       SgAsmElfSegmentTable.C              SgAsmStmt.C                                SgDirectedGraphEdge.C                 SgLabelStatement.C                      SgPointerMemberType.C                        SgTypeWchar.C \
SgAsmDwarfAccessDeclaration.C          SgAsmElfSegmentTableEntry.C         SgAsmStoredString.C                        SgDirectory.C                         SgLabelSymbol.C                         SgPointerType.C                              SgUnaryAddOp.C \
SgAsmDwarfArrayType.C                  SgAsmElfSegmentTableEntryList.C     SgAsmStringStorage.C                       SgDirectoryList.C                     SgLessOrEqualOp.C                       SgPragma.C                                   SgUnaryOp.C \
SgAsmDwarfBaseType.C                   SgAsmElfStringSection.C             SgAsmType128bitFloat.C                     SgDivAssignOp.C                       SgLessThanOp.C                          SgPragmaDeclaration.C                        SgUndefDirectiveStatement.C \
SgAsmDwarfCatchBlock.C                 SgAsmElfStrtab.C                    SgAsmType80bitFloat.C                      SgDivideOp.C                          SgLineDirectiveStatement.C              SgPrintStatement.C                           SgUndirectedGraphEdge.C \
SgAsmDwarfClassTemplate.C              SgAsmElfSymbol.C                    SgAsmTypeByte.C                            SgDotExp.C                            SgLinemarkerDirectiveStatement.C        SgProcedureHeaderStatement.C                 SgUnknownArrayOrFunctionReference.C \
SgAsmDwarfClassType.C                  SgAsmElfSymbolList.C                SgAsmType.C                                SgDotStarOp.C                         SgLinkageModifier.C                     SgProgramHeaderStatement.C                   SgUnknownFile.C \
SgAsmDwarfCommonBlock.C                SgAsmElfSymbolSection.C             SgAsmTypeDoubleFloat.C                     SgDoubleVal.C                         SgLocatedNode.C                         SgProject.C                                  SgUnparse_Info.C \
SgAsmDwarfCommonInclusion.C            SgAsmElfSymverDefinedAux.C          SgAsmTypeDoubleQuadWord.C                  SgDoWhileStmt.C                       SgLocatedNodeSupport.C                  SgPseudoDestructorRefExp.C                   SgUnsignedCharVal.C \
SgAsmDwarfCompilationUnit.C            SgAsmElfSymverDefinedAuxList.C      SgAsmTypeDoubleWord.C                      SgElaboratedTypeModifier.C            SgLongDoubleVal.C                       SgQualifiedName.C                            SgUnsignedIntVal.C \
SgAsmDwarfCompilationUnitList.C        SgAsmElfSymverDefinedEntry.C        SgAsmTypeQuadWord.C                        SgElseDirectiveStatement.C            SgLongIntVal.C                          SgQualifiedNameType.C                        SgUnsignedLongLongIntVal.C \
SgAsmDwarfCondition.C                  SgAsmElfSymverDefinedEntryList.C    SgAsmTypeSingleFloat.C                     SgElseifDirectiveStatement.C          SgLongLongIntVal.C                      SgReadStatement.C                            SgUnsignedLongVal.C \
SgAsmDwarfConstant.C                   SgAsmElfSymverDefinedSection.C      SgAsmTypeVector.C                          SgElseWhereStatement.C                SgLshiftAssignOp.C                      SgRealPartOp.C                               SgUnsignedShortVal.C \
SgAsmDwarfConstruct.C                  SgAsmElfSymverEntry.C               SgAsmTypeWord.C                            SgEmptyDirectiveStatement.C           SgLshiftOp.C                            SgReferenceType.C                            SgUPC_AccessModifier.C \
SgAsmDwarfConstructList.C              SgAsmElfSymverEntryList.C           SgAsmUnaryArmSpecialRegisterList.C         SgEndfileStatement.C                  SgMemberFunctionDeclaration.C           SgRefExp.C                                   SgUpcBarrierStatement.C \
SgAsmDwarfConstType.C                  SgAsmElfSymverNeededAux.C           SgAsmUnaryExpression.C                     SgEndifDirectiveStatement.C           SgMemberFunctionRefExp.C                SgRenamePair.C                               SgUpcBlocksizeof.C \
SgAsmDwarfDwarfProcedure.C             SgAsmElfSymverNeededAuxList.C       SgAsmUnaryMinus.C                          SgEntryStatement.C                    SgMemberFunctionSymbol.C                SgRenameSymbol.C                             SgUpcElemsizeof.C \
SgAsmDwarfEntryPoint.C                 SgAsmElfSymverNeededEntry.C         SgAsmUnaryPlus.C                           SgEnumDeclaration.C                   SgMemberFunctionType.C                  SgReturnStmt.C                               SgUpcFenceStatement.C \
SgAsmDwarfEnumerationType.C            SgAsmElfSymverNeededEntryList.C     SgAsmUnaryRrx.C                            SgEnumFieldSymbol.C                   SgMinusAssignOp.C                       SgRewindStatement.C                          SgUpcForAllStatement.C \
SgAsmDwarfEnumerator.C                 SgAsmElfSymverNeededSection.C       SgAsmValueExpression.C                     SgEnumSymbol.C                        SgMinusMinusOp.C                        SgRshiftAssignOp.C                           SgUpcLocalsizeof.C \
SgAsmDwarfFileType.C                   SgAsmElfSymverSection.C             SgAsmVectorValueExpression.C               SgEnumType.C                          SgMinusOp.C                             SgRshiftOp.C                                 SgUpcMythread.C \
SgAsmDwarfFormalParameter.C            SgAsmExecutableFileFormat.C         SgAsmWordValueExpression.C                 SgEnumVal.C                           SgModAssignOp.C                         SgScopeOp.C                                  SgUpcNotifyStatement.C \
SgAsmDwarfFormatLabel.C                SgAsmExpression.C                   SgAsmx86Instruction.C                      SgEqualityOp.C                        SgModifier.C                            SgScopeStatement.C                           SgUpcThreads.C \
SgAsmDwarfFriend.C                     SgAsmExprListExp.C                  SgAsmx86RegisterReferenceExpression.C      SgEquivalenceStatement.C              SgModifierNodes.C                       SgSequenceStatement.C                        SgUpcWaitStatement.C \
SgAsmDwarfFunctionTemplate.C           SgAsmFieldDeclaration.C             SgAssignedGotoStatement.C                  SgErrorDirectiveStatement.C           SgModifierType.C                        SgShortVal.C                                 SgUserDefinedBinaryOp.C \
SgAsmDwarfImportedDeclaration.C        SgAsmFunction.C          SgAssignInitializer.C                      SgExponentiationOp.C                  SgModOp.C                               SgSizeOfOp.C                                 SgUserDefinedUnaryOp.C \
SgAsmDwarfImportedModule.C             SgAsmGenericDLL.C                   SgAssignOp.C                               SgExpression.C                        SgModuleStatement.C                     SgSourceFile.C                               SgUseStatement.C \
SgAsmDwarfImportedUnit.C               SgAsmGenericDLLList.C               SgAssignStatement.C                        SgExpressionRoot.C                    SgModuleSymbol.C                        SgSpawnStmt.C                                SgUsingDeclarationStatement.C \
SgAsmDwarfInformation.C                SgAsmGenericFile.C                  SgAssociateStatement.C                     SgExprListExp.C                       SgMultAssignOp.C                        SgSpecialFunctionModifier.C                  SgUsingDirectiveStatement.C \
SgAsmDwarfInheritance.C                SgAsmGenericFileList.C              SgAsteriskShapeExp.C                       SgExprStatement.C                     SgMultiplyOp.C                          SgStatement.C                                SgValueExp.C \
SgAsmDwarfInlinedSubroutine.C          SgAsmGenericFormat.C                SgAttribute.C                              SgFile.C                              SgName.C                                SgStatementExpression.C                      SgVarArgCopyOp.C \
SgAsmDwarfInterfaceType.C              SgAsmGenericHeader.C                SgAttributeSpecificationStatement.C        Sg_File_Info.C                        SgNamedType.C                           SgStatementFunctionStatement.C               SgVarArgEndOp.C \
SgAsmDwarfLabel.C                      SgAsmGenericHeaderList.C            SgBackspaceStatement.C                     SgFileList.C                          SgNameGroup.C                           SgStopOrPauseStatement.C                     SgVarArgOp.C \
SgAsmDwarfLexicalBlock.C               SgAsmGenericSection.C               SgBaseClass.C                              SgFloatVal.C                          SgNamelistStatement.C                   SgStorageModifier.C                          SgVarArgStartOneOperandOp.C \
SgAsmDwarfLine.C                       SgAsmGenericSectionList.C           SgBaseClassModifier.C                      SgFlushStatement.C                    SgNamespaceAliasDeclarationStatement.C  SgStringKeyedBidirectionalGraph.C            SgVarArgStartOp.C \
SgAsmDwarfLineList.C                   SgAsmGenericString.C                SgBasicBlock.C                             SgForAllStatement.C                   SgNamespaceDeclarationStatement.C       SgStringVal.C                                SgVariableDeclaration.C \
SgAsmDwarfMacro.C                      SgAsmGenericStrtab.C                SgBidirectionalGraph.C                     SgForInitStatement.C                  SgNamespaceDefinitionStatement.C        SgSubscriptExpression.C                      SgVariableDefinition.C \
SgAsmDwarfMacroList.C                  SgAsmGenericSymbol.C                SgBinaryComposite.C                        SgFormatItem.C                        SgNamespaceSymbol.C                     SgSubtractOp.C                               SgVariableSymbol.C \
SgAsmDwarfMember.C                     SgAsmGenericSymbolList.C            SgBinaryOp.C                               SgFormatItemList.C                    SgNewExp.C                              SgSupport.C                                  SgVariantExpression.C \
SgAsmDwarfModule.C                     SgAsmInstruction.C                  SgBitAndOp.C                               SgFormatStatement.C                   SgNode.C                                SgSwitchStatement.C                          SgVariantStatement.C \
SgAsmDwarfMutableType.C                SgAsmInterpretation.C               SgBitAttribute.C                           SgForStatement.C                      SgNotEqualOp.C                          SgSymbol.C                                   SgVarRefExp.C \
SgAsmDwarfNamelist.C                   SgAsmInterpretationList.C           SgBitComplementOp.C                        SgFortranDo.C                         SgNotOp.C                               SgSymbolTable.C                              SgWaitStatement.C \
SgAsmDwarfNamelistItem.C               SgAsmLEEntryPoint.C                 SgBitOrOp.C                                SgFortranIncludeLine.C                SgNullExpression.C                      SgTemplateArgument.C                         SgWarningDirectiveStatement.C \
SgAsmDwarfNamespace.C                  SgAsmLEEntryTable.C                 SgBitXorOp.C                               SgFortranNonblockedDo.C               SgNullifyStatement.C                    SgTemplateArgumentList.C                     SgWcharVal.C \
SgAsmDwarfPackedType.C                 SgAsmLEFileHeader.C                 SgBlockDataStatement.C                     SgFuncDecl_attr.C                     SgNullStatement.C                       SgTemplateDeclaration.C                      SgWhereStatement.C \
SgAsmDwarfPartialUnit.C                SgAsmLENameTable.C                  SgBoolValExp.C                             SgFunctionCallExp.C                   SgOmpAtomicStatement.C                  SgTemplateInstantiationDecl.C                SgWhileStmt.C \
SgAsmDwarfPointerType.C                SgAsmLEPageTable.C                  SgBreakStmt.C                              SgFunctionDeclaration.C               SgOmpBarrierStatement.C                 SgTemplateInstantiationDefn.C                SgWriteStatement.C \
SgAsmDwarfPtrToMemberType.C            SgAsmLEPageTableEntry.C             SgCaseOptionStmt.C                         SgFunctionDefinition.C                SgOmpBodyStatement.C                    SgTemplateInstantiationDirectiveStatement.C  SgXorAssignOp.C \
SgAsmDwarfReferenceType.C              SgAsmLERelocTable.C                 SgCastExp.C                                SgFunctionModifier.C                  SgOmpClauseBodyStatement.C              SgTemplateInstantiationFunctionDecl.C        SgHeaderFileBody.C






