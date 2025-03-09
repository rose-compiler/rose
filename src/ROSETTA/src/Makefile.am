include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

EXTRA_DIST = CMakeLists.txt \
   ROSETTA_macros.h  grammar.h grammarString.h AstNodeClass.h

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .

CXX_GRAMMAR_DIR   = $(top_builddir)/src/frontend/SageIII

CXX_PREPROC_CODES = $(CXX_GRAMMAR_DIR)/Cxx_Grammar.h

BUILT_SOURCES = $(CXX_PREPROC_CODES)

# Rasmussen (04/17/2019): Support for ATerms has been deprecated as it is no longer needed
# and likely never fully implemented nor tested.  Files remain in the src tree but are no
# longer built. Files buildAtermSupport.C and grammarAtermConstructorDefinitionMacros.macro
# has been removed.

# The generation of the ROSE Grammars depend upon source code from these files
GRAMMAR_SOURCE_FILES = \
     ../Grammar/Statement.code \
     ../Grammar/grammarClassDefinitionMacros.macro \
     ../Grammar/grammarVariantFunctionDefinitionMacros.macro \
     ../Grammar/Common.code \
     ../Grammar/Support.code \
     ../Grammar/grammarConstructorDefinitionMacros.macro \
     ../Grammar/node.macro \
     ../Grammar/Cxx_GlobalDeclarations.macro \
     ../Grammar/Symbol.code \
     ../Grammar/grammarDestructorDefinitionMacros.macro \
     ../Grammar/parserCode.macro \
     ../Grammar/Cxx_Support_Header.macro \
     ../Grammar/Type.code \
     ../Grammar/grammarIsClassNameFunctionDefinitionMacros.macro \
     ../Grammar/parserPrototype.macro \
     ../Grammar/Expression.code \
     ../Grammar/copyMemberFunction.macro \
     ../Grammar/grammarMainClassDeclatationMacros.macro \
     ../Grammar/parserSourceCode.macro \
     ../Grammar/LocatedNode.code \
     ../Grammar/dataMemberAccessFunctions.macro \
     ../Grammar/grammarMainClassParseFunctionSourceCode.macro \
     ../Grammar/Node.code \
     ../Grammar/grammarClassDeclarationMacros.macro \
     ../Grammar/grammarMainClassSourceCodeMacros.macro \
     ../Grammar/grammarNewDeleteOperatorMacros.macro \
     ../Grammar/grammarTraverseMemoryPool.macro \
     ../Grammar/grammarNodeId.macro \
     ../Grammar/grammarCheckingIfDataMembersAreInMemoryPool.macro \
     ../Grammar/grammarStorageClassDeclatationMacros.macro \
     ../Grammar/grammarStorageClassDefinitionMacros.macro \
     ../Grammar/grammarAST_FileIoHeader.code \
     ../Grammar/grammarAST_FileIoSource.code  \
     ../Grammar/grammarSourceOfIRNodesAstFileIOSupport.macro \
     ../Grammar/grammarStaticDataManagingClassHeader.macro \
     ../Grammar/grammarStaticDataManagingClassStorageClassHeader.macro \
     ../Grammar/grammarReturnDataMemberPointers.macro \
     ../Grammar/grammarProcessDataMemberReferenceToPointers.macro \
     ../Grammar/grammarGetChildIndex.macro \
     ../astNodeList

# DQ (4/6/2006): Removed from Jochen's new version
# ../Grammar/grammarConstructorOfStorageClass.macro



# VALGRIND_OPTIONS = -v --num-callers=12 --leak-check=yes --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
# DQ (11/9/2016): Use option to turn off the memory leak detection because we are not 
# conserned with memory leaks in the ROSE code generator.
# VALGRIND = $(ADDRESS_SANITIZER_OPTIONS)
VALGRIND = ASAN_OPTIONS=detect_leaks=0
else
VALGRIND =
endif

