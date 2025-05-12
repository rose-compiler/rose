#include <sage3basic.h>
#include "RoseCompatibility.h"

#include <sageInterface.h>
#include <sageInterfaceAda.h>
#include <sageGeneric.h>
#include <nodeQuery.h>

#include <tuple>
#include <numeric>
#include <unordered_set>

#include "CodeThornLib.h"

#include "ClassHierarchyAnalysis.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext/for_each.hpp>
// #include <boost/range/join.hpp>

namespace si = SageInterface;
namespace ct = CodeThorn;
namespace adapt = boost::adaptors;

namespace
{
  static constexpr bool firstDecisiveComparison = false; // syntactic sugar tag

  // WARNING - THIS CLASS IS NOT REENTRANT IN NESTED LOOPS, AS THE
  //   SgScopeStatement HOLDS STATE.
  struct ConstSymbolTableIterator
  {
      using iterator_category = std::forward_iterator_tag;
      using value_type = const SgSymbol&;
      using difference_type = std::ptrdiff_t;
      using pointer = const SgSymbol*;
      using reference = value_type;

      ConstSymbolTableIterator(const SgScopeStatement& s, const SgSymbol* el)
      : scope(&s), current(el)
      {}

      reference operator*()  const { return *current; }
      pointer   operator->() const { return current; }

      ConstSymbolTableIterator& operator++()
      {
        current = scope->next_any_symbol();
        return *this;
      }

      ConstSymbolTableIterator operator++(int)
      {
        ConstSymbolTableIterator tmp{*this};
        ++(*this);
        return tmp;
      }

      // Equality operator
      bool operator==(const ConstSymbolTableIterator& that) const
      {
        return this->current == that.current;
      }

      bool operator!=(const ConstSymbolTableIterator& that) const
      {
        return this->current != that.current;
      }

    private:
      sg::NotNull<const SgScopeStatement> scope;
      const SgSymbol*                     current;
  };

  ConstSymbolTableIterator
  symbolBegin(const SgScopeStatement& scope)
  {
    return ConstSymbolTableIterator{scope, scope.first_any_symbol()};
  }

  ConstSymbolTableIterator
  symbolLimit(const SgScopeStatement& scope)
  {
    return ConstSymbolTableIterator{scope, nullptr};
  }


  using SymbolRange = boost::iterator_range<ConstSymbolTableIterator>;

  std::string typeNameOf(const SgClassDeclaration* dcl)
  {
    return SG_DEREF(dcl).get_name();
  }

  std::string typeNameOf(const SgClassDefinition& def)
  {
    return typeNameOf(def.get_declaration());
  }

  struct ExcludeTemplates
  {
    void handle(SgTemplateClassDeclaration&)          {}
    void handle(SgTemplateClassDefinition&)           {}
    void handle(SgTemplateFunctionDeclaration&)       {}
    void handle(SgTemplateFunctionDefinition&)        {}
    void handle(SgTemplateMemberFunctionDeclaration&) {}
    void handle(SgTemplateVariableDeclaration&)       {}

    static bool templated(const SgNode* n);
    //~ void handle(SgTemplateTypedefDeclaration&)        {} // may need some handling
  };

  bool ExcludeTemplates::templated(const SgNode* n)
  {
    return (  isSgTemplateClassDeclaration(n)
           || isSgTemplateClassDefinition(n)
           || isSgTemplateFunctionDeclaration(n)
           || isSgTemplateFunctionDefinition(n)
           || isSgTemplateMemberFunctionDeclaration(n)
           || isSgTemplateVariableDeclaration(n)
           );
  }

  bool isVirtual(const SgFunctionDeclaration& dcl)
  {
    // \note * ROSE has isVirtual even when it is not
    //         explicit in the source code.
    //       * virtual may not be set on the function definition.
    return dcl.get_functionModifier().isVirtual();
  }

  bool isPureVirtual(const SgFunctionDeclaration& dcl)
  {
    return dcl.get_functionModifier().isPureVirtual();
  }

  bool isPureVirtual(ct::FunctionKeyType fn)
  {
    return isPureVirtual(SG_DEREF(fn));
  }

  bool hasDefinition(const SgFunctionDeclaration& dcl)
  {
    return dcl.get_definingDeclaration() != nullptr;
  }

  bool hasDefinition(ct::FunctionKeyType fn)
  {
    return hasDefinition(SG_DEREF(fn));
  }

  std::string functionName(ct::FunctionKeyType fn)
  {
    return SG_DEREF(fn).get_name();
  }

  const SgFunctionType& functionType(ct::FunctionKeyType fn)
  {
    return SG_DEREF(SG_DEREF(fn).get_type());
  }

  bool isSecondaryClassDefinition(const SgClassDefinition& n)
  {
    SgClassDeclaration&     decl   = SG_DEREF(n.get_declaration());
    SgDeclarationStatement* defdcl = decl.get_definingDeclaration();
    SgClassDeclaration&     clsdef = SG_DEREF(isSgClassDeclaration(defdcl));

    return &n != clsdef.get_definition();
  }


  const SgType&
  stripDeclType(const SgType& ty)
  {
    if (const SgDeclType* dclty = isSgDeclType(&ty))
      return SG_DEREF(dclty->get_base_type());

    return ty;
  }


  const SgType*
  stripTypeAlias(const SgType* ty)
  {
    if (const SgTypedefType* tydef = isSgTypedefType(ty))
      return stripTypeAlias(tydef->get_base_type());

    return ty ? &stripDeclType(*ty) : ty;
  }

  const SgType&
  stripTypeAlias(const SgType& ty)
  {
    return SG_DEREF(stripTypeAlias(&ty));
  }


  const SgType&
  returnType(const SgFunctionType& ft)
  {
    return SG_DEREF(ft.get_return_type());
  }


  const SgType&
  typeOfExpr(const SgExpression& n)
  {
    return SG_DEREF(n.get_type());
  }

  const SgClassType*
  classtypeUnderType(const SgType& n, int otherStrips = 0)
  {
    const unsigned char STRIP_TO_CLASS = ( ct::STRIP_MODIFIER_ALIAS
                                         | SgType::STRIP_REFERENCE_TYPE
                                         | SgType::STRIP_RVALUE_REFERENCE_TYPE
                                         | otherStrips
                                         );

    return isSgClassType(&stripDeclType(SG_DEREF(stripDeclType(n).stripType(STRIP_TO_CLASS))));
  }

  const SgClassType*
  classtypeOfExpr(const SgExpression& n, int otherStrips = 0)
  {
    return classtypeUnderType(typeOfExpr(n), otherStrips);
  }


  struct NodeCollector : ExcludeTemplates
  {
      typedef std::unordered_set<SgNode*> NodeTracker;

      NodeCollector( NodeTracker& visited,
                     ct::ClassAnalysis& classes,
                     ct::CastAnalysis& casts,
                     const ct::RoseCompatibilityBridge& cb
                   )
      : visitedNodes(&visited), allClasses(&classes), allCasts(&casts), compat(&cb), currentClass(nullptr)
      {}

      void descend(SgNode& n, ct::ClassAnalysis::value_type* cls = nullptr);

      using ExcludeTemplates::handle;

      void handle(SgNode& n)                  { descend(n); }

      // classes
      void handle(SgClassDefinition& n)
      {
        // exclude secondary definitions (AST merge?)
        if (isSecondaryClassDefinition(n))
          return;

        auto emplaced = allClasses->emplace(&n, ct::ClassData{});

        ASSERT_require(emplaced.second);
        emplaced.first->second.abstractClass(n.get_isAbstract());

        descend(n, &*emplaced.first);
      }
/*
      void handle(SgMemberFunctionDeclaration& n)
      {
        if (currentClass && isVirtual(n))
          currentClass->second.virtualFunctions().emplace_back(compat->functionId(&n));

        descend(n);
      }

      void handle(SgVariableDeclaration& n)
      {
        if (currentClass && !si::isStatic(&n))
          currentClass->second.dataMembers().emplace_back(compat->variableId(&n));

        descend(n);
      }
*/
      // \todo consider providing a handler for SgClassDeclaration
      //       that only descends if the traversal is in allClass mode
      //       i.e., classes->containsAllClasses()


      // casts
      void handle(SgCastExp& n)
      {
        SgExpression& expr = SG_DEREF(n.get_operand());

        // store cast types and cast expressions
        (*allCasts)[ct::CastDesc{&typeOfExpr(expr), &typeOfExpr(n)}].push_back(&n);

        descend(n);
      }

      NodeCollector withClass(ct::ClassAnalysis::value_type* cls)
      {
        NodeCollector tmp{*this};

        tmp.currentClass = cls;
        return tmp;
      }

    private:
      NodeTracker*                       visitedNodes;
      ct::ClassAnalysis*                 allClasses;
      ct::CastAnalysis*                  allCasts;
      const ct::RoseCompatibilityBridge* compat;
      ct::ClassAnalysis::value_type*     currentClass;
  };

  template <class SetT, class ElemT>
  inline
  bool alreadyProcessed(SetT& s, const ElemT& e)
  {
    return !s.insert(e).second;
  }

  void NodeCollector::descend(SgNode& n, ct::ClassAnalysis::value_type* cls)
  {
    if (alreadyProcessed(*visitedNodes, &n))
      return;

    sg::traverseDispatchedChildren(this->withClass(cls), n);
  }

  //
  // memory pool traversal

  struct CollectClassesFromMemoryPool : ROSE_VisitTraversal
  {
      explicit
      CollectClassesFromMemoryPool(ct::ClassAnalysis& classes)
      : allClasses(classes)
      {}

      const SgNode* underTemplatedAncestor(SgNode* n)
      {
        const bool isRegular  = (  (n == nullptr)
                                || isSgGlobal(n)
                                );

        if (isRegular) return nullptr;

        if (ExcludeTemplates::templated(n)) return n;

        auto res = templatedAncestor.emplace(std::make_pair(n, n));

        if (!res.second) return res.first->second;

        const SgNode* underTemplate = underTemplatedAncestor(n->get_parent());

        res.first->second = underTemplate;
        return underTemplate;
      }

      // Required traversal function
      void visit (SgNode* node) override;

      //~ ~ResetDefinitionsInNonDefiningClassDeclarationsOnMemoryPool() {}

    private:
      ct::ClassAnalysis&                     allClasses;
      std::map<const SgNode*, const SgNode*> templatedAncestor;
  };

