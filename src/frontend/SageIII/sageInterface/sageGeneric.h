#ifndef _SAGEGENERIC_H

#define _SAGEGENERIC_H 1

/// \file sageGeneric.h
/// This file implements generic (template) sage query functions
/// Currently this includes functions for:
/// - dispatching according to the type of a sage node (dispatch)
/// - finding the ancestor with a specific node type (ancestor)
/// - recovering the type of a sage node assertively (assert_node_type)

// note: the comments are right aligned to support code-blocks doxygen 1.3.X :)

#include <Cxx_GrammarVisitorSupport.h>
#include <type_traits>

#if !defined(NDEBUG)
#include <typeinfo>
#include <sstream>
#endif /* NDEBUG */

#define WITH_BINARY_NODES 0
#define WITH_UNTYPED_NODES 0

// #include "Cxx_Grammar.h"

// DQ (10/5/2014): We can't include this here.
// #include "rose.h"

#define SG_UNEXPECTED_NODE(X)       (sg::unexpected_node(X, __FILE__, __LINE__))
#define SG_DEREF(X)                 (sg::deref(X, __FILE__, __LINE__))
#define SG_ASSERT_TYPE(SAGENODE, N) (sg::assert_sage_type<SAGENODE>(N, __FILE__, __LINE__))
#define SG_ERROR_IF(COND, MSG)      (sg::report_error_if(COND, MSG, __FILE__, __LINE__))


/// This namespace contains template functions that operate on the ROSE AST
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

  //
  // error reporting

  /// converts object of type E to T via string conversion
  template <class T, class E>
  static inline
  T conv(const E& el)
  {
    T                 res;
#if !defined(NDEBUG)
    std::stringstream s;

    s << el;
    s >> res;
#endif /* NDEBUG */
    return res;
  }

  // \note implemented in SageInterfaceAda.h
  // \{
  [[noreturn]]
  void report_error(std::string desc, const char* file = nullptr, size_t ln = 0);

  [[noreturn]]
  void unexpected_node(const SgNode& n, const char* file = nullptr, size_t ln = 0);
  /// \}


  static inline
  void report_error_if(bool iserror, const std::string& desc, const char* file = nullptr, size_t ln = 0)
  {
    if (!iserror) return;

    report_error(desc, file, ln);
  }

/// \brief  dereferences an object (= checked dereference in debug mode)
  template <class T>
  T& deref(T* ptr, const char* file = 0, size_t ln = 0)
  {
    report_error_if(!ptr, "assertion failed: null dereference ", file, ln);
    return *ptr;
  }

  template <bool conv>
  struct ConditionalEnable : std::true_type // enable
  {};

  template <>
  struct ConditionalEnable<false>  // disable
  {};

  template <class U, class T>
  struct EnableConversion : ConditionalEnable<!std::is_same<U,T>::value && std::is_convertible<U,T>::value>
  {};

  /// experimental class for returning non-null pointers
  template <class T>
  struct NotNull
  {
      /// standard constructor testing that p is not nullptr
      /// \param p a not-null pointer
      /// \pre p != nullptr
      // not explicit to enable auto-conversion
      NotNull(T* p)
      : ptr(p)
      {
        SG_ERROR_IF(ptr == nullptr, "failed null pointer check.");
      }

      NotNull(const NotNull&)            = default;
      NotNull& operator=(const NotNull&) = default;
      ~NotNull()                         = default;

      /// converting ctor for derived types and non-const versions of T
      /// \note
      ///   WithConversion<U*,T*>::value may fail SFINAE when
      ///   U is not convertible to T.
      template <class U, bool = EnableConversion<U*,T*>::value>
      NotNull(NotNull<U> nn)
      : ptr(nn.pointer())
      {}

      /// dereference operator returns reference to object
      T& operator*()  const { return *ptr; }

      /// arrow operator returns pointer to object
      T* operator->() const { return ptr; }

      /// implicit conversion operator
      operator T*()   const { return ptr; }

      /// explicit conversion operator
      T* pointer()    const { return ptr; }

    private:
      T* ptr; // must be initialized in ctors

      // no default construction
      NotNull()                     = delete;

      // NEITHER delete NOR define move ctor/assignment,
      //   to make the compiler use the copy-versions.
      // NotNull(NotNull&&)           = delete;
      // NotNull& operator(NotNull&&) = delete;
  };

  template <class T, class U, bool = EnableConversion<U*,T*>::value || EnableConversion<T*,U*>::value>
  bool operator<(NotNull<T> lhs, NotNull<U> rhs)
  {
    return lhs.pointer() < rhs.pointer();
  }

