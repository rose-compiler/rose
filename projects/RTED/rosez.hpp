
#ifndef _ROSEZ_HPP

#define _ROSEZ_HPP

#include <iostream>
#include <typeinfo>

#include "rose.h"

namespace ez
{
  /// \brief portable implementation to avoid compiler warnings due to an
  ///        unused parameter.
  template <class RoseNode>
  inline
  void unused(const RoseNode&) {}

  /// \brief    uncovers the type of SgNode and passes it to an
  ///           overloaded function handle in RoseVisitor.
  /// \param rv an instance of a rose visitor; note that the argument is
  ///           passed by value (similar to STL's for_each).
  /// \param n  a Sage node
  /// \return   a copy of the RoseVisitor object
  template <class RoseVisitor>
  RoseVisitor
  visitSgNode(RoseVisitor rv, SgNode* n)
  {
    ROSE_ASSERT( n );

    switch (n->variantT())
    {
      case V_SgName:
        rv.handle(static_cast<SgName&>(*n));
        break;

      case V_SgSymbolTable:
        rv.handle(static_cast<SgSymbolTable&>(*n));
        break;

      case V_SgPragma:
        rv.handle(static_cast<SgPragma&>(*n));
        break;

      case V_SgModifierNodes:
        rv.handle(static_cast<SgModifierNodes&>(*n));
        break;

      case V_SgConstVolatileModifier:
        rv.handle(static_cast<SgConstVolatileModifier&>(*n));
        break;

      case V_SgStorageModifier:
        rv.handle(static_cast<SgStorageModifier&>(*n));
        break;

      case V_SgAccessModifier:
        rv.handle(static_cast<SgAccessModifier&>(*n));
        break;

      case V_SgFunctionModifier:
        rv.handle(static_cast<SgFunctionModifier&>(*n));
        break;

      case V_SgUPC_AccessModifier:
        rv.handle(static_cast<SgUPC_AccessModifier&>(*n));
        break;

      case V_SgLinkageModifier:
        rv.handle(static_cast<SgLinkageModifier&>(*n));
        break;

      case V_SgSpecialFunctionModifier:
        rv.handle(static_cast<SgSpecialFunctionModifier&>(*n));
        break;

      case V_SgTypeModifier:
        rv.handle(static_cast<SgTypeModifier&>(*n));
        break;

      case V_SgElaboratedTypeModifier:
        rv.handle(static_cast<SgElaboratedTypeModifier&>(*n));
        break;

      case V_SgBaseClassModifier:
        rv.handle(static_cast<SgBaseClassModifier&>(*n));
        break;

      case V_SgDeclarationModifier:
        rv.handle(static_cast<SgDeclarationModifier&>(*n));
        break;

      case V_SgOpenclAccessModeModifier:
        rv.handle(static_cast<SgOpenclAccessModeModifier&>(*n));
        break;

      case V_SgModifier:
        rv.handle(static_cast<SgModifier&>(*n));
        break;

      case V_Sg_File_Info:
        rv.handle(static_cast<Sg_File_Info&>(*n));
        break;

      case V_SgSourceFile:
        rv.handle(static_cast<SgSourceFile&>(*n));
        break;

      case V_SgBinaryComposite:
        rv.handle(static_cast<SgBinaryComposite&>(*n));
        break;

      case V_SgUnknownFile:
        rv.handle(static_cast<SgUnknownFile&>(*n));
        break;

      case V_SgFile:
        rv.handle(static_cast<SgFile&>(*n));
        break;

      case V_SgFileList:
        rv.handle(static_cast<SgFileList&>(*n));
        break;

      case V_SgDirectory:
        rv.handle(static_cast<SgDirectory&>(*n));
        break;

      case V_SgDirectoryList:
        rv.handle(static_cast<SgDirectoryList&>(*n));
        break;

      case V_SgProject:
        rv.handle(static_cast<SgProject&>(*n));
        break;

      case V_SgOptions:
        rv.handle(static_cast<SgOptions&>(*n));
        break;

      case V_SgUnparse_Info:
        rv.handle(static_cast<SgUnparse_Info&>(*n));
        break;

      case V_SgFuncDecl_attr:
        rv.handle(static_cast<SgFuncDecl_attr&>(*n));
        break;

      case V_SgClassDecl_attr:
        rv.handle(static_cast<SgClassDecl_attr&>(*n));
        break;

      case V_SgTypedefSeq:
        rv.handle(static_cast<SgTypedefSeq&>(*n));
        break;

      case V_SgFunctionParameterTypeList:
        rv.handle(static_cast<SgFunctionParameterTypeList&>(*n));
        break;

      case V_SgTemplateParameter:
        rv.handle(static_cast<SgTemplateParameter&>(*n));
        break;

      case V_SgTemplateArgument:
        rv.handle(static_cast<SgTemplateArgument&>(*n));
        break;

      case V_SgTemplateParameterList:
        rv.handle(static_cast<SgTemplateParameterList&>(*n));
        break;

      case V_SgTemplateArgumentList:
        rv.handle(static_cast<SgTemplateArgumentList&>(*n));
        break;

      case V_SgBitAttribute:
        rv.handle(static_cast<SgBitAttribute&>(*n));
        break;

      case V_SgAttribute:
        rv.handle(static_cast<SgAttribute&>(*n));
        break;

      case V_SgBaseClass:
        rv.handle(static_cast<SgBaseClass&>(*n));
        break;

      case V_SgUndirectedGraphEdge:
        rv.handle(static_cast<SgUndirectedGraphEdge&>(*n));
        break;

      case V_SgDirectedGraphEdge:
        rv.handle(static_cast<SgDirectedGraphEdge&>(*n));
        break;

      case V_SgGraphNode:
        rv.handle(static_cast<SgGraphNode&>(*n));
        break;

      case V_SgGraphEdge:
        rv.handle(static_cast<SgGraphEdge&>(*n));
        break;

      case V_SgStringKeyedBidirectionalGraph:
        rv.handle(static_cast<SgStringKeyedBidirectionalGraph&>(*n));
        break;

      case V_SgIntKeyedBidirectionalGraph:
        rv.handle(static_cast<SgIntKeyedBidirectionalGraph&>(*n));
        break;

      case V_SgBidirectionalGraph:
        rv.handle(static_cast<SgBidirectionalGraph&>(*n));
        break;

      case V_SgIncidenceDirectedGraph:
        rv.handle(static_cast<SgIncidenceDirectedGraph&>(*n));
        break;

      case V_SgIncidenceUndirectedGraph:
        rv.handle(static_cast<SgIncidenceUndirectedGraph&>(*n));
        break;

      case V_SgGraph:
        rv.handle(static_cast<SgGraph&>(*n));
        break;

      case V_SgGraphNodeList:
        rv.handle(static_cast<SgGraphNodeList&>(*n));
        break;

      case V_SgGraphEdgeList:
        rv.handle(static_cast<SgGraphEdgeList&>(*n));
        break;

      case V_SgQualifiedName:
        rv.handle(static_cast<SgQualifiedName&>(*n));
        break;

      case V_SgNameGroup:
        rv.handle(static_cast<SgNameGroup&>(*n));
        break;

      case V_SgCommonBlockObject:
        rv.handle(static_cast<SgCommonBlockObject&>(*n));
        break;

      case V_SgDimensionObject:
        rv.handle(static_cast<SgDimensionObject&>(*n));
        break;

      case V_SgDataStatementGroup:
        rv.handle(static_cast<SgDataStatementGroup&>(*n));
        break;

      case V_SgDataStatementObject:
        rv.handle(static_cast<SgDataStatementObject&>(*n));
        break;

      case V_SgDataStatementValue:
        rv.handle(static_cast<SgDataStatementValue&>(*n));
        break;

      case V_SgFormatItem:
        rv.handle(static_cast<SgFormatItem&>(*n));
        break;

      case V_SgFormatItemList:
        rv.handle(static_cast<SgFormatItemList&>(*n));
        break;

      case V_SgTypeTable:
        rv.handle(static_cast<SgTypeTable&>(*n));
        break;

      case V_SgSupport:
        rv.handle(static_cast<SgSupport&>(*n));
        break;

      case V_SgForStatement:
        rv.handle(static_cast<SgForStatement&>(*n));
        break;

      case V_SgForInitStatement:
        rv.handle(static_cast<SgForInitStatement&>(*n));
        break;

      case V_SgCatchStatementSeq:
        rv.handle(static_cast<SgCatchStatementSeq&>(*n));
        break;

      case V_SgFunctionParameterList:
        rv.handle(static_cast<SgFunctionParameterList&>(*n));
        break;

      case V_SgCtorInitializerList:
        rv.handle(static_cast<SgCtorInitializerList&>(*n));
        break;

      case V_SgBasicBlock:
        rv.handle(static_cast<SgBasicBlock&>(*n));
        break;

      case V_SgGlobal:
        rv.handle(static_cast<SgGlobal&>(*n));
        break;

      case V_SgIfStmt:
        rv.handle(static_cast<SgIfStmt&>(*n));
        break;

      case V_SgFunctionDefinition:
        rv.handle(static_cast<SgFunctionDefinition&>(*n));
        break;

      case V_SgWhileStmt:
        rv.handle(static_cast<SgWhileStmt&>(*n));
        break;

      case V_SgDoWhileStmt:
        rv.handle(static_cast<SgDoWhileStmt&>(*n));
        break;

      case V_SgSwitchStatement:
        rv.handle(static_cast<SgSwitchStatement&>(*n));
        break;

      case V_SgCatchOptionStmt:
        rv.handle(static_cast<SgCatchOptionStmt&>(*n));
        break;

      case V_SgVariableDeclaration:
        rv.handle(static_cast<SgVariableDeclaration&>(*n));
        break;

      case V_SgVariableDefinition:
        rv.handle(static_cast<SgVariableDefinition&>(*n));
        break;

      case V_SgEnumDeclaration:
        rv.handle(static_cast<SgEnumDeclaration&>(*n));
        break;

      case V_SgAsmStmt:
        rv.handle(static_cast<SgAsmStmt&>(*n));
        break;

      case V_SgTypedefDeclaration:
        rv.handle(static_cast<SgTypedefDeclaration&>(*n));
        break;

      case V_SgFunctionTypeTable:
        rv.handle(static_cast<SgFunctionTypeTable&>(*n));
        break;

      case V_SgExprStatement:
        rv.handle(static_cast<SgExprStatement&>(*n));
        break;

      case V_SgLabelStatement:
        rv.handle(static_cast<SgLabelStatement&>(*n));
        break;

      case V_SgCaseOptionStmt:
        rv.handle(static_cast<SgCaseOptionStmt&>(*n));
        break;

      case V_SgTryStmt:
        rv.handle(static_cast<SgTryStmt&>(*n));
        break;

      case V_SgDefaultOptionStmt:
        rv.handle(static_cast<SgDefaultOptionStmt&>(*n));
        break;

      case V_SgBreakStmt:
        rv.handle(static_cast<SgBreakStmt&>(*n));
        break;

      case V_SgContinueStmt:
        rv.handle(static_cast<SgContinueStmt&>(*n));
        break;

      case V_SgReturnStmt:
        rv.handle(static_cast<SgReturnStmt&>(*n));
        break;

      case V_SgGotoStatement:
        rv.handle(static_cast<SgGotoStatement&>(*n));
        break;

      case V_SgSpawnStmt:
        rv.handle(static_cast<SgSpawnStmt&>(*n));
        break;

      case V_SgNullStatement:
        rv.handle(static_cast<SgNullStatement&>(*n));
        break;

      case V_SgVariantStatement:
        rv.handle(static_cast<SgVariantStatement&>(*n));
        break;

      case V_SgPragmaDeclaration:
        rv.handle(static_cast<SgPragmaDeclaration&>(*n));
        break;

      case V_SgTemplateDeclaration:
        rv.handle(static_cast<SgTemplateDeclaration&>(*n));
        break;

      case V_SgTemplateInstantiationDecl:
        rv.handle(static_cast<SgTemplateInstantiationDecl&>(*n));
        break;

      case V_SgTemplateInstantiationDefn:
        rv.handle(static_cast<SgTemplateInstantiationDefn&>(*n));
        break;

      case V_SgTemplateInstantiationFunctionDecl:
        rv.handle(static_cast<SgTemplateInstantiationFunctionDecl&>(*n));
        break;

      case V_SgTemplateInstantiationMemberFunctionDecl:
        rv.handle(static_cast<SgTemplateInstantiationMemberFunctionDecl&>(*n));
        break;

      case V_SgProgramHeaderStatement:
        rv.handle(static_cast<SgProgramHeaderStatement&>(*n));
        break;

      case V_SgProcedureHeaderStatement:
        rv.handle(static_cast<SgProcedureHeaderStatement&>(*n));
        break;

      case V_SgEntryStatement:
        rv.handle(static_cast<SgEntryStatement&>(*n));
        break;

      case V_SgFortranNonblockedDo:
        rv.handle(static_cast<SgFortranNonblockedDo&>(*n));
        break;

      case V_SgInterfaceStatement:
        rv.handle(static_cast<SgInterfaceStatement&>(*n));
        break;

      case V_SgParameterStatement:
        rv.handle(static_cast<SgParameterStatement&>(*n));
        break;

      case V_SgCommonBlock:
        rv.handle(static_cast<SgCommonBlock&>(*n));
        break;

      case V_SgModuleStatement:
        rv.handle(static_cast<SgModuleStatement&>(*n));
        break;

      case V_SgUseStatement:
        rv.handle(static_cast<SgUseStatement&>(*n));
        break;

      case V_SgStopOrPauseStatement:
        rv.handle(static_cast<SgStopOrPauseStatement&>(*n));
        break;

      case V_SgPrintStatement:
        rv.handle(static_cast<SgPrintStatement&>(*n));
        break;

      case V_SgReadStatement:
        rv.handle(static_cast<SgReadStatement&>(*n));
        break;

      case V_SgWriteStatement:
        rv.handle(static_cast<SgWriteStatement&>(*n));
        break;

      case V_SgOpenStatement:
        rv.handle(static_cast<SgOpenStatement&>(*n));
        break;

      case V_SgCloseStatement:
        rv.handle(static_cast<SgCloseStatement&>(*n));
        break;

      case V_SgInquireStatement:
        rv.handle(static_cast<SgInquireStatement&>(*n));
        break;

      case V_SgFlushStatement:
        rv.handle(static_cast<SgFlushStatement&>(*n));
        break;

      case V_SgBackspaceStatement:
        rv.handle(static_cast<SgBackspaceStatement&>(*n));
        break;

      case V_SgRewindStatement:
        rv.handle(static_cast<SgRewindStatement&>(*n));
        break;

      case V_SgEndfileStatement:
        rv.handle(static_cast<SgEndfileStatement&>(*n));
        break;

      case V_SgWaitStatement:
        rv.handle(static_cast<SgWaitStatement&>(*n));
        break;

      case V_SgCAFWithTeamStatement:
        rv.handle(static_cast<SgCAFWithTeamStatement&>(*n));
        break;

      case V_SgFormatStatement:
        rv.handle(static_cast<SgFormatStatement&>(*n));
        break;

      case V_SgFortranDo:
        rv.handle(static_cast<SgFortranDo&>(*n));
        break;

      case V_SgForAllStatement:
        rv.handle(static_cast<SgForAllStatement&>(*n));
        break;

      case V_SgIOStatement:
        rv.handle(static_cast<SgIOStatement&>(*n));
        break;

      case V_SgUpcNotifyStatement:
        rv.handle(static_cast<SgUpcNotifyStatement&>(*n));
        break;

      case V_SgUpcWaitStatement:
        rv.handle(static_cast<SgUpcWaitStatement&>(*n));
        break;

      case V_SgUpcBarrierStatement:
        rv.handle(static_cast<SgUpcBarrierStatement&>(*n));
        break;

      case V_SgUpcFenceStatement:
        rv.handle(static_cast<SgUpcFenceStatement&>(*n));
        break;

      case V_SgUpcForAllStatement:
        rv.handle(static_cast<SgUpcForAllStatement&>(*n));
        break;

      case V_SgOmpParallelStatement:
        rv.handle(static_cast<SgOmpParallelStatement&>(*n));
        break;

      case V_SgOmpSingleStatement:
        rv.handle(static_cast<SgOmpSingleStatement&>(*n));
        break;

      case V_SgOmpTaskStatement:
        rv.handle(static_cast<SgOmpTaskStatement&>(*n));
        break;

      case V_SgOmpForStatement:
        rv.handle(static_cast<SgOmpForStatement&>(*n));
        break;

      case V_SgOmpDoStatement:
        rv.handle(static_cast<SgOmpDoStatement&>(*n));
        break;

      case V_SgOmpSectionsStatement:
        rv.handle(static_cast<SgOmpSectionsStatement&>(*n));
        break;

      case V_SgOmpClauseBodyStatement:
        rv.handle(static_cast<SgOmpClauseBodyStatement&>(*n));
        break;

      case V_SgOmpAtomicStatement:
        rv.handle(static_cast<SgOmpAtomicStatement&>(*n));
        break;

      case V_SgOmpMasterStatement:
        rv.handle(static_cast<SgOmpMasterStatement&>(*n));
        break;

      case V_SgOmpSectionStatement:
        rv.handle(static_cast<SgOmpSectionStatement&>(*n));
        break;

      case V_SgOmpOrderedStatement:
        rv.handle(static_cast<SgOmpOrderedStatement&>(*n));
        break;

      case V_SgOmpWorkshareStatement:
        rv.handle(static_cast<SgOmpWorkshareStatement&>(*n));
        break;

      case V_SgOmpCriticalStatement:
        rv.handle(static_cast<SgOmpCriticalStatement&>(*n));
        break;

      case V_SgOmpBodyStatement:
        rv.handle(static_cast<SgOmpBodyStatement&>(*n));
        break;

      case V_SgBlockDataStatement:
        rv.handle(static_cast<SgBlockDataStatement&>(*n));
        break;

      case V_SgImplicitStatement:
        rv.handle(static_cast<SgImplicitStatement&>(*n));
        break;

      case V_SgStatementFunctionStatement:
        rv.handle(static_cast<SgStatementFunctionStatement&>(*n));
        break;

      case V_SgWhereStatement:
        rv.handle(static_cast<SgWhereStatement&>(*n));
        break;

      case V_SgNullifyStatement:
        rv.handle(static_cast<SgNullifyStatement&>(*n));
        break;

      case V_SgEquivalenceStatement:
        rv.handle(static_cast<SgEquivalenceStatement&>(*n));
        break;

      case V_SgDerivedTypeStatement:
        rv.handle(static_cast<SgDerivedTypeStatement&>(*n));
        break;

      case V_SgAttributeSpecificationStatement:
        rv.handle(static_cast<SgAttributeSpecificationStatement&>(*n));
        break;

      case V_SgAllocateStatement:
        rv.handle(static_cast<SgAllocateStatement&>(*n));
        break;

      case V_SgDeallocateStatement:
        rv.handle(static_cast<SgDeallocateStatement&>(*n));
        break;

      case V_SgContainsStatement:
        rv.handle(static_cast<SgContainsStatement&>(*n));
        break;

      case V_SgSequenceStatement:
        rv.handle(static_cast<SgSequenceStatement&>(*n));
        break;

      case V_SgElseWhereStatement:
        rv.handle(static_cast<SgElseWhereStatement&>(*n));
        break;

      case V_SgArithmeticIfStatement:
        rv.handle(static_cast<SgArithmeticIfStatement&>(*n));
        break;

      case V_SgAssignStatement:
        rv.handle(static_cast<SgAssignStatement&>(*n));
        break;

      case V_SgComputedGotoStatement:
        rv.handle(static_cast<SgComputedGotoStatement&>(*n));
        break;

      case V_SgAssignedGotoStatement:
        rv.handle(static_cast<SgAssignedGotoStatement&>(*n));
        break;

      case V_SgNamelistStatement:
        rv.handle(static_cast<SgNamelistStatement&>(*n));
        break;

      case V_SgImportStatement:
        rv.handle(static_cast<SgImportStatement&>(*n));
        break;

      case V_SgAssociateStatement:
        rv.handle(static_cast<SgAssociateStatement&>(*n));
        break;

      case V_SgFortranIncludeLine:
        rv.handle(static_cast<SgFortranIncludeLine&>(*n));
        break;

      case V_SgNamespaceDeclarationStatement:
        rv.handle(static_cast<SgNamespaceDeclarationStatement&>(*n));
        break;

      case V_SgNamespaceAliasDeclarationStatement:
        rv.handle(static_cast<SgNamespaceAliasDeclarationStatement&>(*n));
        break;

      case V_SgNamespaceDefinitionStatement:
        rv.handle(static_cast<SgNamespaceDefinitionStatement&>(*n));
        break;

      case V_SgUsingDeclarationStatement:
        rv.handle(static_cast<SgUsingDeclarationStatement&>(*n));
        break;

      case V_SgUsingDirectiveStatement:
        rv.handle(static_cast<SgUsingDirectiveStatement&>(*n));
        break;

      case V_SgTemplateInstantiationDirectiveStatement:
        rv.handle(static_cast<SgTemplateInstantiationDirectiveStatement&>(*n));
        break;

      case V_SgClassDeclaration:
        rv.handle(static_cast<SgClassDeclaration&>(*n));
        break;

      case V_SgClassDefinition:
        rv.handle(static_cast<SgClassDefinition&>(*n));
        break;

      case V_SgScopeStatement:
        rv.handle(static_cast<SgScopeStatement&>(*n));
        break;

      case V_SgMemberFunctionDeclaration:
        rv.handle(static_cast<SgMemberFunctionDeclaration&>(*n));
        break;

      case V_SgFunctionDeclaration:
        rv.handle(static_cast<SgFunctionDeclaration&>(*n));
        break;

      case V_SgIncludeDirectiveStatement:
        rv.handle(static_cast<SgIncludeDirectiveStatement&>(*n));
        break;

      case V_SgDefineDirectiveStatement:
        rv.handle(static_cast<SgDefineDirectiveStatement&>(*n));
        break;

      case V_SgUndefDirectiveStatement:
        rv.handle(static_cast<SgUndefDirectiveStatement&>(*n));
        break;

      case V_SgIfdefDirectiveStatement:
        rv.handle(static_cast<SgIfdefDirectiveStatement&>(*n));
        break;

      case V_SgIfndefDirectiveStatement:
        rv.handle(static_cast<SgIfndefDirectiveStatement&>(*n));
        break;

      case V_SgIfDirectiveStatement:
        rv.handle(static_cast<SgIfDirectiveStatement&>(*n));
        break;

      case V_SgDeadIfDirectiveStatement:
        rv.handle(static_cast<SgDeadIfDirectiveStatement&>(*n));
        break;

      case V_SgElseDirectiveStatement:
        rv.handle(static_cast<SgElseDirectiveStatement&>(*n));
        break;

      case V_SgElseifDirectiveStatement:
        rv.handle(static_cast<SgElseifDirectiveStatement&>(*n));
        break;

      case V_SgEndifDirectiveStatement:
        rv.handle(static_cast<SgEndifDirectiveStatement&>(*n));
        break;

      case V_SgLineDirectiveStatement:
        rv.handle(static_cast<SgLineDirectiveStatement&>(*n));
        break;

      case V_SgWarningDirectiveStatement:
        rv.handle(static_cast<SgWarningDirectiveStatement&>(*n));
        break;

      case V_SgErrorDirectiveStatement:
        rv.handle(static_cast<SgErrorDirectiveStatement&>(*n));
        break;

      case V_SgEmptyDirectiveStatement:
        rv.handle(static_cast<SgEmptyDirectiveStatement&>(*n));
        break;

      case V_SgIncludeNextDirectiveStatement:
        rv.handle(static_cast<SgIncludeNextDirectiveStatement&>(*n));
        break;

      case V_SgIdentDirectiveStatement:
        rv.handle(static_cast<SgIdentDirectiveStatement&>(*n));
        break;

      case V_SgLinemarkerDirectiveStatement:
        rv.handle(static_cast<SgLinemarkerDirectiveStatement&>(*n));
        break;

      case V_SgC_PreprocessorDirectiveStatement:
        rv.handle(static_cast<SgC_PreprocessorDirectiveStatement&>(*n));
        break;

      case V_SgClinkageStartStatement:
        rv.handle(static_cast<SgClinkageStartStatement&>(*n));
        break;

      case V_SgClinkageEndStatement:
        rv.handle(static_cast<SgClinkageEndStatement&>(*n));
        break;

      case V_SgClinkageDeclarationStatement:
        rv.handle(static_cast<SgClinkageDeclarationStatement&>(*n));
        break;

      case V_SgOmpFlushStatement:
        rv.handle(static_cast<SgOmpFlushStatement&>(*n));
        break;

      case V_SgOmpBarrierStatement:
        rv.handle(static_cast<SgOmpBarrierStatement&>(*n));
        break;

      case V_SgOmpTaskwaitStatement:
        rv.handle(static_cast<SgOmpTaskwaitStatement&>(*n));
        break;

      case V_SgOmpThreadprivateStatement:
        rv.handle(static_cast<SgOmpThreadprivateStatement&>(*n));
        break;

      case V_SgDeclarationStatement:
        rv.handle(static_cast<SgDeclarationStatement&>(*n));
        break;

      case V_SgStatement:
        rv.handle(static_cast<SgStatement&>(*n));
        break;

      case V_SgExprListExp:
        rv.handle(static_cast<SgExprListExp&>(*n));
        break;

      case V_SgVarRefExp:
        rv.handle(static_cast<SgVarRefExp&>(*n));
        break;

      case V_SgLabelRefExp:
        rv.handle(static_cast<SgLabelRefExp&>(*n));
        break;

      case V_SgClassNameRefExp:
        rv.handle(static_cast<SgClassNameRefExp&>(*n));
        break;

      case V_SgFunctionRefExp:
        rv.handle(static_cast<SgFunctionRefExp&>(*n));
        break;

      case V_SgMemberFunctionRefExp:
        rv.handle(static_cast<SgMemberFunctionRefExp&>(*n));
        break;

      case V_SgFunctionCallExp:
        rv.handle(static_cast<SgFunctionCallExp&>(*n));
        break;

      case V_SgSizeOfOp:
        rv.handle(static_cast<SgSizeOfOp&>(*n));
        break;

      case V_SgUpcLocalsizeofExpression:
        rv.handle(static_cast<SgUpcLocalsizeofExpression&>(*n));
        break;

      case V_SgUpcBlocksizeofExpression:
        rv.handle(static_cast<SgUpcBlocksizeofExpression&>(*n));
        break;

      case V_SgUpcElemsizeofExpression:
        rv.handle(static_cast<SgUpcElemsizeofExpression&>(*n));
        break;

      case V_SgVarArgStartOp:
        rv.handle(static_cast<SgVarArgStartOp&>(*n));
        break;

      case V_SgVarArgStartOneOperandOp:
        rv.handle(static_cast<SgVarArgStartOneOperandOp&>(*n));
        break;

      case V_SgVarArgOp:
        rv.handle(static_cast<SgVarArgOp&>(*n));
        break;

      case V_SgVarArgEndOp:
        rv.handle(static_cast<SgVarArgEndOp&>(*n));
        break;

      case V_SgVarArgCopyOp:
        rv.handle(static_cast<SgVarArgCopyOp&>(*n));
        break;

      case V_SgTypeIdOp:
        rv.handle(static_cast<SgTypeIdOp&>(*n));
        break;

      case V_SgConditionalExp:
        rv.handle(static_cast<SgConditionalExp&>(*n));
        break;

      case V_SgNewExp:
        rv.handle(static_cast<SgNewExp&>(*n));
        break;

      case V_SgDeleteExp:
        rv.handle(static_cast<SgDeleteExp&>(*n));
        break;

      case V_SgThisExp:
        rv.handle(static_cast<SgThisExp&>(*n));
        break;

      case V_SgRefExp:
        rv.handle(static_cast<SgRefExp&>(*n));
        break;

      case V_SgAggregateInitializer:
        rv.handle(static_cast<SgAggregateInitializer&>(*n));
        break;

      case V_SgConstructorInitializer:
        rv.handle(static_cast<SgConstructorInitializer&>(*n));
        break;

      case V_SgAssignInitializer:
        rv.handle(static_cast<SgAssignInitializer&>(*n));
        break;

      case V_SgExpressionRoot:
        rv.handle(static_cast<SgExpressionRoot&>(*n));
        break;

      case V_SgMinusOp:
        rv.handle(static_cast<SgMinusOp&>(*n));
        break;

      case V_SgUnaryAddOp:
        rv.handle(static_cast<SgUnaryAddOp&>(*n));
        break;

      case V_SgNotOp:
        rv.handle(static_cast<SgNotOp&>(*n));
        break;

      case V_SgPointerDerefExp:
        rv.handle(static_cast<SgPointerDerefExp&>(*n));
        break;

      case V_SgAddressOfOp:
        rv.handle(static_cast<SgAddressOfOp&>(*n));
        break;

      case V_SgMinusMinusOp:
        rv.handle(static_cast<SgMinusMinusOp&>(*n));
        break;

      case V_SgPlusPlusOp:
        rv.handle(static_cast<SgPlusPlusOp&>(*n));
        break;

      case V_SgBitComplementOp:
        rv.handle(static_cast<SgBitComplementOp&>(*n));
        break;

      case V_SgRealPartOp:
        rv.handle(static_cast<SgRealPartOp&>(*n));
        break;

      case V_SgImagPartOp:
        rv.handle(static_cast<SgImagPartOp&>(*n));
        break;

      case V_SgConjugateOp:
        rv.handle(static_cast<SgConjugateOp&>(*n));
        break;

      case V_SgCastExp:
        rv.handle(static_cast<SgCastExp&>(*n));
        break;

      case V_SgThrowOp:
        rv.handle(static_cast<SgThrowOp&>(*n));
        break;

      case V_SgArrowExp:
        rv.handle(static_cast<SgArrowExp&>(*n));
        break;

      case V_SgDotExp:
        rv.handle(static_cast<SgDotExp&>(*n));
        break;

      case V_SgDotStarOp:
        rv.handle(static_cast<SgDotStarOp&>(*n));
        break;

      case V_SgArrowStarOp:
        rv.handle(static_cast<SgArrowStarOp&>(*n));
        break;

      case V_SgEqualityOp:
        rv.handle(static_cast<SgEqualityOp&>(*n));
        break;

      case V_SgLessThanOp:
        rv.handle(static_cast<SgLessThanOp&>(*n));
        break;

      case V_SgGreaterThanOp:
        rv.handle(static_cast<SgGreaterThanOp&>(*n));
        break;

      case V_SgNotEqualOp:
        rv.handle(static_cast<SgNotEqualOp&>(*n));
        break;

      case V_SgLessOrEqualOp:
        rv.handle(static_cast<SgLessOrEqualOp&>(*n));
        break;

      case V_SgGreaterOrEqualOp:
        rv.handle(static_cast<SgGreaterOrEqualOp&>(*n));
        break;

      case V_SgAddOp:
        rv.handle(static_cast<SgAddOp&>(*n));
        break;

      case V_SgSubtractOp:
        rv.handle(static_cast<SgSubtractOp&>(*n));
        break;

      case V_SgMultiplyOp:
        rv.handle(static_cast<SgMultiplyOp&>(*n));
        break;

      case V_SgDivideOp:
        rv.handle(static_cast<SgDivideOp&>(*n));
        break;

      case V_SgIntegerDivideOp:
        rv.handle(static_cast<SgIntegerDivideOp&>(*n));
        break;

      case V_SgModOp:
        rv.handle(static_cast<SgModOp&>(*n));
        break;

      case V_SgAndOp:
        rv.handle(static_cast<SgAndOp&>(*n));
        break;

      case V_SgOrOp:
        rv.handle(static_cast<SgOrOp&>(*n));
        break;

      case V_SgBitXorOp:
        rv.handle(static_cast<SgBitXorOp&>(*n));
        break;

      case V_SgBitAndOp:
        rv.handle(static_cast<SgBitAndOp&>(*n));
        break;

      case V_SgBitOrOp:
        rv.handle(static_cast<SgBitOrOp&>(*n));
        break;

      case V_SgCommaOpExp:
        rv.handle(static_cast<SgCommaOpExp&>(*n));
        break;

      case V_SgLshiftOp:
        rv.handle(static_cast<SgLshiftOp&>(*n));
        break;

      case V_SgRshiftOp:
        rv.handle(static_cast<SgRshiftOp&>(*n));
        break;

      case V_SgPntrArrRefExp:
        rv.handle(static_cast<SgPntrArrRefExp&>(*n));
        break;

      case V_SgScopeOp:
        rv.handle(static_cast<SgScopeOp&>(*n));
        break;

      case V_SgAssignOp:
        rv.handle(static_cast<SgAssignOp&>(*n));
        break;

      case V_SgPlusAssignOp:
        rv.handle(static_cast<SgPlusAssignOp&>(*n));
        break;

      case V_SgMinusAssignOp:
        rv.handle(static_cast<SgMinusAssignOp&>(*n));
        break;

      case V_SgAndAssignOp:
        rv.handle(static_cast<SgAndAssignOp&>(*n));
        break;

      case V_SgIorAssignOp:
        rv.handle(static_cast<SgIorAssignOp&>(*n));
        break;

      case V_SgMultAssignOp:
        rv.handle(static_cast<SgMultAssignOp&>(*n));
        break;

      case V_SgDivAssignOp:
        rv.handle(static_cast<SgDivAssignOp&>(*n));
        break;

      case V_SgModAssignOp:
        rv.handle(static_cast<SgModAssignOp&>(*n));
        break;

      case V_SgXorAssignOp:
        rv.handle(static_cast<SgXorAssignOp&>(*n));
        break;

      case V_SgLshiftAssignOp:
        rv.handle(static_cast<SgLshiftAssignOp&>(*n));
        break;

      case V_SgRshiftAssignOp:
        rv.handle(static_cast<SgRshiftAssignOp&>(*n));
        break;

      case V_SgConcatenationOp:
        rv.handle(static_cast<SgConcatenationOp&>(*n));
        break;

      case V_SgBoolValExp:
        rv.handle(static_cast<SgBoolValExp&>(*n));
        break;

      case V_SgStringVal:
        rv.handle(static_cast<SgStringVal&>(*n));
        break;

      case V_SgShortVal:
        rv.handle(static_cast<SgShortVal&>(*n));
        break;

      case V_SgCharVal:
        rv.handle(static_cast<SgCharVal&>(*n));
        break;

      case V_SgUnsignedCharVal:
        rv.handle(static_cast<SgUnsignedCharVal&>(*n));
        break;

      case V_SgWcharVal:
        rv.handle(static_cast<SgWcharVal&>(*n));
        break;

      case V_SgUnsignedShortVal:
        rv.handle(static_cast<SgUnsignedShortVal&>(*n));
        break;

      case V_SgIntVal:
        rv.handle(static_cast<SgIntVal&>(*n));
        break;

      case V_SgEnumVal:
        rv.handle(static_cast<SgEnumVal&>(*n));
        break;

      case V_SgUnsignedIntVal:
        rv.handle(static_cast<SgUnsignedIntVal&>(*n));
        break;

      case V_SgLongIntVal:
        rv.handle(static_cast<SgLongIntVal&>(*n));
        break;

      case V_SgLongLongIntVal:
        rv.handle(static_cast<SgLongLongIntVal&>(*n));
        break;

      case V_SgUnsignedLongLongIntVal:
        rv.handle(static_cast<SgUnsignedLongLongIntVal&>(*n));
        break;

      case V_SgUnsignedLongVal:
        rv.handle(static_cast<SgUnsignedLongVal&>(*n));
        break;

      case V_SgFloatVal:
        rv.handle(static_cast<SgFloatVal&>(*n));
        break;

      case V_SgDoubleVal:
        rv.handle(static_cast<SgDoubleVal&>(*n));
        break;

      case V_SgLongDoubleVal:
        rv.handle(static_cast<SgLongDoubleVal&>(*n));
        break;

      case V_SgUpcThreads:
        rv.handle(static_cast<SgUpcThreads&>(*n));
        break;

      case V_SgUpcMythread:
        rv.handle(static_cast<SgUpcMythread&>(*n));
        break;

      case V_SgComplexVal:
        rv.handle(static_cast<SgComplexVal&>(*n));
        break;

      case V_SgNullExpression:
        rv.handle(static_cast<SgNullExpression&>(*n));
        break;

      case V_SgVariantExpression:
        rv.handle(static_cast<SgVariantExpression&>(*n));
        break;

      case V_SgStatementExpression:
        rv.handle(static_cast<SgStatementExpression&>(*n));
        break;

      case V_SgAsmOp:
        rv.handle(static_cast<SgAsmOp&>(*n));
        break;

      case V_SgCudaKernelExecConfig:
        rv.handle(static_cast<SgCudaKernelExecConfig&>(*n));
        break;

      case V_SgCudaKernelCallExp:
        rv.handle(static_cast<SgCudaKernelCallExp&>(*n));
        break;

      case V_SgSubscriptExpression:
        rv.handle(static_cast<SgSubscriptExpression&>(*n));
        break;

      case V_SgColonShapeExp:
        rv.handle(static_cast<SgColonShapeExp&>(*n));
        break;

      case V_SgAsteriskShapeExp:
        rv.handle(static_cast<SgAsteriskShapeExp&>(*n));
        break;

      case V_SgIOItemExpression:
        rv.handle(static_cast<SgIOItemExpression&>(*n));
        break;

      case V_SgImpliedDo:
        rv.handle(static_cast<SgImpliedDo&>(*n));
        break;

      case V_SgExponentiationOp:
        rv.handle(static_cast<SgExponentiationOp&>(*n));
        break;

      case V_SgUnknownArrayOrFunctionReference:
        rv.handle(static_cast<SgUnknownArrayOrFunctionReference&>(*n));
        break;

      case V_SgActualArgumentExpression:
        rv.handle(static_cast<SgActualArgumentExpression&>(*n));
        break;

      case V_SgUserDefinedBinaryOp:
        rv.handle(static_cast<SgUserDefinedBinaryOp&>(*n));
        break;

      case V_SgPointerAssignOp:
        rv.handle(static_cast<SgPointerAssignOp&>(*n));
        break;

      case V_SgCAFCoExpression:
        rv.handle(static_cast<SgCAFCoExpression&>(*n));
        break;

      case V_SgDesignatedInitializer:
        rv.handle(static_cast<SgDesignatedInitializer&>(*n));
        break;

      case V_SgInitializer:
        rv.handle(static_cast<SgInitializer&>(*n));
        break;

      case V_SgUserDefinedUnaryOp:
        rv.handle(static_cast<SgUserDefinedUnaryOp&>(*n));
        break;

      case V_SgPseudoDestructorRefExp:
        rv.handle(static_cast<SgPseudoDestructorRefExp&>(*n));
        break;

      case V_SgUnaryOp:
        rv.handle(static_cast<SgUnaryOp&>(*n));
        break;

      case V_SgBinaryOp:
        rv.handle(static_cast<SgBinaryOp&>(*n));
        break;

      case V_SgValueExp:
        rv.handle(static_cast<SgValueExp&>(*n));
        break;

      case V_SgExpression:
        rv.handle(static_cast<SgExpression&>(*n));
        break;

      case V_SgVariableSymbol:
        rv.handle(static_cast<SgVariableSymbol&>(*n));
        break;

      case V_SgFunctionTypeSymbol:
        rv.handle(static_cast<SgFunctionTypeSymbol&>(*n));
        break;

      case V_SgClassSymbol:
        rv.handle(static_cast<SgClassSymbol&>(*n));
        break;

      case V_SgTemplateSymbol:
        rv.handle(static_cast<SgTemplateSymbol&>(*n));
        break;

      case V_SgEnumSymbol:
        rv.handle(static_cast<SgEnumSymbol&>(*n));
        break;

      case V_SgEnumFieldSymbol:
        rv.handle(static_cast<SgEnumFieldSymbol&>(*n));
        break;

      case V_SgTypedefSymbol:
        rv.handle(static_cast<SgTypedefSymbol&>(*n));
        break;

      case V_SgMemberFunctionSymbol:
        rv.handle(static_cast<SgMemberFunctionSymbol&>(*n));
        break;

      case V_SgLabelSymbol:
        rv.handle(static_cast<SgLabelSymbol&>(*n));
        break;

      case V_SgDefaultSymbol:
        rv.handle(static_cast<SgDefaultSymbol&>(*n));
        break;

      case V_SgNamespaceSymbol:
        rv.handle(static_cast<SgNamespaceSymbol&>(*n));
        break;

      case V_SgIntrinsicSymbol:
        rv.handle(static_cast<SgIntrinsicSymbol&>(*n));
        break;

      case V_SgModuleSymbol:
        rv.handle(static_cast<SgModuleSymbol&>(*n));
        break;

      case V_SgInterfaceSymbol:
        rv.handle(static_cast<SgInterfaceSymbol&>(*n));
        break;

      case V_SgCommonSymbol:
        rv.handle(static_cast<SgCommonSymbol&>(*n));
        break;

      case V_SgRenameSymbol:
        rv.handle(static_cast<SgRenameSymbol&>(*n));
        break;

      case V_SgFunctionSymbol:
        rv.handle(static_cast<SgFunctionSymbol&>(*n));
        break;

      case V_SgAsmBinaryAddressSymbol:
        rv.handle(static_cast<SgAsmBinaryAddressSymbol&>(*n));
        break;

      case V_SgAsmBinaryDataSymbol:
        rv.handle(static_cast<SgAsmBinaryDataSymbol&>(*n));
        break;

      case V_SgAliasSymbol:
        rv.handle(static_cast<SgAliasSymbol&>(*n));
        break;

      case V_SgSymbol:
        rv.handle(static_cast<SgSymbol&>(*n));
        break;

      case V_SgAsmBlock:
        rv.handle(static_cast<SgAsmBlock&>(*n));
        break;

      case V_SgAsmOperandList:
        rv.handle(static_cast<SgAsmOperandList&>(*n));
        break;

      case V_SgAsmArmInstruction:
        rv.handle(static_cast<SgAsmArmInstruction&>(*n));
        break;

      case V_SgAsmx86Instruction:
        rv.handle(static_cast<SgAsmx86Instruction&>(*n));
        break;

      case V_SgAsmPowerpcInstruction:
        rv.handle(static_cast<SgAsmPowerpcInstruction&>(*n));
        break;

      case V_SgAsmInstruction:
        rv.handle(static_cast<SgAsmInstruction&>(*n));
        break;

      case V_SgAsmDataStructureDeclaration:
        rv.handle(static_cast<SgAsmDataStructureDeclaration&>(*n));
        break;

      case V_SgAsmFunctionDeclaration:
        rv.handle(static_cast<SgAsmFunctionDeclaration&>(*n));
        break;

      case V_SgAsmFieldDeclaration:
        rv.handle(static_cast<SgAsmFieldDeclaration&>(*n));
        break;

      case V_SgAsmDeclaration:
        rv.handle(static_cast<SgAsmDeclaration&>(*n));
        break;

      case V_SgAsmStatement:
        rv.handle(static_cast<SgAsmStatement&>(*n));
        break;

      case V_SgAsmBinaryAdd:
        rv.handle(static_cast<SgAsmBinaryAdd&>(*n));
        break;

      case V_SgAsmBinarySubtract:
        rv.handle(static_cast<SgAsmBinarySubtract&>(*n));
        break;

      case V_SgAsmBinaryMultiply:
        rv.handle(static_cast<SgAsmBinaryMultiply&>(*n));
        break;

      case V_SgAsmBinaryDivide:
        rv.handle(static_cast<SgAsmBinaryDivide&>(*n));
        break;

      case V_SgAsmBinaryMod:
        rv.handle(static_cast<SgAsmBinaryMod&>(*n));
        break;

      case V_SgAsmBinaryAddPreupdate:
        rv.handle(static_cast<SgAsmBinaryAddPreupdate&>(*n));
        break;

      case V_SgAsmBinarySubtractPreupdate:
        rv.handle(static_cast<SgAsmBinarySubtractPreupdate&>(*n));
        break;

      case V_SgAsmBinaryAddPostupdate:
        rv.handle(static_cast<SgAsmBinaryAddPostupdate&>(*n));
        break;

      case V_SgAsmBinarySubtractPostupdate:
        rv.handle(static_cast<SgAsmBinarySubtractPostupdate&>(*n));
        break;

      case V_SgAsmBinaryLsl:
        rv.handle(static_cast<SgAsmBinaryLsl&>(*n));
        break;

      case V_SgAsmBinaryLsr:
        rv.handle(static_cast<SgAsmBinaryLsr&>(*n));
        break;

      case V_SgAsmBinaryAsr:
        rv.handle(static_cast<SgAsmBinaryAsr&>(*n));
        break;

      case V_SgAsmBinaryRor:
        rv.handle(static_cast<SgAsmBinaryRor&>(*n));
        break;

      case V_SgAsmBinaryExpression:
        rv.handle(static_cast<SgAsmBinaryExpression&>(*n));
        break;

      case V_SgAsmUnaryPlus:
        rv.handle(static_cast<SgAsmUnaryPlus&>(*n));
        break;

      case V_SgAsmUnaryMinus:
        rv.handle(static_cast<SgAsmUnaryMinus&>(*n));
        break;

      case V_SgAsmUnaryRrx:
        rv.handle(static_cast<SgAsmUnaryRrx&>(*n));
        break;

      case V_SgAsmUnaryArmSpecialRegisterList:
        rv.handle(static_cast<SgAsmUnaryArmSpecialRegisterList&>(*n));
        break;

      case V_SgAsmUnaryExpression:
        rv.handle(static_cast<SgAsmUnaryExpression&>(*n));
        break;

      case V_SgAsmMemoryReferenceExpression:
        rv.handle(static_cast<SgAsmMemoryReferenceExpression&>(*n));
        break;

      case V_SgAsmControlFlagsExpression:
        rv.handle(static_cast<SgAsmControlFlagsExpression&>(*n));
        break;

      case V_SgAsmCommonSubExpression:
        rv.handle(static_cast<SgAsmCommonSubExpression&>(*n));
        break;

      case V_SgAsmx86RegisterReferenceExpression:
        rv.handle(static_cast<SgAsmx86RegisterReferenceExpression&>(*n));
        break;

      case V_SgAsmArmRegisterReferenceExpression:
        rv.handle(static_cast<SgAsmArmRegisterReferenceExpression&>(*n));
        break;

      case V_SgAsmPowerpcRegisterReferenceExpression:
        rv.handle(static_cast<SgAsmPowerpcRegisterReferenceExpression&>(*n));
        break;

      case V_SgAsmRegisterReferenceExpression:
        rv.handle(static_cast<SgAsmRegisterReferenceExpression&>(*n));
        break;

      case V_SgAsmByteValueExpression:
        rv.handle(static_cast<SgAsmByteValueExpression&>(*n));
        break;

      case V_SgAsmWordValueExpression:
        rv.handle(static_cast<SgAsmWordValueExpression&>(*n));
        break;

      case V_SgAsmDoubleWordValueExpression:
        rv.handle(static_cast<SgAsmDoubleWordValueExpression&>(*n));
        break;

      case V_SgAsmQuadWordValueExpression:
        rv.handle(static_cast<SgAsmQuadWordValueExpression&>(*n));
        break;

      case V_SgAsmSingleFloatValueExpression:
        rv.handle(static_cast<SgAsmSingleFloatValueExpression&>(*n));
        break;

      case V_SgAsmDoubleFloatValueExpression:
        rv.handle(static_cast<SgAsmDoubleFloatValueExpression&>(*n));
        break;

      case V_SgAsmVectorValueExpression:
        rv.handle(static_cast<SgAsmVectorValueExpression&>(*n));
        break;

      case V_SgAsmValueExpression:
        rv.handle(static_cast<SgAsmValueExpression&>(*n));
        break;

      case V_SgAsmExprListExp:
        rv.handle(static_cast<SgAsmExprListExp&>(*n));
        break;

      case V_SgAsmExpression:
        rv.handle(static_cast<SgAsmExpression&>(*n));
        break;

      case V_SgAsmTypeByte:
        rv.handle(static_cast<SgAsmTypeByte&>(*n));
        break;

      case V_SgAsmTypeWord:
        rv.handle(static_cast<SgAsmTypeWord&>(*n));
        break;

      case V_SgAsmTypeDoubleWord:
        rv.handle(static_cast<SgAsmTypeDoubleWord&>(*n));
        break;

      case V_SgAsmTypeQuadWord:
        rv.handle(static_cast<SgAsmTypeQuadWord&>(*n));
        break;

      case V_SgAsmTypeDoubleQuadWord:
        rv.handle(static_cast<SgAsmTypeDoubleQuadWord&>(*n));
        break;

      case V_SgAsmTypeSingleFloat:
        rv.handle(static_cast<SgAsmTypeSingleFloat&>(*n));
        break;

      case V_SgAsmTypeDoubleFloat:
        rv.handle(static_cast<SgAsmTypeDoubleFloat&>(*n));
        break;

      case V_SgAsmType80bitFloat:
        rv.handle(static_cast<SgAsmType80bitFloat&>(*n));
        break;

      case V_SgAsmType128bitFloat:
        rv.handle(static_cast<SgAsmType128bitFloat&>(*n));
        break;

      case V_SgAsmTypeVector:
        rv.handle(static_cast<SgAsmTypeVector&>(*n));
        break;

      case V_SgAsmType:
        rv.handle(static_cast<SgAsmType&>(*n));
        break;

      case V_SgAsmGenericDLL:
        rv.handle(static_cast<SgAsmGenericDLL&>(*n));
        break;

      case V_SgAsmPEImportHNTEntryList:
        rv.handle(static_cast<SgAsmPEImportHNTEntryList&>(*n));
        break;

      case V_SgAsmPEImportILTEntryList:
        rv.handle(static_cast<SgAsmPEImportILTEntryList&>(*n));
        break;

      case V_SgAsmPEImportDirectoryList:
        rv.handle(static_cast<SgAsmPEImportDirectoryList&>(*n));
        break;

      case V_SgAsmGenericFormat:
        rv.handle(static_cast<SgAsmGenericFormat&>(*n));
        break;

      case V_SgAsmGenericFile:
        rv.handle(static_cast<SgAsmGenericFile&>(*n));
        break;

      case V_SgAsmElfFileHeader:
        rv.handle(static_cast<SgAsmElfFileHeader&>(*n));
        break;

      case V_SgAsmPEFileHeader:
        rv.handle(static_cast<SgAsmPEFileHeader&>(*n));
        break;

      case V_SgAsmNEFileHeader:
        rv.handle(static_cast<SgAsmNEFileHeader&>(*n));
        break;

      case V_SgAsmLEFileHeader:
        rv.handle(static_cast<SgAsmLEFileHeader&>(*n));
        break;

      case V_SgAsmDOSFileHeader:
        rv.handle(static_cast<SgAsmDOSFileHeader&>(*n));
        break;

      case V_SgAsmGenericHeader:
        rv.handle(static_cast<SgAsmGenericHeader&>(*n));
        break;

      case V_SgAsmElfRelocSection:
        rv.handle(static_cast<SgAsmElfRelocSection&>(*n));
        break;

      case V_SgAsmElfDynamicSection:
        rv.handle(static_cast<SgAsmElfDynamicSection&>(*n));
        break;

      case V_SgAsmElfSymbolSection:
        rv.handle(static_cast<SgAsmElfSymbolSection&>(*n));
        break;

      case V_SgAsmElfStringSection:
        rv.handle(static_cast<SgAsmElfStringSection&>(*n));
        break;

      case V_SgAsmElfEHFrameSection:
        rv.handle(static_cast<SgAsmElfEHFrameSection&>(*n));
        break;

      case V_SgAsmElfNoteSection:
        rv.handle(static_cast<SgAsmElfNoteSection&>(*n));
        break;

      case V_SgAsmElfSymverSection:
        rv.handle(static_cast<SgAsmElfSymverSection&>(*n));
        break;

      case V_SgAsmElfSymverDefinedSection:
        rv.handle(static_cast<SgAsmElfSymverDefinedSection&>(*n));
        break;

      case V_SgAsmElfSymverNeededSection:
        rv.handle(static_cast<SgAsmElfSymverNeededSection&>(*n));
        break;

      case V_SgAsmElfStrtab:
        rv.handle(static_cast<SgAsmElfStrtab&>(*n));
        break;

      case V_SgAsmCoffStrtab:
        rv.handle(static_cast<SgAsmCoffStrtab&>(*n));
        break;

      case V_SgAsmGenericStrtab:
        rv.handle(static_cast<SgAsmGenericStrtab&>(*n));
        break;

      case V_SgAsmElfSection:
        rv.handle(static_cast<SgAsmElfSection&>(*n));
        break;

      case V_SgAsmElfSectionTable:
        rv.handle(static_cast<SgAsmElfSectionTable&>(*n));
        break;

      case V_SgAsmElfSegmentTable:
        rv.handle(static_cast<SgAsmElfSegmentTable&>(*n));
        break;

      case V_SgAsmPEImportSection:
        rv.handle(static_cast<SgAsmPEImportSection&>(*n));
        break;

      case V_SgAsmPEExportSection:
        rv.handle(static_cast<SgAsmPEExportSection&>(*n));
        break;

      case V_SgAsmPEStringSection:
        rv.handle(static_cast<SgAsmPEStringSection&>(*n));
        break;

      case V_SgAsmPESection:
        rv.handle(static_cast<SgAsmPESection&>(*n));
        break;

      case V_SgAsmPESectionTable:
        rv.handle(static_cast<SgAsmPESectionTable&>(*n));
        break;

      case V_SgAsmCoffSymbolTable:
        rv.handle(static_cast<SgAsmCoffSymbolTable&>(*n));
        break;

      case V_SgAsmDOSExtendedHeader:
        rv.handle(static_cast<SgAsmDOSExtendedHeader&>(*n));
        break;

      case V_SgAsmNESection:
        rv.handle(static_cast<SgAsmNESection&>(*n));
        break;

      case V_SgAsmNESectionTable:
        rv.handle(static_cast<SgAsmNESectionTable&>(*n));
        break;

      case V_SgAsmNENameTable:
        rv.handle(static_cast<SgAsmNENameTable&>(*n));
        break;

      case V_SgAsmNEModuleTable:
        rv.handle(static_cast<SgAsmNEModuleTable&>(*n));
        break;

      case V_SgAsmNEStringTable:
        rv.handle(static_cast<SgAsmNEStringTable&>(*n));
        break;

      case V_SgAsmNEEntryTable:
        rv.handle(static_cast<SgAsmNEEntryTable&>(*n));
        break;

      case V_SgAsmNERelocTable:
        rv.handle(static_cast<SgAsmNERelocTable&>(*n));
        break;

      case V_SgAsmLESection:
        rv.handle(static_cast<SgAsmLESection&>(*n));
        break;

      case V_SgAsmLESectionTable:
        rv.handle(static_cast<SgAsmLESectionTable&>(*n));
        break;

      case V_SgAsmLENameTable:
        rv.handle(static_cast<SgAsmLENameTable&>(*n));
        break;

      case V_SgAsmLEPageTable:
        rv.handle(static_cast<SgAsmLEPageTable&>(*n));
        break;

      case V_SgAsmLEEntryTable:
        rv.handle(static_cast<SgAsmLEEntryTable&>(*n));
        break;

      case V_SgAsmLERelocTable:
        rv.handle(static_cast<SgAsmLERelocTable&>(*n));
        break;

      case V_SgAsmGenericSection:
        rv.handle(static_cast<SgAsmGenericSection&>(*n));
        break;

      case V_SgAsmCoffSymbol:
        rv.handle(static_cast<SgAsmCoffSymbol&>(*n));
        break;

      case V_SgAsmElfSymbol:
        rv.handle(static_cast<SgAsmElfSymbol&>(*n));
        break;

      case V_SgAsmGenericSymbol:
        rv.handle(static_cast<SgAsmGenericSymbol&>(*n));
        break;

      case V_SgAsmElfSectionTableEntry:
        rv.handle(static_cast<SgAsmElfSectionTableEntry&>(*n));
        break;

      case V_SgAsmElfSegmentTableEntry:
        rv.handle(static_cast<SgAsmElfSegmentTableEntry&>(*n));
        break;

      case V_SgAsmElfSegmentTableEntryList:
        rv.handle(static_cast<SgAsmElfSegmentTableEntryList&>(*n));
        break;

      case V_SgAsmElfRelocEntry:
        rv.handle(static_cast<SgAsmElfRelocEntry&>(*n));
        break;

      case V_SgAsmElfRelocEntryList:
        rv.handle(static_cast<SgAsmElfRelocEntryList&>(*n));
        break;

      case V_SgAsmElfDynamicEntry:
        rv.handle(static_cast<SgAsmElfDynamicEntry&>(*n));
        break;

      case V_SgAsmElfDynamicEntryList:
        rv.handle(static_cast<SgAsmElfDynamicEntryList&>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCI:
        rv.handle(static_cast<SgAsmElfEHFrameEntryCI&>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCIList:
        rv.handle(static_cast<SgAsmElfEHFrameEntryCIList&>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFD:
        rv.handle(static_cast<SgAsmElfEHFrameEntryFD&>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFDList:
        rv.handle(static_cast<SgAsmElfEHFrameEntryFDList&>(*n));
        break;

      case V_SgAsmElfNoteEntry:
        rv.handle(static_cast<SgAsmElfNoteEntry&>(*n));
        break;

      case V_SgAsmElfNoteEntryList:
        rv.handle(static_cast<SgAsmElfNoteEntryList&>(*n));
        break;

      case V_SgAsmElfSymverEntry:
        rv.handle(static_cast<SgAsmElfSymverEntry&>(*n));
        break;

      case V_SgAsmElfSymverEntryList:
        rv.handle(static_cast<SgAsmElfSymverEntryList&>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntry:
        rv.handle(static_cast<SgAsmElfSymverDefinedEntry&>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntryList:
        rv.handle(static_cast<SgAsmElfSymverDefinedEntryList&>(*n));
        break;

      case V_SgAsmElfSymverDefinedAux:
        rv.handle(static_cast<SgAsmElfSymverDefinedAux&>(*n));
        break;

      case V_SgAsmElfSymverDefinedAuxList:
        rv.handle(static_cast<SgAsmElfSymverDefinedAuxList&>(*n));
        break;

      case V_SgAsmElfSymverNeededEntry:
        rv.handle(static_cast<SgAsmElfSymverNeededEntry&>(*n));
        break;

      case V_SgAsmElfSymverNeededEntryList:
        rv.handle(static_cast<SgAsmElfSymverNeededEntryList&>(*n));
        break;

      case V_SgAsmElfSymverNeededAux:
        rv.handle(static_cast<SgAsmElfSymverNeededAux&>(*n));
        break;

      case V_SgAsmElfSymverNeededAuxList:
        rv.handle(static_cast<SgAsmElfSymverNeededAuxList&>(*n));
        break;

      case V_SgAsmPERVASizePair:
        rv.handle(static_cast<SgAsmPERVASizePair&>(*n));
        break;

      case V_SgAsmPEExportDirectory:
        rv.handle(static_cast<SgAsmPEExportDirectory&>(*n));
        break;

      case V_SgAsmPEExportEntry:
        rv.handle(static_cast<SgAsmPEExportEntry&>(*n));
        break;

      case V_SgAsmPEImportDirectory:
        rv.handle(static_cast<SgAsmPEImportDirectory&>(*n));
        break;

      case V_SgAsmPEImportILTEntry:
        rv.handle(static_cast<SgAsmPEImportILTEntry&>(*n));
        break;

      case V_SgAsmPEImportHNTEntry:
        rv.handle(static_cast<SgAsmPEImportHNTEntry&>(*n));
        break;

      case V_SgAsmPEImportLookupTable:
        rv.handle(static_cast<SgAsmPEImportLookupTable&>(*n));
        break;

      case V_SgAsmPESectionTableEntry:
        rv.handle(static_cast<SgAsmPESectionTableEntry&>(*n));
        break;

      case V_SgAsmNEEntryPoint:
        rv.handle(static_cast<SgAsmNEEntryPoint&>(*n));
        break;

      case V_SgAsmNERelocEntry:
        rv.handle(static_cast<SgAsmNERelocEntry&>(*n));
        break;

      case V_SgAsmNESectionTableEntry:
        rv.handle(static_cast<SgAsmNESectionTableEntry&>(*n));
        break;

      case V_SgAsmLEPageTableEntry:
        rv.handle(static_cast<SgAsmLEPageTableEntry&>(*n));
        break;

      case V_SgAsmLEEntryPoint:
        rv.handle(static_cast<SgAsmLEEntryPoint&>(*n));
        break;

      case V_SgAsmLESectionTableEntry:
        rv.handle(static_cast<SgAsmLESectionTableEntry&>(*n));
        break;

      case V_SgAsmGenericSectionList:
        rv.handle(static_cast<SgAsmGenericSectionList&>(*n));
        break;

      case V_SgAsmGenericHeaderList:
        rv.handle(static_cast<SgAsmGenericHeaderList&>(*n));
        break;

      case V_SgAsmGenericSymbolList:
        rv.handle(static_cast<SgAsmGenericSymbolList&>(*n));
        break;

      case V_SgAsmElfSymbolList:
        rv.handle(static_cast<SgAsmElfSymbolList&>(*n));
        break;

      case V_SgAsmCoffSymbolList:
        rv.handle(static_cast<SgAsmCoffSymbolList&>(*n));
        break;

      case V_SgAsmGenericDLLList:
        rv.handle(static_cast<SgAsmGenericDLLList&>(*n));
        break;

      case V_SgAsmPERVASizePairList:
        rv.handle(static_cast<SgAsmPERVASizePairList&>(*n));
        break;

      case V_SgAsmPEExportEntryList:
        rv.handle(static_cast<SgAsmPEExportEntryList&>(*n));
        break;

      case V_SgAsmBasicString:
        rv.handle(static_cast<SgAsmBasicString&>(*n));
        break;

      case V_SgAsmStoredString:
        rv.handle(static_cast<SgAsmStoredString&>(*n));
        break;

      case V_SgAsmGenericString:
        rv.handle(static_cast<SgAsmGenericString&>(*n));
        break;

      case V_SgAsmStringStorage:
        rv.handle(static_cast<SgAsmStringStorage&>(*n));
        break;

      case V_SgAsmDwarfMacro:
        rv.handle(static_cast<SgAsmDwarfMacro&>(*n));
        break;

      case V_SgAsmDwarfLine:
        rv.handle(static_cast<SgAsmDwarfLine&>(*n));
        break;

      case V_SgAsmDwarfMacroList:
        rv.handle(static_cast<SgAsmDwarfMacroList&>(*n));
        break;

      case V_SgAsmDwarfLineList:
        rv.handle(static_cast<SgAsmDwarfLineList&>(*n));
        break;

      case V_SgAsmDwarfArrayType:
        rv.handle(static_cast<SgAsmDwarfArrayType&>(*n));
        break;

      case V_SgAsmDwarfClassType:
        rv.handle(static_cast<SgAsmDwarfClassType&>(*n));
        break;

      case V_SgAsmDwarfEntryPoint:
        rv.handle(static_cast<SgAsmDwarfEntryPoint&>(*n));
        break;

      case V_SgAsmDwarfEnumerationType:
        rv.handle(static_cast<SgAsmDwarfEnumerationType&>(*n));
        break;

      case V_SgAsmDwarfFormalParameter:
        rv.handle(static_cast<SgAsmDwarfFormalParameter&>(*n));
        break;

      case V_SgAsmDwarfImportedDeclaration:
        rv.handle(static_cast<SgAsmDwarfImportedDeclaration&>(*n));
        break;

      case V_SgAsmDwarfLabel:
        rv.handle(static_cast<SgAsmDwarfLabel&>(*n));
        break;

      case V_SgAsmDwarfLexicalBlock:
        rv.handle(static_cast<SgAsmDwarfLexicalBlock&>(*n));
        break;

      case V_SgAsmDwarfMember:
        rv.handle(static_cast<SgAsmDwarfMember&>(*n));
        break;

      case V_SgAsmDwarfPointerType:
        rv.handle(static_cast<SgAsmDwarfPointerType&>(*n));
        break;

      case V_SgAsmDwarfReferenceType:
        rv.handle(static_cast<SgAsmDwarfReferenceType&>(*n));
        break;

      case V_SgAsmDwarfCompilationUnit:
        rv.handle(static_cast<SgAsmDwarfCompilationUnit&>(*n));
        break;

      case V_SgAsmDwarfStringType:
        rv.handle(static_cast<SgAsmDwarfStringType&>(*n));
        break;

      case V_SgAsmDwarfStructureType:
        rv.handle(static_cast<SgAsmDwarfStructureType&>(*n));
        break;

      case V_SgAsmDwarfSubroutineType:
        rv.handle(static_cast<SgAsmDwarfSubroutineType&>(*n));
        break;

      case V_SgAsmDwarfTypedef:
        rv.handle(static_cast<SgAsmDwarfTypedef&>(*n));
        break;

      case V_SgAsmDwarfUnionType:
        rv.handle(static_cast<SgAsmDwarfUnionType&>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedParameters:
        rv.handle(static_cast<SgAsmDwarfUnspecifiedParameters&>(*n));
        break;

      case V_SgAsmDwarfVariant:
        rv.handle(static_cast<SgAsmDwarfVariant&>(*n));
        break;

      case V_SgAsmDwarfCommonBlock:
        rv.handle(static_cast<SgAsmDwarfCommonBlock&>(*n));
        break;

      case V_SgAsmDwarfCommonInclusion:
        rv.handle(static_cast<SgAsmDwarfCommonInclusion&>(*n));
        break;

      case V_SgAsmDwarfInheritance:
        rv.handle(static_cast<SgAsmDwarfInheritance&>(*n));
        break;

      case V_SgAsmDwarfInlinedSubroutine:
        rv.handle(static_cast<SgAsmDwarfInlinedSubroutine&>(*n));
        break;

      case V_SgAsmDwarfModule:
        rv.handle(static_cast<SgAsmDwarfModule&>(*n));
        break;

      case V_SgAsmDwarfPtrToMemberType:
        rv.handle(static_cast<SgAsmDwarfPtrToMemberType&>(*n));
        break;

      case V_SgAsmDwarfSetType:
        rv.handle(static_cast<SgAsmDwarfSetType&>(*n));
        break;

      case V_SgAsmDwarfSubrangeType:
        rv.handle(static_cast<SgAsmDwarfSubrangeType&>(*n));
        break;

      case V_SgAsmDwarfWithStmt:
        rv.handle(static_cast<SgAsmDwarfWithStmt&>(*n));
        break;

      case V_SgAsmDwarfAccessDeclaration:
        rv.handle(static_cast<SgAsmDwarfAccessDeclaration&>(*n));
        break;

      case V_SgAsmDwarfBaseType:
        rv.handle(static_cast<SgAsmDwarfBaseType&>(*n));
        break;

      case V_SgAsmDwarfCatchBlock:
        rv.handle(static_cast<SgAsmDwarfCatchBlock&>(*n));
        break;

      case V_SgAsmDwarfConstType:
        rv.handle(static_cast<SgAsmDwarfConstType&>(*n));
        break;

      case V_SgAsmDwarfConstant:
        rv.handle(static_cast<SgAsmDwarfConstant&>(*n));
        break;

      case V_SgAsmDwarfEnumerator:
        rv.handle(static_cast<SgAsmDwarfEnumerator&>(*n));
        break;

      case V_SgAsmDwarfFileType:
        rv.handle(static_cast<SgAsmDwarfFileType&>(*n));
        break;

      case V_SgAsmDwarfFriend:
        rv.handle(static_cast<SgAsmDwarfFriend&>(*n));
        break;

      case V_SgAsmDwarfNamelist:
        rv.handle(static_cast<SgAsmDwarfNamelist&>(*n));
        break;

      case V_SgAsmDwarfNamelistItem:
        rv.handle(static_cast<SgAsmDwarfNamelistItem&>(*n));
        break;

      case V_SgAsmDwarfPackedType:
        rv.handle(static_cast<SgAsmDwarfPackedType&>(*n));
        break;

      case V_SgAsmDwarfSubprogram:
        rv.handle(static_cast<SgAsmDwarfSubprogram&>(*n));
        break;

      case V_SgAsmDwarfTemplateTypeParameter:
        rv.handle(static_cast<SgAsmDwarfTemplateTypeParameter&>(*n));
        break;

      case V_SgAsmDwarfTemplateValueParameter:
        rv.handle(static_cast<SgAsmDwarfTemplateValueParameter&>(*n));
        break;

      case V_SgAsmDwarfThrownType:
        rv.handle(static_cast<SgAsmDwarfThrownType&>(*n));
        break;

      case V_SgAsmDwarfTryBlock:
        rv.handle(static_cast<SgAsmDwarfTryBlock&>(*n));
        break;

      case V_SgAsmDwarfVariantPart:
        rv.handle(static_cast<SgAsmDwarfVariantPart&>(*n));
        break;

      case V_SgAsmDwarfVariable:
        rv.handle(static_cast<SgAsmDwarfVariable&>(*n));
        break;

      case V_SgAsmDwarfVolatileType:
        rv.handle(static_cast<SgAsmDwarfVolatileType&>(*n));
        break;

      case V_SgAsmDwarfDwarfProcedure:
        rv.handle(static_cast<SgAsmDwarfDwarfProcedure&>(*n));
        break;

      case V_SgAsmDwarfRestrictType:
        rv.handle(static_cast<SgAsmDwarfRestrictType&>(*n));
        break;

      case V_SgAsmDwarfInterfaceType:
        rv.handle(static_cast<SgAsmDwarfInterfaceType&>(*n));
        break;

      case V_SgAsmDwarfNamespace:
        rv.handle(static_cast<SgAsmDwarfNamespace&>(*n));
        break;

      case V_SgAsmDwarfImportedModule:
        rv.handle(static_cast<SgAsmDwarfImportedModule&>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedType:
        rv.handle(static_cast<SgAsmDwarfUnspecifiedType&>(*n));
        break;

      case V_SgAsmDwarfPartialUnit:
        rv.handle(static_cast<SgAsmDwarfPartialUnit&>(*n));
        break;

      case V_SgAsmDwarfImportedUnit:
        rv.handle(static_cast<SgAsmDwarfImportedUnit&>(*n));
        break;

      case V_SgAsmDwarfMutableType:
        rv.handle(static_cast<SgAsmDwarfMutableType&>(*n));
        break;

      case V_SgAsmDwarfCondition:
        rv.handle(static_cast<SgAsmDwarfCondition&>(*n));
        break;

      case V_SgAsmDwarfSharedType:
        rv.handle(static_cast<SgAsmDwarfSharedType&>(*n));
        break;

      case V_SgAsmDwarfFormatLabel:
        rv.handle(static_cast<SgAsmDwarfFormatLabel&>(*n));
        break;

      case V_SgAsmDwarfFunctionTemplate:
        rv.handle(static_cast<SgAsmDwarfFunctionTemplate&>(*n));
        break;

      case V_SgAsmDwarfClassTemplate:
        rv.handle(static_cast<SgAsmDwarfClassTemplate&>(*n));
        break;

      case V_SgAsmDwarfUpcSharedType:
        rv.handle(static_cast<SgAsmDwarfUpcSharedType&>(*n));
        break;

      case V_SgAsmDwarfUpcStrictType:
        rv.handle(static_cast<SgAsmDwarfUpcStrictType&>(*n));
        break;

      case V_SgAsmDwarfUpcRelaxedType:
        rv.handle(static_cast<SgAsmDwarfUpcRelaxedType&>(*n));
        break;

      case V_SgAsmDwarfUnknownConstruct:
        rv.handle(static_cast<SgAsmDwarfUnknownConstruct&>(*n));
        break;

      case V_SgAsmDwarfConstruct:
        rv.handle(static_cast<SgAsmDwarfConstruct&>(*n));
        break;

      case V_SgAsmDwarfConstructList:
        rv.handle(static_cast<SgAsmDwarfConstructList&>(*n));
        break;

      case V_SgAsmDwarfCompilationUnitList:
        rv.handle(static_cast<SgAsmDwarfCompilationUnitList&>(*n));
        break;

      case V_SgAsmDwarfInformation:
        rv.handle(static_cast<SgAsmDwarfInformation&>(*n));
        break;

      case V_SgAsmExecutableFileFormat:
        rv.handle(static_cast<SgAsmExecutableFileFormat&>(*n));
        break;

      case V_SgAsmInterpretation:
        rv.handle(static_cast<SgAsmInterpretation&>(*n));
        break;

      case V_SgAsmInterpretationList:
        rv.handle(static_cast<SgAsmInterpretationList&>(*n));
        break;

      case V_SgAsmGenericFileList:
        rv.handle(static_cast<SgAsmGenericFileList&>(*n));
        break;

      case V_SgAsmNode:
        rv.handle(static_cast<SgAsmNode&>(*n));
        break;

      case V_SgInitializedName:
        rv.handle(static_cast<SgInitializedName&>(*n));
        break;

      case V_SgOmpOrderedClause:
        rv.handle(static_cast<SgOmpOrderedClause&>(*n));
        break;

      case V_SgOmpNowaitClause:
        rv.handle(static_cast<SgOmpNowaitClause&>(*n));
        break;

      case V_SgOmpUntiedClause:
        rv.handle(static_cast<SgOmpUntiedClause&>(*n));
        break;

      case V_SgOmpDefaultClause:
        rv.handle(static_cast<SgOmpDefaultClause&>(*n));
        break;

      case V_SgOmpCollapseClause:
        rv.handle(static_cast<SgOmpCollapseClause&>(*n));
        break;

      case V_SgOmpIfClause:
        rv.handle(static_cast<SgOmpIfClause&>(*n));
        break;

      case V_SgOmpNumThreadsClause:
        rv.handle(static_cast<SgOmpNumThreadsClause&>(*n));
        break;

      case V_SgOmpExpressionClause:
        rv.handle(static_cast<SgOmpExpressionClause&>(*n));
        break;

      case V_SgOmpCopyprivateClause:
        rv.handle(static_cast<SgOmpCopyprivateClause&>(*n));
        break;

      case V_SgOmpPrivateClause:
        rv.handle(static_cast<SgOmpPrivateClause&>(*n));
        break;

      case V_SgOmpFirstprivateClause:
        rv.handle(static_cast<SgOmpFirstprivateClause&>(*n));
        break;

      case V_SgOmpSharedClause:
        rv.handle(static_cast<SgOmpSharedClause&>(*n));
        break;

      case V_SgOmpCopyinClause:
        rv.handle(static_cast<SgOmpCopyinClause&>(*n));
        break;

      case V_SgOmpLastprivateClause:
        rv.handle(static_cast<SgOmpLastprivateClause&>(*n));
        break;

      case V_SgOmpReductionClause:
        rv.handle(static_cast<SgOmpReductionClause&>(*n));
        break;

      case V_SgOmpVariablesClause:
        rv.handle(static_cast<SgOmpVariablesClause&>(*n));
        break;

      case V_SgOmpScheduleClause:
        rv.handle(static_cast<SgOmpScheduleClause&>(*n));
        break;

      case V_SgOmpClause:
        rv.handle(static_cast<SgOmpClause&>(*n));
        break;

      case V_SgRenamePair:
        rv.handle(static_cast<SgRenamePair&>(*n));
        break;

      case V_SgInterfaceBody:
        rv.handle(static_cast<SgInterfaceBody&>(*n));
        break;

      case V_SgLocatedNodeSupport:
        rv.handle(static_cast<SgLocatedNodeSupport&>(*n));
        break;

      case V_SgToken:
        rv.handle(static_cast<SgToken&>(*n));
        break;

      case V_SgLocatedNode:
        rv.handle(static_cast<SgLocatedNode&>(*n));
        break;

      case V_SgNode:
        rv.handle(static_cast<SgNode&>(*n));
        break;

      //
      // Types

      case V_SgTypeUnknown:
        rv.handle(static_cast<SgTypeUnknown&>(*n));
        break;

      case V_SgTypeChar:
        rv.handle(static_cast<SgTypeChar&>(*n));
        break;

      case V_SgTypeSignedChar:
        rv.handle(static_cast<SgTypeSignedChar&>(*n));
        break;

      case V_SgTypeUnsignedChar:
        rv.handle(static_cast<SgTypeUnsignedChar&>(*n));
        break;

      case V_SgTypeShort:
        rv.handle(static_cast<SgTypeShort&>(*n));
        break;

      case V_SgTypeSignedShort:
        rv.handle(static_cast<SgTypeSignedShort&>(*n));
        break;

      case V_SgTypeUnsignedShort:
        rv.handle(static_cast<SgTypeUnsignedShort&>(*n));
        break;

      case V_SgTypeInt:
        rv.handle(static_cast<SgTypeInt&>(*n));
        break;

      case V_SgTypeSignedInt:
        rv.handle(static_cast<SgTypeSignedInt&>(*n));
        break;

      case V_SgTypeUnsignedInt:
        rv.handle(static_cast<SgTypeUnsignedInt&>(*n));
        break;

      case V_SgTypeLong:
        rv.handle(static_cast<SgTypeLong&>(*n));
        break;

      case V_SgTypeSignedLong:
        rv.handle(static_cast<SgTypeSignedLong&>(*n));
        break;

      case V_SgTypeUnsignedLong:
        rv.handle(static_cast<SgTypeUnsignedLong&>(*n));
        break;

      case V_SgTypeVoid:
        rv.handle(static_cast<SgTypeVoid&>(*n));
        break;

      case V_SgTypeGlobalVoid:
        rv.handle(static_cast<SgTypeGlobalVoid&>(*n));
        break;

      case V_SgTypeWchar:
        rv.handle(static_cast<SgTypeWchar&>(*n));
        break;

      case V_SgTypeFloat:
        rv.handle(static_cast<SgTypeFloat&>(*n));
        break;

      case V_SgTypeDouble:
        rv.handle(static_cast<SgTypeDouble&>(*n));
        break;

      case V_SgTypeLongLong:
        rv.handle(static_cast<SgTypeLongLong&>(*n));
        break;

      case V_SgTypeSignedLongLong:
        rv.handle(static_cast<SgTypeSignedLongLong&>(*n));
        break;

      case V_SgTypeUnsignedLongLong:
        rv.handle(static_cast<SgTypeUnsignedLongLong&>(*n));
        break;

      case V_SgTypeLongDouble:
        rv.handle(static_cast<SgTypeLongDouble&>(*n));
        break;

      case V_SgTypeString:
        rv.handle(static_cast<SgTypeString&>(*n));
        break;

      case V_SgTypeBool:
        rv.handle(static_cast<SgTypeBool&>(*n));
        break;

      case V_SgTypeComplex:
        rv.handle(static_cast<SgTypeComplex&>(*n));
        break;

      case V_SgTypeImaginary:
        rv.handle(static_cast<SgTypeImaginary&>(*n));
        break;

      case V_SgTypeDefault:
        rv.handle(static_cast<SgTypeDefault&>(*n));
        break;

      case V_SgPointerMemberType:
        rv.handle(static_cast<SgPointerMemberType&>(*n));
        break;

      case V_SgReferenceType:
        rv.handle(static_cast<SgReferenceType&>(*n));
        break;

      case V_SgTypeCAFTeam:
        rv.handle(static_cast<SgTypeCAFTeam&>(*n));
        break;

      case V_SgClassType:
        rv.handle(static_cast<SgClassType&>(*n));
        break;

      case V_SgTemplateType:
        rv.handle(static_cast<SgTemplateType&>(*n));
        break;

      case V_SgEnumType:
        rv.handle(static_cast<SgEnumType&>(*n));
        break;

      case V_SgTypedefType:
        rv.handle(static_cast<SgTypedefType&>(*n));
        break;

      case V_SgModifierType:
        rv.handle(static_cast<SgModifierType&>(*n));
        break;

      case V_SgPartialFunctionModifierType:
        rv.handle(static_cast<SgPartialFunctionModifierType&>(*n));
        break;

      case V_SgArrayType:
        rv.handle(static_cast<SgArrayType&>(*n));
        break;

      case V_SgTypeEllipse:
        rv.handle(static_cast<SgTypeEllipse&>(*n));
        break;

      case V_SgTypeCrayPointer:
        rv.handle(static_cast<SgTypeCrayPointer&>(*n));
        break;

      case V_SgPartialFunctionType:
        rv.handle(static_cast<SgPartialFunctionType&>(*n));
        break;

      case V_SgMemberFunctionType:
        rv.handle(static_cast<SgMemberFunctionType&>(*n));
        break;

      case V_SgFunctionType:
        rv.handle(static_cast<SgFunctionType&>(*n));
        break;

      case V_SgPointerType:
        rv.handle(static_cast<SgPointerType&>(*n));
        break;

      case V_SgNamedType:
        rv.handle(static_cast<SgNamedType&>(*n));
        break;

      case V_SgQualifiedNameType:
        rv.handle(static_cast<SgQualifiedNameType&>(*n));
        break;

      case V_SgNumVariants: /* fall-through */
      case V_SgType: /* fall-through */
      default:
        std::cerr << "unexpected type: " << typeid(*n).name() << std::endl;
        ROSE_ASSERT(false);
    }

    return rv;
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