  void CollectClassesFromMemoryPool::visit (SgNode* node)
  {
    // For now we just do this for the SgClassDeclaration
    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);

    if (classDeclaration == nullptr) return;
    if (/*const SgNode* ancestor =*/ underTemplatedAncestor(classDeclaration)) return;

    SgClassDefinition* classDef = classDeclaration->get_definition();
    if (classDef == nullptr) return;

    auto emplaced = allClasses.emplace(classDef, ct::ClassData{});

    ASSERT_require(emplaced.second);
    emplaced.first->second.abstractClass(classDef->get_isAbstract());
  }


  struct CollectMembers
  {
    void operator()(ct::ClassAnalysis::value_type& entry) const
    {
      sg::NotNull<const SgClassDefinition> clsdef = entry.first;

      auto extractMembers =
         [&entry](const SgSymbol& sym) -> void
         {
           ct::RoseCompatibilityBridge compat;

           if (const SgMemberFunctionSymbol* memfnsym = isSgMemberFunctionSymbol(&sym))
           {
             sg::NotNull<SgMemberFunctionDeclaration> memfun = memfnsym->get_declaration();

             if (isVirtual(*memfun))
               entry.second.virtualFunctions().emplace_back(compat.functionId(memfun));
           }
           else if (const SgVariableSymbol* memvarsym = isSgVariableSymbol(&sym))
           {
             sg::NotNull<SgInitializedName>      memvar = memvarsym->get_declaration();
             sg::NotNull<SgDeclarationStatement> memdcl = isSgDeclarationStatement(memvar->get_parent());

             if (!si::isStatic(memdcl))
               entry.second.dataMembers().emplace_back(compat.variableId(memvar));
           }
         };

      std::for_each( symbolBegin(*clsdef), symbolLimit(*clsdef),
                     extractMembers
                   );
    }
  };

  void
  collectMembers(ct::ClassAnalysis& classes)
  {
    boost::range::for_each(classes, CollectMembers{});
  }

  void
  collectClassesFromMemoryPool(ct::ClassAnalysis& classes)
  {
    // similar to DQ's memory pool traversal
    // DQ (10/23/2024): This function collects all of the classDeclarations from the memory pool.
    // I think that the list that we get from the query of the AST, is insufficent and that more
    // are available in the memory pool.

    // ResetTemplateNamesOnMemoryPool traversal;
    CollectClassesFromMemoryPool traversal(classes);

    SgClassDeclaration::traverseMemoryPoolNodes(traversal);
    SgTemplateInstantiationDecl::traverseMemoryPoolNodes(traversal);

    collectMembers(classes);
  }


  template <class V>
  int cmpValue(const V& lhs, const V& rhs)
  {
    enum cmpresult : int { LT = -1, EQ = 0, GT = 1 };

    if (lhs < rhs) return LT;
    if (rhs < lhs) return GT;

    return EQ;
  }

  int cmpArrayLength(const SgArrayType& lhs, const SgArrayType& rhs)
  {
    int               cmpres = 0;
    ct::AbstractValue lhsval = ct::evaluateExpressionWithEmptyState(lhs.get_index());
    ct::AbstractValue rhsval = ct::evaluateExpressionWithEmptyState(rhs.get_index());

    // \todo check correctness of array length comparison
    firstDecisiveComparison
    || (cmpres = cmpValue(lhsval.isConstInt(), rhsval.isConstInt()))
    || (!lhsval.isConstInt()) // if not const, then assume that types are equal
    || (cmpres = cmpValue(lhsval.getIntValue(), rhsval.getIntValue()))
    ;

    return cmpres;
  }


  template <class SageModifier>
  int cmpModifier(const SageModifier& lhs, const SageModifier& rhs)
  {
    return cmpValue(lhs.get_modifier(), rhs.get_modifier());
  }

  int cmpModifier(const SgTypeModifier& lhs, const SgTypeModifier& rhs)
  {
    int cmpres = 0;

    firstDecisiveComparison
    || (cmpres = cmpValue(lhs.get_modifierVector(), rhs.get_modifierVector()))
    || (cmpres = cmpModifier(lhs.get_constVolatileModifier(), rhs.get_constVolatileModifier()))
    || (cmpres = cmpModifier(lhs.get_elaboratedTypeModifier(), rhs.get_elaboratedTypeModifier()))
    || (cmpres = cmpModifier(lhs.get_upcModifier(), rhs.get_upcModifier()))
    ;

    return cmpres;
  }


  /// type comparator
  struct TypeComparator : sg::DispatchHandler<int>
  {
      explicit
      TypeComparator( const SgType& rhs_type,
                      bool arrayDecay = false,
                      bool ignoreFunctionReturn = false
                    )
      : Base(),
        rhs(rhs_type),
        withArrayToPointerDecay(arrayDecay),
        ignoreFunctionReturnType(ignoreFunctionReturn)
      {}

      ~TypeComparator() = default;

      /// unreachable base case (non type nodes) => error
      void handle2(const SgNode& n, const SgNode&) { SG_UNEXPECTED_NODE(n); }

      // base case
      //   if we get here than the types must be different
      //   ==> we need separate handlers for non-shared types
      void handle2(const SgType& lhs, const SgType& rhs)
      {
        res = cmpValue(&lhs, &rhs);
      }

      void handle2(const SgNamedType& lhs, const SgNamedType& rhs)
      {
        const SgDeclarationStatement& lhsdcl = SG_DEREF(lhs.get_declaration());
        const SgDeclarationStatement& rhsdcl = SG_DEREF(rhs.get_declaration());
        const SgDeclarationStatement* lhskey = lhsdcl.get_firstNondefiningDeclaration();
        const SgDeclarationStatement* rhskey = rhsdcl.get_firstNondefiningDeclaration();

        res = cmpValue(lhskey, rhskey);
      }

      void handle2(const SgPointerType& lhs, const SgPointerType& rhs)
      {
        res = cmpBaseTypes(lhs, rhs);
      }

      void handle2(const SgArrayType& lhs, const SgArrayType& rhs)
      {
        firstDecisiveComparison
        || (res = cmpBaseTypes(lhs, rhs))
        || (res = cmpArrayLength(lhs, rhs))
        ;
      }

      void handle2(const SgReferenceType& lhs, const SgReferenceType& rhs)
      {
        res = cmpBaseTypes(lhs, rhs);
      }

      void handle2(const SgRvalueReferenceType& lhs, const SgRvalueReferenceType& rhs)
      {
        res = cmpBaseTypes(lhs, rhs);
      }

      void handle2(const SgModifierType& lhs, const SgModifierType& rhs)
      {
        // short circuit eval to compute the result based on the first divergent element
        firstDecisiveComparison
        || (res = cmpModifier(lhs.get_typeModifier(), rhs.get_typeModifier()))
        || (res = cmpBaseTypes(lhs, rhs))
        ;
      }

      void handle2(const SgFunctionType& lhs, const SgFunctionType& rhs)
      {
        res = cmpFunctionTypes(lhs, rhs);
      }

      void handle2(const SgMemberFunctionType& lhs, const SgMemberFunctionType& rhs)
      {
        firstDecisiveComparison
        //~ || (res = cmpValue(lhs.get_ref_qualifiers(),  rhs.get_ref_qualifiers())) // PP (10/4/22): does not seem to be used
        || (res = cmpValue(lhs.get_mfunc_specifier(), rhs.get_mfunc_specifier()))
        || (res = cmpValue(lhs.get_has_ellipses(),    rhs.get_has_ellipses()))
        || (res = cmpFunctionTypes(lhs, rhs, ignoreFunctionReturnType))
        ;
      }

      /// base handler, should never be reached
      void handleTypes(const SgNode& n, const SgNode& /* tag */) { SG_UNEXPECTED_NODE(n); }

      /// main handler checking if lhs and rhs dynamic types match
      /// \details
      ///   types of rhs and lhs match:
      ///     - yes: the type comparison will be continued within the
      ///            corresponding handle2 functions.
      ///     - no:  res is set according to variantT
      template <class SageType>
      void handleTypes(const SageType& lhs, const SgType& /* tag */)
      {
        // typedef types should not be skipped before they are dispatched
        ASSERT_require(lhs.variantT() != V_SgTypedefType);
        ASSERT_require(rhs.variantT() != V_SgTypedefType);

        //~ res = cmpValue(SageType::static_variant, rhs.variantT());
        res = cmpValue(lhs.variantT(), rhs.variantT());

        // we are done, if the comparison was decisive
        if (res) return;

        //~ handle2(lhs, SG_ASSERT_TYPE(const SageType, rhs));
        handle2(lhs, static_cast<const SageType&>(rhs));
      }

      // forwarding handler that separates types from non-types
      template <class SageNode>
      void handle(const SageNode& lhs)
      {
        handleTypes(lhs, lhs);
      }

      /// special handlers that deal with array to pointer decay (if the flag is enabled)
      /// \{
      void handle(const SgArrayType& lhs)
      {
        if (!withArrayToPointerDecay)
          handleTypes(lhs, lhs);
        else
          handleArrayDecay(lhs, rhs);
      }

      void handle(const SgPointerType& lhs)
      {
        if (!withArrayToPointerDecay)
          handleTypes(lhs, lhs);
        else
          handleArrayDecay(lhs, rhs);
      }
      /// \}

    private:

      /// type comparison with array decay
      /// \details
      ///   treats arrays equivalent to pointers
      void handleArrayDecay(const SgType& lhs, const SgType& rhs)
      {
        std::tuple<const SgType*, VariantT> lhs_decay = decayedTypeData(lhs);
        std::tuple<const SgType*, VariantT> rhs_decay = decayedTypeData(rhs);

        firstDecisiveComparison
        || (res = cmpValue(std::get<1>(lhs_decay), std::get<1>(rhs_decay)))
        || (res = cmpTypes(std::get<0>(lhs_decay), std::get<0>(rhs_decay)), false)
        ;
      }

      // returns the base of arrays and pointers and a "decayed" variantT
      std::tuple<const SgType*, VariantT>
      decayedTypeData(const SgType& n)
      {
        // arrays treated as pointers after decay
        if (const SgArrayType* at = isSgArrayType(&n))
          return std::make_pair(at->get_base_type(), V_SgPointerType);

        if (const SgPointerType* pt = isSgPointerType(&n))
          return std::make_pair(pt->get_base_type(), V_SgPointerType);

        return std::make_pair(&n, n.variantT());
      }

      // compare base types
      template <class SageType>
      int cmpBaseTypes(const SageType& lhs, const SageType& rhs)
      {
        return cmpTypes(lhs.get_base_type(), rhs.get_base_type(), false);
      }

      int cmpFunctionTypes(const SgFunctionType& lhs, const SgFunctionType& rhs, bool ignoreReturn = false);

      int cmpTypes(const SgType* lhs, const SgType* rhs);
      int cmpTypes(const SgType* lhs, const SgType* rhs, bool ptrDecay);


    public:
      static
      int compare(const SgType&, const SgType&, bool ptrDecay = false, bool ignoreFuncReturn = false);

    private:
      const SgType& rhs;
      bool          withArrayToPointerDecay  = false;
      bool          ignoreFunctionReturnType = false;
  };


  int
  TypeComparator::compare(const SgType& lhs, const SgType& rhs, bool ptrDecay, bool ignoreFuncReturn)
  {
    const SgType& llhs = stripTypeAlias(lhs);
    const SgType& rrhs = stripTypeAlias(rhs);

    // types are the same if the type pointers are the same
    if (&llhs == &rrhs) return 0;

    // or if they are structurally the same
    return sg::dispatch(TypeComparator{rrhs, ptrDecay, ignoreFuncReturn}, &llhs);
  }

  int
  TypeComparator::cmpTypes(const SgType* lhs, const SgType* rhs, bool ptrDecay)
  {
    return compare(SG_DEREF(lhs), SG_DEREF(rhs), ptrDecay, ignoreFunctionReturnType);
  }

  int
  TypeComparator::cmpTypes(const SgType* lhs, const SgType* rhs)
  {
    return cmpTypes(lhs, rhs, withArrayToPointerDecay);
  }

  int
  cmpParamTypes(const SgFunctionType& lhs, const SgFunctionType& rhs)
  {
    static constexpr bool withArrayDecay = true;

    const SgTypePtrList& lhsparm = lhs.get_arguments();
    const SgTypePtrList& rhsparm = rhs.get_arguments();
    size_t               i       = 0;
    size_t               N       = lhsparm.size();
    int                  cmpres  = cmpValue(N, rhsparm.size());

    while ((cmpres == 0) && (i < N))
    {
      cmpres = TypeComparator::compare(SG_DEREF(lhsparm.at(i)), SG_DEREF(rhsparm.at(i)), withArrayDecay);
      ++i;
    }

    return cmpres;
  }

  int
  TypeComparator::cmpFunctionTypes(const SgFunctionType& lhs, const SgFunctionType& rhs, bool ignoreReturn)
  {
    // since C++ cannot return arrays, both true and false should be correct
    static constexpr bool returnDecay     = true;

    int cmpres = 0;

    firstDecisiveComparison
    || (cmpres = cmpParamTypes(lhs, rhs))
    || ignoreReturn
    || (cmpres = cmpTypes(&returnType(lhs), &returnType(rhs), returnDecay))
    ;

    return cmpres;
  }

  std::vector<const SgClassDefinition*>
  collectClassAndAncestors(const SgClassDefinition* cls, std::vector<const SgClassDefinition*>&& res = {})
  {
    ASSERT_not_null(cls);

    for (const SgBaseClass* bscls : cls->get_inheritances())
    {
      ASSERT_not_null(bscls);

      if (!bscls->get_isDirectBaseClass()) continue;

      const SgClassDeclaration* bsdcl = bscls->get_base_class();

      if (!bsdcl)
      {
        msgError() << "base class declaration is not available "
                   << typeid(*bscls).name()
                   << std::endl;

        continue;
      }

      const SgDeclarationStatement* defdcl = bsdcl->get_definingDeclaration();
      const SgClassDeclaration*     bsdefn = isSgClassDeclaration(defdcl);
      ASSERT_not_null(bsdefn);

      res = collectClassAndAncestors(bsdefn->get_definition(), std::move(res));
    }

    res.push_back(cls);
    return res;
  }

  using TemplateAncestorBase = std::tuple<bool, const SgNode*>;
  struct TemplateAncestor : TemplateAncestorBase
  {
    using base = TemplateAncestorBase;
    using base::base;

    bool          hasTemplateAncestor() const { return std::get<0>(*this); }
    const SgNode* templateAncestor()    const { return std::get<1>(*this); }
  };

  TemplateAncestor fromTemplate(const SgNode* n);

  // uses the same exclusion criteria as the AST traversal to identify whether a node is a template or not
  struct FromTemplate : sg::DispatchHandler<TemplateAncestor>, ExcludeTemplates
  {
    FromTemplate()
    : Base({ true, nullptr }), ExcludeTemplates()
    {}

    using ExcludeTemplates::handle;

    void handle(const SgNode& n)   { res = fromTemplate(n.get_parent()); }
    void handle(const SgGlobal& n) { res = { false, nullptr }; }
  };

  TemplateAncestor
  fromTemplate(const SgNode* n)
  {
    if (n == nullptr) return { false, nullptr };

    TemplateAncestor res = sg::dispatch(FromTemplate{}, n);

    if (res.hasTemplateAncestor() && (res.templateAncestor() == nullptr))
      return { true, n };

    return res;
  }

  void logPathToGlobal(std::ostream& os, const SgNode* n)
  {
    os << "  >" << n->class_name();

    n = n->get_parent();

    while (n && !isSgGlobal(n))
    {
      os << "." << n->class_name();

      n = n->get_parent();
    }

    os << std::endl;
  }
}