/**
 * struct DispatchHandler
 *
 * @brief Base class for any handlers passed to @ref dispatch
 *
 * This templated class should be used as a BaseClass for Handlers to
 * be passed to dispatch.  "handle" functions will have to be
 * implemented for each possible type to be handled.  @ref _ReturnType
 * holds any data that should be returned from the traversal.
 **/
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
    // rvalue ctor
    VisitDispatcher(RoseVisitor&& rosevisitor, std::false_type)
    : rv(std::move(rosevisitor))
    {}

    // lvalue ctor
    VisitDispatcher(const RoseVisitor& rosevisitor, std::true_type)
    : rv(rosevisitor)
    {}

    GEN_VISIT(SgAccessModifier)
    GEN_VISIT(SgActualArgumentExpression)
    GEN_VISIT(SgAbsOp)
    GEN_VISIT(SgAdaAccessType)
    GEN_VISIT(SgAdaAcceptStmt)
    GEN_VISIT(SgAdaComponentClause)
    GEN_VISIT(SgAdaDelayStmt)
    GEN_VISIT(SgAdaEntryDecl)
    GEN_VISIT(SgAdaExitStmt)
    GEN_VISIT(SgAdaDiscreteType)
    GEN_VISIT(SgAdaFloatVal)
    GEN_VISIT(SgAdaFormalType)
    GEN_VISIT(SgAdaFormalTypeDecl)
    GEN_VISIT(SgAdaFunctionRenamingDecl)
    GEN_VISIT(SgAdaGenericDecl)
    GEN_VISIT(SgAdaGenericInstanceDecl)
    GEN_VISIT(SgAdaGenericDefn)
    GEN_VISIT(SgAdaIndexConstraint)
    GEN_VISIT(SgAdaAttributeClause)
    GEN_VISIT(SgAdaLoopStmt)
    GEN_VISIT(SgAdaModularType)
    GEN_VISIT(SgAdaPackageBody)
    GEN_VISIT(SgAdaPackageBodyDecl)
    GEN_VISIT(SgAdaPackageSpec)
    GEN_VISIT(SgAdaPackageSpecDecl)
    GEN_VISIT(SgAdaPackageSymbol)
    GEN_VISIT(SgAdaRangeConstraint)
    GEN_VISIT(SgAdaRepresentationClause)
    GEN_VISIT(SgAdaEnumRepresentationClause)
    GEN_VISIT(SgAdaRenamingDecl)
    GEN_VISIT(SgAdaSelectStmt)
    GEN_VISIT(SgAdaSelectAlternativeStmt)
    GEN_VISIT(SgAdaSubtype)
    GEN_VISIT(SgAdaDerivedType)
    GEN_VISIT(SgAdaAttributeExp)
    GEN_VISIT(SgAdaTaskBody)
    GEN_VISIT(SgAdaTaskBodyDecl)
    GEN_VISIT(SgAdaTaskSpec)
    GEN_VISIT(SgAdaTaskSpecDecl)
    GEN_VISIT(SgAdaTaskSymbol)
    GEN_VISIT(SgAdaRenamingSymbol)
    GEN_VISIT(SgAdaTaskRefExp)
    GEN_VISIT(SgAdaRenamingRefExp)
    GEN_VISIT(SgAdaTaskType)
    GEN_VISIT(SgAdaTaskTypeDecl)
    GEN_VISIT(SgAdaTerminateStmt)
    GEN_VISIT(SgAdaTypeConstraint)
    GEN_VISIT(SgAddOp)
    GEN_VISIT(SgAddressOfOp)
    GEN_VISIT(SgAggregateInitializer)
    GEN_VISIT(SgAliasSymbol)
    GEN_VISIT(SgAllocateStatement)
    GEN_VISIT(SgAndAssignOp)
    GEN_VISIT(SgAndOp)
    GEN_VISIT(SgArithmeticIfStatement)
    GEN_VISIT(SgArrayType)
    GEN_VISIT(SgArrowExp)
    GEN_VISIT(SgArrowStarOp)
    GEN_VISIT(SgAssertStmt)
    GEN_VISIT(SgAssignInitializer)
    GEN_VISIT(SgAssignOp)
    GEN_VISIT(SgAssignStatement)
    GEN_VISIT(SgAssignedGotoStatement)
    GEN_VISIT(SgAssociateStatement)
    GEN_VISIT(SgAsteriskShapeExp)
    GEN_VISIT(SgAtOp)
    GEN_VISIT(SgAttribute)
    GEN_VISIT(SgAttributeSpecificationStatement)
    GEN_VISIT(SgAutoType)
    GEN_VISIT(SgAwaitExpression)
    GEN_VISIT(SgBackspaceStatement)
    GEN_VISIT(SgBaseClass)
    GEN_VISIT(SgExpBaseClass)
    GEN_VISIT(SgBaseClassModifier)
    GEN_VISIT(SgBasicBlock)
    GEN_VISIT(SgBidirectionalGraph)
    GEN_VISIT(SgBinaryOp)
    GEN_VISIT(SgBitAndOp)
    GEN_VISIT(SgBitAttribute)
    GEN_VISIT(SgBitComplementOp)
    GEN_VISIT(SgBitEqvOp)
    GEN_VISIT(SgBitOrOp)
    GEN_VISIT(SgBitXorOp)
    GEN_VISIT(SgBlockDataStatement)
    GEN_VISIT(SgBoolValExp)
    GEN_VISIT(SgBreakStmt)
    GEN_VISIT(SgBracedInitializer)
    GEN_VISIT(SgC_PreprocessorDirectiveStatement)
    GEN_VISIT(SgCaseOptionStmt)
    GEN_VISIT(SgCastExp)
    GEN_VISIT(SgCatchOptionStmt)
    GEN_VISIT(SgCatchStatementSeq)
    GEN_VISIT(SgCharVal)
    GEN_VISIT(SgChar16Val)
    GEN_VISIT(SgChar32Val)
    GEN_VISIT(SgChooseExpression)
    GEN_VISIT(SgClassDecl_attr)
    GEN_VISIT(SgClassDeclaration)
    GEN_VISIT(SgClassDefinition)
    GEN_VISIT(SgClassNameRefExp)
    GEN_VISIT(SgClassSymbol)
    GEN_VISIT(SgClassType)
    GEN_VISIT(SgClinkageDeclarationStatement)
    GEN_VISIT(SgClinkageEndStatement)
    GEN_VISIT(SgClinkageStartStatement)
    GEN_VISIT(SgCloseStatement)
    GEN_VISIT(SgColonShapeExp)
    GEN_VISIT(SgCommaOpExp)
    GEN_VISIT(SgCommonBlock)
    GEN_VISIT(SgCommonBlockObject)
    GEN_VISIT(SgCommonSymbol)
    GEN_VISIT(SgComplexVal)
    GEN_VISIT(SgComprehension)
    GEN_VISIT(SgCompoundAssignOp)
    GEN_VISIT(SgCompoundInitializer)
    GEN_VISIT(SgCompoundLiteralExp)
    GEN_VISIT(SgComputedGotoStatement)
    GEN_VISIT(SgConcatenationOp)
    GEN_VISIT(SgConditionalExp)
    GEN_VISIT(SgConjugateOp)
    GEN_VISIT(SgConstVolatileModifier)
    GEN_VISIT(SgConstructorInitializer)
    GEN_VISIT(SgContainsStatement)
    GEN_VISIT(SgContinueStmt)
    GEN_VISIT(SgCtorInitializerList)
    GEN_VISIT(SgDataStatementGroup)
    GEN_VISIT(SgDataStatementObject)
    GEN_VISIT(SgDataStatementValue)
    GEN_VISIT(SgDeadIfDirectiveStatement)
    GEN_VISIT(SgDeallocateStatement)
    GEN_VISIT(SgDeclarationModifier)
    GEN_VISIT(SgDeclarationScope)
    GEN_VISIT(SgDeclarationStatement)
    GEN_VISIT(SgDeclType)
    GEN_VISIT(SgDefaultOptionStmt)
    GEN_VISIT(SgDefaultSymbol)
    GEN_VISIT(SgDefineDirectiveStatement)
    GEN_VISIT(SgDeleteExp)
    GEN_VISIT(SgDerivedTypeStatement)
    GEN_VISIT(SgDesignatedInitializer)
    GEN_VISIT(SgDictionaryComprehension)
    GEN_VISIT(SgDictionaryExp)
    GEN_VISIT(SgDimensionObject)
    GEN_VISIT(SgDirectory)
    GEN_VISIT(SgDirectoryList)
    GEN_VISIT(SgDivAssignOp)
    GEN_VISIT(SgDivideOp)
    GEN_VISIT(SgDoWhileStmt)
    GEN_VISIT(SgDotExp)
    GEN_VISIT(SgDotStarOp)
    GEN_VISIT(SgDoubleVal)
    GEN_VISIT(SgElaboratedTypeModifier)
    GEN_VISIT(SgElementwiseOp)
    GEN_VISIT(SgElementwiseAddOp)
    GEN_VISIT(SgElementwiseDivideOp)
    GEN_VISIT(SgElementwiseLeftDivideOp)
    GEN_VISIT(SgElementwiseMultiplyOp)
    GEN_VISIT(SgElementwisePowerOp)
    GEN_VISIT(SgElementwiseSubtractOp)
    GEN_VISIT(SgElseDirectiveStatement)
    GEN_VISIT(SgElseWhereStatement)
    GEN_VISIT(SgElseifDirectiveStatement)
    GEN_VISIT(SgEmptyDeclaration)
    GEN_VISIT(SgEmptyDirectiveStatement)
    GEN_VISIT(SgEndfileStatement)
    GEN_VISIT(SgEndifDirectiveStatement)
    GEN_VISIT(SgEntryStatement)
    GEN_VISIT(SgEnumDeclaration)
    GEN_VISIT(SgEnumFieldSymbol)
    GEN_VISIT(SgEnumSymbol)
    GEN_VISIT(SgEnumType)
    GEN_VISIT(SgEnumVal)
    GEN_VISIT(SgEqualityOp)
    GEN_VISIT(SgEquivalenceStatement)
    GEN_VISIT(SgErrorDirectiveStatement)
    GEN_VISIT(SgExecStatement)
    GEN_VISIT(SgExponentiationOp)
    GEN_VISIT(SgExponentiationAssignOp)
    GEN_VISIT(SgExprListExp)
    GEN_VISIT(SgExprStatement)
    GEN_VISIT(SgExpression)
    GEN_VISIT(SgExpressionRoot)
    GEN_VISIT(SgFile)
    GEN_VISIT(SgFileList)
    GEN_VISIT(SgFloatVal)
    GEN_VISIT(SgFloat128Val)
    GEN_VISIT(SgFloat80Val)
    GEN_VISIT(SgFoldExpression)
    GEN_VISIT(SgFlushStatement)
    GEN_VISIT(SgForAllStatement)
    GEN_VISIT(SgForInitStatement)
    GEN_VISIT(SgForStatement)
    GEN_VISIT(SgFormatItem)
    GEN_VISIT(SgFormatItemList)
    GEN_VISIT(SgFormatStatement)
    GEN_VISIT(SgFortranDo)
    GEN_VISIT(SgFortranIncludeLine)
    GEN_VISIT(SgFortranNonblockedDo)
    GEN_VISIT(SgFuncDecl_attr)
    GEN_VISIT(SgFunctionCallExp)
    GEN_VISIT(SgFunctionDeclaration)
    GEN_VISIT(SgFunctionDefinition)
    GEN_VISIT(SgFunctionParameterScope)
    GEN_VISIT(SgFunctionModifier)
    GEN_VISIT(SgFunctionParameterList)
    GEN_VISIT(SgFunctionParameterRefExp)
    GEN_VISIT(SgFunctionParameterTypeList)
    GEN_VISIT(SgFunctionRefExp)
    GEN_VISIT(SgFunctionSymbol)
    GEN_VISIT(SgFunctionType)
    GEN_VISIT(SgFunctionTypeSymbol)
    GEN_VISIT(SgFunctionTypeTable)
    GEN_VISIT(SgTypeTable)
    GEN_VISIT(SgGlobal)
    GEN_VISIT(SgGotoStatement)
    GEN_VISIT(SgGraph)
    GEN_VISIT(SgGraphEdge)
    GEN_VISIT(SgGraphEdgeList)
    GEN_VISIT(SgGraphNode)
    GEN_VISIT(SgGraphNodeList)
    GEN_VISIT(SgGreaterOrEqualOp)
    GEN_VISIT(SgGreaterThanOp)
    GEN_VISIT(SgIOItemExpression)
    GEN_VISIT(SgIOStatement)
    GEN_VISIT(SgIdentDirectiveStatement)
    GEN_VISIT(SgIfDirectiveStatement)
    GEN_VISIT(SgIfStmt)
    GEN_VISIT(SgIfdefDirectiveStatement)
    GEN_VISIT(SgIfndefDirectiveStatement)
    GEN_VISIT(SgImageControlStatement)
    GEN_VISIT(SgImagPartOp)
    GEN_VISIT(SgImplicitStatement)
    GEN_VISIT(SgImpliedDo)
    GEN_VISIT(SgImportStatement)
    GEN_VISIT(SgIncidenceDirectedGraph)
    GEN_VISIT(SgIncidenceUndirectedGraph)
    GEN_VISIT(SgIncludeDirectiveStatement)
    GEN_VISIT(SgIncludeFile)
    GEN_VISIT(SgIncludeNextDirectiveStatement)
    GEN_VISIT(SgInitializedName)
    GEN_VISIT(SgInitializer)
    GEN_VISIT(SgInquireStatement)
    GEN_VISIT(SgIntKeyedBidirectionalGraph)
    GEN_VISIT(SgIntVal)
    GEN_VISIT(SgIntegerDivideOp)
    GEN_VISIT(SgIntegerDivideAssignOp)
    GEN_VISIT(SgInterfaceBody)
    GEN_VISIT(SgHeaderFileBody)
    GEN_VISIT(SgHeaderFileReport)
    GEN_VISIT(SgInterfaceStatement)
    GEN_VISIT(SgInterfaceSymbol)
    GEN_VISIT(SgIntrinsicSymbol)
    GEN_VISIT(SgIsOp)
    GEN_VISIT(SgIsNotOp)
    GEN_VISIT(SgIorAssignOp)
    GEN_VISIT(SgJovialBitType)
    GEN_VISIT(SgJovialBitVal)
    GEN_VISIT(SgJovialTableType)
    GEN_VISIT(SgJovialCompoolStatement)
    GEN_VISIT(SgJovialForThenStatement)
    GEN_VISIT(SgJovialDefineDeclaration)
    GEN_VISIT(SgJovialDirectiveStatement)
    GEN_VISIT(SgJovialOverlayDeclaration)
    GEN_VISIT(SgJovialTablePresetExp)
    GEN_VISIT(SgJovialTableStatement)
    GEN_VISIT(SgKeyDatumPair)
    GEN_VISIT(SgCudaKernelExecConfig)
    GEN_VISIT(SgCudaKernelCallExp)
    GEN_VISIT(SgLabelRefExp)
    GEN_VISIT(SgLabelStatement)
    GEN_VISIT(SgJavaLabelStatement)
    GEN_VISIT(SgLabelSymbol)
    GEN_VISIT(SgJavaLabelSymbol)
    GEN_VISIT(SgLambdaCapture)
    GEN_VISIT(SgLambdaCaptureList)
    GEN_VISIT(SgLambdaExp)
    GEN_VISIT(SgLambdaRefExp)
    GEN_VISIT(SgLeftDivideOp)
    GEN_VISIT(SgLessOrEqualOp)
    GEN_VISIT(SgLessThanOp)
    GEN_VISIT(SgLineDirectiveStatement)
    GEN_VISIT(SgLinemarkerDirectiveStatement)
    GEN_VISIT(SgLinkageModifier)
    GEN_VISIT(SgListComprehension)
    GEN_VISIT(SgListExp)
    GEN_VISIT(SgLocatedNode)
    GEN_VISIT(SgLocatedNodeSupport)
    GEN_VISIT(SgLongDoubleVal)
    GEN_VISIT(SgLongIntVal)
    GEN_VISIT(SgLongLongIntVal)
    GEN_VISIT(SgLshiftAssignOp)
    GEN_VISIT(SgLshiftOp)
    GEN_VISIT(SgMagicColonExp)
    GEN_VISIT(SgMatrixExp)
    GEN_VISIT(SgMatrixTransposeOp)
    GEN_VISIT(SgMatlabForStatement)
    GEN_VISIT(SgMemberFunctionDeclaration)
    GEN_VISIT(SgMemberFunctionRefExp)
    GEN_VISIT(SgMemberFunctionSymbol)
    GEN_VISIT(SgMemberFunctionType)
    GEN_VISIT(SgMembershipOp)
    GEN_VISIT(SgMicrosoftAttributeDeclaration)
    GEN_VISIT(SgMinusAssignOp)
    GEN_VISIT(SgMinusMinusOp)
    GEN_VISIT(SgMinusOp)
    GEN_VISIT(SgModAssignOp)
    GEN_VISIT(SgModOp)
    GEN_VISIT(SgModifier)
    GEN_VISIT(SgModifierNodes)
    GEN_VISIT(SgModifierType)
    GEN_VISIT(SgModuleStatement)
    GEN_VISIT(SgModuleSymbol)
    GEN_VISIT(SgMultAssignOp)
    GEN_VISIT(SgMultiplyOp)
    GEN_VISIT(SgName)
    GEN_VISIT(SgNameGroup)
    GEN_VISIT(SgNamedType)
    GEN_VISIT(SgNamelistStatement)
    GEN_VISIT(SgNamespaceAliasDeclarationStatement)
    GEN_VISIT(SgNamespaceDeclarationStatement)
    GEN_VISIT(SgNamespaceDefinitionStatement)
    GEN_VISIT(SgNamespaceSymbol)
    GEN_VISIT(SgNaryOp)
    GEN_VISIT(SgNaryBooleanOp)
    GEN_VISIT(SgNaryComparisonOp)
    GEN_VISIT(SgNewExp)
    GEN_VISIT(SgNode)
    GEN_VISIT(SgNoexceptOp)
    GEN_VISIT(SgNotEqualOp)
    GEN_VISIT(SgNotOp)
    GEN_VISIT(SgNonMembershipOp)
    GEN_VISIT(SgNonrealDecl)
    GEN_VISIT(SgNonrealRefExp)
    GEN_VISIT(SgNonrealSymbol)
    GEN_VISIT(SgNonrealType)
    GEN_VISIT(SgNonrealBaseClass)
    GEN_VISIT(SgNullExpression)
    GEN_VISIT(SgNullptrValExp)
    GEN_VISIT(SgNullStatement)
    GEN_VISIT(SgNullifyStatement)
    GEN_VISIT(SgOmpAtomicStatement)
    GEN_VISIT(SgOmpBarrierStatement)
    GEN_VISIT(SgOmpCriticalStatement)
    GEN_VISIT(SgOmpClauseBodyStatement)
    GEN_VISIT(SgOmpBodyStatement)
    GEN_VISIT(SgOmpDoStatement)
    GEN_VISIT(SgOmpFlushStatement)
    GEN_VISIT(SgOmpDeclareSimdStatement)
    GEN_VISIT(SgOmpForStatement)
    GEN_VISIT(SgOmpForSimdStatement)
    GEN_VISIT(SgOmpMasterStatement)
    GEN_VISIT(SgOmpOrderedStatement)
    GEN_VISIT(SgOmpParallelStatement)
    GEN_VISIT(SgOmpSectionStatement)
    GEN_VISIT(SgOmpSectionsStatement)
    GEN_VISIT(SgOmpSingleStatement)
    GEN_VISIT(SgOmpTaskStatement)
    GEN_VISIT(SgOmpTaskwaitStatement)
    GEN_VISIT(SgOmpThreadprivateStatement)
    GEN_VISIT(SgOmpWorkshareStatement)
    GEN_VISIT(SgOmpTargetStatement)
    GEN_VISIT(SgOmpTargetDataStatement)
    GEN_VISIT(SgOmpSimdStatement)
    GEN_VISIT(SgOmpClause)
    GEN_VISIT(SgOmpBeginClause)
    GEN_VISIT(SgOmpCollapseClause)
    GEN_VISIT(SgOmpCopyinClause)
    GEN_VISIT(SgOmpCopyprivateClause)
    GEN_VISIT(SgOmpDefaultClause)
    GEN_VISIT(SgOmpEndClause)
    GEN_VISIT(SgOmpExpressionClause)
    GEN_VISIT(SgOmpFirstprivateClause)
    GEN_VISIT(SgOmpIfClause)
    GEN_VISIT(SgOmpFinalClause)
    GEN_VISIT(SgOmpPriorityClause)
    GEN_VISIT(SgOmpDeviceClause)
    GEN_VISIT(SgOmpLastprivateClause)
    GEN_VISIT(SgOmpNowaitClause)
    GEN_VISIT(SgOmpNumThreadsClause)
    GEN_VISIT(SgOmpOrderedClause)
    GEN_VISIT(SgOmpPrivateClause)
    GEN_VISIT(SgOmpReductionClause)
    GEN_VISIT(SgOmpScheduleClause)
    GEN_VISIT(SgOmpSharedClause)
    GEN_VISIT(SgOmpUntiedClause)
    GEN_VISIT(SgOmpMergeableClause)
    GEN_VISIT(SgOmpVariablesClause)
    GEN_VISIT(SgOmpMapClause)
    GEN_VISIT(SgOmpSafelenClause)
    GEN_VISIT(SgOmpSimdlenClause)
    GEN_VISIT(SgOmpLinearClause)
    GEN_VISIT(SgOmpUniformClause)
    GEN_VISIT(SgOmpAlignedClause)
    GEN_VISIT(SgOmpProcBindClause)
    GEN_VISIT(SgOmpAtomicClause)
    GEN_VISIT(SgOmpInbranchClause)
    GEN_VISIT(SgOmpNotinbranchClause)
    GEN_VISIT(SgOmpDependClause)
    GEN_VISIT(SgOpenclAccessModeModifier)
    GEN_VISIT(SgOpenStatement)
    GEN_VISIT(SgOptions)
    GEN_VISIT(SgOrOp)
    GEN_VISIT(SgParameterStatement)
    GEN_VISIT(SgPartialFunctionModifierType)
    GEN_VISIT(SgPartialFunctionType)
    GEN_VISIT(SgPassStatement)
    GEN_VISIT(SgPlusAssignOp)
    GEN_VISIT(SgPlusPlusOp)
    GEN_VISIT(SgPntrArrRefExp)
    GEN_VISIT(SgPointerAssignOp)
    GEN_VISIT(SgPointerDerefExp)
    GEN_VISIT(SgPointerMemberType)
    GEN_VISIT(SgPointerType)
    GEN_VISIT(SgPowerOp)
    GEN_VISIT(SgPragma)
    GEN_VISIT(SgPragmaDeclaration)
    GEN_VISIT(SgPrintStatement)
    GEN_VISIT(SgProcedureHeaderStatement)
    GEN_VISIT(SgProgramHeaderStatement)
    GEN_VISIT(SgProject)
    GEN_VISIT(SgPseudoDestructorRefExp)
    GEN_VISIT(SgPythonGlobalStmt)
    GEN_VISIT(SgPythonPrintStmt)
    GEN_VISIT(SgQualifiedName)
    GEN_VISIT(SgQualifiedNameType)
    GEN_VISIT(SgRangeExp)
    GEN_VISIT(SgRangeBasedForStatement)
    GEN_VISIT(SgReadStatement)
    GEN_VISIT(SgRealPartOp)
    GEN_VISIT(SgRefExp)
    GEN_VISIT(SgReferenceType)
    GEN_VISIT(SgRemOp)
    GEN_VISIT(SgRenamePair)
    GEN_VISIT(SgRenameSymbol)
    GEN_VISIT(SgReplicationOp)
    GEN_VISIT(SgReturnStmt)
    GEN_VISIT(SgRewindStatement)
    GEN_VISIT(SgRshiftAssignOp)
    GEN_VISIT(SgRshiftOp)
    GEN_VISIT(SgRvalueReferenceType)
    GEN_VISIT(SgJavaUnsignedRshiftAssignOp)
    GEN_VISIT(SgJavaUnsignedRshiftOp)
    GEN_VISIT(SgScopeOp)
    GEN_VISIT(SgScopeStatement)
    GEN_VISIT(SgSequenceStatement)
    GEN_VISIT(SgSetComprehension)
    GEN_VISIT(SgShortVal)
    GEN_VISIT(SgSizeOfOp)
    GEN_VISIT(SgAlignOfOp)
    GEN_VISIT(SgJavaInstanceOfOp)
    GEN_VISIT(SgSourceFile)
    GEN_VISIT(SgSpaceshipOp)
    GEN_VISIT(SgSpawnStmt)
    GEN_VISIT(SgSyncAllStatement)
    GEN_VISIT(SgSyncImagesStatement)
    GEN_VISIT(SgSyncMemoryStatement)
    GEN_VISIT(SgSyncTeamStatement)
    GEN_VISIT(SgLockStatement)
    GEN_VISIT(SgUnlockStatement)
    GEN_VISIT(SgJavaThrowStatement)
    GEN_VISIT(SgJavaForEachStatement)
    GEN_VISIT(SgJavaSynchronizedStatement)
    GEN_VISIT(SgJavaParameterizedType)
    GEN_VISIT(SgJavaWildcardType)
    GEN_VISIT(SgProcessControlStatement)
    GEN_VISIT(SgSpecialFunctionModifier)
    GEN_VISIT(SgStatement)
    GEN_VISIT(SgStaticAssertionDeclaration)
    GEN_VISIT(SgStmtDeclarationStatement)
    GEN_VISIT(SgStatementExpression)
    GEN_VISIT(SgStatementFunctionStatement)
    GEN_VISIT(SgStorageModifier)
    GEN_VISIT(SgStringConversion)
    GEN_VISIT(SgStringKeyedBidirectionalGraph)
    GEN_VISIT(SgStringVal)
    GEN_VISIT(SgStructureModifier)
    GEN_VISIT(SgSubscriptExpression)
    GEN_VISIT(SgSubtractOp)
    GEN_VISIT(SgSupport)
    GEN_VISIT(SgSwitchStatement)
    GEN_VISIT(SgSymbol)
    GEN_VISIT(SgSymbolTable)
    GEN_VISIT(SgTemplateArgument)
    GEN_VISIT(SgTemplateArgumentList)
    GEN_VISIT(SgTemplateDeclaration)
    GEN_VISIT(SgTemplateClassDeclaration)
    GEN_VISIT(SgTemplateClassSymbol)
    GEN_VISIT(SgTemplateFunctionDeclaration)
    GEN_VISIT(SgTemplateFunctionRefExp)
    GEN_VISIT(SgTemplateFunctionSymbol)
    GEN_VISIT(SgTemplateMemberFunctionDeclaration)
    GEN_VISIT(SgTemplateMemberFunctionRefExp)
    GEN_VISIT(SgTemplateMemberFunctionSymbol)
    GEN_VISIT(SgTemplateTypedefDeclaration)
    GEN_VISIT(SgTemplateTypedefSymbol)
    GEN_VISIT(SgTemplateVariableDeclaration)
    GEN_VISIT(SgTemplateVariableSymbol)
    GEN_VISIT(SgTemplateClassDefinition)
    GEN_VISIT(SgTemplateFunctionDefinition)
    GEN_VISIT(SgTemplateInstantiationDecl)
    GEN_VISIT(SgTemplateInstantiationDefn)
    GEN_VISIT(SgTemplateInstantiationDirectiveStatement)
    GEN_VISIT(SgTemplateInstantiationFunctionDecl)
    GEN_VISIT(SgTemplateInstantiationMemberFunctionDecl)
    GEN_VISIT(SgTemplateInstantiationTypedefDeclaration)
    GEN_VISIT(SgTemplateParameter)
    GEN_VISIT(SgTemplateParameterVal)
    GEN_VISIT(SgTemplateParameterList)
    GEN_VISIT(SgTemplateSymbol)
    GEN_VISIT(SgTemplateType)
    GEN_VISIT(SgThisExp)
    GEN_VISIT(SgTypeTraitBuiltinOperator)
    GEN_VISIT(SgSuperExp)
    GEN_VISIT(SgThrowOp)
    GEN_VISIT(SgToken)
    GEN_VISIT(SgTryStmt)
    GEN_VISIT(SgTupleExp)
    GEN_VISIT(SgType)
    GEN_VISIT(SgTypeBool)
    GEN_VISIT(SgTypeChar)
    GEN_VISIT(SgTypeChar16)
    GEN_VISIT(SgTypeChar32)
    GEN_VISIT(SgTypeComplex)
    GEN_VISIT(SgTypeDefault)
    GEN_VISIT(SgTypeExpression)
    GEN_VISIT(SgTypeLabel)
    GEN_VISIT(SgTypeDouble)
    GEN_VISIT(SgTypeEllipse)
    GEN_VISIT(SgTypeFixed)
    GEN_VISIT(SgTypeFloat)
    GEN_VISIT(SgTypeFloat128)
    GEN_VISIT(SgTypeFloat80)
    GEN_VISIT(SgTypeGlobalVoid)
    GEN_VISIT(SgTypeIdOp)
    GEN_VISIT(SgTypeImaginary)
    GEN_VISIT(SgTypeInt)
    GEN_VISIT(SgTypeLong)
    GEN_VISIT(SgTypeLongDouble)
    GEN_VISIT(SgTypeLongLong)
    GEN_VISIT(SgTypeModifier)
    GEN_VISIT(SgTypeMatrix)
    GEN_VISIT(SgTypeTuple)
    GEN_VISIT(SgTypeNullptr)
    GEN_VISIT(SgTypeOfType)
    GEN_VISIT(SgTypeShort)
    GEN_VISIT(SgTypeSigned128bitInteger)
    GEN_VISIT(SgTypeSignedChar)
    GEN_VISIT(SgTypeSignedInt)
    GEN_VISIT(SgTypeSignedLong)
    GEN_VISIT(SgTypeSignedLongLong)
    GEN_VISIT(SgTypeSignedShort)
    GEN_VISIT(SgTypeString)
    GEN_VISIT(SgTypeUnknown)
    GEN_VISIT(SgTypeUnsigned128bitInteger)
    GEN_VISIT(SgTypeUnsignedChar)
    GEN_VISIT(SgTypeUnsignedInt)
    GEN_VISIT(SgTypeUnsignedLong)
    GEN_VISIT(SgTypeUnsignedLongLong)
    GEN_VISIT(SgTypeUnsignedShort)
    GEN_VISIT(SgTypeVoid)
    GEN_VISIT(SgTypeWchar)
    GEN_VISIT(SgTypedefDeclaration)
    GEN_VISIT(SgTypedefSeq)
    GEN_VISIT(SgTypedefSymbol)
    GEN_VISIT(SgTypedefType)
    GEN_VISIT(SgUPC_AccessModifier)
    GEN_VISIT(SgUnaryAddOp)
    GEN_VISIT(SgUnaryOp)
    GEN_VISIT(SgUndefDirectiveStatement)
    GEN_VISIT(SgUndirectedGraphEdge)
    GEN_VISIT(SgUnknownArrayOrFunctionReference)
    GEN_VISIT(SgUnknownFile)
    GEN_VISIT(SgUnparse_Info)
    GEN_VISIT(SgUnsignedCharVal)
    GEN_VISIT(SgUnsignedIntVal)
    GEN_VISIT(SgUnsignedLongLongIntVal)
    GEN_VISIT(SgUnsignedLongVal)
    GEN_VISIT(SgUnsignedShortVal)
    GEN_VISIT(SgUpcBarrierStatement)
    GEN_VISIT(SgUpcBlocksizeofExpression)
    GEN_VISIT(SgUpcElemsizeofExpression)
    GEN_VISIT(SgUpcFenceStatement)
    GEN_VISIT(SgUpcForAllStatement)
    GEN_VISIT(SgUpcLocalsizeofExpression)
    GEN_VISIT(SgUpcMythread)
    GEN_VISIT(SgUpcNotifyStatement)
    GEN_VISIT(SgUpcThreads)
    GEN_VISIT(SgUpcWaitStatement)
    GEN_VISIT(SgUseStatement)
    GEN_VISIT(SgUserDefinedBinaryOp)
    GEN_VISIT(SgUserDefinedUnaryOp)
    GEN_VISIT(SgUsingDeclarationStatement)
    GEN_VISIT(SgUsingDirectiveStatement)
    GEN_VISIT(SgValueExp)
    GEN_VISIT(SgVarArgCopyOp)
    GEN_VISIT(SgVarArgEndOp)
    GEN_VISIT(SgVarArgOp)
    GEN_VISIT(SgVarArgStartOneOperandOp)
    GEN_VISIT(SgVarArgStartOp)
    GEN_VISIT(SgVarRefExp)
    GEN_VISIT(SgVariableDeclaration)
    GEN_VISIT(SgVariableDefinition)
    GEN_VISIT(SgVariableSymbol)
    GEN_VISIT(SgVariantExpression)
    GEN_VISIT(SgVariantStatement)
    GEN_VISIT(SgVoidVal)
    GEN_VISIT(SgWaitStatement)
    GEN_VISIT(SgWarningDirectiveStatement)
    GEN_VISIT(SgWithStatement)
    GEN_VISIT(SgWcharVal)
    GEN_VISIT(SgWhereStatement)
    GEN_VISIT(SgWhileStmt)
    GEN_VISIT(SgWriteStatement)
    GEN_VISIT(SgXorAssignOp)
    GEN_VISIT(SgYieldExpression)
    GEN_VISIT(Sg_File_Info)
    GEN_VISIT(SgTypeCAFTeam)
    GEN_VISIT(SgCAFWithTeamStatement)
    GEN_VISIT(SgCAFCoExpression)
    GEN_VISIT(SgCallExpression)
    GEN_VISIT(SgTypeCrayPointer)
    GEN_VISIT(SgJavaImportStatement)
    GEN_VISIT(SgJavaPackageDeclaration)
    GEN_VISIT(SgJavaPackageStatement)
    GEN_VISIT(SgJavaImportStatementList)
    GEN_VISIT(SgJavaClassDeclarationList)
    GEN_VISIT(SgJavaMemberValuePair)
    GEN_VISIT(SgJavaAnnotation)
    GEN_VISIT(SgJavaMarkerAnnotation)
    GEN_VISIT(SgJavaSingleMemberAnnotation)
    GEN_VISIT(SgJavaNormalAnnotation)
    GEN_VISIT(SgJavaTypeExpression)
    GEN_VISIT(SgJavaQualifiedType)
    GEN_VISIT(SgClassExp)
    GEN_VISIT(SgJavaUnionType)
    GEN_VISIT(SgJavaParameterType)
    GEN_VISIT(SgAsyncStmt)
    GEN_VISIT(SgFinishStmt)
    GEN_VISIT(SgAtStmt)
    GEN_VISIT(SgAtomicStmt)
