#include "sage3basic.h"
#include "sageBuilder.h"
#include "sageGeneric.h"
#include "sageInterface.h"
#include "RoseAst.h"

#include <memory>
#include <boost/range/adaptor/reversed.hpp>

#include "Normalization.h"
#include "NormalizationCxx.h"

namespace sb = SageBuilder;
namespace si = SageInterface;
namespace ct = CodeThorn;
namespace adapt = boost::adaptors;

namespace CodeThorn
{
namespace
{
  constexpr unsigned char STRIP_MODIFIER_ALIAS = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_TYPEDEF_TYPE;

  struct CxxTransformStats
  {
    int cnt = 0;
  };

  std::ostream& operator<<(std::ostream& os, const CxxTransformStats& stat)
  {
    return os << stat.cnt;
  }

  // internal use
  void normalizeCtorDtor(SgNode* root, CxxTransformStats& stats);

  constexpr bool ERROR_TOLERANT = true;

  // auxiliary wrapper for printing Sg_File_Info objects
  struct SrcLoc
  {
    explicit
    SrcLoc(SgLocatedNode& n)
    : info(n.get_file_info())
    {}

    Sg_File_Info* info;
  };

  std::ostream& operator<<(std::ostream& os, SrcLoc el)
  {
    return os << el.info->get_filenameString()
              << "@" << el.info->get_line() << ":" << el.info->get_col();
  }

  SgExpression* elemAt(SgExprListExp& lst, int pos)
  {
    return lst.get_expressions().at(pos);
  }

  SgExpression* elemAt(SgExprListExp* lst, int pos)
  {
    return elemAt(SG_DEREF(lst), pos);
  }


  // \brief handlers to skip template processing
  struct ExcludeTemplates
  {
    void handle(SgTemplateClassDeclaration&)          {}
    void handle(SgTemplateClassDefinition&)           {}
    void handle(SgTemplateFunctionDeclaration&)       {}
    void handle(SgTemplateFunctionDefinition&)        {}
    void handle(SgTemplateMemberFunctionDeclaration&) {}
    void handle(SgTemplateTypedefDeclaration&)        {}
    void handle(SgTemplateVariableDeclaration&)       {}
  };

  //
  // constants
  static const std::string DTOR_PREFIX("~");

  //
  // logging