namespace CodeThorn
{

std::string typeNameOf(ClassKeyType key)
{
  return key ? ::typeNameOf(*key) : "-classdef-is-null-";
}

boost::optional<std::string>
missingDiagnostics(ClassKeyType key)
{
  bool               extra = false;
  std::ostringstream os;

  TemplateAncestor   res = fromTemplate(key);

  if (res.hasTemplateAncestor())
  {
    ASSERT_not_null(res.templateAncestor());

    extra = true;
    os <<   "  - The class is either a templated class or it has a templated ancestor node"
       << "\n    in the ROSE AST. Thus, the class was not traversed by the class hierarchy analysis."
       << "\n    (Templates are currently not supported by the class hierarchy analysis.)"
       << "\n  - information on the templated AST node:"
       << "\n    Type of the sage node in the ROSE AST: " << res.templateAncestor()->class_name()
       << "\n    SageInterface::get_name(n) = " << si::get_name(res.templateAncestor())
       << std::endl;
  }

  if (key && isSecondaryClassDefinition(*key))
  {
    extra = true;
    os <<   "  - This is a secondary class definition:"
       << "\n    isSgClassDeclaration(classdef->get_declaration()->get_definingDeclaration())"
       <<        "->get_definition != classdef"
       << "\n    classdef = " << key
       << "\n    classname = " << SG_DEREF(key->get_declaration()).get_name()
       << "\n    from ASTMerge?"
       << std::endl;
  }

  if (!extra)
    return boost::none; // std::nullopt

  return os.str();
}


std::string dbgInfo(ClassKeyType key, std::size_t numParents)
{
  const SgNode*      curr = key;
  std::ostringstream os;

  while (numParents && curr)
  {
    os << " -> " << typeid(*curr).name();
    if (const SgDeclarationStatement* dcl = isSgDeclarationStatement(curr))
      os << "[" << si::get_name(dcl) << "]";

    const SgNode* prev = curr;

    curr = curr->get_parent();

    if (curr)
    {
      std::vector<SgNode*> succ = curr->get_traversalSuccessorContainer();

      if (std::find(succ.begin(), succ.end(), prev) == succ.end())
        os << "!";
    }

    --numParents;
  }

  if (curr)
  {
    os << " -> " << typeid(*curr).name();
    if (const SgDeclarationStatement* dcl = isSgDeclarationStatement(curr))
      os << "[" << si::get_name(dcl) << "]";
  }
  else
    os << " -> nullptr";

  return os.str();
}


void inheritanceEdges( const SgClassDefinition* def,
                       std::function<void(const SgClassDefinition*, const SgClassDefinition*, bool)> fn
                     )
{
  const SgClassDefinition&  clazz = SG_DEREF(def);
  const SgBaseClassPtrList& bases = clazz.get_inheritances();

  for (SgBaseClass* pbase : bases)
  {
    SgBaseClass&         base   = SG_DEREF(pbase);
    const bool           direct = base.get_isDirectBaseClass();

    if (!direct) continue;

    SgClassDeclaration*  pdecl  = base.get_base_class();

    if (!pdecl)
    {
      msgWarn() << "base class declaration is not available "
                << typeid(base).name()
                << std::endl;

      continue;
    }

    SgBaseClassModifier&    bmod   = SG_DEREF(base.get_baseClassModifier());
    SgClassDeclaration&     decl   = SG_DEREF(pdecl);
    SgDeclarationStatement* defdcl = decl.get_definingDeclaration();
    SgClassDeclaration&     clsdef = SG_DEREF(isSgClassDeclaration(defdcl));

    fn(def, clsdef.get_definition(), bmod.isVirtual());
  }
}


namespace
{
  ClassKeyType classDefinition_opt(SgDeclarationStatement& n)
  {
    SgDeclarationStatement* defdcl = n.get_definingDeclaration();
    SgClassDeclaration*     clsdcl = isSgClassDeclaration(defdcl);

    return defdcl ? SG_DEREF(clsdcl).get_definition()
                  : nullptr
                  ;
  }

  ClassKeyType classDefinition_opt(const SgClassType& n)
  {
    return classDefinition_opt(SG_DEREF( n.get_declaration() ));
  }

  struct ClassDefinition : sg::DispatchHandler<std::pair<ClassKeyType, TypeKeyType> >
  {
      void descend(const SgNode* n)               { res = sg::dispatch(*this, n); }

      void handle(const SgNode& n)                { SG_UNEXPECTED_NODE(n); }
      void handle(const SgType& n)                { res = std::make_pair(nullptr, &n); }

      void handle(const SgModifierType& n)        { descend(n.get_base_type()); }
      void handle(const SgTypedefType& n)         { descend(n.get_base_type()); }
      void handle(const SgPointerType& n)         { descend(n.get_base_type()); }
      void handle(const SgReferenceType& n)       { descend(n.get_base_type()); }
      void handle(const SgRvalueReferenceType& n) { descend(n.get_base_type()); }
      void handle(const SgDeclType& n)            { descend(SG_DEREF(n.get_base_expression()).get_type()); }
      //~ void handle(SgAutoType& n)            { /* what is the real type? */ }

