// MACHINE GENERATED SOURCE FILE WITH ROSE (Grammar.h)--- DO NOT MODIFY!

#include "sage3basic.h"

#include "AST_FILE_IO.h"

// The header file ("rose_config.h") should only be included by source files that require it.
#include "rose_config.h"

#if _MSC_VER
#define USE_CPP_NEW_DELETE_OPERATORS 0
#endif


using namespace std;
void SgNode::getClassHierarchySubTreeFunction( VariantT v, std::vector<VariantT>& subTreeVariants){
switch(v){
 case V_SgName:
{
break;
}
case V_SgSymbolTable:
{
break;
}
case V_SgPragma:
{
break;
}
case V_SgModifierNodes:
{
break;
}
case V_SgConstVolatileModifier:
{
break;
}
case V_SgStorageModifier:
{
break;
}
case V_SgAccessModifier:
{
break;
}
case V_SgFunctionModifier:
{
break;
}
case V_SgUPC_AccessModifier:
{
break;
}
case V_SgLinkageModifier:
{
break;
}
case V_SgSpecialFunctionModifier:
{
break;
}
case V_SgTypeModifier:
{
break;
}
case V_SgElaboratedTypeModifier:
{
break;
}
case V_SgBaseClassModifier:
{
break;
}
case V_SgDeclarationModifier:
{
break;
}
case V_SgOpenclAccessModeModifier:
{
break;
}
case V_SgModifier:
{
subTreeVariants.push_back(V_SgModifierNodes);
subTreeVariants.push_back(V_SgConstVolatileModifier);
subTreeVariants.push_back(V_SgStorageModifier);
subTreeVariants.push_back(V_SgAccessModifier);
subTreeVariants.push_back(V_SgFunctionModifier);
subTreeVariants.push_back(V_SgUPC_AccessModifier);
subTreeVariants.push_back(V_SgSpecialFunctionModifier);
subTreeVariants.push_back(V_SgElaboratedTypeModifier);
subTreeVariants.push_back(V_SgLinkageModifier);
subTreeVariants.push_back(V_SgBaseClassModifier);
subTreeVariants.push_back(V_SgTypeModifier);
subTreeVariants.push_back(V_SgDeclarationModifier);
subTreeVariants.push_back(V_SgOpenclAccessModeModifier);
break;
}
case V_Sg_File_Info:
{
break;
}
case V_SgSourceFile:
{
break;
}
case V_SgBinaryComposite:
{
break;
}
case V_SgUnknownFile:
{
break;
}
case V_SgFile:
{
subTreeVariants.push_back(V_SgSourceFile);
subTreeVariants.push_back(V_SgBinaryComposite);
subTreeVariants.push_back(V_SgUnknownFile);
break;
}
case V_SgFileList:
{
break;
}
case V_SgDirectory:
{
break;
}
case V_SgDirectoryList:
{
break;
}
case V_SgProject:
{
break;
}
case V_SgOptions:
{
break;
}
case V_SgUnparse_Info:
{
break;
}
case V_SgFuncDecl_attr:
{
break;
}
case V_SgClassDecl_attr:
{
break;
}
case V_SgTypedefSeq:
{
break;
}
case V_SgFunctionParameterTypeList:
{
break;
}
case V_SgTemplateParameter:
{
break;
}
case V_SgTemplateArgument:
{
break;
}
case V_SgTemplateParameterList:
{
break;
}
case V_SgTemplateArgumentList:
{
break;
}
case V_SgBitAttribute:
{
subTreeVariants.push_back(V_SgFuncDecl_attr);
subTreeVariants.push_back(V_SgClassDecl_attr);
break;
}
case V_SgAttribute:
{
subTreeVariants.push_back(V_SgPragma);
subTreeVariants.push_back(V_SgBitAttribute);
break;
}
case V_SgBaseClass:
{
break;
}
case V_SgUndirectedGraphEdge:
{
break;
}
case V_SgDirectedGraphEdge:
{
break;
}
case V_SgGraphNode:
{
break;
}
case V_SgGraphEdge:
{
subTreeVariants.push_back(V_SgDirectedGraphEdge);
subTreeVariants.push_back(V_SgUndirectedGraphEdge);
break;
}
case V_SgStringKeyedBidirectionalGraph:
{
break;
}
case V_SgIntKeyedBidirectionalGraph:
{
break;
}
case V_SgBidirectionalGraph:
{
subTreeVariants.push_back(V_SgStringKeyedBidirectionalGraph);
subTreeVariants.push_back(V_SgIntKeyedBidirectionalGraph);
break;
}
case V_SgIncidenceDirectedGraph:
{
subTreeVariants.push_back(V_SgBidirectionalGraph);
break;
}
case V_SgIncidenceUndirectedGraph:
{
break;
}
case V_SgGraph:
{
subTreeVariants.push_back(V_SgIncidenceDirectedGraph);
subTreeVariants.push_back(V_SgIncidenceUndirectedGraph);
break;
}
case V_SgGraphNodeList:
{
break;
}
case V_SgGraphEdgeList:
{
break;
}
case V_SgQualifiedName:
{
break;
}
case V_SgNameGroup:
{
break;
}
case V_SgDimensionObject:
{
break;
}
case V_SgDataStatementGroup:
{
break;
}
case V_SgDataStatementObject:
{
break;
}
case V_SgDataStatementValue:
{
break;
}
case V_SgFormatItem:
{
break;
}
case V_SgFormatItemList:
{
break;
}
case V_SgTypeTable:
{
break;
}
case V_SgSupport:
{
subTreeVariants.push_back(V_SgModifier);
subTreeVariants.push_back(V_SgName);
subTreeVariants.push_back(V_SgSymbolTable);
subTreeVariants.push_back(V_SgAttribute);
subTreeVariants.push_back(V_Sg_File_Info);
subTreeVariants.push_back(V_SgFile);
subTreeVariants.push_back(V_SgProject);
subTreeVariants.push_back(V_SgOptions);
subTreeVariants.push_back(V_SgUnparse_Info);
subTreeVariants.push_back(V_SgBaseClass);
subTreeVariants.push_back(V_SgTypedefSeq);
subTreeVariants.push_back(V_SgTemplateParameter);
subTreeVariants.push_back(V_SgTemplateArgument);
subTreeVariants.push_back(V_SgDirectory);
subTreeVariants.push_back(V_SgFileList);
subTreeVariants.push_back(V_SgDirectoryList);
subTreeVariants.push_back(V_SgFunctionParameterTypeList);
subTreeVariants.push_back(V_SgQualifiedName);
subTreeVariants.push_back(V_SgTemplateArgumentList);
subTreeVariants.push_back(V_SgTemplateParameterList);
subTreeVariants.push_back(V_SgGraph);
subTreeVariants.push_back(V_SgGraphNode);
subTreeVariants.push_back(V_SgGraphEdge);
subTreeVariants.push_back(V_SgGraphNodeList);
subTreeVariants.push_back(V_SgGraphEdgeList);
subTreeVariants.push_back(V_SgTypeTable);
subTreeVariants.push_back(V_SgNameGroup);
subTreeVariants.push_back(V_SgDimensionObject);
subTreeVariants.push_back(V_SgFormatItem);
subTreeVariants.push_back(V_SgFormatItemList);
subTreeVariants.push_back(V_SgDataStatementGroup);
subTreeVariants.push_back(V_SgDataStatementObject);
subTreeVariants.push_back(V_SgDataStatementValue);
break;
}
case V_SgTypeUnknown:
{
break;
}
case V_SgTypeChar:
{
break;
}
case V_SgTypeSignedChar:
{
break;
}
case V_SgTypeUnsignedChar:
{
break;
}
case V_SgTypeShort:
{
break;
}
case V_SgTypeSignedShort:
{
break;
}
case V_SgTypeUnsignedShort:
{
break;
}
case V_SgTypeInt:
{
break;
}
case V_SgTypeSignedInt:
{
break;
}
case V_SgTypeUnsignedInt:
{
break;
}
case V_SgTypeLong:
{
break;
}
case V_SgTypeSignedLong:
{
break;
}
case V_SgTypeUnsignedLong:
{
break;
}
case V_SgTypeVoid:
{
break;
}
case V_SgTypeGlobalVoid:
{
break;
}
case V_SgTypeWchar:
{
break;
}
case V_SgTypeFloat:
{
break;
}
case V_SgTypeDouble:
{
break;
}
case V_SgTypeLongLong:
{
break;
}
case V_SgTypeSignedLongLong:
{
break;
}
case V_SgTypeUnsignedLongLong:
{
break;
}
case V_SgTypeLongDouble:
{
break;
}
case V_SgTypeString:
{
break;
}
case V_SgTypeBool:
{
break;
}
case V_SgTypeComplex:
{
break;
}
case V_SgTypeImaginary:
{
break;
}
case V_SgTypeDefault:
{
break;
}
case V_SgPointerMemberType:
{
break;
}
case V_SgReferenceType:
{
break;
}
case V_SgTypeCAFTeam:
{
break;
}
case V_SgTypeLabel:
{
break;
}
case V_SgClassType:
{
break;
}
case V_SgTemplateType:
{
break;
}
case V_SgEnumType:
{
break;
}
case V_SgTypedefType:
{
break;
}
case V_SgModifierType:
{
break;
}
case V_SgPartialFunctionModifierType:
{
break;
}
case V_SgArrayType:
{
break;
}
case V_SgTypeEllipse:
{
break;
}
case V_SgTypeCrayPointer:
{
break;
}
case V_SgPartialFunctionType:
{
subTreeVariants.push_back(V_SgPartialFunctionModifierType);
break;
}
case V_SgMemberFunctionType:
{
subTreeVariants.push_back(V_SgPartialFunctionType);
break;
}
case V_SgFunctionType:
{
subTreeVariants.push_back(V_SgMemberFunctionType);
break;
}
case V_SgPointerType:
{
subTreeVariants.push_back(V_SgPointerMemberType);
break;
}
case V_SgNamedType:
{
subTreeVariants.push_back(V_SgClassType);
subTreeVariants.push_back(V_SgEnumType);
subTreeVariants.push_back(V_SgTypedefType);
break;
}
case V_SgQualifiedNameType:
{
break;
}
case V_SgType:
{
subTreeVariants.push_back(V_SgTypeUnknown);
subTreeVariants.push_back(V_SgTypeChar);
subTreeVariants.push_back(V_SgTypeSignedChar);
subTreeVariants.push_back(V_SgTypeUnsignedChar);
subTreeVariants.push_back(V_SgTypeShort);
subTreeVariants.push_back(V_SgTypeSignedShort);
subTreeVariants.push_back(V_SgTypeUnsignedShort);
subTreeVariants.push_back(V_SgTypeInt);
subTreeVariants.push_back(V_SgTypeSignedInt);
subTreeVariants.push_back(V_SgTypeUnsignedInt);
subTreeVariants.push_back(V_SgTypeLong);
subTreeVariants.push_back(V_SgTypeSignedLong);
subTreeVariants.push_back(V_SgTypeUnsignedLong);
subTreeVariants.push_back(V_SgTypeVoid);
subTreeVariants.push_back(V_SgTypeGlobalVoid);
subTreeVariants.push_back(V_SgTypeWchar);
subTreeVariants.push_back(V_SgTypeFloat);
subTreeVariants.push_back(V_SgTypeDouble);
subTreeVariants.push_back(V_SgTypeLongLong);
subTreeVariants.push_back(V_SgTypeSignedLongLong);
subTreeVariants.push_back(V_SgTypeUnsignedLongLong);
subTreeVariants.push_back(V_SgTypeLongDouble);
subTreeVariants.push_back(V_SgTypeString);
subTreeVariants.push_back(V_SgTypeBool);
subTreeVariants.push_back(V_SgPointerType);
subTreeVariants.push_back(V_SgReferenceType);
subTreeVariants.push_back(V_SgNamedType);
subTreeVariants.push_back(V_SgModifierType);
subTreeVariants.push_back(V_SgFunctionType);
subTreeVariants.push_back(V_SgArrayType);
subTreeVariants.push_back(V_SgTypeEllipse);
subTreeVariants.push_back(V_SgTemplateType);
subTreeVariants.push_back(V_SgQualifiedNameType);
subTreeVariants.push_back(V_SgTypeComplex);
subTreeVariants.push_back(V_SgTypeImaginary);
subTreeVariants.push_back(V_SgTypeDefault);
subTreeVariants.push_back(V_SgTypeCAFTeam);
subTreeVariants.push_back(V_SgTypeCrayPointer);
subTreeVariants.push_back(V_SgTypeLabel);
break;
}
case V_SgForStatement:
{
break;
}
case V_SgForInitStatement:
{
break;
}
case V_SgCatchStatementSeq:
{
break;
}
case V_SgFunctionParameterList:
{
break;
}
case V_SgCtorInitializerList:
{
break;
}
case V_SgBasicBlock:
{
break;
}
case V_SgGlobal:
{
break;
}
case V_SgIfStmt:
{
break;
}
case V_SgFunctionDefinition:
{
break;
}
case V_SgWhileStmt:
{
break;
}
case V_SgDoWhileStmt:
{
break;
}
case V_SgSwitchStatement:
{
break;
}
case V_SgCatchOptionStmt:
{
break;
}
case V_SgVariableDeclaration:
{
break;
}
case V_SgVariableDefinition:
{
break;
}
case V_SgEnumDeclaration:
{
break;
}
case V_SgAsmStmt:
{
break;
}
case V_SgTypedefDeclaration:
{
break;
}
case V_SgFunctionTypeTable:
{
break;
}
case V_SgExprStatement:
{
break;
}
case V_SgLabelStatement:
{
break;
}
case V_SgCaseOptionStmt:
{
break;
}
case V_SgTryStmt:
{
break;
}
case V_SgDefaultOptionStmt:
{
break;
}
case V_SgBreakStmt:
{
break;
}
case V_SgContinueStmt:
{
break;
}
case V_SgReturnStmt:
{
break;
}
case V_SgGotoStatement:
{
break;
}
case V_SgSpawnStmt:
{
break;
}
case V_SgNullStatement:
{
break;
}
case V_SgVariantStatement:
{
break;
}
case V_SgPragmaDeclaration:
{
break;
}
case V_SgTemplateDeclaration:
{
break;
}
case V_SgTemplateInstantiationDecl:
{
break;
}
case V_SgTemplateInstantiationDefn:
{
break;
}
case V_SgTemplateInstantiationFunctionDecl:
{
break;
}
case V_SgTemplateInstantiationMemberFunctionDecl:
{
break;
}
case V_SgProgramHeaderStatement:
{
break;
}
case V_SgProcedureHeaderStatement:
{
break;
}
case V_SgEntryStatement:
{
break;
}
case V_SgFortranNonblockedDo:
{
break;
}
case V_SgInterfaceStatement:
{
break;
}
case V_SgParameterStatement:
{
break;
}
case V_SgCommonBlock:
{
break;
}
case V_SgModuleStatement:
{
break;
}
case V_SgUseStatement:
{
break;
}
case V_SgStopOrPauseStatement:
{
break;
}
case V_SgPrintStatement:
{
break;
}
case V_SgReadStatement:
{
break;
}
case V_SgWriteStatement:
{
break;
}
case V_SgOpenStatement:
{
break;
}
case V_SgCloseStatement:
{
break;
}
case V_SgInquireStatement:
{
break;
}
case V_SgFlushStatement:
{
break;
}
case V_SgBackspaceStatement:
{
break;
}
case V_SgRewindStatement:
{
break;
}
case V_SgEndfileStatement:
{
break;
}
case V_SgWaitStatement:
{
break;
}
case V_SgCAFWithTeamStatement:
{
break;
}
case V_SgFormatStatement:
{
break;
}
case V_SgFortranDo:
{
subTreeVariants.push_back(V_SgFortranNonblockedDo);
break;
}
case V_SgForAllStatement:
{
break;
}
case V_SgIOStatement:
{
subTreeVariants.push_back(V_SgPrintStatement);
subTreeVariants.push_back(V_SgReadStatement);
subTreeVariants.push_back(V_SgWriteStatement);
subTreeVariants.push_back(V_SgOpenStatement);
subTreeVariants.push_back(V_SgCloseStatement);
subTreeVariants.push_back(V_SgInquireStatement);
subTreeVariants.push_back(V_SgFlushStatement);
subTreeVariants.push_back(V_SgBackspaceStatement);
subTreeVariants.push_back(V_SgRewindStatement);
subTreeVariants.push_back(V_SgEndfileStatement);
subTreeVariants.push_back(V_SgWaitStatement);
break;
}
case V_SgUpcNotifyStatement:
{
break;
}
case V_SgUpcWaitStatement:
{
break;
}
case V_SgUpcBarrierStatement:
{
break;
}
case V_SgUpcFenceStatement:
{
break;
}
case V_SgUpcForAllStatement:
{
break;
}
case V_SgOmpParallelStatement:
{
break;
}
case V_SgOmpSingleStatement:
{
break;
}
case V_SgOmpTaskStatement:
{
break;
}
case V_SgOmpForStatement:
{
break;
}
case V_SgOmpDoStatement:
{
break;
}
case V_SgOmpSectionsStatement:
{
break;
}
case V_SgOmpClauseBodyStatement:
{
subTreeVariants.push_back(V_SgOmpParallelStatement);
subTreeVariants.push_back(V_SgOmpSingleStatement);
subTreeVariants.push_back(V_SgOmpTaskStatement);
subTreeVariants.push_back(V_SgOmpForStatement);
subTreeVariants.push_back(V_SgOmpDoStatement);
subTreeVariants.push_back(V_SgOmpSectionsStatement);
break;
}
case V_SgOmpAtomicStatement:
{
break;
}
case V_SgOmpMasterStatement:
{
break;
}
case V_SgOmpSectionStatement:
{
break;
}
case V_SgOmpOrderedStatement:
{
break;
}
case V_SgOmpWorkshareStatement:
{
break;
}
case V_SgOmpCriticalStatement:
{
break;
}
case V_SgOmpBodyStatement:
{
subTreeVariants.push_back(V_SgOmpAtomicStatement);
subTreeVariants.push_back(V_SgOmpMasterStatement);
subTreeVariants.push_back(V_SgOmpOrderedStatement);
subTreeVariants.push_back(V_SgOmpCriticalStatement);
subTreeVariants.push_back(V_SgOmpSectionStatement);
subTreeVariants.push_back(V_SgOmpWorkshareStatement);
subTreeVariants.push_back(V_SgOmpClauseBodyStatement);
break;
}
case V_SgJavaImportStatement:
{
break;
}
case V_SgBlockDataStatement:
{
break;
}
case V_SgImplicitStatement:
{
break;
}
case V_SgStatementFunctionStatement:
{
break;
}
case V_SgWhereStatement:
{
break;
}
case V_SgNullifyStatement:
{
break;
}
case V_SgEquivalenceStatement:
{
break;
}
case V_SgDerivedTypeStatement:
{
break;
}
case V_SgAttributeSpecificationStatement:
{
break;
}
case V_SgAllocateStatement:
{
break;
}
case V_SgDeallocateStatement:
{
break;
}
case V_SgContainsStatement:
{
break;
}
case V_SgSequenceStatement:
{
break;
}
case V_SgElseWhereStatement:
{
break;
}
case V_SgArithmeticIfStatement:
{
break;
}
case V_SgAssignStatement:
{
break;
}
case V_SgComputedGotoStatement:
{
break;
}
case V_SgAssignedGotoStatement:
{
break;
}
case V_SgNamelistStatement:
{
break;
}
case V_SgImportStatement:
{
break;
}
case V_SgAssociateStatement:
{
break;
}
case V_SgFortranIncludeLine:
{
break;
}
case V_SgNamespaceDeclarationStatement:
{
break;
}
case V_SgNamespaceAliasDeclarationStatement:
{
break;
}
case V_SgNamespaceDefinitionStatement:
{
break;
}
case V_SgUsingDeclarationStatement:
{
break;
}
case V_SgUsingDirectiveStatement:
{
break;
}
case V_SgTemplateInstantiationDirectiveStatement:
{
break;
}
case V_SgClassDeclaration:
{
subTreeVariants.push_back(V_SgTemplateInstantiationDecl);
subTreeVariants.push_back(V_SgDerivedTypeStatement);
subTreeVariants.push_back(V_SgModuleStatement);
break;
}
case V_SgClassDefinition:
{
subTreeVariants.push_back(V_SgTemplateInstantiationDefn);
break;
}
case V_SgScopeStatement:
{
subTreeVariants.push_back(V_SgGlobal);
subTreeVariants.push_back(V_SgBasicBlock);
subTreeVariants.push_back(V_SgIfStmt);
subTreeVariants.push_back(V_SgForStatement);
subTreeVariants.push_back(V_SgFunctionDefinition);
subTreeVariants.push_back(V_SgClassDefinition);
subTreeVariants.push_back(V_SgWhileStmt);
subTreeVariants.push_back(V_SgDoWhileStmt);
subTreeVariants.push_back(V_SgSwitchStatement);
subTreeVariants.push_back(V_SgCatchOptionStmt);
subTreeVariants.push_back(V_SgNamespaceDefinitionStatement);
subTreeVariants.push_back(V_SgBlockDataStatement);
subTreeVariants.push_back(V_SgAssociateStatement);
subTreeVariants.push_back(V_SgFortranDo);
subTreeVariants.push_back(V_SgForAllStatement);
subTreeVariants.push_back(V_SgUpcForAllStatement);
subTreeVariants.push_back(V_SgCAFWithTeamStatement);
break;
}
case V_SgMemberFunctionDeclaration:
{
subTreeVariants.push_back(V_SgTemplateInstantiationMemberFunctionDecl);
break;
}
case V_SgFunctionDeclaration:
{
subTreeVariants.push_back(V_SgMemberFunctionDeclaration);
subTreeVariants.push_back(V_SgTemplateInstantiationFunctionDecl);
subTreeVariants.push_back(V_SgProgramHeaderStatement);
subTreeVariants.push_back(V_SgProcedureHeaderStatement);
subTreeVariants.push_back(V_SgEntryStatement);
break;
}
case V_SgIncludeDirectiveStatement:
{
break;
}
case V_SgDefineDirectiveStatement:
{
break;
}
case V_SgUndefDirectiveStatement:
{
break;
}
case V_SgIfdefDirectiveStatement:
{
break;
}
case V_SgIfndefDirectiveStatement:
{
break;
}
case V_SgIfDirectiveStatement:
{
break;
}
case V_SgDeadIfDirectiveStatement:
{
break;
}
case V_SgElseDirectiveStatement:
{
break;
}
case V_SgElseifDirectiveStatement:
{
break;
}
case V_SgEndifDirectiveStatement:
{
break;
}
case V_SgLineDirectiveStatement:
{
break;
}
case V_SgWarningDirectiveStatement:
{
break;
}
case V_SgErrorDirectiveStatement:
{
break;
}
case V_SgEmptyDirectiveStatement:
{
break;
}
case V_SgIncludeNextDirectiveStatement:
{
break;
}
case V_SgIdentDirectiveStatement:
{
break;
}
case V_SgLinemarkerDirectiveStatement:
{
break;
}
case V_SgC_PreprocessorDirectiveStatement:
{
subTreeVariants.push_back(V_SgIncludeDirectiveStatement);
subTreeVariants.push_back(V_SgDefineDirectiveStatement);
subTreeVariants.push_back(V_SgUndefDirectiveStatement);
subTreeVariants.push_back(V_SgIfdefDirectiveStatement);
subTreeVariants.push_back(V_SgIfndefDirectiveStatement);
subTreeVariants.push_back(V_SgIfDirectiveStatement);
subTreeVariants.push_back(V_SgDeadIfDirectiveStatement);
subTreeVariants.push_back(V_SgElseDirectiveStatement);
subTreeVariants.push_back(V_SgElseifDirectiveStatement);
subTreeVariants.push_back(V_SgEndifDirectiveStatement);
subTreeVariants.push_back(V_SgLineDirectiveStatement);
subTreeVariants.push_back(V_SgWarningDirectiveStatement);
subTreeVariants.push_back(V_SgErrorDirectiveStatement);
subTreeVariants.push_back(V_SgEmptyDirectiveStatement);
subTreeVariants.push_back(V_SgIncludeNextDirectiveStatement);
subTreeVariants.push_back(V_SgIdentDirectiveStatement);
subTreeVariants.push_back(V_SgLinemarkerDirectiveStatement);
break;
}
case V_SgClinkageStartStatement:
{
break;
}
case V_SgClinkageEndStatement:
{
break;
}
case V_SgClinkageDeclarationStatement:
{
subTreeVariants.push_back(V_SgClinkageStartStatement);
subTreeVariants.push_back(V_SgClinkageEndStatement);
break;
}
case V_SgOmpFlushStatement:
{
break;
}
case V_SgOmpBarrierStatement:
{
break;
}
case V_SgOmpTaskwaitStatement:
{
break;
}
case V_SgOmpThreadprivateStatement:
{
break;
}
case V_SgDeclarationStatement:
{
subTreeVariants.push_back(V_SgFunctionParameterList);
subTreeVariants.push_back(V_SgVariableDeclaration);
subTreeVariants.push_back(V_SgVariableDefinition);
subTreeVariants.push_back(V_SgClinkageDeclarationStatement);
subTreeVariants.push_back(V_SgEnumDeclaration);
subTreeVariants.push_back(V_SgAsmStmt);
subTreeVariants.push_back(V_SgAttributeSpecificationStatement);
subTreeVariants.push_back(V_SgFormatStatement);
subTreeVariants.push_back(V_SgTemplateDeclaration);
subTreeVariants.push_back(V_SgTemplateInstantiationDirectiveStatement);
subTreeVariants.push_back(V_SgUseStatement);
subTreeVariants.push_back(V_SgParameterStatement);
subTreeVariants.push_back(V_SgNamespaceDeclarationStatement);
subTreeVariants.push_back(V_SgEquivalenceStatement);
subTreeVariants.push_back(V_SgInterfaceStatement);
subTreeVariants.push_back(V_SgNamespaceAliasDeclarationStatement);
subTreeVariants.push_back(V_SgCommonBlock);
subTreeVariants.push_back(V_SgTypedefDeclaration);
subTreeVariants.push_back(V_SgStatementFunctionStatement);
subTreeVariants.push_back(V_SgCtorInitializerList);
subTreeVariants.push_back(V_SgPragmaDeclaration);
subTreeVariants.push_back(V_SgUsingDirectiveStatement);
subTreeVariants.push_back(V_SgClassDeclaration);
subTreeVariants.push_back(V_SgImplicitStatement);
subTreeVariants.push_back(V_SgUsingDeclarationStatement);
subTreeVariants.push_back(V_SgNamelistStatement);
subTreeVariants.push_back(V_SgImportStatement);
subTreeVariants.push_back(V_SgFunctionDeclaration);
subTreeVariants.push_back(V_SgContainsStatement);
subTreeVariants.push_back(V_SgC_PreprocessorDirectiveStatement);
subTreeVariants.push_back(V_SgOmpThreadprivateStatement);
subTreeVariants.push_back(V_SgFortranIncludeLine);
subTreeVariants.push_back(V_SgJavaImportStatement);
break;
}
case V_SgStatement:
{
subTreeVariants.push_back(V_SgScopeStatement);
subTreeVariants.push_back(V_SgFunctionTypeTable);
subTreeVariants.push_back(V_SgDeclarationStatement);
subTreeVariants.push_back(V_SgExprStatement);
subTreeVariants.push_back(V_SgLabelStatement);
subTreeVariants.push_back(V_SgCaseOptionStmt);
subTreeVariants.push_back(V_SgTryStmt);
subTreeVariants.push_back(V_SgDefaultOptionStmt);
subTreeVariants.push_back(V_SgBreakStmt);
subTreeVariants.push_back(V_SgContinueStmt);
subTreeVariants.push_back(V_SgReturnStmt);
subTreeVariants.push_back(V_SgGotoStatement);
subTreeVariants.push_back(V_SgSpawnStmt);
subTreeVariants.push_back(V_SgNullStatement);
subTreeVariants.push_back(V_SgVariantStatement);
subTreeVariants.push_back(V_SgForInitStatement);
subTreeVariants.push_back(V_SgCatchStatementSeq);
subTreeVariants.push_back(V_SgStopOrPauseStatement);
subTreeVariants.push_back(V_SgIOStatement);
subTreeVariants.push_back(V_SgWhereStatement);
subTreeVariants.push_back(V_SgElseWhereStatement);
subTreeVariants.push_back(V_SgNullifyStatement);
subTreeVariants.push_back(V_SgArithmeticIfStatement);
subTreeVariants.push_back(V_SgAssignStatement);
subTreeVariants.push_back(V_SgComputedGotoStatement);
subTreeVariants.push_back(V_SgAssignedGotoStatement);
subTreeVariants.push_back(V_SgAllocateStatement);
subTreeVariants.push_back(V_SgDeallocateStatement);
subTreeVariants.push_back(V_SgUpcNotifyStatement);
subTreeVariants.push_back(V_SgUpcWaitStatement);
subTreeVariants.push_back(V_SgUpcBarrierStatement);
subTreeVariants.push_back(V_SgUpcFenceStatement);
subTreeVariants.push_back(V_SgOmpBarrierStatement);
subTreeVariants.push_back(V_SgOmpTaskwaitStatement);
subTreeVariants.push_back(V_SgOmpFlushStatement);
subTreeVariants.push_back(V_SgOmpBodyStatement);
subTreeVariants.push_back(V_SgSequenceStatement);
break;
}
case V_SgExprListExp:
{
break;
}
case V_SgVarRefExp:
{
break;
}
case V_SgLabelRefExp:
{
break;
}
case V_SgClassNameRefExp:
{
break;
}
case V_SgFunctionRefExp:
{
break;
}
case V_SgMemberFunctionRefExp:
{
break;
}
case V_SgFunctionCallExp:
{
break;
}
case V_SgSizeOfOp:
{
break;
}
case V_SgUpcLocalsizeofExpression:
{
break;
}
case V_SgUpcBlocksizeofExpression:
{
break;
}
case V_SgUpcElemsizeofExpression:
{
break;
}
case V_SgVarArgStartOp:
{
break;
}
case V_SgVarArgStartOneOperandOp:
{
break;
}
case V_SgVarArgOp:
{
break;
}
case V_SgVarArgEndOp:
{
break;
}
case V_SgVarArgCopyOp:
{
break;
}
case V_SgTypeIdOp:
{
break;
}
case V_SgConditionalExp:
{
break;
}
case V_SgNewExp:
{
break;
}
case V_SgDeleteExp:
{
break;
}
case V_SgThisExp:
{
break;
}
case V_SgRefExp:
{
break;
}
case V_SgAggregateInitializer:
{
break;
}
case V_SgConstructorInitializer:
{
break;
}
case V_SgAssignInitializer:
{
break;
}
case V_SgExpressionRoot:
{
break;
}
case V_SgMinusOp:
{
break;
}
case V_SgUnaryAddOp:
{
break;
}
case V_SgNotOp:
{
break;
}
case V_SgPointerDerefExp:
{
break;
}
case V_SgAddressOfOp:
{
break;
}
case V_SgMinusMinusOp:
{
break;
}
case V_SgPlusPlusOp:
{
break;
}
case V_SgBitComplementOp:
{
break;
}
case V_SgRealPartOp:
{
break;
}
case V_SgImagPartOp:
{
break;
}
case V_SgConjugateOp:
{
break;
}
case V_SgCastExp:
{
break;
}
case V_SgThrowOp:
{
break;
}
case V_SgArrowExp:
{
break;
}
case V_SgDotExp:
{
break;
}
case V_SgDotStarOp:
{
break;
}
case V_SgArrowStarOp:
{
break;
}
case V_SgEqualityOp:
{
break;
}
case V_SgLessThanOp:
{
break;
}
case V_SgGreaterThanOp:
{
break;
}
case V_SgNotEqualOp:
{
break;
}
case V_SgLessOrEqualOp:
{
break;
}
case V_SgGreaterOrEqualOp:
{
break;
}
case V_SgAddOp:
{
break;
}
case V_SgSubtractOp:
{
break;
}
case V_SgMultiplyOp:
{
break;
}
case V_SgDivideOp:
{
break;
}
case V_SgIntegerDivideOp:
{
break;
}
case V_SgModOp:
{
break;
}
case V_SgAndOp:
{
break;
}
case V_SgOrOp:
{
break;
}
case V_SgBitXorOp:
{
break;
}
case V_SgBitAndOp:
{
break;
}
case V_SgBitOrOp:
{
break;
}
case V_SgCommaOpExp:
{
break;
}
case V_SgLshiftOp:
{
break;
}
case V_SgRshiftOp:
{
break;
}
case V_SgPntrArrRefExp:
{
break;
}
case V_SgScopeOp:
{
break;
}
case V_SgAssignOp:
{
break;
}
case V_SgPlusAssignOp:
{
break;
}
case V_SgMinusAssignOp:
{
break;
}
case V_SgAndAssignOp:
{
break;
}
case V_SgIorAssignOp:
{
break;
}
case V_SgMultAssignOp:
{
break;
}
case V_SgDivAssignOp:
{
break;
}
case V_SgModAssignOp:
{
break;
}
case V_SgXorAssignOp:
{
break;
}
case V_SgLshiftAssignOp:
{
break;
}
case V_SgRshiftAssignOp:
{
break;
}
case V_SgConcatenationOp:
{
break;
}
case V_SgBoolValExp:
{
break;
}
case V_SgStringVal:
{
break;
}
case V_SgShortVal:
{
break;
}
case V_SgCharVal:
{
break;
}
case V_SgUnsignedCharVal:
{
break;
}
case V_SgWcharVal:
{
break;
}
case V_SgUnsignedShortVal:
{
break;
}
case V_SgIntVal:
{
break;
}
case V_SgEnumVal:
{
break;
}
case V_SgUnsignedIntVal:
{
break;
}
case V_SgLongIntVal:
{
break;
}
case V_SgLongLongIntVal:
{
break;
}
case V_SgUnsignedLongLongIntVal:
{
break;
}
case V_SgUnsignedLongVal:
{
break;
}
case V_SgFloatVal:
{
break;
}
case V_SgDoubleVal:
{
break;
}
case V_SgLongDoubleVal:
{
break;
}
case V_SgUpcThreads:
{
break;
}
case V_SgUpcMythread:
{
break;
}
case V_SgComplexVal:
{
break;
}
case V_SgNullExpression:
{
break;
}
case V_SgVariantExpression:
{
break;
}
case V_SgStatementExpression:
{
break;
}
case V_SgAsmOp:
{
break;
}
case V_SgCudaKernelExecConfig:
{
break;
}
case V_SgCudaKernelCallExp:
{
break;
}
case V_SgSubscriptExpression:
{
break;
}
case V_SgColonShapeExp:
{
break;
}
case V_SgAsteriskShapeExp:
{
break;
}
case V_SgIOItemExpression:
{
break;
}
case V_SgImpliedDo:
{
break;
}
case V_SgExponentiationOp:
{
break;
}
case V_SgUnknownArrayOrFunctionReference:
{
break;
}
case V_SgActualArgumentExpression:
{
break;
}
case V_SgUserDefinedBinaryOp:
{
break;
}
case V_SgPointerAssignOp:
{
break;
}
case V_SgCAFCoExpression:
{
break;
}
case V_SgDesignatedInitializer:
{
break;
}
case V_SgInitializer:
{
subTreeVariants.push_back(V_SgAggregateInitializer);
subTreeVariants.push_back(V_SgConstructorInitializer);
subTreeVariants.push_back(V_SgAssignInitializer);
subTreeVariants.push_back(V_SgDesignatedInitializer);
break;
}
case V_SgUserDefinedUnaryOp:
{
break;
}
case V_SgPseudoDestructorRefExp:
{
break;
}
case V_SgUnaryOp:
{
subTreeVariants.push_back(V_SgExpressionRoot);
subTreeVariants.push_back(V_SgMinusOp);
subTreeVariants.push_back(V_SgUnaryAddOp);
subTreeVariants.push_back(V_SgNotOp);
subTreeVariants.push_back(V_SgPointerDerefExp);
subTreeVariants.push_back(V_SgAddressOfOp);
subTreeVariants.push_back(V_SgMinusMinusOp);
subTreeVariants.push_back(V_SgPlusPlusOp);
subTreeVariants.push_back(V_SgBitComplementOp);
subTreeVariants.push_back(V_SgCastExp);
subTreeVariants.push_back(V_SgThrowOp);
subTreeVariants.push_back(V_SgRealPartOp);
subTreeVariants.push_back(V_SgImagPartOp);
subTreeVariants.push_back(V_SgConjugateOp);
subTreeVariants.push_back(V_SgUserDefinedUnaryOp);
break;
}
case V_SgBinaryOp:
{
subTreeVariants.push_back(V_SgArrowExp);
subTreeVariants.push_back(V_SgDotExp);
subTreeVariants.push_back(V_SgDotStarOp);
subTreeVariants.push_back(V_SgArrowStarOp);
subTreeVariants.push_back(V_SgEqualityOp);
subTreeVariants.push_back(V_SgLessThanOp);
subTreeVariants.push_back(V_SgGreaterThanOp);
subTreeVariants.push_back(V_SgNotEqualOp);
subTreeVariants.push_back(V_SgLessOrEqualOp);
subTreeVariants.push_back(V_SgGreaterOrEqualOp);
subTreeVariants.push_back(V_SgAddOp);
subTreeVariants.push_back(V_SgSubtractOp);
subTreeVariants.push_back(V_SgMultiplyOp);
subTreeVariants.push_back(V_SgDivideOp);
subTreeVariants.push_back(V_SgIntegerDivideOp);
subTreeVariants.push_back(V_SgModOp);
subTreeVariants.push_back(V_SgAndOp);
subTreeVariants.push_back(V_SgOrOp);
subTreeVariants.push_back(V_SgBitXorOp);
subTreeVariants.push_back(V_SgBitAndOp);
subTreeVariants.push_back(V_SgBitOrOp);
subTreeVariants.push_back(V_SgCommaOpExp);
subTreeVariants.push_back(V_SgLshiftOp);
subTreeVariants.push_back(V_SgRshiftOp);
subTreeVariants.push_back(V_SgPntrArrRefExp);
subTreeVariants.push_back(V_SgScopeOp);
subTreeVariants.push_back(V_SgAssignOp);
subTreeVariants.push_back(V_SgPlusAssignOp);
subTreeVariants.push_back(V_SgMinusAssignOp);
subTreeVariants.push_back(V_SgAndAssignOp);
subTreeVariants.push_back(V_SgIorAssignOp);
subTreeVariants.push_back(V_SgMultAssignOp);
subTreeVariants.push_back(V_SgDivAssignOp);
subTreeVariants.push_back(V_SgModAssignOp);
subTreeVariants.push_back(V_SgXorAssignOp);
subTreeVariants.push_back(V_SgLshiftAssignOp);
subTreeVariants.push_back(V_SgRshiftAssignOp);
subTreeVariants.push_back(V_SgExponentiationOp);
subTreeVariants.push_back(V_SgConcatenationOp);
subTreeVariants.push_back(V_SgPointerAssignOp);
subTreeVariants.push_back(V_SgUserDefinedBinaryOp);
break;
}
case V_SgValueExp:
{
subTreeVariants.push_back(V_SgBoolValExp);
subTreeVariants.push_back(V_SgStringVal);
subTreeVariants.push_back(V_SgShortVal);
subTreeVariants.push_back(V_SgCharVal);
subTreeVariants.push_back(V_SgUnsignedCharVal);
subTreeVariants.push_back(V_SgWcharVal);
subTreeVariants.push_back(V_SgUnsignedShortVal);
subTreeVariants.push_back(V_SgIntVal);
subTreeVariants.push_back(V_SgEnumVal);
subTreeVariants.push_back(V_SgUnsignedIntVal);
subTreeVariants.push_back(V_SgLongIntVal);
subTreeVariants.push_back(V_SgLongLongIntVal);
subTreeVariants.push_back(V_SgUnsignedLongLongIntVal);
subTreeVariants.push_back(V_SgUnsignedLongVal);
subTreeVariants.push_back(V_SgFloatVal);
subTreeVariants.push_back(V_SgDoubleVal);
subTreeVariants.push_back(V_SgLongDoubleVal);
subTreeVariants.push_back(V_SgComplexVal);
subTreeVariants.push_back(V_SgUpcThreads);
subTreeVariants.push_back(V_SgUpcMythread);
break;
}
case V_SgExpression:
{
subTreeVariants.push_back(V_SgUnaryOp);
subTreeVariants.push_back(V_SgBinaryOp);
subTreeVariants.push_back(V_SgExprListExp);
subTreeVariants.push_back(V_SgVarRefExp);
subTreeVariants.push_back(V_SgClassNameRefExp);
subTreeVariants.push_back(V_SgFunctionRefExp);
subTreeVariants.push_back(V_SgMemberFunctionRefExp);
subTreeVariants.push_back(V_SgValueExp);
subTreeVariants.push_back(V_SgFunctionCallExp);
subTreeVariants.push_back(V_SgSizeOfOp);
subTreeVariants.push_back(V_SgUpcLocalsizeofExpression);
subTreeVariants.push_back(V_SgUpcBlocksizeofExpression);
subTreeVariants.push_back(V_SgUpcElemsizeofExpression);
subTreeVariants.push_back(V_SgTypeIdOp);
subTreeVariants.push_back(V_SgConditionalExp);
subTreeVariants.push_back(V_SgNewExp);
subTreeVariants.push_back(V_SgDeleteExp);
subTreeVariants.push_back(V_SgThisExp);
subTreeVariants.push_back(V_SgRefExp);
subTreeVariants.push_back(V_SgInitializer);
subTreeVariants.push_back(V_SgVarArgStartOp);
subTreeVariants.push_back(V_SgVarArgOp);
subTreeVariants.push_back(V_SgVarArgEndOp);
subTreeVariants.push_back(V_SgVarArgCopyOp);
subTreeVariants.push_back(V_SgVarArgStartOneOperandOp);
subTreeVariants.push_back(V_SgNullExpression);
subTreeVariants.push_back(V_SgVariantExpression);
subTreeVariants.push_back(V_SgSubscriptExpression);
subTreeVariants.push_back(V_SgColonShapeExp);
subTreeVariants.push_back(V_SgAsteriskShapeExp);
subTreeVariants.push_back(V_SgImpliedDo);
subTreeVariants.push_back(V_SgIOItemExpression);
subTreeVariants.push_back(V_SgStatementExpression);
subTreeVariants.push_back(V_SgAsmOp);
subTreeVariants.push_back(V_SgLabelRefExp);
subTreeVariants.push_back(V_SgActualArgumentExpression);
subTreeVariants.push_back(V_SgUnknownArrayOrFunctionReference);
subTreeVariants.push_back(V_SgPseudoDestructorRefExp);
subTreeVariants.push_back(V_SgCAFCoExpression);
subTreeVariants.push_back(V_SgCudaKernelCallExp);
subTreeVariants.push_back(V_SgCudaKernelExecConfig);
break;
}
case V_SgVariableSymbol:
{
break;
}
case V_SgFunctionTypeSymbol:
{
break;
}
case V_SgClassSymbol:
{
break;
}
case V_SgTemplateSymbol:
{
break;
}
case V_SgEnumSymbol:
{
break;
}
case V_SgEnumFieldSymbol:
{
break;
}
case V_SgTypedefSymbol:
{
break;
}
case V_SgMemberFunctionSymbol:
{
break;
}
case V_SgLabelSymbol:
{
break;
}
case V_SgDefaultSymbol:
{
break;
}
case V_SgNamespaceSymbol:
{
break;
}
case V_SgIntrinsicSymbol:
{
break;
}
case V_SgModuleSymbol:
{
break;
}
case V_SgInterfaceSymbol:
{
break;
}
case V_SgCommonSymbol:
{
break;
}
case V_SgRenameSymbol:
{
break;
}
case V_SgFunctionSymbol:
{
subTreeVariants.push_back(V_SgMemberFunctionSymbol);
subTreeVariants.push_back(V_SgRenameSymbol);
break;
}
case V_SgAsmBinaryAddressSymbol:
{
break;
}
case V_SgAsmBinaryDataSymbol:
{
break;
}
case V_SgAliasSymbol:
{
break;
}
case V_SgSymbol:
{
subTreeVariants.push_back(V_SgVariableSymbol);
subTreeVariants.push_back(V_SgFunctionSymbol);
subTreeVariants.push_back(V_SgFunctionTypeSymbol);
subTreeVariants.push_back(V_SgClassSymbol);
subTreeVariants.push_back(V_SgTemplateSymbol);
subTreeVariants.push_back(V_SgEnumSymbol);
subTreeVariants.push_back(V_SgEnumFieldSymbol);
subTreeVariants.push_back(V_SgTypedefSymbol);
subTreeVariants.push_back(V_SgLabelSymbol);
subTreeVariants.push_back(V_SgDefaultSymbol);
subTreeVariants.push_back(V_SgNamespaceSymbol);
subTreeVariants.push_back(V_SgIntrinsicSymbol);
subTreeVariants.push_back(V_SgModuleSymbol);
subTreeVariants.push_back(V_SgInterfaceSymbol);
subTreeVariants.push_back(V_SgCommonSymbol);
subTreeVariants.push_back(V_SgAliasSymbol);
subTreeVariants.push_back(V_SgAsmBinaryAddressSymbol);
subTreeVariants.push_back(V_SgAsmBinaryDataSymbol);
break;
}
case V_SgAsmBlock:
{
break;
}
case V_SgAsmOperandList:
{
break;
}
case V_SgAsmArmInstruction:
{
break;
}
case V_SgAsmx86Instruction:
{
break;
}
case V_SgAsmPowerpcInstruction:
{
break;
}
case V_SgAsmInstruction:
{
subTreeVariants.push_back(V_SgAsmx86Instruction);
subTreeVariants.push_back(V_SgAsmArmInstruction);
subTreeVariants.push_back(V_SgAsmPowerpcInstruction);
break;
}
case V_SgAsmDataStructureDeclaration:
{
break;
}
case V_SgAsmFunctionDeclaration:
{
break;
}
case V_SgAsmFieldDeclaration:
{
break;
}
case V_SgAsmDeclaration:
{
subTreeVariants.push_back(V_SgAsmDataStructureDeclaration);
subTreeVariants.push_back(V_SgAsmFunctionDeclaration);
subTreeVariants.push_back(V_SgAsmFieldDeclaration);
break;
}
case V_SgAsmStatement:
{
subTreeVariants.push_back(V_SgAsmDeclaration);
subTreeVariants.push_back(V_SgAsmBlock);
subTreeVariants.push_back(V_SgAsmInstruction);
break;
}
case V_SgAsmBinaryAdd:
{
break;
}
case V_SgAsmBinarySubtract:
{
break;
}
case V_SgAsmBinaryMultiply:
{
break;
}
case V_SgAsmBinaryDivide:
{
break;
}
case V_SgAsmBinaryMod:
{
break;
}
case V_SgAsmBinaryAddPreupdate:
{
break;
}
case V_SgAsmBinarySubtractPreupdate:
{
break;
}
case V_SgAsmBinaryAddPostupdate:
{
break;
}
case V_SgAsmBinarySubtractPostupdate:
{
break;
}
case V_SgAsmBinaryLsl:
{
break;
}
case V_SgAsmBinaryLsr:
{
break;
}
case V_SgAsmBinaryAsr:
{
break;
}
case V_SgAsmBinaryRor:
{
break;
}
case V_SgAsmBinaryExpression:
{
subTreeVariants.push_back(V_SgAsmBinaryAdd);
subTreeVariants.push_back(V_SgAsmBinarySubtract);
subTreeVariants.push_back(V_SgAsmBinaryMultiply);
subTreeVariants.push_back(V_SgAsmBinaryDivide);
subTreeVariants.push_back(V_SgAsmBinaryMod);
subTreeVariants.push_back(V_SgAsmBinaryAddPreupdate);
subTreeVariants.push_back(V_SgAsmBinarySubtractPreupdate);
subTreeVariants.push_back(V_SgAsmBinaryAddPostupdate);
subTreeVariants.push_back(V_SgAsmBinarySubtractPostupdate);
subTreeVariants.push_back(V_SgAsmBinaryLsl);
subTreeVariants.push_back(V_SgAsmBinaryLsr);
subTreeVariants.push_back(V_SgAsmBinaryAsr);
subTreeVariants.push_back(V_SgAsmBinaryRor);
break;
}
case V_SgAsmUnaryPlus:
{
break;
}
case V_SgAsmUnaryMinus:
{
break;
}
case V_SgAsmUnaryRrx:
{
break;
}
case V_SgAsmUnaryArmSpecialRegisterList:
{
break;
}
case V_SgAsmUnaryExpression:
{
subTreeVariants.push_back(V_SgAsmUnaryPlus);
subTreeVariants.push_back(V_SgAsmUnaryMinus);
subTreeVariants.push_back(V_SgAsmUnaryRrx);
subTreeVariants.push_back(V_SgAsmUnaryArmSpecialRegisterList);
break;
}
case V_SgAsmMemoryReferenceExpression:
{
break;
}
case V_SgAsmControlFlagsExpression:
{
break;
}
case V_SgAsmCommonSubExpression:
{
break;
}
case V_SgAsmx86RegisterReferenceExpression:
{
break;
}
case V_SgAsmArmRegisterReferenceExpression:
{
break;
}
case V_SgAsmPowerpcRegisterReferenceExpression:
{
break;
}
case V_SgAsmRegisterReferenceExpression:
{
subTreeVariants.push_back(V_SgAsmx86RegisterReferenceExpression);
subTreeVariants.push_back(V_SgAsmArmRegisterReferenceExpression);
subTreeVariants.push_back(V_SgAsmPowerpcRegisterReferenceExpression);
break;
}
case V_SgAsmByteValueExpression:
{
break;
}
case V_SgAsmWordValueExpression:
{
break;
}
case V_SgAsmDoubleWordValueExpression:
{
break;
}
case V_SgAsmQuadWordValueExpression:
{
break;
}
case V_SgAsmSingleFloatValueExpression:
{
break;
}
case V_SgAsmDoubleFloatValueExpression:
{
break;
}
case V_SgAsmVectorValueExpression:
{
break;
}
case V_SgAsmValueExpression:
{
subTreeVariants.push_back(V_SgAsmByteValueExpression);
subTreeVariants.push_back(V_SgAsmWordValueExpression);
subTreeVariants.push_back(V_SgAsmDoubleWordValueExpression);
subTreeVariants.push_back(V_SgAsmQuadWordValueExpression);
subTreeVariants.push_back(V_SgAsmSingleFloatValueExpression);
subTreeVariants.push_back(V_SgAsmDoubleFloatValueExpression);
subTreeVariants.push_back(V_SgAsmVectorValueExpression);
break;
}
case V_SgAsmExprListExp:
{
break;
}
case V_SgAsmExpression:
{
subTreeVariants.push_back(V_SgAsmValueExpression);
subTreeVariants.push_back(V_SgAsmBinaryExpression);
subTreeVariants.push_back(V_SgAsmUnaryExpression);
subTreeVariants.push_back(V_SgAsmMemoryReferenceExpression);
subTreeVariants.push_back(V_SgAsmRegisterReferenceExpression);
subTreeVariants.push_back(V_SgAsmControlFlagsExpression);
subTreeVariants.push_back(V_SgAsmCommonSubExpression);
subTreeVariants.push_back(V_SgAsmExprListExp);
break;
}
case V_SgAsmTypeByte:
{
break;
}
case V_SgAsmTypeWord:
{
break;
}
case V_SgAsmTypeDoubleWord:
{
break;
}
case V_SgAsmTypeQuadWord:
{
break;
}
case V_SgAsmTypeDoubleQuadWord:
{
break;
}
case V_SgAsmTypeSingleFloat:
{
break;
}
case V_SgAsmTypeDoubleFloat:
{
break;
}
case V_SgAsmType80bitFloat:
{
break;
}
case V_SgAsmType128bitFloat:
{
break;
}
case V_SgAsmTypeVector:
{
break;
}
case V_SgAsmType:
{
subTreeVariants.push_back(V_SgAsmTypeByte);
subTreeVariants.push_back(V_SgAsmTypeWord);
subTreeVariants.push_back(V_SgAsmTypeDoubleWord);
subTreeVariants.push_back(V_SgAsmTypeQuadWord);
subTreeVariants.push_back(V_SgAsmTypeDoubleQuadWord);
subTreeVariants.push_back(V_SgAsmType80bitFloat);
subTreeVariants.push_back(V_SgAsmType128bitFloat);
subTreeVariants.push_back(V_SgAsmTypeSingleFloat);
subTreeVariants.push_back(V_SgAsmTypeDoubleFloat);
subTreeVariants.push_back(V_SgAsmTypeVector);
break;
}
case V_SgAsmGenericDLL:
{
break;
}
case V_SgAsmPEImportHNTEntryList:
{
break;
}
case V_SgAsmPEImportILTEntryList:
{
break;
}
case V_SgAsmPEImportDirectoryList:
{
break;
}
case V_SgAsmGenericFormat:
{
break;
}
case V_SgAsmGenericFile:
{
break;
}
case V_SgAsmElfFileHeader:
{
break;
}
case V_SgAsmPEFileHeader:
{
break;
}
case V_SgAsmNEFileHeader:
{
break;
}
case V_SgAsmLEFileHeader:
{
break;
}
case V_SgAsmDOSFileHeader:
{
break;
}
case V_SgAsmGenericHeader:
{
subTreeVariants.push_back(V_SgAsmPEFileHeader);
subTreeVariants.push_back(V_SgAsmLEFileHeader);
subTreeVariants.push_back(V_SgAsmNEFileHeader);
subTreeVariants.push_back(V_SgAsmDOSFileHeader);
subTreeVariants.push_back(V_SgAsmElfFileHeader);
break;
}
case V_SgAsmElfRelocSection:
{
break;
}
case V_SgAsmElfDynamicSection:
{
break;
}
case V_SgAsmElfSymbolSection:
{
break;
}
case V_SgAsmElfStringSection:
{
break;
}
case V_SgAsmElfEHFrameSection:
{
break;
}
case V_SgAsmElfNoteSection:
{
break;
}
case V_SgAsmElfSymverSection:
{
break;
}
case V_SgAsmElfSymverDefinedSection:
{
break;
}
case V_SgAsmElfSymverNeededSection:
{
break;
}
case V_SgAsmElfStrtab:
{
break;
}
case V_SgAsmCoffStrtab:
{
break;
}
case V_SgAsmGenericStrtab:
{
subTreeVariants.push_back(V_SgAsmElfStrtab);
subTreeVariants.push_back(V_SgAsmCoffStrtab);
break;
}
case V_SgAsmElfSection:
{
subTreeVariants.push_back(V_SgAsmElfSymbolSection);
subTreeVariants.push_back(V_SgAsmElfRelocSection);
subTreeVariants.push_back(V_SgAsmElfDynamicSection);
subTreeVariants.push_back(V_SgAsmElfStringSection);
subTreeVariants.push_back(V_SgAsmElfNoteSection);
subTreeVariants.push_back(V_SgAsmElfEHFrameSection);
subTreeVariants.push_back(V_SgAsmElfSymverSection);
subTreeVariants.push_back(V_SgAsmElfSymverDefinedSection);
subTreeVariants.push_back(V_SgAsmElfSymverNeededSection);
break;
}
case V_SgAsmElfSectionTable:
{
break;
}
case V_SgAsmElfSegmentTable:
{
break;
}
case V_SgAsmPEImportSection:
{
break;
}
case V_SgAsmPEExportSection:
{
break;
}
case V_SgAsmPEStringSection:
{
break;
}
case V_SgAsmPESection:
{
subTreeVariants.push_back(V_SgAsmPEImportSection);
subTreeVariants.push_back(V_SgAsmPEExportSection);
subTreeVariants.push_back(V_SgAsmPEStringSection);
break;
}
case V_SgAsmPESectionTable:
{
break;
}
case V_SgAsmCoffSymbolTable:
{
break;
}
case V_SgAsmDOSExtendedHeader:
{
break;
}
case V_SgAsmNESection:
{
break;
}
case V_SgAsmNESectionTable:
{
break;
}
case V_SgAsmNENameTable:
{
break;
}
case V_SgAsmNEModuleTable:
{
break;
}
case V_SgAsmNEStringTable:
{
break;
}
case V_SgAsmNEEntryTable:
{
break;
}
case V_SgAsmNERelocTable:
{
break;
}
case V_SgAsmLESection:
{
break;
}
case V_SgAsmLESectionTable:
{
break;
}
case V_SgAsmLENameTable:
{
break;
}
case V_SgAsmLEPageTable:
{
break;
}
case V_SgAsmLEEntryTable:
{
break;
}
case V_SgAsmLERelocTable:
{
break;
}
case V_SgAsmGenericSection:
{
subTreeVariants.push_back(V_SgAsmGenericHeader);
subTreeVariants.push_back(V_SgAsmElfSection);
subTreeVariants.push_back(V_SgAsmElfSectionTable);
subTreeVariants.push_back(V_SgAsmElfSegmentTable);
subTreeVariants.push_back(V_SgAsmPESection);
subTreeVariants.push_back(V_SgAsmPESectionTable);
subTreeVariants.push_back(V_SgAsmDOSExtendedHeader);
subTreeVariants.push_back(V_SgAsmCoffSymbolTable);
subTreeVariants.push_back(V_SgAsmNESection);
subTreeVariants.push_back(V_SgAsmNESectionTable);
subTreeVariants.push_back(V_SgAsmNENameTable);
subTreeVariants.push_back(V_SgAsmNEModuleTable);
subTreeVariants.push_back(V_SgAsmNEStringTable);
subTreeVariants.push_back(V_SgAsmNEEntryTable);
subTreeVariants.push_back(V_SgAsmNERelocTable);
subTreeVariants.push_back(V_SgAsmLESection);
subTreeVariants.push_back(V_SgAsmLESectionTable);
subTreeVariants.push_back(V_SgAsmLENameTable);
subTreeVariants.push_back(V_SgAsmLEPageTable);
subTreeVariants.push_back(V_SgAsmLEEntryTable);
subTreeVariants.push_back(V_SgAsmLERelocTable);
break;
}
case V_SgAsmCoffSymbol:
{
break;
}
case V_SgAsmElfSymbol:
{
break;
}
case V_SgAsmGenericSymbol:
{
subTreeVariants.push_back(V_SgAsmCoffSymbol);
subTreeVariants.push_back(V_SgAsmElfSymbol);
break;
}
case V_SgAsmElfSectionTableEntry:
{
break;
}
case V_SgAsmElfSegmentTableEntry:
{
break;
}
case V_SgAsmElfSegmentTableEntryList:
{
break;
}
case V_SgAsmElfRelocEntry:
{
break;
}
case V_SgAsmElfRelocEntryList:
{
break;
}
case V_SgAsmElfDynamicEntry:
{
break;
}
case V_SgAsmElfDynamicEntryList:
{
break;
}
case V_SgAsmElfEHFrameEntryCI:
{
break;
}
case V_SgAsmElfEHFrameEntryCIList:
{
break;
}
case V_SgAsmElfEHFrameEntryFD:
{
break;
}
case V_SgAsmElfEHFrameEntryFDList:
{
break;
}
case V_SgAsmElfNoteEntry:
{
break;
}
case V_SgAsmElfNoteEntryList:
{
break;
}
case V_SgAsmElfSymverEntry:
{
break;
}
case V_SgAsmElfSymverEntryList:
{
break;
}
case V_SgAsmElfSymverDefinedEntry:
{
break;
}
case V_SgAsmElfSymverDefinedEntryList:
{
break;
}
case V_SgAsmElfSymverDefinedAux:
{
break;
}
case V_SgAsmElfSymverDefinedAuxList:
{
break;
}
case V_SgAsmElfSymverNeededEntry:
{
break;
}
case V_SgAsmElfSymverNeededEntryList:
{
break;
}
case V_SgAsmElfSymverNeededAux:
{
break;
}
case V_SgAsmElfSymverNeededAuxList:
{
break;
}
case V_SgAsmPERVASizePair:
{
break;
}
case V_SgAsmPEExportDirectory:
{
break;
}
case V_SgAsmPEExportEntry:
{
break;
}
case V_SgAsmPEImportDirectory:
{
break;
}
case V_SgAsmPEImportILTEntry:
{
break;
}
case V_SgAsmPEImportHNTEntry:
{
break;
}
case V_SgAsmPEImportLookupTable:
{
break;
}
case V_SgAsmPESectionTableEntry:
{
break;
}
case V_SgAsmNEEntryPoint:
{
break;
}
case V_SgAsmNERelocEntry:
{
break;
}
case V_SgAsmNESectionTableEntry:
{
break;
}
case V_SgAsmLEPageTableEntry:
{
break;
}
case V_SgAsmLEEntryPoint:
{
break;
}
case V_SgAsmLESectionTableEntry:
{
break;
}
case V_SgAsmGenericSectionList:
{
break;
}
case V_SgAsmGenericHeaderList:
{
break;
}
case V_SgAsmGenericSymbolList:
{
break;
}
case V_SgAsmElfSymbolList:
{
break;
}
case V_SgAsmCoffSymbolList:
{
break;
}
case V_SgAsmGenericDLLList:
{
break;
}
case V_SgAsmPERVASizePairList:
{
break;
}
case V_SgAsmPEExportEntryList:
{
break;
}
case V_SgAsmBasicString:
{
break;
}
case V_SgAsmStoredString:
{
break;
}
case V_SgAsmGenericString:
{
subTreeVariants.push_back(V_SgAsmBasicString);
subTreeVariants.push_back(V_SgAsmStoredString);
break;
}
case V_SgAsmStringStorage:
{
break;
}
case V_SgAsmDwarfMacro:
{
break;
}
case V_SgAsmDwarfLine:
{
break;
}
case V_SgAsmDwarfMacroList:
{
break;
}
case V_SgAsmDwarfLineList:
{
break;
}
case V_SgAsmDwarfArrayType:
{
break;
}
case V_SgAsmDwarfClassType:
{
break;
}
case V_SgAsmDwarfEntryPoint:
{
break;
}
case V_SgAsmDwarfEnumerationType:
{
break;
}
case V_SgAsmDwarfFormalParameter:
{
break;
}
case V_SgAsmDwarfImportedDeclaration:
{
break;
}
case V_SgAsmDwarfLabel:
{
break;
}
case V_SgAsmDwarfLexicalBlock:
{
break;
}
case V_SgAsmDwarfMember:
{
break;
}
case V_SgAsmDwarfPointerType:
{
break;
}
case V_SgAsmDwarfReferenceType:
{
break;
}
case V_SgAsmDwarfCompilationUnit:
{
break;
}
case V_SgAsmDwarfStringType:
{
break;
}
case V_SgAsmDwarfStructureType:
{
break;
}
case V_SgAsmDwarfSubroutineType:
{
break;
}
case V_SgAsmDwarfTypedef:
{
break;
}
case V_SgAsmDwarfUnionType:
{
break;
}
case V_SgAsmDwarfUnspecifiedParameters:
{
break;
}
case V_SgAsmDwarfVariant:
{
break;
}
case V_SgAsmDwarfCommonBlock:
{
break;
}
case V_SgAsmDwarfCommonInclusion:
{
break;
}
case V_SgAsmDwarfInheritance:
{
break;
}
case V_SgAsmDwarfInlinedSubroutine:
{
break;
}
case V_SgAsmDwarfModule:
{
break;
}
case V_SgAsmDwarfPtrToMemberType:
{
break;
}
case V_SgAsmDwarfSetType:
{
break;
}
case V_SgAsmDwarfSubrangeType:
{
break;
}
case V_SgAsmDwarfWithStmt:
{
break;
}
case V_SgAsmDwarfAccessDeclaration:
{
break;
}
case V_SgAsmDwarfBaseType:
{
break;
}
case V_SgAsmDwarfCatchBlock:
{
break;
}
case V_SgAsmDwarfConstType:
{
break;
}
case V_SgAsmDwarfConstant:
{
break;
}
case V_SgAsmDwarfEnumerator:
{
break;
}
case V_SgAsmDwarfFileType:
{
break;
}
case V_SgAsmDwarfFriend:
{
break;
}
case V_SgAsmDwarfNamelist:
{
break;
}
case V_SgAsmDwarfNamelistItem:
{
break;
}
case V_SgAsmDwarfPackedType:
{
break;
}
case V_SgAsmDwarfSubprogram:
{
break;
}
case V_SgAsmDwarfTemplateTypeParameter:
{
break;
}
case V_SgAsmDwarfTemplateValueParameter:
{
break;
}
case V_SgAsmDwarfThrownType:
{
break;
}
case V_SgAsmDwarfTryBlock:
{
break;
}
case V_SgAsmDwarfVariantPart:
{
break;
}
case V_SgAsmDwarfVariable:
{
break;
}
case V_SgAsmDwarfVolatileType:
{
break;
}
case V_SgAsmDwarfDwarfProcedure:
{
break;
}
case V_SgAsmDwarfRestrictType:
{
break;
}
case V_SgAsmDwarfInterfaceType:
{
break;
}
case V_SgAsmDwarfNamespace:
{
break;
}
case V_SgAsmDwarfImportedModule:
{
break;
}
case V_SgAsmDwarfUnspecifiedType:
{
break;
}
case V_SgAsmDwarfPartialUnit:
{
break;
}
case V_SgAsmDwarfImportedUnit:
{
break;
}
case V_SgAsmDwarfMutableType:
{
break;
}
case V_SgAsmDwarfCondition:
{
break;
}
case V_SgAsmDwarfSharedType:
{
break;
}
case V_SgAsmDwarfFormatLabel:
{
break;
}
case V_SgAsmDwarfFunctionTemplate:
{
break;
}
case V_SgAsmDwarfClassTemplate:
{
break;
}
case V_SgAsmDwarfUpcSharedType:
{
break;
}
case V_SgAsmDwarfUpcStrictType:
{
break;
}
case V_SgAsmDwarfUpcRelaxedType:
{
break;
}
case V_SgAsmDwarfUnknownConstruct:
{
break;
}
case V_SgAsmDwarfConstruct:
{
subTreeVariants.push_back(V_SgAsmDwarfArrayType);
subTreeVariants.push_back(V_SgAsmDwarfClassType);
subTreeVariants.push_back(V_SgAsmDwarfEntryPoint);
subTreeVariants.push_back(V_SgAsmDwarfEnumerationType);
subTreeVariants.push_back(V_SgAsmDwarfFormalParameter);
subTreeVariants.push_back(V_SgAsmDwarfImportedDeclaration);
subTreeVariants.push_back(V_SgAsmDwarfLabel);
subTreeVariants.push_back(V_SgAsmDwarfLexicalBlock);
subTreeVariants.push_back(V_SgAsmDwarfMember);
subTreeVariants.push_back(V_SgAsmDwarfPointerType);
subTreeVariants.push_back(V_SgAsmDwarfReferenceType);
subTreeVariants.push_back(V_SgAsmDwarfCompilationUnit);
subTreeVariants.push_back(V_SgAsmDwarfStringType);
subTreeVariants.push_back(V_SgAsmDwarfStructureType);
subTreeVariants.push_back(V_SgAsmDwarfSubroutineType);
subTreeVariants.push_back(V_SgAsmDwarfTypedef);
subTreeVariants.push_back(V_SgAsmDwarfUnionType);
subTreeVariants.push_back(V_SgAsmDwarfUnspecifiedParameters);
subTreeVariants.push_back(V_SgAsmDwarfVariant);
subTreeVariants.push_back(V_SgAsmDwarfCommonBlock);
subTreeVariants.push_back(V_SgAsmDwarfCommonInclusion);
subTreeVariants.push_back(V_SgAsmDwarfInheritance);
subTreeVariants.push_back(V_SgAsmDwarfInlinedSubroutine);
subTreeVariants.push_back(V_SgAsmDwarfModule);
subTreeVariants.push_back(V_SgAsmDwarfPtrToMemberType);
subTreeVariants.push_back(V_SgAsmDwarfSetType);
subTreeVariants.push_back(V_SgAsmDwarfSubrangeType);
subTreeVariants.push_back(V_SgAsmDwarfWithStmt);
subTreeVariants.push_back(V_SgAsmDwarfAccessDeclaration);
subTreeVariants.push_back(V_SgAsmDwarfBaseType);
subTreeVariants.push_back(V_SgAsmDwarfCatchBlock);
subTreeVariants.push_back(V_SgAsmDwarfConstType);
subTreeVariants.push_back(V_SgAsmDwarfConstant);
subTreeVariants.push_back(V_SgAsmDwarfEnumerator);
subTreeVariants.push_back(V_SgAsmDwarfFileType);
subTreeVariants.push_back(V_SgAsmDwarfFriend);
subTreeVariants.push_back(V_SgAsmDwarfNamelist);
subTreeVariants.push_back(V_SgAsmDwarfNamelistItem);
subTreeVariants.push_back(V_SgAsmDwarfPackedType);
subTreeVariants.push_back(V_SgAsmDwarfSubprogram);
subTreeVariants.push_back(V_SgAsmDwarfTemplateTypeParameter);
subTreeVariants.push_back(V_SgAsmDwarfTemplateValueParameter);
subTreeVariants.push_back(V_SgAsmDwarfThrownType);
subTreeVariants.push_back(V_SgAsmDwarfTryBlock);
subTreeVariants.push_back(V_SgAsmDwarfVariantPart);
subTreeVariants.push_back(V_SgAsmDwarfVariable);
subTreeVariants.push_back(V_SgAsmDwarfVolatileType);
subTreeVariants.push_back(V_SgAsmDwarfDwarfProcedure);
subTreeVariants.push_back(V_SgAsmDwarfRestrictType);
subTreeVariants.push_back(V_SgAsmDwarfInterfaceType);
subTreeVariants.push_back(V_SgAsmDwarfNamespace);
subTreeVariants.push_back(V_SgAsmDwarfImportedModule);
subTreeVariants.push_back(V_SgAsmDwarfUnspecifiedType);
subTreeVariants.push_back(V_SgAsmDwarfPartialUnit);
subTreeVariants.push_back(V_SgAsmDwarfImportedUnit);
subTreeVariants.push_back(V_SgAsmDwarfMutableType);
subTreeVariants.push_back(V_SgAsmDwarfCondition);
subTreeVariants.push_back(V_SgAsmDwarfSharedType);
subTreeVariants.push_back(V_SgAsmDwarfFormatLabel);
subTreeVariants.push_back(V_SgAsmDwarfFunctionTemplate);
subTreeVariants.push_back(V_SgAsmDwarfClassTemplate);
subTreeVariants.push_back(V_SgAsmDwarfUpcSharedType);
subTreeVariants.push_back(V_SgAsmDwarfUpcStrictType);
subTreeVariants.push_back(V_SgAsmDwarfUpcRelaxedType);
subTreeVariants.push_back(V_SgAsmDwarfUnknownConstruct);
break;
}
case V_SgAsmDwarfConstructList:
{
break;
}
case V_SgAsmDwarfCompilationUnitList:
{
break;
}
case V_SgAsmDwarfInformation:
{
subTreeVariants.push_back(V_SgAsmDwarfMacro);
subTreeVariants.push_back(V_SgAsmDwarfMacroList);
subTreeVariants.push_back(V_SgAsmDwarfLine);
subTreeVariants.push_back(V_SgAsmDwarfLineList);
subTreeVariants.push_back(V_SgAsmDwarfCompilationUnitList);
subTreeVariants.push_back(V_SgAsmDwarfConstruct);
subTreeVariants.push_back(V_SgAsmDwarfConstructList);
break;
}
case V_SgAsmExecutableFileFormat:
{
subTreeVariants.push_back(V_SgAsmGenericDLL);
subTreeVariants.push_back(V_SgAsmGenericFormat);
subTreeVariants.push_back(V_SgAsmGenericDLLList);
subTreeVariants.push_back(V_SgAsmElfEHFrameEntryFD);
subTreeVariants.push_back(V_SgAsmGenericFile);
subTreeVariants.push_back(V_SgAsmGenericSection);
subTreeVariants.push_back(V_SgAsmGenericSymbol);
subTreeVariants.push_back(V_SgAsmGenericStrtab);
subTreeVariants.push_back(V_SgAsmGenericSymbolList);
subTreeVariants.push_back(V_SgAsmGenericSectionList);
subTreeVariants.push_back(V_SgAsmGenericHeaderList);
subTreeVariants.push_back(V_SgAsmGenericString);
subTreeVariants.push_back(V_SgAsmElfSectionTableEntry);
subTreeVariants.push_back(V_SgAsmElfSegmentTableEntry);
subTreeVariants.push_back(V_SgAsmElfSymbolList);
subTreeVariants.push_back(V_SgAsmPEImportILTEntry);
subTreeVariants.push_back(V_SgAsmElfRelocEntry);
subTreeVariants.push_back(V_SgAsmElfRelocEntryList);
subTreeVariants.push_back(V_SgAsmPEExportEntry);
subTreeVariants.push_back(V_SgAsmPEExportEntryList);
subTreeVariants.push_back(V_SgAsmElfDynamicEntry);
subTreeVariants.push_back(V_SgAsmElfDynamicEntryList);
subTreeVariants.push_back(V_SgAsmElfSegmentTableEntryList);
subTreeVariants.push_back(V_SgAsmStringStorage);
subTreeVariants.push_back(V_SgAsmElfNoteEntry);
subTreeVariants.push_back(V_SgAsmElfNoteEntryList);
subTreeVariants.push_back(V_SgAsmElfSymverEntry);
subTreeVariants.push_back(V_SgAsmElfSymverEntryList);
subTreeVariants.push_back(V_SgAsmElfSymverDefinedEntry);
subTreeVariants.push_back(V_SgAsmElfSymverDefinedEntryList);
subTreeVariants.push_back(V_SgAsmElfSymverDefinedAux);
subTreeVariants.push_back(V_SgAsmElfSymverDefinedAuxList);
subTreeVariants.push_back(V_SgAsmElfSymverNeededEntry);
subTreeVariants.push_back(V_SgAsmElfSymverNeededEntryList);
subTreeVariants.push_back(V_SgAsmElfSymverNeededAux);
subTreeVariants.push_back(V_SgAsmElfSymverNeededAuxList);
subTreeVariants.push_back(V_SgAsmPEImportDirectory);
subTreeVariants.push_back(V_SgAsmPEImportHNTEntry);
subTreeVariants.push_back(V_SgAsmPESectionTableEntry);
subTreeVariants.push_back(V_SgAsmPEExportDirectory);
subTreeVariants.push_back(V_SgAsmPERVASizePair);
subTreeVariants.push_back(V_SgAsmCoffSymbolList);
subTreeVariants.push_back(V_SgAsmPERVASizePairList);
subTreeVariants.push_back(V_SgAsmElfEHFrameEntryCI);
subTreeVariants.push_back(V_SgAsmPEImportHNTEntryList);
subTreeVariants.push_back(V_SgAsmPEImportILTEntryList);
subTreeVariants.push_back(V_SgAsmPEImportLookupTable);
subTreeVariants.push_back(V_SgAsmPEImportDirectoryList);
subTreeVariants.push_back(V_SgAsmNEEntryPoint);
subTreeVariants.push_back(V_SgAsmNERelocEntry);
subTreeVariants.push_back(V_SgAsmNESectionTableEntry);
subTreeVariants.push_back(V_SgAsmElfEHFrameEntryCIList);
subTreeVariants.push_back(V_SgAsmLEPageTableEntry);
subTreeVariants.push_back(V_SgAsmLEEntryPoint);
subTreeVariants.push_back(V_SgAsmLESectionTableEntry);
subTreeVariants.push_back(V_SgAsmElfEHFrameEntryFDList);
subTreeVariants.push_back(V_SgAsmDwarfInformation);
break;
}
case V_SgAsmInterpretation:
{
break;
}
case V_SgAsmInterpretationList:
{
break;
}
case V_SgAsmGenericFileList:
{
break;
}
case V_SgAsmNode:
{
subTreeVariants.push_back(V_SgAsmStatement);
subTreeVariants.push_back(V_SgAsmExpression);
subTreeVariants.push_back(V_SgAsmInterpretation);
subTreeVariants.push_back(V_SgAsmOperandList);
subTreeVariants.push_back(V_SgAsmType);
subTreeVariants.push_back(V_SgAsmExecutableFileFormat);
subTreeVariants.push_back(V_SgAsmInterpretationList);
subTreeVariants.push_back(V_SgAsmGenericFileList);
break;
}
case V_SgCommonBlockObject:
{
break;
}
case V_SgInitializedName:
{
break;
}
case V_SgOmpOrderedClause:
{
break;
}
case V_SgOmpNowaitClause:
{
break;
}
case V_SgOmpUntiedClause:
{
break;
}
case V_SgOmpDefaultClause:
{
break;
}
case V_SgOmpCollapseClause:
{
break;
}
case V_SgOmpIfClause:
{
break;
}
case V_SgOmpNumThreadsClause:
{
break;
}
case V_SgOmpExpressionClause:
{
subTreeVariants.push_back(V_SgOmpCollapseClause);
subTreeVariants.push_back(V_SgOmpIfClause);
subTreeVariants.push_back(V_SgOmpNumThreadsClause);
break;
}
case V_SgOmpCopyprivateClause:
{
break;
}
case V_SgOmpPrivateClause:
{
break;
}
case V_SgOmpFirstprivateClause:
{
break;
}
case V_SgOmpSharedClause:
{
break;
}
case V_SgOmpCopyinClause:
{
break;
}
case V_SgOmpLastprivateClause:
{
break;
}
case V_SgOmpReductionClause:
{
break;
}
case V_SgOmpVariablesClause:
{
subTreeVariants.push_back(V_SgOmpCopyprivateClause);
subTreeVariants.push_back(V_SgOmpPrivateClause);
subTreeVariants.push_back(V_SgOmpFirstprivateClause);
subTreeVariants.push_back(V_SgOmpSharedClause);
subTreeVariants.push_back(V_SgOmpCopyinClause);
subTreeVariants.push_back(V_SgOmpLastprivateClause);
subTreeVariants.push_back(V_SgOmpReductionClause);
break;
}
case V_SgOmpScheduleClause:
{
break;
}
case V_SgOmpClause:
{
subTreeVariants.push_back(V_SgOmpOrderedClause);
subTreeVariants.push_back(V_SgOmpNowaitClause);
subTreeVariants.push_back(V_SgOmpUntiedClause);
subTreeVariants.push_back(V_SgOmpDefaultClause);
subTreeVariants.push_back(V_SgOmpExpressionClause);
subTreeVariants.push_back(V_SgOmpVariablesClause);
subTreeVariants.push_back(V_SgOmpScheduleClause);
break;
}
case V_SgRenamePair:
{
break;
}
case V_SgInterfaceBody:
{
break;
}
case V_SgLocatedNodeSupport:
{
subTreeVariants.push_back(V_SgCommonBlockObject);
subTreeVariants.push_back(V_SgInitializedName);
subTreeVariants.push_back(V_SgInterfaceBody);
subTreeVariants.push_back(V_SgRenamePair);
subTreeVariants.push_back(V_SgOmpClause);
break;
}
case V_SgToken:
{
break;
}
case V_SgLocatedNode:
{
subTreeVariants.push_back(V_SgLocatedNodeSupport);
subTreeVariants.push_back(V_SgStatement);
subTreeVariants.push_back(V_SgExpression);
subTreeVariants.push_back(V_SgToken);
break;
}
case V_SgNode:
{
subTreeVariants.push_back(V_SgSupport);
subTreeVariants.push_back(V_SgType);
subTreeVariants.push_back(V_SgLocatedNode);
subTreeVariants.push_back(V_SgSymbol);
subTreeVariants.push_back(V_SgAsmNode);
break;
}
default:
{ }
}

};




std::vector<VariantT> SgNode::getClassHierarchySubTreeFunction( VariantT v){
std::vector<VariantT> subTreeVariants;
getClassHierarchySubTreeFunction(v, subTreeVariants);
return subTreeVariants;
}
