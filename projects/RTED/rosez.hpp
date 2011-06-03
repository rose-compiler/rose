
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

  /// \brief basic dispatcher for visitSgNode
  ///        ( invokes roseVisitor.handle(sgnode) )
  template <class T>
  struct VisitDispatcher
  {
    template <class SageNode>
    static
    void dispatch(T& rv, SageNode& node)
    {
      rv.handle(node);
    }
  };

  /// \brief dispatcher for visitors passed as pointers
  ///        ( invokes roseVisitor->handle(sgnode) )
  template <class T>
  struct VisitDispatcher<T*>
  {
    template <class SageNode>
    static
    void dispatch(T* rv, SageNode& node)
    {
      rv->handle(node);
    }
  };

  /// \brief for internal use (use visitSgNode instead)
  template <class RoseVisitor, class SageNode>
  static inline
  RoseVisitor
  _visitSgNode(RoseVisitor rv, SageNode* n)
  {
    typedef ez::VisitDispatcher<RoseVisitor> Dispatcher;

    ROSE_ASSERT( n );

    switch (n->variantT())
    {
      case V_SgName:
        Dispatcher::dispatch(rv, assume_sgtype<SgName>(*n));
        break;

      case V_SgSymbolTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgSymbolTable>(*n));
        break;

      case V_SgPragma:
        Dispatcher::dispatch(rv, assume_sgtype<SgPragma>(*n));
        break;

      case V_SgModifierNodes:
        Dispatcher::dispatch(rv, assume_sgtype<SgModifierNodes>(*n));
        break;

      case V_SgConstVolatileModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgConstVolatileModifier>(*n));
        break;

      case V_SgStorageModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgStorageModifier>(*n));
        break;

      case V_SgAccessModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgAccessModifier>(*n));
        break;

      case V_SgFunctionModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionModifier>(*n));
        break;

      case V_SgUPC_AccessModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgUPC_AccessModifier>(*n));
        break;

      case V_SgLinkageModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgLinkageModifier>(*n));
        break;

      case V_SgSpecialFunctionModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgSpecialFunctionModifier>(*n));
        break;

      case V_SgTypeModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeModifier>(*n));
        break;

      case V_SgElaboratedTypeModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgElaboratedTypeModifier>(*n));
        break;

      case V_SgBaseClassModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgBaseClassModifier>(*n));
        break;

      case V_SgDeclarationModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgDeclarationModifier>(*n));
        break;

      case V_SgOpenclAccessModeModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgOpenclAccessModeModifier>(*n));
        break;

      case V_SgModifier:
        Dispatcher::dispatch(rv, assume_sgtype<SgModifier>(*n));
        break;

      case V_Sg_File_Info:
        Dispatcher::dispatch(rv, assume_sgtype<Sg_File_Info>(*n));
        break;

      case V_SgSourceFile:
        Dispatcher::dispatch(rv, assume_sgtype<SgSourceFile>(*n));
        break;

      case V_SgBinaryComposite:
        Dispatcher::dispatch(rv, assume_sgtype<SgBinaryComposite>(*n));
        break;

      case V_SgUnknownFile:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnknownFile>(*n));
        break;

      case V_SgFile:
        Dispatcher::dispatch(rv, assume_sgtype<SgFile>(*n));
        break;

      case V_SgFileList:
        Dispatcher::dispatch(rv, assume_sgtype<SgFileList>(*n));
        break;

      case V_SgDirectory:
        Dispatcher::dispatch(rv, assume_sgtype<SgDirectory>(*n));
        break;

      case V_SgDirectoryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgDirectoryList>(*n));
        break;

      case V_SgProject:
        Dispatcher::dispatch(rv, assume_sgtype<SgProject>(*n));
        break;

      case V_SgOptions:
        Dispatcher::dispatch(rv, assume_sgtype<SgOptions>(*n));
        break;

      case V_SgUnparse_Info:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnparse_Info>(*n));
        break;

      case V_SgFuncDecl_attr:
        Dispatcher::dispatch(rv, assume_sgtype<SgFuncDecl_attr>(*n));
        break;

      case V_SgClassDecl_attr:
        Dispatcher::dispatch(rv, assume_sgtype<SgClassDecl_attr>(*n));
        break;

      case V_SgTypedefSeq:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypedefSeq>(*n));
        break;

      case V_SgFunctionParameterTypeList:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionParameterTypeList>(*n));
        break;

      case V_SgTemplateParameter:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateParameter>(*n));
        break;

      case V_SgTemplateArgument:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateArgument>(*n));
        break;

      case V_SgTemplateParameterList:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateParameterList>(*n));
        break;

      case V_SgTemplateArgumentList:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateArgumentList>(*n));
        break;

      case V_SgBitAttribute:
        Dispatcher::dispatch(rv, assume_sgtype<SgBitAttribute>(*n));
        break;

      case V_SgAttribute:
        Dispatcher::dispatch(rv, assume_sgtype<SgAttribute>(*n));
        break;

      case V_SgBaseClass:
        Dispatcher::dispatch(rv, assume_sgtype<SgBaseClass>(*n));
        break;

      case V_SgUndirectedGraphEdge:
        Dispatcher::dispatch(rv, assume_sgtype<SgUndirectedGraphEdge>(*n));
        break;

      case V_SgDirectedGraphEdge:
        Dispatcher::dispatch(rv, assume_sgtype<SgDirectedGraphEdge>(*n));
        break;

      case V_SgGraphNode:
        Dispatcher::dispatch(rv, assume_sgtype<SgGraphNode>(*n));
        break;

      case V_SgGraphEdge:
        Dispatcher::dispatch(rv, assume_sgtype<SgGraphEdge>(*n));
        break;

      case V_SgStringKeyedBidirectionalGraph:
        Dispatcher::dispatch(rv, assume_sgtype<SgStringKeyedBidirectionalGraph>(*n));
        break;

      case V_SgIntKeyedBidirectionalGraph:
        Dispatcher::dispatch(rv, assume_sgtype<SgIntKeyedBidirectionalGraph>(*n));
        break;

      case V_SgBidirectionalGraph:
        Dispatcher::dispatch(rv, assume_sgtype<SgBidirectionalGraph>(*n));
        break;

      case V_SgIncidenceDirectedGraph:
        Dispatcher::dispatch(rv, assume_sgtype<SgIncidenceDirectedGraph>(*n));
        break;

      case V_SgIncidenceUndirectedGraph:
        Dispatcher::dispatch(rv, assume_sgtype<SgIncidenceUndirectedGraph>(*n));
        break;

      case V_SgGraph:
        Dispatcher::dispatch(rv, assume_sgtype<SgGraph>(*n));
        break;

      case V_SgGraphNodeList:
        Dispatcher::dispatch(rv, assume_sgtype<SgGraphNodeList>(*n));
        break;

      case V_SgGraphEdgeList:
        Dispatcher::dispatch(rv, assume_sgtype<SgGraphEdgeList>(*n));
        break;

      case V_SgQualifiedName:
        Dispatcher::dispatch(rv, assume_sgtype<SgQualifiedName>(*n));
        break;

      case V_SgNameGroup:
        Dispatcher::dispatch(rv, assume_sgtype<SgNameGroup>(*n));
        break;

      case V_SgCommonBlockObject:
        Dispatcher::dispatch(rv, assume_sgtype<SgCommonBlockObject>(*n));
        break;

      case V_SgDimensionObject:
        Dispatcher::dispatch(rv, assume_sgtype<SgDimensionObject>(*n));
        break;

      case V_SgDataStatementGroup:
        Dispatcher::dispatch(rv, assume_sgtype<SgDataStatementGroup>(*n));
        break;

      case V_SgDataStatementObject:
        Dispatcher::dispatch(rv, assume_sgtype<SgDataStatementObject>(*n));
        break;

      case V_SgDataStatementValue:
        Dispatcher::dispatch(rv, assume_sgtype<SgDataStatementValue>(*n));
        break;

      case V_SgFormatItem:
        Dispatcher::dispatch(rv, assume_sgtype<SgFormatItem>(*n));
        break;

      case V_SgFormatItemList:
        Dispatcher::dispatch(rv, assume_sgtype<SgFormatItemList>(*n));
        break;

      case V_SgTypeTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeTable>(*n));
        break;

      case V_SgSupport:
        Dispatcher::dispatch(rv, assume_sgtype<SgSupport>(*n));
        break;

      case V_SgForStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgForStatement>(*n));
        break;

      case V_SgForInitStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgForInitStatement>(*n));
        break;

      case V_SgCatchStatementSeq:
        Dispatcher::dispatch(rv, assume_sgtype<SgCatchStatementSeq>(*n));
        break;

      case V_SgFunctionParameterList:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionParameterList>(*n));
        break;

      case V_SgCtorInitializerList:
        Dispatcher::dispatch(rv, assume_sgtype<SgCtorInitializerList>(*n));
        break;

      case V_SgBasicBlock:
        Dispatcher::dispatch(rv, assume_sgtype<SgBasicBlock>(*n));
        break;

      case V_SgGlobal:
        Dispatcher::dispatch(rv, assume_sgtype<SgGlobal>(*n));
        break;

      case V_SgIfStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgIfStmt>(*n));
        break;

      case V_SgFunctionDefinition:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionDefinition>(*n));
        break;

      case V_SgWhileStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgWhileStmt>(*n));
        break;

      case V_SgDoWhileStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgDoWhileStmt>(*n));
        break;

      case V_SgSwitchStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgSwitchStatement>(*n));
        break;

      case V_SgCatchOptionStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgCatchOptionStmt>(*n));
        break;

      case V_SgVariableDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgVariableDeclaration>(*n));
        break;

      case V_SgVariableDefinition:
        Dispatcher::dispatch(rv, assume_sgtype<SgVariableDefinition>(*n));
        break;

      case V_SgEnumDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgEnumDeclaration>(*n));
        break;

      case V_SgAsmStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmStmt>(*n));
        break;

      case V_SgTypedefDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypedefDeclaration>(*n));
        break;

      case V_SgFunctionTypeTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionTypeTable>(*n));
        break;

      case V_SgExprStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgExprStatement>(*n));
        break;

      case V_SgLabelStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgLabelStatement>(*n));
        break;

      case V_SgCaseOptionStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgCaseOptionStmt>(*n));
        break;

      case V_SgTryStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgTryStmt>(*n));
        break;

      case V_SgDefaultOptionStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgDefaultOptionStmt>(*n));
        break;

      case V_SgBreakStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgBreakStmt>(*n));
        break;

      case V_SgContinueStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgContinueStmt>(*n));
        break;

      case V_SgReturnStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgReturnStmt>(*n));
        break;

      case V_SgGotoStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgGotoStatement>(*n));
        break;

      case V_SgSpawnStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgSpawnStmt>(*n));
        break;

      case V_SgNullStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgNullStatement>(*n));
        break;

      case V_SgVariantStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgVariantStatement>(*n));
        break;

      case V_SgPragmaDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgPragmaDeclaration>(*n));
        break;

      case V_SgTemplateDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateDeclaration>(*n));
        break;

      case V_SgTemplateInstantiationDecl:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateInstantiationDecl>(*n));
        break;

      case V_SgTemplateInstantiationDefn:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateInstantiationDefn>(*n));
        break;

      case V_SgTemplateInstantiationFunctionDecl:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateInstantiationFunctionDecl>(*n));
        break;

      case V_SgTemplateInstantiationMemberFunctionDecl:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateInstantiationMemberFunctionDecl>(*n));
        break;

      case V_SgProgramHeaderStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgProgramHeaderStatement>(*n));
        break;

      case V_SgProcedureHeaderStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgProcedureHeaderStatement>(*n));
        break;

      case V_SgEntryStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgEntryStatement>(*n));
        break;

      case V_SgFortranNonblockedDo:
        Dispatcher::dispatch(rv, assume_sgtype<SgFortranNonblockedDo>(*n));
        break;

      case V_SgInterfaceStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgInterfaceStatement>(*n));
        break;

      case V_SgParameterStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgParameterStatement>(*n));
        break;

      case V_SgCommonBlock:
        Dispatcher::dispatch(rv, assume_sgtype<SgCommonBlock>(*n));
        break;

      case V_SgModuleStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgModuleStatement>(*n));
        break;

      case V_SgUseStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUseStatement>(*n));
        break;

      case V_SgStopOrPauseStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgStopOrPauseStatement>(*n));
        break;

      case V_SgPrintStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgPrintStatement>(*n));
        break;

      case V_SgReadStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgReadStatement>(*n));
        break;

      case V_SgWriteStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgWriteStatement>(*n));
        break;

      case V_SgOpenStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOpenStatement>(*n));
        break;

      case V_SgCloseStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgCloseStatement>(*n));
        break;

      case V_SgInquireStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgInquireStatement>(*n));
        break;

      case V_SgFlushStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgFlushStatement>(*n));
        break;

      case V_SgBackspaceStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgBackspaceStatement>(*n));
        break;

      case V_SgRewindStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgRewindStatement>(*n));
        break;

      case V_SgEndfileStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgEndfileStatement>(*n));
        break;

      case V_SgWaitStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgWaitStatement>(*n));
        break;

      case V_SgCAFWithTeamStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgCAFWithTeamStatement>(*n));
        break;

      case V_SgFormatStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgFormatStatement>(*n));
        break;

      case V_SgFortranDo:
        Dispatcher::dispatch(rv, assume_sgtype<SgFortranDo>(*n));
        break;

      case V_SgForAllStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgForAllStatement>(*n));
        break;

      case V_SgIOStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgIOStatement>(*n));
        break;

      case V_SgUpcNotifyStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcNotifyStatement>(*n));
        break;

      case V_SgUpcWaitStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcWaitStatement>(*n));
        break;

      case V_SgUpcBarrierStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcBarrierStatement>(*n));
        break;

      case V_SgUpcFenceStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcFenceStatement>(*n));
        break;

      case V_SgUpcForAllStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcForAllStatement>(*n));
        break;

      case V_SgOmpParallelStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpParallelStatement>(*n));
        break;

      case V_SgOmpSingleStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpSingleStatement>(*n));
        break;

      case V_SgOmpTaskStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpTaskStatement>(*n));
        break;

      case V_SgOmpForStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpForStatement>(*n));
        break;

      case V_SgOmpDoStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpDoStatement>(*n));
        break;

      case V_SgOmpSectionsStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpSectionsStatement>(*n));
        break;

      case V_SgOmpClauseBodyStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpClauseBodyStatement>(*n));
        break;

      case V_SgOmpAtomicStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpAtomicStatement>(*n));
        break;

      case V_SgOmpMasterStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpMasterStatement>(*n));
        break;

      case V_SgOmpSectionStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpSectionStatement>(*n));
        break;

      case V_SgOmpOrderedStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpOrderedStatement>(*n));
        break;

      case V_SgOmpWorkshareStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpWorkshareStatement>(*n));
        break;

      case V_SgOmpCriticalStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpCriticalStatement>(*n));
        break;

      case V_SgOmpBodyStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpBodyStatement>(*n));
        break;

      case V_SgBlockDataStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgBlockDataStatement>(*n));
        break;

      case V_SgImplicitStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgImplicitStatement>(*n));
        break;

      case V_SgStatementFunctionStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgStatementFunctionStatement>(*n));
        break;

      case V_SgWhereStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgWhereStatement>(*n));
        break;

      case V_SgNullifyStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgNullifyStatement>(*n));
        break;

      case V_SgEquivalenceStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgEquivalenceStatement>(*n));
        break;

      case V_SgDerivedTypeStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgDerivedTypeStatement>(*n));
        break;

      case V_SgAttributeSpecificationStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgAttributeSpecificationStatement>(*n));
        break;

      case V_SgAllocateStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgAllocateStatement>(*n));
        break;

      case V_SgDeallocateStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgDeallocateStatement>(*n));
        break;

      case V_SgContainsStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgContainsStatement>(*n));
        break;

      case V_SgSequenceStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgSequenceStatement>(*n));
        break;

      case V_SgElseWhereStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgElseWhereStatement>(*n));
        break;

      case V_SgArithmeticIfStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgArithmeticIfStatement>(*n));
        break;

      case V_SgAssignStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgAssignStatement>(*n));
        break;

      case V_SgComputedGotoStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgComputedGotoStatement>(*n));
        break;

      case V_SgAssignedGotoStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgAssignedGotoStatement>(*n));
        break;

      case V_SgNamelistStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgNamelistStatement>(*n));
        break;

      case V_SgImportStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgImportStatement>(*n));
        break;

      case V_SgAssociateStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgAssociateStatement>(*n));
        break;

      case V_SgFortranIncludeLine:
        Dispatcher::dispatch(rv, assume_sgtype<SgFortranIncludeLine>(*n));
        break;

      case V_SgNamespaceDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgNamespaceDeclarationStatement>(*n));
        break;

      case V_SgNamespaceAliasDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgNamespaceAliasDeclarationStatement>(*n));
        break;

      case V_SgNamespaceDefinitionStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgNamespaceDefinitionStatement>(*n));
        break;

      case V_SgUsingDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUsingDeclarationStatement>(*n));
        break;

      case V_SgUsingDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUsingDirectiveStatement>(*n));
        break;

      case V_SgTemplateInstantiationDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateInstantiationDirectiveStatement>(*n));
        break;

      case V_SgClassDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgClassDeclaration>(*n));
        break;

      case V_SgClassDefinition:
        Dispatcher::dispatch(rv, assume_sgtype<SgClassDefinition>(*n));
        break;

      case V_SgScopeStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgScopeStatement>(*n));
        break;

      case V_SgMemberFunctionDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgMemberFunctionDeclaration>(*n));
        break;

      case V_SgFunctionDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionDeclaration>(*n));
        break;

      case V_SgIncludeDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgIncludeDirectiveStatement>(*n));
        break;

      case V_SgDefineDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgDefineDirectiveStatement>(*n));
        break;

      case V_SgUndefDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgUndefDirectiveStatement>(*n));
        break;

      case V_SgIfdefDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgIfdefDirectiveStatement>(*n));
        break;

      case V_SgIfndefDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgIfndefDirectiveStatement>(*n));
        break;

      case V_SgIfDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgIfDirectiveStatement>(*n));
        break;

      case V_SgDeadIfDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgDeadIfDirectiveStatement>(*n));
        break;

      case V_SgElseDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgElseDirectiveStatement>(*n));
        break;

      case V_SgElseifDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgElseifDirectiveStatement>(*n));
        break;

      case V_SgEndifDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgEndifDirectiveStatement>(*n));
        break;

      case V_SgLineDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgLineDirectiveStatement>(*n));
        break;

      case V_SgWarningDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgWarningDirectiveStatement>(*n));
        break;

      case V_SgErrorDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgErrorDirectiveStatement>(*n));
        break;

      case V_SgEmptyDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgEmptyDirectiveStatement>(*n));
        break;

      case V_SgIncludeNextDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgIncludeNextDirectiveStatement>(*n));
        break;

      case V_SgIdentDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgIdentDirectiveStatement>(*n));
        break;

      case V_SgLinemarkerDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgLinemarkerDirectiveStatement>(*n));
        break;

      case V_SgC_PreprocessorDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgC_PreprocessorDirectiveStatement>(*n));
        break;

      case V_SgClinkageStartStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgClinkageStartStatement>(*n));
        break;

      case V_SgClinkageEndStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgClinkageEndStatement>(*n));
        break;

      case V_SgClinkageDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgClinkageDeclarationStatement>(*n));
        break;

      case V_SgOmpFlushStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpFlushStatement>(*n));
        break;

      case V_SgOmpBarrierStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpBarrierStatement>(*n));
        break;

      case V_SgOmpTaskwaitStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpTaskwaitStatement>(*n));
        break;

      case V_SgOmpThreadprivateStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpThreadprivateStatement>(*n));
        break;

      case V_SgDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgDeclarationStatement>(*n));
        break;

      case V_SgStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgStatement>(*n));
        break;

      case V_SgExprListExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgExprListExp>(*n));
        break;

      case V_SgVarRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgVarRefExp>(*n));
        break;

      case V_SgLabelRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgLabelRefExp>(*n));
        break;

      case V_SgClassNameRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgClassNameRefExp>(*n));
        break;

      case V_SgFunctionRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionRefExp>(*n));
        break;

      case V_SgMemberFunctionRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgMemberFunctionRefExp>(*n));
        break;

      case V_SgFunctionCallExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionCallExp>(*n));
        break;

      case V_SgSizeOfOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgSizeOfOp>(*n));
        break;

      case V_SgUpcLocalsizeofExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcLocalsizeofExpression>(*n));
        break;

      case V_SgUpcBlocksizeofExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcBlocksizeofExpression>(*n));
        break;

      case V_SgUpcElemsizeofExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcElemsizeofExpression>(*n));
        break;

      case V_SgVarArgStartOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgVarArgStartOp>(*n));
        break;

      case V_SgVarArgStartOneOperandOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgVarArgStartOneOperandOp>(*n));
        break;

      case V_SgVarArgOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgVarArgOp>(*n));
        break;

      case V_SgVarArgEndOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgVarArgEndOp>(*n));
        break;

      case V_SgVarArgCopyOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgVarArgCopyOp>(*n));
        break;

      case V_SgTypeIdOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeIdOp>(*n));
        break;

      case V_SgConditionalExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgConditionalExp>(*n));
        break;

      case V_SgNewExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgNewExp>(*n));
        break;

      case V_SgDeleteExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgDeleteExp>(*n));
        break;

      case V_SgThisExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgThisExp>(*n));
        break;

      case V_SgRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgRefExp>(*n));
        break;

      case V_SgAggregateInitializer:
        Dispatcher::dispatch(rv, assume_sgtype<SgAggregateInitializer>(*n));
        break;

      case V_SgConstructorInitializer:
        Dispatcher::dispatch(rv, assume_sgtype<SgConstructorInitializer>(*n));
        break;

      case V_SgAssignInitializer:
        Dispatcher::dispatch(rv, assume_sgtype<SgAssignInitializer>(*n));
        break;

      case V_SgExpressionRoot:
        Dispatcher::dispatch(rv, assume_sgtype<SgExpressionRoot>(*n));
        break;

      case V_SgMinusOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgMinusOp>(*n));
        break;

      case V_SgUnaryAddOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnaryAddOp>(*n));
        break;

      case V_SgNotOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgNotOp>(*n));
        break;

      case V_SgPointerDerefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgPointerDerefExp>(*n));
        break;

      case V_SgAddressOfOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAddressOfOp>(*n));
        break;

      case V_SgMinusMinusOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgMinusMinusOp>(*n));
        break;

      case V_SgPlusPlusOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgPlusPlusOp>(*n));
        break;

      case V_SgBitComplementOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgBitComplementOp>(*n));
        break;

      case V_SgRealPartOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgRealPartOp>(*n));
        break;

      case V_SgImagPartOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgImagPartOp>(*n));
        break;

      case V_SgConjugateOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgConjugateOp>(*n));
        break;

      case V_SgCastExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgCastExp>(*n));
        break;

      case V_SgThrowOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgThrowOp>(*n));
        break;

      case V_SgArrowExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgArrowExp>(*n));
        break;

      case V_SgDotExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgDotExp>(*n));
        break;

      case V_SgDotStarOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgDotStarOp>(*n));
        break;

      case V_SgArrowStarOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgArrowStarOp>(*n));
        break;

      case V_SgEqualityOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgEqualityOp>(*n));
        break;

      case V_SgLessThanOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgLessThanOp>(*n));
        break;

      case V_SgGreaterThanOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgGreaterThanOp>(*n));
        break;

      case V_SgNotEqualOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgNotEqualOp>(*n));
        break;

      case V_SgLessOrEqualOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgLessOrEqualOp>(*n));
        break;

      case V_SgGreaterOrEqualOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgGreaterOrEqualOp>(*n));
        break;

      case V_SgAddOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAddOp>(*n));
        break;

      case V_SgSubtractOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgSubtractOp>(*n));
        break;

      case V_SgMultiplyOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgMultiplyOp>(*n));
        break;

      case V_SgDivideOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgDivideOp>(*n));
        break;

      case V_SgIntegerDivideOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgIntegerDivideOp>(*n));
        break;

      case V_SgModOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgModOp>(*n));
        break;

      case V_SgAndOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAndOp>(*n));
        break;

      case V_SgOrOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgOrOp>(*n));
        break;

      case V_SgBitXorOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgBitXorOp>(*n));
        break;

      case V_SgBitAndOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgBitAndOp>(*n));
        break;

      case V_SgBitOrOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgBitOrOp>(*n));
        break;

      case V_SgCommaOpExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgCommaOpExp>(*n));
        break;

      case V_SgLshiftOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgLshiftOp>(*n));
        break;

      case V_SgRshiftOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgRshiftOp>(*n));
        break;

      case V_SgPntrArrRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgPntrArrRefExp>(*n));
        break;

      case V_SgScopeOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgScopeOp>(*n));
        break;

      case V_SgAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAssignOp>(*n));
        break;

      case V_SgPlusAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgPlusAssignOp>(*n));
        break;

      case V_SgMinusAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgMinusAssignOp>(*n));
        break;

      case V_SgAndAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAndAssignOp>(*n));
        break;

      case V_SgIorAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgIorAssignOp>(*n));
        break;

      case V_SgMultAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgMultAssignOp>(*n));
        break;

      case V_SgDivAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgDivAssignOp>(*n));
        break;

      case V_SgModAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgModAssignOp>(*n));
        break;

      case V_SgXorAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgXorAssignOp>(*n));
        break;

      case V_SgLshiftAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgLshiftAssignOp>(*n));
        break;

      case V_SgRshiftAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgRshiftAssignOp>(*n));
        break;

      case V_SgConcatenationOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgConcatenationOp>(*n));
        break;

      case V_SgBoolValExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgBoolValExp>(*n));
        break;

      case V_SgStringVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgStringVal>(*n));
        break;

      case V_SgShortVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgShortVal>(*n));
        break;

      case V_SgCharVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgCharVal>(*n));
        break;

      case V_SgUnsignedCharVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnsignedCharVal>(*n));
        break;

      case V_SgWcharVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgWcharVal>(*n));
        break;

      case V_SgUnsignedShortVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnsignedShortVal>(*n));
        break;

      case V_SgIntVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgIntVal>(*n));
        break;

      case V_SgEnumVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgEnumVal>(*n));
        break;

      case V_SgUnsignedIntVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnsignedIntVal>(*n));
        break;

      case V_SgLongIntVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgLongIntVal>(*n));
        break;

      case V_SgLongLongIntVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgLongLongIntVal>(*n));
        break;

      case V_SgUnsignedLongLongIntVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnsignedLongLongIntVal>(*n));
        break;

      case V_SgUnsignedLongVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnsignedLongVal>(*n));
        break;

      case V_SgFloatVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgFloatVal>(*n));
        break;

      case V_SgDoubleVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgDoubleVal>(*n));
        break;

      case V_SgLongDoubleVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgLongDoubleVal>(*n));
        break;

      case V_SgUpcThreads:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcThreads>(*n));
        break;

      case V_SgUpcMythread:
        Dispatcher::dispatch(rv, assume_sgtype<SgUpcMythread>(*n));
        break;

      case V_SgComplexVal:
        Dispatcher::dispatch(rv, assume_sgtype<SgComplexVal>(*n));
        break;

      case V_SgNullExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgNullExpression>(*n));
        break;

      case V_SgVariantExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgVariantExpression>(*n));
        break;

      case V_SgStatementExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgStatementExpression>(*n));
        break;

      case V_SgAsmOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmOp>(*n));
        break;

      case V_SgCudaKernelExecConfig:
        Dispatcher::dispatch(rv, assume_sgtype<SgCudaKernelExecConfig>(*n));
        break;

      case V_SgCudaKernelCallExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgCudaKernelCallExp>(*n));
        break;

      case V_SgSubscriptExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgSubscriptExpression>(*n));
        break;

      case V_SgColonShapeExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgColonShapeExp>(*n));
        break;

      case V_SgAsteriskShapeExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsteriskShapeExp>(*n));
        break;

      case V_SgIOItemExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgIOItemExpression>(*n));
        break;

      case V_SgImpliedDo:
        Dispatcher::dispatch(rv, assume_sgtype<SgImpliedDo>(*n));
        break;

      case V_SgExponentiationOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgExponentiationOp>(*n));
        break;

      case V_SgUnknownArrayOrFunctionReference:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnknownArrayOrFunctionReference>(*n));
        break;

      case V_SgActualArgumentExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgActualArgumentExpression>(*n));
        break;

      case V_SgUserDefinedBinaryOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgUserDefinedBinaryOp>(*n));
        break;

      case V_SgPointerAssignOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgPointerAssignOp>(*n));
        break;

      case V_SgCAFCoExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgCAFCoExpression>(*n));
        break;

      case V_SgDesignatedInitializer:
        Dispatcher::dispatch(rv, assume_sgtype<SgDesignatedInitializer>(*n));
        break;

      case V_SgInitializer:
        Dispatcher::dispatch(rv, assume_sgtype<SgInitializer>(*n));
        break;

      case V_SgUserDefinedUnaryOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgUserDefinedUnaryOp>(*n));
        break;

      case V_SgPseudoDestructorRefExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgPseudoDestructorRefExp>(*n));
        break;

      case V_SgUnaryOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgUnaryOp>(*n));
        break;

      case V_SgBinaryOp:
        Dispatcher::dispatch(rv, assume_sgtype<SgBinaryOp>(*n));
        break;

      case V_SgValueExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgValueExp>(*n));
        break;

      case V_SgExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgExpression>(*n));
        break;

      case V_SgVariableSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgVariableSymbol>(*n));
        break;

      case V_SgFunctionTypeSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionTypeSymbol>(*n));
        break;

      case V_SgClassSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgClassSymbol>(*n));
        break;

      case V_SgTemplateSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateSymbol>(*n));
        break;

      case V_SgEnumSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgEnumSymbol>(*n));
        break;

      case V_SgEnumFieldSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgEnumFieldSymbol>(*n));
        break;

      case V_SgTypedefSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypedefSymbol>(*n));
        break;

      case V_SgMemberFunctionSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgMemberFunctionSymbol>(*n));
        break;

      case V_SgLabelSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgLabelSymbol>(*n));
        break;

      case V_SgDefaultSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgDefaultSymbol>(*n));
        break;

      case V_SgNamespaceSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgNamespaceSymbol>(*n));
        break;

      case V_SgIntrinsicSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgIntrinsicSymbol>(*n));
        break;

      case V_SgModuleSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgModuleSymbol>(*n));
        break;

      case V_SgInterfaceSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgInterfaceSymbol>(*n));
        break;

      case V_SgCommonSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgCommonSymbol>(*n));
        break;

      case V_SgRenameSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgRenameSymbol>(*n));
        break;

      case V_SgFunctionSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionSymbol>(*n));
        break;

      case V_SgAsmBinaryAddressSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryAddressSymbol>(*n));
        break;

      case V_SgAsmBinaryDataSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryDataSymbol>(*n));
        break;

      case V_SgAliasSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgAliasSymbol>(*n));
        break;

      case V_SgSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgSymbol>(*n));
        break;

      case V_SgAsmBlock:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBlock>(*n));
        break;

      case V_SgAsmOperandList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmOperandList>(*n));
        break;

      case V_SgAsmArmInstruction:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmArmInstruction>(*n));
        break;

      case V_SgAsmx86Instruction:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmx86Instruction>(*n));
        break;

      case V_SgAsmPowerpcInstruction:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPowerpcInstruction>(*n));
        break;

      case V_SgAsmInstruction:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmInstruction>(*n));
        break;

      case V_SgAsmDataStructureDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDataStructureDeclaration>(*n));
        break;

      case V_SgAsmFunctionDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmFunctionDeclaration>(*n));
        break;

      case V_SgAsmFieldDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmFieldDeclaration>(*n));
        break;

      case V_SgAsmDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDeclaration>(*n));
        break;

      case V_SgAsmStatement:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmStatement>(*n));
        break;

      case V_SgAsmBinaryAdd:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryAdd>(*n));
        break;

      case V_SgAsmBinarySubtract:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinarySubtract>(*n));
        break;

      case V_SgAsmBinaryMultiply:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryMultiply>(*n));
        break;

      case V_SgAsmBinaryDivide:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryDivide>(*n));
        break;

      case V_SgAsmBinaryMod:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryMod>(*n));
        break;

      case V_SgAsmBinaryAddPreupdate:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryAddPreupdate>(*n));
        break;

      case V_SgAsmBinarySubtractPreupdate:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinarySubtractPreupdate>(*n));
        break;

      case V_SgAsmBinaryAddPostupdate:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryAddPostupdate>(*n));
        break;

      case V_SgAsmBinarySubtractPostupdate:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinarySubtractPostupdate>(*n));
        break;

      case V_SgAsmBinaryLsl:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryLsl>(*n));
        break;

      case V_SgAsmBinaryLsr:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryLsr>(*n));
        break;

      case V_SgAsmBinaryAsr:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryAsr>(*n));
        break;

      case V_SgAsmBinaryRor:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryRor>(*n));
        break;

      case V_SgAsmBinaryExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBinaryExpression>(*n));
        break;

      case V_SgAsmUnaryPlus:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmUnaryPlus>(*n));
        break;

      case V_SgAsmUnaryMinus:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmUnaryMinus>(*n));
        break;

      case V_SgAsmUnaryRrx:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmUnaryRrx>(*n));
        break;

      case V_SgAsmUnaryArmSpecialRegisterList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmUnaryArmSpecialRegisterList>(*n));
        break;

      case V_SgAsmUnaryExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmUnaryExpression>(*n));
        break;

      case V_SgAsmMemoryReferenceExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmMemoryReferenceExpression>(*n));
        break;

      case V_SgAsmControlFlagsExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmControlFlagsExpression>(*n));
        break;

      case V_SgAsmCommonSubExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmCommonSubExpression>(*n));
        break;

      case V_SgAsmx86RegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmx86RegisterReferenceExpression>(*n));
        break;

      case V_SgAsmArmRegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmArmRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmPowerpcRegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPowerpcRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmRegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmByteValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmByteValueExpression>(*n));
        break;

      case V_SgAsmWordValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmWordValueExpression>(*n));
        break;

      case V_SgAsmDoubleWordValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDoubleWordValueExpression>(*n));
        break;

      case V_SgAsmQuadWordValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmQuadWordValueExpression>(*n));
        break;

      case V_SgAsmSingleFloatValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmSingleFloatValueExpression>(*n));
        break;

      case V_SgAsmDoubleFloatValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDoubleFloatValueExpression>(*n));
        break;

      case V_SgAsmVectorValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmVectorValueExpression>(*n));
        break;

      case V_SgAsmValueExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmValueExpression>(*n));
        break;

      case V_SgAsmExprListExp:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmExprListExp>(*n));
        break;

      case V_SgAsmExpression:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmExpression>(*n));
        break;

      case V_SgAsmTypeByte:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeByte>(*n));
        break;

      case V_SgAsmTypeWord:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeWord>(*n));
        break;

      case V_SgAsmTypeDoubleWord:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeDoubleWord>(*n));
        break;

      case V_SgAsmTypeQuadWord:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeQuadWord>(*n));
        break;

      case V_SgAsmTypeDoubleQuadWord:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeDoubleQuadWord>(*n));
        break;

      case V_SgAsmTypeSingleFloat:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeSingleFloat>(*n));
        break;

      case V_SgAsmTypeDoubleFloat:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeDoubleFloat>(*n));
        break;

      case V_SgAsmType80bitFloat:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmType80bitFloat>(*n));
        break;

      case V_SgAsmType128bitFloat:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmType128bitFloat>(*n));
        break;

      case V_SgAsmTypeVector:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmTypeVector>(*n));
        break;

      case V_SgAsmType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmType>(*n));
        break;

      case V_SgAsmGenericDLL:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericDLL>(*n));
        break;

      case V_SgAsmPEImportHNTEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportHNTEntryList>(*n));
        break;

      case V_SgAsmPEImportILTEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportILTEntryList>(*n));
        break;

      case V_SgAsmPEImportDirectoryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportDirectoryList>(*n));
        break;

      case V_SgAsmGenericFormat:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericFormat>(*n));
        break;

      case V_SgAsmGenericFile:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericFile>(*n));
        break;

      case V_SgAsmElfFileHeader:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfFileHeader>(*n));
        break;

      case V_SgAsmPEFileHeader:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEFileHeader>(*n));
        break;

      case V_SgAsmNEFileHeader:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNEFileHeader>(*n));
        break;

      case V_SgAsmLEFileHeader:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLEFileHeader>(*n));
        break;

      case V_SgAsmDOSFileHeader:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDOSFileHeader>(*n));
        break;

      case V_SgAsmGenericHeader:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericHeader>(*n));
        break;

      case V_SgAsmElfRelocSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfRelocSection>(*n));
        break;

      case V_SgAsmElfDynamicSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfDynamicSection>(*n));
        break;

      case V_SgAsmElfSymbolSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymbolSection>(*n));
        break;

      case V_SgAsmElfStringSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfStringSection>(*n));
        break;

      case V_SgAsmElfEHFrameSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfEHFrameSection>(*n));
        break;

      case V_SgAsmElfNoteSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfNoteSection>(*n));
        break;

      case V_SgAsmElfSymverSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverSection>(*n));
        break;

      case V_SgAsmElfSymverDefinedSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverDefinedSection>(*n));
        break;

      case V_SgAsmElfSymverNeededSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverNeededSection>(*n));
        break;

      case V_SgAsmElfStrtab:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfStrtab>(*n));
        break;

      case V_SgAsmCoffStrtab:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmCoffStrtab>(*n));
        break;

      case V_SgAsmGenericStrtab:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericStrtab>(*n));
        break;

      case V_SgAsmElfSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSection>(*n));
        break;

      case V_SgAsmElfSectionTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSectionTable>(*n));
        break;

      case V_SgAsmElfSegmentTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSegmentTable>(*n));
        break;

      case V_SgAsmPEImportSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportSection>(*n));
        break;

      case V_SgAsmPEExportSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEExportSection>(*n));
        break;

      case V_SgAsmPEStringSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEStringSection>(*n));
        break;

      case V_SgAsmPESection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPESection>(*n));
        break;

      case V_SgAsmPESectionTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPESectionTable>(*n));
        break;

      case V_SgAsmCoffSymbolTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmCoffSymbolTable>(*n));
        break;

      case V_SgAsmDOSExtendedHeader:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDOSExtendedHeader>(*n));
        break;

      case V_SgAsmNESection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNESection>(*n));
        break;

      case V_SgAsmNESectionTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNESectionTable>(*n));
        break;

      case V_SgAsmNENameTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNENameTable>(*n));
        break;

      case V_SgAsmNEModuleTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNEModuleTable>(*n));
        break;

      case V_SgAsmNEStringTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNEStringTable>(*n));
        break;

      case V_SgAsmNEEntryTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNEEntryTable>(*n));
        break;

      case V_SgAsmNERelocTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNERelocTable>(*n));
        break;

      case V_SgAsmLESection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLESection>(*n));
        break;

      case V_SgAsmLESectionTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLESectionTable>(*n));
        break;

      case V_SgAsmLENameTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLENameTable>(*n));
        break;

      case V_SgAsmLEPageTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLEPageTable>(*n));
        break;

      case V_SgAsmLEEntryTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLEEntryTable>(*n));
        break;

      case V_SgAsmLERelocTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLERelocTable>(*n));
        break;

      case V_SgAsmGenericSection:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericSection>(*n));
        break;

      case V_SgAsmCoffSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmCoffSymbol>(*n));
        break;

      case V_SgAsmElfSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymbol>(*n));
        break;

      case V_SgAsmGenericSymbol:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericSymbol>(*n));
        break;

      case V_SgAsmElfSectionTableEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSectionTableEntry>(*n));
        break;

      case V_SgAsmElfSegmentTableEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSegmentTableEntry>(*n));
        break;

      case V_SgAsmElfSegmentTableEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSegmentTableEntryList>(*n));
        break;

      case V_SgAsmElfRelocEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfRelocEntry>(*n));
        break;

      case V_SgAsmElfRelocEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfRelocEntryList>(*n));
        break;

      case V_SgAsmElfDynamicEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfDynamicEntry>(*n));
        break;

      case V_SgAsmElfDynamicEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfDynamicEntryList>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCI:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfEHFrameEntryCI>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCIList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfEHFrameEntryCIList>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFD:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfEHFrameEntryFD>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFDList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfEHFrameEntryFDList>(*n));
        break;

      case V_SgAsmElfNoteEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfNoteEntry>(*n));
        break;

      case V_SgAsmElfNoteEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfNoteEntryList>(*n));
        break;

      case V_SgAsmElfSymverEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverEntry>(*n));
        break;

      case V_SgAsmElfSymverEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverEntryList>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverDefinedEntry>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverDefinedEntryList>(*n));
        break;

      case V_SgAsmElfSymverDefinedAux:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverDefinedAux>(*n));
        break;

      case V_SgAsmElfSymverDefinedAuxList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverDefinedAuxList>(*n));
        break;

      case V_SgAsmElfSymverNeededEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverNeededEntry>(*n));
        break;

      case V_SgAsmElfSymverNeededEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverNeededEntryList>(*n));
        break;

      case V_SgAsmElfSymverNeededAux:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverNeededAux>(*n));
        break;

      case V_SgAsmElfSymverNeededAuxList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymverNeededAuxList>(*n));
        break;

      case V_SgAsmPERVASizePair:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPERVASizePair>(*n));
        break;

      case V_SgAsmPEExportDirectory:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEExportDirectory>(*n));
        break;

      case V_SgAsmPEExportEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEExportEntry>(*n));
        break;

      case V_SgAsmPEImportDirectory:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportDirectory>(*n));
        break;

      case V_SgAsmPEImportILTEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportILTEntry>(*n));
        break;

      case V_SgAsmPEImportHNTEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportHNTEntry>(*n));
        break;

      case V_SgAsmPEImportLookupTable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEImportLookupTable>(*n));
        break;

      case V_SgAsmPESectionTableEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPESectionTableEntry>(*n));
        break;

      case V_SgAsmNEEntryPoint:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNEEntryPoint>(*n));
        break;

      case V_SgAsmNERelocEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNERelocEntry>(*n));
        break;

      case V_SgAsmNESectionTableEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNESectionTableEntry>(*n));
        break;

      case V_SgAsmLEPageTableEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLEPageTableEntry>(*n));
        break;

      case V_SgAsmLEEntryPoint:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLEEntryPoint>(*n));
        break;

      case V_SgAsmLESectionTableEntry:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmLESectionTableEntry>(*n));
        break;

      case V_SgAsmGenericSectionList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericSectionList>(*n));
        break;

      case V_SgAsmGenericHeaderList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericHeaderList>(*n));
        break;

      case V_SgAsmGenericSymbolList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericSymbolList>(*n));
        break;

      case V_SgAsmElfSymbolList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmElfSymbolList>(*n));
        break;

      case V_SgAsmCoffSymbolList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmCoffSymbolList>(*n));
        break;

      case V_SgAsmGenericDLLList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericDLLList>(*n));
        break;

      case V_SgAsmPERVASizePairList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPERVASizePairList>(*n));
        break;

      case V_SgAsmPEExportEntryList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmPEExportEntryList>(*n));
        break;

      case V_SgAsmBasicString:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmBasicString>(*n));
        break;

      case V_SgAsmStoredString:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmStoredString>(*n));
        break;

      case V_SgAsmGenericString:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericString>(*n));
        break;

      case V_SgAsmStringStorage:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmStringStorage>(*n));
        break;

      case V_SgAsmDwarfMacro:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfMacro>(*n));
        break;

      case V_SgAsmDwarfLine:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfLine>(*n));
        break;

      case V_SgAsmDwarfMacroList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfMacroList>(*n));
        break;

      case V_SgAsmDwarfLineList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfLineList>(*n));
        break;

      case V_SgAsmDwarfArrayType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfArrayType>(*n));
        break;

      case V_SgAsmDwarfClassType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfClassType>(*n));
        break;

      case V_SgAsmDwarfEntryPoint:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfEntryPoint>(*n));
        break;

      case V_SgAsmDwarfEnumerationType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfEnumerationType>(*n));
        break;

      case V_SgAsmDwarfFormalParameter:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfFormalParameter>(*n));
        break;

      case V_SgAsmDwarfImportedDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfImportedDeclaration>(*n));
        break;

      case V_SgAsmDwarfLabel:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfLabel>(*n));
        break;

      case V_SgAsmDwarfLexicalBlock:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfLexicalBlock>(*n));
        break;

      case V_SgAsmDwarfMember:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfMember>(*n));
        break;

      case V_SgAsmDwarfPointerType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfPointerType>(*n));
        break;

      case V_SgAsmDwarfReferenceType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfReferenceType>(*n));
        break;

      case V_SgAsmDwarfCompilationUnit:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfCompilationUnit>(*n));
        break;

      case V_SgAsmDwarfStringType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfStringType>(*n));
        break;

      case V_SgAsmDwarfStructureType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfStructureType>(*n));
        break;

      case V_SgAsmDwarfSubroutineType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfSubroutineType>(*n));
        break;

      case V_SgAsmDwarfTypedef:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfTypedef>(*n));
        break;

      case V_SgAsmDwarfUnionType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfUnionType>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedParameters:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfUnspecifiedParameters>(*n));
        break;

      case V_SgAsmDwarfVariant:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfVariant>(*n));
        break;

      case V_SgAsmDwarfCommonBlock:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfCommonBlock>(*n));
        break;

      case V_SgAsmDwarfCommonInclusion:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfCommonInclusion>(*n));
        break;

      case V_SgAsmDwarfInheritance:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfInheritance>(*n));
        break;

      case V_SgAsmDwarfInlinedSubroutine:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfInlinedSubroutine>(*n));
        break;

      case V_SgAsmDwarfModule:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfModule>(*n));
        break;

      case V_SgAsmDwarfPtrToMemberType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfPtrToMemberType>(*n));
        break;

      case V_SgAsmDwarfSetType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfSetType>(*n));
        break;

      case V_SgAsmDwarfSubrangeType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfSubrangeType>(*n));
        break;

      case V_SgAsmDwarfWithStmt:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfWithStmt>(*n));
        break;

      case V_SgAsmDwarfAccessDeclaration:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfAccessDeclaration>(*n));
        break;

      case V_SgAsmDwarfBaseType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfBaseType>(*n));
        break;

      case V_SgAsmDwarfCatchBlock:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfCatchBlock>(*n));
        break;

      case V_SgAsmDwarfConstType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfConstType>(*n));
        break;

      case V_SgAsmDwarfConstant:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfConstant>(*n));
        break;

      case V_SgAsmDwarfEnumerator:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfEnumerator>(*n));
        break;

      case V_SgAsmDwarfFileType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfFileType>(*n));
        break;

      case V_SgAsmDwarfFriend:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfFriend>(*n));
        break;

      case V_SgAsmDwarfNamelist:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfNamelist>(*n));
        break;

      case V_SgAsmDwarfNamelistItem:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfNamelistItem>(*n));
        break;

      case V_SgAsmDwarfPackedType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfPackedType>(*n));
        break;

      case V_SgAsmDwarfSubprogram:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfSubprogram>(*n));
        break;

      case V_SgAsmDwarfTemplateTypeParameter:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfTemplateTypeParameter>(*n));
        break;

      case V_SgAsmDwarfTemplateValueParameter:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfTemplateValueParameter>(*n));
        break;

      case V_SgAsmDwarfThrownType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfThrownType>(*n));
        break;

      case V_SgAsmDwarfTryBlock:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfTryBlock>(*n));
        break;

      case V_SgAsmDwarfVariantPart:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfVariantPart>(*n));
        break;

      case V_SgAsmDwarfVariable:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfVariable>(*n));
        break;

      case V_SgAsmDwarfVolatileType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfVolatileType>(*n));
        break;

      case V_SgAsmDwarfDwarfProcedure:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfDwarfProcedure>(*n));
        break;

      case V_SgAsmDwarfRestrictType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfRestrictType>(*n));
        break;

      case V_SgAsmDwarfInterfaceType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfInterfaceType>(*n));
        break;

      case V_SgAsmDwarfNamespace:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfNamespace>(*n));
        break;

      case V_SgAsmDwarfImportedModule:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfImportedModule>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfUnspecifiedType>(*n));
        break;

      case V_SgAsmDwarfPartialUnit:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfPartialUnit>(*n));
        break;

      case V_SgAsmDwarfImportedUnit:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfImportedUnit>(*n));
        break;

      case V_SgAsmDwarfMutableType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfMutableType>(*n));
        break;

      case V_SgAsmDwarfCondition:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfCondition>(*n));
        break;

      case V_SgAsmDwarfSharedType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfSharedType>(*n));
        break;

      case V_SgAsmDwarfFormatLabel:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfFormatLabel>(*n));
        break;

      case V_SgAsmDwarfFunctionTemplate:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfFunctionTemplate>(*n));
        break;

      case V_SgAsmDwarfClassTemplate:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfClassTemplate>(*n));
        break;

      case V_SgAsmDwarfUpcSharedType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfUpcSharedType>(*n));
        break;

      case V_SgAsmDwarfUpcStrictType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfUpcStrictType>(*n));
        break;

      case V_SgAsmDwarfUpcRelaxedType:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfUpcRelaxedType>(*n));
        break;

      case V_SgAsmDwarfUnknownConstruct:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfUnknownConstruct>(*n));
        break;

      case V_SgAsmDwarfConstruct:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfConstruct>(*n));
        break;

      case V_SgAsmDwarfConstructList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfConstructList>(*n));
        break;

      case V_SgAsmDwarfCompilationUnitList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfCompilationUnitList>(*n));
        break;

      case V_SgAsmDwarfInformation:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmDwarfInformation>(*n));
        break;

      case V_SgAsmExecutableFileFormat:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmExecutableFileFormat>(*n));
        break;

      case V_SgAsmInterpretation:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmInterpretation>(*n));
        break;

      case V_SgAsmInterpretationList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmInterpretationList>(*n));
        break;

      case V_SgAsmGenericFileList:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmGenericFileList>(*n));
        break;

      case V_SgAsmNode:
        Dispatcher::dispatch(rv, assume_sgtype<SgAsmNode>(*n));
        break;

      case V_SgInitializedName:
        Dispatcher::dispatch(rv, assume_sgtype<SgInitializedName>(*n));
        break;

      case V_SgOmpOrderedClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpOrderedClause>(*n));
        break;

      case V_SgOmpNowaitClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpNowaitClause>(*n));
        break;

      case V_SgOmpUntiedClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpUntiedClause>(*n));
        break;

      case V_SgOmpDefaultClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpDefaultClause>(*n));
        break;

      case V_SgOmpCollapseClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpCollapseClause>(*n));
        break;

      case V_SgOmpIfClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpIfClause>(*n));
        break;

      case V_SgOmpNumThreadsClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpNumThreadsClause>(*n));
        break;

      case V_SgOmpExpressionClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpExpressionClause>(*n));
        break;

      case V_SgOmpCopyprivateClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpCopyprivateClause>(*n));
        break;

      case V_SgOmpPrivateClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpPrivateClause>(*n));
        break;

      case V_SgOmpFirstprivateClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpFirstprivateClause>(*n));
        break;

      case V_SgOmpSharedClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpSharedClause>(*n));
        break;

      case V_SgOmpCopyinClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpCopyinClause>(*n));
        break;

      case V_SgOmpLastprivateClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpLastprivateClause>(*n));
        break;

      case V_SgOmpReductionClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpReductionClause>(*n));
        break;

      case V_SgOmpVariablesClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpVariablesClause>(*n));
        break;

      case V_SgOmpScheduleClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpScheduleClause>(*n));
        break;

      case V_SgOmpClause:
        Dispatcher::dispatch(rv, assume_sgtype<SgOmpClause>(*n));
        break;

      case V_SgRenamePair:
        Dispatcher::dispatch(rv, assume_sgtype<SgRenamePair>(*n));
        break;

      case V_SgInterfaceBody:
        Dispatcher::dispatch(rv, assume_sgtype<SgInterfaceBody>(*n));
        break;

      case V_SgLocatedNodeSupport:
        Dispatcher::dispatch(rv, assume_sgtype<SgLocatedNodeSupport>(*n));
        break;

      case V_SgToken:
        Dispatcher::dispatch(rv, assume_sgtype<SgToken>(*n));
        break;

      case V_SgLocatedNode:
        Dispatcher::dispatch(rv, assume_sgtype<SgLocatedNode>(*n));
        break;

      case V_SgNode:
        Dispatcher::dispatch(rv, assume_sgtype<SgNode>(*n));
        break;

      //
      // Types

      case V_SgTypeUnknown:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeUnknown>(*n));
        break;

      case V_SgTypeChar:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeChar>(*n));
        break;

      case V_SgTypeSignedChar:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeSignedChar>(*n));
        break;

      case V_SgTypeUnsignedChar:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeUnsignedChar>(*n));
        break;

      case V_SgTypeShort:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeShort>(*n));
        break;

      case V_SgTypeSignedShort:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeSignedShort>(*n));
        break;

      case V_SgTypeUnsignedShort:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeUnsignedShort>(*n));
        break;

      case V_SgTypeInt:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeInt>(*n));
        break;

      case V_SgTypeSignedInt:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeSignedInt>(*n));
        break;

      case V_SgTypeUnsignedInt:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeUnsignedInt>(*n));
        break;

      case V_SgTypeLong:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeLong>(*n));
        break;

      case V_SgTypeSignedLong:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeSignedLong>(*n));
        break;

      case V_SgTypeUnsignedLong:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeUnsignedLong>(*n));
        break;

      case V_SgTypeVoid:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeVoid>(*n));
        break;

      case V_SgTypeGlobalVoid:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeGlobalVoid>(*n));
        break;

      case V_SgTypeWchar:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeWchar>(*n));
        break;

      case V_SgTypeFloat:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeFloat>(*n));
        break;

      case V_SgTypeDouble:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeDouble>(*n));
        break;

      case V_SgTypeLongLong:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeLongLong>(*n));
        break;

      case V_SgTypeSignedLongLong:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeSignedLongLong>(*n));
        break;

      case V_SgTypeUnsignedLongLong:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeUnsignedLongLong>(*n));
        break;

      case V_SgTypeLongDouble:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeLongDouble>(*n));
        break;

      case V_SgTypeString:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeString>(*n));
        break;

      case V_SgTypeBool:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeBool>(*n));
        break;

      case V_SgTypeComplex:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeComplex>(*n));
        break;

      case V_SgTypeImaginary:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeImaginary>(*n));
        break;

      case V_SgTypeDefault:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeDefault>(*n));
        break;

      case V_SgPointerMemberType:
        Dispatcher::dispatch(rv, assume_sgtype<SgPointerMemberType>(*n));
        break;

      case V_SgReferenceType:
        Dispatcher::dispatch(rv, assume_sgtype<SgReferenceType>(*n));
        break;

      case V_SgTypeCAFTeam:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeCAFTeam>(*n));
        break;

      case V_SgClassType:
        Dispatcher::dispatch(rv, assume_sgtype<SgClassType>(*n));
        break;

      case V_SgTemplateType:
        Dispatcher::dispatch(rv, assume_sgtype<SgTemplateType>(*n));
        break;

      case V_SgEnumType:
        Dispatcher::dispatch(rv, assume_sgtype<SgEnumType>(*n));
        break;

      case V_SgTypedefType:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypedefType>(*n));
        break;

      case V_SgModifierType:
        Dispatcher::dispatch(rv, assume_sgtype<SgModifierType>(*n));
        break;

      case V_SgPartialFunctionModifierType:
        Dispatcher::dispatch(rv, assume_sgtype<SgPartialFunctionModifierType>(*n));
        break;

      case V_SgArrayType:
        Dispatcher::dispatch(rv, assume_sgtype<SgArrayType>(*n));
        break;

      case V_SgTypeEllipse:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeEllipse>(*n));
        break;

      case V_SgTypeCrayPointer:
        Dispatcher::dispatch(rv, assume_sgtype<SgTypeCrayPointer>(*n));
        break;

      case V_SgPartialFunctionType:
        Dispatcher::dispatch(rv, assume_sgtype<SgPartialFunctionType>(*n));
        break;

      case V_SgMemberFunctionType:
        Dispatcher::dispatch(rv, assume_sgtype<SgMemberFunctionType>(*n));
        break;

      case V_SgFunctionType:
        Dispatcher::dispatch(rv, assume_sgtype<SgFunctionType>(*n));
        break;

      case V_SgPointerType:
        Dispatcher::dispatch(rv, assume_sgtype<SgPointerType>(*n));
        break;

      case V_SgNamedType:
        Dispatcher::dispatch(rv, assume_sgtype<SgNamedType>(*n));
        break;

      case V_SgQualifiedNameType:
        Dispatcher::dispatch(rv, assume_sgtype<SgQualifiedNameType>(*n));
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
  /// \tparam   RoseVisitor, the visitor that will be called back with
  ///           the recovered type information. The handle function with
  ///           the most suitable SgNode type will get invoked.
  /// \param rv an instance of a rose visitor; note that the argument is
  ///           passed by value (similar to STL's for_each).
  /// \param n  a Sage node
  /// \return   a copy of the RoseVisitor object
  /// \example
  /// \code
  ///   struct Handler
  ///   {
  ///      // base case (all uninteresting nodes fall into this function)
  ///      void handle(const SgNode&) {}
  ///
  ///      // expression specific code
  ///      void handle(const SgExpression& n) { ++expr; }
  ///
  ///      // declaration specific code
  ///      void handle(const SgDeclarationStatement& n) { ++decl; }
  ///   };
  ///
  ///
  /// \endcode
  /// \details
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

  template <class SageNode>
  struct DefaultHandler
  {
    void handle(SageNode&) {}
  };

  /// \brief   helper class for _ancestor
  /// \details implements a type switch over the Rose AST to find an
  ///          ancestor node with a given type. The search fails
  ///          at SgProject nodes.
  /// \tparam  AncestorNode the type of ancestors to look for
  /// \tparam  QualSgNode either 'const SgNode*' or 'SgNode*' depending
  ///          if the search is over constant nodes or not.
  template <class AncestorNode, class QualSgNode>
  struct AncestorTypeFinder : DefaultHandler<const SgProject>
  {
    typedef DefaultHandler<const SgProject>       Base;
    typedef std::pair<AncestorNode*, QualSgNode*> Pair;

    Pair res;

    AncestorTypeFinder()
    : Base(), res(NULL, NULL)
    {}

    // handling of const SgProject is outsourced to DefaultHandler
    // thus, AncestorNode = const SgProject does not cause conflicts
    using Base::handle;

    void handle(QualSgNode& n) { res.second = n.get_parent(); }
    void handle(AncestorNode& n) { res.first = &n; }

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

  template <class SageNode1, class SageNode2>
  struct ezConstRecovery
  {
    typedef SageNode2 type;
  };

  template <class SageNode1, class SageNode2>
  struct ezConstRecovery<const SageNode1, SageNode2>
  {
    typedef const SageNode2 type;
  };

  template <class T>
  static
  void log_type(const T& n)
  {
    std::cerr << typeid(n).name() << std::endl;
  }

  template <class SageNode>
  struct TypeRecoveryHandler
  {
    typedef typename ezConstRecovery<SageNode, SgNode>::type SgBaseNode;

    SageNode* res;

    TypeRecoveryHandler()
    : res(NULL)
    {}

    void handle(SgBaseNode& n) { log_type(n); assert(false); }
    void handle(SageNode& n) { res = &n; }

    operator SageNode* () { return res; }
  };

  template <class SageNode>
  SageNode* assert_sage_type(SgNode* n)
  {
    return ez::visitSgNode(TypeRecoveryHandler<SageNode>(), n);
  }

  template <class SageNode>
  const SageNode* assert_sage_type(const SgNode* n)
  {
    return ez::visitSgNode(TypeRecoveryHandler<const SageNode>(), n);
  }
}

#endif /* _ROSEZ_HPP */
