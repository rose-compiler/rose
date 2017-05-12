#ifndef _SAGEGENERIC_H

#define _SAGEGENERIC_H

/// \file sageGeneric.h
/// This file implements generic (template) sage query functions
/// Currently this includes functions for:
/// - dispatching according to the type of a sage node (dispatch)
/// - finding the ancestor with a specific node type (ancestor)
/// - recovering the type of a sage node assertively (assert_node_type)

// note: the comments are right aligned to support code-blocks doxygen 1.3.X :)

#include <stdexcept>

#if !defined(NDEBUG)
#include <typeinfo>
#include <iostream>
#endif /* NDEBUG */


#define WITH_BINARY_ANALYSIS 0

// #include "Cxx_Grammar.h"

// DQ (10/5/2014): We can't include this here.
// #include "rose.h"


namespace sg
{
  //
  // non sage specific utilities

/// \brief portable implementation to avoid compiler warnings for
///        unused parameter.
  template <class T>
  static inline
  void unused(const T&) {}

/// \brief  dereferences an object (= checked dereference in debug mode)
  template <class T>
  T& deref(T* ptr)
  {
    assert(ptr);
    return *ptr;
  }

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

  //
  // error reporting

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

#define GEN_VISIT(X) \
    void visit(X * n) { rv.handle(*n); }

  template <class RoseVisitor>
  struct VisitDispatcher : ROSE_VisitorPatternDefaultBase
  {
    explicit
    VisitDispatcher(const RoseVisitor& rosevisitor)
    : rv(rosevisitor)
    {}

    GEN_VISIT(SgNode)

    GEN_VISIT(SgName)
    GEN_VISIT(SgSymbolTable)
    GEN_VISIT(SgPragma)
    GEN_VISIT(SgModifierNodes)
    GEN_VISIT(SgConstVolatileModifier)
    GEN_VISIT(SgStorageModifier)
    GEN_VISIT(SgAccessModifier)
    GEN_VISIT(SgFunctionModifier)
    GEN_VISIT(SgUPC_AccessModifier)
    GEN_VISIT(SgLinkageModifier)
    GEN_VISIT(SgSpecialFunctionModifier)
    GEN_VISIT(SgTypeModifier)
    GEN_VISIT(SgElaboratedTypeModifier)
    GEN_VISIT(SgBaseClassModifier)
    GEN_VISIT(SgDeclarationModifier)
    GEN_VISIT(SgOpenclAccessModeModifier)
    GEN_VISIT(SgModifier)
    GEN_VISIT(Sg_File_Info)
    GEN_VISIT(SgSourceFile)