//    GEN_VISIT(SgClassPropertyList)
    GEN_VISIT(SgWhenStmt)
    GEN_VISIT(SgAtExp)
    GEN_VISIT(SgFinishExp)
    GEN_VISIT(SgHereExp)
    GEN_VISIT(SgDotDotExp)
    GEN_VISIT(SgAdaOthersExp)
    GEN_VISIT(SgAdaUnitRefExp)
    GEN_VISIT(SgAdaDiscriminatedTypeDecl)
    GEN_VISIT(SgAdaDiscriminatedType)
    GEN_VISIT(SgAdaDiscriminantConstraint)
    GEN_VISIT(SgAdaGenericSymbol)

    GEN_VISIT(SgAdaProtectedBody)
    GEN_VISIT(SgAdaProtectedBodyDecl)
    GEN_VISIT(SgAdaProtectedSpec)
    GEN_VISIT(SgAdaProtectedSpecDecl)
    GEN_VISIT(SgAdaProtectedSymbol)
    GEN_VISIT(SgAdaProtectedRefExp)
    GEN_VISIT(SgAdaProtectedType)
    GEN_VISIT(SgAdaProtectedTypeDecl)
    GEN_VISIT(SgAdaDigitsConstraint)
    GEN_VISIT(SgAdaAncestorInitializer)
    GEN_VISIT(SgAdaDeltaConstraint)
    GEN_VISIT(SgAdaSubroutineType)
    GEN_VISIT(SgAdaGenericInstanceSymbol)
    GEN_VISIT(SgAdaFormalPackageDecl)
    GEN_VISIT(SgAdaFormalPackageSymbol)
    GEN_VISIT(SgAdaNullConstraint)
    GEN_VISIT(SgAdaUnscopedBlock)
    GEN_VISIT(SgAdaVariantDecl)
    GEN_VISIT(SgAdaVariantWhenStmt)
    GEN_VISIT(SgJovialLabelDeclaration)

    GEN_VISIT(SgRangeType)