  inline
  auto logInfo() -> decltype(Normalization::logger[Sawyer::Message::INFO])
  {
    return Normalization::logger[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(Normalization::logger[Sawyer::Message::WARN])
  {
    return Normalization::logger[Sawyer::Message::WARN];
  }

  inline
  auto logError() -> decltype(Normalization::logger[Sawyer::Message::ERROR])
  {
    return Normalization::logger[Sawyer::Message::ERROR];
  }

  inline
  auto logTrace() -> decltype(Normalization::logger[Sawyer::Message::TRACE])
  {
    return Normalization::logger[Sawyer::Message::TRACE];
  }

  //
  // convenience functions

  /// returns the only SgInitializedName object of a variable declaration
  SgInitializedName& onlyName(const SgVariableDeclaration& n)
  {
    const SgInitializedNamePtrList& lst = n.get_variables();

    ROSE_ASSERT(lst.size() == 1 && lst[0]);
    return *lst[0];
  }


  //
  // transformation wrappers

  // borrowed from XPlacer
  struct BaseTransform
  {
      virtual ~BaseTransform() = default;
      BaseTransform()          = default;

      virtual void execute(CxxTransformStats& stat) = 0;

    private:
      BaseTransform(const BaseTransform&)            = delete;
      BaseTransform(BaseTransform&&)                 = delete;
      BaseTransform& operator=(const BaseTransform&) = delete;
      BaseTransform& operator=(BaseTransform&&)      = delete;
  };

  /// Polymorphic wrapper for concrete actions.
  template <class ConcreteTransform>
  struct PolyTransform : BaseTransform
  {
      explicit
      PolyTransform(ConcreteTransform&& concreteTf)
      : tf(std::move(concreteTf))
      {}

      void execute(CxxTransformStats& stat) ROSE_OVERRIDE
      {
        tf.execute(stat);
      }

    private:
      ConcreteTransform tf;

      PolyTransform()                                = delete;
      PolyTransform(const PolyTransform&)            = delete;
      PolyTransform(PolyTransform&&)                 = delete;
      PolyTransform& operator=(PolyTransform&&)      = delete;
      PolyTransform& operator=(const PolyTransform&) = delete;
  };

  /// Generic walue wrapper around polymorphic actions.
  struct AnyTransform
  {
      template <class ConcreteTransform>
      AnyTransform(ConcreteTransform&& a)
      : tf(new PolyTransform<ConcreteTransform>(std::move(a)))
      {}

      AnyTransform(AnyTransform&& other)            = default;
      AnyTransform& operator=(AnyTransform&& other) = default;
      ~AnyTransform()                               = default;

      void execute(CxxTransformStats& stat) { tf->execute(stat); }

    private:
      std::unique_ptr<BaseTransform> tf;

      // copy ctor + assignment
      AnyTransform(const AnyTransform& other)            = delete;
      AnyTransform& operator=(const AnyTransform& other) = delete;
  };

  typedef std::vector<AnyTransform> transformation_container;

  // end transformations

  //
  // functor/function decorator to memoize results


  /// \private
  template <class Fn>
  struct fn_traits : fn_traits<decltype(&Fn::operator())> { /* use overloads */ };

  /// \private
  /// for const member operator() and non-mutable lambda's
  template <class R, class C, class... Args>
  struct fn_traits<R (C::*) (Args...) const>
  {
    typedef std::tuple<Args...> arguments_t;
    typedef R                   result_t;
  };

  /// \private
  /// for non-const member operator() and mutable lambda's
  template <class R, class C, class... Args>
  struct fn_traits<R (C::*) (Args...)>
  {
    typedef std::tuple<Args...> arguments_t;
    typedef R                   result_t;
  };

  /// \private
  /// for freestanding functions
  template <class R, class... Args>
  struct fn_traits<R (*) (Args...)>
  {
    typedef std::tuple<Args...> arguments_t;
    typedef R                   result_t;
  };

  /// \brief   decorator on functions to cache and reuse results
  /// \details On the first invocation with a set of arguments, the result
  ///          is computed and memoized. On later invocations, the memoized
  ///          result is returned.
  /// \tparam Fn the type of the function or functor
  /// \todo unordered_map may be faster
  template <class Fn>
  struct Memoizer
  {
      typedef Fn                                      func_t;
      typedef typename fn_traits<func_t>::result_t    result_t;
      typedef typename fn_traits<func_t>::arguments_t arguments_t;
      typedef std::map<arguments_t, result_t>         result_cache_t;

      explicit
      Memoizer(Fn f)
      : func(f)
      {}

      Memoizer()                           = default;
      Memoizer(const Memoizer&)            = default;
      Memoizer(Memoizer&&)                 = default;
      Memoizer& operator=(Memoizer&&)      = default;
      Memoizer& operator=(const Memoizer&) = default;

      /// \tparam Args an argument pack consisting of less-than comparable components
      /// \param  args the arguments to func
      /// \return the result of calling func(args...)
      template <class... Args>
      result_t& operator()(Args... args)
      {
        typedef typename result_cache_t::iterator cache_iterator;

        cache_iterator pos = cache.find(std::tie(args...));

        if (pos != cache.end())
        {
          ++num_hits;
          return pos->second;
        }

        arguments_t desc(args...);

        return cache.emplace(std::move(desc), func(std::forward<Args...>(args)...)).first->second;
      }

      void clear() { cache.clear(); }

      size_t size() const { return cache.size(); }
      size_t hits() const { return num_hits; }

    private:
      size_t         num_hits = 0;
      func_t         func;
      result_cache_t cache;
  };

  template <class Fn>
  std::ostream& operator<<(std::ostream& os, const Memoizer<Fn>& memo)
  {
    return os << memo.hits()
              << " <hits -- size> "
              << memo.size();
  }

  template <class Fn>
  inline
  Memoizer<Fn> memoizer(Fn fn)
  {
    return Memoizer<Fn>(fn);
  }

  // end memoization wrapper

  //
  // convenience functions + functors

  SgBasicBlock& getCtorBody(SgMemberFunctionDeclaration& n)
  {
    SgFunctionDefinition& def = SG_DEREF(n.get_definition());

    return SG_DEREF(def.get_body());
  }

  SgClassDefinition& getClassDef(SgDeclarationStatement& n)
  {
    SgDeclarationStatement& defdcl = SG_DEREF(n.get_definingDeclaration());
    SgClassDeclaration&     clsdef = SG_ASSERT_TYPE(SgClassDeclaration, defdcl);

    return SG_DEREF(clsdef.get_definition());
  }

  SgClassDefinition& getClassDef(SgMemberFunctionDeclaration& n)
  {
    return getClassDef(SG_DEREF(n.get_associatedClassDeclaration()));
  }

  SgClassDefinition* getClassDefOpt(SgClassType& n)
  {
    SgDeclarationStatement& dcl    = SG_DEREF( n.get_declaration() );
    SgDeclarationStatement* defdcl = dcl.get_definingDeclaration();

    logWarn() << dcl.get_mangled_name() << std::endl;

    return defdcl ? SG_ASSERT_TYPE(SgClassDeclaration, *defdcl).get_definition()
                  : nullptr
                  ;
  }


  /// a compiler generated destructor is required, if
  ///   (1) no destructor has been specified
  ///   (2) AND at least one data member has or requires a non-trivial destructor.
  struct TriviallyDestructible : sg::DispatchHandler<bool>
  {
    typedef sg::DispatchHandler<bool> base;

    TriviallyDestructible()
    : base(true)
    {}

    void handle(SgNode& n)         { SG_UNEXPECTED_NODE(n); }

    // base case
    void handle(SgType&)           { /* default */ }

    // types with constructors/destructors
    void handle(SgClassType& n)
    {
      SgClassDefinition* clsdef = getClassDefOpt(n);

      res = clsdef ? check(clsdef).first : false;
    }

    // types that need to be skipped
    void handle(SgArrayType& n)    { res = check(n.get_base_type()); }
    void handle(SgTypedefType& n)  { res = check(n.get_base_type()); }
    void handle(SgModifierType& n) { res = check(n.get_base_type()); }

    static
    bool check(SgType* n);

    static
    std::pair<bool, SgMemberFunctionDeclaration*>
    check(SgClassDefinition* n);
  };

  bool
  TriviallyDestructible::check(SgType* ty)
  {
    return sg::dispatch(TriviallyDestructible(), ty);
  }



  SgMemberFunctionDeclaration*
  isDtor(SgMemberFunctionDeclaration& n)
  {
    return n.get_specialFunctionModifier().isDestructor()? &n : nullptr;
  }

  SgMemberFunctionDeclaration*
  isDtor(SgNode& n)
  {
    SgMemberFunctionDeclaration* dcl = isSgMemberFunctionDeclaration(&n);

    return dcl ? isDtor(*dcl) : dcl;
  }

  /*
  SgMemberFunctionDeclaration*
  isDtorBlock(SgBasicBlock& n)
  {
    SgNode& parent      = SG_DEREF(n.get_parent());
    SgNode& grandparent = SG_DEREF(parent.get_parent());

    return isDtor(grandparent);
  }
  */


  struct TriviallyDestructibleDecl : sg::DispatchHandler<std::pair<bool, SgMemberFunctionDeclaration*> >
  {
    typedef sg::DispatchHandler<std::pair<bool, SgMemberFunctionDeclaration*> > base;

    TriviallyDestructibleDecl()
    : base(std::make_pair(true, nullptr))
    {}

    void nontrivial(SgMemberFunctionDeclaration* dtor = nullptr)
    {
      res = std::make_pair(false, dtor);
    }

    void chkClassDeclaration(SgClassDeclaration& n);

    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }

    void handle(SgDeclarationStatement&) { }
    void handle(SgTypedefDeclaration&)   { }
    void handle(SgEnumDeclaration&)      { }

    void handle(SgClassDeclaration& n)
    {
      // this is only concerned about anonymous unions / classes
      //   (classes a non standard extension accepted by many compilers),
      //   whose members get embedded into the surrounding scope.
      //   Any other case should not produce non-trivial destructors.
      if (!n.get_explicit_anonymous()) return;

      chkClassDeclaration(n);
    }

    void handle(SgMemberFunctionDeclaration& n)
    {
      if (isDtor(n)) nontrivial(&n);
    }

    void handle(SgFunctionDeclaration& n) { } // subsumes the two beneath
    void handle(SgTemplateFunctionDeclaration& n) { }
    void handle(SgTemplateInstantiationFunctionDecl& n)
    {
      logWarn() << "instance " << n.get_name() << std::endl;
    }

    void handle(SgVariableDeclaration& n)
    {
      if (si::isStatic(&n)) return;

      if (!TriviallyDestructible::check(onlyName(n).get_type()))
        nontrivial();
    }

    static
    ReturnType check(SgDeclarationStatement* n);

    static
    ReturnType check(SgBaseClass* n);
  };


  void
  TriviallyDestructibleDecl::chkClassDeclaration(SgClassDeclaration& n)
  {
    SgDeclarationStatement* defdcl = n.get_definingDeclaration();
    if (!defdcl) { nontrivial(); return; }

    SgClassDeclaration&     clsdcl = SG_ASSERT_TYPE(SgClassDeclaration, *defdcl);
    const bool              trvl   = TriviallyDestructible::check(clsdcl.get_definition()).first;

    if (!trvl) nontrivial();
  }

  TriviallyDestructibleDecl::ReturnType
  TriviallyDestructibleDecl::check(SgDeclarationStatement* n)
  {
    return sg::dispatch(TriviallyDestructibleDecl(), n);
  }

  TriviallyDestructibleDecl::ReturnType
  TriviallyDestructibleDecl::check(SgBaseClass* n)
  {
    SgClassDeclaration*       cls = SG_DEREF(n).get_base_class();
    TriviallyDestructibleDecl test;

    test.chkClassDeclaration(SG_DEREF(cls));
    return test;
  }

  std::pair<bool, SgMemberFunctionDeclaration*>
  TriviallyDestructible::check(SgClassDefinition* def)
  {
    ROSE_ASSERT(def);

    bool trivial_class = true;

    // this loop checks if all members are trivially destructable
    //   and if the class has a declared destructor
    for (SgDeclarationStatement* mem : def->get_members())
    {
      ROSE_ASSERT(mem);

      if (!(TriviallyDestructibleDecl::check(mem).first))
        trivial_class = false;

      if (SgMemberFunctionDeclaration* cand = isSgMemberFunctionDeclaration(mem))
      {
        if (isDtor(*cand)) return std::make_pair(false, cand);
      }
    }

    if (!trivial_class) return std::make_pair(false, nullptr);

    for (SgBaseClass* baseclass : def->get_inheritances())
    {
      ROSE_ASSERT(baseclass);

      if (  baseclass->get_isDirectBaseClass()
         && (!(TriviallyDestructibleDecl::check(baseclass).first))
         )
      {
        return std::make_pair(false, nullptr);
      }
    }

    return std::make_pair(true, nullptr);
  }

  struct SameName
  {
    explicit
    SameName(SgInitializedName& var)
    : name(var.get_name())
    {}

    bool operator()(SgInitializedName* cand)
    {
      ROSE_ASSERT(cand);
      return cand->get_name() == name;
    }

    SgName name;
  };

  SgInitializer*
  getMemberInitializer(SgInitializedName& var, SgCtorInitializerList& ctorini)
  {
    SgInitializedNamePtrList&                lst = ctorini.get_ctors();
    SgInitializedNamePtrList::const_iterator pos = std::find_if(lst.begin(), lst.end(), SameName(var));

    return (pos != lst.end()) ? (*pos)->get_initializer() : var.get_initializer();
  }

  struct ConstructorInitializerListError : std::logic_error
  {
    using base = std::logic_error;

    ConstructorInitializerListError(const std::string& what, SgInitializedName* ini)
    : base(what), initname(ini)
    {}

    SgInitializedName* initname;
  };

  struct SameClassDef
  {
    explicit
    SameClassDef(SgBaseClass& base)
    : classdef(getClassDef(SG_DEREF(base.get_base_class())))
    {}

    bool operator()(SgInitializedName* cand)
    {
      if (ERROR_TOLERANT && !(cand && cand->get_initializer()))
      {
        throw ConstructorInitializerListError("unusual constructor list element", cand);
      }

      ROSE_ASSERT(cand && cand->get_initializer());
      SgConstructorInitializer* ctorini = isSgConstructorInitializer(cand->get_initializer());

      // if it is not a base class initialization, it must be member variable initialization
      //   -> ignore
      // \todo once we see a member variable initialization, the base class was not found,
      //       and the iteration can be aborted.
      if (!ctorini) return false;

      SgMemberFunctionDeclaration& mfn = SG_DEREF( ctorini->get_declaration() );

      return &getClassDef(mfn) == &classdef;
    }

    SgClassDefinition& classdef;
  };

  SgInitializer*
  getBaseInitializer(SgBaseClass& base, SgCtorInitializerList& ctorini)
  {
    SgInitializedNamePtrList&                lst = ctorini.get_ctors();
    SgInitializedNamePtrList::const_iterator pos = std::find_if(lst.begin(), lst.end(), SameClassDef(base));

    return (pos != lst.end()) ? (*pos)->get_initializer() : nullptr;
  }


  SgArrayType* isArrayType(SgType* t)
  {
    if (SgArrayType* arrty = isSgArrayType(t))
      return arrty;

    if (SgTypedefType* tdty = isSgTypedefType(t))
      return isArrayType(tdty->get_base_type());

    if (SgModifierType* modty = isSgModifierType(t))
      return isArrayType(modty->get_base_type());

    return nullptr;
  }


  //
  // Creator functions

  SgStatement&
  createMemberCall( SgExpression& obj,
                    SgMemberFunctionDeclaration& fnDcl,
                    SgExprListExp& args,
                    bool virtualCall = false
                  );

  Sg_File_Info* dummyFileInfo()
  {
    return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  }

  SgForStatement&
  createArrayElementOperation( SgArrayType& arrty,
                               SgExpression& elem,
                               SgMemberFunctionDeclaration& fnDcl,
                               SgExprListExp& args,
                               bool virtualCall = false
                             )
  {
    // loop skeleton
    SgForStatement&        sgnode  = SG_DEREF( new SgForStatement(dummyFileInfo()) );
    SgExpression&          start   = SG_DEREF( sb::buildIntVal(0) );
    std::string            varname = si::generateUniqueVariableName(&sgnode, "dtorloop");
    SgInitializer&         varini  = SG_DEREF( sb::buildAssignInitializer(&start, start.get_type()) );
    SgVariableDeclaration& var     = SG_DEREF( sb::buildVariableDeclaration(varname, sb::buildIntType(), &varini, &sgnode) );

    ROSE_ASSERT(sgnode.get_for_init_stmt() != nullptr);
    sgnode.append_init_stmt(&var);

    // test
    SgExpression&          limit   = SG_DEREF( si::deepCopy(arrty.get_index()) );
    SgExpression&          vartst  = SG_DEREF( sb::buildVarRefExp(&var) );
    SgExpression&          lt      = SG_DEREF( sb::buildLessThanOp(&vartst, &limit) );
    SgStatement&           ltstmt  = SG_DEREF( sb::buildExprStatement(&lt) );

    sg::linkParentChild(sgnode, ltstmt, &SgForStatement::set_test);

    // increment
    SgExpression&          varinc  = SG_DEREF( sb::buildVarRefExp(&var) );
    SgExpression&          inc     = SG_DEREF( sb::buildPlusPlusOp(&varinc) );

    sg::linkParentChild(sgnode, inc, &SgForStatement::set_increment);

    // index expression + body recursion
    SgExpression&          varidx  = SG_DEREF( sb::buildVarRefExp(&var) );
    SgExpression&          indexed = SG_DEREF( sb::buildPntrArrRefExp(&elem, &varidx) );

    ROSE_ASSERT(arrty.get_base_type() == indexed.get_type());
    SgStatement&           bdy     = createMemberCall(indexed, fnDcl, args, virtualCall);
    //~ was: bdy = sg::dispatch(Creator{indexed}, arrty.get_base_type());

    sg::linkParentChild(sgnode, bdy, &SgForStatement::set_loop_body);

    // done
    return sgnode;
  }


  SgExpression&
  createMemberSelection(SgExpression& expr, SgMemberFunctionRefExp& fn)
  {
    if (si::isPointerType(expr.get_type()))
      return SG_DEREF(sb::buildArrowExp(&expr, &fn));

    return SG_DEREF(sb::buildDotExp(&expr, &fn));
  }

  SgStatement&
  createMemberCall( SgExpression& obj,
                    SgMemberFunctionDeclaration& fnDcl,
                    SgExprListExp& args,
                    bool virtualCall
                  )
  {
    if (SgArrayType* arrTy = isArrayType(obj.get_type()))
      return createArrayElementOperation(*arrTy, obj, fnDcl, args, virtualCall);

    SgSymbol*               sym    = fnDcl.search_for_symbol_from_symbol_table();
    SgMemberFunctionSymbol& fnSym  = SG_DEREF(isSgMemberFunctionSymbol(sym));
    SgMemberFunctionRefExp& fnRef  = SG_DEREF(sb::buildMemberFunctionRefExp( &fnSym,
                                                                             virtualCall,
                                                                             false /* no qualifier */
                                                                           ));

    SgExpression&           target = createMemberSelection(obj, fnRef);
    SgExpression&           call   = SG_DEREF(sb::buildFunctionCallExp(&target, &args));

    return SG_DEREF(sb::buildExprStatement(&call));
  }


  SgStatement&
  createMemberCall( SgInitializedName& var,
                    SgMemberFunctionDeclaration& fnDcl,
                    SgExprListExp& args,
                    bool virtualCall = false
                  )
  {
    SgScopeStatement&       scope  = SG_DEREF(si::getEnclosingScope(&var));
    SgVarRefExp&            varRef = SG_DEREF(sb::buildVarRefExp(&var, &scope));

    return createMemberCall(varRef, fnDcl, args, virtualCall);
  }

  SgStatement&
  createMemberCallFromConstructorInitializer(SgInitializedName& var, SgConstructorInitializer& ini)
  {
    SgMemberFunctionDeclaration& ctorDecl = SG_DEREF(ini.get_declaration());
    SgExprListExp&               ctorArgs = SG_DEREF(si::deepCopy(ini.get_args()));

    return createMemberCall(var, ctorDecl, ctorArgs);
  }


  SgConstructorInitializer*
  constructorInitializer(SgConstructorInitializer& ini);

  SgConstructorInitializer*
  constructorInitializer(SgAggregateInitializer& ini)
  {
    SgConstructorInitializer* sub = isSgConstructorInitializer(elemAt(ini.get_initializers(), 0));

    return sub ? constructorInitializer(*sub) : nullptr;
  }

  SgConstructorInitializer*
  constructorInitializer(SgConstructorInitializer& ini)
  {
    if (ini.get_declaration())
      return &ini;

    // maybe it is a nested array constructor?
    SgAggregateInitializer* agg = isSgAggregateInitializer(elemAt(ini.get_args(), 0));

    return agg ? constructorInitializer(*agg) : nullptr;
  }

  SgConstructorInitializer*
  constructorInitializer(SgInitializer* ini)
  {
    if (SgConstructorInitializer* init = isSgConstructorInitializer(ini))
      return constructorInitializer(*init);

    if (SgAggregateInitializer* init = isSgAggregateInitializer(ini))
      return constructorInitializer(*init);

    return nullptr;
  }


  SgStatement&
  createMemberCallFromConstructorInitializer(SgExpression& obj, SgConstructorInitializer& ini)
  {
    SgConstructorInitializer&    ctorInit = SG_DEREF(constructorInitializer(ini));
    SgMemberFunctionDeclaration& ctorDecl = SG_DEREF(ctorInit.get_declaration());
    SgExprListExp&               ctorArgs = SG_DEREF(si::deepCopy(ini.get_args()));

    return createMemberCall(obj, ctorDecl, ctorArgs);
  }


  SgExprStatement&
  createAssignFromAssignInitializer(SgInitializedName& var, SgAssignInitializer& ini)
  {
    SgScopeStatement& scope   = SG_DEREF(si::getEnclosingScope(&var));
    SgVarRefExp&      lhs     = SG_DEREF(sb::buildVarRefExp(&var, &scope));
    SgExpression&     rhs     = SG_DEREF(si::deepCopy(ini.get_operand()));
    SgExpression&     expr    = SG_DEREF(sb::buildAssignOp(&lhs, &rhs));

    return SG_DEREF(sb::buildExprStatement(&expr));
  }


  //
  // C++ normalizing transformers

  SgMemberFunctionDeclaration*
  obtainDefaultCtorIfAvail(SgType& ty);

  /// inserts the initialization of a member variable into a block (i.e., ctor body).
  /// \note
  ///   the ast representation deviates from standard C++
  ///   struct S { S() {}; std::string s; };
  ///   => struct S { S() { s.S(); }; std::string s; };
  struct MemberVariableCtorTransformer
  {
      MemberVariableCtorTransformer(SgBasicBlock& where, SgInitializedName& what, SgInitializer* how)
      : blk(where), var(what), ini(how)
      {}

      SgStatement* createDefaultInitializerIfNeeded()
      {
        SgType&                      varty = SG_DEREF(var.get_type());
        SgMemberFunctionDeclaration* ctor  = obtainDefaultCtorIfAvail(varty);

        if (ctor == nullptr)
          return nullptr;
        // alternative: initialize with NullExpression

        SgExprListExp&               args  = SG_DEREF(sb::buildExprListExp());

        return &createMemberCall(var, *ctor, args);
      }

      SgStatement* fromInitializer()
      {
        if (ini == nullptr)
          return nullptr;

        if (SgConstructorInitializer* ctorInit = isSgConstructorInitializer(ini))
          return &createMemberCallFromConstructorInitializer(var, *ctorInit);

        if (SgAssignInitializer* assignInit = isSgAssignInitializer(ini))
          return &createAssignFromAssignInitializer(var, *assignInit);

        logError() << "Unknown initializer type: "
                   << typeid(*ini).name()
                   << std::endl;
        ROSE_ASSERT(false);
      }

      void execute(CxxTransformStats&)
      {
        // if a newStmt is generated then append it
        if ((newStmt = fromInitializer()) || (newStmt = createDefaultInitializerIfNeeded()))
          blk.prepend_statement(newStmt);
      }

    private:
      SgBasicBlock&      blk;
      SgInitializedName& var;
      SgInitializer*     ini;

      SgStatement*       newStmt = nullptr;
  };


  bool isConvertibleTo(SgExpression& expr, SgInitializedName& parm)
  {
    // \todo implement
    return true;
  }

  bool
  paramsHaveDefaultValues(SgMemberFunctionDeclaration* fn, SgInitializedNamePtrList& parms, size_t from)
  {
    const size_t eoparams = parms.size();

    for (size_t i = from; i < eoparams; ++i)
    {
      SgInitializedName& parm = SG_DEREF(parms.at(i));

      if (!parm.get_initializer())
      {
        logWarn() << "Did not find parameter default value in declaration of " << fn->get_name()
                  << ". Incomplete checking for sibling (forward) declarations."
                  << std::endl;

        return false;
      }
    }

    // \todo check for other declarations visible at the call site
    return true;
  }


  struct FindFunction
  {
      typedef SgDeclarationStatementPtrList::iterator iterator;
      typedef std::pair<iterator, iterator>           result;

      explicit
      FindFunction(const std::string& name, SgExprListExp* ctorargs = nullptr)
      : funname(name), args(ctorargs)
      {}

      FindFunction(const std::string& name, SgExprListExp& ctorargs)
      : FindFunction(name, &ctorargs)
      {}

      bool operator()(SgDeclarationStatement* dcl)
      {
        SgMemberFunctionDeclaration* mem = isSgMemberFunctionDeclaration(dcl);

        if (!mem || (funname != std::string(mem->get_name())))
          return false;

        // if there are no args and the name matches this is a candidate
        //   in particular for dtors.
        if (!args)
          return true;

        SgFunctionParameterList&     fplst = SG_DEREF(mem->get_parameterList());
        SgInitializedNamePtrList&    parms = fplst.get_args();
        SgExpressionPtrList&         exprs = args->get_expressions();

        if (exprs.size() > parms.size())
          return false;

        // \todo we need to ONLY check that mem is a ctor, dtor, cctor, or mctor (nothing else)
        for (size_t i = 0; i < exprs.size(); ++i)
        {
          if (!isConvertibleTo(SG_DEREF(exprs.at(i)), SG_DEREF(parms.at(i))))
            return false;
        }

        return paramsHaveDefaultValues(mem, parms, exprs.size());
      }

      static
      result
      find(SgClassDefinition& clsdef, const std::string& n, SgExprListExp* args = nullptr)
      {
        SgDeclarationStatementPtrList&          lst = clsdef.get_members();
        SgDeclarationStatementPtrList::iterator zzz = lst.end();

        return result(std::find_if(lst.begin(), zzz, FindFunction{n, args}), zzz);
      }

      static
      result
      find(SgClassDefinition& clsdef, const std::string& n, SgExprListExp& args)
      {
        return find(clsdef, n, &args);
      }

      static
      bool found(result res)
      {
        return res.first != res.second;
      }

      static
      SgMemberFunctionDeclaration&
      declaration(result res)
      {
        return SG_DEREF( isSgMemberFunctionDeclaration(*res.first) );
      }

    private:
      const std::string funname;
      SgExprListExp*    args; // optional
  };

  void markCompilerGenerated(SgLocatedNode& n)
  {
    n.set_endOfConstruct(dummyFileInfo());
    n.set_startOfConstruct(dummyFileInfo());
    n.set_file_info(dummyFileInfo());
  }

  void setSpecialFunctionModifier(SgSpecialFunctionModifier& mod, bool ctor)
  {
    if (ctor) mod.setConstructor(); else mod.setDestructor();
  }

  std::string nameOf(SgClassDefinition& clsdef)
  {
    return SG_DEREF(clsdef.get_declaration()).get_name();
  }

  std::string nameDtor(SgClassDefinition& clsdef)
  {
    return DTOR_PREFIX + nameOf(clsdef);
  }

  std::string nameCtorDtor(SgClassDefinition& clsdef, bool ctor)
  {
    return ctor ? nameOf(clsdef) : nameDtor(clsdef);
  }

  SgMemberFunctionDeclaration&
  mkCtorDtor(SgClassDefinition& scope, bool ctor)
  {
    SgName                       nm(nameCtorDtor(scope, ctor));
    SgType&                      ty  = SG_DEREF(sb::buildVoidType());
    SgFunctionParameterList&     lst = SG_DEREF(sb::buildFunctionParameterList());
    SgMemberFunctionDeclaration& dcl = SG_DEREF(sb::buildNondefiningMemberFunctionDeclaration(nm, &ty, &lst, &scope));
    SgFunctionParameterScope&    psc = SG_DEREF(new SgFunctionParameterScope(dummyFileInfo()));

    markCompilerGenerated(lst);
    markCompilerGenerated(dcl);
    markCompilerGenerated(psc);

    setSpecialFunctionModifier(dcl.get_specialFunctionModifier(), ctor);

    dcl.set_functionParameterScope(&psc);
    psc.set_parent(&dcl);
    dcl.set_firstNondefiningDeclaration(&dcl);

    return dcl;
  }

  SgTemplateArgumentPtrList*
  cloneTemplateArguments(SgTemplateInstantiationMemberFunctionDecl* dcl)
  {
    if (dcl == nullptr) return nullptr;

    SgTemplateArgumentPtrList& templargs = dcl->get_templateArguments();
    SgTemplateArgumentPtrList& res = SG_DEREF(new SgTemplateArgumentPtrList);

    res.reserve(templargs.size());
    for (SgTemplateArgument* tparam : templargs)
      res.push_back(si::deepCopy(tparam));

    return &res;
  }


  SgMemberFunctionDeclaration&
  mkCtorDtorDef(SgClassDefinition& clsdef, SgMemberFunctionDeclaration& nondef, bool ctor)
  {
    typedef SgTemplateInstantiationMemberFunctionDecl TemplateMemberFunction;

    ROSE_ASSERT(nondef.get_definingDeclaration() == nullptr);

    SgName                       nm   = nondef.get_name();
    SgType&                      ty   = SG_DEREF(nondef.get_orig_return_type());
    SgFunctionParameterList&     lst  = SG_DEREF(sb::buildFunctionParameterList());
    TemplateMemberFunction*      tmpl = isSgTemplateInstantiationMemberFunctionDecl(&nondef);
    SgMemberFunctionDeclaration* pdcl = sb::buildDefiningMemberFunctionDeclaration( nm,
                                                                                    &ty,
                                                                                    &lst,
                                                                                    &clsdef,
                                                                                    nullptr /* decorator list */,
                                                                                    tmpl != nullptr /* build template instance */,
                                                                                    0,
                                                                                    &nondef,
                                                                                    cloneTemplateArguments(tmpl)
                                                                                  );
    SgMemberFunctionDeclaration& dcl  = SG_DEREF(pdcl);
    SgFunctionParameterScope&    psc  = SG_DEREF(new SgFunctionParameterScope(dummyFileInfo()));

    ROSE_ASSERT(dcl.get_parent() != nullptr);
    ROSE_ASSERT(dcl.get_definition() != nullptr);
    ROSE_ASSERT(dcl.get_CtorInitializerList() != nullptr);
    ROSE_ASSERT(dcl.get_functionParameterScope() == nullptr);
    ROSE_ASSERT(nondef.get_definingDeclaration() != nullptr);

    setSpecialFunctionModifier(dcl.get_specialFunctionModifier(), ctor);
    dcl.set_functionParameterScope(&psc);

    psc.set_parent(&dcl);

    return dcl;
  }

  SgMemberFunctionDeclaration&
  createCtorDtor(SgClassDefinition& clsdef, bool ctor)
  {
    SgMemberFunctionDeclaration& nondef  = mkCtorDtor(clsdef, ctor);
    SgMemberFunctionDeclaration& ctordef = mkCtorDtorDef(clsdef, nondef, ctor);

    clsdef.append_member(&ctordef);
    return ctordef;
  }


  /// obtains a reference to a compiler generatable constructor in class \ref clazz that can take ctorargs.
  /// If none is defined, an empty constructor is added to the class
  SgMemberFunctionDeclaration&
  obtainGeneratableFunction(SgClassDefinition& clsdef, const std::string& n, SgExprListExp& ctorargs)
  {
    ROSE_ASSERT(ctorargs.get_expressions().size() == 0); // \todo handle copy and move ctors

    FindFunction::result res = FindFunction::find(clsdef, n, ctorargs);

    return FindFunction::found(res) ? FindFunction::declaration(res)
                                    : createCtorDtor(clsdef, n.at(0) != '~')
                                    ;
  }

  std::string
  getName(SgClassDefinition& clsdef)
  {
    return SG_DEREF( clsdef.get_declaration() ).get_name();
  }

  SgMemberFunctionDeclaration&
  obtainGeneratableCtor(SgClassDefinition& clsdef, SgExprListExp& ctorargs)
  {
    return obtainGeneratableFunction(clsdef, getName(clsdef), ctorargs);
  }

  SgMemberFunctionDeclaration&
  obtainGeneratableDtor(SgClassDefinition& clsdef, SgExprListExp& ctorargs)
  {
    const std::string dtorname = DTOR_PREFIX + getName(clsdef);

    return obtainGeneratableFunction(clsdef, dtorname, ctorargs);
  }

  SgMemberFunctionDeclaration*
  obtainDefaultCtorIfAvail(SgType& ty)
  {
    using ExprListGuard = std::unique_ptr<SgExprListExp>;
    // \todo also skip usingg declarations (aka SgTemplateTypedefs)

    SgType*            elemty = ty.stripType(STRIP_MODIFIER_ALIAS);
    SgClassType*       clsty  = isSgClassType(elemty);

    if (!clsty) return nullptr;

    logInfo() << "link to default ctor" << std::endl;

    // args is temporary
    //~ SgExprListExp      emptyargs;
    ExprListGuard      emptyargs{ sb::buildExprListExp() };
    SgClassDefinition& clsdef = getClassDef(SG_DEREF(clsty->get_declaration()));

    return &obtainGeneratableCtor( clsdef, *emptyargs );
  }


  struct BaseCtorCallTransformer
  {
      BaseCtorCallTransformer(SgBasicBlock& where, SgBaseClass& what, SgInitializer* how)
      : blk(where), baseclass(what), ini(isSgConstructorInitializer(how))
      {
        // if how != nullptr then ini != nullptr
        ROSE_ASSERT((!how) || ini);
      }

      SgThisExp& mkThisExp(SgClassDeclaration& cls) const
      {
        SgSymbol& sym = SG_DEREF( cls.search_for_symbol_from_symbol_table() );

        return SG_DEREF( sb::buildThisExp(&sym) );
      }

      SgStatement& mkDefaultInitializer(SgClassDeclaration& clazz) const
      {
        SgExprListExp&               args  = SG_DEREF( sb::buildExprListExp() );
        SgMemberFunctionDeclaration& ctor  = obtainGeneratableCtor(getClassDef(clazz), args);
        SgExpression&                self  = mkThisExp(clazz);

        return createMemberCall(self, ctor, args, false /* non-virtual call */);
      }

      SgStatement& mkCtorCall() const
      {
        SgClassDeclaration& clazz = SG_DEREF( baseclass.get_base_class() );

        if (!ini) return mkDefaultInitializer(clazz);

        SgExpression&       self  = mkThisExp(clazz);

        return createMemberCallFromConstructorInitializer(self, *ini);
      }

      void execute(CxxTransformStats&)
      {
        newStmt = &mkCtorCall();

        blk.prepend_statement(newStmt);
      }

    private:
      SgBasicBlock&             blk;
      SgBaseClass&              baseclass;
      SgConstructorInitializer* ini;

      SgStatement*              newStmt = nullptr;
  };

  SgFunctionSymbol&
  get_symbol(SgFunctionDeclaration& fundecl)
  {
    SgSymbol& symbl = SG_DEREF( fundecl.search_for_symbol_from_symbol_table() );

    return SG_ASSERT_TYPE(SgFunctionSymbol, symbl);
  }

  struct BaseDtorCallTransformer
  {
      BaseDtorCallTransformer(SgBasicBlock& where, SgBaseClass& what)
      : blk(where), baseclass(what)
      {}

      SgExprStatement& mkDtorCall() const
      {
        SgExprListExp&               args  = SG_DEREF( sb::buildExprListExp() );
        SgClassDeclaration&          clazz = SG_DEREF( baseclass.get_base_class() );
        SgMemberFunctionDeclaration& dtor  = obtainGeneratableDtor(getClassDef(clazz), args);
        SgFunctionSymbol&            symbl = get_symbol(dtor);
        SgFunctionCallExp&           call  = SG_DEREF( sb::buildFunctionCallExp(&symbl, &args) );

        return SG_DEREF( sb::buildExprStatement(&call) );
      }

      void execute(CxxTransformStats&)
      {
        newStmt = &mkDtorCall();

        blk.append_statement(newStmt);
      }

    private:
      SgBasicBlock&    blk;
      SgBaseClass&     baseclass;

      SgExprStatement* newStmt = nullptr;
  };

#if 0
  struct DtorCallCreator : sg::DispatchHandler<SgStatement*>
  {
      using base = sg::DispatchHandler<SgStatement*>;

      explicit
      DtorCallCreator(SgExpression& expr)
      : base(), elem(expr)
      {}

      void descend(SgNode* n) { res = sg::dispatch(*this, n); }

      SgForStatement&
      createLoopOverArray(SgArrayType& arrty)
      {
        return ::createLoopOverArray<DtorCallCreator>(arrty);
      }

      void handle(SgNode& n)         { SG_UNEXPECTED_NODE(n); }

      void handle(SgModifierType& n) { descend(n.get_base_type()); }
      void handle(SgTypedefType& n)  { descend(n.get_base_type()); }
      void handle(SgPointerType& n)  { descend(n.get_base_type()); }

      void handle(SgClassType& n)
      {
        SgClassDefinition&           clsdef   = SG_DEREF( getClassDefOpt(n) );
        SgExprListExp&               args     = SG_DEREF( sb::buildExprListExp() );
        SgMemberFunctionDeclaration& dtorDcl  = obtainGeneratableDtor(clsdef, args);

        res = &createMemberCall(elem, dtorDcl, args, false /* non-virtual call (full type is known) */);
      }

      void handle(SgArrayType& n)
      {
        res = &createLoopOverArray(n);
      }

    private:
      SgExpression& elem;
  };
#endif


  /// compares two
  bool sameObject(const SgNode* lhs, const SgNode* rhs)
  {
    return dynamic_cast<const void*>(lhs) == dynamic_cast<const void*>(rhs);
  }

  bool isNormalizedSageNode(const SgLocatedNode& n)
  {
    static const std::string TRANSFORM = "transformation" ;

    ROSE_ASSERT(n.get_file_info());

    //~ std::cerr << n.unparseToString() << " @" << n.get_file_info()->get_filenameString()
              //~ << std::endl;

    return TRANSFORM == n.get_file_info()->get_filenameString();
  }

  bool isTemporary(const SgInitializedName& n)
  {
    return isNormalizedSageNode(n);
  }

  bool useRequiresLifetimeExtension(const SgInitializedName& n, const SgVariableDeclaration* use)
  {
    if (!use) return false;

    const SgInitializedName&   usevar = onlyName(*use);
    if (!SgNodeHelper::isReferenceType(usevar.get_type())) return false;

    const SgAssignInitializer* init = isSgAssignInitializer(usevar.get_initializer());
    if (init) return false;

    const SgVarRefExp*         varref = isSgVarRefExp(init->get_operand());

    return (  varref
           && sameObject(n.get_symbol_from_symbol_table(), varref->get_symbol())
           );
  }



  bool needsLifetimeExtension(SgInitializedName& n)
  {
    bool                   res  = false;
    SgVariableDeclaration& decl = sg::ancestor<SgVariableDeclaration>(n);
    SgStatement*           next = si::getNextStatement(&decl);

    while (!res && next)
    {
      res = useRequiresLifetimeExtension(n, isSgVariableDeclaration(next));
      next = si::getNextStatement(next);
    }

    return res;
  }

  SgStatement& dtorCallLocation(SgStatement& stmt, SgInitializedName& var)
  {
    SgVariableDeclaration& decl = sg::ancestor<SgVariableDeclaration>(var);
    SgStatement*           next = si::getNextStatement(&decl);

    while (isNormalizedSageNode(SG_DEREF(next)))
      next = si::getNextStatement(next);

    return SG_DEREF(next);
  }

  struct VarInsertDtorTransformer
  {
      VarInsertDtorTransformer(SgBasicBlock& where, SgStatement& pos, SgInitializedName& what)
      : blk(where), stmt(pos), var(what)
      {}

      void execute(CxxTransformStats&)
      {
        static constexpr unsigned char STRIP_TO_CLASSTYPE = STRIP_MODIFIER_ALIAS | SgType::STRIP_ARRAY_TYPE;

        SgType&                      varTy      = SG_DEREF(var.get_type());
        SgType*                      varTyBase  = varTy.stripType(STRIP_TO_CLASSTYPE);
        SgClassType&                 clsTy      = SG_DEREF(isSgClassType(varTyBase));
        SgClassDefinition&           clsDef     = SG_DEREF(getClassDefOpt(clsTy));

        // build new statement
        SgScopeStatement*            scope      = si::getEnclosingScope(&var);
        SgExpression&                destructed = SG_DEREF(sb::buildVarRefExp(&var, scope));
        SgExprListExp&               args       = SG_DEREF(sb::buildExprListExp());
        SgMemberFunctionDeclaration& dtorDcl    = obtainGeneratableDtor(clsDef, args);

        // we know the full type, thus we can use a non-virtual call
        dtorCall = &createMemberCall(destructed, dtorDcl, args, false /* non-virtual call */);

        if (isTemporary(var) && !needsLifetimeExtension(var))
        {
          SgStatement& inspos = dtorCallLocation(stmt, var);

          si::insertStatement(&inspos, dtorCall, false /* after */);
        }
        else if (sameObject(&stmt, &blk))
        {
          si::appendStatement(dtorCall, &blk);
        }
        else
        {
          si::insertStatement(&stmt, dtorCall, true /* before */);
        }
      }

    private:
      SgBasicBlock&      blk;
      SgStatement&       stmt;
      SgInitializedName& var;

      SgStatement*       dtorCall = nullptr;
  };


  struct CtorInitListTransformer
  {
      explicit
      CtorInitListTransformer(SgCtorInitializerList& n)
      : ctorlst(n)
      {}

      void execute(CxxTransformStats&)
      {
        // \todo delete elements before removing them from the list
        ctorlst.get_ctors().clear();
      }

    private:
      SgCtorInitializerList& ctorlst;
  };


  struct ConstructorTransformer
  {
      explicit
      ConstructorTransformer(SgMemberFunctionDeclaration& nondefiningCtor)
      : ctor(nondefiningCtor)
      {}

      void execute(CxxTransformStats& stat)
      {
        // is it already done?
        if (ctor.get_definingDeclaration()) return;

        // \todo cannot yet handle SgTemplateInstantiationMemberFunctionDecl
        if (isSgTemplateInstantiationMemberFunctionDecl(&ctor))
        {
          logError() << "Definition for SgTemplateInstantiationMemberFunctionDecl not generated: "
                     << ctor.get_name()
                     << std::endl;
          return;
        }

        SgClassDefinition&           clsdef  = sg::ancestor<SgClassDefinition>(ctor);
        SgMemberFunctionDeclaration& ctordef = mkCtorDtorDef(clsdef, ctor, true /* ctor */);

        clsdef.prepend_member(&ctordef);

        // initialize members
        normalizeCtorDtor(&ctordef, stat);
      }

    private:
      SgMemberFunctionDeclaration& ctor;
  };

  struct DestructorTransformer
  {
      explicit
      DestructorTransformer(SgClassDefinition& clsdef)
      : cls(clsdef)
      {}

      void execute(CxxTransformStats& stat)
      {
        logTrace() << "create dtor: " << nameCtorDtor(cls, false)
                   << " // " << SrcLoc(cls)
                   << std::endl;

        SgMemberFunctionDeclaration& dtor = createCtorDtor(cls, false /* dtor */);

        // destruct member variables that have class type
        normalizeCtorDtor(&dtor, stat);
      }

    private:
      SgClassDefinition& cls;
  };

  /// extracts the declared variables in a scope
  /// \details
  ///   the variables extracted for a function definition are the parameters
  /// \{
  SgInitializedNamePtrList variableList(const SgFunctionDefinition& n)
  {
    return SG_DEREF(n.get_declaration()).get_args();
  }

  SgInitializedNamePtrList variableList(const SgBasicBlock& n, const SgStatement& stmt)
  {
    using Iterator = SgStatementPtrList::const_iterator;

    SgInitializedNamePtrList  res;
    const SgStatementPtrList& lst   = n.get_statements();
    Iterator                  pos   = lst.begin();
    Iterator                  limit = lst.end();

    while (pos != limit && (!sameObject(*pos, &stmt)))
    {
      if (const SgVariableDeclaration* var = isSgVariableDeclaration(*pos))
        res.push_back(&onlyName(*var));

      ++pos;
    }

    return res;
  }

  SgInitializedNamePtrList variableList(const SgScopeStatement& n)
  {
    SgInitializedNamePtrList res;

    for (SgStatement* stmt : n.generateStatementList())
    {
      if (SgVariableDeclaration* var = isSgVariableDeclaration(stmt))
        res.push_back(&onlyName(*var));
    }

    return res;
  }

  SgInitializedNamePtrList variableList(const SgScopeStatement& n, const SgStatement& pos)
  {
    if (const SgBasicBlock* block = isSgBasicBlock(&n))
      return variableList(*block, pos);

    if (const SgFunctionDefinition* func = isSgFunctionDefinition(&n))
      return variableList(*func);

    return variableList(n);
  }
  /// \}


  /// records all variables from scope \ref n that needs to be destructed
  ///   at point \ref pos.
  /// \param n          the scope whose variables need to be destructed
  /// \param posInScope scope's child, and ancestor of \ref pos
  ///                   e.g., pos -> block -> block -> n
  ///                                         ^^^ posInScope
  ///                   if \ref n is a SgBasicBlock, posInScope is needed to
  ///                   destruct variables that have been allocated before posInScope.
  /// \param blk        the block where the destructors need to be inserted
  /// \param pos        insert position in blk
  /// \param transf     any new destruction will be added to the sequence of
  ///                   transformations.
  void recordScopedDestructors( SgScopeStatement& n,
                                SgStatement& posInScope,
                                SgBasicBlock& blk,
                                SgStatement& pos,
                                std::vector<AnyTransform>& transf
                              )
  {
    SgInitializedNamePtrList vars = variableList(n, posInScope);

    for (SgInitializedName* var : adapt::reverse(vars))
    {
      ROSE_ASSERT(var);

      if (!TriviallyDestructible::check(var->get_type()))
      {
        //~ logInfo() << "nontrivial: " << var->get_name() << " " << varty->get_mangled()
                  //~ << std::endl;
        transf.emplace_back(VarInsertDtorTransformer{blk, pos, *var});
        logTrace() << "gen dtor-sequence for: " << typeid(n).name()
                   << " / " << var->get_name()
                   << std::endl;
      }
    }
  }

  struct ScopeDestructorTransformer
  {
      explicit
      ScopeDestructorTransformer(SgStatement& where, SgBasicBlock& block, SgScopeStatement& outerLimit)
      : pos(&where), blk(&block), limit(&outerLimit)
      {}

      void execute(CxxTransformStats& stat)
      {
        logTrace() << "create block dtor: " << typeid(blk).name()
                   << " to: " << typeid(limit).name()
                   << " // " << SrcLoc(*blk)
                   << std::endl;

        std::vector<AnyTransform> transf;

        SgStatement* prev = pos;
        for (SgScopeStatement* curr = blk; curr != limit; curr = si::getEnclosingScope(curr))
        {
          recordScopedDestructors(SG_DEREF(curr), *prev, *blk, *pos, transf);

          prev = curr;
        }

        stat.cnt += transf.size();
        for (AnyTransform& tf : transf)
          tf.execute(stat);
      }

    private:
      ScopeDestructorTransformer() = delete;

      SgStatement* const      pos;
      SgBasicBlock* const     blk;
      SgScopeStatement* const limit;
  };

  bool isVirtualBase(SgBaseClass& base)
  {
    return SG_DEREF(base.get_baseClassModifier()).isVirtual();
  }

  bool isGenerateableCtor(SgMemberFunctionDeclaration& n)
  {
    SgFunctionParameterList&  fplst = SG_DEREF(n.get_parameterList());
    SgInitializedNamePtrList& parms = fplst.get_args();

    return parms.size() == 0;
  }

  bool needsCompilerGeneration(SgMemberFunctionDeclaration& n)
  {
    // \todo how do we distinguish from a generated definition and
    //       a constructor defined in a different translation unit?
    return (  n.get_definingDeclaration() == nullptr
           && isGenerateableCtor(n)
           );
  }

  struct InitSplitTransformer
  {
      InitSplitTransformer(SgInitializedName& vardcl, SgConstructorInitializer& varini)
      : var(vardcl), ini(varini)
      {}

      void execute(CxxTransformStats&)
      {
        logTrace() << "split object allocation and initialization"
                   << std::endl;

        // A a = A(); -> A a; a.A();
        newstmt = &createMemberCallFromConstructorInitializer(var, ini);

        SgStatement& decl = sg::ancestor<SgStatement>(var);

        si::insertStatement(&decl, newstmt, false /* insert after */);
        var.set_initializer(nullptr);
        // delete &ini;
      }

    private:
      SgInitializedName&        var;
      SgConstructorInitializer& ini;
      SgStatement*              newstmt = nullptr;
  };


  struct NewSplitTransformer
  {
      NewSplitTransformer(SgNewExp& newop, SgVariableSymbol* sy)
      : newexp(newop), varsym(sy)
      {}

      void createVariableIfNeeded()
      {
        if (varsym) return;

        // create new temporary variable
        SgScopeStatement&      scope  = SG_DEREF(si::getEnclosingScope(&newexp));
        std::string            nm     = si::generateUniqueVariableName(&scope, "tmpnew");
        SgType&                ty     = SG_DEREF(newexp.get_type());
        SgInitializer*         ini    = nullptr; // set later
        SgVariableDeclaration& vardcl = SG_DEREF(sb::buildVariableDeclaration(nm, &ty, ini, &scope));

        // set varsym
        newAllocStmt = &vardcl;
        varsym       = isSgVariableSymbol(onlyName(vardcl).search_for_symbol_from_symbol_table());
        ROSE_ASSERT(varsym);
      }

      bool isPlacementNew() const
      {
        SgExprListExp*       exp = newexp.get_placement_args();

        if (!exp) return false;

        SgExpressionPtrList& lst = exp->get_expressions();

        return (lst.size() == 1) && si::isPointerType(lst[0]->get_type());
      }

      void normalizePlacementNew()
      {
        SgExpression&  place     = SG_DEREF(elemAt(newexp.get_placement_args(), 0));
        SgExpression&  place_cpy = SG_DEREF(si::deepCopy(&place));

        if (newAllocStmt)
        {
          SgInitializedName& var = onlyName(*newAllocStmt);

          var.set_initializer(sb::buildAssignInitializer_nfi(&place_cpy, place.get_type()));
        }
        else
          newNewExpr = &place_cpy;
      }

      void normalizeAllocatingNew()
      {
        if (!newAllocStmt) return;

        SgExpression&      newexp_cpy = SG_DEREF(si::deepCopy(&newexp));
        SgInitializedName& var = onlyName(*newAllocStmt);

        var.set_initializer(sb::buildAssignInitializer_nfi(&newexp_cpy, newexp_cpy.get_type()));
      }

      void normalizeNew()
      {
        if (isPlacementNew())
          normalizePlacementNew();
        else
          normalizeAllocatingNew();
      }

      void execute(CxxTransformStats&)
      {
        logTrace() << "split new allocation and initialization"
                   << std::endl;

        // if the new expr is not part of an assignment or variable
        //   declaration, create a new variable
        createVariableIfNeeded();

        // normalize the new expression
        normalizeNew();

        // create the initialization call
        SgConstructorInitializer& ini     = SG_DEREF(newexp.get_constructor_args());
        SgVarRefExp&              varref  = SG_DEREF(sb::buildVarRefExp(varsym));

        newCtorStmt = &createMemberCallFromConstructorInitializer(varref, ini);

        SgStatement*              currentNewStmt = &sg::ancestor<SgStatement>(newexp);

        // execute transformation
        if (newAllocStmt)
        {
          ROSE_ASSERT(newNewExpr == nullptr);
          si::replaceStatement(currentNewStmt, newAllocStmt, true /* move preproc info */);

          currentNewStmt = newAllocStmt;
        }
        else if (newNewExpr)
        {
          // replace new expression with a simplified placement new
          si::replaceExpression(&newexp, newNewExpr, true /* no delete */);
        }

        // insert the initialization (constructor calls)
        si::insertStatement(currentNewStmt, newCtorStmt, false /* insert after */);

        // deallocation
        // if (newAllocStmt) delete &currentNewStmt;
        // if (newNewExpr)   delete &newexp;
      }

    private:
      SgNewExp&              newexp;
      SgVariableSymbol*      varsym;

      SgVariableDeclaration* newAllocStmt = nullptr;
      SgExpression*          newNewExpr   = nullptr;
      SgStatement*           newCtorStmt  = nullptr;
  };

  struct DeleteSplitTransformer
  {
      DeleteSplitTransformer(SgDeleteExp& delop, SgMemberFunctionDeclaration& dtorDcl)
      : delexp(delop), dtor(dtorDcl)
      {}

      void execute(CxxTransformStats&)
      {
        logTrace() << "split new allocation and initialization"
                   << std::endl;

        SgVarRefExp&   varRef    = SG_DEREF(isSgVarRefExp(delexp.get_variable()));

        // build new stmt
        SgExprListExp& args      = SG_DEREF( sb::buildExprListExp() );
        SgVarRefExp&   newVarRef = SG_DEREF(si::deepCopy(&varRef));

        // \todo check if dtor is a virtual function
        newDtorStmt = &createMemberCall(newVarRef, dtor, args, true /* virtual */);

        // insert statement
        SgStatement&   stmt      = sg::ancestor<SgStatement>(delexp);

        si::insertStatement(&stmt, newDtorStmt, true /* insert before */);

        // deallocation - nothing to deallocate
      }

    private:
      SgDeleteExp&                 delexp;
      SgMemberFunctionDeclaration& dtor;

      SgStatement*                 newDtorStmt  = nullptr;
  };



  SgInitializedName& returnParameter(SgFunctionDeclaration& fn)
  {
    SgFunctionParameterList&  plst = SG_DEREF(fn.get_parameterList());
    SgInitializedNamePtrList& parms = plst.get_args();

    ROSE_ASSERT(parms.size());
    return SG_DEREF(parms.back());
  }

  struct RVOReturnStmtTransformer
  {
      static constexpr const char* returnParameterName = "_tmprvo";

      RVOReturnStmtTransformer(SgReturnStmt& retstmt, SgFunctionDeclaration& fundcl)
      : ret(retstmt), fun(fundcl)
      {}

      void execute(CxxTransformStats&)
      {
        logTrace() << "return object optimization"
                   << std::endl;

        SgInitializedName&        parm = returnParameter(fun);
        ROSE_ASSERT(  SgNodeHelper::isPointerType(parm.get_type())
                   && (parm.get_name() == returnParameterName)
                   );

        SgExpression*             expr = ret.get_expression();
        SgConstructorInitializer& ini  = SG_DEREF(isSgConstructorInitializer(expr));
        SgStatement&              stmt = createMemberCallFromConstructorInitializer(parm, ini);

        ret.set_expression(sb::buildNullExpression());
        si::insertStatement(&ret, &stmt, true /* before */);
      }

    private:
      SgReturnStmt&          ret;
      SgFunctionDeclaration& fun;
  };

  struct RVOInitializationTransformer
  {
      RVOInitializationTransformer(SgInitializedName& vardcl, SgAssignInitializer& varini)
      : var(vardcl), ini(varini)
      {}

      void execute(CxxTransformStats&)
      {
        logTrace() << "return value optimization"
                   << std::endl;

        SgCallExpression& call = SG_DEREF(isSgCallExpression(ini.get_operand()));
        SgExprListExp&    args = SG_DEREF(call.get_args());

        SgScopeStatement& scope = sg::ancestor<SgScopeStatement>(var);
        SgVarRefExp&      vref  = SG_DEREF(sb::buildVarRefExp(&var, &scope));
        SgExpression&     vptr  = SG_DEREF(sb::buildAddressOfOp(&vref));

        si::appendExpression(&args, &vptr);

        SgExprStatement&  stmt  = SG_DEREF(sb::buildExprStatement(&call));
        SgStatement&      prev  = sg::ancestor<SgStatement>(var);

        si::insertStatement(&prev, &stmt, false /* after */);

        var.set_initializer(nullptr);
        ini.set_operand(nullptr);
        //~ delete &ini;
      }

    private:
      SgInitializedName&   var;
      SgAssignInitializer& ini;
  };


  struct RVOParameterTransformer
  {
      RVOParameterTransformer(SgFunctionDeclaration& func, SgType& retTy)
      : fn(func), ty(retTy)
      {}

      void execute(CxxTransformStats&)
      {
        logTrace() << "gen return parameter for " << fn.get_name()
                   << std::endl << fn.unparseToString()
                   << std::endl;

        SgFunctionType& fnty  = SG_DEREF(fn.get_type());

        SgInitializedName* parm =
           sb::buildInitializedName( RVOReturnStmtTransformer::returnParameterName,
                                     sb::buildPointerType(&ty),
                                     nullptr
                                   );

        ROSE_ASSERT(parm);
        /*SgVariableSymbol* sym =*/ si::appendArg(fn.get_parameterList(), parm);

        fnty.set_return_type(sb::buildVoidType());
      }

    private:
      SgFunctionDeclaration& fn;
      SgType&                ty;
  };



  //
  // memoized functors


  SgInitializedNamePtrList
  extractNonStaticMemberVars(const SgClassDefinition* cls)
  {
    SgInitializedNamePtrList res;

    for (SgDeclarationStatement* cand : cls->get_members())
    {
      SgVariableDeclaration* dcl = isSgVariableDeclaration(cand);

      if (dcl && !si::isStatic(dcl))
      {
        res.push_back(&onlyName(*dcl));
      }
    }

    return res;
  }

  auto getMemberVars = memoizer(extractNonStaticMemberVars);

  auto getDirectNonVirtualBases =
           memoizer( [](const SgClassDefinition* cls) -> SgBaseClassPtrList
                     {
                       SgBaseClassPtrList res;

                       for (SgBaseClass* cand : cls->get_inheritances())
                       {
                         ROSE_ASSERT(cand);

                         if (cand->get_isDirectBaseClass() && !isVirtualBase(*cand))
                           res.push_back(cand);
                       }

                       return res;
                     }
                   );

  auto getAllVirtualBases =
           memoizer( [](const SgClassDefinition* cls) -> SgBaseClassPtrList
                     {
                       SgBaseClassPtrList res;

                       for (SgBaseClass* cand : cls->get_inheritances())
                       {
                         ROSE_ASSERT(cand);

                         if (isVirtualBase(*cand))
                           res.push_back(cand);
                       }

                       return res;
                     }
                   );

  void clearMemoized()
  {
    logInfo() << getDirectNonVirtualBases << " getDirectNonVirtualBases - cache\n"
              << getAllVirtualBases       << " getAllVirtualBases - cache\n"
              << getMemberVars            << " getMemberVars - cache\n"
              << std::endl;

    getDirectNonVirtualBases.clear();
    getAllVirtualBases.clear();
    getMemberVars.clear();
  }

  // end memoized functors



  void normalizeCtorDef(SgMemberFunctionDeclaration& fun, transformation_container& cont)
  {
    if (!fun.get_definition()) return;

    SgBasicBlock&          blk = getCtorBody(fun);
    SgClassDefinition&     cls = getClassDef(fun);
    SgCtorInitializerList& lst = SG_DEREF( fun.get_CtorInitializerList() );

    // explicitly construct all member variables;
    //   execute the transformations in reverse order
    //   (the last transformation appears first in code)
    for (SgInitializedName* var : adapt::reverse(getMemberVars(&cls)))
    {
      SgInitializer* ini = getMemberInitializer(*var, lst);

      cont.emplace_back(MemberVariableCtorTransformer{blk, *var, ini});
    }

    // explicitly construct all direct non-virtual bases;
    //   execute the transformations in reverse order
    //   (the last transformation appears first in code)
    for (SgBaseClass* base : adapt::reverse(getDirectNonVirtualBases(&cls)))
    {
      try
      {
        SgInitializer* ini = getBaseInitializer(*base, lst);

        cont.emplace_back(BaseCtorCallTransformer{blk, *base, ini});
      }
      catch (const ConstructorInitializerListError& err)
      {
        logError() << "Constructor Initializer List Error in: " << fun.get_name() << std::endl;

        if (err.initname == nullptr)
          logError() << "An SgInitializedName element is NULL" << std::endl;
        else if (err.initname->get_initializer() == nullptr)
          logError() << "An SgInitializedName element " << err.initname->get_name() << " has a NULL initializer: "
                     << err.initname->unparseToString()
                     << std::endl;
        else
          logError() << "Unknown condition" << std::endl;

        logError() << "Skipping generation of one base class initializer!" << std::endl;
      }
    }

    // log errors for unhandled virtual base classes
    if (getAllVirtualBases(&cls).size())
    {
      logError() << "virtual base class normalization in constructor NOT YET IMPLEMENTED: " << fun.get_name()
                 << std::endl;

      throw std::logic_error("virtual base class normalization in constructor NOT YET IMPLEMENTED");
    }

    // the initializer list is emptied.
    //   while it is not part of the ICFG, its nodes would be seen by
    //   the normalization.
    cont.emplace_back(CtorInitListTransformer{lst});
  }

  void normalizeDtorDef(SgMemberFunctionDeclaration& fun, transformation_container& cont)
  {
    if (!fun.get_definition()) return;

    SgBasicBlock&      blk = getCtorBody(fun);
    SgClassDefinition& cls = getClassDef(fun);

    // recordScopedDestructors(blk, blk, blk, cont);

    // explicitly destruct all member variables of class type;
    //   execute the transformations in reverse order
    for (SgInitializedName* var : adapt::reverse(getMemberVars(&cls)))
    {
      SgType* varty = var->get_type();

      if (!TriviallyDestructible::check(varty))
      {
        //~ logInfo() << "nontrivial: " << var->get_name() << " " << varty->get_mangled()
                  //~ << std::endl;

        cont.emplace_back(VarInsertDtorTransformer{blk, blk, *var});
      }
    }

    // explicitly destruct all direct non-virtual base classes;
    //   execute the transformations in reverse order
    for (SgBaseClass* base : adapt::reverse(getDirectNonVirtualBases(&cls)))
    {
      cont.emplace_back(BaseDtorCallTransformer{blk, *base});
    }
  }

  SgScopeStatement&
  destructionLimit(SgFunctionDefinition& n)
  {
    SgScopeStatement* limit = si::getEnclosingScope(&n);
    ROSE_ASSERT(limit);

    // if n is a destructor include also the class' scope to destruct
    //   its data members.
    if (isDtor(n))
    {
      limit = si::getEnclosingScope(&n);
      ROSE_ASSERT(isSgClassDefinition(limit));
    }

    return *limit;
  }

  SgScopeStatement&
  destructionLimit(SgScopeStatement& n)
  {
    SgFunctionDefinition* fundef = isSgFunctionDefinition(&n);

    // if n is a function definition, compute limit for function destruction,
    //   otherwise return existing limit.
    return fundef ? destructionLimit(*fundef) : n;
  }

  SgScopeStatement&
  enclosingScope(SgScopeStatement* n)
  {
    ROSE_ASSERT(n);

    return SG_DEREF(si::getEnclosingScope(n));
  }

  struct GeneratorBase : ExcludeTemplates
  {
      using container = transformation_container;
      using node_set  = std::unordered_set<SgNode*>;

      GeneratorBase(container& transf, node_set& visited)
      : cont(&transf), knownNodes(&visited)
      {}

      GeneratorBase(GeneratorBase&&)                 = default;
      GeneratorBase& operator=(GeneratorBase&&)      = default;
      GeneratorBase(const GeneratorBase&)            = default;
      GeneratorBase& operator=(const GeneratorBase&) = default;

      node_set& visitedNodes() { return *knownNodes; }

      template <class Transformation>
      void record(Transformation tf)
      {
        cont->emplace_back(std::move(tf));
      }

      using ExcludeTemplates::handle;

    protected:
      container*            cont       = nullptr;
      node_set*             knownNodes = nullptr;

    private:
      GeneratorBase() = delete;
  };


  template <class SetT, class ElemT>
  inline
  bool alreadyProcessed(SetT& s, const ElemT& e)
  {
    return !s.insert(e).second;
  }
/*
  template <class SetT, class ElemT>
  inline
  bool containsElement(const SetT& s, const ElemT& e)
  {
    return s.find(e) != s.end();
  }
*/
  template <class Transformer>
  void descend(Transformer tf, SgNode& n)
  {
    if (alreadyProcessed(tf.visitedNodes(), &n))
      return;

    sg::traverseChildren(std::move(tf), n);
  }

  /// adds constructors and destructors as needed and normalizes these functions
  struct CxxCtorDtorGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      void handle(SgNode& n);

      void handle(SgConstructorInitializer& n)
      {
        SgMemberFunctionDeclaration* ctor = n.get_declaration();

        // do nothing if there is
        //   - no declaration
        //   - or has a definition
        if (!ctor || !needsCompilerGeneration(*ctor))
        {
          SgClassDeclaration* cls = n.get_class_decl();

          logInfo() << "no need to gen ctor: "
                    << (cls ? std::string(cls->get_name()) : std::string("null"))
                    << "/ " << ctor
                    << std::endl;
        }
        else
        {
          record(ConstructorTransformer{*ctor});
        }

        descend(*this, n);
      }

      void handle(SgMemberFunctionDeclaration& n)
      {
        if (n.get_specialFunctionModifier().isConstructor())
          normalizeCtorDef(n, *cont);

        // dtors are normalized when all destructors are inserted
        //~ else if (isDtor(n))
        //~ {
          //~ normalizeDtorDef(n, *cont);
        //~ }

        descend(*this, n);
      }

      void handle(SgClassDefinition& n)
      {
        using trivial_result_t = std::pair<bool, SgMemberFunctionDeclaration*>;

        trivial_result_t res = TriviallyDestructible::check(&n);

        if (!res.first /* not trivial */ && !res.second /* no user defined dtor */)
        {
          {
            SgNode& parent      = SG_DEREF(n.get_parent());
            SgNode& grandparent = SG_DEREF(parent.get_parent());

            logInfo() << "generate dtor: " << nameOf(n)
                      << "\n       parent: " << typeid(parent).name() << " @" << &parent
                      << "\n  grandparent: " << typeid(grandparent).name() << " @" << &grandparent
                      << std::endl;
          }

          record(DestructorTransformer{n});
        }

        descend(*this, n);
      }
  };

