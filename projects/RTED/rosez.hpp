
#ifndef _ROSEZ_HPP

#define _ROSEZ_HPP

#include <iostream>
#include <typeinfo>

#include "rose.h"

namespace ez
{
  /// \brief portable implementation to avoid compiler warnings due to an
  ///        unused parameter.
  template <class T>
  static inline
  void unused(const T&) {}

  template <class SageNode>
  static inline
  SageNode& assume_sgtype(SgNode& n)
  {
    return static_cast<SageNode&>(n);
  }

  template <class SageNode>
  static inline
  const SageNode& assume_sgtype(const SgNode& n)
  {
    return static_cast<const SageNode&>(n);
  }

  /// \brief for internal use (use visitSgNode instead)
  template <class RoseVisitor, class SageNode>
  static inline
  RoseVisitor
  _visitSgNode(RoseVisitor rv, SageNode* n)
  {
    ROSE_ASSERT( n );

    switch (n->variantT())
    {
      case V_SgName:
        rv.handle(assume_sgtype<SgName>(*n));
        break;

      case V_SgSymbolTable:
        rv.handle(assume_sgtype<SgSymbolTable>(*n));
        break;

      case V_SgPragma:
        rv.handle(assume_sgtype<SgPragma>(*n));
        break;

      case V_SgModifierNodes:
        rv.handle(assume_sgtype<SgModifierNodes>(*n));
        break;

      case V_SgConstVolatileModifier:
        rv.handle(assume_sgtype<SgConstVolatileModifier>(*n));
        break;

      case V_SgStorageModifier:
        rv.handle(assume_sgtype<SgStorageModifier>(*n));
        break;

      case V_SgAccessModifier:
        rv.handle(assume_sgtype<SgAccessModifier>(*n));
        break;

      case V_SgFunctionModifier:
        rv.handle(assume_sgtype<SgFunctionModifier>(*n));
        break;

      case V_SgUPC_AccessModifier:
        rv.handle(assume_sgtype<SgUPC_AccessModifier>(*n));
        break;

      case V_SgLinkageModifier:
        rv.handle(assume_sgtype<SgLinkageModifier>(*n));
        break;

      case V_SgSpecialFunctionModifier:
        rv.handle(assume_sgtype<SgSpecialFunctionModifier>(*n));
        break;

      case V_SgTypeModifier:
        rv.handle(assume_sgtype<SgTypeModifier>(*n));
        break;

      case V_SgElaboratedTypeModifier:
        rv.handle(assume_sgtype<SgElaboratedTypeModifier>(*n));
        break;

      case V_SgBaseClassModifier:
        rv.handle(assume_sgtype<SgBaseClassModifier>(*n));
        break;

      case V_SgDeclarationModifier:
        rv.handle(assume_sgtype<SgDeclarationModifier>(*n));
        break;

      case V_SgOpenclAccessModeModifier:
        rv.handle(assume_sgtype<SgOpenclAccessModeModifier>(*n));
        break;

      case V_SgModifier:
        rv.handle(assume_sgtype<SgModifier>(*n));
        break;

      case V_Sg_File_Info:
        rv.handle(assume_sgtype<Sg_File_Info>(*n));
        break;

      case V_SgSourceFile:
        rv.handle(assume_sgtype<SgSourceFile>(*n));
        break;

      case V_SgBinaryComposite:
        rv.handle(assume_sgtype<SgBinaryComposite>(*n));
        break;

      case V_SgUnknownFile:
        rv.handle(assume_sgtype<SgUnknownFile>(*n));
        break;

      case V_SgFile:
        rv.handle(assume_sgtype<SgFile>(*n));
        break;

      case V_SgFileList:
        rv.handle(assume_sgtype<SgFileList>(*n));
        break;

      case V_SgDirectory:
        rv.handle(assume_sgtype<SgDirectory>(*n));
        break;

      case V_SgDirectoryList:
        rv.handle(assume_sgtype<SgDirectoryList>(*n));
        break;

      case V_SgProject:
        rv.handle(assume_sgtype<SgProject>(*n));
        break;

      case V_SgOptions:
        rv.handle(assume_sgtype<SgOptions>(*n));
        break;

      case V_SgUnparse_Info:
        rv.handle(assume_sgtype<SgUnparse_Info>(*n));
        break;

      case V_SgFuncDecl_attr:
        rv.handle(assume_sgtype<SgFuncDecl_attr>(*n));
        break;

      case V_SgClassDecl_attr:
        rv.handle(assume_sgtype<SgClassDecl_attr>(*n));
        break;

      case V_SgTypedefSeq:
        rv.handle(assume_sgtype<SgTypedefSeq>(*n));
        break;

      case V_SgFunctionParameterTypeList:
        rv.handle(assume_sgtype<SgFunctionParameterTypeList>(*n));
        break;

      case V_SgTemplateParameter:
        rv.handle(assume_sgtype<SgTemplateParameter>(*n));
        break;

      case V_SgTemplateArgument:
        rv.handle(assume_sgtype<SgTemplateArgument>(*n));
        break;

      case V_SgTemplateParameterList:
        rv.handle(assume_sgtype<SgTemplateParameterList>(*n));
        break;

      case V_SgTemplateArgumentList:
        rv.handle(assume_sgtype<SgTemplateArgumentList>(*n));
        break;

      case V_SgBitAttribute:
        rv.handle(assume_sgtype<SgBitAttribute>(*n));
        break;

      case V_SgAttribute:
        rv.handle(assume_sgtype<SgAttribute>(*n));
        break;

      case V_SgBaseClass:
        rv.handle(assume_sgtype<SgBaseClass>(*n));
        break;

      case V_SgUndirectedGraphEdge:
        rv.handle(assume_sgtype<SgUndirectedGraphEdge>(*n));
        break;

      case V_SgDirectedGraphEdge:
        rv.handle(assume_sgtype<SgDirectedGraphEdge>(*n));
        break;

      case V_SgGraphNode:
        rv.handle(assume_sgtype<SgGraphNode>(*n));
        break;

      case V_SgGraphEdge:
        rv.handle(assume_sgtype<SgGraphEdge>(*n));
        break;

      case V_SgStringKeyedBidirectionalGraph:
        rv.handle(assume_sgtype<SgStringKeyedBidirectionalGraph>(*n));
        break;

      case V_SgIntKeyedBidirectionalGraph:
        rv.handle(assume_sgtype<SgIntKeyedBidirectionalGraph>(*n));
        break;

      case V_SgBidirectionalGraph:
        rv.handle(assume_sgtype<SgBidirectionalGraph>(*n));
        break;

      case V_SgIncidenceDirectedGraph:
        rv.handle(assume_sgtype<SgIncidenceDirectedGraph>(*n));
        break;

      case V_SgIncidenceUndirectedGraph:
        rv.handle(assume_sgtype<SgIncidenceUndirectedGraph>(*n));
        break;

      case V_SgGraph:
        rv.handle(assume_sgtype<SgGraph>(*n));
        break;

      case V_SgGraphNodeList:
        rv.handle(assume_sgtype<SgGraphNodeList>(*n));
        break;

      case V_SgGraphEdgeList:
        rv.handle(assume_sgtype<SgGraphEdgeList>(*n));
        break;

      case V_SgQualifiedName:
        rv.handle(assume_sgtype<SgQualifiedName>(*n));
        break;

      case V_SgNameGroup:
        rv.handle(assume_sgtype<SgNameGroup>(*n));
        break;

      case V_SgCommonBlockObject:
        rv.handle(assume_sgtype<SgCommonBlockObject>(*n));
        break;

      case V_SgDimensionObject:
        rv.handle(assume_sgtype<SgDimensionObject>(*n));
        break;

      case V_SgDataStatementGroup:
        rv.handle(assume_sgtype<SgDataStatementGroup>(*n));
        break;

      case V_SgDataStatementObject:
        rv.handle(assume_sgtype<SgDataStatementObject>(*n));
        break;

      case V_SgDataStatementValue:
        rv.handle(assume_sgtype<SgDataStatementValue>(*n));
        break;

      case V_SgFormatItem:
        rv.handle(assume_sgtype<SgFormatItem>(*n));
        break;

      case V_SgFormatItemList:
        rv.handle(assume_sgtype<SgFormatItemList>(*n));
        break;

      case V_SgTypeTable:
        rv.handle(assume_sgtype<SgTypeTable>(*n));
        break;

      case V_SgSupport:
        rv.handle(assume_sgtype<SgSupport>(*n));
        break;

      case V_SgForStatement:
        rv.handle(assume_sgtype<SgForStatement>(*n));
        break;

      case V_SgForInitStatement:
        rv.handle(assume_sgtype<SgForInitStatement>(*n));
        break;

      case V_SgCatchStatementSeq:
        rv.handle(assume_sgtype<SgCatchStatementSeq>(*n));
        break;

      case V_SgFunctionParameterList:
        rv.handle(assume_sgtype<SgFunctionParameterList>(*n));
        break;

      case V_SgCtorInitializerList:
        rv.handle(assume_sgtype<SgCtorInitializerList>(*n));
        break;

      case V_SgBasicBlock:
        rv.handle(assume_sgtype<SgBasicBlock>(*n));
        break;

      case V_SgGlobal:
        rv.handle(assume_sgtype<SgGlobal>(*n));
        break;

      case V_SgIfStmt:
        rv.handle(assume_sgtype<SgIfStmt>(*n));
        break;

      case V_SgFunctionDefinition:
        rv.handle(assume_sgtype<SgFunctionDefinition>(*n));
        break;

      case V_SgWhileStmt:
        rv.handle(assume_sgtype<SgWhileStmt>(*n));
        break;

      case V_SgDoWhileStmt:
        rv.handle(assume_sgtype<SgDoWhileStmt>(*n));
        break;

      case V_SgSwitchStatement:
        rv.handle(assume_sgtype<SgSwitchStatement>(*n));
        break;

      case V_SgCatchOptionStmt:
        rv.handle(assume_sgtype<SgCatchOptionStmt>(*n));
        break;

      case V_SgVariableDeclaration:
        rv.handle(assume_sgtype<SgVariableDeclaration>(*n));
        break;

      case V_SgVariableDefinition:
        rv.handle(assume_sgtype<SgVariableDefinition>(*n));
        break;

      case V_SgEnumDeclaration:
        rv.handle(assume_sgtype<SgEnumDeclaration>(*n));
        break;

      case V_SgAsmStmt:
        rv.handle(assume_sgtype<SgAsmStmt>(*n));
        break;

      case V_SgTypedefDeclaration:
        rv.handle(assume_sgtype<SgTypedefDeclaration>(*n));
        break;

      case V_SgFunctionTypeTable:
        rv.handle(assume_sgtype<SgFunctionTypeTable>(*n));
        break;

      case V_SgExprStatement:
        rv.handle(assume_sgtype<SgExprStatement>(*n));
        break;

      case V_SgLabelStatement:
        rv.handle(assume_sgtype<SgLabelStatement>(*n));
        break;

      case V_SgCaseOptionStmt:
        rv.handle(assume_sgtype<SgCaseOptionStmt>(*n));
        break;

      case V_SgTryStmt:
        rv.handle(assume_sgtype<SgTryStmt>(*n));
        break;

      case V_SgDefaultOptionStmt:
        rv.handle(assume_sgtype<SgDefaultOptionStmt>(*n));
        break;

      case V_SgBreakStmt:
        rv.handle(assume_sgtype<SgBreakStmt>(*n));
        break;

      case V_SgContinueStmt:
        rv.handle(assume_sgtype<SgContinueStmt>(*n));
        break;

      case V_SgReturnStmt:
        rv.handle(assume_sgtype<SgReturnStmt>(*n));
        break;

      case V_SgGotoStatement:
        rv.handle(assume_sgtype<SgGotoStatement>(*n));
        break;

      case V_SgSpawnStmt:
        rv.handle(assume_sgtype<SgSpawnStmt>(*n));
        break;

      case V_SgNullStatement:
        rv.handle(assume_sgtype<SgNullStatement>(*n));
        break;

      case V_SgVariantStatement:
        rv.handle(assume_sgtype<SgVariantStatement>(*n));
        break;

      case V_SgPragmaDeclaration:
        rv.handle(assume_sgtype<SgPragmaDeclaration>(*n));
        break;

      case V_SgTemplateDeclaration:
        rv.handle(assume_sgtype<SgTemplateDeclaration>(*n));
        break;

      case V_SgTemplateInstantiationDecl:
        rv.handle(assume_sgtype<SgTemplateInstantiationDecl>(*n));
        break;

      case V_SgTemplateInstantiationDefn:
        rv.handle(assume_sgtype<SgTemplateInstantiationDefn>(*n));
        break;

      case V_SgTemplateInstantiationFunctionDecl:
        rv.handle(assume_sgtype<SgTemplateInstantiationFunctionDecl>(*n));
        break;

      case V_SgTemplateInstantiationMemberFunctionDecl:
        rv.handle(assume_sgtype<SgTemplateInstantiationMemberFunctionDecl>(*n));
        break;

      case V_SgProgramHeaderStatement:
        rv.handle(assume_sgtype<SgProgramHeaderStatement>(*n));
        break;

      case V_SgProcedureHeaderStatement:
        rv.handle(assume_sgtype<SgProcedureHeaderStatement>(*n));
        break;

      case V_SgEntryStatement:
        rv.handle(assume_sgtype<SgEntryStatement>(*n));
        break;

      case V_SgFortranNonblockedDo:
        rv.handle(assume_sgtype<SgFortranNonblockedDo>(*n));
        break;

      case V_SgInterfaceStatement:
        rv.handle(assume_sgtype<SgInterfaceStatement>(*n));
        break;

      case V_SgParameterStatement:
        rv.handle(assume_sgtype<SgParameterStatement>(*n));
        break;

      case V_SgCommonBlock:
        rv.handle(assume_sgtype<SgCommonBlock>(*n));
        break;

      case V_SgModuleStatement:
        rv.handle(assume_sgtype<SgModuleStatement>(*n));
        break;

      case V_SgUseStatement:
        rv.handle(assume_sgtype<SgUseStatement>(*n));
        break;

      case V_SgStopOrPauseStatement:
        rv.handle(assume_sgtype<SgStopOrPauseStatement>(*n));
        break;

      case V_SgPrintStatement:
        rv.handle(assume_sgtype<SgPrintStatement>(*n));
        break;

      case V_SgReadStatement:
        rv.handle(assume_sgtype<SgReadStatement>(*n));
        break;

      case V_SgWriteStatement:
        rv.handle(assume_sgtype<SgWriteStatement>(*n));
        break;

      case V_SgOpenStatement:
        rv.handle(assume_sgtype<SgOpenStatement>(*n));
        break;

      case V_SgCloseStatement:
        rv.handle(assume_sgtype<SgCloseStatement>(*n));
        break;

      case V_SgInquireStatement:
        rv.handle(assume_sgtype<SgInquireStatement>(*n));
        break;

      case V_SgFlushStatement:
        rv.handle(assume_sgtype<SgFlushStatement>(*n));
        break;

      case V_SgBackspaceStatement:
        rv.handle(assume_sgtype<SgBackspaceStatement>(*n));
        break;

      case V_SgRewindStatement:
        rv.handle(assume_sgtype<SgRewindStatement>(*n));
        break;

      case V_SgEndfileStatement:
        rv.handle(assume_sgtype<SgEndfileStatement>(*n));
        break;

      case V_SgWaitStatement:
        rv.handle(assume_sgtype<SgWaitStatement>(*n));
        break;

      case V_SgCAFWithTeamStatement:
        rv.handle(assume_sgtype<SgCAFWithTeamStatement>(*n));
        break;

      case V_SgFormatStatement:
        rv.handle(assume_sgtype<SgFormatStatement>(*n));
        break;

      case V_SgFortranDo:
        rv.handle(assume_sgtype<SgFortranDo>(*n));
        break;

      case V_SgForAllStatement:
        rv.handle(assume_sgtype<SgForAllStatement>(*n));
        break;

      case V_SgIOStatement:
        rv.handle(assume_sgtype<SgIOStatement>(*n));
        break;

      case V_SgUpcNotifyStatement:
        rv.handle(assume_sgtype<SgUpcNotifyStatement>(*n));
        break;

      case V_SgUpcWaitStatement:
        rv.handle(assume_sgtype<SgUpcWaitStatement>(*n));
        break;

      case V_SgUpcBarrierStatement:
        rv.handle(assume_sgtype<SgUpcBarrierStatement>(*n));
        break;

      case V_SgUpcFenceStatement:
        rv.handle(assume_sgtype<SgUpcFenceStatement>(*n));
        break;

      case V_SgUpcForAllStatement:
        rv.handle(assume_sgtype<SgUpcForAllStatement>(*n));
        break;

      case V_SgOmpParallelStatement:
        rv.handle(assume_sgtype<SgOmpParallelStatement>(*n));
        break;

      case V_SgOmpSingleStatement:
        rv.handle(assume_sgtype<SgOmpSingleStatement>(*n));
        break;

      case V_SgOmpTaskStatement:
        rv.handle(assume_sgtype<SgOmpTaskStatement>(*n));
        break;

      case V_SgOmpForStatement:
        rv.handle(assume_sgtype<SgOmpForStatement>(*n));
        break;

      case V_SgOmpDoStatement:
        rv.handle(assume_sgtype<SgOmpDoStatement>(*n));
        break;

      case V_SgOmpSectionsStatement:
        rv.handle(assume_sgtype<SgOmpSectionsStatement>(*n));
        break;

      case V_SgOmpClauseBodyStatement:
        rv.handle(assume_sgtype<SgOmpClauseBodyStatement>(*n));
        break;

      case V_SgOmpAtomicStatement:
        rv.handle(assume_sgtype<SgOmpAtomicStatement>(*n));
        break;

      case V_SgOmpMasterStatement:
        rv.handle(assume_sgtype<SgOmpMasterStatement>(*n));
        break;

      case V_SgOmpSectionStatement:
        rv.handle(assume_sgtype<SgOmpSectionStatement>(*n));
        break;

      case V_SgOmpOrderedStatement:
        rv.handle(assume_sgtype<SgOmpOrderedStatement>(*n));
        break;

      case V_SgOmpWorkshareStatement:
        rv.handle(assume_sgtype<SgOmpWorkshareStatement>(*n));
        break;

      case V_SgOmpCriticalStatement:
        rv.handle(assume_sgtype<SgOmpCriticalStatement>(*n));
        break;

      case V_SgOmpBodyStatement:
        rv.handle(assume_sgtype<SgOmpBodyStatement>(*n));
        break;

      case V_SgBlockDataStatement:
        rv.handle(assume_sgtype<SgBlockDataStatement>(*n));
        break;

      case V_SgImplicitStatement:
        rv.handle(assume_sgtype<SgImplicitStatement>(*n));
        break;

      case V_SgStatementFunctionStatement:
        rv.handle(assume_sgtype<SgStatementFunctionStatement>(*n));
        break;

      case V_SgWhereStatement:
        rv.handle(assume_sgtype<SgWhereStatement>(*n));
        break;

      case V_SgNullifyStatement:
        rv.handle(assume_sgtype<SgNullifyStatement>(*n));
        break;

      case V_SgEquivalenceStatement:
        rv.handle(assume_sgtype<SgEquivalenceStatement>(*n));
        break;

      case V_SgDerivedTypeStatement:
        rv.handle(assume_sgtype<SgDerivedTypeStatement>(*n));
        break;

      case V_SgAttributeSpecificationStatement:
        rv.handle(assume_sgtype<SgAttributeSpecificationStatement>(*n));
        break;

      case V_SgAllocateStatement:
        rv.handle(assume_sgtype<SgAllocateStatement>(*n));
        break;

      case V_SgDeallocateStatement:
        rv.handle(assume_sgtype<SgDeallocateStatement>(*n));
        break;

      case V_SgContainsStatement:
        rv.handle(assume_sgtype<SgContainsStatement>(*n));
        break;

      case V_SgSequenceStatement:
        rv.handle(assume_sgtype<SgSequenceStatement>(*n));
        break;

      case V_SgElseWhereStatement:
        rv.handle(assume_sgtype<SgElseWhereStatement>(*n));
        break;

      case V_SgArithmeticIfStatement:
        rv.handle(assume_sgtype<SgArithmeticIfStatement>(*n));
        break;

      case V_SgAssignStatement:
        rv.handle(assume_sgtype<SgAssignStatement>(*n));
        break;

      case V_SgComputedGotoStatement:
        rv.handle(assume_sgtype<SgComputedGotoStatement>(*n));
        break;

      case V_SgAssignedGotoStatement:
        rv.handle(assume_sgtype<SgAssignedGotoStatement>(*n));
        break;

      case V_SgNamelistStatement:
        rv.handle(assume_sgtype<SgNamelistStatement>(*n));
        break;

      case V_SgImportStatement:
        rv.handle(assume_sgtype<SgImportStatement>(*n));
        break;

      case V_SgAssociateStatement:
        rv.handle(assume_sgtype<SgAssociateStatement>(*n));
        break;

      case V_SgFortranIncludeLine:
        rv.handle(assume_sgtype<SgFortranIncludeLine>(*n));
        break;

      case V_SgNamespaceDeclarationStatement:
        rv.handle(assume_sgtype<SgNamespaceDeclarationStatement>(*n));
        break;

      case V_SgNamespaceAliasDeclarationStatement:
        rv.handle(assume_sgtype<SgNamespaceAliasDeclarationStatement>(*n));
        break;

      case V_SgNamespaceDefinitionStatement:
        rv.handle(assume_sgtype<SgNamespaceDefinitionStatement>(*n));
        break;

      case V_SgUsingDeclarationStatement:
        rv.handle(assume_sgtype<SgUsingDeclarationStatement>(*n));
        break;

      case V_SgUsingDirectiveStatement:
        rv.handle(assume_sgtype<SgUsingDirectiveStatement>(*n));
        break;

      case V_SgTemplateInstantiationDirectiveStatement:
        rv.handle(assume_sgtype<SgTemplateInstantiationDirectiveStatement>(*n));
        break;

      case V_SgClassDeclaration:
        rv.handle(assume_sgtype<SgClassDeclaration>(*n));
        break;

      case V_SgClassDefinition:
        rv.handle(assume_sgtype<SgClassDefinition>(*n));
        break;

      case V_SgScopeStatement:
        rv.handle(assume_sgtype<SgScopeStatement>(*n));
        break;

      case V_SgMemberFunctionDeclaration:
        rv.handle(assume_sgtype<SgMemberFunctionDeclaration>(*n));
        break;

      case V_SgFunctionDeclaration:
        rv.handle(assume_sgtype<SgFunctionDeclaration>(*n));
        break;

      case V_SgIncludeDirectiveStatement:
        rv.handle(assume_sgtype<SgIncludeDirectiveStatement>(*n));
        break;

      case V_SgDefineDirectiveStatement:
        rv.handle(assume_sgtype<SgDefineDirectiveStatement>(*n));
        break;

      case V_SgUndefDirectiveStatement:
        rv.handle(assume_sgtype<SgUndefDirectiveStatement>(*n));
        break;

      case V_SgIfdefDirectiveStatement:
        rv.handle(assume_sgtype<SgIfdefDirectiveStatement>(*n));
        break;

      case V_SgIfndefDirectiveStatement:
        rv.handle(assume_sgtype<SgIfndefDirectiveStatement>(*n));
        break;

      case V_SgIfDirectiveStatement:
        rv.handle(assume_sgtype<SgIfDirectiveStatement>(*n));
        break;

      case V_SgDeadIfDirectiveStatement:
        rv.handle(assume_sgtype<SgDeadIfDirectiveStatement>(*n));
        break;

      case V_SgElseDirectiveStatement:
        rv.handle(assume_sgtype<SgElseDirectiveStatement>(*n));
        break;

      case V_SgElseifDirectiveStatement:
        rv.handle(assume_sgtype<SgElseifDirectiveStatement>(*n));
        break;

      case V_SgEndifDirectiveStatement:
        rv.handle(assume_sgtype<SgEndifDirectiveStatement>(*n));
        break;

      case V_SgLineDirectiveStatement:
        rv.handle(assume_sgtype<SgLineDirectiveStatement>(*n));
        break;

      case V_SgWarningDirectiveStatement:
        rv.handle(assume_sgtype<SgWarningDirectiveStatement>(*n));
        break;

      case V_SgErrorDirectiveStatement:
        rv.handle(assume_sgtype<SgErrorDirectiveStatement>(*n));
        break;

      case V_SgEmptyDirectiveStatement:
        rv.handle(assume_sgtype<SgEmptyDirectiveStatement>(*n));
        break;

      case V_SgIncludeNextDirectiveStatement:
        rv.handle(assume_sgtype<SgIncludeNextDirectiveStatement>(*n));
        break;

      case V_SgIdentDirectiveStatement:
        rv.handle(assume_sgtype<SgIdentDirectiveStatement>(*n));
        break;

      case V_SgLinemarkerDirectiveStatement:
        rv.handle(assume_sgtype<SgLinemarkerDirectiveStatement>(*n));
        break;

      case V_SgC_PreprocessorDirectiveStatement:
        rv.handle(assume_sgtype<SgC_PreprocessorDirectiveStatement>(*n));
        break;

      case V_SgClinkageStartStatement:
        rv.handle(assume_sgtype<SgClinkageStartStatement>(*n));
        break;

      case V_SgClinkageEndStatement:
        rv.handle(assume_sgtype<SgClinkageEndStatement>(*n));
        break;

      case V_SgClinkageDeclarationStatement:
        rv.handle(assume_sgtype<SgClinkageDeclarationStatement>(*n));
        break;

      case V_SgOmpFlushStatement:
        rv.handle(assume_sgtype<SgOmpFlushStatement>(*n));
        break;

      case V_SgOmpBarrierStatement:
        rv.handle(assume_sgtype<SgOmpBarrierStatement>(*n));
        break;

      case V_SgOmpTaskwaitStatement:
        rv.handle(assume_sgtype<SgOmpTaskwaitStatement>(*n));
        break;

      case V_SgOmpThreadprivateStatement:
        rv.handle(assume_sgtype<SgOmpThreadprivateStatement>(*n));
        break;

      case V_SgDeclarationStatement:
        rv.handle(assume_sgtype<SgDeclarationStatement>(*n));
        break;

      case V_SgStatement:
        rv.handle(assume_sgtype<SgStatement>(*n));
        break;

      case V_SgExprListExp:
        rv.handle(assume_sgtype<SgExprListExp>(*n));
        break;

      case V_SgVarRefExp:
        rv.handle(assume_sgtype<SgVarRefExp>(*n));
        break;

      case V_SgLabelRefExp:
        rv.handle(assume_sgtype<SgLabelRefExp>(*n));
        break;

      case V_SgClassNameRefExp:
        rv.handle(assume_sgtype<SgClassNameRefExp>(*n));
        break;

      case V_SgFunctionRefExp:
        rv.handle(assume_sgtype<SgFunctionRefExp>(*n));
        break;

      case V_SgMemberFunctionRefExp:
        rv.handle(assume_sgtype<SgMemberFunctionRefExp>(*n));
        break;

      case V_SgFunctionCallExp:
        rv.handle(assume_sgtype<SgFunctionCallExp>(*n));
        break;

      case V_SgSizeOfOp:
        rv.handle(assume_sgtype<SgSizeOfOp>(*n));
        break;

      case V_SgUpcLocalsizeofExpression:
        rv.handle(assume_sgtype<SgUpcLocalsizeofExpression>(*n));
        break;

      case V_SgUpcBlocksizeofExpression:
        rv.handle(assume_sgtype<SgUpcBlocksizeofExpression>(*n));
        break;

      case V_SgUpcElemsizeofExpression:
        rv.handle(assume_sgtype<SgUpcElemsizeofExpression>(*n));
        break;

      case V_SgVarArgStartOp:
        rv.handle(assume_sgtype<SgVarArgStartOp>(*n));
        break;

      case V_SgVarArgStartOneOperandOp:
        rv.handle(assume_sgtype<SgVarArgStartOneOperandOp>(*n));
        break;

      case V_SgVarArgOp:
        rv.handle(assume_sgtype<SgVarArgOp>(*n));
        break;

      case V_SgVarArgEndOp:
        rv.handle(assume_sgtype<SgVarArgEndOp>(*n));
        break;

      case V_SgVarArgCopyOp:
        rv.handle(assume_sgtype<SgVarArgCopyOp>(*n));
        break;

      case V_SgTypeIdOp:
        rv.handle(assume_sgtype<SgTypeIdOp>(*n));
        break;

      case V_SgConditionalExp:
        rv.handle(assume_sgtype<SgConditionalExp>(*n));
        break;

      case V_SgNewExp:
        rv.handle(assume_sgtype<SgNewExp>(*n));
        break;

      case V_SgDeleteExp:
        rv.handle(assume_sgtype<SgDeleteExp>(*n));
        break;

      case V_SgThisExp:
        rv.handle(assume_sgtype<SgThisExp>(*n));
        break;

      case V_SgRefExp:
        rv.handle(assume_sgtype<SgRefExp>(*n));
        break;

      case V_SgAggregateInitializer:
        rv.handle(assume_sgtype<SgAggregateInitializer>(*n));
        break;

      case V_SgConstructorInitializer:
        rv.handle(assume_sgtype<SgConstructorInitializer>(*n));
        break;

      case V_SgAssignInitializer:
        rv.handle(assume_sgtype<SgAssignInitializer>(*n));
        break;

      case V_SgExpressionRoot:
        rv.handle(assume_sgtype<SgExpressionRoot>(*n));
        break;

      case V_SgMinusOp:
        rv.handle(assume_sgtype<SgMinusOp>(*n));
        break;

      case V_SgUnaryAddOp:
        rv.handle(assume_sgtype<SgUnaryAddOp>(*n));
        break;

      case V_SgNotOp:
        rv.handle(assume_sgtype<SgNotOp>(*n));
        break;

      case V_SgPointerDerefExp:
        rv.handle(assume_sgtype<SgPointerDerefExp>(*n));
        break;

      case V_SgAddressOfOp:
        rv.handle(assume_sgtype<SgAddressOfOp>(*n));
        break;

      case V_SgMinusMinusOp:
        rv.handle(assume_sgtype<SgMinusMinusOp>(*n));
        break;

      case V_SgPlusPlusOp:
        rv.handle(assume_sgtype<SgPlusPlusOp>(*n));
        break;

      case V_SgBitComplementOp:
        rv.handle(assume_sgtype<SgBitComplementOp>(*n));
        break;

      case V_SgRealPartOp:
        rv.handle(assume_sgtype<SgRealPartOp>(*n));
        break;

      case V_SgImagPartOp:
        rv.handle(assume_sgtype<SgImagPartOp>(*n));
        break;

      case V_SgConjugateOp:
        rv.handle(assume_sgtype<SgConjugateOp>(*n));
        break;

      case V_SgCastExp:
        rv.handle(assume_sgtype<SgCastExp>(*n));
        break;

      case V_SgThrowOp:
        rv.handle(assume_sgtype<SgThrowOp>(*n));
        break;

      case V_SgArrowExp:
        rv.handle(assume_sgtype<SgArrowExp>(*n));
        break;

      case V_SgDotExp:
        rv.handle(assume_sgtype<SgDotExp>(*n));
        break;

      case V_SgDotStarOp:
        rv.handle(assume_sgtype<SgDotStarOp>(*n));
        break;

      case V_SgArrowStarOp:
        rv.handle(assume_sgtype<SgArrowStarOp>(*n));
        break;

      case V_SgEqualityOp:
        rv.handle(assume_sgtype<SgEqualityOp>(*n));
        break;

      case V_SgLessThanOp:
        rv.handle(assume_sgtype<SgLessThanOp>(*n));
        break;

      case V_SgGreaterThanOp:
        rv.handle(assume_sgtype<SgGreaterThanOp>(*n));
        break;

      case V_SgNotEqualOp:
        rv.handle(assume_sgtype<SgNotEqualOp>(*n));
        break;

      case V_SgLessOrEqualOp:
        rv.handle(assume_sgtype<SgLessOrEqualOp>(*n));
        break;

      case V_SgGreaterOrEqualOp:
        rv.handle(assume_sgtype<SgGreaterOrEqualOp>(*n));
        break;

      case V_SgAddOp:
        rv.handle(assume_sgtype<SgAddOp>(*n));
        break;

      case V_SgSubtractOp:
        rv.handle(assume_sgtype<SgSubtractOp>(*n));
        break;

      case V_SgMultiplyOp:
        rv.handle(assume_sgtype<SgMultiplyOp>(*n));
        break;

      case V_SgDivideOp:
        rv.handle(assume_sgtype<SgDivideOp>(*n));
        break;

      case V_SgIntegerDivideOp:
        rv.handle(assume_sgtype<SgIntegerDivideOp>(*n));
        break;

      case V_SgModOp:
        rv.handle(assume_sgtype<SgModOp>(*n));
        break;

      case V_SgAndOp:
        rv.handle(assume_sgtype<SgAndOp>(*n));
        break;

      case V_SgOrOp:
        rv.handle(assume_sgtype<SgOrOp>(*n));
        break;

      case V_SgBitXorOp:
        rv.handle(assume_sgtype<SgBitXorOp>(*n));
        break;

      case V_SgBitAndOp:
        rv.handle(assume_sgtype<SgBitAndOp>(*n));
        break;

      case V_SgBitOrOp:
        rv.handle(assume_sgtype<SgBitOrOp>(*n));
        break;

      case V_SgCommaOpExp:
        rv.handle(assume_sgtype<SgCommaOpExp>(*n));
        break;

      case V_SgLshiftOp:
        rv.handle(assume_sgtype<SgLshiftOp>(*n));
        break;

      case V_SgRshiftOp:
        rv.handle(assume_sgtype<SgRshiftOp>(*n));
        break;

      case V_SgPntrArrRefExp:
        rv.handle(assume_sgtype<SgPntrArrRefExp>(*n));
        break;

      case V_SgScopeOp:
        rv.handle(assume_sgtype<SgScopeOp>(*n));
        break;

      case V_SgAssignOp:
        rv.handle(assume_sgtype<SgAssignOp>(*n));
        break;

      case V_SgPlusAssignOp:
        rv.handle(assume_sgtype<SgPlusAssignOp>(*n));
        break;

      case V_SgMinusAssignOp:
        rv.handle(assume_sgtype<SgMinusAssignOp>(*n));
        break;

      case V_SgAndAssignOp:
        rv.handle(assume_sgtype<SgAndAssignOp>(*n));
        break;

      case V_SgIorAssignOp:
        rv.handle(assume_sgtype<SgIorAssignOp>(*n));
        break;

      case V_SgMultAssignOp:
        rv.handle(assume_sgtype<SgMultAssignOp>(*n));
        break;

      case V_SgDivAssignOp:
        rv.handle(assume_sgtype<SgDivAssignOp>(*n));
        break;

      case V_SgModAssignOp:
        rv.handle(assume_sgtype<SgModAssignOp>(*n));
        break;

      case V_SgXorAssignOp:
        rv.handle(assume_sgtype<SgXorAssignOp>(*n));
        break;

      case V_SgLshiftAssignOp:
        rv.handle(assume_sgtype<SgLshiftAssignOp>(*n));
        break;

      case V_SgRshiftAssignOp:
        rv.handle(assume_sgtype<SgRshiftAssignOp>(*n));
        break;

      case V_SgConcatenationOp:
        rv.handle(assume_sgtype<SgConcatenationOp>(*n));
        break;

      case V_SgBoolValExp:
        rv.handle(assume_sgtype<SgBoolValExp>(*n));
        break;

      case V_SgStringVal:
        rv.handle(assume_sgtype<SgStringVal>(*n));
        break;

      case V_SgShortVal:
        rv.handle(assume_sgtype<SgShortVal>(*n));
        break;

      case V_SgCharVal:
        rv.handle(assume_sgtype<SgCharVal>(*n));
        break;

      case V_SgUnsignedCharVal:
        rv.handle(assume_sgtype<SgUnsignedCharVal>(*n));
        break;

      case V_SgWcharVal:
        rv.handle(assume_sgtype<SgWcharVal>(*n));
        break;

      case V_SgUnsignedShortVal:
        rv.handle(assume_sgtype<SgUnsignedShortVal>(*n));
        break;

      case V_SgIntVal:
        rv.handle(assume_sgtype<SgIntVal>(*n));
        break;

      case V_SgEnumVal:
        rv.handle(assume_sgtype<SgEnumVal>(*n));
        break;

      case V_SgUnsignedIntVal:
        rv.handle(assume_sgtype<SgUnsignedIntVal>(*n));
        break;

      case V_SgLongIntVal:
        rv.handle(assume_sgtype<SgLongIntVal>(*n));
        break;

      case V_SgLongLongIntVal:
        rv.handle(assume_sgtype<SgLongLongIntVal>(*n));
        break;

      case V_SgUnsignedLongLongIntVal:
        rv.handle(assume_sgtype<SgUnsignedLongLongIntVal>(*n));
        break;

      case V_SgUnsignedLongVal:
        rv.handle(assume_sgtype<SgUnsignedLongVal>(*n));
        break;

      case V_SgFloatVal:
        rv.handle(assume_sgtype<SgFloatVal>(*n));
        break;

      case V_SgDoubleVal:
        rv.handle(assume_sgtype<SgDoubleVal>(*n));
        break;

      case V_SgLongDoubleVal:
        rv.handle(assume_sgtype<SgLongDoubleVal>(*n));
        break;

      case V_SgUpcThreads:
        rv.handle(assume_sgtype<SgUpcThreads>(*n));
        break;

      case V_SgUpcMythread:
        rv.handle(assume_sgtype<SgUpcMythread>(*n));
        break;

      case V_SgComplexVal:
        rv.handle(assume_sgtype<SgComplexVal>(*n));
        break;

      case V_SgNullExpression:
        rv.handle(assume_sgtype<SgNullExpression>(*n));
        break;

      case V_SgVariantExpression:
        rv.handle(assume_sgtype<SgVariantExpression>(*n));
        break;

      case V_SgStatementExpression:
        rv.handle(assume_sgtype<SgStatementExpression>(*n));
        break;

      case V_SgAsmOp:
        rv.handle(assume_sgtype<SgAsmOp>(*n));
        break;

      case V_SgCudaKernelExecConfig:
        rv.handle(assume_sgtype<SgCudaKernelExecConfig>(*n));
        break;

      case V_SgCudaKernelCallExp:
        rv.handle(assume_sgtype<SgCudaKernelCallExp>(*n));
        break;

      case V_SgSubscriptExpression:
        rv.handle(assume_sgtype<SgSubscriptExpression>(*n));
        break;

      case V_SgColonShapeExp:
        rv.handle(assume_sgtype<SgColonShapeExp>(*n));
        break;

      case V_SgAsteriskShapeExp:
        rv.handle(assume_sgtype<SgAsteriskShapeExp>(*n));
        break;

      case V_SgIOItemExpression:
        rv.handle(assume_sgtype<SgIOItemExpression>(*n));
        break;

      case V_SgImpliedDo:
        rv.handle(assume_sgtype<SgImpliedDo>(*n));
        break;

      case V_SgExponentiationOp:
        rv.handle(assume_sgtype<SgExponentiationOp>(*n));
        break;

      case V_SgUnknownArrayOrFunctionReference:
        rv.handle(assume_sgtype<SgUnknownArrayOrFunctionReference>(*n));
        break;

      case V_SgActualArgumentExpression:
        rv.handle(assume_sgtype<SgActualArgumentExpression>(*n));
        break;

      case V_SgUserDefinedBinaryOp:
        rv.handle(assume_sgtype<SgUserDefinedBinaryOp>(*n));
        break;

      case V_SgPointerAssignOp:
        rv.handle(assume_sgtype<SgPointerAssignOp>(*n));
        break;

      case V_SgCAFCoExpression:
        rv.handle(assume_sgtype<SgCAFCoExpression>(*n));
        break;

      case V_SgDesignatedInitializer:
        rv.handle(assume_sgtype<SgDesignatedInitializer>(*n));
        break;

      case V_SgInitializer:
        rv.handle(assume_sgtype<SgInitializer>(*n));
        break;

      case V_SgUserDefinedUnaryOp:
        rv.handle(assume_sgtype<SgUserDefinedUnaryOp>(*n));
        break;

      case V_SgPseudoDestructorRefExp:
        rv.handle(assume_sgtype<SgPseudoDestructorRefExp>(*n));
        break;

      case V_SgUnaryOp:
        rv.handle(assume_sgtype<SgUnaryOp>(*n));
        break;

      case V_SgBinaryOp:
        rv.handle(assume_sgtype<SgBinaryOp>(*n));
        break;

      case V_SgValueExp:
        rv.handle(assume_sgtype<SgValueExp>(*n));
        break;

      case V_SgExpression:
        rv.handle(assume_sgtype<SgExpression>(*n));
        break;

      case V_SgVariableSymbol:
        rv.handle(assume_sgtype<SgVariableSymbol>(*n));
        break;

      case V_SgFunctionTypeSymbol:
        rv.handle(assume_sgtype<SgFunctionTypeSymbol>(*n));
        break;

      case V_SgClassSymbol:
        rv.handle(assume_sgtype<SgClassSymbol>(*n));
        break;

      case V_SgTemplateSymbol:
        rv.handle(assume_sgtype<SgTemplateSymbol>(*n));
        break;

      case V_SgEnumSymbol:
        rv.handle(assume_sgtype<SgEnumSymbol>(*n));
        break;

      case V_SgEnumFieldSymbol:
        rv.handle(assume_sgtype<SgEnumFieldSymbol>(*n));
        break;

      case V_SgTypedefSymbol:
        rv.handle(assume_sgtype<SgTypedefSymbol>(*n));
        break;

      case V_SgMemberFunctionSymbol:
        rv.handle(assume_sgtype<SgMemberFunctionSymbol>(*n));
        break;

      case V_SgLabelSymbol:
        rv.handle(assume_sgtype<SgLabelSymbol>(*n));
        break;

      case V_SgDefaultSymbol:
        rv.handle(assume_sgtype<SgDefaultSymbol>(*n));
        break;

      case V_SgNamespaceSymbol:
        rv.handle(assume_sgtype<SgNamespaceSymbol>(*n));
        break;

      case V_SgIntrinsicSymbol:
        rv.handle(assume_sgtype<SgIntrinsicSymbol>(*n));
        break;

      case V_SgModuleSymbol:
        rv.handle(assume_sgtype<SgModuleSymbol>(*n));
        break;

      case V_SgInterfaceSymbol:
        rv.handle(assume_sgtype<SgInterfaceSymbol>(*n));
        break;

      case V_SgCommonSymbol:
        rv.handle(assume_sgtype<SgCommonSymbol>(*n));
        break;

      case V_SgRenameSymbol:
        rv.handle(assume_sgtype<SgRenameSymbol>(*n));
        break;

      case V_SgFunctionSymbol:
        rv.handle(assume_sgtype<SgFunctionSymbol>(*n));
        break;

      case V_SgAsmBinaryAddressSymbol:
        rv.handle(assume_sgtype<SgAsmBinaryAddressSymbol>(*n));
        break;

      case V_SgAsmBinaryDataSymbol:
        rv.handle(assume_sgtype<SgAsmBinaryDataSymbol>(*n));
        break;

      case V_SgAliasSymbol:
        rv.handle(assume_sgtype<SgAliasSymbol>(*n));
        break;

      case V_SgSymbol:
        rv.handle(assume_sgtype<SgSymbol>(*n));
        break;

      case V_SgAsmBlock:
        rv.handle(assume_sgtype<SgAsmBlock>(*n));
        break;

      case V_SgAsmOperandList:
        rv.handle(assume_sgtype<SgAsmOperandList>(*n));
        break;

      case V_SgAsmArmInstruction:
        rv.handle(assume_sgtype<SgAsmArmInstruction>(*n));
        break;

      case V_SgAsmx86Instruction:
        rv.handle(assume_sgtype<SgAsmx86Instruction>(*n));
        break;

      case V_SgAsmPowerpcInstruction:
        rv.handle(assume_sgtype<SgAsmPowerpcInstruction>(*n));
        break;

      case V_SgAsmInstruction:
        rv.handle(assume_sgtype<SgAsmInstruction>(*n));
        break;

      case V_SgAsmDataStructureDeclaration:
        rv.handle(assume_sgtype<SgAsmDataStructureDeclaration>(*n));
        break;

      case V_SgAsmFunctionDeclaration:
        rv.handle(assume_sgtype<SgAsmFunctionDeclaration>(*n));
        break;

      case V_SgAsmFieldDeclaration:
        rv.handle(assume_sgtype<SgAsmFieldDeclaration>(*n));
        break;

      case V_SgAsmDeclaration:
        rv.handle(assume_sgtype<SgAsmDeclaration>(*n));
        break;

      case V_SgAsmStatement:
        rv.handle(assume_sgtype<SgAsmStatement>(*n));
        break;

      case V_SgAsmBinaryAdd:
        rv.handle(assume_sgtype<SgAsmBinaryAdd>(*n));
        break;

      case V_SgAsmBinarySubtract:
        rv.handle(assume_sgtype<SgAsmBinarySubtract>(*n));
        break;

      case V_SgAsmBinaryMultiply:
        rv.handle(assume_sgtype<SgAsmBinaryMultiply>(*n));
        break;

      case V_SgAsmBinaryDivide:
        rv.handle(assume_sgtype<SgAsmBinaryDivide>(*n));
        break;

      case V_SgAsmBinaryMod:
        rv.handle(assume_sgtype<SgAsmBinaryMod>(*n));
        break;

      case V_SgAsmBinaryAddPreupdate:
        rv.handle(assume_sgtype<SgAsmBinaryAddPreupdate>(*n));
        break;

      case V_SgAsmBinarySubtractPreupdate:
        rv.handle(assume_sgtype<SgAsmBinarySubtractPreupdate>(*n));
        break;

      case V_SgAsmBinaryAddPostupdate:
        rv.handle(assume_sgtype<SgAsmBinaryAddPostupdate>(*n));
        break;

      case V_SgAsmBinarySubtractPostupdate:
        rv.handle(assume_sgtype<SgAsmBinarySubtractPostupdate>(*n));
        break;

      case V_SgAsmBinaryLsl:
        rv.handle(assume_sgtype<SgAsmBinaryLsl>(*n));
        break;

      case V_SgAsmBinaryLsr:
        rv.handle(assume_sgtype<SgAsmBinaryLsr>(*n));
        break;

      case V_SgAsmBinaryAsr:
        rv.handle(assume_sgtype<SgAsmBinaryAsr>(*n));
        break;

      case V_SgAsmBinaryRor:
        rv.handle(assume_sgtype<SgAsmBinaryRor>(*n));
        break;

      case V_SgAsmBinaryExpression:
        rv.handle(assume_sgtype<SgAsmBinaryExpression>(*n));
        break;

      case V_SgAsmUnaryPlus:
        rv.handle(assume_sgtype<SgAsmUnaryPlus>(*n));
        break;

      case V_SgAsmUnaryMinus:
        rv.handle(assume_sgtype<SgAsmUnaryMinus>(*n));
        break;

      case V_SgAsmUnaryRrx:
        rv.handle(assume_sgtype<SgAsmUnaryRrx>(*n));
        break;

      case V_SgAsmUnaryArmSpecialRegisterList:
        rv.handle(assume_sgtype<SgAsmUnaryArmSpecialRegisterList>(*n));
        break;

      case V_SgAsmUnaryExpression:
        rv.handle(assume_sgtype<SgAsmUnaryExpression>(*n));
        break;

      case V_SgAsmMemoryReferenceExpression:
        rv.handle(assume_sgtype<SgAsmMemoryReferenceExpression>(*n));
        break;

      case V_SgAsmControlFlagsExpression:
        rv.handle(assume_sgtype<SgAsmControlFlagsExpression>(*n));
        break;

      case V_SgAsmCommonSubExpression:
        rv.handle(assume_sgtype<SgAsmCommonSubExpression>(*n));
        break;

      case V_SgAsmx86RegisterReferenceExpression:
        rv.handle(assume_sgtype<SgAsmx86RegisterReferenceExpression>(*n));
        break;

      case V_SgAsmArmRegisterReferenceExpression:
        rv.handle(assume_sgtype<SgAsmArmRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmPowerpcRegisterReferenceExpression:
        rv.handle(assume_sgtype<SgAsmPowerpcRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmRegisterReferenceExpression:
        rv.handle(assume_sgtype<SgAsmRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmByteValueExpression:
        rv.handle(assume_sgtype<SgAsmByteValueExpression>(*n));
        break;

      case V_SgAsmWordValueExpression:
        rv.handle(assume_sgtype<SgAsmWordValueExpression>(*n));
        break;

      case V_SgAsmDoubleWordValueExpression:
        rv.handle(assume_sgtype<SgAsmDoubleWordValueExpression>(*n));
        break;

      case V_SgAsmQuadWordValueExpression:
        rv.handle(assume_sgtype<SgAsmQuadWordValueExpression>(*n));
        break;

      case V_SgAsmSingleFloatValueExpression:
        rv.handle(assume_sgtype<SgAsmSingleFloatValueExpression>(*n));
        break;

      case V_SgAsmDoubleFloatValueExpression:
        rv.handle(assume_sgtype<SgAsmDoubleFloatValueExpression>(*n));
        break;

      case V_SgAsmVectorValueExpression:
        rv.handle(assume_sgtype<SgAsmVectorValueExpression>(*n));
        break;

      case V_SgAsmValueExpression:
        rv.handle(assume_sgtype<SgAsmValueExpression>(*n));
        break;

      case V_SgAsmExprListExp:
        rv.handle(assume_sgtype<SgAsmExprListExp>(*n));
        break;

      case V_SgAsmExpression:
        rv.handle(assume_sgtype<SgAsmExpression>(*n));
        break;

      case V_SgAsmTypeByte:
        rv.handle(assume_sgtype<SgAsmTypeByte>(*n));
        break;

      case V_SgAsmTypeWord:
        rv.handle(assume_sgtype<SgAsmTypeWord>(*n));
        break;

      case V_SgAsmTypeDoubleWord:
        rv.handle(assume_sgtype<SgAsmTypeDoubleWord>(*n));
        break;

      case V_SgAsmTypeQuadWord:
        rv.handle(assume_sgtype<SgAsmTypeQuadWord>(*n));
        break;

      case V_SgAsmTypeDoubleQuadWord:
        rv.handle(assume_sgtype<SgAsmTypeDoubleQuadWord>(*n));
        break;

      case V_SgAsmTypeSingleFloat:
        rv.handle(assume_sgtype<SgAsmTypeSingleFloat>(*n));
        break;

      case V_SgAsmTypeDoubleFloat:
        rv.handle(assume_sgtype<SgAsmTypeDoubleFloat>(*n));
        break;

      case V_SgAsmType80bitFloat:
        rv.handle(assume_sgtype<SgAsmType80bitFloat>(*n));
        break;

      case V_SgAsmType128bitFloat:
        rv.handle(assume_sgtype<SgAsmType128bitFloat>(*n));
        break;

      case V_SgAsmTypeVector:
        rv.handle(assume_sgtype<SgAsmTypeVector>(*n));
        break;

      case V_SgAsmType:
        rv.handle(assume_sgtype<SgAsmType>(*n));
        break;

      case V_SgAsmGenericDLL:
        rv.handle(assume_sgtype<SgAsmGenericDLL>(*n));
        break;

      case V_SgAsmPEImportHNTEntryList:
        rv.handle(assume_sgtype<SgAsmPEImportHNTEntryList>(*n));
        break;

      case V_SgAsmPEImportILTEntryList:
        rv.handle(assume_sgtype<SgAsmPEImportILTEntryList>(*n));
        break;

      case V_SgAsmPEImportDirectoryList:
        rv.handle(assume_sgtype<SgAsmPEImportDirectoryList>(*n));
        break;

      case V_SgAsmGenericFormat:
        rv.handle(assume_sgtype<SgAsmGenericFormat>(*n));
        break;

      case V_SgAsmGenericFile:
        rv.handle(assume_sgtype<SgAsmGenericFile>(*n));
        break;

      case V_SgAsmElfFileHeader:
        rv.handle(assume_sgtype<SgAsmElfFileHeader>(*n));
        break;

      case V_SgAsmPEFileHeader:
        rv.handle(assume_sgtype<SgAsmPEFileHeader>(*n));
        break;

      case V_SgAsmNEFileHeader:
        rv.handle(assume_sgtype<SgAsmNEFileHeader>(*n));
        break;

      case V_SgAsmLEFileHeader:
        rv.handle(assume_sgtype<SgAsmLEFileHeader>(*n));
        break;

      case V_SgAsmDOSFileHeader:
        rv.handle(assume_sgtype<SgAsmDOSFileHeader>(*n));
        break;

      case V_SgAsmGenericHeader:
        rv.handle(assume_sgtype<SgAsmGenericHeader>(*n));
        break;

      case V_SgAsmElfRelocSection:
        rv.handle(assume_sgtype<SgAsmElfRelocSection>(*n));
        break;

      case V_SgAsmElfDynamicSection:
        rv.handle(assume_sgtype<SgAsmElfDynamicSection>(*n));
        break;

      case V_SgAsmElfSymbolSection:
        rv.handle(assume_sgtype<SgAsmElfSymbolSection>(*n));
        break;

      case V_SgAsmElfStringSection:
        rv.handle(assume_sgtype<SgAsmElfStringSection>(*n));
        break;

      case V_SgAsmElfEHFrameSection:
        rv.handle(assume_sgtype<SgAsmElfEHFrameSection>(*n));
        break;

      case V_SgAsmElfNoteSection:
        rv.handle(assume_sgtype<SgAsmElfNoteSection>(*n));
        break;

      case V_SgAsmElfSymverSection:
        rv.handle(assume_sgtype<SgAsmElfSymverSection>(*n));
        break;

      case V_SgAsmElfSymverDefinedSection:
        rv.handle(assume_sgtype<SgAsmElfSymverDefinedSection>(*n));
        break;

      case V_SgAsmElfSymverNeededSection:
        rv.handle(assume_sgtype<SgAsmElfSymverNeededSection>(*n));
        break;

      case V_SgAsmElfStrtab:
        rv.handle(assume_sgtype<SgAsmElfStrtab>(*n));
        break;

      case V_SgAsmCoffStrtab:
        rv.handle(assume_sgtype<SgAsmCoffStrtab>(*n));
        break;

      case V_SgAsmGenericStrtab:
        rv.handle(assume_sgtype<SgAsmGenericStrtab>(*n));
        break;

      case V_SgAsmElfSection:
        rv.handle(assume_sgtype<SgAsmElfSection>(*n));
        break;

      case V_SgAsmElfSectionTable:
        rv.handle(assume_sgtype<SgAsmElfSectionTable>(*n));
        break;

      case V_SgAsmElfSegmentTable:
        rv.handle(assume_sgtype<SgAsmElfSegmentTable>(*n));
        break;

      case V_SgAsmPEImportSection:
        rv.handle(assume_sgtype<SgAsmPEImportSection>(*n));
        break;

      case V_SgAsmPEExportSection:
        rv.handle(assume_sgtype<SgAsmPEExportSection>(*n));
        break;

      case V_SgAsmPEStringSection:
        rv.handle(assume_sgtype<SgAsmPEStringSection>(*n));
        break;

      case V_SgAsmPESection:
        rv.handle(assume_sgtype<SgAsmPESection>(*n));
        break;

      case V_SgAsmPESectionTable:
        rv.handle(assume_sgtype<SgAsmPESectionTable>(*n));
        break;

      case V_SgAsmCoffSymbolTable:
        rv.handle(assume_sgtype<SgAsmCoffSymbolTable>(*n));
        break;

      case V_SgAsmDOSExtendedHeader:
        rv.handle(assume_sgtype<SgAsmDOSExtendedHeader>(*n));
        break;

      case V_SgAsmNESection:
        rv.handle(assume_sgtype<SgAsmNESection>(*n));
        break;

      case V_SgAsmNESectionTable:
        rv.handle(assume_sgtype<SgAsmNESectionTable>(*n));
        break;

      case V_SgAsmNENameTable:
        rv.handle(assume_sgtype<SgAsmNENameTable>(*n));
        break;

      case V_SgAsmNEModuleTable:
        rv.handle(assume_sgtype<SgAsmNEModuleTable>(*n));
        break;

      case V_SgAsmNEStringTable:
        rv.handle(assume_sgtype<SgAsmNEStringTable>(*n));
        break;

      case V_SgAsmNEEntryTable:
        rv.handle(assume_sgtype<SgAsmNEEntryTable>(*n));
        break;

      case V_SgAsmNERelocTable:
        rv.handle(assume_sgtype<SgAsmNERelocTable>(*n));
        break;

      case V_SgAsmLESection:
        rv.handle(assume_sgtype<SgAsmLESection>(*n));
        break;

      case V_SgAsmLESectionTable:
        rv.handle(assume_sgtype<SgAsmLESectionTable>(*n));
        break;

      case V_SgAsmLENameTable:
        rv.handle(assume_sgtype<SgAsmLENameTable>(*n));
        break;

      case V_SgAsmLEPageTable:
        rv.handle(assume_sgtype<SgAsmLEPageTable>(*n));
        break;

      case V_SgAsmLEEntryTable:
        rv.handle(assume_sgtype<SgAsmLEEntryTable>(*n));
        break;

      case V_SgAsmLERelocTable:
        rv.handle(assume_sgtype<SgAsmLERelocTable>(*n));
        break;

      case V_SgAsmGenericSection:
        rv.handle(assume_sgtype<SgAsmGenericSection>(*n));
        break;

      case V_SgAsmCoffSymbol:
        rv.handle(assume_sgtype<SgAsmCoffSymbol>(*n));
        break;

      case V_SgAsmElfSymbol:
        rv.handle(assume_sgtype<SgAsmElfSymbol>(*n));
        break;

      case V_SgAsmGenericSymbol:
        rv.handle(assume_sgtype<SgAsmGenericSymbol>(*n));
        break;

      case V_SgAsmElfSectionTableEntry:
        rv.handle(assume_sgtype<SgAsmElfSectionTableEntry>(*n));
        break;

      case V_SgAsmElfSegmentTableEntry:
        rv.handle(assume_sgtype<SgAsmElfSegmentTableEntry>(*n));
        break;

      case V_SgAsmElfSegmentTableEntryList:
        rv.handle(assume_sgtype<SgAsmElfSegmentTableEntryList>(*n));
        break;

      case V_SgAsmElfRelocEntry:
        rv.handle(assume_sgtype<SgAsmElfRelocEntry>(*n));
        break;

      case V_SgAsmElfRelocEntryList:
        rv.handle(assume_sgtype<SgAsmElfRelocEntryList>(*n));
        break;

      case V_SgAsmElfDynamicEntry:
        rv.handle(assume_sgtype<SgAsmElfDynamicEntry>(*n));
        break;

      case V_SgAsmElfDynamicEntryList:
        rv.handle(assume_sgtype<SgAsmElfDynamicEntryList>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCI:
        rv.handle(assume_sgtype<SgAsmElfEHFrameEntryCI>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCIList:
        rv.handle(assume_sgtype<SgAsmElfEHFrameEntryCIList>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFD:
        rv.handle(assume_sgtype<SgAsmElfEHFrameEntryFD>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFDList:
        rv.handle(assume_sgtype<SgAsmElfEHFrameEntryFDList>(*n));
        break;

      case V_SgAsmElfNoteEntry:
        rv.handle(assume_sgtype<SgAsmElfNoteEntry>(*n));
        break;

      case V_SgAsmElfNoteEntryList:
        rv.handle(assume_sgtype<SgAsmElfNoteEntryList>(*n));
        break;

      case V_SgAsmElfSymverEntry:
        rv.handle(assume_sgtype<SgAsmElfSymverEntry>(*n));
        break;

      case V_SgAsmElfSymverEntryList:
        rv.handle(assume_sgtype<SgAsmElfSymverEntryList>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntry:
        rv.handle(assume_sgtype<SgAsmElfSymverDefinedEntry>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntryList:
        rv.handle(assume_sgtype<SgAsmElfSymverDefinedEntryList>(*n));
        break;

      case V_SgAsmElfSymverDefinedAux:
        rv.handle(assume_sgtype<SgAsmElfSymverDefinedAux>(*n));
        break;

      case V_SgAsmElfSymverDefinedAuxList:
        rv.handle(assume_sgtype<SgAsmElfSymverDefinedAuxList>(*n));
        break;

      case V_SgAsmElfSymverNeededEntry:
        rv.handle(assume_sgtype<SgAsmElfSymverNeededEntry>(*n));
        break;

      case V_SgAsmElfSymverNeededEntryList:
        rv.handle(assume_sgtype<SgAsmElfSymverNeededEntryList>(*n));
        break;

      case V_SgAsmElfSymverNeededAux:
        rv.handle(assume_sgtype<SgAsmElfSymverNeededAux>(*n));
        break;

      case V_SgAsmElfSymverNeededAuxList:
        rv.handle(assume_sgtype<SgAsmElfSymverNeededAuxList>(*n));
        break;

      case V_SgAsmPERVASizePair:
        rv.handle(assume_sgtype<SgAsmPERVASizePair>(*n));
        break;

      case V_SgAsmPEExportDirectory:
        rv.handle(assume_sgtype<SgAsmPEExportDirectory>(*n));
        break;

      case V_SgAsmPEExportEntry:
        rv.handle(assume_sgtype<SgAsmPEExportEntry>(*n));
        break;

      case V_SgAsmPEImportDirectory:
        rv.handle(assume_sgtype<SgAsmPEImportDirectory>(*n));
        break;

      case V_SgAsmPEImportILTEntry:
        rv.handle(assume_sgtype<SgAsmPEImportILTEntry>(*n));
        break;

      case V_SgAsmPEImportHNTEntry:
        rv.handle(assume_sgtype<SgAsmPEImportHNTEntry>(*n));
        break;

      case V_SgAsmPEImportLookupTable:
        rv.handle(assume_sgtype<SgAsmPEImportLookupTable>(*n));
        break;

      case V_SgAsmPESectionTableEntry:
        rv.handle(assume_sgtype<SgAsmPESectionTableEntry>(*n));
        break;

      case V_SgAsmNEEntryPoint:
        rv.handle(assume_sgtype<SgAsmNEEntryPoint>(*n));
        break;

      case V_SgAsmNERelocEntry:
        rv.handle(assume_sgtype<SgAsmNERelocEntry>(*n));
        break;

      case V_SgAsmNESectionTableEntry:
        rv.handle(assume_sgtype<SgAsmNESectionTableEntry>(*n));
        break;

      case V_SgAsmLEPageTableEntry:
        rv.handle(assume_sgtype<SgAsmLEPageTableEntry>(*n));
        break;

      case V_SgAsmLEEntryPoint:
        rv.handle(assume_sgtype<SgAsmLEEntryPoint>(*n));
        break;

      case V_SgAsmLESectionTableEntry:
        rv.handle(assume_sgtype<SgAsmLESectionTableEntry>(*n));
        break;

      case V_SgAsmGenericSectionList:
        rv.handle(assume_sgtype<SgAsmGenericSectionList>(*n));
        break;

      case V_SgAsmGenericHeaderList:
        rv.handle(assume_sgtype<SgAsmGenericHeaderList>(*n));
        break;

      case V_SgAsmGenericSymbolList:
        rv.handle(assume_sgtype<SgAsmGenericSymbolList>(*n));
        break;

      case V_SgAsmElfSymbolList:
        rv.handle(assume_sgtype<SgAsmElfSymbolList>(*n));
        break;

      case V_SgAsmCoffSymbolList:
        rv.handle(assume_sgtype<SgAsmCoffSymbolList>(*n));
        break;

      case V_SgAsmGenericDLLList:
        rv.handle(assume_sgtype<SgAsmGenericDLLList>(*n));
        break;

      case V_SgAsmPERVASizePairList:
        rv.handle(assume_sgtype<SgAsmPERVASizePairList>(*n));
        break;

      case V_SgAsmPEExportEntryList:
        rv.handle(assume_sgtype<SgAsmPEExportEntryList>(*n));
        break;

      case V_SgAsmBasicString:
        rv.handle(assume_sgtype<SgAsmBasicString>(*n));
        break;

      case V_SgAsmStoredString:
        rv.handle(assume_sgtype<SgAsmStoredString>(*n));
        break;

      case V_SgAsmGenericString:
        rv.handle(assume_sgtype<SgAsmGenericString>(*n));
        break;

      case V_SgAsmStringStorage:
        rv.handle(assume_sgtype<SgAsmStringStorage>(*n));
        break;

      case V_SgAsmDwarfMacro:
        rv.handle(assume_sgtype<SgAsmDwarfMacro>(*n));
        break;

      case V_SgAsmDwarfLine:
        rv.handle(assume_sgtype<SgAsmDwarfLine>(*n));
        break;

      case V_SgAsmDwarfMacroList:
        rv.handle(assume_sgtype<SgAsmDwarfMacroList>(*n));
        break;

      case V_SgAsmDwarfLineList:
        rv.handle(assume_sgtype<SgAsmDwarfLineList>(*n));
        break;

      case V_SgAsmDwarfArrayType:
        rv.handle(assume_sgtype<SgAsmDwarfArrayType>(*n));
        break;

      case V_SgAsmDwarfClassType:
        rv.handle(assume_sgtype<SgAsmDwarfClassType>(*n));
        break;

      case V_SgAsmDwarfEntryPoint:
        rv.handle(assume_sgtype<SgAsmDwarfEntryPoint>(*n));
        break;

      case V_SgAsmDwarfEnumerationType:
        rv.handle(assume_sgtype<SgAsmDwarfEnumerationType>(*n));
        break;

      case V_SgAsmDwarfFormalParameter:
        rv.handle(assume_sgtype<SgAsmDwarfFormalParameter>(*n));
        break;

      case V_SgAsmDwarfImportedDeclaration:
        rv.handle(assume_sgtype<SgAsmDwarfImportedDeclaration>(*n));
        break;

      case V_SgAsmDwarfLabel:
        rv.handle(assume_sgtype<SgAsmDwarfLabel>(*n));
        break;

      case V_SgAsmDwarfLexicalBlock:
        rv.handle(assume_sgtype<SgAsmDwarfLexicalBlock>(*n));
        break;

      case V_SgAsmDwarfMember:
        rv.handle(assume_sgtype<SgAsmDwarfMember>(*n));
        break;

      case V_SgAsmDwarfPointerType:
        rv.handle(assume_sgtype<SgAsmDwarfPointerType>(*n));
        break;

      case V_SgAsmDwarfReferenceType:
        rv.handle(assume_sgtype<SgAsmDwarfReferenceType>(*n));
        break;

      case V_SgAsmDwarfCompilationUnit:
        rv.handle(assume_sgtype<SgAsmDwarfCompilationUnit>(*n));
        break;

      case V_SgAsmDwarfStringType:
        rv.handle(assume_sgtype<SgAsmDwarfStringType>(*n));
        break;

      case V_SgAsmDwarfStructureType:
        rv.handle(assume_sgtype<SgAsmDwarfStructureType>(*n));
        break;

      case V_SgAsmDwarfSubroutineType:
        rv.handle(assume_sgtype<SgAsmDwarfSubroutineType>(*n));
        break;

      case V_SgAsmDwarfTypedef:
        rv.handle(assume_sgtype<SgAsmDwarfTypedef>(*n));
        break;

      case V_SgAsmDwarfUnionType:
        rv.handle(assume_sgtype<SgAsmDwarfUnionType>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedParameters:
        rv.handle(assume_sgtype<SgAsmDwarfUnspecifiedParameters>(*n));
        break;

      case V_SgAsmDwarfVariant:
        rv.handle(assume_sgtype<SgAsmDwarfVariant>(*n));
        break;

      case V_SgAsmDwarfCommonBlock:
        rv.handle(assume_sgtype<SgAsmDwarfCommonBlock>(*n));
        break;

      case V_SgAsmDwarfCommonInclusion:
        rv.handle(assume_sgtype<SgAsmDwarfCommonInclusion>(*n));
        break;

      case V_SgAsmDwarfInheritance:
        rv.handle(assume_sgtype<SgAsmDwarfInheritance>(*n));
        break;

      case V_SgAsmDwarfInlinedSubroutine:
        rv.handle(assume_sgtype<SgAsmDwarfInlinedSubroutine>(*n));
        break;

      case V_SgAsmDwarfModule:
        rv.handle(assume_sgtype<SgAsmDwarfModule>(*n));
        break;

      case V_SgAsmDwarfPtrToMemberType:
        rv.handle(assume_sgtype<SgAsmDwarfPtrToMemberType>(*n));
        break;

      case V_SgAsmDwarfSetType:
        rv.handle(assume_sgtype<SgAsmDwarfSetType>(*n));
        break;

      case V_SgAsmDwarfSubrangeType:
        rv.handle(assume_sgtype<SgAsmDwarfSubrangeType>(*n));
        break;

      case V_SgAsmDwarfWithStmt:
        rv.handle(assume_sgtype<SgAsmDwarfWithStmt>(*n));
        break;

      case V_SgAsmDwarfAccessDeclaration:
        rv.handle(assume_sgtype<SgAsmDwarfAccessDeclaration>(*n));
        break;

      case V_SgAsmDwarfBaseType:
        rv.handle(assume_sgtype<SgAsmDwarfBaseType>(*n));
        break;

      case V_SgAsmDwarfCatchBlock:
        rv.handle(assume_sgtype<SgAsmDwarfCatchBlock>(*n));
        break;

      case V_SgAsmDwarfConstType:
        rv.handle(assume_sgtype<SgAsmDwarfConstType>(*n));
        break;

      case V_SgAsmDwarfConstant:
        rv.handle(assume_sgtype<SgAsmDwarfConstant>(*n));
        break;

      case V_SgAsmDwarfEnumerator:
        rv.handle(assume_sgtype<SgAsmDwarfEnumerator>(*n));
        break;

      case V_SgAsmDwarfFileType:
        rv.handle(assume_sgtype<SgAsmDwarfFileType>(*n));
        break;

      case V_SgAsmDwarfFriend:
        rv.handle(assume_sgtype<SgAsmDwarfFriend>(*n));
        break;

      case V_SgAsmDwarfNamelist:
        rv.handle(assume_sgtype<SgAsmDwarfNamelist>(*n));
        break;

      case V_SgAsmDwarfNamelistItem:
        rv.handle(assume_sgtype<SgAsmDwarfNamelistItem>(*n));
        break;

      case V_SgAsmDwarfPackedType:
        rv.handle(assume_sgtype<SgAsmDwarfPackedType>(*n));
        break;

      case V_SgAsmDwarfSubprogram:
        rv.handle(assume_sgtype<SgAsmDwarfSubprogram>(*n));
        break;

      case V_SgAsmDwarfTemplateTypeParameter:
        rv.handle(assume_sgtype<SgAsmDwarfTemplateTypeParameter>(*n));
        break;

      case V_SgAsmDwarfTemplateValueParameter:
        rv.handle(assume_sgtype<SgAsmDwarfTemplateValueParameter>(*n));
        break;

      case V_SgAsmDwarfThrownType:
        rv.handle(assume_sgtype<SgAsmDwarfThrownType>(*n));
        break;

      case V_SgAsmDwarfTryBlock:
        rv.handle(assume_sgtype<SgAsmDwarfTryBlock>(*n));
        break;

      case V_SgAsmDwarfVariantPart:
        rv.handle(assume_sgtype<SgAsmDwarfVariantPart>(*n));
        break;

      case V_SgAsmDwarfVariable:
        rv.handle(assume_sgtype<SgAsmDwarfVariable>(*n));
        break;

      case V_SgAsmDwarfVolatileType:
        rv.handle(assume_sgtype<SgAsmDwarfVolatileType>(*n));
        break;

      case V_SgAsmDwarfDwarfProcedure:
        rv.handle(assume_sgtype<SgAsmDwarfDwarfProcedure>(*n));
        break;

      case V_SgAsmDwarfRestrictType:
        rv.handle(assume_sgtype<SgAsmDwarfRestrictType>(*n));
        break;

      case V_SgAsmDwarfInterfaceType:
        rv.handle(assume_sgtype<SgAsmDwarfInterfaceType>(*n));
        break;

      case V_SgAsmDwarfNamespace:
        rv.handle(assume_sgtype<SgAsmDwarfNamespace>(*n));
        break;

      case V_SgAsmDwarfImportedModule:
        rv.handle(assume_sgtype<SgAsmDwarfImportedModule>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedType:
        rv.handle(assume_sgtype<SgAsmDwarfUnspecifiedType>(*n));
        break;

      case V_SgAsmDwarfPartialUnit:
        rv.handle(assume_sgtype<SgAsmDwarfPartialUnit>(*n));
        break;

      case V_SgAsmDwarfImportedUnit:
        rv.handle(assume_sgtype<SgAsmDwarfImportedUnit>(*n));
        break;

      case V_SgAsmDwarfMutableType:
        rv.handle(assume_sgtype<SgAsmDwarfMutableType>(*n));
        break;

      case V_SgAsmDwarfCondition:
        rv.handle(assume_sgtype<SgAsmDwarfCondition>(*n));
        break;

      case V_SgAsmDwarfSharedType:
        rv.handle(assume_sgtype<SgAsmDwarfSharedType>(*n));
        break;

      case V_SgAsmDwarfFormatLabel:
        rv.handle(assume_sgtype<SgAsmDwarfFormatLabel>(*n));
        break;

      case V_SgAsmDwarfFunctionTemplate:
        rv.handle(assume_sgtype<SgAsmDwarfFunctionTemplate>(*n));
        break;

      case V_SgAsmDwarfClassTemplate:
        rv.handle(assume_sgtype<SgAsmDwarfClassTemplate>(*n));
        break;

      case V_SgAsmDwarfUpcSharedType:
        rv.handle(assume_sgtype<SgAsmDwarfUpcSharedType>(*n));
        break;

      case V_SgAsmDwarfUpcStrictType:
        rv.handle(assume_sgtype<SgAsmDwarfUpcStrictType>(*n));
        break;

      case V_SgAsmDwarfUpcRelaxedType:
        rv.handle(assume_sgtype<SgAsmDwarfUpcRelaxedType>(*n));
        break;

      case V_SgAsmDwarfUnknownConstruct:
        rv.handle(assume_sgtype<SgAsmDwarfUnknownConstruct>(*n));
        break;

      case V_SgAsmDwarfConstruct:
        rv.handle(assume_sgtype<SgAsmDwarfConstruct>(*n));
        break;

      case V_SgAsmDwarfConstructList:
        rv.handle(assume_sgtype<SgAsmDwarfConstructList>(*n));
        break;

      case V_SgAsmDwarfCompilationUnitList:
        rv.handle(assume_sgtype<SgAsmDwarfCompilationUnitList>(*n));
        break;

      case V_SgAsmDwarfInformation:
        rv.handle(assume_sgtype<SgAsmDwarfInformation>(*n));
        break;

      case V_SgAsmExecutableFileFormat:
        rv.handle(assume_sgtype<SgAsmExecutableFileFormat>(*n));
        break;

      case V_SgAsmInterpretation:
        rv.handle(assume_sgtype<SgAsmInterpretation>(*n));
        break;

      case V_SgAsmInterpretationList:
        rv.handle(assume_sgtype<SgAsmInterpretationList>(*n));
        break;

      case V_SgAsmGenericFileList:
        rv.handle(assume_sgtype<SgAsmGenericFileList>(*n));
        break;

      case V_SgAsmNode:
        rv.handle(assume_sgtype<SgAsmNode>(*n));
        break;

      case V_SgInitializedName:
        rv.handle(assume_sgtype<SgInitializedName>(*n));
        break;

      case V_SgOmpOrderedClause:
        rv.handle(assume_sgtype<SgOmpOrderedClause>(*n));
        break;

      case V_SgOmpNowaitClause:
        rv.handle(assume_sgtype<SgOmpNowaitClause>(*n));
        break;

      case V_SgOmpUntiedClause:
        rv.handle(assume_sgtype<SgOmpUntiedClause>(*n));
        break;

      case V_SgOmpDefaultClause:
        rv.handle(assume_sgtype<SgOmpDefaultClause>(*n));
        break;

      case V_SgOmpCollapseClause:
        rv.handle(assume_sgtype<SgOmpCollapseClause>(*n));
        break;

      case V_SgOmpIfClause:
        rv.handle(assume_sgtype<SgOmpIfClause>(*n));
        break;

      case V_SgOmpNumThreadsClause:
        rv.handle(assume_sgtype<SgOmpNumThreadsClause>(*n));
        break;

      case V_SgOmpExpressionClause:
        rv.handle(assume_sgtype<SgOmpExpressionClause>(*n));
        break;

      case V_SgOmpCopyprivateClause:
        rv.handle(assume_sgtype<SgOmpCopyprivateClause>(*n));
        break;

      case V_SgOmpPrivateClause:
        rv.handle(assume_sgtype<SgOmpPrivateClause>(*n));
        break;

      case V_SgOmpFirstprivateClause:
        rv.handle(assume_sgtype<SgOmpFirstprivateClause>(*n));
        break;

      case V_SgOmpSharedClause:
        rv.handle(assume_sgtype<SgOmpSharedClause>(*n));
        break;

      case V_SgOmpCopyinClause:
        rv.handle(assume_sgtype<SgOmpCopyinClause>(*n));
        break;

      case V_SgOmpLastprivateClause:
        rv.handle(assume_sgtype<SgOmpLastprivateClause>(*n));
        break;

      case V_SgOmpReductionClause:
        rv.handle(assume_sgtype<SgOmpReductionClause>(*n));
        break;

      case V_SgOmpVariablesClause:
        rv.handle(assume_sgtype<SgOmpVariablesClause>(*n));
        break;

      case V_SgOmpScheduleClause:
        rv.handle(assume_sgtype<SgOmpScheduleClause>(*n));
        break;

      case V_SgOmpClause:
        rv.handle(assume_sgtype<SgOmpClause>(*n));
        break;

      case V_SgRenamePair:
        rv.handle(assume_sgtype<SgRenamePair>(*n));
        break;

      case V_SgInterfaceBody:
        rv.handle(assume_sgtype<SgInterfaceBody>(*n));
        break;

      case V_SgLocatedNodeSupport:
        rv.handle(assume_sgtype<SgLocatedNodeSupport>(*n));
        break;

      case V_SgToken:
        rv.handle(assume_sgtype<SgToken>(*n));
        break;

      case V_SgLocatedNode:
        rv.handle(assume_sgtype<SgLocatedNode>(*n));
        break;

      case V_SgNode:
        rv.handle(assume_sgtype<SgNode>(*n));
        break;

      //
      // Types

      case V_SgTypeUnknown:
        rv.handle(assume_sgtype<SgTypeUnknown>(*n));
        break;

      case V_SgTypeChar:
        rv.handle(assume_sgtype<SgTypeChar>(*n));
        break;

      case V_SgTypeSignedChar:
        rv.handle(assume_sgtype<SgTypeSignedChar>(*n));
        break;

      case V_SgTypeUnsignedChar:
        rv.handle(assume_sgtype<SgTypeUnsignedChar>(*n));
        break;

      case V_SgTypeShort:
        rv.handle(assume_sgtype<SgTypeShort>(*n));
        break;

      case V_SgTypeSignedShort:
        rv.handle(assume_sgtype<SgTypeSignedShort>(*n));
        break;

      case V_SgTypeUnsignedShort:
        rv.handle(assume_sgtype<SgTypeUnsignedShort>(*n));
        break;

      case V_SgTypeInt:
        rv.handle(assume_sgtype<SgTypeInt>(*n));
        break;

      case V_SgTypeSignedInt:
        rv.handle(assume_sgtype<SgTypeSignedInt>(*n));
        break;

      case V_SgTypeUnsignedInt:
        rv.handle(assume_sgtype<SgTypeUnsignedInt>(*n));
        break;

      case V_SgTypeLong:
        rv.handle(assume_sgtype<SgTypeLong>(*n));
        break;

      case V_SgTypeSignedLong:
        rv.handle(assume_sgtype<SgTypeSignedLong>(*n));
        break;

      case V_SgTypeUnsignedLong:
        rv.handle(assume_sgtype<SgTypeUnsignedLong>(*n));
        break;

      case V_SgTypeVoid:
        rv.handle(assume_sgtype<SgTypeVoid>(*n));
        break;

      case V_SgTypeGlobalVoid:
        rv.handle(assume_sgtype<SgTypeGlobalVoid>(*n));
        break;

      case V_SgTypeWchar:
        rv.handle(assume_sgtype<SgTypeWchar>(*n));
        break;

      case V_SgTypeFloat:
        rv.handle(assume_sgtype<SgTypeFloat>(*n));
        break;

      case V_SgTypeDouble:
        rv.handle(assume_sgtype<SgTypeDouble>(*n));
        break;

      case V_SgTypeLongLong:
        rv.handle(assume_sgtype<SgTypeLongLong>(*n));
        break;

      case V_SgTypeSignedLongLong:
        rv.handle(assume_sgtype<SgTypeSignedLongLong>(*n));
        break;

      case V_SgTypeUnsignedLongLong:
        rv.handle(assume_sgtype<SgTypeUnsignedLongLong>(*n));
        break;

      case V_SgTypeLongDouble:
        rv.handle(assume_sgtype<SgTypeLongDouble>(*n));
        break;

      case V_SgTypeString:
        rv.handle(assume_sgtype<SgTypeString>(*n));
        break;

      case V_SgTypeBool:
        rv.handle(assume_sgtype<SgTypeBool>(*n));
        break;

      case V_SgTypeComplex:
        rv.handle(assume_sgtype<SgTypeComplex>(*n));
        break;

      case V_SgTypeImaginary:
        rv.handle(assume_sgtype<SgTypeImaginary>(*n));
        break;

      case V_SgTypeDefault:
        rv.handle(assume_sgtype<SgTypeDefault>(*n));
        break;

      case V_SgPointerMemberType:
        rv.handle(assume_sgtype<SgPointerMemberType>(*n));
        break;

      case V_SgReferenceType:
        rv.handle(assume_sgtype<SgReferenceType>(*n));
        break;

      case V_SgTypeCAFTeam:
        rv.handle(assume_sgtype<SgTypeCAFTeam>(*n));
        break;

      case V_SgClassType:
        rv.handle(assume_sgtype<SgClassType>(*n));
        break;

      case V_SgTemplateType:
        rv.handle(assume_sgtype<SgTemplateType>(*n));
        break;

      case V_SgEnumType:
        rv.handle(assume_sgtype<SgEnumType>(*n));
        break;

      case V_SgTypedefType:
        rv.handle(assume_sgtype<SgTypedefType>(*n));
        break;

      case V_SgModifierType:
        rv.handle(assume_sgtype<SgModifierType>(*n));
        break;

      case V_SgPartialFunctionModifierType:
        rv.handle(assume_sgtype<SgPartialFunctionModifierType>(*n));
        break;

      case V_SgArrayType:
        rv.handle(assume_sgtype<SgArrayType>(*n));
        break;

      case V_SgTypeEllipse:
        rv.handle(assume_sgtype<SgTypeEllipse>(*n));
        break;

      case V_SgTypeCrayPointer:
        rv.handle(assume_sgtype<SgTypeCrayPointer>(*n));
        break;

      case V_SgPartialFunctionType:
        rv.handle(assume_sgtype<SgPartialFunctionType>(*n));
        break;

      case V_SgMemberFunctionType:
        rv.handle(assume_sgtype<SgMemberFunctionType>(*n));
        break;

      case V_SgFunctionType:
        rv.handle(assume_sgtype<SgFunctionType>(*n));
        break;

      case V_SgPointerType:
        rv.handle(assume_sgtype<SgPointerType>(*n));
        break;

      case V_SgNamedType:
        rv.handle(assume_sgtype<SgNamedType>(*n));
        break;

      case V_SgQualifiedNameType:
        rv.handle(assume_sgtype<SgQualifiedNameType>(*n));
        break;

      case V_SgNumVariants: /* fall-through */
      case V_SgType: /* fall-through */
      default:
        std::cerr << "unexpected type: " << typeid(*n).name() << std::endl;
        ROSE_ASSERT(false);
    }

    return rv;
  }


  /// \brief    uncovers the type of SgNode and passes it to an
  ///           overloaded function handle in RoseVisitor.
  /// \param rv an instance of a rose visitor; note that the argument is
  ///           passed by value (similar to STL's for_each).
  /// \param n  a Sage node
  /// \return   a copy of the RoseVisitor object
  template <class RoseVisitor>
  inline
  RoseVisitor
  visitSgNode(const RoseVisitor& rv, SgNode* n)
  {
    return _visitSgNode(rv, n);
  }

  ///! \overload
  template <class RoseVisitor>
  inline
  RoseVisitor
  visitSgNode(const RoseVisitor& rv, const SgNode* n)
  {
    return _visitSgNode(rv, n);
  }


  /// \brief   helper class for _ancestor
  /// \details implements a type switch over the Rose AST to find an
  ///          ancestor node with a given type. The search fails
  ///          at SgProject nodes.
  /// \tparam  AncestorNode the type of ancestors to look for
  /// \tparam  QualSgNode either 'const SgNode*' or 'SgNode*' depending
  ///          if the search is over constant nodes or not.
  template <class AncestorNode, class QualSgNode>
  struct AncestorTypeFinder
  {
    typedef std::pair<AncestorNode*, QualSgNode*> Pair;

    Pair res;

    AncestorTypeFinder()
    : res(NULL, NULL)
    {}

    void handle(QualSgNode& n) { res.second = n.get_parent(); }
    void handle(AncestorNode& n) { res.first = &n; }

    // \pp \todo should be disabled when QualSgNode is also 'const SgProject'
    void handle(const SgProject&) {}

    operator Pair() const { return res; }
  };

  /// \brief   implements the ancestor search
  /// \details see comments for AncestorTypeFinder
  template <class AncestorNode, class QualSgNode>
  AncestorNode* _ancestor(QualSgNode& n)
  {
    typedef AncestorTypeFinder<AncestorNode, QualSgNode> AncestorFinder;

    typename AncestorFinder::Pair res(NULL, n.get_parent());

    while (res.second != NULL)
    {
      res = ez::visitSgNode(AncestorFinder(), res.second);
    }

    return res.first;
  }

  /// \brief   finds an ancestor node with a given type
  /// \details the function family comes in four variants:
  ///          SgNode*       -> AncestorNode*       ( result can be NULL )
  ///          const SgNode* -> const AncestorNode* ( result can be NULL )
  ///          SgNode&       -> AncestorNode&       ( assert(false) when an ancestor of
  ///                                                 the specified type cannot be found )
  ///          const SgNode& -> const AncestorNode& ( assert(false) when an ancestor of
  ///                                                 the specified type cannot be found )
  template <class AncestorNode>
  AncestorNode* ancestor(SgNode* n)
  {
    if (n == NULL) return NULL;

    return _ancestor<AncestorNode>(*n);
  }

  ///! \overload
  template <class AncestorNode>
  const AncestorNode* ancestor(const SgNode* n)
  {
    if (n == NULL) return NULL;

    return _ancestor<const AncestorNode>(*n);
  }

  ///! \overload
  template <class AncestorNode>
  AncestorNode& ancestor(SgNode& n)
  {
    AncestorNode* res = _ancestor<AncestorNode>(n);

    ROSE_ASSERT(res);
    return *res;
  }

  ///! \overload
  template <class AncestorNode>
  const AncestorNode& ancestor(const SgNode& n)
  {
    const AncestorNode* res = _ancestor<const AncestorNode>(n);

    ROSE_ASSERT(res);
    return *res;
  }
}

#endif /* _ROSEZ_HPP */