    GEN_VISIT(SgUnknownFile)
    GEN_VISIT(SgFile)
    GEN_VISIT(SgFileList)
    GEN_VISIT(SgDirectory)
    GEN_VISIT(SgDirectoryList)
    GEN_VISIT(SgProject)
    GEN_VISIT(SgOptions)
    GEN_VISIT(SgUnparse_Info)
    GEN_VISIT(SgFuncDecl_attr)
    GEN_VISIT(SgClassDecl_attr)
    GEN_VISIT(SgTypedefSeq)
    GEN_VISIT(SgFunctionParameterTypeList)
    GEN_VISIT(SgTemplateParameter)
    GEN_VISIT(SgTemplateArgument)
    GEN_VISIT(SgTemplateParameterList)
    GEN_VISIT(SgTemplateArgumentList)
    GEN_VISIT(SgTemplateMemberFunctionDeclaration)
    GEN_VISIT(SgTemplateFunctionDeclaration)
    GEN_VISIT(SgTemplateFunctionDefinition)
    GEN_VISIT(SgTemplateVariableDeclaration)
    GEN_VISIT(SgTemplateParameterVal)
    GEN_VISIT(SgTemplateFunctionRefExp)
    GEN_VISIT(SgTemplateMemberFunctionRefExp)
    GEN_VISIT(SgTypeTraitBuiltinOperator)
    GEN_VISIT(SgBitAttribute)
    GEN_VISIT(SgAttribute)
    GEN_VISIT(SgBaseClass)
    GEN_VISIT(SgUndirectedGraphEdge)
    GEN_VISIT(SgDirectedGraphEdge)
    GEN_VISIT(SgGraphNode)
    GEN_VISIT(SgGraphEdge)
    GEN_VISIT(SgStringKeyedBidirectionalGraph)
    GEN_VISIT(SgIntKeyedBidirectionalGraph)
    GEN_VISIT(SgBidirectionalGraph)
    GEN_VISIT(SgIncidenceDirectedGraph)
    GEN_VISIT(SgIncidenceUndirectedGraph)
    GEN_VISIT(SgGraph)
    GEN_VISIT(SgGraphNodeList)
    GEN_VISIT(SgGraphEdgeList)
    GEN_VISIT(SgQualifiedName)
    GEN_VISIT(SgNameGroup)
    GEN_VISIT(SgCommonBlockObject)
    GEN_VISIT(SgDimensionObject)
    GEN_VISIT(SgDataStatementGroup)
    GEN_VISIT(SgDataStatementObject)
    GEN_VISIT(SgDataStatementValue)
    GEN_VISIT(SgFormatItem)
    GEN_VISIT(SgFormatItemList)
    GEN_VISIT(SgTypeTable)
    GEN_VISIT(SgSupport)
    GEN_VISIT(SgForStatement)
    GEN_VISIT(SgMatlabForStatement)
    GEN_VISIT(SgForInitStatement)
    GEN_VISIT(SgCatchStatementSeq)
    GEN_VISIT(SgFunctionParameterList)
    GEN_VISIT(SgCtorInitializerList)
    GEN_VISIT(SgBasicBlock)
    GEN_VISIT(SgGlobal)
    GEN_VISIT(SgIfStmt)
    GEN_VISIT(SgFunctionDefinition)
    GEN_VISIT(SgWhileStmt)
    GEN_VISIT(SgDoWhileStmt)
    GEN_VISIT(SgSwitchStatement)
    GEN_VISIT(SgCatchOptionStmt)
    GEN_VISIT(SgVariableDeclaration)
    GEN_VISIT(SgVariableDefinition)
    GEN_VISIT(SgEnumDeclaration)
    GEN_VISIT(SgAsmStmt)
    GEN_VISIT(SgTypedefDeclaration)
    GEN_VISIT(SgFunctionTypeTable)
    GEN_VISIT(SgExprStatement)
    GEN_VISIT(SgLabelStatement)
    GEN_VISIT(SgCaseOptionStmt)
    GEN_VISIT(SgTryStmt)
    GEN_VISIT(SgDefaultOptionStmt)
    GEN_VISIT(SgBreakStmt)
    GEN_VISIT(SgContinueStmt)
    GEN_VISIT(SgReturnStmt)
    GEN_VISIT(SgGotoStatement)
    GEN_VISIT(SgSpawnStmt)
    GEN_VISIT(SgNullStatement)
    GEN_VISIT(SgVariantStatement)
    GEN_VISIT(SgPragmaDeclaration)
    GEN_VISIT(SgTemplateDeclaration)
    GEN_VISIT(SgTemplateInstantiationDecl)
    GEN_VISIT(SgTemplateInstantiationDefn)
    GEN_VISIT(SgTemplateClassDeclaration)
    GEN_VISIT(SgTemplateClassDefinition)
    GEN_VISIT(SgTemplateInstantiationFunctionDecl)
    GEN_VISIT(SgTemplateInstantiationMemberFunctionDecl)
    GEN_VISIT(SgProgramHeaderStatement)
    GEN_VISIT(SgProcedureHeaderStatement)
    GEN_VISIT(SgEntryStatement)
    GEN_VISIT(SgFortranNonblockedDo)
    GEN_VISIT(SgInterfaceStatement)
    GEN_VISIT(SgParameterStatement)
    GEN_VISIT(SgCommonBlock)
    GEN_VISIT(SgModuleStatement)
    GEN_VISIT(SgUseStatement)
    GEN_VISIT(SgStopOrPauseStatement)
    GEN_VISIT(SgPrintStatement)
    GEN_VISIT(SgReadStatement)
    GEN_VISIT(SgWriteStatement)
    GEN_VISIT(SgOpenStatement)
    GEN_VISIT(SgCloseStatement)
    GEN_VISIT(SgInquireStatement)
    GEN_VISIT(SgFlushStatement)
    GEN_VISIT(SgBackspaceStatement)
    GEN_VISIT(SgRewindStatement)
    GEN_VISIT(SgEndfileStatement)
    GEN_VISIT(SgWaitStatement)
    GEN_VISIT(SgCAFWithTeamStatement)
    GEN_VISIT(SgFormatStatement)
    GEN_VISIT(SgFortranDo)
    GEN_VISIT(SgForAllStatement)
    GEN_VISIT(SgIOStatement)
    GEN_VISIT(SgUpcNotifyStatement)
    GEN_VISIT(SgUpcWaitStatement)
    GEN_VISIT(SgUpcBarrierStatement)
    GEN_VISIT(SgUpcFenceStatement)
    GEN_VISIT(SgUpcForAllStatement)
    GEN_VISIT(SgOmpParallelStatement)
    GEN_VISIT(SgOmpSingleStatement)
    GEN_VISIT(SgOmpSimdStatement)
    GEN_VISIT(SgOmpTaskStatement)
    GEN_VISIT(SgOmpForStatement)
    GEN_VISIT(SgOmpDoStatement)
    GEN_VISIT(SgOmpSectionsStatement)
    GEN_VISIT(SgOmpClauseBodyStatement)
    GEN_VISIT(SgOmpAtomicStatement)
    GEN_VISIT(SgOmpMasterStatement)
    GEN_VISIT(SgOmpSectionStatement)
    GEN_VISIT(SgOmpOrderedStatement)
    GEN_VISIT(SgOmpWorkshareStatement)
    GEN_VISIT(SgOmpCriticalStatement)
    GEN_VISIT(SgOmpBodyStatement)
    GEN_VISIT(SgBlockDataStatement)
    GEN_VISIT(SgImplicitStatement)
    GEN_VISIT(SgStatementFunctionStatement)
    GEN_VISIT(SgWhereStatement)
    GEN_VISIT(SgNullifyStatement)
    GEN_VISIT(SgEquivalenceStatement)
    GEN_VISIT(SgDerivedTypeStatement)
    GEN_VISIT(SgAttributeSpecificationStatement)
    GEN_VISIT(SgAllocateStatement)
    GEN_VISIT(SgDeallocateStatement)
    GEN_VISIT(SgContainsStatement)
    GEN_VISIT(SgSequenceStatement)
    GEN_VISIT(SgElseWhereStatement)
    GEN_VISIT(SgArithmeticIfStatement)
    GEN_VISIT(SgAssignStatement)
    GEN_VISIT(SgComputedGotoStatement)
    GEN_VISIT(SgAssignedGotoStatement)
    GEN_VISIT(SgNamelistStatement)
    GEN_VISIT(SgImportStatement)
    GEN_VISIT(SgAssociateStatement)
    GEN_VISIT(SgFortranIncludeLine)
    GEN_VISIT(SgNamespaceDeclarationStatement)
    GEN_VISIT(SgNamespaceAliasDeclarationStatement)
    GEN_VISIT(SgNamespaceDefinitionStatement)
    GEN_VISIT(SgUsingDeclarationStatement)
    GEN_VISIT(SgUsingDirectiveStatement)
    GEN_VISIT(SgTemplateInstantiationDirectiveStatement)
    GEN_VISIT(SgClassDeclaration)
    GEN_VISIT(SgClassDefinition)
    GEN_VISIT(SgScopeStatement)
    GEN_VISIT(SgMemberFunctionDeclaration)
    GEN_VISIT(SgFunctionDeclaration)
    GEN_VISIT(SgIncludeDirectiveStatement)
    GEN_VISIT(SgDefineDirectiveStatement)
    GEN_VISIT(SgUndefDirectiveStatement)
    GEN_VISIT(SgIfdefDirectiveStatement)
    GEN_VISIT(SgIfndefDirectiveStatement)
    GEN_VISIT(SgIfDirectiveStatement)
    GEN_VISIT(SgDeadIfDirectiveStatement)
    GEN_VISIT(SgElseDirectiveStatement)
    GEN_VISIT(SgElseifDirectiveStatement)
    GEN_VISIT(SgEndifDirectiveStatement)
    GEN_VISIT(SgLineDirectiveStatement)
    GEN_VISIT(SgWarningDirectiveStatement)
    GEN_VISIT(SgErrorDirectiveStatement)
    GEN_VISIT(SgEmptyDirectiveStatement)
    GEN_VISIT(SgIncludeNextDirectiveStatement)
    GEN_VISIT(SgIdentDirectiveStatement)
    GEN_VISIT(SgLinemarkerDirectiveStatement)
    GEN_VISIT(SgC_PreprocessorDirectiveStatement)
    GEN_VISIT(SgClinkageStartStatement)
    GEN_VISIT(SgClinkageEndStatement)
    GEN_VISIT(SgClinkageDeclarationStatement)
    GEN_VISIT(SgOmpFlushStatement)
    GEN_VISIT(SgOmpBarrierStatement)
    GEN_VISIT(SgOmpTaskwaitStatement)
    GEN_VISIT(SgOmpThreadprivateStatement)
    GEN_VISIT(SgDeclarationStatement)
    GEN_VISIT(SgExprListExp)
    GEN_VISIT(SgVarRefExp)
    GEN_VISIT(SgLabelRefExp)
    GEN_VISIT(SgClassNameRefExp)
    GEN_VISIT(SgFunctionRefExp)
    GEN_VISIT(SgMemberFunctionRefExp)
    GEN_VISIT(SgFunctionCallExp)
    GEN_VISIT(SgSizeOfOp)
    GEN_VISIT(SgUpcLocalsizeofExpression)
    GEN_VISIT(SgUpcBlocksizeofExpression)
    GEN_VISIT(SgUpcElemsizeofExpression)
    GEN_VISIT(SgVarArgStartOp)
    GEN_VISIT(SgVarArgStartOneOperandOp)
    GEN_VISIT(SgVarArgOp)
    GEN_VISIT(SgVarArgEndOp)
    GEN_VISIT(SgVarArgCopyOp)
    GEN_VISIT(SgTypeIdOp)
    GEN_VISIT(SgConditionalExp)
    GEN_VISIT(SgNewExp)
    GEN_VISIT(SgDeleteExp)
    GEN_VISIT(SgThisExp)
    GEN_VISIT(SgRefExp)
    GEN_VISIT(SgAggregateInitializer)
    GEN_VISIT(SgConstructorInitializer)
    GEN_VISIT(SgAssignInitializer)
    GEN_VISIT(SgExpressionRoot)
    GEN_VISIT(SgMinusOp)
    GEN_VISIT(SgUnaryAddOp)
    GEN_VISIT(SgNotOp)
    GEN_VISIT(SgPointerDerefExp)
    GEN_VISIT(SgAddressOfOp)
    GEN_VISIT(SgMinusMinusOp)
    GEN_VISIT(SgPlusPlusOp)
    GEN_VISIT(SgBitComplementOp)
    GEN_VISIT(SgRealPartOp)
    GEN_VISIT(SgImagPartOp)
    GEN_VISIT(SgConjugateOp)
    GEN_VISIT(SgCastExp)
    GEN_VISIT(SgThrowOp)
    GEN_VISIT(SgArrowExp)
    GEN_VISIT(SgDotExp)
    GEN_VISIT(SgDotStarOp)
    GEN_VISIT(SgArrowStarOp)
    GEN_VISIT(SgEqualityOp)
    GEN_VISIT(SgLessThanOp)
    GEN_VISIT(SgGreaterThanOp)
    GEN_VISIT(SgNotEqualOp)
    GEN_VISIT(SgLessOrEqualOp)
    GEN_VISIT(SgGreaterOrEqualOp)
    GEN_VISIT(SgAddOp)
    GEN_VISIT(SgSubtractOp)
    GEN_VISIT(SgMultiplyOp)
    GEN_VISIT(SgDivideOp)
    GEN_VISIT(SgIntegerDivideOp)
    GEN_VISIT(SgModOp)
    GEN_VISIT(SgAndOp)
    GEN_VISIT(SgOrOp)
    GEN_VISIT(SgBitXorOp)
    GEN_VISIT(SgBitAndOp)
    GEN_VISIT(SgBitOrOp)
    GEN_VISIT(SgCommaOpExp)
    GEN_VISIT(SgLshiftOp)
    GEN_VISIT(SgRshiftOp)
    GEN_VISIT(SgPntrArrRefExp)
    GEN_VISIT(SgScopeOp)
    GEN_VISIT(SgAssignOp)
    GEN_VISIT(SgPlusAssignOp)
    GEN_VISIT(SgMinusAssignOp)
    GEN_VISIT(SgAndAssignOp)
    GEN_VISIT(SgIorAssignOp)
    GEN_VISIT(SgMultAssignOp)
    GEN_VISIT(SgDivAssignOp)
    GEN_VISIT(SgModAssignOp)
    GEN_VISIT(SgXorAssignOp)
    GEN_VISIT(SgLshiftAssignOp)
    GEN_VISIT(SgRshiftAssignOp)
    GEN_VISIT(SgConcatenationOp)
    GEN_VISIT(SgBoolValExp)
    GEN_VISIT(SgStringVal)
    GEN_VISIT(SgShortVal)
    GEN_VISIT(SgCharVal)
    GEN_VISIT(SgUnsignedCharVal)
    GEN_VISIT(SgWcharVal)
    GEN_VISIT(SgUnsignedShortVal)
    GEN_VISIT(SgIntVal)
    GEN_VISIT(SgEnumVal)
    GEN_VISIT(SgUnsignedIntVal)
    GEN_VISIT(SgLongIntVal)
    GEN_VISIT(SgLongLongIntVal)
    GEN_VISIT(SgUnsignedLongLongIntVal)
    GEN_VISIT(SgUnsignedLongVal)
    GEN_VISIT(SgFloatVal)
    GEN_VISIT(SgDoubleVal)
    GEN_VISIT(SgLongDoubleVal)
    GEN_VISIT(SgUpcThreads)
    GEN_VISIT(SgUpcMythread)
    GEN_VISIT(SgComplexVal)
    GEN_VISIT(SgNullExpression)
    GEN_VISIT(SgVariantExpression)
    GEN_VISIT(SgStatementExpression)
    GEN_VISIT(SgAsmOp)
    GEN_VISIT(SgCudaKernelExecConfig)
    GEN_VISIT(SgCudaKernelCallExp)
    GEN_VISIT(SgSubscriptExpression)
    GEN_VISIT(SgColonShapeExp)
    GEN_VISIT(SgAsteriskShapeExp)
    GEN_VISIT(SgIOItemExpression)
    GEN_VISIT(SgImpliedDo)
    GEN_VISIT(SgExponentiationOp)
    GEN_VISIT(SgUnknownArrayOrFunctionReference)
    GEN_VISIT(SgActualArgumentExpression)
    GEN_VISIT(SgUserDefinedBinaryOp)
    GEN_VISIT(SgPointerAssignOp)
    GEN_VISIT(SgCAFCoExpression)
    GEN_VISIT(SgDesignatedInitializer)
    GEN_VISIT(SgInitializer)
    GEN_VISIT(SgUserDefinedUnaryOp)
    GEN_VISIT(SgPseudoDestructorRefExp)
    GEN_VISIT(SgUnaryOp)
    GEN_VISIT(SgBinaryOp)
    GEN_VISIT(SgValueExp)
    GEN_VISIT(SgVariableSymbol)
    GEN_VISIT(SgFunctionTypeSymbol)
    GEN_VISIT(SgClassSymbol)
    GEN_VISIT(SgTemplateSymbol)
    GEN_VISIT(SgEnumSymbol)
    GEN_VISIT(SgEnumFieldSymbol)
    GEN_VISIT(SgTypedefSymbol)
    GEN_VISIT(SgMemberFunctionSymbol)
    GEN_VISIT(SgLabelSymbol)
    GEN_VISIT(SgDefaultSymbol)
    GEN_VISIT(SgNamespaceSymbol)
    GEN_VISIT(SgIntrinsicSymbol)
    GEN_VISIT(SgModuleSymbol)
    GEN_VISIT(SgInterfaceSymbol)
    GEN_VISIT(SgCommonSymbol)
    GEN_VISIT(SgRenameSymbol)
    GEN_VISIT(SgFunctionSymbol)
    GEN_VISIT(SgAsmBinaryAddressSymbol)
    GEN_VISIT(SgAsmBinaryDataSymbol)
    GEN_VISIT(SgAliasSymbol)
    GEN_VISIT(SgSymbol)

#if WITH_BINARY_ANALYSIS
    GEN_VISIT(SgBinaryComposite)
    GEN_VISIT(SgAsmBlock)
    GEN_VISIT(SgAsmOperandList)
    GEN_VISIT(SgAsmArmInstruction)
    GEN_VISIT(SgAsmX86Instruction)
    GEN_VISIT(SgAsmPowerpcInstruction)
    GEN_VISIT(SgAsmInstruction)
    GEN_VISIT(SgAsmStatement)
    GEN_VISIT(SgAsmBinaryAdd)
    GEN_VISIT(SgAsmBinarySubtract)
    GEN_VISIT(SgAsmBinaryMultiply)
    GEN_VISIT(SgAsmBinaryDivide)
    GEN_VISIT(SgAsmBinaryMod)
    GEN_VISIT(SgAsmBinaryAddPreupdate)
    GEN_VISIT(SgAsmBinarySubtractPreupdate)
    GEN_VISIT(SgAsmBinaryAddPostupdate)
    GEN_VISIT(SgAsmBinarySubtractPostupdate)
    GEN_VISIT(SgAsmBinaryLsl)
    GEN_VISIT(SgAsmBinaryLsr)
    GEN_VISIT(SgAsmBinaryAsr)
    GEN_VISIT(SgAsmBinaryRor)
    GEN_VISIT(SgAsmBinaryExpression)
    GEN_VISIT(SgAsmUnaryPlus)
    GEN_VISIT(SgAsmUnaryMinus)
    GEN_VISIT(SgAsmUnaryRrx)
    GEN_VISIT(SgAsmUnaryArmSpecialRegisterList)
    GEN_VISIT(SgAsmUnaryExpression)
    GEN_VISIT(SgAsmMemoryReferenceExpression)
    GEN_VISIT(SgAsmControlFlagsExpression)
    GEN_VISIT(SgAsmCommonSubExpression)
    GEN_VISIT(SgAsmDirectRegisterExpression)
    GEN_VISIT(SgAsmIndirectRegisterExpression)
    GEN_VISIT(SgAsmIntegerValueExpression)
    GEN_VISIT(SgAsmFloatValueExpression)
    GEN_VISIT(SgAsmValueExpression)
    GEN_VISIT(SgAsmExprListExp)
    GEN_VISIT(SgAsmExpression)
    GEN_VISIT(SgAsmIntegerType)
    GEN_VISIT(SgAsmFloatType)
    GEN_VISIT(SgAsmScalarType)
    GEN_VISIT(SgAsmVectorType)
    GEN_VISIT(SgAsmType)
    GEN_VISIT(SgAsmGenericDLL)
    GEN_VISIT(SgAsmPEImportItemList)
    GEN_VISIT(SgAsmPEImportDirectoryList)
    GEN_VISIT(SgAsmGenericFormat)
    GEN_VISIT(SgAsmGenericFile)
    GEN_VISIT(SgAsmElfFileHeader)
    GEN_VISIT(SgAsmPEFileHeader)
    GEN_VISIT(SgAsmNEFileHeader)
    GEN_VISIT(SgAsmLEFileHeader)
    GEN_VISIT(SgAsmDOSFileHeader)
    GEN_VISIT(SgAsmGenericHeader)
    GEN_VISIT(SgAsmElfRelocSection)
    GEN_VISIT(SgAsmElfDynamicSection)
    GEN_VISIT(SgAsmElfSymbolSection)
    GEN_VISIT(SgAsmElfStringSection)
    GEN_VISIT(SgAsmElfEHFrameSection)
    GEN_VISIT(SgAsmElfNoteSection)
    GEN_VISIT(SgAsmElfSymverSection)
    GEN_VISIT(SgAsmElfSymverDefinedSection)
    GEN_VISIT(SgAsmElfSymverNeededSection)
    GEN_VISIT(SgAsmElfStrtab)
    GEN_VISIT(SgAsmCoffStrtab)
    GEN_VISIT(SgAsmGenericStrtab)
    GEN_VISIT(SgAsmElfSection)
    GEN_VISIT(SgAsmElfSectionTable)
    GEN_VISIT(SgAsmElfSegmentTable)
    GEN_VISIT(SgAsmPEImportSection)
    GEN_VISIT(SgAsmPEExportSection)
    GEN_VISIT(SgAsmPEStringSection)
    GEN_VISIT(SgAsmPESection)
    GEN_VISIT(SgAsmPESectionTable)
    GEN_VISIT(SgAsmCoffSymbolTable)
    GEN_VISIT(SgAsmDOSExtendedHeader)
    GEN_VISIT(SgAsmNESection)
    GEN_VISIT(SgAsmNESectionTable)
    GEN_VISIT(SgAsmNENameTable)
    GEN_VISIT(SgAsmNEModuleTable)
    GEN_VISIT(SgAsmNEStringTable)
    GEN_VISIT(SgAsmNEEntryTable)
    GEN_VISIT(SgAsmNERelocTable)
    GEN_VISIT(SgAsmLESection)
    GEN_VISIT(SgAsmLESectionTable)
    GEN_VISIT(SgAsmLENameTable)
    GEN_VISIT(SgAsmLEPageTable)
    GEN_VISIT(SgAsmLEEntryTable)
    GEN_VISIT(SgAsmLERelocTable)
    GEN_VISIT(SgAsmGenericSection)
    GEN_VISIT(SgAsmCoffSymbol)
    GEN_VISIT(SgAsmElfSymbol)
    GEN_VISIT(SgAsmGenericSymbol)
    GEN_VISIT(SgAsmElfSectionTableEntry)
    GEN_VISIT(SgAsmElfSegmentTableEntry)
    GEN_VISIT(SgAsmElfSegmentTableEntryList)
    GEN_VISIT(SgAsmElfRelocEntry)
    GEN_VISIT(SgAsmElfRelocEntryList)
    GEN_VISIT(SgAsmElfDynamicEntry)
    GEN_VISIT(SgAsmElfDynamicEntryList)
    GEN_VISIT(SgAsmElfEHFrameEntryCI)
    GEN_VISIT(SgAsmElfEHFrameEntryCIList)
    GEN_VISIT(SgAsmElfEHFrameEntryFD)
    GEN_VISIT(SgAsmElfEHFrameEntryFDList)
    GEN_VISIT(SgAsmElfNoteEntry)
    GEN_VISIT(SgAsmElfNoteEntryList)
    GEN_VISIT(SgAsmElfSymverEntry)
    GEN_VISIT(SgAsmElfSymverEntryList)
    GEN_VISIT(SgAsmElfSymverDefinedEntry)
    GEN_VISIT(SgAsmElfSymverDefinedEntryList)
    GEN_VISIT(SgAsmElfSymverDefinedAux)
    GEN_VISIT(SgAsmElfSymverDefinedAuxList)
    GEN_VISIT(SgAsmElfSymverNeededEntry)
    GEN_VISIT(SgAsmElfSymverNeededEntryList)
    GEN_VISIT(SgAsmElfSymverNeededAux)
    GEN_VISIT(SgAsmElfSymverNeededAuxList)
    GEN_VISIT(SgAsmPERVASizePair)
    GEN_VISIT(SgAsmPEExportDirectory)
    GEN_VISIT(SgAsmPEExportEntry)
    GEN_VISIT(SgAsmPEImportDirectory)
    GEN_VISIT(SgAsmPEImportItem)
    GEN_VISIT(SgAsmPESectionTableEntry)
    GEN_VISIT(SgAsmNEEntryPoint)
    GEN_VISIT(SgAsmNERelocEntry)
    GEN_VISIT(SgAsmNESectionTableEntry)
    GEN_VISIT(SgAsmLEPageTableEntry)
    GEN_VISIT(SgAsmLEEntryPoint)
    GEN_VISIT(SgAsmLESectionTableEntry)
    GEN_VISIT(SgAsmGenericSectionList)
    GEN_VISIT(SgAsmGenericHeaderList)
    GEN_VISIT(SgAsmGenericSymbolList)
    GEN_VISIT(SgAsmElfSymbolList)
    GEN_VISIT(SgAsmCoffSymbolList)
    GEN_VISIT(SgAsmGenericDLLList)
    GEN_VISIT(SgAsmPERVASizePairList)
    GEN_VISIT(SgAsmPEExportEntryList)
    GEN_VISIT(SgAsmBasicString)
    GEN_VISIT(SgAsmStoredString)
    GEN_VISIT(SgAsmGenericString)
    GEN_VISIT(SgAsmStringStorage)
    GEN_VISIT(SgAsmDwarfMacro)
    GEN_VISIT(SgAsmDwarfLine)
    GEN_VISIT(SgAsmDwarfMacroList)
    GEN_VISIT(SgAsmDwarfLineList)
    GEN_VISIT(SgAsmDwarfArrayType)
    GEN_VISIT(SgAsmDwarfClassType)
    GEN_VISIT(SgAsmDwarfEntryPoint)
    GEN_VISIT(SgAsmDwarfEnumerationType)
    GEN_VISIT(SgAsmDwarfFormalParameter)
    GEN_VISIT(SgAsmDwarfImportedDeclaration)
    GEN_VISIT(SgAsmDwarfLabel)
    GEN_VISIT(SgAsmDwarfLexicalBlock)
    GEN_VISIT(SgAsmDwarfMember)
    GEN_VISIT(SgAsmDwarfPointerType)
    GEN_VISIT(SgAsmDwarfReferenceType)
    GEN_VISIT(SgAsmDwarfCompilationUnit)
    GEN_VISIT(SgAsmDwarfStringType)
    GEN_VISIT(SgAsmDwarfStructureType)
    GEN_VISIT(SgAsmDwarfSubroutineType)
    GEN_VISIT(SgAsmDwarfTypedef)
    GEN_VISIT(SgAsmDwarfUnionType)
    GEN_VISIT(SgAsmDwarfUnspecifiedParameters)
    GEN_VISIT(SgAsmDwarfVariant)
    GEN_VISIT(SgAsmDwarfCommonBlock)
    GEN_VISIT(SgAsmDwarfCommonInclusion)
    GEN_VISIT(SgAsmDwarfInheritance)
    GEN_VISIT(SgAsmDwarfInlinedSubroutine)
    GEN_VISIT(SgAsmDwarfModule)
    GEN_VISIT(SgAsmDwarfPtrToMemberType)
    GEN_VISIT(SgAsmDwarfSetType)
    GEN_VISIT(SgAsmDwarfSubrangeType)
    GEN_VISIT(SgAsmDwarfWithStmt)
    GEN_VISIT(SgAsmDwarfAccessDeclaration)
    GEN_VISIT(SgAsmDwarfBaseType)
    GEN_VISIT(SgAsmDwarfCatchBlock)
    GEN_VISIT(SgAsmDwarfConstType)
    GEN_VISIT(SgAsmDwarfConstant)
    GEN_VISIT(SgAsmDwarfEnumerator)
    GEN_VISIT(SgAsmDwarfFileType)
    GEN_VISIT(SgAsmDwarfFriend)
    GEN_VISIT(SgAsmDwarfNamelist)
    GEN_VISIT(SgAsmDwarfNamelistItem)
    GEN_VISIT(SgAsmDwarfPackedType)
    GEN_VISIT(SgAsmDwarfSubprogram)
    GEN_VISIT(SgAsmDwarfTemplateTypeParameter)
    GEN_VISIT(SgAsmDwarfTemplateValueParameter)
    GEN_VISIT(SgAsmDwarfThrownType)
    GEN_VISIT(SgAsmDwarfTryBlock)
    GEN_VISIT(SgAsmDwarfVariantPart)
    GEN_VISIT(SgAsmDwarfVariable)
    GEN_VISIT(SgAsmDwarfVolatileType)
    GEN_VISIT(SgAsmDwarfDwarfProcedure)
    GEN_VISIT(SgAsmDwarfRestrictType)
    GEN_VISIT(SgAsmDwarfInterfaceType)
    GEN_VISIT(SgAsmDwarfNamespace)
    GEN_VISIT(SgAsmDwarfImportedModule)
    GEN_VISIT(SgAsmDwarfUnspecifiedType)
    GEN_VISIT(SgAsmDwarfPartialUnit)
    GEN_VISIT(SgAsmDwarfImportedUnit)
    GEN_VISIT(SgAsmDwarfMutableType)
    GEN_VISIT(SgAsmDwarfCondition)
    GEN_VISIT(SgAsmDwarfSharedType)
    GEN_VISIT(SgAsmDwarfFormatLabel)
    GEN_VISIT(SgAsmDwarfFunctionTemplate)
    GEN_VISIT(SgAsmDwarfClassTemplate)
    GEN_VISIT(SgAsmDwarfUpcSharedType)
    GEN_VISIT(SgAsmDwarfUpcStrictType)
    GEN_VISIT(SgAsmDwarfUpcRelaxedType)
    GEN_VISIT(SgAsmDwarfUnknownConstruct)
    GEN_VISIT(SgAsmDwarfConstruct)
    GEN_VISIT(SgAsmDwarfConstructList)
    GEN_VISIT(SgAsmDwarfCompilationUnitList)
    GEN_VISIT(SgAsmDwarfInformation)
    GEN_VISIT(SgAsmExecutableFileFormat)
    GEN_VISIT(SgAsmInterpretation)
    GEN_VISIT(SgAsmInterpretationList)
    GEN_VISIT(SgAsmGenericFileList)
    GEN_VISIT(SgAsmNode)
#endif /* WITH_BINARY_ANALYSIS */