  void CxxCtorDtorGenerator::handle(SgNode& n)
  {
    descend(*this, n);
  }


  /// passes over object initialization
  ///   breaks up an object declaration into allocation and initialization
  ///   transformations if A is a non-trivial user defined type:
  ///     A a = A(x, y, z); => A a; a->A(x, y, z);
  ///     A* a = new A(1);  => A* a = new A; a->A(1);
  ///     a = new A(1);     => a = new A; a->A(1);
  ///     a = new (p) A(1); => a = p; a->A(1);
  ///     delete a;         => a->~A(); delete a;
  ///     \todo array versions
  struct CxxAllocInitSplitGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      void descend(SgNode& n);

      void handle(SgNode& n) { descend(n); }

      void handle(SgInitializedName& n)
      {
        if (SgConstructorInitializer* init = constructorInitializer(n.get_initializer()))
          record(InitSplitTransformer{n, *init});

        varsym = isSgVariableSymbol(n.search_for_symbol_from_symbol_table());
        // \note varsym can be null in case this is a forward function declaration

        descend(n);
      }

      void handle(SgAssignOp& n)
      {
        if (SgVarRefExp* lhs = isSgVarRefExp(n.get_lhs_operand()))
          varsym = lhs->get_symbol();

        descend(n);
      }