#if WITH_BINARY_NODES
    GEN_VISIT(SgAsmAarch64AtOperand)
    GEN_VISIT(SgAsmAarch64BarrierOperand)
    GEN_VISIT(SgAsmAarch64CImmediateOperand)
    GEN_VISIT(SgAsmAarch64Instruction)
    GEN_VISIT(SgAsmAarch64PrefetchOperand)
    GEN_VISIT(SgAsmAarch64SysMoveOperand)
    GEN_VISIT(SgAsmBasicString)
    GEN_VISIT(SgAsmBinaryAdd)
    GEN_VISIT(SgAsmBinaryAddPostupdate)
    GEN_VISIT(SgAsmBinaryAddPreupdate)
    GEN_VISIT(SgAsmBinaryAsr)
    GEN_VISIT(SgAsmBinaryDivide)
    GEN_VISIT(SgAsmBinaryExpression)
    GEN_VISIT(SgAsmBinaryLsl)
    GEN_VISIT(SgAsmBinaryLsr)
    GEN_VISIT(SgAsmBinaryMod)
    GEN_VISIT(SgAsmBinaryMsl)
    GEN_VISIT(SgAsmBinaryMultiply)
    GEN_VISIT(SgAsmBinaryRor)
    GEN_VISIT(SgAsmBinarySubtract)
    GEN_VISIT(SgAsmBinarySubtractPostupdate)
    GEN_VISIT(SgAsmBinarySubtractPreupdate)
    GEN_VISIT(SgAsmBlock)
    GEN_VISIT(SgAsmCoffStrtab)
    GEN_VISIT(SgAsmCoffSymbol)
    GEN_VISIT(SgAsmCoffSymbolList)
    GEN_VISIT(SgAsmCoffSymbolTable)
    GEN_VISIT(SgAsmCommonSubExpression)
    GEN_VISIT(SgAsmControlFlagsExpression)
    GEN_VISIT(SgAsmConstantExpression)
    GEN_VISIT(SgAsmDOSExtendedHeader)
    GEN_VISIT(SgAsmDOSFileHeader)
    GEN_VISIT(SgAsmSynthesizedDataStructureDeclaration)
    GEN_VISIT(SgAsmSynthesizedDeclaration)
    GEN_VISIT(SgAsmDirectRegisterExpression)
    GEN_VISIT(SgAsmDwarfAccessDeclaration)
    GEN_VISIT(SgAsmDwarfArrayType)
    GEN_VISIT(SgAsmDwarfBaseType)
    GEN_VISIT(SgAsmDwarfCatchBlock)
    GEN_VISIT(SgAsmDwarfClassTemplate)
    GEN_VISIT(SgAsmDwarfClassType)
    GEN_VISIT(SgAsmDwarfCommonBlock)
    GEN_VISIT(SgAsmDwarfCommonInclusion)
    GEN_VISIT(SgAsmDwarfCompilationUnit)
    GEN_VISIT(SgAsmDwarfCompilationUnitList)
    GEN_VISIT(SgAsmDwarfCondition)
    GEN_VISIT(SgAsmDwarfConstType)
    GEN_VISIT(SgAsmDwarfConstant)
    GEN_VISIT(SgAsmDwarfConstruct)
    GEN_VISIT(SgAsmDwarfConstructList)
    GEN_VISIT(SgAsmDwarfDwarfProcedure)
    GEN_VISIT(SgAsmDwarfEntryPoint)
    GEN_VISIT(SgAsmDwarfEnumerationType)
    GEN_VISIT(SgAsmDwarfEnumerator)
    GEN_VISIT(SgAsmDwarfFileType)
    GEN_VISIT(SgAsmDwarfFormalParameter)
    GEN_VISIT(SgAsmDwarfFormatLabel)
    GEN_VISIT(SgAsmDwarfFriend)
    GEN_VISIT(SgAsmDwarfFunctionTemplate)
    GEN_VISIT(SgAsmDwarfImportedDeclaration)
    GEN_VISIT(SgAsmDwarfImportedModule)
    GEN_VISIT(SgAsmDwarfImportedUnit)
    GEN_VISIT(SgAsmDwarfInformation)
    GEN_VISIT(SgAsmDwarfInheritance)
    GEN_VISIT(SgAsmDwarfInlinedSubroutine)
    GEN_VISIT(SgAsmDwarfInterfaceType)
    GEN_VISIT(SgAsmDwarfLabel)
    GEN_VISIT(SgAsmDwarfLexicalBlock)
    GEN_VISIT(SgAsmDwarfLine)
    GEN_VISIT(SgAsmDwarfLineList)
    GEN_VISIT(SgAsmDwarfMacro)
    GEN_VISIT(SgAsmDwarfMacroList)
    GEN_VISIT(SgAsmDwarfMember)
    GEN_VISIT(SgAsmDwarfModule)
    GEN_VISIT(SgAsmDwarfMutableType)
    GEN_VISIT(SgAsmDwarfNamelist)
    GEN_VISIT(SgAsmDwarfNamelistItem)
    GEN_VISIT(SgAsmDwarfNamespace)
    GEN_VISIT(SgAsmDwarfPackedType)
    GEN_VISIT(SgAsmDwarfPartialUnit)
    GEN_VISIT(SgAsmDwarfPointerType)
    GEN_VISIT(SgAsmDwarfPtrToMemberType)
    GEN_VISIT(SgAsmDwarfReferenceType)
    GEN_VISIT(SgAsmDwarfRestrictType)
    GEN_VISIT(SgAsmDwarfSetType)
    GEN_VISIT(SgAsmDwarfSharedType)
    GEN_VISIT(SgAsmDwarfStringType)
    GEN_VISIT(SgAsmDwarfStructureType)
    GEN_VISIT(SgAsmDwarfSubprogram)
    GEN_VISIT(SgAsmDwarfSubrangeType)
    GEN_VISIT(SgAsmDwarfSubroutineType)
    GEN_VISIT(SgAsmDwarfTemplateTypeParameter)
    GEN_VISIT(SgAsmDwarfTemplateValueParameter)
    GEN_VISIT(SgAsmDwarfThrownType)
    GEN_VISIT(SgAsmDwarfTryBlock)
    GEN_VISIT(SgAsmDwarfTypedef)
    GEN_VISIT(SgAsmDwarfUnionType)
    GEN_VISIT(SgAsmDwarfUnknownConstruct)
    GEN_VISIT(SgAsmDwarfUnspecifiedParameters)
    GEN_VISIT(SgAsmDwarfUnspecifiedType)
    GEN_VISIT(SgAsmDwarfUpcRelaxedType)
    GEN_VISIT(SgAsmDwarfUpcSharedType)
    GEN_VISIT(SgAsmDwarfUpcStrictType)
    GEN_VISIT(SgAsmDwarfVariable)
    GEN_VISIT(SgAsmDwarfVariant)
    GEN_VISIT(SgAsmDwarfVariantPart)
    GEN_VISIT(SgAsmDwarfVolatileType)
    GEN_VISIT(SgAsmDwarfWithStmt)
    GEN_VISIT(SgAsmElfDynamicEntry)
    GEN_VISIT(SgAsmElfDynamicEntryList)
    GEN_VISIT(SgAsmElfDynamicSection)
    GEN_VISIT(SgAsmElfEHFrameEntryCI)
    GEN_VISIT(SgAsmElfEHFrameEntryCIList)
    GEN_VISIT(SgAsmElfEHFrameEntryFD)
    GEN_VISIT(SgAsmElfEHFrameEntryFDList)
    GEN_VISIT(SgAsmElfEHFrameSection)
    GEN_VISIT(SgAsmElfFileHeader)
    GEN_VISIT(SgAsmElfNoteEntry)
    GEN_VISIT(SgAsmElfNoteEntryList)
    GEN_VISIT(SgAsmElfNoteSection)
    GEN_VISIT(SgAsmElfRelocEntry)
    GEN_VISIT(SgAsmElfRelocEntryList)
    GEN_VISIT(SgAsmElfRelocSection)
    GEN_VISIT(SgAsmElfSection)
    GEN_VISIT(SgAsmElfSectionTable)
    GEN_VISIT(SgAsmElfSectionTableEntry)
    GEN_VISIT(SgAsmElfSegmentTable)
    GEN_VISIT(SgAsmElfSegmentTableEntry)
    GEN_VISIT(SgAsmElfSegmentTableEntryList)
    GEN_VISIT(SgAsmElfStringSection)
    GEN_VISIT(SgAsmElfStrtab)
    GEN_VISIT(SgAsmElfSymbol)
    GEN_VISIT(SgAsmElfSymbolList)
    GEN_VISIT(SgAsmElfSymbolSection)
    GEN_VISIT(SgAsmElfSymverDefinedAux)
    GEN_VISIT(SgAsmElfSymverDefinedAuxList)
    GEN_VISIT(SgAsmElfSymverDefinedEntry)
    GEN_VISIT(SgAsmElfSymverDefinedEntryList)
    GEN_VISIT(SgAsmElfSymverDefinedSection)
    GEN_VISIT(SgAsmElfSymverEntry)
    GEN_VISIT(SgAsmElfSymverEntryList)
    GEN_VISIT(SgAsmElfSymverNeededAux)
    GEN_VISIT(SgAsmElfSymverNeededAuxList)
    GEN_VISIT(SgAsmElfSymverNeededEntry)
    GEN_VISIT(SgAsmElfSymverNeededEntryList)
    GEN_VISIT(SgAsmElfSymverNeededSection)
    GEN_VISIT(SgAsmElfSymverSection)
    GEN_VISIT(SgAsmExecutableFileFormat)
    GEN_VISIT(SgAsmExprListExp)
    GEN_VISIT(SgAsmExpression)
    GEN_VISIT(SgAsmSynthesizedFieldDeclaration)
    GEN_VISIT(SgAsmFloatType)
    GEN_VISIT(SgAsmFloatValueExpression)
    GEN_VISIT(SgAsmFunction)
    GEN_VISIT(SgAsmGenericDLL)
    GEN_VISIT(SgAsmGenericDLLList)
    GEN_VISIT(SgAsmGenericFile)
    GEN_VISIT(SgAsmGenericFileList)
    GEN_VISIT(SgAsmGenericFormat)
    GEN_VISIT(SgAsmGenericHeader)
    GEN_VISIT(SgAsmGenericHeaderList)
    GEN_VISIT(SgAsmGenericSection)
    GEN_VISIT(SgAsmGenericSectionList)
    GEN_VISIT(SgAsmGenericString)
    GEN_VISIT(SgAsmGenericStrtab)
    GEN_VISIT(SgAsmGenericSymbol)
    GEN_VISIT(SgAsmGenericSymbolList)
    GEN_VISIT(SgAsmIndirectRegisterExpression)
    GEN_VISIT(SgAsmInstruction)
    GEN_VISIT(SgAsmIntegerValueExpression)
    GEN_VISIT(SgAsmIntegerType)
    GEN_VISIT(SgAsmInterpretation)
    GEN_VISIT(SgAsmInterpretationList)
    GEN_VISIT(SgAsmLEEntryPoint)
    GEN_VISIT(SgAsmLEEntryTable)
    GEN_VISIT(SgAsmLEFileHeader)
    GEN_VISIT(SgAsmLENameTable)
    GEN_VISIT(SgAsmLEPageTable)
    GEN_VISIT(SgAsmLEPageTableEntry)
    GEN_VISIT(SgAsmLERelocTable)
    GEN_VISIT(SgAsmLESection)
    GEN_VISIT(SgAsmLESectionTable)
    GEN_VISIT(SgAsmLESectionTableEntry)
    GEN_VISIT(SgAsmM68kInstruction)
    GEN_VISIT(SgAsmMemoryReferenceExpression)
    GEN_VISIT(SgAsmMipsInstruction)
    GEN_VISIT(SgAsmNEEntryPoint)
    GEN_VISIT(SgAsmNEEntryTable)
    GEN_VISIT(SgAsmNEFileHeader)
    GEN_VISIT(SgAsmNEModuleTable)
    GEN_VISIT(SgAsmNENameTable)
    GEN_VISIT(SgAsmNERelocEntry)
    GEN_VISIT(SgAsmNERelocTable)
    GEN_VISIT(SgAsmNESection)
    GEN_VISIT(SgAsmNESectionTable)
    GEN_VISIT(SgAsmNESectionTableEntry)
    GEN_VISIT(SgAsmNEStringTable)
    GEN_VISIT(SgAsmNode)
    GEN_VISIT(SgAsmOp)
    GEN_VISIT(SgAsmOperandList)
    GEN_VISIT(SgAsmPEExportDirectory)
    GEN_VISIT(SgAsmPEExportEntry)
    GEN_VISIT(SgAsmPEExportEntryList)
    GEN_VISIT(SgAsmPEExportSection)
    GEN_VISIT(SgAsmPEFileHeader)
    GEN_VISIT(SgAsmPEImportDirectory)
    GEN_VISIT(SgAsmPEImportDirectoryList)
    GEN_VISIT(SgAsmPEImportItem)
    GEN_VISIT(SgAsmPEImportItemList)
    GEN_VISIT(SgAsmPEImportSection)
    GEN_VISIT(SgAsmPERVASizePair)
    GEN_VISIT(SgAsmPERVASizePairList)
    GEN_VISIT(SgAsmPESection)
    GEN_VISIT(SgAsmPESectionTable)
    GEN_VISIT(SgAsmPESectionTableEntry)
    GEN_VISIT(SgAsmPEStringSection)
    GEN_VISIT(SgAsmPointerType)
    GEN_VISIT(SgAsmPowerpcInstruction)
    GEN_VISIT(SgAsmRegisterNames)
    GEN_VISIT(SgAsmRegisterReferenceExpression)
    GEN_VISIT(SgAsmRiscOperation)
    GEN_VISIT(SgAsmScalarType)
    GEN_VISIT(SgAsmStatement)
    GEN_VISIT(SgAsmStaticData)
    GEN_VISIT(SgAsmStmt)
    GEN_VISIT(SgAsmStoredString)
    GEN_VISIT(SgAsmStringStorage)
    GEN_VISIT(SgAsmType)
    GEN_VISIT(SgAsmUnaryExpression)
    GEN_VISIT(SgAsmUnaryMinus)
    GEN_VISIT(SgAsmUnaryPlus)
    GEN_VISIT(SgAsmUnaryRrx)
    GEN_VISIT(SgAsmUnarySignedExtend)
    GEN_VISIT(SgAsmUnaryUnsignedExtend)
    GEN_VISIT(SgAsmUnaryTruncate)
    GEN_VISIT(SgAsmValueExpression)
    GEN_VISIT(SgAsmVectorType)
    GEN_VISIT(SgAsmVoidType)
    GEN_VISIT(SgAsmX86Instruction)
    GEN_VISIT(SgAsmBinaryAddressSymbol)
    GEN_VISIT(SgAsmBinaryDataSymbol)
    GEN_VISIT(SgAsmNullInstruction)

    GEN_VISIT(SgAsmJvmModuleMainClass)
    GEN_VISIT(SgAsmInstructionList)
    GEN_VISIT(SgAsmCilNode)
    GEN_VISIT(SgAsmCilAssembly)
    GEN_VISIT(SgAsmCilAssemblyOS)
    GEN_VISIT(SgAsmCilAssemblyProcessor)
    GEN_VISIT(SgAsmCilAssemblyRef)
    GEN_VISIT(SgAsmCilAssemblyRefOS)
    GEN_VISIT(SgAsmCilAssemblyRefProcessor)
    GEN_VISIT(SgAsmCilClassLayout)
    GEN_VISIT(SgAsmCilConstant)
    GEN_VISIT(SgAsmCilCustomAttribute)
    GEN_VISIT(SgAsmCilDeclSecurity)
    GEN_VISIT(SgAsmCilEvent)
    GEN_VISIT(SgAsmCilEventMap)
    GEN_VISIT(SgAsmCilExportedType)
    GEN_VISIT(SgAsmCilField)
    GEN_VISIT(SgAsmCilFieldLayout)
    GEN_VISIT(SgAsmCilFieldMarshal)
    GEN_VISIT(SgAsmCilFieldRVA)
    GEN_VISIT(SgAsmCilFile)
    GEN_VISIT(SgAsmCilGenericParam)
    GEN_VISIT(SgAsmCilGenericParamConstraint)
    GEN_VISIT(SgAsmCilImplMap)
    GEN_VISIT(SgAsmCilInterfaceImpl)
    GEN_VISIT(SgAsmCilManifestResource)
    GEN_VISIT(SgAsmCilMemberRef)
    GEN_VISIT(SgAsmCilMethodDef)
    GEN_VISIT(SgAsmCilMethodImpl)
    GEN_VISIT(SgAsmCilMethodSemantics)
    GEN_VISIT(SgAsmCilMethodSpec)
    GEN_VISIT(SgAsmCilModule)
    GEN_VISIT(SgAsmCilModuleRef)
    GEN_VISIT(SgAsmCilNestedClass)
    GEN_VISIT(SgAsmCilParam)
    GEN_VISIT(SgAsmCilProperty)
    GEN_VISIT(SgAsmCilPropertyMap)
    GEN_VISIT(SgAsmCilStandAloneSig)
    GEN_VISIT(SgAsmCilTypeDef)
    GEN_VISIT(SgAsmCilTypeRef)
    GEN_VISIT(SgAsmCilTypeSpec)
    GEN_VISIT(SgAdaParameterList)
    GEN_VISIT(SgAsmCilMetadata)
    GEN_VISIT(SgAsmCilMetadataRoot)
    GEN_VISIT(SgAsmCilDataStream)
    GEN_VISIT(SgAsmCilMetadataHeap)
    GEN_VISIT(SgAsmCilUint8Heap)
    GEN_VISIT(SgAsmCilUint32Heap)
    GEN_VISIT(SgAsmCliHeader)

    GEN_VISIT(SgAsmJvmLocalVariableEntry)
    GEN_VISIT(SgAsmJvmLocalVariableTable)
    GEN_VISIT(SgAsmJvmLocalVariableTypeEntry)
    GEN_VISIT(SgAsmJvmLocalVariableTypeTable)
    GEN_VISIT(SgAsmUserInstruction)
    GEN_VISIT(SgAsmJvmMethodParameters)
    GEN_VISIT(SgAsmJvmMethodParametersEntry)
    GEN_VISIT(SgAsmVoidType)
    GEN_VISIT(SgAsmPointerType)

    // incomplete