    GEN_VISIT(SgInitializedName)
    GEN_VISIT(SgOmpOrderedClause)
    GEN_VISIT(SgOmpNowaitClause)
    GEN_VISIT(SgOmpUntiedClause)
    GEN_VISIT(SgOmpDefaultClause)
    GEN_VISIT(SgOmpCollapseClause)
    GEN_VISIT(SgOmpIfClause)
    GEN_VISIT(SgOmpNumThreadsClause)
    GEN_VISIT(SgOmpExpressionClause)
    GEN_VISIT(SgOmpCopyprivateClause)
    GEN_VISIT(SgOmpPrivateClause)
    GEN_VISIT(SgOmpFirstprivateClause)
    GEN_VISIT(SgOmpSharedClause)
    GEN_VISIT(SgOmpCopyinClause)
    GEN_VISIT(SgOmpLastprivateClause)
    GEN_VISIT(SgOmpReductionClause)
    GEN_VISIT(SgOmpVariablesClause)
    GEN_VISIT(SgOmpScheduleClause)
    GEN_VISIT(SgOmpClause)
    GEN_VISIT(SgRenamePair)
    GEN_VISIT(SgInterfaceBody)
    GEN_VISIT(SgLocatedNodeSupport)
    GEN_VISIT(SgToken)

