#ifndef _SAGEGENERIC_HPP

/// \file sageGeneric.hpp
///       This file implements generic (template) sage query functions
///       Currently this includes functions for:
///       - dispatching according to the type of a sage node (dispatch)
///       - finding the ancestor with a specific node type (ancestor)
///       - recovering the type of a sage node assertively (assert_node_type)
/// \email peter.pirkelbauer@llnl.gov

#define _SAGEGENERIC_HPP

#include <stdexcept>

#if !defined(NDEBUG)
#include <typeinfo>
#include <iostream>
#endif /* NDEBUG */

#include "rose.h"

namespace sg
{
  //
  // non sage specific utilities

  /// \brief portable implementation to avoid compiler warnings for
  ///        unused parameter.
  template <class T>
  static inline
  void unused(const T&) {}

  /// \brief projects the constness of T1 on T2
  template <class T1, class T2>
  struct ConstLike
  {
    typedef T2 type;
  };

  template <class T1, class T2>
  struct ConstLike<const T1, T2>
  {
    typedef const T2 type;
  };

  static inline
  void unexpected_node(const SgNode& n)
  {
    sg::unused(n);

#if !defined(NDEBUG)
    std::cerr << typeid(n).name() << std::endl;
#endif

    ROSE_ASSERT(false);
    throw std::logic_error("Encountered unexpected sage node. Please send a bug report to the maintainer.");
  }

  //
  // Convenience class

  template <class _ReturnType>
  struct DispatchHandler
  {
      typedef _ReturnType                 ReturnType;
      typedef DispatchHandler<ReturnType> Base;

      DispatchHandler()
      : res()
      {}

      explicit
      DispatchHandler(const ReturnType& defaultval)
      : res(defaultval)
      {}

      operator ReturnType() const { return res; }

    protected:
      ReturnType res;
  };


  //
  // Sage query functions

  /// \brief *unchecked* down cast from SgNode to SageNode
  template <class SageNode>
  static inline
  SageNode& assume_sage_type(SgNode& n)
  {
    return static_cast<SageNode&>(n);
  }

  /// \overload
  /// \note     overloaded to preserve constness
  template <class SageNode>
  static inline
  const SageNode& assume_sage_type(const SgNode& n)
  {
    return static_cast<const SageNode&>(n);
  }

  /// \brief implementation for dispatcher objects
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

  /// \brief implementation for dispatcher pointers
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

  /// \brief for internal use (use dispatch instead)
  template <class RoseVisitor, class SageNode>
  static inline
  RoseVisitor
  _dispatch(RoseVisitor rv, SageNode* n)
  {
    typedef sg::VisitDispatcher<RoseVisitor> Dispatcher;

    ROSE_ASSERT( n );

    switch (n->variantT())
    {
      case V_SgName:
        Dispatcher::dispatch(rv, assume_sage_type<SgName>(*n));
        break;

      case V_SgSymbolTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgSymbolTable>(*n));
        break;

      case V_SgPragma:
        Dispatcher::dispatch(rv, assume_sage_type<SgPragma>(*n));
        break;

      case V_SgModifierNodes:
        Dispatcher::dispatch(rv, assume_sage_type<SgModifierNodes>(*n));
        break;

      case V_SgConstVolatileModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgConstVolatileModifier>(*n));
        break;