#endif /* WITH_BINARY_NODES */

/*
    GEN_VISIT(SgBinaryComposite)
    GEN_VISIT(SgComprehensionList)
    *
    GEN_VISIT(SgDirectedGraph)
    GEN_VISIT(SgDirectedGraphEdge)
    GEN_VISIT(SgDirectedGraphNode)

    GEN_VISIT(SgUnknownMemberFunctionType)
*/

    RoseVisitor rv;
  };

#undef GEN_VISIT

#if 0
  constexpr
  auto nullptrHandlerTest(...) -> std::false_type;

  template <class RoseVisitor>
  constexpr
  auto nullptrHandlerTest(RoseVisitor&&) -> decltype(std::declval<RoseVisitor>().handle(nullptr), std::true_type{});

  template <class T>
  struct NullHandler
  {
    template <class RoseVisitor>
    static RoseVisitor handle(RoseVisitor&& rv)
    {
      rv.handle(nullptr);
      return rv;
    }
  };

  template <>
  struct NullHandler<std::false_type>
  {
    template <class RoseVisitor>
    static
    RoseVisitor handle(RoseVisitor&& rv)
    {
      ASSERT_not_null(nullptr);
      return rv;
    }
  };
#endif

  template <class RoseVisitor>
  inline
  typename std::remove_const<typename std::remove_reference<RoseVisitor>::type>::type
  _dispatch(RoseVisitor&& rv, SgNode* n)
  {
    using RoseVisitorNoref = typename std::remove_reference<RoseVisitor>::type;
    using RoseHandler      = typename std::remove_const<RoseVisitorNoref>::type;

    ASSERT_not_null(n);
    //~ if (n == nullptr)
      //~ return NullHandler<decltype(nullptrHandlerTest(std::declval<RoseVisitor>()))>
                 //~ ::handle(std::forward<RoseVisitor>(rv));

    VisitDispatcher<RoseHandler> vis( std::forward<RoseVisitor>(rv),
                                      std::is_lvalue_reference<RoseVisitor>()
                                    );

    n->accept(vis);
    return std::move(vis).rv;
  }