      void handle(SgNewExp& n)
      {
        record(NewSplitTransformer{n, varsym});
      }

      void handle(SgDeleteExp& n)
      {
        using trivial_check_result = std::pair<bool, SgMemberFunctionDeclaration*>;

        SgVarRefExp&   varRef     = SG_DEREF(isSgVarRefExp(n.get_variable()));
        SgType*        varType    = varRef.get_type();
        SgPointerType& varPtrType = SG_DEREF(isSgPointerType(varType->stripType(STRIP_MODIFIER_ALIAS)));
        SgType*        ty         = varPtrType.get_base_type();
        SgClassType&   clsTy      = SG_DEREF(isSgClassType(ty->stripType(STRIP_MODIFIER_ALIAS)));

        trivial_check_result triv = TriviallyDestructible::check(getClassDefOpt(clsTy));

        if (!triv.first)
        {
          record(DeleteSplitTransformer{n, SG_DEREF(triv.second)});
        }
      }

    private:
      SgVariableSymbol* varsym = nullptr;
  };

  void CxxAllocInitSplitGenerator::descend(SgNode& n)
  {
    ::ct::descend(*this, n);
  }

  SgType* optimizedReturnType(SgType& ty)
  {
    SgClassType* clsTy = isSgClassType(ty.stripType(STRIP_MODIFIER_ALIAS));

    return (clsTy && !si::IsTrivial(clsTy)) ? &ty : nullptr;
  }