      void handle(const SgClassType& n)           { res = std::make_pair(classDefinition_opt(n), &n); }

      static
      ReturnType get(TypeKeyType n);
  };
}


std::pair<ClassKeyType, TypeKeyType>
getClassCastInfo (TypeKeyType n)
{
  return sg::dispatch(ClassDefinition{}, n);
}

std::ostream& operator<<(std::ostream& os, const CastWriterDbg& cw)
{
  const SgCastExp&    castnode = SG_DEREF(cw.node);
  const Sg_File_Info& where    = SG_DEREF(castnode.get_startOfConstruct());

  os << castnode.unparseToString()
     << " [" << castnode.get_cast_type() << "] "
     << "@" << where.get_line() << ":" << where.get_col();

  return os;
}

int
RoseCompatibilityBridge::compareNames(FunctionKeyType lhs, FunctionKeyType rhs) const
{
  const std::string lhsname{functionName(lhs)};
  const std::string rhsname{functionName(rhs)};
  const bool        lhsIsDtor = lhsname.front() == '~';
  const bool        rhsIsDtor = rhsname.front() == '~';
  const int         cmpres = (lhsIsDtor && rhsIsDtor) ? 0 : lhsname.compare(rhsname);

  return cmpres;
}

int compareTypes(const SgType& lhs, const SgType& rhs)
{
  return TypeComparator::compare(lhs, rhs, false, false);
}


int
RoseCompatibilityBridge::compareFunctionTypes(FunctionTypeKeyType lhs, FunctionTypeKeyType rhs, bool exclReturn) const
{
  static constexpr bool withArrayDecay = true;

  return TypeComparator::compare(SG_DEREF(lhs), SG_DEREF(rhs), exclReturn, withArrayDecay);
}


int
RoseCompatibilityBridge::compareTypes(FunctionKeyType lhs, FunctionKeyType rhs, bool exclReturn) const
{
  const SgFunctionType& lhsty = functionType(lhs);
  const SgFunctionType& rhsty = functionType(rhs);

  const int res = compareFunctionTypes(&lhsty, &rhsty, exclReturn);

  return res;
}

VariableKeyType
RoseCompatibilityBridge::variableId(SgInitializedName* var) const
{
  ASSERT_not_null(var);

  return var;
}

VariableKeyType
RoseCompatibilityBridge::variableId(SgVariableDeclaration* var) const
{
  ASSERT_not_null(var);

  SgInitializedNamePtrList& lst = var->get_variables();
  ASSERT_require(lst.size() == 1);

  return variableId(lst[0]);
}

std::string
RoseCompatibilityBridge::nameOf(VariableKeyType vid) const
{
  //~ return vid.toString(varMap);
  return SG_DEREF(vid).get_name();
}

std::string
RoseCompatibilityBridge::nameOf(FunctionKeyType fun) const
{
  return functionName(fun);
}

std::intptr_t
RoseCompatibilityBridge::numericId(AnyKeyType id) const
{
  return std::intptr_t(id);
}

std::string
RoseCompatibilityBridge::uniqueName(AnyKeyType id) const
{
  // ClassKeyType
  if (ClassKeyType cls = isSgClassDefinition(id))
  {
    sg::NotNull<const SgClassDeclaration> dcl = isSgClassDeclaration(cls->get_parent());

    return dcl->get_mangled_name();
  }

  // FunctionKeyType
  if (FunctionKeyType fun = isSgFunctionDeclaration(id))
    return fun->get_mangled_name();

  // TypeKeyType, FunctionTypeKeyType
  if (TypeKeyType ty = isSgType(id))
    return ty->get_mangled();

  // VariableKeyType
  if (VariableKeyType var = isSgInitializedName(id))
    return var->get_mangled_name();

  // invalid (null) ID
  if (id == AnyKeyType{})
    return "<null>";

  // ExpressionKeyType, CastKeyType
  if (/*ExpressionKeyType exp =*/ isSgExpression(id))
    throw std::runtime_error{"Unable to compute unique name for this id."};

  // unexpected types
  throw std::logic_error{"Internal-ERROR: Unexpected ID type as input to uniqueName."};
}

namespace
{
  struct CovarianceChecker : sg::DispatchHandler<RoseCompatibilityBridge::TypeRelation>
  {
      explicit
      CovarianceChecker(const ClassAnalysis& classAnalysis, const SgType& baseType)
      : Base(RoseCompatibilityBridge::unrelated), classes(classAnalysis), baseTy(&baseType)
      {}

      // non class types are checked for type equality
      ReturnType
      sameOrCovariant(const SgType& drvTy, const SgType& basTy)
      {
        return compareTypes(drvTy, basTy) == 0 ? RoseCompatibilityBridge::sametype
                                               : RoseCompatibilityBridge::unrelated
                                               ;
      }

      ReturnType
      sameOrCovariant(const SgModifierType& drvTy, const SgModifierType& basTy)
      {
        const SgType& baseBaseTy = SG_DEREF(basTy.get_base_type());

        if (cmpModifier(drvTy.get_typeModifier(), basTy.get_typeModifier()) != 0)
          return RoseCompatibilityBridge::unrelated;

        return sg::dispatch(CovarianceChecker{classes, baseBaseTy}, drvTy.get_base_type());
      }

      // class types are checked for covariance
      ReturnType
      sameOrCovariant(const SgClassType& drvClass, const SgClassType& basClass)
      {
        if (compareTypes(drvClass, basClass) == 0) return RoseCompatibilityBridge::sametype;

        const SgClassDeclaration* drvDcl0 = isSgClassDeclaration(drvClass.get_declaration());
        const SgClassDeclaration* basDcl0 = isSgClassDeclaration(basClass.get_declaration());
        if (!drvDcl0 || !basDcl0) return RoseCompatibilityBridge::unrelated;

        const SgClassDeclaration* drvDcl  = isSgClassDeclaration(drvDcl0->get_definingDeclaration());
        const SgClassDeclaration* basDcl  = isSgClassDeclaration(basDcl0->get_definingDeclaration());
        if (!drvDcl || !basDcl) return RoseCompatibilityBridge::unrelated;

        const SgClassDefinition*  drvDef  = drvDcl->get_definition();
        const SgClassDefinition*  basDef  = basDcl->get_definition();

        if (drvDef == basDef)
          return RoseCompatibilityBridge::sametype;

        bool isCovariant = false;

        try
        {
          isCovariant = classes.isBaseOf(basDef, drvDef);
        }
        catch (const std::out_of_range&)
        {
          static int prnNumWarn = 3;

          if (classes.containsAllClasses()) throw;

          if (prnNumWarn > 0)
          {
            --prnNumWarn;
            msgWarn() << "assuming covariant return [requires full translation unit analysis]"
                      << (prnNumWarn ? "" : "...")
                      << std::endl;
          }

          isCovariant = true;
        }

        if (isCovariant)
          return RoseCompatibilityBridge::covariant;

        return RoseCompatibilityBridge::unrelated;
      }

      ReturnType
      descend(const SgType* ty) const
      {
        return sg::dispatch(*this, ty);
      }

      /// generic template routine to check for covariance
      template <class SageType>
      ReturnType
      check(const SageType& drvTy)
      {
        baseTy = stripTypeAlias(baseTy);

        if (drvTy.variantT() != SG_DEREF(baseTy).variantT())
          return RoseCompatibilityBridge::unrelated;

        return sameOrCovariant(drvTy, static_cast<const SageType&>(*baseTy));
      }

      void handle(const SgNode& drv)                { SG_UNEXPECTED_NODE(drv); }

      void handle(const SgType& drv)                { res = check(drv); }

      // skip typedefs
      void handle(const SgTypedefType& drv)         { res = descend(drv.get_base_type()); }

      // covariance is only maintained through classes and modifiers
      // @{
      void handle(const SgClassType& drv)           { res = check(drv); }
      void handle(const SgModifierType& drv)        { res = check(drv); }
      // @}

      // if same prefix, it should have been removed by CovariantPrefixChecker
      // @{
      void handle(const SgReferenceType& drv)       { res = RoseCompatibilityBridge::unrelated; }
      void handle(const SgRvalueReferenceType& drv) { res = RoseCompatibilityBridge::unrelated; }
      // void handle(const SgPointerType& drv) { can occur and are handled through check }
      // @}

    private:
      const ClassAnalysis& classes;
      const SgType*        baseTy;
  };


  using PolymorphicRootTypesBase = std::tuple<const SgType*, const SgType*>;

  struct PolymorphicRootTypes : PolymorphicRootTypesBase
  {
    using base = PolymorphicRootTypesBase;
    using base::base;

    const SgType* derivedRoot() const { return std::get<0>(*this); }
    const SgType* baseRoot()    const { return std::get<0>(*this); }
  };

  /// checks that both types have the same covariant prefixes (e.g., reference, pointer, rvalue-reference)
  ///   to a class type.
  struct CovariantPrefixChecker : sg::DispatchHandler<PolymorphicRootTypes>
  {
      explicit
      CovariantPrefixChecker(const SgType& baseType)
      : basTy(&baseType)
      {}

      static
      bool typeChk(const SgType&, const SgType&)
      {
        return true;
      }

      static
      bool typeChk(const SgModifierType& derived, const SgModifierType& base)
      {
        return cmpModifier(derived.get_typeModifier(), base.get_typeModifier()) == 0;
      }

      ReturnType
      descend(SgType* ty) const
      {
        return sg::dispatch(*this, ty);
      }

      /// generic template routine to check for covariance
      /// if @chw is null, the check tests for strict equality
      template <class T>
      ReturnType
      check(const T& drvTy)
      {
        basTy = stripTypeAlias(basTy);

        // symmetric check for strict type equality
        if (drvTy.variantT() != SG_DEREF(basTy).variantT())
          return ReturnType{ nullptr, nullptr };

        //~ const T&   baseType    = SG_ASSERT_TYPE(const T, *basTy);
        const T&   baseType    = static_cast<const T&>(*basTy);
        const bool prefixMatch = typeChk(drvTy, baseType);

        return prefixMatch ? ReturnType{ drvTy.get_base_type(), baseType.get_base_type() }
                           : ReturnType{ nullptr, nullptr } ;
      }

      void handle(const SgNode& drv)                { SG_UNEXPECTED_NODE(drv); }

      void handle(const SgType& drv)                {}

      // skip typedefs
      void handle(const SgTypedefType& drv)         { res = descend(drv.get_base_type()); }

      // the modifiers must equal
      void handle(const SgModifierType& drv)        { res = check(drv); }