/// \brief    uncovers the type of SgNode and passes it to an
///           function "handle" in RoseVisitor. which should be
///           overloaded with every possible target node.  After the
///           traversal, RoseVisitor should contain the intended return data.
/// \tparam   RoseVisitor. The visitor that will be called back with
///           the recovered type information. It must implement
///           "handle." The handle function with the most suitable SgNode type will get invoked.
/// \param rv an instance of a rose visitor; ie any class with a
///           "handle" function.  Note: rv is essentially
///           passed by value (similar to STL's for_each).
/// \param n  The root of the tree to visit. @ref SgNode
/// \return   a copy of the RoseVisitor object, that will contain the
///           intended return data.
/// \details  The following code has two classes.
///           - Counter counts the number of all expression and statement nodes.
///             It implements handlers for @ref SgNode (not interesting nodes),
///             for @ref SgExpression and @ref SgStatement (to count the nodes).
///           - Traversal inherits from @ref ASTTraversal and contains a counter.
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
#if 0
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
#endif

  template <class RoseVisitor>
  inline
  typename std::remove_const<typename std::remove_reference<RoseVisitor>::type>::type
  dispatch(RoseVisitor&& rv, SgNode* n)
  {
    //~ return std::move(rv);
    return _dispatch(std::forward<RoseVisitor>(rv), n);
  }

  template <class RoseVisitor>
  inline
  typename std::remove_const<typename std::remove_reference<RoseVisitor>::type>::type
  dispatch(RoseVisitor&& rv, const SgNode* n)
  {
    //~ return std::move(rv);
    return _dispatch(std::forward<RoseVisitor>(rv), const_cast<SgNode*>(n));
  }