# Files that CxxGrammarMetaProgram *might* generate, depending on what ROSE features are enabled.
rosetta_generated_definitions =				\
	  SgNode.h					\
	  SgAsmAarch32Coprocessor.h			\
	  SgAsmAarch32Instruction.h			\
	  SgAsmAarch64AtOperand.h			\
	  SgAsmAarch64BarrierOperand.h			\
	  SgAsmAarch64CImmediateOperand.h		\
	  SgAsmAarch64Instruction.h			\
	  SgAsmAarch64PrefetchOperand.h			\
	  SgAsmAarch64PState.h				\
	  SgAsmAarch64SysMoveOperand.h			\
	  SgAsmBasicString.h				\
	  SgAsmBinaryAdd.h				\
	  SgAsmBinaryAsr.h				\
	  SgAsmBinaryConcat.h				\
	  SgAsmBinaryDivide.h				\
	  SgAsmBinaryExpression.h			\
	  SgAsmBinaryLsl.h				\
	  SgAsmBinaryLsr.h				\
	  SgAsmBinaryMod.h				\
	  SgAsmBinaryMsl.h				\
	  SgAsmBinaryMultiply.h				\
	  SgAsmBinaryPostupdate.h			\
	  SgAsmBinaryPreupdate.h			\
	  SgAsmBinaryRor.h				\
	  SgAsmBinarySubtract.h				\
	  SgAsmBlock.h					\
	  SgAsmByteOrder.h				\
	  SgAsmCilAssembly.h				\
	  SgAsmCilAssemblyOS.h				\
	  SgAsmCilAssemblyOSTable.h			\
	  SgAsmCilAssemblyProcessor.h			\
	  SgAsmCilAssemblyProcessorTable.h		\
	  SgAsmCilAssemblyRef.h				\
	  SgAsmCilAssemblyRefOS.h			\
	  SgAsmCilAssemblyRefOSTable.h			\
	  SgAsmCilAssemblyRefProcessor.h		\
	  SgAsmCilAssemblyRefProcessorTable.h		\
	  SgAsmCilAssemblyRefTable.h			\
	  SgAsmCilAssemblyTable.h			\
	  SgAsmCilClassLayout.h				\
	  SgAsmCilClassLayoutTable.h			\
	  SgAsmCilConstant.h				\
	  SgAsmCilConstantTable.h			\
	  SgAsmCilCustomAttribute.h			\
	  SgAsmCilCustomAttributeTable.h		\
	  SgAsmCilDataStream.h				\
	  SgAsmCilDeclSecurity.h			\
	  SgAsmCilDeclSecurityTable.h			\
	  SgAsmCilEvent.h				\
	  SgAsmCilEventMap.h				\
	  SgAsmCilEventMapTable.h			\
	  SgAsmCilEventTable.h				\
	  SgAsmCilExceptionData.h			\
	  SgAsmCilExportedType.h			\
	  SgAsmCilExportedTypeTable.h			\
	  SgAsmCilField.h				\
	  SgAsmCilFieldLayout.h				\
	  SgAsmCilFieldLayoutTable.h			\
	  SgAsmCilFieldMarshal.h			\
	  SgAsmCilFieldMarshalTable.h			\
	  SgAsmCilFieldRVA.h				\
	  SgAsmCilFieldRVATable.h			\
	  SgAsmCilFieldTable.h				\
	  SgAsmCilFile.h				\
	  SgAsmCilFileTable.h				\
	  SgAsmCilGenericParamConstraint.h		\
	  SgAsmCilGenericParamConstraintTable.h		\
	  SgAsmCilGenericParam.h			\
	  SgAsmCilGenericParamTable.h			\
	  SgAsmCilImplMap.h				\
	  SgAsmCilImplMapTable.h			\
	  SgAsmCilInstruction.h				\
	  SgAsmCilInterfaceImpl.h			\
	  SgAsmCilInterfaceImplTable.h			\
	  SgAsmCilManifestResource.h			\
	  SgAsmCilManifestResourceTable.h		\
	  SgAsmCilMemberRef.h				\
	  SgAsmCilMemberRefTable.h			\
	  SgAsmCilMetadata.h				\
	  SgAsmCilMetadataHeap.h			\
	  SgAsmCilMetadataRoot.h			\
	  SgAsmCilMethodData.h				\
	  SgAsmCilMethodDef.h				\
	  SgAsmCilMethodDefTable.h			\
	  SgAsmCilMethodImpl.h				\
	  SgAsmCilMethodImplTable.h			\
	  SgAsmCilMethodSemantics.h			\
	  SgAsmCilMethodSemanticsTable.h		\
	  SgAsmCilMethodSpec.h				\
	  SgAsmCilMethodSpecTable.h			\
	  SgAsmCilModule.h				\
	  SgAsmCilModuleRef.h				\
	  SgAsmCilModuleRefTable.h			\
	  SgAsmCilModuleTable.h				\
	  SgAsmCilNestedClass.h				\
	  SgAsmCilNestedClassTable.h			\
	  SgAsmCilNode.h				\
	  SgAsmCilParam.h				\
	  SgAsmCilParamTable.h				\
	  SgAsmCilProperty.h				\
	  SgAsmCilPropertyMap.h				\
	  SgAsmCilPropertyMapTable.h			\
	  SgAsmCilPropertyTable.h			\
	  SgAsmCilStandAloneSig.h			\
	  SgAsmCilStandAloneSigTable.h			\
	  SgAsmCilTypeDef.h				\
	  SgAsmCilTypeDefTable.h			\
	  SgAsmCilTypeRef.h				\
	  SgAsmCilTypeRefTable.h			\
	  SgAsmCilTypeSpec.h				\
	  SgAsmCilTypeSpecTable.h			\
	  SgAsmCilUint32Heap.h				\
	  SgAsmCilUint8Heap.h				\
	  SgAsmCliHeader.h				\
	  SgAsmCoffStrtab.h				\
	  SgAsmCoffSymbol.h				\
	  SgAsmCoffSymbolList.h				\
	  SgAsmCoffSymbolTable.h			\
	  SgAsmCommonSubExpression.h			\
	  SgAsmConstantExpression.h			\
	  SgAsmControlFlagsExpression.h			\
	  SgAsmDirectRegisterExpression.h		\
	  SgAsmDOSExtendedHeader.h			\
	  SgAsmDOSFileHeader.h				\
	  SgAsmDwarfAccessDeclaration.h			\
	  SgAsmDwarfArrayType.h				\
	  SgAsmDwarfBaseType.h				\
	  SgAsmDwarfCatchBlock.h			\
	  SgAsmDwarfClassTemplate.h			\
	  SgAsmDwarfClassType.h				\
	  SgAsmDwarfCommonBlock.h			\
	  SgAsmDwarfCommonInclusion.h			\
	  SgAsmDwarfCompilationUnit.h			\
	  SgAsmDwarfCompilationUnitList.h		\
	  SgAsmDwarfCondition.h				\
	  SgAsmDwarfConstant.h				\
	  SgAsmDwarfConstruct.h				\
	  SgAsmDwarfConstructList.h			\
	  SgAsmDwarfConstType.h				\
	  SgAsmDwarfDwarfProcedure.h			\
	  SgAsmDwarfEntryPoint.h			\
	  SgAsmDwarfEnumerationType.h			\
	  SgAsmDwarfEnumerator.h			\
	  SgAsmDwarfFileType.h				\
	  SgAsmDwarfFormalParameter.h			\
	  SgAsmDwarfFormatLabel.h			\
	  SgAsmDwarfFriend.h				\
	  SgAsmDwarfFunctionTemplate.h			\
	  SgAsmDwarfImportedDeclaration.h		\
	  SgAsmDwarfImportedModule.h			\
	  SgAsmDwarfImportedUnit.h			\
	  SgAsmDwarfInformation.h			\
	  SgAsmDwarfInheritance.h			\
	  SgAsmDwarfInlinedSubroutine.h			\
	  SgAsmDwarfInterfaceType.h			\
	  SgAsmDwarfLabel.h				\
	  SgAsmDwarfLexicalBlock.h			\
	  SgAsmDwarfLine.h				\
	  SgAsmDwarfLineList.h				\
	  SgAsmDwarfMacro.h				\
	  SgAsmDwarfMacroList.h				\
	  SgAsmDwarfMember.h				\
	  SgAsmDwarfModule.h				\
	  SgAsmDwarfMutableType.h			\
	  SgAsmDwarfNamelist.h				\
	  SgAsmDwarfNamelistItem.h			\
	  SgAsmDwarfNamespace.h				\
	  SgAsmDwarfPackedType.h			\
	  SgAsmDwarfPartialUnit.h			\
	  SgAsmDwarfPointerType.h			\
	  SgAsmDwarfPtrToMemberType.h			\
	  SgAsmDwarfReferenceType.h			\
	  SgAsmDwarfRestrictType.h			\
	  SgAsmDwarfSetType.h				\
	  SgAsmDwarfSharedType.h			\
	  SgAsmDwarfStringType.h			\
	  SgAsmDwarfStructureType.h			\
	  SgAsmDwarfSubprogram.h			\
	  SgAsmDwarfSubrangeType.h			\
	  SgAsmDwarfSubroutineType.h			\
	  SgAsmDwarfTemplateTypeParameter.h		\
	  SgAsmDwarfTemplateValueParameter.h		\
	  SgAsmDwarfThrownType.h			\
	  SgAsmDwarfTryBlock.h				\
	  SgAsmDwarfTypedef.h				\
	  SgAsmDwarfUnionType.h				\
	  SgAsmDwarfUnknownConstruct.h			\
	  SgAsmDwarfUnspecifiedParameters.h		\
	  SgAsmDwarfUnspecifiedType.h			\
	  SgAsmDwarfUpcRelaxedType.h			\
	  SgAsmDwarfUpcSharedType.h			\
	  SgAsmDwarfUpcStrictType.h			\
	  SgAsmDwarfVariable.h				\
	  SgAsmDwarfVariant.h				\
	  SgAsmDwarfVariantPart.h			\
	  SgAsmDwarfVolatileType.h			\
	  SgAsmDwarfWithStmt.h				\
	  SgAsmElfDynamicEntry.h			\
	  SgAsmElfDynamicEntryList.h			\
	  SgAsmElfDynamicSection.h			\
	  SgAsmElfEHFrameEntryCI.h			\
	  SgAsmElfEHFrameEntryCIList.h			\
	  SgAsmElfEHFrameEntryFD.h			\
	  SgAsmElfEHFrameEntryFDList.h			\
	  SgAsmElfEHFrameSection.h			\
	  SgAsmElfFileHeader.h				\
	  SgAsmElfNoteEntry.h				\
	  SgAsmElfNoteEntryList.h			\
	  SgAsmElfNoteSection.h				\
	  SgAsmElfRelocEntry.h				\
	  SgAsmElfRelocEntryList.h			\
	  SgAsmElfRelocSection.h			\
	  SgAsmElfSection.h				\
	  SgAsmElfSectionTableEntry.h			\
	  SgAsmElfSectionTable.h			\
	  SgAsmElfSegmentTableEntry.h			\
	  SgAsmElfSegmentTableEntryList.h		\
	  SgAsmElfSegmentTable.h			\
	  SgAsmElfStringSection.h			\
	  SgAsmElfStrtab.h				\
	  SgAsmElfSymbol.h				\
	  SgAsmElfSymbolList.h				\
	  SgAsmElfSymbolSection.h			\
	  SgAsmElfSymverDefinedAux.h			\
	  SgAsmElfSymverDefinedAuxList.h		\
	  SgAsmElfSymverDefinedEntry.h			\
	  SgAsmElfSymverDefinedEntryList.h		\
	  SgAsmElfSymverDefinedSection.h		\
	  SgAsmElfSymverEntry.h				\
	  SgAsmElfSymverEntryList.h			\
	  SgAsmElfSymverNeededAux.h			\
	  SgAsmElfSymverNeededAuxList.h			\
	  SgAsmElfSymverNeededEntry.h			\
	  SgAsmElfSymverNeededEntryList.h		\
	  SgAsmElfSymverNeededSection.h			\
	  SgAsmElfSymverSection.h			\
	  SgAsmExecutableFileFormat.h			\
	  SgAsmExpression.h				\
	  SgAsmExprListExp.h				\
	  SgAsmFloatType.h				\
	  SgAsmFloatValueExpression.h			\
	  SgAsmFunction.h				\
	  SgAsmGenericDLL.h				\
	  SgAsmGenericDLLList.h				\
	  SgAsmGenericFile.h				\
	  SgAsmGenericFileList.h			\
	  SgAsmGenericFormat.h				\
	  SgAsmGenericHeader.h				\
	  SgAsmGenericHeaderList.h			\
	  SgAsmGenericSection.h				\
	  SgAsmGenericSectionList.h			\
	  SgAsmGenericString.h				\
	  SgAsmGenericStrtab.h				\
	  SgAsmGenericSymbol.h				\
	  SgAsmGenericSymbolList.h			\
	  SgAsmIndirectRegisterExpression.h		\
	  SgAsmInstruction.h				\
	  SgAsmInstructionList.h			\
	  SgAsmIntegerType.h				\
	  SgAsmIntegerValueExpression.h			\
	  SgAsmInterpretation.h				\
	  SgAsmInterpretationList.h			\
	  SgAsmJvmAttribute.h				\
	  SgAsmJvmAttributeTable.h			\
	  SgAsmJvmBootstrapMethod.h			\
	  SgAsmJvmBootstrapMethods.h			\
	  SgAsmJvmClass.h				\
	  SgAsmJvmCodeAttribute.h			\
	  SgAsmJvmConstantPoolEntry.h			\
	  SgAsmJvmConstantPool.h			\
	  SgAsmJvmConstantValue.h			\
	  SgAsmJvmDeprecated.h				\
	  SgAsmJvmEnclosingMethod.h			\
	  SgAsmJvmExceptionHandler.h			\
	  SgAsmJvmExceptions.h				\
	  SgAsmJvmExceptionTable.h			\
	  SgAsmJvmField.h				\
	  SgAsmJvmFieldTable.h				\
	  SgAsmJvmFileHeader.h				\
	  SgAsmJvmInnerClassesEntry.h			\
	  SgAsmJvmInnerClasses.h			\
	  SgAsmJvmInstruction.h				\
	  SgAsmJvmLineNumberEntry.h			\
	  SgAsmJvmLineNumberTable.h			\
	  SgAsmJvmLocalVariableEntry.h			\
	  SgAsmJvmLocalVariableTable.h			\
	  SgAsmJvmLocalVariableTypeEntry.h		\
	  SgAsmJvmLocalVariableTypeTable.h		\
	  SgAsmJvmMethod.h				\
	  SgAsmJvmMethodParametersEntry.h		\
	  SgAsmJvmMethodParameters.h			\
	  SgAsmJvmMethodTable.h				\
	  SgAsmJvmModuleMainClass.h			\
	  SgAsmJvmNestHost.h				\
	  SgAsmJvmNestMembers.h				\
	  SgAsmJvmNode.h				\
	  SgAsmJvmSignature.h				\
	  SgAsmJvmSourceFile.h				\
	  SgAsmJvmStackMapFrame.h			\
	  SgAsmJvmStackMapTable.h			\
	  SgAsmJvmStackMapVerificationType.h		\
	  SgAsmJvmSynthetic.h				\
	  SgAsmLEEntryPoint.h				\
	  SgAsmLEEntryTable.h				\
	  SgAsmLEFileHeader.h				\
	  SgAsmLENameTable.h				\
	  SgAsmLEPageTableEntry.h			\
	  SgAsmLEPageTable.h				\
	  SgAsmLERelocTable.h				\
	  SgAsmLESection.h				\
	  SgAsmLESectionTableEntry.h			\
	  SgAsmLESectionTable.h				\
	  SgAsmM68kInstruction.h			\
	  SgAsmMemoryReferenceExpression.h		\
	  SgAsmMipsInstruction.h			\
	  SgAsmNEEntryPoint.h				\
	  SgAsmNEEntryTable.h				\
	  SgAsmNEFileHeader.h				\
	  SgAsmNEModuleTable.h				\
	  SgAsmNENameTable.h				\
	  SgAsmNERelocEntry.h				\
	  SgAsmNERelocTable.h				\
	  SgAsmNESection.h				\
	  SgAsmNESectionTableEntry.h			\
	  SgAsmNESectionTable.h				\
	  SgAsmNEStringTable.h				\
	  SgAsmNode.h					\
	  SgAsmNullInstruction.h			\
	  SgAsmOperandList.h				\
	  SgAsmPEExportDirectory.h			\
	  SgAsmPEExportEntry.h				\
	  SgAsmPEExportEntryList.h			\
	  SgAsmPEExportSection.h			\
	  SgAsmPEFileHeader.h				\
	  SgAsmPEImportDirectory.h			\
	  SgAsmPEImportDirectoryList.h			\
	  SgAsmPEImportItem.h				\
	  SgAsmPEImportItemList.h			\
	  SgAsmPEImportSection.h			\
	  SgAsmPERVASizePair.h				\
	  SgAsmPERVASizePairList.h			\
	  SgAsmPESection.h				\
	  SgAsmPESectionTableEntry.h			\
	  SgAsmPESectionTable.h				\
	  SgAsmPEStringSection.h			\
	  SgAsmPointerType.h				\
	  SgAsmPowerpcInstruction.h			\
	  SgAsmRegisterNames.h				\
	  SgAsmRegisterReferenceExpression.h		\
	  SgAsmRiscOperation.h				\
	  SgAsmScalarType.h				\
	  SgAsmStackExpression.h			\
	  SgAsmStatement.h				\
	  SgAsmStaticData.h				\
	  SgAsmStoredString.h				\
	  SgAsmStringStorage.h				\
	  SgAsmSynthesizedDataStructureDeclaration.h	\
	  SgAsmSynthesizedDeclaration.h			\
	  SgAsmSynthesizedFieldDeclaration.h		\
	  SgAsmType.h					\
	  SgAsmUnaryExpression.h			\
	  SgAsmUnaryMinus.h				\
	  SgAsmUnaryPlus.h				\
	  SgAsmUnaryRrx.h				\
	  SgAsmUnarySignedExtend.h			\
	  SgAsmUnaryTruncate.h				\
	  SgAsmUnaryUnsignedExtend.h			\
	  SgAsmUserInstruction.h			\
	  SgAsmValueExpression.h			\
	  SgAsmVectorType.h				\
	  SgAsmVoidType.h				\
	  SgAsmX86Instruction.h