      // polymorphic root types (must also equal)
      // @{
      void handle(const SgReferenceType& drv)       { res = check(drv); }
      void handle(const SgRvalueReferenceType& drv) { res = check(drv); }
      void handle(const SgPointerType& drv)         { res = check(drv); }
      // @}
    private:
      const SgType* basTy;
  };
}


RoseCompatibilityBridge::TypeRelation
RoseCompatibilityBridge::areSameOrCovariant( const ClassAnalysis& classes,
                                             TypeKeyType basId,
                                             TypeKeyType drvId
                                           ) const
{
  ASSERT_require(basId != TypeKeyType{});
  ASSERT_require(drvId != TypeKeyType{});

  // skip equal polymorphic stem (e.g., reference, pointer)
  const PolymorphicRootTypes rootTypes = sg::dispatch(CovariantPrefixChecker{*basId}, drvId);
  const SgType*              drvRoot   = rootTypes.derivedRoot();

  if (!drvRoot)
  {
    //~ std::cerr << "w/o root: " << basId.getIdCode() << "<>" << drvId.getIdCode() << std::endl;
    return sg::dispatch(CovarianceChecker{classes, *basId}, drvId);
  }

  //~ std::cerr << "w/  root: " << basId.getIdCode() << "<>" << drvId.getIdCode() << std::endl;
  // test if the roots are covariant
  return sg::dispatch(CovarianceChecker{classes, SG_DEREF(rootTypes.baseRoot())}, drvRoot);
}


RoseCompatibilityBridge::TypeRelation
RoseCompatibilityBridge::haveSameOrCovariantReturn( const ClassAnalysis& classes,
                                                    FunctionKeyType basId,
                                                    FunctionKeyType drvId
                                                  ) const
{
  const SgFunctionType&      basTy = functionType(basId);
  const SgFunctionType&      drvTy = functionType(drvId);

  return areSameOrCovariant(classes, basTy.get_return_type(), drvTy.get_return_type());
}


FunctionKeyType
RoseCompatibilityBridge::functionId(const SgFunctionDeclaration* fun) const
{
  return fun ? &keyDecl(*const_cast<SgFunctionDeclaration*>(fun)) : fun;
}

void
RoseCompatibilityBridge::extractFromProject(ClassAnalysis& classes, CastAnalysis& casts, ASTRootType n) const
{
  NodeCollector::NodeTracker visited;

  sg::dispatch(NodeCollector{visited, classes, casts, *this}, n);

  collectMembers(classes);
}

void
RoseCompatibilityBridge::extractFromMemoryPool(ClassAnalysis& classes) const
{
  collectClassesFromMemoryPool(classes);
}

void
RoseCompatibilityBridge::extractClassAndBaseClasses(ClassAnalysis& classes, ClassKeyType n) const
{
  std::vector<ClassKeyType>  ancestors = collectClassAndAncestors(n);
  CastAnalysis               tmpCasts;
  NodeCollector::NodeTracker visited;

  for (ClassKeyType cls : ancestors)
  {
    if (classes.find(cls) == classes.end())
    {
      sg::dispatch(NodeCollector{visited, classes, tmpCasts, *this}, cls);
    }
  }

  // \todo this is super inefficient, because we really only need to collect
  //       the members for classes that were newly discovered.
  // Can this be moved into the loop?
  collectMembers(classes);
}

bool
RoseCompatibilityBridge::isPureVirtual(FunctionKeyType id) const
{
  return ::isPureVirtual(id);
}

bool RoseCompatibilityBridge::hasDefinition(FunctionKeyType id) const
{
  return ::hasDefinition(id);
}

FuncNameFn
RoseCompatibilityBridge::functionNaming() const
{
  return [](FunctionKeyType id) -> std::string { return RoseCompatibilityBridge{}.nameOf(id); };
}

VarNameFn
RoseCompatibilityBridge::variableNaming() const
{
  return [](VariableKeyType id) -> std::string { return RoseCompatibilityBridge{}.nameOf(id); };
}


ClassNameFn
RoseCompatibilityBridge::classNaming() const
{
  return typeNameOf;
}

}

namespace
{
  // \todo complete these functions
  // https://stackoverflow.com/questions/15590832/conditions-under-which-compiler-will-not-define-implicits-constructor-destruct

  using CvQualificationBase = std::tuple<const SgType*, std::uint_fast8_t>;
  struct CvQualification : CvQualificationBase
  {
    enum Kind : std::uint8_t
    {
      none = 0, constqual = (1<<0), volatilequal = (1<<1), all = constqual | volatilequal
    };

    using base = CvQualificationBase;
    using base::base;

    const SgType* type()        const { return std::get<0>(*this); }
    bool          hasConst()    const { return (std::get<1>(*this) & constqual) == constqual; }
    bool          hasVolatile() const { return (std::get<1>(*this) & volatilequal) == volatilequal; }
    bool          hasNone()     const { return (std::get<1>(*this) & all) == none; }
  };


  std::uint_fast8_t
  cvQualifier(const SgConstVolatileModifier& cvmod, std::uint_fast8_t qual = 0)
  {
    if (cvmod.isVolatile()) qual += CvQualification::volatilequal;
    if (cvmod.isConst())    qual += CvQualification::constqual;

    return qual;
  }

  std::uint_fast8_t
  cvQualifier(const SgTypeModifier& tymod, std::uint_fast8_t qual = 0)
  {
    return cvQualifier(tymod.get_constVolatileModifier(), qual);
  }

  std::uint_fast8_t
  cvQualifier(const SgModifierType& modty, std::uint_fast8_t qual = 0)
  {
    return cvQualifier(modty.get_typeModifier(), qual);
  }

  CvQualification
  removeCvQual(const SgType* ty, int qualifier = CvQualification::none)
  {
    if (const SgModifierType* modTy = isSgModifierType(ty))
      return removeCvQual(modTy->get_base_type(), cvQualifier(*modTy, qualifier));

    return { ty, qualifier };
  }

  using RefRemovalFn = std::function<sg::NotNull<const SgType>(sg::NotNull<const SgType>) >;

  sg::NotNull<const SgType>
  removeRvalueReference(sg::NotNull<const SgType> ty)
  {
    if (const SgRvalueReferenceType* refty = isSgRvalueReferenceType(&*ty))
      return refty->get_base_type();

    return ty;
  }

  sg::NotNull<const SgType>
  removeReference(sg::NotNull<const SgType> ty)
  {
    if (const SgReferenceType* refty = isSgReferenceType(&*ty))
      return refty->get_base_type();

    return ty;
  }

  sg::NotNull<const SgType>
  removeArray(sg::NotNull<const SgType> ty)
  {
    if (const SgArrayType* arrty = isSgArrayType(&*ty))
      return arrty->get_base_type();

    return ty;
  }


  ///
  /// new code below

  using ParameterTypeDescBase = std::tuple<bool, bool, bool, bool, bool>;
  struct ParameterTypeDesc : ParameterTypeDescBase
  {
    using base = ParameterTypeDescBase;
    using base::base;

    bool        paramHasClassType()   const { return std::get<0>(*this); }
    bool        withReference()       const { return std::get<1>(*this); }
    bool        withRValueReference() const { return std::get<2>(*this); }
    bool        withConst()           const { return std::get<3>(*this); }
    bool        withVolatile()        const { return std::get<4>(*this); }
  };


  ParameterTypeDesc
  specialMemberFunctionParameter(const SgMemberFunctionDeclaration& mfn, ct::ClassKeyType clkey)
  {
    const SgFunctionParameterList&  paramList = SG_DEREF(mfn.get_parameterList());
    const SgInitializedNamePtrList& params    = paramList.get_args();
    const std::size_t               numParams = params.size();

    ASSERT_require(numParams > 0);
    sg::NotNull<const SgType> const paramTy   = SG_DEREF(params.front()).get_type();
    sg::NotNull<const SgType> const unRefTy   = removeReference(paramTy);
    const bool                      hasRef    = paramTy != unRefTy;
    sg::NotNull<const SgType> const unRvalueRefTy = removeRvalueReference(paramTy);
    const bool                      hasRValueRef  = paramTy != unRvalueRefTy;

    // only one, ref or rvalueRed can be true
    ASSERT_require((hasRef & hasRValueRef) == false);

    sg::NotNull<const SgType> const afterRefTy = hasRef ? unRefTy : unRvalueRefTy;
    CvQualification                 unCvTy = removeCvQual(unRefTy);
    const SgClassType* const        paramClassTy = isSgClassType(unCvTy.type());

    // check that the parameter type is aligned with the enclosing class of the member function.
    if (!paramClassTy || (ct::getClassDefOpt(*paramClassTy) != clkey))
      return { /*all false*/ };

    return { true, hasRef, hasRValueRef, unCvTy.hasConst(), unCvTy.hasVolatile() };
  }


  ParameterTypeDesc
  specialMemberFunctionParameter(const SgMemberFunctionDeclaration& mfn)
  {
    return specialMemberFunctionParameter(mfn, &ct::getClassDefForFunction(mfn));
  }

  bool isMoveParameterType(ParameterTypeDesc parm)
  {
    return (  parm.paramHasClassType()
           && parm.withRValueReference()
           && !parm.withConst()
           && !parm.withVolatile()
           );
  }

  bool isCopyParameterType(ParameterTypeDesc parm)
  {
    // this casts a wide net, wider than needed
    //   purpose is to accept signatures that compilers
    //   also accept.
    return (  parm.paramHasClassType()
           && !parm.withRValueReference()
           );
  }

  std::tuple<std::size_t, std::size_t>
  countArguments(const SgFunctionDeclaration& n)
  {
    const SgFunctionParameterList&  paramList = SG_DEREF(n.get_parameterList());
    const SgInitializedNamePtrList& params = paramList.get_args();
    const std::size_t               numParams = params.size();

    const auto                      paramBeg  = params.begin();
    const auto                      paramLim  = params.end();
    auto  isDefaulted =
             [](const SgInitializedName* p) -> bool
             {
               return (p != nullptr) && (p->get_initializer() != nullptr);
             };
    const auto                      paramPos  = std::find_if(paramBeg, paramLim, isDefaulted);
    const std::size_t               numNonDefault = std::distance(paramBeg, paramPos);

    return { numNonDefault, numParams };
  }