      case V_SgStorageModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgStorageModifier>(*n));
        break;

      case V_SgAccessModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgAccessModifier>(*n));
        break;

      case V_SgFunctionModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionModifier>(*n));
        break;

      case V_SgUPC_AccessModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgUPC_AccessModifier>(*n));
        break;

      case V_SgLinkageModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgLinkageModifier>(*n));
        break;

      case V_SgSpecialFunctionModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgSpecialFunctionModifier>(*n));
        break;

      case V_SgTypeModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeModifier>(*n));
        break;

      case V_SgElaboratedTypeModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgElaboratedTypeModifier>(*n));
        break;

      case V_SgBaseClassModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgBaseClassModifier>(*n));
        break;

      case V_SgDeclarationModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgDeclarationModifier>(*n));
        break;

      case V_SgOpenclAccessModeModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgOpenclAccessModeModifier>(*n));
        break;

      case V_SgModifier:
        Dispatcher::dispatch(rv, assume_sage_type<SgModifier>(*n));
        break;

      case V_Sg_File_Info:
        Dispatcher::dispatch(rv, assume_sage_type<Sg_File_Info>(*n));
        break;

      case V_SgSourceFile:
        Dispatcher::dispatch(rv, assume_sage_type<SgSourceFile>(*n));
        break;

      case V_SgBinaryComposite:
        Dispatcher::dispatch(rv, assume_sage_type<SgBinaryComposite>(*n));
        break;

      case V_SgUnknownFile:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnknownFile>(*n));
        break;

      case V_SgFile:
        Dispatcher::dispatch(rv, assume_sage_type<SgFile>(*n));
        break;

      case V_SgFileList:
        Dispatcher::dispatch(rv, assume_sage_type<SgFileList>(*n));
        break;

      case V_SgDirectory:
        Dispatcher::dispatch(rv, assume_sage_type<SgDirectory>(*n));
        break;

      case V_SgDirectoryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgDirectoryList>(*n));
        break;

      case V_SgProject:
        Dispatcher::dispatch(rv, assume_sage_type<SgProject>(*n));
        break;

      case V_SgOptions:
        Dispatcher::dispatch(rv, assume_sage_type<SgOptions>(*n));
        break;

      case V_SgUnparse_Info:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnparse_Info>(*n));
        break;

      case V_SgFuncDecl_attr:
        Dispatcher::dispatch(rv, assume_sage_type<SgFuncDecl_attr>(*n));
        break;

      case V_SgClassDecl_attr:
        Dispatcher::dispatch(rv, assume_sage_type<SgClassDecl_attr>(*n));
        break;

      case V_SgTypedefSeq:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypedefSeq>(*n));
        break;

      case V_SgFunctionParameterTypeList:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionParameterTypeList>(*n));
        break;

      case V_SgTemplateParameter:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateParameter>(*n));
        break;

      case V_SgTemplateArgument:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateArgument>(*n));
        break;

      case V_SgTemplateParameterList:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateParameterList>(*n));
        break;

      case V_SgTemplateArgumentList:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateArgumentList>(*n));
        break;

      case V_SgBitAttribute:
        Dispatcher::dispatch(rv, assume_sage_type<SgBitAttribute>(*n));
        break;

      case V_SgAttribute:
        Dispatcher::dispatch(rv, assume_sage_type<SgAttribute>(*n));
        break;

      case V_SgBaseClass:
        Dispatcher::dispatch(rv, assume_sage_type<SgBaseClass>(*n));
        break;

      case V_SgUndirectedGraphEdge:
        Dispatcher::dispatch(rv, assume_sage_type<SgUndirectedGraphEdge>(*n));
        break;

      case V_SgDirectedGraphEdge:
        Dispatcher::dispatch(rv, assume_sage_type<SgDirectedGraphEdge>(*n));
        break;

      case V_SgGraphNode:
        Dispatcher::dispatch(rv, assume_sage_type<SgGraphNode>(*n));
        break;

      case V_SgGraphEdge:
        Dispatcher::dispatch(rv, assume_sage_type<SgGraphEdge>(*n));
        break;

      case V_SgStringKeyedBidirectionalGraph:
        Dispatcher::dispatch(rv, assume_sage_type<SgStringKeyedBidirectionalGraph>(*n));
        break;

      case V_SgIntKeyedBidirectionalGraph:
        Dispatcher::dispatch(rv, assume_sage_type<SgIntKeyedBidirectionalGraph>(*n));
        break;

      case V_SgBidirectionalGraph:
        Dispatcher::dispatch(rv, assume_sage_type<SgBidirectionalGraph>(*n));
        break;

      case V_SgIncidenceDirectedGraph:
        Dispatcher::dispatch(rv, assume_sage_type<SgIncidenceDirectedGraph>(*n));
        break;

      case V_SgIncidenceUndirectedGraph:
        Dispatcher::dispatch(rv, assume_sage_type<SgIncidenceUndirectedGraph>(*n));
        break;

      case V_SgGraph:
        Dispatcher::dispatch(rv, assume_sage_type<SgGraph>(*n));
        break;

      case V_SgGraphNodeList:
        Dispatcher::dispatch(rv, assume_sage_type<SgGraphNodeList>(*n));
        break;

      case V_SgGraphEdgeList:
        Dispatcher::dispatch(rv, assume_sage_type<SgGraphEdgeList>(*n));
        break;

      case V_SgQualifiedName:
        Dispatcher::dispatch(rv, assume_sage_type<SgQualifiedName>(*n));
        break;

      case V_SgNameGroup:
        Dispatcher::dispatch(rv, assume_sage_type<SgNameGroup>(*n));
        break;

      case V_SgCommonBlockObject:
        Dispatcher::dispatch(rv, assume_sage_type<SgCommonBlockObject>(*n));
        break;

      case V_SgDimensionObject:
        Dispatcher::dispatch(rv, assume_sage_type<SgDimensionObject>(*n));
        break;

      case V_SgDataStatementGroup:
        Dispatcher::dispatch(rv, assume_sage_type<SgDataStatementGroup>(*n));
        break;

      case V_SgDataStatementObject:
        Dispatcher::dispatch(rv, assume_sage_type<SgDataStatementObject>(*n));
        break;

      case V_SgDataStatementValue:
        Dispatcher::dispatch(rv, assume_sage_type<SgDataStatementValue>(*n));
        break;

      case V_SgFormatItem:
        Dispatcher::dispatch(rv, assume_sage_type<SgFormatItem>(*n));
        break;

      case V_SgFormatItemList:
        Dispatcher::dispatch(rv, assume_sage_type<SgFormatItemList>(*n));
        break;

      case V_SgTypeTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeTable>(*n));
        break;

      case V_SgSupport:
        Dispatcher::dispatch(rv, assume_sage_type<SgSupport>(*n));
        break;

      case V_SgForStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgForStatement>(*n));
        break;

      case V_SgForInitStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgForInitStatement>(*n));
        break;

      case V_SgCatchStatementSeq:
        Dispatcher::dispatch(rv, assume_sage_type<SgCatchStatementSeq>(*n));
        break;

      case V_SgFunctionParameterList:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionParameterList>(*n));
        break;

      case V_SgCtorInitializerList:
        Dispatcher::dispatch(rv, assume_sage_type<SgCtorInitializerList>(*n));
        break;

      case V_SgBasicBlock:
        Dispatcher::dispatch(rv, assume_sage_type<SgBasicBlock>(*n));
        break;

      case V_SgGlobal:
        Dispatcher::dispatch(rv, assume_sage_type<SgGlobal>(*n));
        break;

      case V_SgIfStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgIfStmt>(*n));
        break;

      case V_SgFunctionDefinition:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionDefinition>(*n));
        break;

      case V_SgWhileStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgWhileStmt>(*n));
        break;

      case V_SgDoWhileStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgDoWhileStmt>(*n));
        break;

      case V_SgSwitchStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgSwitchStatement>(*n));
        break;

      case V_SgCatchOptionStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgCatchOptionStmt>(*n));
        break;

      case V_SgVariableDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgVariableDeclaration>(*n));
        break;

      case V_SgVariableDefinition:
        Dispatcher::dispatch(rv, assume_sage_type<SgVariableDefinition>(*n));
        break;

      case V_SgEnumDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgEnumDeclaration>(*n));
        break;

      case V_SgAsmStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmStmt>(*n));
        break;

      case V_SgTypedefDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypedefDeclaration>(*n));
        break;

      case V_SgFunctionTypeTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionTypeTable>(*n));
        break;

      case V_SgExprStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgExprStatement>(*n));
        break;

      case V_SgLabelStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgLabelStatement>(*n));
        break;

      case V_SgCaseOptionStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgCaseOptionStmt>(*n));
        break;

      case V_SgTryStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgTryStmt>(*n));
        break;

      case V_SgDefaultOptionStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgDefaultOptionStmt>(*n));
        break;

      case V_SgBreakStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgBreakStmt>(*n));
        break;

      case V_SgContinueStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgContinueStmt>(*n));
        break;

      case V_SgReturnStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgReturnStmt>(*n));
        break;

      case V_SgGotoStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgGotoStatement>(*n));
        break;

      case V_SgSpawnStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgSpawnStmt>(*n));
        break;

      case V_SgNullStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgNullStatement>(*n));
        break;

      case V_SgVariantStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgVariantStatement>(*n));
        break;

      case V_SgPragmaDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgPragmaDeclaration>(*n));
        break;

      case V_SgTemplateDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateDeclaration>(*n));
        break;

      case V_SgTemplateInstantiationDecl:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateInstantiationDecl>(*n));
        break;

      case V_SgTemplateInstantiationDefn:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateInstantiationDefn>(*n));
        break;

      case V_SgTemplateInstantiationFunctionDecl:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateInstantiationFunctionDecl>(*n));
        break;

      case V_SgTemplateInstantiationMemberFunctionDecl:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateInstantiationMemberFunctionDecl>(*n));
        break;

      case V_SgProgramHeaderStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgProgramHeaderStatement>(*n));
        break;

      case V_SgProcedureHeaderStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgProcedureHeaderStatement>(*n));
        break;

      case V_SgEntryStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgEntryStatement>(*n));
        break;

      case V_SgFortranNonblockedDo:
        Dispatcher::dispatch(rv, assume_sage_type<SgFortranNonblockedDo>(*n));
        break;

      case V_SgInterfaceStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgInterfaceStatement>(*n));
        break;

      case V_SgParameterStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgParameterStatement>(*n));
        break;

      case V_SgCommonBlock:
        Dispatcher::dispatch(rv, assume_sage_type<SgCommonBlock>(*n));
        break;

      case V_SgModuleStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgModuleStatement>(*n));
        break;

      case V_SgUseStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUseStatement>(*n));
        break;

      case V_SgStopOrPauseStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgStopOrPauseStatement>(*n));
        break;

      case V_SgPrintStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgPrintStatement>(*n));
        break;

      case V_SgReadStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgReadStatement>(*n));
        break;

      case V_SgWriteStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgWriteStatement>(*n));
        break;

      case V_SgOpenStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOpenStatement>(*n));
        break;

      case V_SgCloseStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgCloseStatement>(*n));
        break;

      case V_SgInquireStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgInquireStatement>(*n));
        break;

      case V_SgFlushStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgFlushStatement>(*n));
        break;

      case V_SgBackspaceStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgBackspaceStatement>(*n));
        break;

      case V_SgRewindStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgRewindStatement>(*n));
        break;

      case V_SgEndfileStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgEndfileStatement>(*n));
        break;

      case V_SgWaitStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgWaitStatement>(*n));
        break;

      case V_SgCAFWithTeamStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgCAFWithTeamStatement>(*n));
        break;

      case V_SgFormatStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgFormatStatement>(*n));
        break;

      case V_SgFortranDo:
        Dispatcher::dispatch(rv, assume_sage_type<SgFortranDo>(*n));
        break;

      case V_SgForAllStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgForAllStatement>(*n));
        break;

      case V_SgIOStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgIOStatement>(*n));
        break;

      case V_SgUpcNotifyStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcNotifyStatement>(*n));
        break;

      case V_SgUpcWaitStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcWaitStatement>(*n));
        break;

      case V_SgUpcBarrierStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcBarrierStatement>(*n));
        break;

      case V_SgUpcFenceStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcFenceStatement>(*n));
        break;

      case V_SgUpcForAllStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcForAllStatement>(*n));
        break;

      case V_SgOmpParallelStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpParallelStatement>(*n));
        break;

      case V_SgOmpSingleStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpSingleStatement>(*n));
        break;

      case V_SgOmpTaskStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpTaskStatement>(*n));
        break;

      case V_SgOmpForStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpForStatement>(*n));
        break;

      case V_SgOmpDoStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpDoStatement>(*n));
        break;

      case V_SgOmpSectionsStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpSectionsStatement>(*n));
        break;

      case V_SgOmpClauseBodyStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpClauseBodyStatement>(*n));
        break;

      case V_SgOmpAtomicStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpAtomicStatement>(*n));
        break;

      case V_SgOmpMasterStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpMasterStatement>(*n));
        break;

      case V_SgOmpSectionStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpSectionStatement>(*n));
        break;

      case V_SgOmpOrderedStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpOrderedStatement>(*n));
        break;

      case V_SgOmpWorkshareStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpWorkshareStatement>(*n));
        break;

      case V_SgOmpCriticalStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpCriticalStatement>(*n));
        break;

      case V_SgOmpBodyStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpBodyStatement>(*n));
        break;

      case V_SgBlockDataStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgBlockDataStatement>(*n));
        break;

      case V_SgImplicitStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgImplicitStatement>(*n));
        break;

      case V_SgStatementFunctionStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgStatementFunctionStatement>(*n));
        break;

      case V_SgWhereStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgWhereStatement>(*n));
        break;

      case V_SgNullifyStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgNullifyStatement>(*n));
        break;

      case V_SgEquivalenceStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgEquivalenceStatement>(*n));
        break;

      case V_SgDerivedTypeStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgDerivedTypeStatement>(*n));
        break;

      case V_SgAttributeSpecificationStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgAttributeSpecificationStatement>(*n));
        break;

      case V_SgAllocateStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgAllocateStatement>(*n));
        break;

      case V_SgDeallocateStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgDeallocateStatement>(*n));
        break;

      case V_SgContainsStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgContainsStatement>(*n));
        break;

      case V_SgSequenceStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgSequenceStatement>(*n));
        break;

      case V_SgElseWhereStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgElseWhereStatement>(*n));
        break;

      case V_SgArithmeticIfStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgArithmeticIfStatement>(*n));
        break;

      case V_SgAssignStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgAssignStatement>(*n));
        break;

      case V_SgComputedGotoStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgComputedGotoStatement>(*n));
        break;

      case V_SgAssignedGotoStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgAssignedGotoStatement>(*n));
        break;

      case V_SgNamelistStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgNamelistStatement>(*n));
        break;

      case V_SgImportStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgImportStatement>(*n));
        break;

      case V_SgAssociateStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgAssociateStatement>(*n));
        break;

      case V_SgFortranIncludeLine:
        Dispatcher::dispatch(rv, assume_sage_type<SgFortranIncludeLine>(*n));
        break;

      case V_SgNamespaceDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgNamespaceDeclarationStatement>(*n));
        break;

      case V_SgNamespaceAliasDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgNamespaceAliasDeclarationStatement>(*n));
        break;

      case V_SgNamespaceDefinitionStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgNamespaceDefinitionStatement>(*n));
        break;

      case V_SgUsingDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUsingDeclarationStatement>(*n));
        break;

      case V_SgUsingDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUsingDirectiveStatement>(*n));
        break;

      case V_SgTemplateInstantiationDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateInstantiationDirectiveStatement>(*n));
        break;

      case V_SgClassDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgClassDeclaration>(*n));
        break;

      case V_SgClassDefinition:
        Dispatcher::dispatch(rv, assume_sage_type<SgClassDefinition>(*n));
        break;

      case V_SgScopeStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgScopeStatement>(*n));
        break;

      case V_SgMemberFunctionDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgMemberFunctionDeclaration>(*n));
        break;

      case V_SgFunctionDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionDeclaration>(*n));
        break;

      case V_SgIncludeDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgIncludeDirectiveStatement>(*n));
        break;

      case V_SgDefineDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgDefineDirectiveStatement>(*n));
        break;

      case V_SgUndefDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgUndefDirectiveStatement>(*n));
        break;

      case V_SgIfdefDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgIfdefDirectiveStatement>(*n));
        break;

      case V_SgIfndefDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgIfndefDirectiveStatement>(*n));
        break;

      case V_SgIfDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgIfDirectiveStatement>(*n));
        break;

      case V_SgDeadIfDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgDeadIfDirectiveStatement>(*n));
        break;

      case V_SgElseDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgElseDirectiveStatement>(*n));
        break;

      case V_SgElseifDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgElseifDirectiveStatement>(*n));
        break;

      case V_SgEndifDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgEndifDirectiveStatement>(*n));
        break;

      case V_SgLineDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgLineDirectiveStatement>(*n));
        break;

      case V_SgWarningDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgWarningDirectiveStatement>(*n));
        break;

      case V_SgErrorDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgErrorDirectiveStatement>(*n));
        break;

      case V_SgEmptyDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgEmptyDirectiveStatement>(*n));
        break;

      case V_SgIncludeNextDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgIncludeNextDirectiveStatement>(*n));
        break;

      case V_SgIdentDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgIdentDirectiveStatement>(*n));
        break;

      case V_SgLinemarkerDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgLinemarkerDirectiveStatement>(*n));
        break;

      case V_SgC_PreprocessorDirectiveStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgC_PreprocessorDirectiveStatement>(*n));
        break;

      case V_SgClinkageStartStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgClinkageStartStatement>(*n));
        break;

      case V_SgClinkageEndStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgClinkageEndStatement>(*n));
        break;

      case V_SgClinkageDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgClinkageDeclarationStatement>(*n));
        break;

      case V_SgOmpFlushStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpFlushStatement>(*n));
        break;

      case V_SgOmpBarrierStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpBarrierStatement>(*n));
        break;

      case V_SgOmpTaskwaitStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpTaskwaitStatement>(*n));
        break;

      case V_SgOmpThreadprivateStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpThreadprivateStatement>(*n));
        break;

      case V_SgDeclarationStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgDeclarationStatement>(*n));
        break;

      case V_SgExprListExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgExprListExp>(*n));
        break;

      case V_SgVarRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgVarRefExp>(*n));
        break;

      case V_SgLabelRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgLabelRefExp>(*n));
        break;

      case V_SgClassNameRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgClassNameRefExp>(*n));
        break;

      case V_SgFunctionRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionRefExp>(*n));
        break;

      case V_SgMemberFunctionRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgMemberFunctionRefExp>(*n));
        break;

      case V_SgFunctionCallExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionCallExp>(*n));
        break;

      case V_SgSizeOfOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgSizeOfOp>(*n));
        break;

      case V_SgUpcLocalsizeofExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcLocalsizeofExpression>(*n));
        break;

      case V_SgUpcBlocksizeofExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcBlocksizeofExpression>(*n));
        break;

      case V_SgUpcElemsizeofExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcElemsizeofExpression>(*n));
        break;

      case V_SgVarArgStartOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgVarArgStartOp>(*n));
        break;

      case V_SgVarArgStartOneOperandOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgVarArgStartOneOperandOp>(*n));
        break;

      case V_SgVarArgOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgVarArgOp>(*n));
        break;

      case V_SgVarArgEndOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgVarArgEndOp>(*n));
        break;

      case V_SgVarArgCopyOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgVarArgCopyOp>(*n));
        break;

      case V_SgTypeIdOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeIdOp>(*n));
        break;

      case V_SgConditionalExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgConditionalExp>(*n));
        break;

      case V_SgNewExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgNewExp>(*n));
        break;

      case V_SgDeleteExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgDeleteExp>(*n));
        break;

      case V_SgThisExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgThisExp>(*n));
        break;

      case V_SgRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgRefExp>(*n));
        break;

      case V_SgAggregateInitializer:
        Dispatcher::dispatch(rv, assume_sage_type<SgAggregateInitializer>(*n));
        break;

      case V_SgConstructorInitializer:
        Dispatcher::dispatch(rv, assume_sage_type<SgConstructorInitializer>(*n));
        break;

      case V_SgAssignInitializer:
        Dispatcher::dispatch(rv, assume_sage_type<SgAssignInitializer>(*n));
        break;

      case V_SgExpressionRoot:
        Dispatcher::dispatch(rv, assume_sage_type<SgExpressionRoot>(*n));
        break;

      case V_SgMinusOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgMinusOp>(*n));
        break;

      case V_SgUnaryAddOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnaryAddOp>(*n));
        break;

      case V_SgNotOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgNotOp>(*n));
        break;

      case V_SgPointerDerefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgPointerDerefExp>(*n));
        break;

      case V_SgAddressOfOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAddressOfOp>(*n));
        break;

      case V_SgMinusMinusOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgMinusMinusOp>(*n));
        break;

      case V_SgPlusPlusOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgPlusPlusOp>(*n));
        break;

      case V_SgBitComplementOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgBitComplementOp>(*n));
        break;

      case V_SgRealPartOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgRealPartOp>(*n));
        break;

      case V_SgImagPartOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgImagPartOp>(*n));
        break;

      case V_SgConjugateOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgConjugateOp>(*n));
        break;

      case V_SgCastExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgCastExp>(*n));
        break;

      case V_SgThrowOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgThrowOp>(*n));
        break;

      case V_SgArrowExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgArrowExp>(*n));
        break;

      case V_SgDotExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgDotExp>(*n));
        break;

      case V_SgDotStarOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgDotStarOp>(*n));
        break;

      case V_SgArrowStarOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgArrowStarOp>(*n));
        break;

      case V_SgEqualityOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgEqualityOp>(*n));
        break;

      case V_SgLessThanOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgLessThanOp>(*n));
        break;

      case V_SgGreaterThanOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgGreaterThanOp>(*n));
        break;

      case V_SgNotEqualOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgNotEqualOp>(*n));
        break;

      case V_SgLessOrEqualOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgLessOrEqualOp>(*n));
        break;

      case V_SgGreaterOrEqualOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgGreaterOrEqualOp>(*n));
        break;

      case V_SgAddOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAddOp>(*n));
        break;

      case V_SgSubtractOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgSubtractOp>(*n));
        break;

      case V_SgMultiplyOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgMultiplyOp>(*n));
        break;

      case V_SgDivideOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgDivideOp>(*n));
        break;

      case V_SgIntegerDivideOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgIntegerDivideOp>(*n));
        break;

      case V_SgModOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgModOp>(*n));
        break;

      case V_SgAndOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAndOp>(*n));
        break;

      case V_SgOrOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgOrOp>(*n));
        break;

      case V_SgBitXorOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgBitXorOp>(*n));
        break;

      case V_SgBitAndOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgBitAndOp>(*n));
        break;

      case V_SgBitOrOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgBitOrOp>(*n));
        break;

      case V_SgCommaOpExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgCommaOpExp>(*n));
        break;

      case V_SgLshiftOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgLshiftOp>(*n));
        break;

      case V_SgRshiftOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgRshiftOp>(*n));
        break;

      case V_SgPntrArrRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgPntrArrRefExp>(*n));
        break;

      case V_SgScopeOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgScopeOp>(*n));
        break;

      case V_SgAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAssignOp>(*n));
        break;

      case V_SgPlusAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgPlusAssignOp>(*n));
        break;

      case V_SgMinusAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgMinusAssignOp>(*n));
        break;

      case V_SgAndAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAndAssignOp>(*n));
        break;

      case V_SgIorAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgIorAssignOp>(*n));
        break;

      case V_SgMultAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgMultAssignOp>(*n));
        break;

      case V_SgDivAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgDivAssignOp>(*n));
        break;

      case V_SgModAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgModAssignOp>(*n));
        break;

      case V_SgXorAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgXorAssignOp>(*n));
        break;

      case V_SgLshiftAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgLshiftAssignOp>(*n));
        break;

      case V_SgRshiftAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgRshiftAssignOp>(*n));
        break;

      case V_SgConcatenationOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgConcatenationOp>(*n));
        break;

      case V_SgBoolValExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgBoolValExp>(*n));
        break;

      case V_SgStringVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgStringVal>(*n));
        break;

      case V_SgShortVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgShortVal>(*n));
        break;

      case V_SgCharVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgCharVal>(*n));
        break;

      case V_SgUnsignedCharVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnsignedCharVal>(*n));
        break;

      case V_SgWcharVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgWcharVal>(*n));
        break;

      case V_SgUnsignedShortVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnsignedShortVal>(*n));
        break;

      case V_SgIntVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgIntVal>(*n));
        break;

      case V_SgEnumVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgEnumVal>(*n));
        break;

      case V_SgUnsignedIntVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnsignedIntVal>(*n));
        break;

      case V_SgLongIntVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgLongIntVal>(*n));
        break;

      case V_SgLongLongIntVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgLongLongIntVal>(*n));
        break;

      case V_SgUnsignedLongLongIntVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnsignedLongLongIntVal>(*n));
        break;

      case V_SgUnsignedLongVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnsignedLongVal>(*n));
        break;

      case V_SgFloatVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgFloatVal>(*n));
        break;

      case V_SgDoubleVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgDoubleVal>(*n));
        break;

      case V_SgLongDoubleVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgLongDoubleVal>(*n));
        break;

      case V_SgUpcThreads:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcThreads>(*n));
        break;

      case V_SgUpcMythread:
        Dispatcher::dispatch(rv, assume_sage_type<SgUpcMythread>(*n));
        break;

      case V_SgComplexVal:
        Dispatcher::dispatch(rv, assume_sage_type<SgComplexVal>(*n));
        break;

      case V_SgNullExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgNullExpression>(*n));
        break;

      case V_SgVariantExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgVariantExpression>(*n));
        break;

      case V_SgStatementExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgStatementExpression>(*n));
        break;

      case V_SgAsmOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmOp>(*n));
        break;

      case V_SgCudaKernelExecConfig:
        Dispatcher::dispatch(rv, assume_sage_type<SgCudaKernelExecConfig>(*n));
        break;

      case V_SgCudaKernelCallExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgCudaKernelCallExp>(*n));
        break;

      case V_SgSubscriptExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgSubscriptExpression>(*n));
        break;

      case V_SgColonShapeExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgColonShapeExp>(*n));
        break;

      case V_SgAsteriskShapeExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsteriskShapeExp>(*n));
        break;

      case V_SgIOItemExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgIOItemExpression>(*n));
        break;

      case V_SgImpliedDo:
        Dispatcher::dispatch(rv, assume_sage_type<SgImpliedDo>(*n));
        break;

      case V_SgExponentiationOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgExponentiationOp>(*n));
        break;

      case V_SgUnknownArrayOrFunctionReference:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnknownArrayOrFunctionReference>(*n));
        break;

      case V_SgActualArgumentExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgActualArgumentExpression>(*n));
        break;

      case V_SgUserDefinedBinaryOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgUserDefinedBinaryOp>(*n));
        break;

      case V_SgPointerAssignOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgPointerAssignOp>(*n));
        break;

      case V_SgCAFCoExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgCAFCoExpression>(*n));
        break;

      case V_SgDesignatedInitializer:
        Dispatcher::dispatch(rv, assume_sage_type<SgDesignatedInitializer>(*n));
        break;

      case V_SgInitializer:
        Dispatcher::dispatch(rv, assume_sage_type<SgInitializer>(*n));
        break;

      case V_SgUserDefinedUnaryOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgUserDefinedUnaryOp>(*n));
        break;

      case V_SgPseudoDestructorRefExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgPseudoDestructorRefExp>(*n));
        break;

      case V_SgUnaryOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgUnaryOp>(*n));
        break;

      case V_SgBinaryOp:
        Dispatcher::dispatch(rv, assume_sage_type<SgBinaryOp>(*n));
        break;

      case V_SgValueExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgValueExp>(*n));
        break;

      case V_SgVariableSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgVariableSymbol>(*n));
        break;

      case V_SgFunctionTypeSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionTypeSymbol>(*n));
        break;

      case V_SgClassSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgClassSymbol>(*n));
        break;

      case V_SgTemplateSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateSymbol>(*n));
        break;

      case V_SgEnumSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgEnumSymbol>(*n));
        break;

      case V_SgEnumFieldSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgEnumFieldSymbol>(*n));
        break;

      case V_SgTypedefSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypedefSymbol>(*n));
        break;

      case V_SgMemberFunctionSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgMemberFunctionSymbol>(*n));
        break;

      case V_SgLabelSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgLabelSymbol>(*n));
        break;

      case V_SgDefaultSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgDefaultSymbol>(*n));
        break;

      case V_SgNamespaceSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgNamespaceSymbol>(*n));
        break;

      case V_SgIntrinsicSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgIntrinsicSymbol>(*n));
        break;

      case V_SgModuleSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgModuleSymbol>(*n));
        break;

      case V_SgInterfaceSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgInterfaceSymbol>(*n));
        break;

      case V_SgCommonSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgCommonSymbol>(*n));
        break;

      case V_SgRenameSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgRenameSymbol>(*n));
        break;

      case V_SgFunctionSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionSymbol>(*n));
        break;

      case V_SgAsmBinaryAddressSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryAddressSymbol>(*n));
        break;

      case V_SgAsmBinaryDataSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryDataSymbol>(*n));
        break;

      case V_SgAliasSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgAliasSymbol>(*n));
        break;

      case V_SgSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgSymbol>(*n));
        break;

      case V_SgAsmBlock:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBlock>(*n));
        break;

      case V_SgAsmOperandList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmOperandList>(*n));
        break;

      case V_SgAsmArmInstruction:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmArmInstruction>(*n));
        break;

      case V_SgAsmx86Instruction:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmx86Instruction>(*n));
        break;

      case V_SgAsmPowerpcInstruction:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPowerpcInstruction>(*n));
        break;

      case V_SgAsmInstruction:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmInstruction>(*n));
        break;

      case V_SgAsmDataStructureDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDataStructureDeclaration>(*n));
        break;

      case V_SgAsmFunctionDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmFunctionDeclaration>(*n));
        break;

      case V_SgAsmFieldDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmFieldDeclaration>(*n));
        break;

      case V_SgAsmDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDeclaration>(*n));
        break;

      case V_SgAsmStatement:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmStatement>(*n));
        break;

      case V_SgAsmBinaryAdd:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryAdd>(*n));
        break;

      case V_SgAsmBinarySubtract:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinarySubtract>(*n));
        break;

      case V_SgAsmBinaryMultiply:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryMultiply>(*n));
        break;

      case V_SgAsmBinaryDivide:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryDivide>(*n));
        break;

      case V_SgAsmBinaryMod:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryMod>(*n));
        break;

      case V_SgAsmBinaryAddPreupdate:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryAddPreupdate>(*n));
        break;

      case V_SgAsmBinarySubtractPreupdate:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinarySubtractPreupdate>(*n));
        break;

      case V_SgAsmBinaryAddPostupdate:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryAddPostupdate>(*n));
        break;

      case V_SgAsmBinarySubtractPostupdate:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinarySubtractPostupdate>(*n));
        break;

      case V_SgAsmBinaryLsl:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryLsl>(*n));
        break;

      case V_SgAsmBinaryLsr:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryLsr>(*n));
        break;

      case V_SgAsmBinaryAsr:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryAsr>(*n));
        break;

      case V_SgAsmBinaryRor:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryRor>(*n));
        break;

      case V_SgAsmBinaryExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBinaryExpression>(*n));
        break;

      case V_SgAsmUnaryPlus:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmUnaryPlus>(*n));
        break;

      case V_SgAsmUnaryMinus:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmUnaryMinus>(*n));
        break;

      case V_SgAsmUnaryRrx:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmUnaryRrx>(*n));
        break;

      case V_SgAsmUnaryArmSpecialRegisterList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmUnaryArmSpecialRegisterList>(*n));
        break;

      case V_SgAsmUnaryExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmUnaryExpression>(*n));
        break;

      case V_SgAsmMemoryReferenceExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmMemoryReferenceExpression>(*n));
        break;

      case V_SgAsmControlFlagsExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmControlFlagsExpression>(*n));
        break;

      case V_SgAsmCommonSubExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmCommonSubExpression>(*n));
        break;

      case V_SgAsmx86RegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmx86RegisterReferenceExpression>(*n));
        break;

      case V_SgAsmArmRegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmArmRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmPowerpcRegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPowerpcRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmRegisterReferenceExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmRegisterReferenceExpression>(*n));
        break;

      case V_SgAsmByteValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmByteValueExpression>(*n));
        break;

      case V_SgAsmWordValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmWordValueExpression>(*n));
        break;

      case V_SgAsmDoubleWordValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDoubleWordValueExpression>(*n));
        break;

      case V_SgAsmQuadWordValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmQuadWordValueExpression>(*n));
        break;

      case V_SgAsmSingleFloatValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmSingleFloatValueExpression>(*n));
        break;

      case V_SgAsmDoubleFloatValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDoubleFloatValueExpression>(*n));
        break;

      case V_SgAsmVectorValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmVectorValueExpression>(*n));
        break;

      case V_SgAsmValueExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmValueExpression>(*n));
        break;

      case V_SgAsmExprListExp:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmExprListExp>(*n));
        break;

      case V_SgAsmExpression:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmExpression>(*n));
        break;

      case V_SgAsmTypeByte:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeByte>(*n));
        break;

      case V_SgAsmTypeWord:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeWord>(*n));
        break;

      case V_SgAsmTypeDoubleWord:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeDoubleWord>(*n));
        break;

      case V_SgAsmTypeQuadWord:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeQuadWord>(*n));
        break;

      case V_SgAsmTypeDoubleQuadWord:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeDoubleQuadWord>(*n));
        break;

      case V_SgAsmTypeSingleFloat:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeSingleFloat>(*n));
        break;

      case V_SgAsmTypeDoubleFloat:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeDoubleFloat>(*n));
        break;

      case V_SgAsmType80bitFloat:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmType80bitFloat>(*n));
        break;

      case V_SgAsmType128bitFloat:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmType128bitFloat>(*n));
        break;

      case V_SgAsmTypeVector:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmTypeVector>(*n));
        break;

      case V_SgAsmType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmType>(*n));
        break;

      case V_SgAsmGenericDLL:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericDLL>(*n));
        break;

      case V_SgAsmPEImportHNTEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportHNTEntryList>(*n));
        break;

      case V_SgAsmPEImportILTEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportILTEntryList>(*n));
        break;

      case V_SgAsmPEImportDirectoryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportDirectoryList>(*n));
        break;

      case V_SgAsmGenericFormat:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericFormat>(*n));
        break;

      case V_SgAsmGenericFile:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericFile>(*n));
        break;

      case V_SgAsmElfFileHeader:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfFileHeader>(*n));
        break;

      case V_SgAsmPEFileHeader:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEFileHeader>(*n));
        break;

      case V_SgAsmNEFileHeader:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNEFileHeader>(*n));
        break;

      case V_SgAsmLEFileHeader:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLEFileHeader>(*n));
        break;

      case V_SgAsmDOSFileHeader:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDOSFileHeader>(*n));
        break;

      case V_SgAsmGenericHeader:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericHeader>(*n));
        break;

      case V_SgAsmElfRelocSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfRelocSection>(*n));
        break;

      case V_SgAsmElfDynamicSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfDynamicSection>(*n));
        break;

      case V_SgAsmElfSymbolSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymbolSection>(*n));
        break;

      case V_SgAsmElfStringSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfStringSection>(*n));
        break;

      case V_SgAsmElfEHFrameSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfEHFrameSection>(*n));
        break;

      case V_SgAsmElfNoteSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfNoteSection>(*n));
        break;

      case V_SgAsmElfSymverSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverSection>(*n));
        break;

      case V_SgAsmElfSymverDefinedSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverDefinedSection>(*n));
        break;

      case V_SgAsmElfSymverNeededSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverNeededSection>(*n));
        break;

      case V_SgAsmElfStrtab:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfStrtab>(*n));
        break;

      case V_SgAsmCoffStrtab:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmCoffStrtab>(*n));
        break;

      case V_SgAsmGenericStrtab:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericStrtab>(*n));
        break;

      case V_SgAsmElfSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSection>(*n));
        break;

      case V_SgAsmElfSectionTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSectionTable>(*n));
        break;

      case V_SgAsmElfSegmentTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSegmentTable>(*n));
        break;

      case V_SgAsmPEImportSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportSection>(*n));
        break;

      case V_SgAsmPEExportSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEExportSection>(*n));
        break;

      case V_SgAsmPEStringSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEStringSection>(*n));
        break;

      case V_SgAsmPESection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPESection>(*n));
        break;

      case V_SgAsmPESectionTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPESectionTable>(*n));
        break;

      case V_SgAsmCoffSymbolTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmCoffSymbolTable>(*n));
        break;

      case V_SgAsmDOSExtendedHeader:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDOSExtendedHeader>(*n));
        break;

      case V_SgAsmNESection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNESection>(*n));
        break;

      case V_SgAsmNESectionTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNESectionTable>(*n));
        break;

      case V_SgAsmNENameTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNENameTable>(*n));
        break;

      case V_SgAsmNEModuleTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNEModuleTable>(*n));
        break;

      case V_SgAsmNEStringTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNEStringTable>(*n));
        break;

      case V_SgAsmNEEntryTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNEEntryTable>(*n));
        break;

      case V_SgAsmNERelocTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNERelocTable>(*n));
        break;

      case V_SgAsmLESection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLESection>(*n));
        break;

      case V_SgAsmLESectionTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLESectionTable>(*n));
        break;

      case V_SgAsmLENameTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLENameTable>(*n));
        break;

      case V_SgAsmLEPageTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLEPageTable>(*n));
        break;

      case V_SgAsmLEEntryTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLEEntryTable>(*n));
        break;

      case V_SgAsmLERelocTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLERelocTable>(*n));
        break;

      case V_SgAsmGenericSection:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericSection>(*n));
        break;

      case V_SgAsmCoffSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmCoffSymbol>(*n));
        break;

      case V_SgAsmElfSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymbol>(*n));
        break;

      case V_SgAsmGenericSymbol:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericSymbol>(*n));
        break;

      case V_SgAsmElfSectionTableEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSectionTableEntry>(*n));
        break;

      case V_SgAsmElfSegmentTableEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSegmentTableEntry>(*n));
        break;

      case V_SgAsmElfSegmentTableEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSegmentTableEntryList>(*n));
        break;

      case V_SgAsmElfRelocEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfRelocEntry>(*n));
        break;

      case V_SgAsmElfRelocEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfRelocEntryList>(*n));
        break;

      case V_SgAsmElfDynamicEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfDynamicEntry>(*n));
        break;

      case V_SgAsmElfDynamicEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfDynamicEntryList>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCI:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfEHFrameEntryCI>(*n));
        break;

      case V_SgAsmElfEHFrameEntryCIList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfEHFrameEntryCIList>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFD:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfEHFrameEntryFD>(*n));
        break;

      case V_SgAsmElfEHFrameEntryFDList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfEHFrameEntryFDList>(*n));
        break;

      case V_SgAsmElfNoteEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfNoteEntry>(*n));
        break;

      case V_SgAsmElfNoteEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfNoteEntryList>(*n));
        break;

      case V_SgAsmElfSymverEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverEntry>(*n));
        break;

      case V_SgAsmElfSymverEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverEntryList>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverDefinedEntry>(*n));
        break;

      case V_SgAsmElfSymverDefinedEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverDefinedEntryList>(*n));
        break;

      case V_SgAsmElfSymverDefinedAux:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverDefinedAux>(*n));
        break;

      case V_SgAsmElfSymverDefinedAuxList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverDefinedAuxList>(*n));
        break;

      case V_SgAsmElfSymverNeededEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverNeededEntry>(*n));
        break;

      case V_SgAsmElfSymverNeededEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverNeededEntryList>(*n));
        break;

      case V_SgAsmElfSymverNeededAux:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverNeededAux>(*n));
        break;

      case V_SgAsmElfSymverNeededAuxList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymverNeededAuxList>(*n));
        break;

      case V_SgAsmPERVASizePair:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPERVASizePair>(*n));
        break;

      case V_SgAsmPEExportDirectory:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEExportDirectory>(*n));
        break;

      case V_SgAsmPEExportEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEExportEntry>(*n));
        break;

      case V_SgAsmPEImportDirectory:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportDirectory>(*n));
        break;

      case V_SgAsmPEImportILTEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportILTEntry>(*n));
        break;

      case V_SgAsmPEImportHNTEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportHNTEntry>(*n));
        break;

      case V_SgAsmPEImportLookupTable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEImportLookupTable>(*n));
        break;

      case V_SgAsmPESectionTableEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPESectionTableEntry>(*n));
        break;

      case V_SgAsmNEEntryPoint:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNEEntryPoint>(*n));
        break;

      case V_SgAsmNERelocEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNERelocEntry>(*n));
        break;

      case V_SgAsmNESectionTableEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNESectionTableEntry>(*n));
        break;

      case V_SgAsmLEPageTableEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLEPageTableEntry>(*n));
        break;

      case V_SgAsmLEEntryPoint:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLEEntryPoint>(*n));
        break;

      case V_SgAsmLESectionTableEntry:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmLESectionTableEntry>(*n));
        break;

      case V_SgAsmGenericSectionList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericSectionList>(*n));
        break;

      case V_SgAsmGenericHeaderList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericHeaderList>(*n));
        break;

      case V_SgAsmGenericSymbolList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericSymbolList>(*n));
        break;

      case V_SgAsmElfSymbolList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmElfSymbolList>(*n));
        break;

      case V_SgAsmCoffSymbolList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmCoffSymbolList>(*n));
        break;

      case V_SgAsmGenericDLLList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericDLLList>(*n));
        break;

      case V_SgAsmPERVASizePairList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPERVASizePairList>(*n));
        break;

      case V_SgAsmPEExportEntryList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmPEExportEntryList>(*n));
        break;

      case V_SgAsmBasicString:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmBasicString>(*n));
        break;

      case V_SgAsmStoredString:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmStoredString>(*n));
        break;

      case V_SgAsmGenericString:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericString>(*n));
        break;

      case V_SgAsmStringStorage:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmStringStorage>(*n));
        break;

      case V_SgAsmDwarfMacro:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfMacro>(*n));
        break;

      case V_SgAsmDwarfLine:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfLine>(*n));
        break;

      case V_SgAsmDwarfMacroList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfMacroList>(*n));
        break;

      case V_SgAsmDwarfLineList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfLineList>(*n));
        break;

      case V_SgAsmDwarfArrayType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfArrayType>(*n));
        break;

      case V_SgAsmDwarfClassType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfClassType>(*n));
        break;

      case V_SgAsmDwarfEntryPoint:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfEntryPoint>(*n));
        break;

      case V_SgAsmDwarfEnumerationType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfEnumerationType>(*n));
        break;

      case V_SgAsmDwarfFormalParameter:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfFormalParameter>(*n));
        break;

      case V_SgAsmDwarfImportedDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfImportedDeclaration>(*n));
        break;

      case V_SgAsmDwarfLabel:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfLabel>(*n));
        break;

      case V_SgAsmDwarfLexicalBlock:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfLexicalBlock>(*n));
        break;

      case V_SgAsmDwarfMember:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfMember>(*n));
        break;

      case V_SgAsmDwarfPointerType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfPointerType>(*n));
        break;

      case V_SgAsmDwarfReferenceType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfReferenceType>(*n));
        break;

      case V_SgAsmDwarfCompilationUnit:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfCompilationUnit>(*n));
        break;

      case V_SgAsmDwarfStringType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfStringType>(*n));
        break;

      case V_SgAsmDwarfStructureType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfStructureType>(*n));
        break;

      case V_SgAsmDwarfSubroutineType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfSubroutineType>(*n));
        break;

      case V_SgAsmDwarfTypedef:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfTypedef>(*n));
        break;

      case V_SgAsmDwarfUnionType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfUnionType>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedParameters:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfUnspecifiedParameters>(*n));
        break;

      case V_SgAsmDwarfVariant:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfVariant>(*n));
        break;

      case V_SgAsmDwarfCommonBlock:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfCommonBlock>(*n));
        break;

      case V_SgAsmDwarfCommonInclusion:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfCommonInclusion>(*n));
        break;

      case V_SgAsmDwarfInheritance:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfInheritance>(*n));
        break;

      case V_SgAsmDwarfInlinedSubroutine:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfInlinedSubroutine>(*n));
        break;

      case V_SgAsmDwarfModule:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfModule>(*n));
        break;

      case V_SgAsmDwarfPtrToMemberType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfPtrToMemberType>(*n));
        break;

      case V_SgAsmDwarfSetType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfSetType>(*n));
        break;

      case V_SgAsmDwarfSubrangeType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfSubrangeType>(*n));
        break;

      case V_SgAsmDwarfWithStmt:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfWithStmt>(*n));
        break;

      case V_SgAsmDwarfAccessDeclaration:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfAccessDeclaration>(*n));
        break;

      case V_SgAsmDwarfBaseType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfBaseType>(*n));
        break;

      case V_SgAsmDwarfCatchBlock:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfCatchBlock>(*n));
        break;

      case V_SgAsmDwarfConstType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfConstType>(*n));
        break;

      case V_SgAsmDwarfConstant:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfConstant>(*n));
        break;

      case V_SgAsmDwarfEnumerator:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfEnumerator>(*n));
        break;

      case V_SgAsmDwarfFileType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfFileType>(*n));
        break;

      case V_SgAsmDwarfFriend:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfFriend>(*n));
        break;

      case V_SgAsmDwarfNamelist:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfNamelist>(*n));
        break;

      case V_SgAsmDwarfNamelistItem:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfNamelistItem>(*n));
        break;

      case V_SgAsmDwarfPackedType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfPackedType>(*n));
        break;

      case V_SgAsmDwarfSubprogram:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfSubprogram>(*n));
        break;

      case V_SgAsmDwarfTemplateTypeParameter:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfTemplateTypeParameter>(*n));
        break;

      case V_SgAsmDwarfTemplateValueParameter:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfTemplateValueParameter>(*n));
        break;

      case V_SgAsmDwarfThrownType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfThrownType>(*n));
        break;

      case V_SgAsmDwarfTryBlock:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfTryBlock>(*n));
        break;

      case V_SgAsmDwarfVariantPart:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfVariantPart>(*n));
        break;

      case V_SgAsmDwarfVariable:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfVariable>(*n));
        break;

      case V_SgAsmDwarfVolatileType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfVolatileType>(*n));
        break;

      case V_SgAsmDwarfDwarfProcedure:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfDwarfProcedure>(*n));
        break;

      case V_SgAsmDwarfRestrictType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfRestrictType>(*n));
        break;

      case V_SgAsmDwarfInterfaceType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfInterfaceType>(*n));
        break;

      case V_SgAsmDwarfNamespace:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfNamespace>(*n));
        break;

      case V_SgAsmDwarfImportedModule:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfImportedModule>(*n));
        break;

      case V_SgAsmDwarfUnspecifiedType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfUnspecifiedType>(*n));
        break;

      case V_SgAsmDwarfPartialUnit:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfPartialUnit>(*n));
        break;

      case V_SgAsmDwarfImportedUnit:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfImportedUnit>(*n));
        break;

      case V_SgAsmDwarfMutableType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfMutableType>(*n));
        break;

      case V_SgAsmDwarfCondition:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfCondition>(*n));
        break;

      case V_SgAsmDwarfSharedType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfSharedType>(*n));
        break;

      case V_SgAsmDwarfFormatLabel:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfFormatLabel>(*n));
        break;

      case V_SgAsmDwarfFunctionTemplate:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfFunctionTemplate>(*n));
        break;

      case V_SgAsmDwarfClassTemplate:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfClassTemplate>(*n));
        break;

      case V_SgAsmDwarfUpcSharedType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfUpcSharedType>(*n));
        break;

      case V_SgAsmDwarfUpcStrictType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfUpcStrictType>(*n));
        break;

      case V_SgAsmDwarfUpcRelaxedType:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfUpcRelaxedType>(*n));
        break;

      case V_SgAsmDwarfUnknownConstruct:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfUnknownConstruct>(*n));
        break;

      case V_SgAsmDwarfConstruct:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfConstruct>(*n));
        break;

      case V_SgAsmDwarfConstructList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfConstructList>(*n));
        break;

      case V_SgAsmDwarfCompilationUnitList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfCompilationUnitList>(*n));
        break;

      case V_SgAsmDwarfInformation:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmDwarfInformation>(*n));
        break;

      case V_SgAsmExecutableFileFormat:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmExecutableFileFormat>(*n));
        break;

      case V_SgAsmInterpretation:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmInterpretation>(*n));
        break;

      case V_SgAsmInterpretationList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmInterpretationList>(*n));
        break;

      case V_SgAsmGenericFileList:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmGenericFileList>(*n));
        break;

      case V_SgAsmNode:
        Dispatcher::dispatch(rv, assume_sage_type<SgAsmNode>(*n));
        break;

      case V_SgInitializedName:
        Dispatcher::dispatch(rv, assume_sage_type<SgInitializedName>(*n));
        break;

      case V_SgOmpOrderedClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpOrderedClause>(*n));
        break;

      case V_SgOmpNowaitClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpNowaitClause>(*n));
        break;

      case V_SgOmpUntiedClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpUntiedClause>(*n));
        break;

      case V_SgOmpDefaultClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpDefaultClause>(*n));
        break;

      case V_SgOmpCollapseClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpCollapseClause>(*n));
        break;

      case V_SgOmpIfClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpIfClause>(*n));
        break;

      case V_SgOmpNumThreadsClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpNumThreadsClause>(*n));
        break;

      case V_SgOmpExpressionClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpExpressionClause>(*n));
        break;

      case V_SgOmpCopyprivateClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpCopyprivateClause>(*n));
        break;

      case V_SgOmpPrivateClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpPrivateClause>(*n));
        break;

      case V_SgOmpFirstprivateClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpFirstprivateClause>(*n));
        break;

      case V_SgOmpSharedClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpSharedClause>(*n));
        break;

      case V_SgOmpCopyinClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpCopyinClause>(*n));
        break;

      case V_SgOmpLastprivateClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpLastprivateClause>(*n));
        break;

      case V_SgOmpReductionClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpReductionClause>(*n));
        break;

      case V_SgOmpVariablesClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpVariablesClause>(*n));
        break;

      case V_SgOmpScheduleClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpScheduleClause>(*n));
        break;

      case V_SgOmpClause:
        Dispatcher::dispatch(rv, assume_sage_type<SgOmpClause>(*n));
        break;

      case V_SgRenamePair:
        Dispatcher::dispatch(rv, assume_sage_type<SgRenamePair>(*n));
        break;

      case V_SgInterfaceBody:
        Dispatcher::dispatch(rv, assume_sage_type<SgInterfaceBody>(*n));
        break;

      case V_SgLocatedNodeSupport:
        Dispatcher::dispatch(rv, assume_sage_type<SgLocatedNodeSupport>(*n));
        break;

      case V_SgToken:
        Dispatcher::dispatch(rv, assume_sage_type<SgToken>(*n));
        break;

      //
      // Types

      case V_SgTypeUnknown:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeUnknown>(*n));
        break;

      case V_SgTypeChar:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeChar>(*n));
        break;

      case V_SgTypeSignedChar:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeSignedChar>(*n));
        break;

      case V_SgTypeUnsignedChar:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeUnsignedChar>(*n));
        break;

      case V_SgTypeShort:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeShort>(*n));
        break;

      case V_SgTypeSignedShort:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeSignedShort>(*n));
        break;

      case V_SgTypeUnsignedShort:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeUnsignedShort>(*n));
        break;

      case V_SgTypeInt:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeInt>(*n));
        break;

      case V_SgTypeSignedInt:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeSignedInt>(*n));
        break;

      case V_SgTypeUnsignedInt:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeUnsignedInt>(*n));
        break;

      case V_SgTypeLong:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeLong>(*n));
        break;

      case V_SgTypeSignedLong:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeSignedLong>(*n));
        break;

      case V_SgTypeUnsignedLong:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeUnsignedLong>(*n));
        break;

      case V_SgTypeVoid:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeVoid>(*n));
        break;

      case V_SgTypeGlobalVoid:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeGlobalVoid>(*n));
        break;

      case V_SgTypeWchar:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeWchar>(*n));
        break;

      case V_SgTypeFloat:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeFloat>(*n));
        break;

      case V_SgTypeDouble:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeDouble>(*n));
        break;

      case V_SgTypeLongLong:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeLongLong>(*n));
        break;

      case V_SgTypeSignedLongLong:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeSignedLongLong>(*n));
        break;

      case V_SgTypeUnsignedLongLong:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeUnsignedLongLong>(*n));
        break;

      case V_SgTypeLongDouble:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeLongDouble>(*n));
        break;

      case V_SgTypeString:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeString>(*n));
        break;

      case V_SgTypeBool:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeBool>(*n));
        break;

      case V_SgTypeComplex:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeComplex>(*n));
        break;

      case V_SgTypeImaginary:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeImaginary>(*n));
        break;

      case V_SgTypeDefault:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeDefault>(*n));
        break;

      case V_SgPointerMemberType:
        Dispatcher::dispatch(rv, assume_sage_type<SgPointerMemberType>(*n));
        break;

      case V_SgReferenceType:
        Dispatcher::dispatch(rv, assume_sage_type<SgReferenceType>(*n));
        break;

      case V_SgTypeCAFTeam:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeCAFTeam>(*n));
        break;

      case V_SgClassType:
        Dispatcher::dispatch(rv, assume_sage_type<SgClassType>(*n));
        break;

      case V_SgTemplateType:
        Dispatcher::dispatch(rv, assume_sage_type<SgTemplateType>(*n));
        break;

      case V_SgEnumType:
        Dispatcher::dispatch(rv, assume_sage_type<SgEnumType>(*n));
        break;

      case V_SgTypedefType:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypedefType>(*n));
        break;

      case V_SgModifierType:
        Dispatcher::dispatch(rv, assume_sage_type<SgModifierType>(*n));
        break;

      case V_SgPartialFunctionModifierType:
        Dispatcher::dispatch(rv, assume_sage_type<SgPartialFunctionModifierType>(*n));
        break;

      case V_SgArrayType:
        Dispatcher::dispatch(rv, assume_sage_type<SgArrayType>(*n));
        break;

      case V_SgTypeEllipse:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeEllipse>(*n));
        break;

      case V_SgTypeCrayPointer:
        Dispatcher::dispatch(rv, assume_sage_type<SgTypeCrayPointer>(*n));
        break;

      case V_SgPartialFunctionType:
        Dispatcher::dispatch(rv, assume_sage_type<SgPartialFunctionType>(*n));
        break;

      case V_SgMemberFunctionType:
        Dispatcher::dispatch(rv, assume_sage_type<SgMemberFunctionType>(*n));
        break;

      case V_SgFunctionType:
        Dispatcher::dispatch(rv, assume_sage_type<SgFunctionType>(*n));
        break;

      case V_SgPointerType:
        Dispatcher::dispatch(rv, assume_sage_type<SgPointerType>(*n));
        break;

      case V_SgNamedType:
        Dispatcher::dispatch(rv, assume_sage_type<SgNamedType>(*n));
        break;

      case V_SgQualifiedNameType:
        Dispatcher::dispatch(rv, assume_sage_type<SgQualifiedNameType>(*n));
        break;

      // intermediary types - should not occur during AST traversal
      // \note this list is incomplete
      case V_SgNode:
      case V_SgLocatedNode:
      case V_SgExpression:
      case V_SgStatement:
      case V_SgNumVariants:
      case V_SgType:        /* fall-through */
      default:
        unexpected_node(*n);
    }

    return rv;
  }


  /// \brief    uncovers the type of SgNode and passes it to an
  ///           overloaded function handle in RoseVisitor.
  /// \tparam   RoseVisitor, the visitor that will be called back with
  ///           the recovered type information. The handle function with
  ///           the most suitable SgNode type will get invoked.
  /// \param rv an instance of a rose visitor; note that the argument is essentially
  ///           passed by value (similar to STL's for_each).
  /// \param n  a Sage node
  /// \return   a copy of the RoseVisitor object
  /// \code
  ///   // Counts the number of all expression and statement nodes.
  ///   struct Counter
  ///   {
  ///      size_t expr;
  ///      size_t decl;
  ///
  ///      Counter() : expr(0), decl(0) {}
  ///
  ///      // base case (all uninteresting nodes fall into this function)
  ///      void handle(const SgNode&) {}
  ///
  ///      // expression specific code
  ///      void handle(const SgExpression&) { ++expr; }
  ///
  ///      // statement specific code
  ///      void handle(const SgStatement&)  { ++stmt; }
  ///   };
  ///
  ///   // Traverses all nodes in the subtree passed to run, and
  ///   // prints the expression / statement ratio.
  ///   struct Traversal : ASTTraversal
  ///   {
  ///     Counter ctr;
  ///
  ///     void visit(SgNode* n)
  ///     {
  ///       // Since ctr is copied when passed to dispatch
  ///       // the changed counter state has to be stored back.
  ///       ctr = sg::dispatch(ctr, n);
  ///
  ///       // When the counter object is passed by pointer, the object's data
  ///       // is manipulated directly.
  ///       //   sg::dispatch(&ctr, n);
  ///     }
  ///
  ///     void run(SgNode& root)
  ///     {
  ///       traverse(&root, preorder);
  ///
  ///       std::cout << "Expr/Stmt ratio = " << ratio(ctr.expr, ctr.stmt) << std::endl;
  ///     }
  ///
  ///     static
  ///     float ratio(float a, float b) { return a/b; }
  ///   };
  /// \endcode
  template <class RoseVisitor>
  inline
  RoseVisitor
  dispatch(const RoseVisitor& rv, SgNode* n)
  {
    return _dispatch(rv, n);
  }

  /// \overload
  template <class RoseVisitor>
  inline
  RoseVisitor
  dispatch(const RoseVisitor& rv, const SgNode* n)
  {
    return _dispatch(rv, n);
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
      res = sg::dispatch(AncestorFinder(), res.second);
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
  /// \code
  ///   const SgStatement* enclosingStatement(const SgExpression* e)  { return sg::ancestor<SgStatement>(e); }
  /// \endcode
  template <class AncestorNode>
  AncestorNode* ancestor(SgNode* n)
  {
    if (n == NULL) return NULL;

    return _ancestor<AncestorNode>(*n);
  }

  /// \overload
  template <class AncestorNode>
  const AncestorNode* ancestor(const SgNode* n)
  {
    if (n == NULL) return NULL;

    return _ancestor<const AncestorNode>(*n);
  }

  /// \overload
  template <class AncestorNode>
  AncestorNode& ancestor(SgNode& n)
  {
    AncestorNode* res = _ancestor<AncestorNode>(n);

    ROSE_ASSERT(res);
    return *res;
  }

  /// \overload
  template <class AncestorNode>
  const AncestorNode& ancestor(const SgNode& n)
  {
    const AncestorNode* res = _ancestor<const AncestorNode>(n);

    ROSE_ASSERT(res);
    return *res;
  }

  template <class SageNode>
  struct TypeRecoveryHandler
  {
    typedef typename ConstLike<SageNode, SgNode>::type SgBaseNode;

    SageNode* res;

    TypeRecoveryHandler()
    : res(NULL)
    {}

    void handle(SgBaseNode& n) { unexpected_node(n); }
    void handle(SageNode& n)   { res = &n; }

    operator SageNode* () { return res; }
  };

  /// \brief  asserts that n has type SageNode
  /// \code
  ///   SgStatement* stmt = assert_sage_type<SgStatement>(expr.get_parent());
  ///   ROSE_ASSERT(stmt); // holds b/c assert_sage_type aborts if the input node is not a SgStatement
  /// \endcode
  template <class SageNode>
  SageNode* assert_sage_type(SgNode* n)
  {
    return sg::dispatch(TypeRecoveryHandler<SageNode>(), n);
  }

  /// \overload
  template <class SageNode>
  const SageNode* assert_sage_type(const SgNode* n)
  {
    return sg::dispatch(TypeRecoveryHandler<const SageNode>(), n);
  }

  /// \brief swaps the parent pointer of two nodes
  /// \note  internal use
  static inline
  void swap_parent(SgNode* lhs, SgNode* rhs)
  {
    SgNode* tmp = lhs->get_parent();

    lhs->set_parent(rhs->get_parent());
    rhs->set_parent(tmp);
  }

  /// \overload
  /// \note for non sage nodes (nodes without a parent)
  ///       e.g., Rose_Containers
  static inline
  void swap_parent(void*, void*) {}

  /// \brief  swaps children (of equal kind) between two ancestor nodes of the same type
  /// \tparam SageNode the parent node type
  /// \tparam SageChild the child node type
  /// \param  lhs one parent node
  /// \param  rhs another parent node
  /// \param  getter the getter function to extract the child from @lhs (and @rhs)
  /// \param  setter the setter function to store the child in @lhs (and @rhs)
  template <class SageNode, class SageChild>
  void swap_child(SageNode& lhs, SageNode& rhs, SageChild* (SageNode::*getter) () const, void (SageNode::*setter) (SageChild*))
  {
    SageChild* lhs_child = (lhs.*getter)();
    SageChild* rhs_child = (rhs.*getter)();
    ROSE_ASSERT(lhs_child && rhs_child);

    (lhs.*setter)(rhs_child);
    (rhs.*setter)(lhs_child);

    swap_parent(lhs_child, rhs_child);
  }
}
#endif /* _SAGEGENERIC_HPP */