# Build the grammar for generating the C++ preprocessor.
rosetta_generated: $(CXX_PREPROC_CODES)
$(CXX_PREPROC_CODES): CxxGrammarMetaProgram$(EXEEXT) $(GRAMMAR_SOURCE_FILES)
#  Build a link so that the executable can find the *.code file in ../Grammar
#	rm -f Grammar
#	ln -s $(top_srcdir)/src/ROSETTA/Grammar Grammar
	@echo "Running ROSETTA to generated ROSE IR code:"
#       Add "--verbose" if you want the old, noisy output from CxxGrammarMetaProgram
	mkdir -p  ${top_builddir}/src/frontend/SageIII/Rose/Traits
	touch $(addprefix $(top_builddir)/src/frontend/SageIII/,$(rosetta_generated_definitions))
	$(AM_V_GEN) $(VALGRIND) ./CxxGrammarMetaProgram ${top_builddir}/src/frontend/SageIII/

noinst_PROGRAMS = CxxGrammarMetaProgram
CxxGrammarMetaProgram_SOURCES = \
   CxxGrammarMetaProgram.C \
   grammar.C \
   GrammarFile.C \
   grammarGenerator.C \
   grammarString.C \
   AstNodeClass.C \
   type.C \
   symbol.C \
   expression.C \
   statement.C \
   support.C \
   binaryInstruction.C \
   storageSize.C \
   visitorSupport.C \
   nodeIdSupport.C \
   poolBasedTraversal.C \
   node.C \
   buildAstFileIO.C \
   buildRoseTraits.C \
   buildStorageClasses.C \
   buildConstructorsWithoutSourcePositionInformation.C \
   outputClassesAndFields.C \
   java/terminals.cpp \
   StringUtility.C \
   jovialNodes.C