  SgType* optimizedFunctionReturnType(SgFunctionType& ty)
  {
    return optimizedReturnType(SG_DEREF(ty.get_return_type()));
  }

  /// applies return value optimization to function calls that return objects.
  ///   transformations if A is a non-trivial user defined type:
  ///      A a = f(); => A a; f(&a);
  ///      A f();     => void f(A* res);
  ///      return a;  => res->A(a);
  struct CxxRVOGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      void explore(SgFunctionDeclaration* n); ///< processes \ref n
      void descend(SgNode& n);                ///< processes \ref n's children

      void descendWith(SgNode& n, SgFunctionDeclaration* ctx)
      {
        CxxRVOGenerator cpy(*this);

        cpy.rvoFunc = ctx;

        ::ct::descend(cpy, n);
      }

      void handle(SgNode& n) { descend(n); }

      void handle(SgReturnStmt& n)
      {
        if (rvoFunc && !isSgNullExpression(n.get_expression()))
        {
          //~ if (!isSgConstructorInitializer(n.get_expression()))
            //~ std::cerr << ">> " << n.unparseToString()
                      //~ << "\n@" << sg::ancestor<SgFunctionDeclaration>(n).unparseToString()
                      //~ << std::endl;

          ROSE_ASSERT(isSgConstructorInitializer(n.get_expression()));
          record(RVOReturnStmtTransformer{n, *rvoFunc});
        }
      }