/**
 * struct DefaultHandler
 *
 * Base class for @ref AncestorTypeFinder.  It was probably intended
 * to be a generic BaseClass for many RoseVisitor types, but it isn't
 * used by anything else, and isn't actually necessary.
 * If any specific type is not handled by its
 * derived class, DefaultHandler provides the function to ignore it.
 **/
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
    : Base(), res(nullptr, nullptr)
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
    using AncestorFinder = AncestorTypeFinder<AncestorNode, QualSgNode>;

    typename AncestorFinder::Pair res(nullptr, n.get_parent());

    while (res.second != nullptr)
    {
      res = (typename AncestorFinder::Pair) sg::dispatch(AncestorFinder(), res.second);
    }

    return res.first;
  }

/// \brief   finds an ancestor node with a given type
/// \details the function family comes in four variants:
///          - SgNode*       -> AncestorNode*       ( result can be nullptr )
///          - const SgNode* -> const AncestorNode* ( result can be nullptr )
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
    if (n == nullptr) return nullptr;

    return _ancestor<AncestorNode>(*n);
  }

/// \overload
  template <class AncestorNode>
  const AncestorNode* ancestor(const SgNode* n)
  {
    if (n == nullptr) return nullptr;

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


  namespace
  {
/// \private
    template <class SageNode>
    struct TypeRecoveryHandler
    {
        typedef typename ConstLike<SageNode, SgNode>::type SgBaseNode;

        TypeRecoveryHandler(const char* f = 0, size_t ln = 0)
        : res(nullptr), loc(f), loc_ln(ln)
        {}

        TypeRecoveryHandler(TypeRecoveryHandler&&)            = default;
        TypeRecoveryHandler& operator=(TypeRecoveryHandler&&) = default;

        operator SageNode* ()&& { return res; }

        void handle(SgBaseNode& n) { unexpected_node(n, loc, loc_ln); }
        void handle(SageNode& n)   { res = &n; }

      private:
        SageNode*   res;
        const char* loc;
        size_t      loc_ln;

        TypeRecoveryHandler()                                      = delete;
        TypeRecoveryHandler(const TypeRecoveryHandler&)            = delete;
        TypeRecoveryHandler& operator=(const TypeRecoveryHandler&) = delete;
    };
  }


/// \brief   asserts that n has type SageNode
/// \details the ROSE assert in the following example holds b/c assert_sage_type
///          aborts if the input node is not a SgStatement
/// \code
///   SgStatement* stmt = assert_sage_type<SgStatement>(expr.get_parent());
///   ROSE_ASSERT(stmt);
/// \endcode
/// @{
  template <class SageNode>
  SageNode* assert_sage_type(SgNode* n, const char* f = 0, size_t ln = 0)
  {
    return sg::dispatch(TypeRecoveryHandler<SageNode>(f, ln), n);
  }

  template <class SageNode>
  const SageNode* assert_sage_type(const SgNode* n, const char* f = 0, size_t ln = 0)
  {
    return sg::dispatch(TypeRecoveryHandler<const SageNode>(f, ln), n);
  }

  template <class SageNode>
  SageNode& assert_sage_type(SgNode& n, const char* f = 0, size_t ln = 0)
  {
    return *sg::dispatch(TypeRecoveryHandler<SageNode>(f, ln), &n);
  }

  template <class SageNode>
  const SageNode& assert_sage_type(const SgNode& n, const char* f = 0, size_t ln = 0)
  {
    return *sg::dispatch(TypeRecoveryHandler<const SageNode>(f, ln), &n);
  }
/// @}

  template <class SageNode>
  struct TypeRecovery : DispatchHandler<SageNode*>
  {
    void handle(SgNode&)     { /* res = nullptr; */ }
    void handle(SageNode& n) { this->res = &n; }
  };

  /// \deprecated
  template <class SageNode>
  auto ancestor_path(const SgNode& n) -> SageNode*
  {
    return sg::dispatch(TypeRecovery<SageNode>{}, n.get_parent());
  }

  /// \deprecated
  template <class SageNode, class... SageNodes>
  auto ancestor_path(const SgNode& n) -> decltype(ancestor_path<SageNodes...>(n))
  {
    if (SageNode* parent = ancestor_path<SageNode>(n))
      return ancestor_path<SageNodes...>(*parent);

    return nullptr;
  }


  /// returns the last parent in an ancestor path
  /// \{

  // base case
  template <class SageNode>
  auto ancestorPath(const SgNode& n) -> SageNode*
  {
    return sg::dispatch(TypeRecovery<SageNode>{}, n.get_parent());
  }

  // general case
  template <class SageNode, class... SageNodes>
  auto ancestorPath(const SgNode& n) -> decltype(ancestorPath<SageNodes...>(n))
  {
    if (SageNode* parent = ancestorPath<SageNode>(n))
      return ancestorPath<SageNodes...>(*parent);

    return nullptr;
  }
  /// \}


/// swaps the parent pointer of two nodes
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

/// swaps children (of equal kind) between two ancestor nodes of the same type
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

  /// prevents the dispatch handler being called on nullptr.
  /// \{
  template <class GVisitor>
  struct DispatchHelper
  {
    explicit
    DispatchHelper(GVisitor gv)
    : gvisitor(std::move(gv))
    {}

    void operator()(SgNode* n)
    {
      if (n != nullptr) gvisitor = sg::dispatch(std::move(gvisitor), n);
    }

    operator GVisitor()&& { return std::move(gvisitor); }

    GVisitor gvisitor;
  };


  template <class GVisitor>
  static inline
  DispatchHelper<GVisitor>
  dispatchHelper(GVisitor gv)
  {
    return DispatchHelper<GVisitor>(std::move(gv));
  }

  /// \}

  /// \private
  struct DefaultTraversalSuccessors
  {
    SgNodePtrList operator()(SgNode& n) const
    {
      return n.get_traversalSuccessorContainer();
    }
  };

  /// Dispatches all children of \p n to the generic vistor \p gv. The children
  ///   are computed using a customizable generator. By default n.get_traversalSuccessorContainer()
  ///   is invoked.
  /// \{
  template <class GVisitor, class SuccessorGenerator = DefaultTraversalSuccessors>
  static inline
  GVisitor traverseChildren( GVisitor gv, SgNode& n, SuccessorGenerator gen = {} )
  {
    SgNodePtrList const successors = gen(n);

    return std::for_each( successors.begin(), successors.end(),
                          dispatchHelper(std::move(gv))
                        );
  }

  template <class GVisitor>
  static inline
  GVisitor traverseChildren(GVisitor gv, SgNode* n)
  {
    return traverseChildren(std::move(gv), sg::deref(n));
  }
  /// \}


  /// Links parent node \p parent to child node \p child using the setter method \p setter.
  /// Also sets the parent link from \p child to \p parent.
  template <class SageParent, class SageChild>
  void linkParentChild(SageParent& parent, SageChild& child, void (SageParent::*setter)(SageChild*))
  {
    (parent.*setter)(&child);
    child.set_parent(&parent);
  }

  /// returns the same node \p n upcasted to its base type
  /// \note useful for calling an overloaded function
  /// \{
  template <class SageNode>
  typename SageNode::base_node_type&
  asBaseType(SageNode& n) { return n; }

  template <class SageNode>
  const typename SageNode::base_node_type&
  asBaseType(const SageNode& n) { return n; }

  template <class SageNode>
  typename SageNode::base_node_type*
  asBaseType(SageNode* n) { return n; }

  template <class SageNode>
  const typename SageNode::base_node_type*
  asBaseType(const SageNode* n) { return n; }
  /// \}
}
#endif /* _SAGEGENERIC_H */