  bool hasDestructor(const ct::ClassAnalysis::mapped_type& clazz)
  {
    const ct::SpecialMemberFunctionContainer& specials = clazz.specialMemberFunctions();
    auto       isDestructor = [](const ct::SpecialMemberFunction& smf) -> bool { return smf.isDestructor(); };
    auto const lim          = specials.end();
    auto const pos          = std::find_if(specials.begin(), lim, isDestructor);

    return (pos != lim) && (pos->function() != ct::FunctionKeyType{});
  }

  bool hasConstructor(const ct::ClassAnalysis::mapped_type&, const SgMemberFunctionDeclaration&)
  {
    msgError() << "RoseCompatibilityBridge::hasConstructor is not fully implemented."
               << std::endl;

    return false;
  }

  bool
  hasDefaultAssignType( const SgMemberFunctionDeclaration& mfn,
                        ct::ClassKeyType clkey,
                        std::function<bool(ParameterTypeDesc)> pred
                      )
  {
    std::size_t minargs = 0;
    std::size_t maxargs = 0;

    std::tie(minargs, maxargs) = countArguments(mfn);

    return (  (minargs == 1)
           && (maxargs == 1)
           && pred(specialMemberFunctionParameter(mfn, clkey))
           );
  }

  bool hasDefaultCopyAssignType(const SgMemberFunctionDeclaration& mfn, ct::ClassKeyType clkey)
  {
    return hasDefaultAssignType(mfn, clkey, isCopyParameterType);
  }

  bool hasDefaultMoveAssignType(const SgMemberFunctionDeclaration& mfn, ct::ClassKeyType clkey)
  {
    return hasDefaultAssignType(mfn, clkey, isMoveParameterType);
  }

  bool canGenerateDefaultCopyAssign(const ct::ClassAnalysis::mapped_type& clazz)
  {
    const ct::SpecialMemberFunctionContainer& specials = clazz.specialMemberFunctions();
    auto       isCopyAssign = [](const ct::SpecialMemberFunction& smf) -> bool { return smf.isCopyAssign(); };
    auto const lim          = specials.end();
    auto const pos          = std::find_if(specials.begin(), lim, isCopyAssign);

    // the default copy constructor can be generated if it can be generated but has not been provided
    //   either by the frontend or by the user.
    return (pos != lim) && (pos->function() == ct::FunctionKeyType{});
  }

  bool canGenerateDefaultMoveAssign(const ct::ClassAnalysis::mapped_type& clazz)
  {
    const ct::SpecialMemberFunctionContainer& specials = clazz.specialMemberFunctions();
    auto       isMoveAssign = [](const ct::SpecialMemberFunction& smf) -> bool { return smf.isMoveAssign(); };
    auto const lim          = specials.end();
    auto const pos          = std::find_if(specials.begin(), lim, isMoveAssign);

    // the default move constructor can be generated if it can be generated but has not been provided
    //   either by the frontend or by the user.
    return (pos != lim) && (pos->function() == ct::FunctionKeyType{});
  }

}

namespace CodeThorn
{

bool
RoseCompatibilityBridge::isAutoGeneratable(const ClassAnalysis& all, ClassKeyType clkey, FunctionKeyType fnkey) const
{
  ASSERT_require(clkey != ClassKeyType{});
  ASSERT_require(fnkey != FunctionKeyType{});

  // only member functions that are not available already can be auto-generated
  const SgMemberFunctionDeclaration* memfn = isSgMemberFunctionDeclaration(fnkey);
  if ((memfn == nullptr) || (clkey == &getClassDefForFunction(*memfn))) return false;

  bool                             res   = false;
  const SgSpecialFunctionModifier& fnmod = memfn->get_specialFunctionModifier();

  if (fnmod.isDestructor())
    res = !hasDestructor(all.at(clkey));
  else if (fnmod.isConstructor())
    res = !hasConstructor(all.at(clkey), *memfn);
  else if (memfn->get_name() != "operator=") // \todo consider spaceship operator
    res = false;
  else if (hasDefaultCopyAssignType(*memfn, clkey))
    res = canGenerateDefaultCopyAssign(all.at(clkey));
  else if (hasDefaultMoveAssignType(*memfn, clkey))
    res = canGenerateDefaultMoveAssign(all.at(clkey));

  return res;
}

bool
RoseCompatibilityBridge::isAbstract(ClassKeyType clkey) const
{
  ASSERT_require(clkey != ClassKeyType{});

  return clkey->get_isAbstract();
}

DataMemberType
RoseCompatibilityBridge::typeOf(VariableKeyType var) const
{
  sg::NotNull<const SgType> ty      = var->get_type();
  sg::NotNull<const SgType> unRefTy = removeReference(ty);
  sg::NotNull<const SgType> unArrTy = removeArray(unRefTy);
  CvQualification           unCvTy  = removeCvQual(unArrTy);
  const SgClassDefinition*  clazz   = nullptr;

  if (const SgClassType* clazzty = isSgClassType(unCvTy.type()))
    clazz = getClassDefOpt(*clazzty);

  // std::cerr << "u" << (unCvTy.type() ? typeid(*unCvTy.type()).name() : std::string{"null"}) << std::endl;

  const bool                isConst = unCvTy.hasConst();
  const bool                hasRef  = ty != unRefTy;
  const bool                hasArr  = unRefTy != unArrTy;
  const bool                hasInit = var->get_initializer() != nullptr;

  return { clazz, isConst, hasRef, hasArr, hasInit };
}


SgClassDefinition& getClassDef(const SgDeclarationStatement& n)
{
  SgDeclarationStatement* defdcl = n.get_definingDeclaration();
  SgClassDeclaration&     clsdef = SG_DEREF(isSgClassDeclaration(defdcl));

  return SG_DEREF(clsdef.get_definition());
}

SgClassDefinition& getClassDefForFunction(const SgMemberFunctionDeclaration& n)
{
  return getClassDef(SG_DEREF(n.get_associatedClassDeclaration()));
}


SgClassDefinition* getClassDefOpt(const SgClassType& n)
{
  if (const SgClassDefinition* res = classDefinition_opt(n))
    return const_cast<SgClassDefinition*>(res);

  msgWarn() << "class type without class declaration.." << std::endl;
  return nullptr;
}


SgClassDefinition* getClassDefOpt(const SgInitializedName& n)
{
  SgType&            ty  = SG_DEREF(n.get_type());
  const SgClassType* cls = classtypeUnderType(ty, SgType::STRIP_ARRAY_TYPE);

  return (cls == nullptr) ? nullptr : getClassDefOpt(*cls);
}


SgFunctionDeclaration& keyDecl(SgFunctionDeclaration& fn)
{
  SgFunctionDeclaration* fKey = isSgFunctionDeclaration(fn.get_firstNondefiningDeclaration());
  SgFunctionDeclaration* res  = fKey ? fKey : &fn;

/*
  if (isVirtual(*res) != isVirtual(fn))
  {
    msgError() << "** " << fn.get_name() << " @" << &fn << " - def: " << fn.get_definition() << "   virt: " << isVirtual(fn)
               << "\n   " << res->get_name() << " @" << res << " - def: " << res->get_definition() << "   virt: " << isVirtual(*res)
               << "\n   " << typeid(fn).name()
               << std::endl;

    logPathToGlobal(msgError(), res);
  }
*/

  ASSERT_require(!isVirtual(fn) || isVirtual(*res));
  return *res;
}

namespace
{
  struct AnalyseCallExp : sg::DispatchHandler<CallData>
  {
      AnalyseCallExp(const ct::FunctionPredicate& virtualFunctionTest, const SgExpression& ex)
      : Base(), isVirtualFunction(virtualFunctionTest), ref(&ex)
      {}

      CallData descend(const SgNode* n) const;

      CallData normalCall(const SgFunctionRefExp& n) const;
      CallData objectCall(const SgBinaryOp& n);
      CallData objectOrLambdaCall(const SgDotExp& n);
      //~ CallData memPtrCall(const SgBinaryOp& n) const;
      CallData memberCall(const SgMemberFunctionRefExp& n) const;
      //~ CallData arrPtrCall(const SgPntrArrRefExp& n) const;
      CallData unresolvable(const SgExpression& n) const;

      void handle(const SgNode& n)                   { SG_UNEXPECTED_NODE(n); }

      void handle(const SgExpression& n)             { res = unresolvable(n); }

      void handle(const SgCommaOpExp& n)             { res = descend(n.get_rhs_operand()); }

      void handle(const SgDotStarOp& n)              { res = objectCall(n); }
      void handle(const SgArrowStarOp& n)            { res = objectCall(n); }

      void handle(const SgDotExp& n)                 { res = objectOrLambdaCall(n); }
      void handle(const SgArrowExp& n)               { res = objectCall(n); }

      void handle(const SgMemberFunctionRefExp& n)   { res = memberCall(n); }
      void handle(const SgFunctionRefExp& n)         { res = normalCall(n); }

      void handle(const SgCastExp& n)                { res = descend(n.get_operand()); }

      //
      // incomplete
      //~ void handle(const SgConstructorInitializer& n) { res = castCall{n}; }

      //~ void handle(const SgPointerDerefExp& n)        { res = pointerCall{n}; }
      //~ void handle(const SgPntrArrRefExp& n)          { res = arrPtrCall{n}; }
      //~ void handle(const SgCastExp& n)                { res = castCall{n}; }
      //~ void handle(const SgFunctionCallExp& n)        { res = castCall{n}; }

      //
      // templates are not handled
      //~ void handle(const SgNonrealRefExp& n)          { res = castCall{n}; }
      //~ void handle(const SgTemplateMemberFunctionRefExp& n)          { res = castCall{n}; }
      //~ void handle(const SgTemplateFunctionRefExp& n)          { res = castCall{n}; }

    private:
      const ct::FunctionPredicate& isVirtualFunction;
      const SgExpression*          ref                 = nullptr;
      Optional<ClassKeyType>       receiverKey         = {};
      bool                         polymorphicReceiver = false; // or Optional to distinguish between set/unset
  };

  CallData
  AnalyseCallExp::normalCall(const SgFunctionRefExp& n) const
  {
    RoseCompatibilityBridge      compat;
    const SgFunctionDeclaration* fn = n.getAssociatedFunctionDeclaration();

    ASSERT_require(!receiverKey);
    return { compat.functionId(fn), &n, receiverKey, ref, false };
  }