    //
    // Types
    GEN_VISIT(SgTypeUnknown)
    GEN_VISIT(SgTypeChar)
    GEN_VISIT(SgTypeSignedChar)
    GEN_VISIT(SgTypeUnsignedChar)
    GEN_VISIT(SgTypeShort)
    GEN_VISIT(SgTypeSignedShort)
    GEN_VISIT(SgTypeUnsignedShort)
    GEN_VISIT(SgTypeInt)
    GEN_VISIT(SgTypeSignedInt)
    GEN_VISIT(SgTypeUnsignedInt)
    GEN_VISIT(SgTypeLong)
    GEN_VISIT(SgTypeSignedLong)
    GEN_VISIT(SgTypeUnsignedLong)
    GEN_VISIT(SgTypeVoid)
    GEN_VISIT(SgTypeGlobalVoid)
    GEN_VISIT(SgTypeWchar)
    GEN_VISIT(SgTypeFloat)
    GEN_VISIT(SgTypeDouble)
    GEN_VISIT(SgTypeLongLong)
    GEN_VISIT(SgTypeSignedLongLong)
    GEN_VISIT(SgTypeUnsignedLongLong)
    GEN_VISIT(SgTypeLongDouble)
    GEN_VISIT(SgTypeString)
    GEN_VISIT(SgTypeBool)
    GEN_VISIT(SgTypeComplex)
    GEN_VISIT(SgTypeImaginary)
    GEN_VISIT(SgTypeDefault)
    GEN_VISIT(SgPointerMemberType)
    GEN_VISIT(SgReferenceType)
    GEN_VISIT(SgTypeCAFTeam)
    GEN_VISIT(SgClassType)
    GEN_VISIT(SgTemplateType)
    GEN_VISIT(SgEnumType)
    GEN_VISIT(SgTypedefType)
    GEN_VISIT(SgModifierType)
    GEN_VISIT(SgPartialFunctionModifierType)
    GEN_VISIT(SgArrayType)
    GEN_VISIT(SgTypeEllipse)
    GEN_VISIT(SgTypeCrayPointer)
    GEN_VISIT(SgPartialFunctionType)
    GEN_VISIT(SgMemberFunctionType)
    GEN_VISIT(SgFunctionType)
    GEN_VISIT(SgPointerType)
    GEN_VISIT(SgNamedType)
    GEN_VISIT(SgQualifiedNameType)
   // DQ (4/5/2017): Added this case that shows up using GNU 6.1 and Boost 1.51 (or Boost 1.52).
    GEN_VISIT(SgDeclType)

