template <class FunctionalType>
void AstQueryNamespace::queryMemoryPool(AstQuery<ROSE_VisitTraversal,FunctionalType>& astQuery, VariantVector* variantsToTraverse)
  {
for (VariantVector::iterator it = variantsToTraverse->begin(); it != variantsToTraverse->end(); ++it)
  {
switch(*it){
 case V_SgName: {
  SgName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSymbolTable: {
  SgSymbolTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPragma: {
  SgPragma::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModifierNodes: {
  SgModifierNodes::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConstVolatileModifier: {
  SgConstVolatileModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStorageModifier: {
  SgStorageModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAccessModifier: {
  SgAccessModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionModifier: {
  SgFunctionModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUPC_AccessModifier: {
  SgUPC_AccessModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLinkageModifier: {
  SgLinkageModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSpecialFunctionModifier: {
  SgSpecialFunctionModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeModifier: {
  SgTypeModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElaboratedTypeModifier: {
  SgElaboratedTypeModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBaseClassModifier: {
  SgBaseClassModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeclarationModifier: {
  SgDeclarationModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOpenclAccessModeModifier: {
  SgOpenclAccessModeModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModifier: {
  SgModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_Sg_File_Info: {
  Sg_File_Info::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSourceFile: {
  SgSourceFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBinaryComposite: {
  SgBinaryComposite::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnknownFile: {
  SgUnknownFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFile: {
  SgFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFileList: {
  SgFileList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDirectory: {
  SgDirectory::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDirectoryList: {
  SgDirectoryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgProject: {
  SgProject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOptions: {
  SgOptions::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnparse_Info: {
  SgUnparse_Info::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFuncDecl_attr: {
  SgFuncDecl_attr::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassDecl_attr: {
  SgClassDecl_attr::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefSeq: {
  SgTypedefSeq::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionParameterTypeList: {
  SgFunctionParameterTypeList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateParameter: {
  SgTemplateParameter::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateArgument: {
  SgTemplateArgument::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateParameterList: {
  SgTemplateParameterList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateArgumentList: {
  SgTemplateArgumentList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitAttribute: {
  SgBitAttribute::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAttribute: {
  SgAttribute::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBaseClass: {
  SgBaseClass::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUndirectedGraphEdge: {
  SgUndirectedGraphEdge::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDirectedGraphEdge: {
  SgDirectedGraphEdge::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphNode: {
  SgGraphNode::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphEdge: {
  SgGraphEdge::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStringKeyedBidirectionalGraph: {
  SgStringKeyedBidirectionalGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntKeyedBidirectionalGraph: {
  SgIntKeyedBidirectionalGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBidirectionalGraph: {
  SgBidirectionalGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncidenceDirectedGraph: {
  SgIncidenceDirectedGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncidenceUndirectedGraph: {
  SgIncidenceUndirectedGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraph: {
  SgGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphNodeList: {
  SgGraphNodeList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphEdgeList: {
  SgGraphEdgeList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgQualifiedName: {
  SgQualifiedName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNameGroup: {
  SgNameGroup::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDimensionObject: {
  SgDimensionObject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDataStatementGroup: {
  SgDataStatementGroup::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDataStatementObject: {
  SgDataStatementObject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDataStatementValue: {
  SgDataStatementValue::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFormatItem: {
  SgFormatItem::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFormatItemList: {
  SgFormatItemList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeTable: {
  SgTypeTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSupport: {
  SgSupport::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnknown: {
  SgTypeUnknown::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeChar: {
  SgTypeChar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedChar: {
  SgTypeSignedChar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedChar: {
  SgTypeUnsignedChar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeShort: {
  SgTypeShort::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedShort: {
  SgTypeSignedShort::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedShort: {
  SgTypeUnsignedShort::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeInt: {
  SgTypeInt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedInt: {
  SgTypeSignedInt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedInt: {
  SgTypeUnsignedInt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLong: {
  SgTypeLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedLong: {
  SgTypeSignedLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedLong: {
  SgTypeUnsignedLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeVoid: {
  SgTypeVoid::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeGlobalVoid: {
  SgTypeGlobalVoid::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeWchar: {
  SgTypeWchar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeFloat: {
  SgTypeFloat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeDouble: {
  SgTypeDouble::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLongLong: {
  SgTypeLongLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedLongLong: {
  SgTypeSignedLongLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedLongLong: {
  SgTypeUnsignedLongLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLongDouble: {
  SgTypeLongDouble::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeString: {
  SgTypeString::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeBool: {
  SgTypeBool::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeComplex: {
  SgTypeComplex::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeImaginary: {
  SgTypeImaginary::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeDefault: {
  SgTypeDefault::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerMemberType: {
  SgPointerMemberType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgReferenceType: {
  SgReferenceType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeCAFTeam: {
  SgTypeCAFTeam::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLabel: {
  SgTypeLabel::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassType: {
  SgClassType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateType: {
  SgTemplateType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumType: {
  SgEnumType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefType: {
  SgTypedefType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModifierType: {
  SgModifierType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPartialFunctionModifierType: {
  SgPartialFunctionModifierType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArrayType: {
  SgArrayType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeEllipse: {
  SgTypeEllipse::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeCrayPointer: {
  SgTypeCrayPointer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPartialFunctionType: {
  SgPartialFunctionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionType: {
  SgMemberFunctionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionType: {
  SgFunctionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerType: {
  SgPointerType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamedType: {
  SgNamedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgQualifiedNameType: {
  SgQualifiedNameType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgType: {
  SgType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgForStatement: {
  SgForStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgForInitStatement: {
  SgForInitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCatchStatementSeq: {
  SgCatchStatementSeq::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionParameterList: {
  SgFunctionParameterList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCtorInitializerList: {
  SgCtorInitializerList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBasicBlock: {
  SgBasicBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGlobal: {
  SgGlobal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfStmt: {
  SgIfStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionDefinition: {
  SgFunctionDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWhileStmt: {
  SgWhileStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDoWhileStmt: {
  SgDoWhileStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSwitchStatement: {
  SgSwitchStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCatchOptionStmt: {
  SgCatchOptionStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariableDeclaration: {
  SgVariableDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariableDefinition: {
  SgVariableDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumDeclaration: {
  SgEnumDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmStmt: {
  SgAsmStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefDeclaration: {
  SgTypedefDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionTypeTable: {
  SgFunctionTypeTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExprStatement: {
  SgExprStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLabelStatement: {
  SgLabelStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCaseOptionStmt: {
  SgCaseOptionStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTryStmt: {
  SgTryStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDefaultOptionStmt: {
  SgDefaultOptionStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBreakStmt: {
  SgBreakStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgContinueStmt: {
  SgContinueStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgReturnStmt: {
  SgReturnStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGotoStatement: {
  SgGotoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSpawnStmt: {
  SgSpawnStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNullStatement: {
  SgNullStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariantStatement: {
  SgVariantStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPragmaDeclaration: {
  SgPragmaDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateDeclaration: {
  SgTemplateDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationDecl: {
  SgTemplateInstantiationDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationDefn: {
  SgTemplateInstantiationDefn::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationFunctionDecl: {
  SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationMemberFunctionDecl: {
  SgTemplateInstantiationMemberFunctionDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgProgramHeaderStatement: {
  SgProgramHeaderStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgProcedureHeaderStatement: {
  SgProcedureHeaderStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEntryStatement: {
  SgEntryStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFortranNonblockedDo: {
  SgFortranNonblockedDo::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInterfaceStatement: {
  SgInterfaceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgParameterStatement: {
  SgParameterStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommonBlock: {
  SgCommonBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModuleStatement: {
  SgModuleStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUseStatement: {
  SgUseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStopOrPauseStatement: {
  SgStopOrPauseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPrintStatement: {
  SgPrintStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgReadStatement: {
  SgReadStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWriteStatement: {
  SgWriteStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOpenStatement: {
  SgOpenStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCloseStatement: {
  SgCloseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInquireStatement: {
  SgInquireStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFlushStatement: {
  SgFlushStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBackspaceStatement: {
  SgBackspaceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRewindStatement: {
  SgRewindStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEndfileStatement: {
  SgEndfileStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWaitStatement: {
  SgWaitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCAFWithTeamStatement: {
  SgCAFWithTeamStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFormatStatement: {
  SgFormatStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFortranDo: {
  SgFortranDo::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgForAllStatement: {
  SgForAllStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIOStatement: {
  SgIOStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcNotifyStatement: {
  SgUpcNotifyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcWaitStatement: {
  SgUpcWaitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcBarrierStatement: {
  SgUpcBarrierStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcFenceStatement: {
  SgUpcFenceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcForAllStatement: {
  SgUpcForAllStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpParallelStatement: {
  SgOmpParallelStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSingleStatement: {
  SgOmpSingleStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpTaskStatement: {
  SgOmpTaskStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpForStatement: {
  SgOmpForStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpDoStatement: {
  SgOmpDoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSectionsStatement: {
  SgOmpSectionsStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpClauseBodyStatement: {
  SgOmpClauseBodyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpAtomicStatement: {
  SgOmpAtomicStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpMasterStatement: {
  SgOmpMasterStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSectionStatement: {
  SgOmpSectionStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpOrderedStatement: {
  SgOmpOrderedStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpWorkshareStatement: {
  SgOmpWorkshareStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCriticalStatement: {
  SgOmpCriticalStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpBodyStatement: {
  SgOmpBodyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaImportStatement: {
  SgJavaImportStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBlockDataStatement: {
  SgBlockDataStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImplicitStatement: {
  SgImplicitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStatementFunctionStatement: {
  SgStatementFunctionStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWhereStatement: {
  SgWhereStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNullifyStatement: {
  SgNullifyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEquivalenceStatement: {
  SgEquivalenceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDerivedTypeStatement: {
  SgDerivedTypeStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAttributeSpecificationStatement: {
  SgAttributeSpecificationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAllocateStatement: {
  SgAllocateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeallocateStatement: {
  SgDeallocateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgContainsStatement: {
  SgContainsStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSequenceStatement: {
  SgSequenceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElseWhereStatement: {
  SgElseWhereStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArithmeticIfStatement: {
  SgArithmeticIfStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignStatement: {
  SgAssignStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgComputedGotoStatement: {
  SgComputedGotoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignedGotoStatement: {
  SgAssignedGotoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamelistStatement: {
  SgNamelistStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImportStatement: {
  SgImportStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssociateStatement: {
  SgAssociateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFortranIncludeLine: {
  SgFortranIncludeLine::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceDeclarationStatement: {
  SgNamespaceDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceAliasDeclarationStatement: {
  SgNamespaceAliasDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceDefinitionStatement: {
  SgNamespaceDefinitionStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUsingDeclarationStatement: {
  SgUsingDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUsingDirectiveStatement: {
  SgUsingDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationDirectiveStatement: {
  SgTemplateInstantiationDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassDeclaration: {
  SgClassDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassDefinition: {
  SgClassDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgScopeStatement: {
  SgScopeStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionDeclaration: {
  SgMemberFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionDeclaration: {
  SgFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncludeDirectiveStatement: {
  SgIncludeDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDefineDirectiveStatement: {
  SgDefineDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUndefDirectiveStatement: {
  SgUndefDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfdefDirectiveStatement: {
  SgIfdefDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfndefDirectiveStatement: {
  SgIfndefDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfDirectiveStatement: {
  SgIfDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeadIfDirectiveStatement: {
  SgDeadIfDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElseDirectiveStatement: {
  SgElseDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElseifDirectiveStatement: {
  SgElseifDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEndifDirectiveStatement: {
  SgEndifDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLineDirectiveStatement: {
  SgLineDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWarningDirectiveStatement: {
  SgWarningDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgErrorDirectiveStatement: {
  SgErrorDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEmptyDirectiveStatement: {
  SgEmptyDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncludeNextDirectiveStatement: {
  SgIncludeNextDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIdentDirectiveStatement: {
  SgIdentDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLinemarkerDirectiveStatement: {
  SgLinemarkerDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgC_PreprocessorDirectiveStatement: {
  SgC_PreprocessorDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClinkageStartStatement: {
  SgClinkageStartStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClinkageEndStatement: {
  SgClinkageEndStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClinkageDeclarationStatement: {
  SgClinkageDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpFlushStatement: {
  SgOmpFlushStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpBarrierStatement: {
  SgOmpBarrierStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpTaskwaitStatement: {
  SgOmpTaskwaitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpThreadprivateStatement: {
  SgOmpThreadprivateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeclarationStatement: {
  SgDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStatement: {
  SgStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExprListExp: {
  SgExprListExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarRefExp: {
  SgVarRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLabelRefExp: {
  SgLabelRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassNameRefExp: {
  SgClassNameRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionRefExp: {
  SgFunctionRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionRefExp: {
  SgMemberFunctionRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionCallExp: {
  SgFunctionCallExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSizeOfOp: {
  SgSizeOfOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcLocalsizeofExpression: {
  SgUpcLocalsizeofExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcBlocksizeofExpression: {
  SgUpcBlocksizeofExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcElemsizeofExpression: {
  SgUpcElemsizeofExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgStartOp: {
  SgVarArgStartOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgStartOneOperandOp: {
  SgVarArgStartOneOperandOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgOp: {
  SgVarArgOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgEndOp: {
  SgVarArgEndOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgCopyOp: {
  SgVarArgCopyOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeIdOp: {
  SgTypeIdOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConditionalExp: {
  SgConditionalExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNewExp: {
  SgNewExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeleteExp: {
  SgDeleteExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgThisExp: {
  SgThisExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRefExp: {
  SgRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAggregateInitializer: {
  SgAggregateInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConstructorInitializer: {
  SgConstructorInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignInitializer: {
  SgAssignInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExpressionRoot: {
  SgExpressionRoot::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMinusOp: {
  SgMinusOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnaryAddOp: {
  SgUnaryAddOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNotOp: {
  SgNotOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerDerefExp: {
  SgPointerDerefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAddressOfOp: {
  SgAddressOfOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMinusMinusOp: {
  SgMinusMinusOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPlusPlusOp: {
  SgPlusPlusOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitComplementOp: {
  SgBitComplementOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRealPartOp: {
  SgRealPartOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImagPartOp: {
  SgImagPartOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConjugateOp: {
  SgConjugateOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCastExp: {
  SgCastExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgThrowOp: {
  SgThrowOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArrowExp: {
  SgArrowExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDotExp: {
  SgDotExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDotStarOp: {
  SgDotStarOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArrowStarOp: {
  SgArrowStarOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEqualityOp: {
  SgEqualityOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLessThanOp: {
  SgLessThanOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGreaterThanOp: {
  SgGreaterThanOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNotEqualOp: {
  SgNotEqualOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLessOrEqualOp: {
  SgLessOrEqualOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGreaterOrEqualOp: {
  SgGreaterOrEqualOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAddOp: {
  SgAddOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSubtractOp: {
  SgSubtractOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMultiplyOp: {
  SgMultiplyOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDivideOp: {
  SgDivideOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntegerDivideOp: {
  SgIntegerDivideOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModOp: {
  SgModOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAndOp: {
  SgAndOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOrOp: {
  SgOrOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitXorOp: {
  SgBitXorOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitAndOp: {
  SgBitAndOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitOrOp: {
  SgBitOrOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommaOpExp: {
  SgCommaOpExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLshiftOp: {
  SgLshiftOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRshiftOp: {
  SgRshiftOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPntrArrRefExp: {
  SgPntrArrRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgScopeOp: {
  SgScopeOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignOp: {
  SgAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPlusAssignOp: {
  SgPlusAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMinusAssignOp: {
  SgMinusAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAndAssignOp: {
  SgAndAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIorAssignOp: {
  SgIorAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMultAssignOp: {
  SgMultAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDivAssignOp: {
  SgDivAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModAssignOp: {
  SgModAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgXorAssignOp: {
  SgXorAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLshiftAssignOp: {
  SgLshiftAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRshiftAssignOp: {
  SgRshiftAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConcatenationOp: {
  SgConcatenationOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBoolValExp: {
  SgBoolValExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStringVal: {
  SgStringVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgShortVal: {
  SgShortVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCharVal: {
  SgCharVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedCharVal: {
  SgUnsignedCharVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWcharVal: {
  SgWcharVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedShortVal: {
  SgUnsignedShortVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntVal: {
  SgIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumVal: {
  SgEnumVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedIntVal: {
  SgUnsignedIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLongIntVal: {
  SgLongIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLongLongIntVal: {
  SgLongLongIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedLongLongIntVal: {
  SgUnsignedLongLongIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedLongVal: {
  SgUnsignedLongVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFloatVal: {
  SgFloatVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDoubleVal: {
  SgDoubleVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLongDoubleVal: {
  SgLongDoubleVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcThreads: {
  SgUpcThreads::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcMythread: {
  SgUpcMythread::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgComplexVal: {
  SgComplexVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNullExpression: {
  SgNullExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariantExpression: {
  SgVariantExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStatementExpression: {
  SgStatementExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmOp: {
  SgAsmOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCudaKernelExecConfig: {
  SgCudaKernelExecConfig::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCudaKernelCallExp: {
  SgCudaKernelCallExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSubscriptExpression: {
  SgSubscriptExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgColonShapeExp: {
  SgColonShapeExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsteriskShapeExp: {
  SgAsteriskShapeExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIOItemExpression: {
  SgIOItemExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImpliedDo: {
  SgImpliedDo::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExponentiationOp: {
  SgExponentiationOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnknownArrayOrFunctionReference: {
  SgUnknownArrayOrFunctionReference::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgActualArgumentExpression: {
  SgActualArgumentExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUserDefinedBinaryOp: {
  SgUserDefinedBinaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerAssignOp: {
  SgPointerAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCAFCoExpression: {
  SgCAFCoExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDesignatedInitializer: {
  SgDesignatedInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInitializer: {
  SgInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUserDefinedUnaryOp: {
  SgUserDefinedUnaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPseudoDestructorRefExp: {
  SgPseudoDestructorRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnaryOp: {
  SgUnaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBinaryOp: {
  SgBinaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgValueExp: {
  SgValueExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExpression: {
  SgExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariableSymbol: {
  SgVariableSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionTypeSymbol: {
  SgFunctionTypeSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassSymbol: {
  SgClassSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateSymbol: {
  SgTemplateSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumSymbol: {
  SgEnumSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumFieldSymbol: {
  SgEnumFieldSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefSymbol: {
  SgTypedefSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionSymbol: {
  SgMemberFunctionSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLabelSymbol: {
  SgLabelSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDefaultSymbol: {
  SgDefaultSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceSymbol: {
  SgNamespaceSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntrinsicSymbol: {
  SgIntrinsicSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModuleSymbol: {
  SgModuleSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInterfaceSymbol: {
  SgInterfaceSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommonSymbol: {
  SgCommonSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRenameSymbol: {
  SgRenameSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionSymbol: {
  SgFunctionSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryAddressSymbol: {
  SgAsmBinaryAddressSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryDataSymbol: {
  SgAsmBinaryDataSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAliasSymbol: {
  SgAliasSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSymbol: {
  SgSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBlock: {
  SgAsmBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmOperandList: {
  SgAsmOperandList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmArmInstruction: {
  SgAsmArmInstruction::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmx86Instruction: {
  SgAsmx86Instruction::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPowerpcInstruction: {
  SgAsmPowerpcInstruction::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmInstruction: {
  SgAsmInstruction::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDataStructureDeclaration: {
  SgAsmDataStructureDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmFunctionDeclaration: {
  SgAsmFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmFieldDeclaration: {
  SgAsmFieldDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDeclaration: {
  SgAsmDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmStatement: {
  SgAsmStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryAdd: {
  SgAsmBinaryAdd::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinarySubtract: {
  SgAsmBinarySubtract::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryMultiply: {
  SgAsmBinaryMultiply::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryDivide: {
  SgAsmBinaryDivide::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryMod: {
  SgAsmBinaryMod::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryAddPreupdate: {
  SgAsmBinaryAddPreupdate::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinarySubtractPreupdate: {
  SgAsmBinarySubtractPreupdate::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryAddPostupdate: {
  SgAsmBinaryAddPostupdate::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinarySubtractPostupdate: {
  SgAsmBinarySubtractPostupdate::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryLsl: {
  SgAsmBinaryLsl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryLsr: {
  SgAsmBinaryLsr::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryAsr: {
  SgAsmBinaryAsr::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryRor: {
  SgAsmBinaryRor::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBinaryExpression: {
  SgAsmBinaryExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmUnaryPlus: {
  SgAsmUnaryPlus::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmUnaryMinus: {
  SgAsmUnaryMinus::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmUnaryRrx: {
  SgAsmUnaryRrx::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmUnaryArmSpecialRegisterList: {
  SgAsmUnaryArmSpecialRegisterList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmUnaryExpression: {
  SgAsmUnaryExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmMemoryReferenceExpression: {
  SgAsmMemoryReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmControlFlagsExpression: {
  SgAsmControlFlagsExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmCommonSubExpression: {
  SgAsmCommonSubExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmx86RegisterReferenceExpression: {
  SgAsmx86RegisterReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmArmRegisterReferenceExpression: {
  SgAsmArmRegisterReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPowerpcRegisterReferenceExpression: {
  SgAsmPowerpcRegisterReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmRegisterReferenceExpression: {
  SgAsmRegisterReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmByteValueExpression: {
  SgAsmByteValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmWordValueExpression: {
  SgAsmWordValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDoubleWordValueExpression: {
  SgAsmDoubleWordValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmQuadWordValueExpression: {
  SgAsmQuadWordValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmSingleFloatValueExpression: {
  SgAsmSingleFloatValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDoubleFloatValueExpression: {
  SgAsmDoubleFloatValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmVectorValueExpression: {
  SgAsmVectorValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmValueExpression: {
  SgAsmValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmExprListExp: {
  SgAsmExprListExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmExpression: {
  SgAsmExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeByte: {
  SgAsmTypeByte::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeWord: {
  SgAsmTypeWord::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeDoubleWord: {
  SgAsmTypeDoubleWord::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeQuadWord: {
  SgAsmTypeQuadWord::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeDoubleQuadWord: {
  SgAsmTypeDoubleQuadWord::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeSingleFloat: {
  SgAsmTypeSingleFloat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeDoubleFloat: {
  SgAsmTypeDoubleFloat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmType80bitFloat: {
  SgAsmType80bitFloat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmType128bitFloat: {
  SgAsmType128bitFloat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmTypeVector: {
  SgAsmTypeVector::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmType: {
  SgAsmType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericDLL: {
  SgAsmGenericDLL::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportHNTEntryList: {
  SgAsmPEImportHNTEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportILTEntryList: {
  SgAsmPEImportILTEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportDirectoryList: {
  SgAsmPEImportDirectoryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericFormat: {
  SgAsmGenericFormat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericFile: {
  SgAsmGenericFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfFileHeader: {
  SgAsmElfFileHeader::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEFileHeader: {
  SgAsmPEFileHeader::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNEFileHeader: {
  SgAsmNEFileHeader::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLEFileHeader: {
  SgAsmLEFileHeader::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDOSFileHeader: {
  SgAsmDOSFileHeader::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericHeader: {
  SgAsmGenericHeader::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfRelocSection: {
  SgAsmElfRelocSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfDynamicSection: {
  SgAsmElfDynamicSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymbolSection: {
  SgAsmElfSymbolSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfStringSection: {
  SgAsmElfStringSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfEHFrameSection: {
  SgAsmElfEHFrameSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfNoteSection: {
  SgAsmElfNoteSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverSection: {
  SgAsmElfSymverSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverDefinedSection: {
  SgAsmElfSymverDefinedSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverNeededSection: {
  SgAsmElfSymverNeededSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfStrtab: {
  SgAsmElfStrtab::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmCoffStrtab: {
  SgAsmCoffStrtab::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericStrtab: {
  SgAsmGenericStrtab::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSection: {
  SgAsmElfSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSectionTable: {
  SgAsmElfSectionTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSegmentTable: {
  SgAsmElfSegmentTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportSection: {
  SgAsmPEImportSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEExportSection: {
  SgAsmPEExportSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEStringSection: {
  SgAsmPEStringSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPESection: {
  SgAsmPESection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPESectionTable: {
  SgAsmPESectionTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmCoffSymbolTable: {
  SgAsmCoffSymbolTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDOSExtendedHeader: {
  SgAsmDOSExtendedHeader::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNESection: {
  SgAsmNESection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNESectionTable: {
  SgAsmNESectionTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNENameTable: {
  SgAsmNENameTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNEModuleTable: {
  SgAsmNEModuleTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNEStringTable: {
  SgAsmNEStringTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNEEntryTable: {
  SgAsmNEEntryTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNERelocTable: {
  SgAsmNERelocTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLESection: {
  SgAsmLESection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLESectionTable: {
  SgAsmLESectionTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLENameTable: {
  SgAsmLENameTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLEPageTable: {
  SgAsmLEPageTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLEEntryTable: {
  SgAsmLEEntryTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLERelocTable: {
  SgAsmLERelocTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericSection: {
  SgAsmGenericSection::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmCoffSymbol: {
  SgAsmCoffSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymbol: {
  SgAsmElfSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericSymbol: {
  SgAsmGenericSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSectionTableEntry: {
  SgAsmElfSectionTableEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSegmentTableEntry: {
  SgAsmElfSegmentTableEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSegmentTableEntryList: {
  SgAsmElfSegmentTableEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfRelocEntry: {
  SgAsmElfRelocEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfRelocEntryList: {
  SgAsmElfRelocEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfDynamicEntry: {
  SgAsmElfDynamicEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfDynamicEntryList: {
  SgAsmElfDynamicEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfEHFrameEntryCI: {
  SgAsmElfEHFrameEntryCI::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfEHFrameEntryCIList: {
  SgAsmElfEHFrameEntryCIList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfEHFrameEntryFD: {
  SgAsmElfEHFrameEntryFD::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfEHFrameEntryFDList: {
  SgAsmElfEHFrameEntryFDList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfNoteEntry: {
  SgAsmElfNoteEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfNoteEntryList: {
  SgAsmElfNoteEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverEntry: {
  SgAsmElfSymverEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverEntryList: {
  SgAsmElfSymverEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverDefinedEntry: {
  SgAsmElfSymverDefinedEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverDefinedEntryList: {
  SgAsmElfSymverDefinedEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverDefinedAux: {
  SgAsmElfSymverDefinedAux::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverDefinedAuxList: {
  SgAsmElfSymverDefinedAuxList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverNeededEntry: {
  SgAsmElfSymverNeededEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverNeededEntryList: {
  SgAsmElfSymverNeededEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverNeededAux: {
  SgAsmElfSymverNeededAux::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymverNeededAuxList: {
  SgAsmElfSymverNeededAuxList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPERVASizePair: {
  SgAsmPERVASizePair::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEExportDirectory: {
  SgAsmPEExportDirectory::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEExportEntry: {
  SgAsmPEExportEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportDirectory: {
  SgAsmPEImportDirectory::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportILTEntry: {
  SgAsmPEImportILTEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportHNTEntry: {
  SgAsmPEImportHNTEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEImportLookupTable: {
  SgAsmPEImportLookupTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPESectionTableEntry: {
  SgAsmPESectionTableEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNEEntryPoint: {
  SgAsmNEEntryPoint::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNERelocEntry: {
  SgAsmNERelocEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNESectionTableEntry: {
  SgAsmNESectionTableEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLEPageTableEntry: {
  SgAsmLEPageTableEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLEEntryPoint: {
  SgAsmLEEntryPoint::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmLESectionTableEntry: {
  SgAsmLESectionTableEntry::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericSectionList: {
  SgAsmGenericSectionList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericHeaderList: {
  SgAsmGenericHeaderList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericSymbolList: {
  SgAsmGenericSymbolList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmElfSymbolList: {
  SgAsmElfSymbolList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmCoffSymbolList: {
  SgAsmCoffSymbolList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericDLLList: {
  SgAsmGenericDLLList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPERVASizePairList: {
  SgAsmPERVASizePairList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmPEExportEntryList: {
  SgAsmPEExportEntryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmBasicString: {
  SgAsmBasicString::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmStoredString: {
  SgAsmStoredString::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericString: {
  SgAsmGenericString::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmStringStorage: {
  SgAsmStringStorage::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfMacro: {
  SgAsmDwarfMacro::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfLine: {
  SgAsmDwarfLine::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfMacroList: {
  SgAsmDwarfMacroList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfLineList: {
  SgAsmDwarfLineList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfArrayType: {
  SgAsmDwarfArrayType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfClassType: {
  SgAsmDwarfClassType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfEntryPoint: {
  SgAsmDwarfEntryPoint::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfEnumerationType: {
  SgAsmDwarfEnumerationType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfFormalParameter: {
  SgAsmDwarfFormalParameter::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfImportedDeclaration: {
  SgAsmDwarfImportedDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfLabel: {
  SgAsmDwarfLabel::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfLexicalBlock: {
  SgAsmDwarfLexicalBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfMember: {
  SgAsmDwarfMember::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfPointerType: {
  SgAsmDwarfPointerType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfReferenceType: {
  SgAsmDwarfReferenceType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfCompilationUnit: {
  SgAsmDwarfCompilationUnit::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfStringType: {
  SgAsmDwarfStringType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfStructureType: {
  SgAsmDwarfStructureType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfSubroutineType: {
  SgAsmDwarfSubroutineType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfTypedef: {
  SgAsmDwarfTypedef::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfUnionType: {
  SgAsmDwarfUnionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfUnspecifiedParameters: {
  SgAsmDwarfUnspecifiedParameters::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfVariant: {
  SgAsmDwarfVariant::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfCommonBlock: {
  SgAsmDwarfCommonBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfCommonInclusion: {
  SgAsmDwarfCommonInclusion::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfInheritance: {
  SgAsmDwarfInheritance::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfInlinedSubroutine: {
  SgAsmDwarfInlinedSubroutine::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfModule: {
  SgAsmDwarfModule::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfPtrToMemberType: {
  SgAsmDwarfPtrToMemberType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfSetType: {
  SgAsmDwarfSetType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfSubrangeType: {
  SgAsmDwarfSubrangeType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfWithStmt: {
  SgAsmDwarfWithStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfAccessDeclaration: {
  SgAsmDwarfAccessDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfBaseType: {
  SgAsmDwarfBaseType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfCatchBlock: {
  SgAsmDwarfCatchBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfConstType: {
  SgAsmDwarfConstType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfConstant: {
  SgAsmDwarfConstant::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfEnumerator: {
  SgAsmDwarfEnumerator::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfFileType: {
  SgAsmDwarfFileType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfFriend: {
  SgAsmDwarfFriend::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfNamelist: {
  SgAsmDwarfNamelist::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfNamelistItem: {
  SgAsmDwarfNamelistItem::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfPackedType: {
  SgAsmDwarfPackedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfSubprogram: {
  SgAsmDwarfSubprogram::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfTemplateTypeParameter: {
  SgAsmDwarfTemplateTypeParameter::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfTemplateValueParameter: {
  SgAsmDwarfTemplateValueParameter::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfThrownType: {
  SgAsmDwarfThrownType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfTryBlock: {
  SgAsmDwarfTryBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfVariantPart: {
  SgAsmDwarfVariantPart::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfVariable: {
  SgAsmDwarfVariable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfVolatileType: {
  SgAsmDwarfVolatileType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfDwarfProcedure: {
  SgAsmDwarfDwarfProcedure::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfRestrictType: {
  SgAsmDwarfRestrictType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfInterfaceType: {
  SgAsmDwarfInterfaceType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfNamespace: {
  SgAsmDwarfNamespace::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfImportedModule: {
  SgAsmDwarfImportedModule::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfUnspecifiedType: {
  SgAsmDwarfUnspecifiedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfPartialUnit: {
  SgAsmDwarfPartialUnit::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfImportedUnit: {
  SgAsmDwarfImportedUnit::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfMutableType: {
  SgAsmDwarfMutableType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfCondition: {
  SgAsmDwarfCondition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfSharedType: {
  SgAsmDwarfSharedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfFormatLabel: {
  SgAsmDwarfFormatLabel::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfFunctionTemplate: {
  SgAsmDwarfFunctionTemplate::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfClassTemplate: {
  SgAsmDwarfClassTemplate::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfUpcSharedType: {
  SgAsmDwarfUpcSharedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfUpcStrictType: {
  SgAsmDwarfUpcStrictType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfUpcRelaxedType: {
  SgAsmDwarfUpcRelaxedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfUnknownConstruct: {
  SgAsmDwarfUnknownConstruct::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfConstruct: {
  SgAsmDwarfConstruct::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfConstructList: {
  SgAsmDwarfConstructList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfCompilationUnitList: {
  SgAsmDwarfCompilationUnitList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmDwarfInformation: {
  SgAsmDwarfInformation::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmExecutableFileFormat: {
  SgAsmExecutableFileFormat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmInterpretation: {
  SgAsmInterpretation::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmInterpretationList: {
  SgAsmInterpretationList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmGenericFileList: {
  SgAsmGenericFileList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmNode: {
  SgAsmNode::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommonBlockObject: {
  SgCommonBlockObject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInitializedName: {
  SgInitializedName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpOrderedClause: {
  SgOmpOrderedClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpNowaitClause: {
  SgOmpNowaitClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpUntiedClause: {
  SgOmpUntiedClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpDefaultClause: {
  SgOmpDefaultClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCollapseClause: {
  SgOmpCollapseClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpIfClause: {
  SgOmpIfClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpNumThreadsClause: {
  SgOmpNumThreadsClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpExpressionClause: {
  SgOmpExpressionClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCopyprivateClause: {
  SgOmpCopyprivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpPrivateClause: {
  SgOmpPrivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpFirstprivateClause: {
  SgOmpFirstprivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSharedClause: {
  SgOmpSharedClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCopyinClause: {
  SgOmpCopyinClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpLastprivateClause: {
  SgOmpLastprivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpReductionClause: {
  SgOmpReductionClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpVariablesClause: {
  SgOmpVariablesClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpScheduleClause: {
  SgOmpScheduleClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpClause: {
  SgOmpClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRenamePair: {
  SgRenamePair::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInterfaceBody: {
  SgInterfaceBody::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLocatedNodeSupport: {
  SgLocatedNodeSupport::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgToken: {
  SgToken::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLocatedNode: {
  SgLocatedNode::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNode: {
  SgNode::traverseMemoryPoolNodes(astQuery);
  break;
}
default:
{
  // This is a common error after adding a new IR node (because this function should have been automatically generated).
  std::cout << "Case not implemented in queryMemoryPool(..). Exiting." << std::endl;
  ROSE_ASSERT(false);
  break;
}
}
}

};