# buildConstructorsWithoutSourcePositionInformation.o: buildConstructorsWithoutSourcePositionInformation.C

BoostLibraries = \
  $(BOOST_LDFLAGS) $(BOOST_DATE_TIME_LIB)           \
  $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)  \
  $(BOOST_REGEX_LIB)  $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB)    \
  $(BOOST_WAVE_LIB) $(BOOST_IOSTREAMS_LIB) $(RT_LIBS) $(top_builddir)/src/util/libroseutil.la

CxxGrammarMetaProgram_LDADD = \
  $(ROSE_BOOST_LIBS) \
  $(top_builddir)/src/util/libroseutil.la \
  $(top_builddir)/src/Sawyer/libroseSawyer.la \
  $(ROSE_BOOST_LIBS) \
  $(RT_LIBS)

# DQ (4/12/2016): This is required for use with clang (not clear how to compile ROSE with out it).
if USING_CLANG_COMPILER
CxxGrammarMetaProgram_LDADD += -lstdc++
endif

# DQ (9/11/2008):
# The rose_paths.c is built in ROSE/src/util and this dependence causes
# ROSE to rebuild a lot of code each time configure is rerun. Let's see
# if this can be make a little more tolerable for those without larger
# computers working on ROSE (e.g. students).
# CxxGrammarMetaProgram_DEPENDENCIES = $(CxxGrammarMetaProgram_LDADD)
# CxxGrammarMetaProgram_DEPENDENCIES = 
# CxxGrammarMetaProgram_DEPENDENCIES = $(CxxGrammarMetaProgram_SOURCES)