#if OBSOLETE_CODE


/// \brief executes a functor for a specific node type
/// \details internal use
  template <class SageNode>
  struct TraversalFunction
  {
    typedef void (*TransformHandlerFn)(SageNode*);

    explicit
    TraversalFunction(TransformHandlerFn fun)
    : fn(fun)
    {}

    void handle(SgNode&)     { /* ignore */ }
    void handle(SageNode& n) { fn(&n); }

    TransformHandlerFn fn;
  };

/// \brief creates a functor that executes code for a specific node type
/// \details internal use
  template <class SageNode>
  static inline
  TraversalFunction<SageNode>
  createTraversalFunction(void (* fn)(SageNode*))
  {
    return TraversalFunction<SageNode>(fn);
  }

  //
  // function type extractor
  //   see https://stackoverflow.com/questions/28033251/can-you-extract-types-from-template-parameter-function-signature


  template <class GVisitor>
  struct TraversalClass : AstSimpleProcessing
  {
    explicit
    TraversalClass(GVisitor gv)
    : gvisitor(gv)
    //~ : gvisitor(std::move(gv))
    {}

    void visit(SgNode* n)
    {
      gvisitor = sg::dispatch(gvisitor, n);
    }

    // GVisitor&& visitor() { return std::move(gvisitor); }
    GVisitor visitor() { return gvisitor; }

    GVisitor gvisitor;
  };



  /// \brief calls fn with all applicable nodes in the AST
  /// \details
  ///   extracts the variant type from the function argument in fn
  ///   and collects all nodes using querySubTree.
  ///   After that, iterates through the result list and executes
  ///   fn all all identified nodes.
  template <class F>
  static inline
  F
  forAllNodes(F fn, SgNode* root, AstSimpleProcessing::Order order = postorder)
  {
    ROSE_ASSERT(root);

    TraversalClass<F> tt(fn);
    //~ TraversalClass<F> tt(std::move(fn));

    tt.traverse(root, order);
    return tt.visitor();
  }

  template <class SageNode>
  static inline
  void
  forAllNodes(void (*fn)(SageNode*), SgNode* root, AstSimpleProcessing::Order order = postorder)
  {
    forAllNodes(createTraversalFunction(fn), root, order);
  }

#if !defined(NDEBUG)
  static inline
  std::string nodeType(const SgNode& n)
  {
    return typeid(n).name();
  }

  static inline
  std::string nodeType(const SgNode* n)
  {
    if (n == nullptr) return "<null>";

    return nodeType(*n);
  }
#endif /* !NDEBUG */

#endif /* OBSOLETE_CODE */