  CallData
  AnalyseCallExp::objectOrLambdaCall(const SgDotExp& n)
  {
    if (isSgLambdaExp(n.get_lhs_operand()))
    {
      receiverKey         = nullptr; // \todo set the lambda type
      polymorphicReceiver = false;

      return descend(n.get_rhs_operand());
    }

    if (/*const SgConstructorInitializer* init =*/ isSgConstructorInitializer(n.get_lhs_operand()))
    {
      receiverKey         = nullptr; // \todo set the class type
      polymorphicReceiver = false;

      return descend(n.get_rhs_operand());
    }

    return objectCall(n);
  }


  CallData
  AnalyseCallExp::objectCall(const SgBinaryOp& n)
  {
    // only relevant for pointer calls; maybe these could be passed down...
    const unsigned char             STRIP_PTRS = SgType::STRIP_POINTER_TYPE | SgType::STRIP_ARRAY_TYPE;

    // lhs is the receiver expression
    sg::NotNull<const SgExpression> receiverExpr = n.get_lhs_operand();
    // sg::NotNull<const SgClassType>  receiverClss = classtypeOfExpr(*receiverExpr);
    const SgClassType*              receiverClss = classtypeOfExpr(*receiverExpr, STRIP_PTRS);

    if (!receiverClss)
    {
      msgError() << "unable to find class-type in lhs of " << n.unparseToString()
                 << " w/ expr-type " << typeid(*receiverExpr).name()
                 << " + type " << typeid(typeOfExpr(*receiverExpr)).name()
                 << std::endl;
      throw std::logic_error{"unable to find class-type - AnalyseCallExp::objectCall"};
    }

    receiverKey = classDefinition_opt(*receiverClss);
    ASSERT_require(receiverKey && *receiverKey);

    SgType* const                   receiverBaseType = typeOfExpr(*receiverExpr).stripTypedefsAndModifiers();

    polymorphicReceiver = (  // objects
                             isSgReferenceType(receiverBaseType)
                          || isSgRvalueReferenceType(receiverBaseType)
                             // pointers when the parent is an arrow expression
                          || isSgPointerType(receiverBaseType)
                          || isSgArrayType(receiverBaseType)
                          );

    return descend(n.get_rhs_operand());
  }

  CallData
  AnalyseCallExp::memberCall(const SgMemberFunctionRefExp& n) const
  {
    const SgMemberFunctionDeclaration* mfn     = n.getAssociatedMemberFunctionDeclaration();
    const FunctionKeyType              mfnkey  = RoseCompatibilityBridge{}.functionId(mfn);
    const bool                         virtualCall = (  polymorphicReceiver
                                                     && (n.get_need_qualifier() == 0)
                                                     && isVirtualFunction(mfnkey)
                                                     );

    Optional<ClassKeyType>             keyCopy = receiverKey;

    // if not set from context
    // \todo check that receiverKey matches if already set.
    if (!keyCopy)
      keyCopy = &getClassDefForFunction(SG_DEREF(mfn));

    ASSERT_require(keyCopy);
    return { mfnkey, &n, keyCopy, ref, virtualCall };
  }

  bool
  pointsToMemberFunction(const SgExpression& n)
  {
    return isSgMemberFunctionType(&typeOfExpr(n));
  }

  CallData
  AnalyseCallExp::unresolvable(const SgExpression& n) const
  {
    const bool polymorphicCall = polymorphicReceiver && pointsToMemberFunction(n);

    // virtualCall is conservative, as the runtime state of the
    //   pointer used for dispatch could point to a non-virtual function.
    return { {}, &n, receiverKey, ref, polymorphicCall };
  }

  CallData
  AnalyseCallExp::descend(const SgNode* n) const
  {
    return sg::dispatch(*this, n);
  }

  CallData
  analyseCallExp(const SgFunctionCallExp& n, const ct::FunctionPredicate& virtualFunctionTest)
  {
    return sg::dispatch(AnalyseCallExp{virtualFunctionTest, n}, n.get_function());
  }

  CallData
  analyseCallExp(const SgConstructorInitializer& n, const ct::FunctionPredicate&)
  {
    RoseCompatibilityBridge            compat;
    const SgMemberFunctionDeclaration* mfn = n.get_declaration();
    Optional<FunctionKeyType>          fnkey;

    if (mfn) fnkey = compat.functionId(mfn);

    Optional<ClassKeyType>             clskey;
    const SgClassDeclaration*          clsdcl = n.get_class_decl();

    if (clsdcl) clskey = &getClassDef(*clsdcl);
    else
    {
      msgError() << "NO class in: " << n.get_parent()->unparseToString() << std::endl;

      logPathToGlobal(msgError(), &n);
    }

    return { fnkey, &n, clskey, &n, false };
  }

  struct CallDataFinder : sg::DispatchHandler< std::vector<CallData> >, ExcludeTemplates
  {
      CallDataFinder(ReturnType vec, bool ignoreFunctionRefExp, sg::NotNull<ct::FunctionPredicate> virtualFunctionTest)
      : Base(std::move(vec)), ExcludeTemplates(),
        ignoreFunctionRefs(ignoreFunctionRefExp),
        isVirtualFunction(virtualFunctionTest)
      {}

      explicit
      CallDataFinder(sg::NotNull<ct::FunctionPredicate> virtualFunctionTest)
      : Base(), ExcludeTemplates(), ignoreFunctionRefs(false), isVirtualFunction(virtualFunctionTest)
      {}

      CallDataFinder(CallDataFinder&&)            = default;
      CallDataFinder& operator=(CallDataFinder&&) = default;
      ~CallDataFinder()                           = default;

      void descend(SgNode& n, bool ignoreFunctionRefExp = false);
      void handleNode(SgNode* n, bool ignoreFunctionRefExp = false);
      void handleFnRefExp( const SgFunctionDeclaration& dcl,
                           SgExpression& n,
                           Optional<ClassKeyType> typeBound,
                           bool isVirtual
                         );

      template <class SageExpression>
      void handleCallExp(SageExpression& n)
      {
        std::cerr << "emplace call  " << n.unparseToString() << std::endl;
        res.emplace_back(analyseCallExp(n, *isVirtualFunction));

        descend(n, true);
      }

      void handleSelection(SgBinaryOp&);

      using ExcludeTemplates::handle;

      void handle(SgNode& n)                   { descend(n); }

      void handle(SgFunctionCallExp& n)        { handleCallExp(n); }
      void handle(SgConstructorInitializer& n) { handleCallExp(n); }

      void handle(SgFunctionRefExp& n)
      {
        sg::NotNull<SgFunctionDeclaration> dcl = n.getAssociatedFunctionDeclaration();

        handleFnRefExp(*dcl, n, {}, false);
      }

      void handle(SgMemberFunctionRefExp& n)
      {
        sg::NotNull<SgMemberFunctionDeclaration> dcl = n.getAssociatedMemberFunctionDeclaration();
        sg::NotNull<SgDeclarationStatement>      cls = dcl->get_associatedClassDeclaration();
        ClassKeyType                             key = classDefinition_opt(*cls);

        ASSERT_require(key != ClassKeyType{});
        handleFnRefExp(*dcl, n, key, isVirtual(*dcl)); // todo
      }

      void handle(SgCommaOpExp& n)
      {
        handleNode(n.get_lhs_operand(), true /* no effect on comma's lhs */);
        handleNode(n.get_rhs_operand(), ignoreFunctionRefs);
      }

      void handle(SgDotExp& n)      { handleSelection(n); }
      void handle(SgArrowExp& n)    { handleSelection(n); }
      void handle(SgDotStarOp& n)   { handleSelection(n); }
      void handle(SgArrowStarOp& n) { handleSelection(n); }

      void handle(SgAddressOfOp& n)
      {
        handleNode(n.get_operand(), ignoreFunctionRefs);
      }

    private:
      bool                               ignoreFunctionRefs = false;
      sg::NotNull<ct::FunctionPredicate> isVirtualFunction;

      CallDataFinder()                                 = delete;
      CallDataFinder(const CallDataFinder&)            = delete;
      CallDataFinder& operator=(const CallDataFinder&) = delete;
  };

  void
  CallDataFinder::descend(SgNode& n, bool ignoreFunctionRefExp)
  {
    res = sg::traverseDispatchedChildren( CallDataFinder{ std::move(res), ignoreFunctionRefExp, isVirtualFunction },
                                          n
                                        );
  }

  void
  CallDataFinder::handleNode(SgNode* n, bool ignoreFunctionRefExp)
  {
    res = sg::dispatch( CallDataFinder{ std::move(res), ignoreFunctionRefExp, isVirtualFunction },
                        n
                      );
  }

  void
  CallDataFinder::handleSelection(SgBinaryOp& n)
  {
    // in selection operators like x.f() or (x->*f)() we ignore the function refs
    //   if they appear in a call expression.
    handleNode(n.get_lhs_operand(), false);
    handleNode(n.get_rhs_operand(), ignoreFunctionRefs);
  }

  void
  CallDataFinder::handleFnRefExp(const SgFunctionDeclaration& dcl, SgExpression& n, Optional<ClassKeyType> typeBound, bool isVirtual)
  {
    if (ignoreFunctionRefs) return;

    RoseCompatibilityBridge compat;
    FunctionKeyType         key = compat.functionId(&dcl);

    ASSERT_require(key);
    std::cerr << "emplace fnref " << n.unparseToString() << std::endl;
    res.emplace_back( key, &n, typeBound, nullptr, isVirtual );
  }

  std::vector<CallData>
  functionRelations_internal(const SgFunctionDeclaration* fn, ct::FunctionPredicate isVirtualFunction)
  {
    SgFunctionDeclaration* fndef = isSgFunctionDeclaration(fn->get_definingDeclaration());
    if (fndef == nullptr) return {};

    return sg::dispatch(CallDataFinder{&isVirtualFunction}, fndef);
  }
}

std::vector<CallData>
RoseCompatibilityBridge::functionRelations(FunctionKeyType fn, FunctionPredicate isVirtualFunction) const
{
  return functionRelations_internal(fn, std::move(isVirtualFunction));
}

}


namespace
{
  template <class Fn, class BranchGenerator = sg::DefaultTraversalSuccessors>
  Fn
  traverseBranches(Fn fn, const SgNode& root, BranchGenerator gen = {})
  {
    auto nodeRange = gen(root);

    return std::for_each(nodeRange.begin(), nodeRange.end(), std::move(fn));
  }

  template <class Functor>
  struct UnorderedTraversal
  {
      // no explicit
      UnorderedTraversal(Functor astfun) : fn(std::move(astfun)) {}

      UnorderedTraversal(UnorderedTraversal&&)            = default;
      UnorderedTraversal& operator=(UnorderedTraversal&&) = default;