# DQ (7/8/2005): Avoid building Sage III when util changes!
# $(top_builddir)/util/libroseutil.so:
#	cd $(top_builddir)/util; $(MAKE)


# Force recompilation of source files if any of the header files 
# are changed (does not work if *.o files are not present).
$(librosetta_a_OBJECTS): ROSETTA_macros.h grammar.h grammarString.h AstNodeClass.h

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files cxx_templates Cxx_Grammar* grammar.dot translationSupport.code

distclean-local:
	rm -rf Cxx_Grammar.h Cxx_GrammarDeclarations.h Cxx_GrammarDowncast.h Cxx_GrammarStorageClasses.h Cxx_GrammarVariants.h	\
	  Cxx_Grammar.C Cxx_GrammarTreeTraversal.C Cxx_GrammarTreeTraversalClass.h Cxx_GrammarVisitorSupport.h			\
	  Cxx_GrammarVisitorSupport.h Cxx_GrammarTreeTraversalClass.C								\
	  Cxx_GrammarSgNodeDefinitions.h											\
	  Cxx_GrammarSgAsmNodeDefinitions.h											\
	  Cxx_GrammarSgAsmCilNodeDefinitions.h											\
	  Cxx_GrammarSgAsmDwarfNodeDefinitions.h										\
	  Cxx_GrammarSgAsmJvmNodeDefinitions.h											\
	  grammar.tex grammar.dot
	rm -f $(addprefix $(top_builddir)/src/frontend/SageIII/,$(rosetta_generated_definitions))