      void handle(SgFunctionDeclaration& n)
      {
        SgFunctionDeclaration* rvoFn = nullptr;

        if (SgType* retTy = optimizedFunctionReturnType(SG_DEREF(n.get_type())))
        {
          explore(isSgFunctionDeclaration(n.get_firstNondefiningDeclaration()));
          record(RVOParameterTransformer{n, *retTy});
          rvoFn = &n; // set rvoFunc BEFORE descending into the body (only if RVO is active)
        }

        descendWith(n, rvoFn);
      }

      void handle(SgInitializedName& n)
      {
        SgAssignInitializer* init = isSgAssignInitializer(n.get_initializer());

        if (  init
           && optimizedReturnType(SG_DEREF(n.get_type()))
           && isSgCallExpression(init->get_operand())
           )
          record(RVOInitializationTransformer{n, *init});
      }

    private:
      SgFunctionDeclaration* rvoFunc = nullptr;
  };

  void CxxRVOGenerator::descend(SgNode& n)
  {
    ::ct::descend(*this, n);
  }

  void CxxRVOGenerator::explore(SgFunctionDeclaration* n)
  {
    if (!n || alreadyProcessed(*knownNodes, n)) return;

    sg::dispatch(CxxRVOGenerator{*this}, n);
  }


  /// passes over scopes and control flow interrupting statements
  ///   transformations: inserts destructor calls into the AST for non-trivial destructors
  ///     e.g., { A a; } => { A a; a.~A(); }
  struct CxxObjectDestructionGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      // canonical handling of loop statements
      void loop(SgScopeStatement& n);

      // records a scope sequence for destructor generation
      void recordScopeDestructors(SgStatement& n, SgBasicBlock& start, SgScopeStatement& limit);

      // recursive tree traversal
      void descend(SgNode& n);

      void handle(SgNode& n)         { descend(n); }

      void handle(SgWhileStmt& n)    { loop(n); }
      void handle(SgDoWhileStmt& n)  { loop(n); }
      void handle(SgForStatement& n) { loop(n); }

      void handle(SgSwitchStatement& n)
      {
        breakScope = &n;

        descend(n);
      }

      void handle(SgFunctionDefinition& n)
      {
        functionScope = &n;

        descend(n);

        if (SgMemberFunctionDeclaration* dtor = isDtor(SG_DEREF(n.get_parent())))
          normalizeDtorDef(*dtor, *cont);
      }

      void handle(SgBreakStmt& n)
      {
        ROSE_ASSERT(breakScope && currBlk);

        descend(n);

        recordScopeDestructors(n, *currBlk, enclosingScope(breakScope));
      }

      void handle(SgContinueStmt& n)
      {
        ROSE_ASSERT(continueScope && currBlk);

        descend(n);

        recordScopeDestructors(n, *currBlk, enclosingScope(continueScope));
      }

      void handle(SgReturnStmt& n)
      {
        ROSE_ASSERT(functionScope && currBlk);

        descend(n);

        recordScopeDestructors(n, *currBlk, destructionLimit(*functionScope));
      }

      void handle(SgGotoStatement& n)
      {
        ROSE_ASSERT(currBlk);

        descend(n);

        SgLabelStatement& tgt = SG_DEREF(n.get_label());

        recordScopeDestructors(n, *currBlk, enclosingScope(tgt.get_scope()));
      }

      void handle(SgBasicBlock& n)
      {
        currBlk = &n;

        descend(n);

        recordScopeDestructors(n, *currBlk, destructionLimit(enclosingScope(&n)));
      }

    private:
      SgBasicBlock*         currBlk       = nullptr;
      SgScopeStatement*     breakScope    = nullptr;
      SgScopeStatement*     continueScope = nullptr;
      SgFunctionDefinition* functionScope = nullptr;
  };

  void
  CxxObjectDestructionGenerator::recordScopeDestructors( SgStatement& n,
                                                         SgBasicBlock& start,
                                                         SgScopeStatement& limit
                                                       )
  {
    logTrace() << "pre-dtor call gen: " << typeid(n).name() << std::endl;
    record(ScopeDestructorTransformer{n, start, limit});
  }

  void CxxObjectDestructionGenerator::descend(SgNode& n)
  {
    ::ct::descend(*this, n);
  }

  void CxxObjectDestructionGenerator::loop(SgScopeStatement& n)
  {
    continueScope = breakScope = &n;

    descend(n);
  }

  struct CxxNormalizationCheck : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      // recursive tree traversal
      void descend(SgNode& n);

      void reportNode(SgStatement& n, SgLocatedNode& offender);

      void report(SgExpression& n)             { reportNode(sg::ancestor<SgStatement>(n), n); }

      void handle(SgNode& n)                   { descend(n); }

      void handle(SgConstructorInitializer& n) { report(n); }
      void handle(SgNewExp& n)                 { /* do not descend */ }
  };

  void CxxNormalizationCheck::descend(SgNode& n)
  {
    ::ct::descend(*this, n);
  }

  void CxxNormalizationCheck::reportNode(SgStatement& n, SgLocatedNode& offender)
  {
    std::cerr << "wanted: " << n.unparseToString()
              << " <" << typeid(offender).name() << ">"
              << " parent = " << typeid(*n.get_parent()).name()
              << "\n@" << SrcLoc(offender) << " / " << SrcLoc(*isSgLocatedNode(n.get_parent()))
              << std::endl;
  }

  template <class Transformer>
  GeneratorBase::container
  computeTransform(SgNode* root, CxxTransformStats& stats)
  {
    GeneratorBase::container transformations;
    GeneratorBase::node_set  nodes;

    sg::dispatch(Transformer{transformations, nodes}, root);

    stats.cnt += transformations.size();
    return transformations;
  }


  using CxxTransformGenerator = std::function<std::vector<AnyTransform>(SgNode*, CxxTransformStats&)>;

  void normalize(CxxTransformGenerator gen, SgNode* root, CxxTransformStats& stats)
  {
    for (AnyTransform& tf : gen(root, stats))
      tf.execute(stats);
  }

  template <class Transformer>
  void normalize(SgNode* root, std::string msg)
  {
    CxxTransformStats stats;

    normalize(computeTransform<Transformer>, root, stats);
    logInfo() << "Completed " << stats << ' ' << msg
              << std::endl;
  }


  // conveniance
  void normalizeCtorDtor(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxCtorDtorGenerator>, root, stats);
  }
} // anonymous namespace


  //
  // externally visible function

  void normalizeCxx1(Normalization& norm, SgNode* root)
  {
    logInfo() << "Starting C++ normalization. (Phase 1/2)" << std::endl;
    logTrace() << "Not normalizing templates.." << std::endl;

    //~ normalize<CxxCtorDtorGenerator>(root, "terrific C++ ctor/dtor normalizations...");

    logInfo() << "Finished C++ normalization. (Phase 1/2)" << std::endl;
  }

  void normalizeCxx2(Normalization& norm, SgNode* root)
  {
    logInfo() << "Starting C++ normalization. (Phase 2/2)" << std::endl;
    logTrace() << "Not normalizing templates.." << std::endl;

    normalize<CxxCtorDtorGenerator>         (root, "awesome C++ ctor/dtor normalizations...");
    normalize<CxxAllocInitSplitGenerator>   (root, "brilliant C++ alloc/init splits...");
    normalize<CxxRVOGenerator>              (root, "crucial C++ return value optimizations...");
    normalize<CxxObjectDestructionGenerator>(root, "delightful C++ object destruction insertion...");
    normalize<CxxNormalizationCheck>        (root, "checking AST for unwanted nodes...");

    clearMemoized();
    logInfo() << "Finished C++ normalization. (Phase 2/2)" << std::endl;
  }

  bool cppCreatesTemporaryObject(const SgExpression* n, bool withCplusplus)
  {
    if (!withCplusplus) return false;

    ASSERT_not_null(n);

    const SgClassType* ty = isSgClassType(n->get_type());

    return ty && !si::IsTrivial(ty);

/*
    const SgConstructorInitializer* init = isSgConstructorInitializer(n);

    if (!init) return false;

    // exclude variable declarations
    return isSgInitializedName(init->get_parent()) == nullptr;
*/
  }

  bool cppReturnValueOptimization(const SgReturnStmt* n, bool withCplusplus)
  {
    return withCplusplus && n && isSgConstructorInitializer(n->get_expression());
  }
} // CodeThorn namespace