      void traverse(const SgNode& n, std::true_type)
      {
        fn = traverseBranches(UnorderedTraversal{std::move(fn)}, n);
      }

      void traverse(const SgNode& n, std::false_type)
      {
        traverseBranches(UnorderedTraversal{fn}, n);
      }

      void operator()(const SgNode* n)
      {
        if (ExcludeTemplates::templated(n))
          return;

        fn(n);

        if (n)
          traverse(*n, typename std::is_move_assignable<Functor>::type{} );
      }

      operator Functor() && { return std::move(fn); }

    private:
      Functor fn;

      UnorderedTraversal()                                     = delete;
      UnorderedTraversal(const UnorderedTraversal&)            = delete;
      UnorderedTraversal& operator=(const UnorderedTraversal&) = delete;
  };

  template <class Functor>
  Functor
  unorderedTraversal0(Functor astfun, const SgNode* root)
  {
    UnorderedTraversal<Functor> fn{std::move(astfun)};
    fn(root);

    return fn;
  }

  struct CategorizeFunctions
  {
      explicit
      CategorizeFunctions(std::unordered_set<ct::FunctionKeyType> userDefined)
      : userDefinedFunctions(userDefined)
      {}

      ct::SpecialMemberFunction
      none(ct::FunctionKeyType key) const
      {
        return { key, ct::SpecialMemberFunction::notspecial, false, false, true };
      }

      ct::SpecialMemberFunction
      special(const SgMemberFunctionDeclaration& mfn, std::uint_fast8_t knd, bool copyWithConstRef = false, bool compliant = true) const
      {
        ct::RoseCompatibilityBridge compat;
        ct::FunctionKeyType         key     = compat.functionId(&mfn);
        const auto                  pos     = userDefinedFunctions.find(key);
        const bool                  compGen = pos == userDefinedFunctions.end();

        // should we also check for mfn.isCompilerGenerated()
        //   does not always seem to be set.
        return { &mfn, knd, compGen, copyWithConstRef, compliant };
      }

      // returns false for non-conforming copy assignments, such as operator=(const T)
      bool compliantSignature(const SgMemberFunctionDeclaration& mfn, std::uint_fast8_t knd) const
      {
        constexpr std::uint_fast8_t cassign = ct::SpecialMemberFunction::cassign;

        if ((knd & cassign) != cassign)
          return true;

        ParameterTypeDesc paramDescription = specialMemberFunctionParameter(mfn);

        if (paramDescription.withReference())
          return true;

        return !(paramDescription.withConst() || paramDescription.withVolatile());
      }

      // returns true if this is a copy assignment or copy constructor callable with const T&
      bool copyCallableWithConstRef(const SgMemberFunctionDeclaration& mfn, std::uint_fast8_t knd) const
      {
        constexpr std::uint_fast8_t cassign = ct::SpecialMemberFunction::cassign;
        constexpr std::uint_fast8_t cctor   = ct::SpecialMemberFunction::cctor;

        const bool copyAssignOrCtor = ((knd & cassign) == cassign) || ((knd & cctor) == cctor);

        if (!copyAssignOrCtor)
          return false;

        ParameterTypeDesc paramDescription = specialMemberFunctionParameter(mfn);

        // \todo check: a copy assignment taking a possibly cv qualified T and when
        //              the class offers no cctor. In this case, the copy cannot be
        //              constructed, ..
        if (!paramDescription.withReference())
          return true;

        return paramDescription.withConst();
      }


      std::uint_fast8_t
      chooseKind(const SgMemberFunctionDeclaration& fn, std::uint_fast8_t cp, std::uint_fast8_t mv) const
      {
        // check whether the first argument is either for copy ctor/assign or move ctor/assign or neither.
        ParameterTypeDesc paramDescription = specialMemberFunctionParameter(fn);

        if (isMoveParameterType(paramDescription))
          return mv;

        if (isCopyParameterType(paramDescription))
          return cp;

        return ct::SpecialMemberFunction::notspecial;
      }

      ct::SpecialMemberFunction
      categorize(const SgMemberFunctionSymbol* sym) const
      {
        if (sym == nullptr)
          return none(nullptr);

        ct::CompatibilityBridge            compat;
        const SgMemberFunctionDeclaration& fn = SG_DEREF(sym->get_declaration());

        if (fn.get_specialFunctionModifier().isDestructor())
          return special(fn, ct::SpecialMemberFunction::dtor);

        std::size_t                        maxargs = 0;
        std::size_t                        minargs = 0;

        std::tie(minargs, maxargs) = countArguments(fn);

        if (fn.get_specialFunctionModifier().isConstructor())
        {
          std::uint_fast8_t knd = ct::SpecialMemberFunction::ctor;

          if (minargs == 0)                    knd |= ct::SpecialMemberFunction::dctor;
          if ((minargs <= 1) && (maxargs > 0)) knd |= chooseKind(fn, ct::SpecialMemberFunction::cctor, ct::SpecialMemberFunction::mctor);

          return special(fn, knd, copyCallableWithConstRef(fn, knd));
        }

        if (fn.get_name() == "operator=")
        {
          std::uint_fast8_t knd = ct::SpecialMemberFunction::notspecial;

          if (maxargs == 1)
            knd |= chooseKind(fn, ct::SpecialMemberFunction::cassign, ct::SpecialMemberFunction::massign);

          return special(fn, knd, copyCallableWithConstRef(fn, knd), compliantSignature(fn, knd));
        }

        return none(&fn);
      }

      ct::SpecialMemberFunction
      operator()(const SgSymbol& sym) const
      {
        return categorize(isSgMemberFunctionSymbol(&sym));
      }

    private:
      std::unordered_set<ct::FunctionKeyType> userDefinedFunctions;
  };

  struct AsMemberFunction
  {
    const SgMemberFunctionDeclaration*
    operator()(const SgDeclarationStatement* dcl) const
    {
      return isSgMemberFunctionDeclaration(dcl);
    }
  };

  struct ToFunctionID
  {
    ct::FunctionKeyType
    operator()(const SgMemberFunctionDeclaration* key) const
    {
      return ct::RoseCompatibilityBridge{}.functionId(key);
    }
  };

  std::unordered_set<ct::FunctionKeyType>
  userDefinedFunctions(ct::ClassKeyType clazz)
  {
    auto range = clazz->get_members() | adapt::transformed(AsMemberFunction{})
                                      | adapt::filtered(ct::IsNotNull{})
                                      | adapt::transformed(ToFunctionID{})
                                      ;

    return std::unordered_set<ct::FunctionKeyType>(range.begin(), range.end());
  }
}

namespace CodeThorn
{

std::vector<FunctionKeyType>
RoseCompatibilityBridge::allFunctionKeys(ASTRootType n) const
{
  std::set<FunctionKeyType> fnkeys;
  std::size_t               nc = 0;

  auto collectFunctionKeys =
         [&fnkeys, &nc](const SgNode* n) -> void
         {
           ++nc;

           if (const SgFunctionDeclaration* fn = isSgFunctionDeclaration(n))
             fnkeys.insert(RoseCompatibilityBridge{}.functionId(fn));
         };

  unorderedTraversal0(collectFunctionKeys, n);

  SAWYER_MESG(msgInfo())
            << "Traversed nodes/Unique functions " << nc << "/" << fnkeys.size()
            << std::endl;

  return std::vector<FunctionKeyType>(fnkeys.begin(), fnkeys.end());
}


FunctionPredicate
RoseCompatibilityBridge::functionNamePredicate(std::string name) const
{
  return [nm=std::move(name)](const SgFunctionDeclaration* fn) -> bool
         {
           return fn && (nm == fn->get_name().getString());
         };
}

std::vector<FunctionKeyType>
RoseCompatibilityBridge::allFunctionKeys(ASTRootType n, FunctionPredicate pred) const
{
  std::set<FunctionKeyType> fnkeys;

  auto collectFunctionKeys =
         [&fnkeys, predicate=std::move(pred)](const SgNode* n) -> void
         {
           if (const SgFunctionDeclaration* fn = isSgFunctionDeclaration(n))
             if (predicate(fn))
               fnkeys.insert(RoseCompatibilityBridge{}.functionId(fn));
         };

  unorderedTraversal0(collectFunctionKeys, n);
  return std::vector<FunctionKeyType>(fnkeys.begin(), fnkeys.end());
}


ct::SpecialMemberFunctionContainer
RoseCompatibilityBridge::specialMemberFunctions(ct::ClassKeyType clazz) const
{
  ASSERT_require(clazz != ct::ClassKeyType{});

  // compiler generated special member functions are not always marked
  //   as compiler generated. Thus we cross-check with the member functions
  //   that appear on the class member list.
  std::unordered_set<FunctionKeyType> knownUserFunctions = userDefinedFunctions(clazz);

  auto const beg = symbolBegin(*clazz);
  auto const lim = symbolLimit(*clazz);

  auto onlySpecial = [](const SpecialMemberFunction& el) -> bool { return el.isSpecialFunc(); };
  auto resultRange = SymbolRange{beg, lim} | adapt::transformed(CategorizeFunctions{std::move(knownUserFunctions)})
                                           | adapt::filtered(onlySpecial);

  return ct::SpecialMemberFunctionContainer(resultRange.begin(), resultRange.end());
}

Optional<ClassKeyType>
RoseCompatibilityBridge::classType(FunctionKeyType fn) const
{
  const SgMemberFunctionDeclaration* mfn = isSgMemberFunctionDeclaration(fn);
  if (mfn == nullptr) return {};

  return &getClassDefForFunction(*mfn);
}


//
// SpecialMemberFunction

bool SpecialMemberFunction::isSpecialFunc() const { return kind()             != notspecial; }
bool SpecialMemberFunction::isConstructor() const { return (kind() & ctor)    == ctor; }
bool SpecialMemberFunction::isDefaultCtor() const { return (kind() & dctor)   == dctor; }
bool SpecialMemberFunction::isCopyCtor()    const { return (kind() & cctor)   == cctor; }
bool SpecialMemberFunction::isMoveCtor()    const { return (kind() & mctor)   == mctor; }
bool SpecialMemberFunction::isDestructor()  const { return (kind() & dtor)    == dtor; }
bool SpecialMemberFunction::isCopyAssign()  const { return (kind() & cassign) == cassign; }
bool SpecialMemberFunction::isMoveAssign()  const { return (kind() & massign) == massign; }

}