    RoseVisitor rv;
  };

#undef GEN_VISIT

  template <class RoseVisitor>
  inline
  RoseVisitor
  _dispatch(const RoseVisitor& rv, SgNode* n)
  {
    ROSE_ASSERT(n);

    VisitDispatcher<RoseVisitor> vis(rv);

    n->accept(vis);
    return vis.rv;
  }

/// \brief    uncovers the type of SgNode and passes it to an
///           overloaded function handle in RoseVisitor.
/// \tparam   RoseVisitor the visitor that will be called back with
///           the recovered type information. The handle function with
///           the most suitable SgNode type will get invoked.
/// \param rv an instance of a rose visitor; note that the argument is essentially
///           passed by value (similar to STL's for_each).
/// \param n  a Sage node
/// \return   a copy of the RoseVisitor object
/// \details  The following code has two classes.
///           - Counter counts the number of all expression and statement nodes.
///             It implements handlers for SgNode (not interesting nodes),
///             for SgExpression and SgStatement (to count the nodes).
///           - Traversal inherits from ASTTraversal and contains a counter.
///             The dispatch function is invoked using a Counter object and
///             a pointer to an AST node. Since the counter object is passed
///             by value we need to store back the result (similar to
///             std::for_each).
/// \code
///   struct Counter
///   {
///      size_t expr;
///      size_t decl;
///
///      Counter() : expr(0), decl(0) {}
///
///      void handle(const SgNode&) {}
///      void handle(const SgExpression&) { ++expr; }
///      void handle(const SgStatement&)  { ++stmt; }
///   };
///
///   struct Traversal : ASTTraversal
///   {
///     Counter ctr;
///
///     void visit(SgNode* n)
///     {
///       ctr = sg::dispatch(ctr, n);
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
///             Alternatively, the dispatch function takes a pointer to a
///             handler object. In this case, the counter object is passed
///             as pointer, and ctr is manipulated
///             directly (no need to store back the result).
/// \code
///     void visit(SgNode* n)
///     {
///       sg::dispatch(&ctr, n);
///     }
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
    return _dispatch(rv, const_cast<SgNode*>(n));
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
/// \tparam  QualSgNode either const SgNode* or SgNode* depending
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
///          - SgNode*       -> AncestorNode*       ( result can be NULL )
///          - const SgNode* -> const AncestorNode* ( result can be NULL )
///          - SgNode&       -> AncestorNode&       ( assert(false) when an ancestor of
///                                                   the specified type cannot be found )
///          - const SgNode& -> const AncestorNode& ( assert(false) when an ancestor of
///                                                   the specified type cannot be found )
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

/// \brief   asserts that n has type SageNode
/// \details the ROSE assert in the following example holds b/c assert_sage_type
///          aborts if the input node is not a SgStatement
/// \code
///   SgStatement* stmt = assert_sage_type<SgStatement>(expr.get_parent());
///   ROSE_ASSERT(stmt);
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
/// \param  getter the getter function to extract the child from lhs (and rhs)
/// \param  setter the setter function to store the child in lhs (and rhs)
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
#endif /* _SAGEGENERIC_H */
