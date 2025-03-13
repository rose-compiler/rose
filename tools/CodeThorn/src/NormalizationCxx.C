#include "sage3basic.h"
#include "sageBuilder.h"
#include "sageGeneric.h"
#include "sageInterface.h"
#include "RoseAst.h"

#include <memory>
#include <boost/range/adaptor/reversed.hpp>

#include "Normalization.h"
#include "NormalizationCxx.h"
#include "RoseCompatibility.h"
#include "Memoizer.h"

namespace sb = SageBuilder;
namespace si = SageInterface;
namespace ct = CodeThorn;
namespace adapt = boost::adaptors;

namespace
{
  SgScopeStatement&
  enclosingScope(SgNode& n)
  {
    return SG_DEREF(si::getEnclosingScope(&n));
  }

  SgScopeStatement&
  enclosingScope(sg::NotNull<SgNode> n)
  {
    return enclosingScope(*n);
  }
}

namespace CodeThorn
{
namespace
{
  struct GlobalClassAnalysis
  {
      static
      const ClassAnalysis& get()
      {
        return SG_DEREF(globalClassAnalysis);
      }

      static
      void init(SgProject* prj)
      {
        if (globalClassAnalysis)
          return;

        globalClassAnalysis = new ClassAnalysis(ct::analyzeClasses(prj));
      }

      static
      void clear()
      {
        delete globalClassAnalysis;
      }

    private:
      static ClassAnalysis* globalClassAnalysis;
  };

  ClassAnalysis* GlobalClassAnalysis::globalClassAnalysis = nullptr;

  std::ostream& operator<<(std::ostream& os, const CxxTransformStats& stat)
  {
    return os << stat.cnt;
  }

  constexpr bool ERROR_TOLERANT = true;

  // auxiliary wrapper for printing Sg_File_Info objects
  struct SrcLoc
  {
    explicit
    SrcLoc(SgLocatedNode& n)
    : info(n.get_file_info())
    {}

    sg::NotNull<Sg_File_Info> info;
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

    ASSERT_require(lst.size() == 1 && lst[0]);
    return *lst[0];
  }

  SgBasicBlock&
  getBody(const SgFunctionDeclaration& n)
  {
    const SgFunctionDeclaration& defdecl = SG_DEREF(isSgFunctionDeclaration(n.get_definingDeclaration()));
    const SgFunctionDefinition& fundef = SG_DEREF(defdecl.get_definition());

    return SG_DEREF(fundef.get_body());
  }

  SgType&
  getReturnType(const SgFunctionDeclaration& n)
  {
    return SG_DEREF(SG_DEREF(n.get_type()).get_return_type());
  }

  void copyStatements(const SgBasicBlock& src, SgBasicBlock& tgt)
  {
    for (const SgStatement* orig : src.get_statements())
    {
      SgStatement* copy = si::deepCopy(orig);

      tgt.append_statement(copy);
    }
  }

  void replaceExpression(SgExpression& curr, SgExpression& repl)
  {
    //~ repl.set_parent(curr.get_parent());
    si::replaceExpression(&curr, &repl, true /* no delete */);
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

      void execute(CxxTransformStats& stat) override
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
  // convenience functions + functors

  SgBasicBlock& getCtorBody(const SgMemberFunctionDeclaration& n)
  {
    SgFunctionDefinition& def = SG_DEREF(n.get_definition());

    return SG_DEREF(def.get_body());
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
    check(sg::NotNull<SgClassDefinition> n);
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
  isCtor(SgMemberFunctionDeclaration& n)
  {
    return n.get_specialFunctionModifier().isConstructor()? &n : nullptr;
  }

  SgMemberFunctionDeclaration*
  isCtorDtor(SgMemberFunctionDeclaration& n)
  {
    SgMemberFunctionDeclaration* res = nullptr;

    (res = isCtor(n)) || (res = isDtor(n));
    return res;
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
  TriviallyDestructible::check(sg::NotNull<SgClassDefinition> def)
  {
    bool trivial_class = true;

    // this loop checks if all members are trivially destructable
    //   and if the class has a declared destructor
    for (sg::NotNull<SgDeclarationStatement> mem : def->get_members())
    {
      if (!(TriviallyDestructibleDecl::check(mem).first))
        trivial_class = false;

      if (SgMemberFunctionDeclaration* cand = isSgMemberFunctionDeclaration(mem))
      {
        if (isDtor(*cand)) return std::make_pair(false, cand);
      }
    }

    if (!trivial_class) return std::make_pair(false, nullptr);

    for (sg::NotNull<SgBaseClass> baseclass : def->get_inheritances())
    {
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

    bool operator()(sg::NotNull<SgInitializedName> cand)
    {
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

    SgInitializedName* initname = nullptr;
  };

  struct SameClassDef
  {
      explicit
      SameClassDef(const SgClassDefinition& base)
      : classdef(base)
      {}

      bool operator()(SgInitializedName* cand)
      {
        if (ERROR_TOLERANT && !(cand && cand->get_initializer()))
        {
          throw ConstructorInitializerListError("unusual constructor list element", cand);
        }

        ASSERT_require(cand && cand->get_initializer());
        SgConstructorInitializer* ctorini = isSgConstructorInitializer(cand->get_initializer());

        // if it is not a base class initialization, it must be member variable initialization
        //   -> ignore
        // \todo once we see a member variable initialization, the base class was not found,
        //       and the iteration can be aborted.
        if (!ctorini) return false;

        SgMemberFunctionDeclaration& mfn = SG_DEREF( ctorini->get_declaration() );

        return &getClassDefForFunction(mfn) == &classdef;
      }

    private:
      const SgClassDefinition& classdef;
  };



  // finds the initializer for a specific base class in the constructor initializer list
  // returns nullptr if the class is default initialized.
  // \{
  SgConstructorInitializer*
  getBaseInitializer(const SgClassDefinition& clsdef, SgCtorInitializerList& ctorini)
  {
    SgInitializedNamePtrList&                lst = ctorini.get_ctors();
    SgInitializedNamePtrList::const_iterator pos = std::find_if(lst.begin(), lst.end(), SameClassDef{clsdef});

    return (pos != lst.end()) ? isSgConstructorInitializer((*pos)->get_initializer()) : nullptr;
  }

  SgConstructorInitializer*
  getBaseInitializer(const SgClassDeclaration& clsdcl, SgCtorInitializerList& ctorini)
  {
    return getBaseInitializer(getClassDef(clsdcl), ctorini);
  }

  SgConstructorInitializer*
  getBaseInitializer(const SgBaseClass& base, SgCtorInitializerList& ctorini)
  {
    return getBaseInitializer(SG_DEREF(base.get_base_class()), ctorini);
  }
  // \}


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
                    SgType& objTy,
                    SgMemberFunctionDeclaration& fnDcl,
                    SgExprListExp& args,
                    bool virtualCall = false
                  );

  Sg_File_Info* dummyFileInfo()
  {
    return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  }

  SgForStatement&
  createArrayElementOperation( SgExpression& elem,
                               SgMemberFunctionDeclaration& fnDcl,
                               SgExprListExp& args,
                               SgExpression& limit,
                               bool virtualCall = false
                             )
  {
    // loop skeleton
    SgForStatement&        sgnode  = SG_DEREF( new SgForStatement(dummyFileInfo()) );
    SgExpression&          start   = SG_DEREF( sb::buildIntVal(0) );
    std::string            varname = si::generateUniqueVariableName(&sgnode, "_tmploopvar");
    SgInitializer&         varini  = SG_DEREF( sb::buildAssignInitializer(&start, start.get_type()) );
    SgVariableDeclaration& var     = SG_DEREF( sb::buildVariableDeclaration(varname, sb::buildIntType(), &varini, &sgnode) );

    ASSERT_not_null(sgnode.get_for_init_stmt());
    sgnode.append_init_stmt(&var);

    // test
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
    SgType&                idxTy   = SG_DEREF( indexed.get_type() );
    SgStatement&           bdy     = createMemberCall(indexed, idxTy, fnDcl, args, virtualCall);

    sg::linkParentChild(sgnode, bdy, &SgForStatement::set_loop_body);

    // done
    return sgnode;
  }


  SgExpression&
  createMemberSelection(SgExpression& expr, SgMemberFunctionRefExp& fn)
  {
    SgExpression& res = si::isPointerType(expr.get_type())
                           ? static_cast<SgExpression&>(SG_DEREF(sb::buildArrowExp(&expr, &fn)))
                           : SG_DEREF(sb::buildDotExp(&expr, &fn))
                           ;

    return res;
  }

  SgStatement&
  createMemberCall( SgExpression& obj,
                    SgType& objTy,
                    SgMemberFunctionDeclaration& fnDcl,
                    SgExprListExp& args,
                    bool virtualCall
                  )
  {
    if (SgArrayType* arrTy = isArrayType(&objTy))
    {
      SgExpression& upperBound = SG_DEREF(si::deepCopy(arrTy->get_index()));

      return createArrayElementOperation(obj, fnDcl, args, upperBound, virtualCall);
    }

    SgSymbol*               sym    = fnDcl.search_for_symbol_from_symbol_table();
    SgMemberFunctionSymbol& fnSym  = SG_DEREF(isSgMemberFunctionSymbol(sym));
    SgMemberFunctionRefExp& fnRef  = SG_DEREF(sb::buildMemberFunctionRefExp( &fnSym,
                                                                             virtualCall, /* are these reliably set? */
                                                                             !virtualCall /* qualifiers are needed for non-virtual calls */
                                                                           ));

    SgExpression&           target = createMemberSelection(obj, fnRef);
    SgExpression&           call   = SG_DEREF(sb::buildFunctionCallExp(&target, &args));

    return SG_DEREF(sb::buildExprStatement(&call));
  }

  SgVarRefExp& mkVarRefExp(SgInitializedName& var)
  {
    SgSymbol*         symbol = var.search_for_symbol_from_symbol_table();
    SgVariableSymbol& varSym = SG_DEREF(isSgVariableSymbol(symbol));

    return SG_DEREF(sb::buildVarRefExp(&varSym));
  }

    /// returns static_cast<Base*>(this)
  SgThisExp&
  mkThisExp(const SgClassDeclaration& clsdcl)
  {
    SgSymbol& clssym = SG_DEREF(clsdcl.search_for_symbol_from_symbol_table());

    return SG_DEREF(sb::buildThisExp(&clssym));
  }

  SgExpression&
  mkThisExpForBase(const SgClassDeclaration& clsdcl, const SgClassDeclaration& base, bool cst = false)
  {
    SgThisExp&          self = mkThisExp(clsdcl);
    sg::NotNull<SgType> bseptr = base.get_type();

    if (cst) bseptr = sb::buildConstType(bseptr);

    bseptr = sb::buildPointerType(bseptr);

    return SG_DEREF(sb::buildCastExp(&self, bseptr, SgCastExp::e_static_cast));
  }

  /// returns static static_cast<const Base&>(var)
  SgExpression&
  mkVarRefForBase(SgInitializedName& var, const SgClassDeclaration& base, bool cst = false)
  {
    SgVarRefExp&        varref = mkVarRefExp(var);
    sg::NotNull<SgType> bseptr = base.get_type();

    if (cst) bseptr = sb::buildConstType(bseptr);

    bseptr = sb::buildReferenceType(bseptr);
    return SG_DEREF(sb::buildCastExp(&varref, bseptr, SgCastExp::e_static_cast));
  }


    /// returns this->fld
  SgExpression& mkFieldAccess(const SgClassDeclaration& clsdcl, SgInitializedName& fld)
  {
    SgThisExp&   lhs = mkThisExp(clsdcl);
    //~ SgVarRefExp& rhs = SG_DEREF(sb::buildVarRefExp(&fld, nullptr /* not used */));
    SgVarRefExp& rhs = mkVarRefExp(fld);

    return SG_DEREF(sb::buildArrowExp(&lhs, &rhs));
  }

  /// returns var.fld
  SgExpression& mkFieldAccess(SgInitializedName& var, SgInitializedName& fld)
  {
    //~ SgVarRefExp& lhs = SG_DEREF(sb::buildVarRefExp(&var, nullptr /* unused */));
    //~ SgVarRefExp& rhs = SG_DEREF(sb::buildVarRefExp(&fld, nullptr /* unused */));
    SgVarRefExp& lhs = mkVarRefExp(var);
    SgVarRefExp& rhs = mkVarRefExp(fld);

    return SG_DEREF(sb::buildDotExp(&lhs, &rhs));
  }


  SgStatement&
  createMemberCall( SgInitializedName& var,
                    SgMemberFunctionDeclaration& fnDcl,
                    SgExprListExp& args,
                    bool virtualCall = false
                  )
  {
    SgVarRefExp& varRef = mkVarRefExp(var);
    SgType&      varTy  = SG_DEREF(varRef.get_type());

    return createMemberCall(varRef, varTy, fnDcl, args, virtualCall);
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
  createMemberCallFromConstructorInitializer(SgExpression& obj, SgType& objTy, SgConstructorInitializer& ini)
  {
    SgConstructorInitializer&    ctorInit = SG_DEREF(constructorInitializer(ini));
    SgMemberFunctionDeclaration& ctorDecl = SG_DEREF(ctorInit.get_declaration());
    SgExprListExp&               ctorArgs = SG_DEREF(si::deepCopy(ctorInit.get_args()));

    return createMemberCall(obj, objTy, ctorDecl, ctorArgs);
  }

  SgStatement&
  createMemberCallFromConstructorInitializer(SgInitializedName& var, SgConstructorInitializer& ini)
  {
    SgMemberFunctionDeclaration& ctorDecl = SG_DEREF(ini.get_declaration());
    SgExprListExp&               ctorArgs = SG_DEREF(si::deepCopy(ini.get_args()));

    return createMemberCall(var, ctorDecl, ctorArgs);
  }


  SgExprStatement&
  createMemberAssignFromAssignInitializer(SgInitializedName& var, SgAssignInitializer& ini)
  {
    //~ SgSymbol*         sym = var.search_for_symbol_from_symbol_table();
    //~ SgVariableSymbol& vsy = SG_DEREF(isSgVariableSymbol(sym));
    //~ SgVarRefExp&      lhs = SG_DEREF(sb::buildVarRefExp(&vsy));
    SgClassDeclaration& cls = sg::ancestor<SgClassDeclaration>(var);
    SgExpression&       lhs = mkFieldAccess(cls, var);
    SgExpression&       rhs = SG_DEREF(si::deepCopy(ini.get_operand()));
    SgExpression&       exp = SG_DEREF(sb::buildAssignOp(&lhs, &rhs));

    return SG_DEREF(sb::buildExprStatement(&exp));
  }


  //
  // C++ normalizing transformers

  SgMemberFunctionDeclaration*
  obtainDefaultCtorIfAvail(SgType& ty);

  /// inserts the initialization of a member variable into a block (i.e., ctor body).
  /// \note
  ///   the ast representation deviates from standard C++
  ///   struct S { S() {}; std::string s; };
  ///   => struct S { S() { s.string(); }; std::string s; };
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
          return &createMemberAssignFromAssignInitializer(var, *assignInit);

        logError() << "Unknown initializer type: "
                   << typeid(*ini).name()
                   << std::endl;
        ROSE_ABORT();
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

        // if there are no args and the name matches, mem becomes a candidate
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

  using SpecialFunctionModifierSetter = void (*) (SgSpecialFunctionModifier&);

  void noSpecialFunctionModifierSetter(SgSpecialFunctionModifier&)     {}
  void ctorSpecialFunctionModifierSetter(SgSpecialFunctionModifier& m) { m.setConstructor(); }
  void dtorSpecialFunctionModifierSetter(SgSpecialFunctionModifier& m) { m.setDestructor(); }

  SpecialFunctionModifierSetter
  specialFunctionModifierSetter(bool ctor)
  {
    return ctor ? ctorSpecialFunctionModifierSetter : dtorSpecialFunctionModifierSetter;
  }

  SgMemberFunctionDeclaration&
  mkMemfnDcl( SgClassDefinition& clsdef,
              std::string name,
              SgType& returnType,
              std::function<void (SgFunctionParameterList&, SgScopeStatement&)> completeParamList,
              SpecialFunctionModifierSetter specialModifierSetter = noSpecialFunctionModifierSetter
            )
  {
    SgName                       nm(name);
    SgFunctionParameterList&     lst = SG_DEREF(sb::buildFunctionParameterList());
    SgFunctionParameterScope&    psc = SG_DEREF(new SgFunctionParameterScope(dummyFileInfo()));

    markCompilerGenerated(lst);
    markCompilerGenerated(psc);
    completeParamList(lst, psc);

    SgMemberFunctionDeclaration& dcl = SG_DEREF(sb::buildNondefiningMemberFunctionDeclaration(nm, &returnType, &lst, &clsdef));

    markCompilerGenerated(dcl);

    specialModifierSetter(dcl.get_specialFunctionModifier());
    dcl.set_functionParameterScope(&psc);
    psc.set_parent(&dcl);
    dcl.set_firstNondefiningDeclaration(&dcl);
    return dcl;
  }

  SgMemberFunctionDeclaration&
  mkCtorDtor(SgClassDefinition& scope, bool ctor)
  {
    SgName nm{nameCtorDtor(scope, ctor)};
    auto   paramSetter = [](SgFunctionParameterList&, SgScopeStatement&) -> void {};

    return mkMemfnDcl(scope, nm, SG_DEREF(sb::buildVoidType()), paramSetter, specialFunctionModifierSetter(ctor));
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
  mkMemfnDef( SgClassDefinition& clsdef,
              SgMemberFunctionDeclaration& nondef,
              SpecialFunctionModifierSetter specialModifierSetter = noSpecialFunctionModifierSetter
            )
  {
    typedef SgTemplateInstantiationMemberFunctionDecl TemplateMemberFunction;

    ASSERT_require(nondef.get_definingDeclaration() == nullptr);

    SgName                       nm   = nondef.get_name();
    SgType&                      ty   = getReturnType(nondef);
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

    ASSERT_require(dcl.get_functionParameterScope() == nullptr);
    ASSERT_not_null(dcl.get_parent());
    ASSERT_not_null(dcl.get_definition());
    ASSERT_not_null(dcl.get_CtorInitializerList());
    ASSERT_not_null(nondef.get_definingDeclaration());

    specialModifierSetter(dcl.get_specialFunctionModifier());
    dcl.set_functionParameterScope(&psc);

    psc.set_parent(&dcl);
    return dcl;
  }

  SgMemberFunctionDeclaration&
  mkCtorDtorDef(SgClassDefinition& clsdef, SgMemberFunctionDeclaration& nondef, bool ctor)
  {
    return mkMemfnDef(clsdef, nondef, specialFunctionModifierSetter(ctor));
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
    ASSERT_require(ctorargs.get_expressions().size() == 0); // \todo handle copy and move ctors

    FindFunction::result res = FindFunction::find(clsdef, n, ctorargs);

    //~ logWarn() << n << " " << ctorargs.unparseToString() << " found " << FindFunction::found(res)
              //~ << std::endl;

    return FindFunction::found(res) ? FindFunction::declaration(res)
                                    : createCtorDtor(clsdef, n.at(0) != DTOR_PREFIX.at(0))
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

  template <class Pred>
  struct FunctionPredicate
  {
      FunctionPredicate(const std::string& fnName, Pred&& fnPred)
      : name(fnName), pred(std::move(fnPred))
      {}

      bool operator()(const SgDeclarationStatement* dcl) const
      {
        const SgMemberFunctionDeclaration* memfn = isSgMemberFunctionDeclaration(dcl);

        return memfn && (memfn->get_name() == name) && pred(*memfn);
      }

    private:
      const std::string& name;
      Pred  pred;
  };

  template <class Pred>
  FunctionPredicate<Pred>
  functionPredicate(const std::string& name, Pred&& pred)
  {
    return FunctionPredicate<Pred>{name, std::move(pred)};
  }


  template <class ArgumentListPredicate>
  std::vector<SgMemberFunctionDeclaration*>
  findEligibleMemberFunction(const SgClassDefinition& clsdef, const std::string& name, ArgumentListPredicate argpred)
  {
    using Iterator = SgDeclarationStatementPtrList::const_iterator;

    std::vector<SgMemberFunctionDeclaration*> res;
    const SgDeclarationStatementPtrList&      lst = clsdef.get_members();
    Iterator                                  zzz = lst.end();
    Iterator                                  pos = lst.begin();

    for (;;)
    {
      pos = std::find_if(pos, zzz, functionPredicate(name, std::move(argpred)));
      if (pos == zzz) break;

      res.push_back(isSgMemberFunctionDeclaration(*pos));
    }

    return res;
  }

  enum class CopyAssign : int
  {
    none                = 0,
    volatile_qual       = (1 << 0),
    const_qual          = (1 << 1),
    copy                = (1 << 2),
    ref                 = (1 << 3),
    const_volatile_qual = const_qual + volatile_qual,
    const_ref           = const_qual + ref
  };

  CopyAssign operator|(CopyAssign lhs, CopyAssign rhs)
  {
    return static_cast<CopyAssign>(lhs | rhs);
  }

  std::tuple<const SgType*, CopyAssign>
  skipCVQualifier(const SgType* ty, CopyAssign knd)
  {
    const SgModifierType*          modty = isSgModifierType(ty);

    if (!modty) return std::make_tuple(ty, knd);

    const SgConstVolatileModifier& mods = modty->get_typeModifier().get_constVolatileModifier();

    if (mods.isConst())    knd = knd | CopyAssign::const_qual;
    if (mods.isVolatile()) knd = knd | CopyAssign::volatile_qual;

    return skipCVQualifier(modty->get_base_type(), knd);
  }

  /// returns the kind of operator=
  CopyAssign
  copyAssignParam(const SgMemberFunctionType& memfn)
  {
    const SgTypePtrList& tylst = memfn.get_arguments();
    CopyAssign           res   = CopyAssign::none;

    if (tylst.size() != 1)
      return res;

    const SgType*        argty = tylst[0];

    if (const SgReferenceType* refty = isSgReferenceType(argty))
      std::tie(argty, res) = skipCVQualifier(refty->get_base_type(), CopyAssign::ref);

    if (memfn.get_class_type() != argty)
      return CopyAssign::none;

    return (res == CopyAssign::none) ? CopyAssign::copy : res;
  }

  CopyAssign
  copyAssignParam(sg::NotNull<const SgMemberFunctionDeclaration> memfn)
  {
    return copyAssignParam(SG_DEREF(isSgMemberFunctionType(memfn->get_type())));
  }

  bool
  copyAssignReturn(const SgMemberFunctionType& memfn)
  {
    SgReferenceType* refty = isSgReferenceType(memfn.get_return_type());

    return refty && (refty->get_base_type() == memfn.get_class_type());
  }

  CopyAssign
  copyAssignSignature(const SgMemberFunctionDeclaration& memfn)
  {
    const SgMemberFunctionType& fnty = SG_DEREF(isSgMemberFunctionType(memfn.get_type()));

    return copyAssignReturn(fnty) ? copyAssignParam(fnty) : CopyAssign::none;
  }

  bool
  hasCopyAssignSignature(const SgMemberFunctionDeclaration& memfn)
  {
    return copyAssignSignature(memfn) != CopyAssign::none;
  }

  SgMemberFunctionDeclaration*
  findEligibleCopyAssignment(const SgClassDefinition& clsdef)
  {
    std::vector<SgMemberFunctionDeclaration*> cands = findEligibleMemberFunction(clsdef, "operator=", hasCopyAssignSignature);

    if (cands.size() == 0) return nullptr;

    if (cands.size() > 1)
      logWarn() << "Multiple assignment operators are available. Choosing one RANDOMLY!"
                << std::endl;

    // it is not that random after all ...
    std::sort( cands.begin(), cands.end(),
               [](const SgMemberFunctionDeclaration* lhs, SgMemberFunctionDeclaration* rhs) -> bool
               {
                 // reverse the relationship in CopyAssign
                 //   -> moves const & to front
                 return copyAssignParam(lhs) > copyAssignParam(rhs);
               }
             );

    return cands[0];
  }

/*
  concept EmptyTransformer
  {
    EmptyTransformer(EmptyTransformer&&) = default;
    EmptyTransformer& operator=(EmptyTransformer&&) = default;

    EmptyTransformer() = delete;
    EmptyTransformer(const EmptyTransformer&) = delete;
    EmptyTransformer& operator=(const EmptyTransformer&&) = delete;

    void execute(CxxTransformStats&);
  };
*/


  struct CtorCallCreator
  {
      CtorCallCreator(SgClassDefinition& clz, SgClassDeclaration& basecls, SgConstructorInitializer* how)
      : cls(clz), bsedcl(basecls), ini(how)
      {}

      SgExpression& mkThisExpForBaseCall(SgClassDeclaration& base) const
      {
        return mkThisExpForBase(SG_DEREF(cls.get_declaration()), base);
      }

      SgStatement& mkDefaultInitializer(SgClassDeclaration& clazz, bool ctorcall) const
      {
        SgExprListExp&               args   = SG_DEREF( sb::buildExprListExp() );
        SgClassDefinition&           clsdef = getClassDef(clazz);
        SgMemberFunctionDeclaration& ctor   = ctorcall ? obtainGeneratableCtor(clsdef, args)
                                                       : obtainGeneratableDtor(clsdef, args);
        SgExpression&                self   = mkThisExpForBaseCall(clazz);
        SgType&                      selfTy = SG_DEREF( self.get_type() );

        return createMemberCall(self, selfTy, ctor, args, false /* non-virtual call */);
      }

      SgStatement& mkCtorCall(bool ctorcall = true) const
      {
        SgClassType& clsTy = SG_DEREF( bsedcl.get_type() );

        return ini ? createMemberCallFromConstructorInitializer(mkThisExpForBaseCall(bsedcl), clsTy, *ini)
                   : mkDefaultInitializer(bsedcl, ctorcall);
      }

    private:
      SgClassDefinition&        cls;
      SgClassDeclaration&       bsedcl;
      SgConstructorInitializer* ini;
  };


  struct BaseCtorCallTransformer : CtorCallCreator
  {
      using base = CtorCallCreator;

      BaseCtorCallTransformer(SgClassDefinition& clz, SgBasicBlock& where, SgBaseClass& baseclass, SgConstructorInitializer* how)
      : base(clz, SG_DEREF(baseclass.get_base_class()), how), blk(where)
      {}

      void execute(CxxTransformStats&)
      {
        newStmt = &base::mkCtorCall();

        blk.prepend_statement(newStmt);
      }

    private:
      SgBasicBlock&             blk;

      SgStatement*              newStmt = nullptr;
  };

/*
  SgFunctionSymbol&
  get_symbol(SgFunctionDeclaration& fundecl)
  {
    SgSymbol& symbl = SG_DEREF( fundecl.search_for_symbol_from_symbol_table() );

    return SG_ASSERT_TYPE(SgFunctionSymbol, symbl);
  }
*/

  struct BaseDtorCallTransformer
  {
      BaseDtorCallTransformer(const SgClassDeclaration& clazz, const SgBaseClass& what, SgBasicBlock& where)
      : clsdcl(clazz), baseclass(what), blk(where)
      {}

      SgStatement& mkDtorCall() const
      {
        SgExprListExp&               args    = SG_DEREF( sb::buildExprListExp() );
        SgClassDeclaration&          basedcl = SG_DEREF( baseclass.get_base_class() );
        SgMemberFunctionDeclaration& dtor    = obtainGeneratableDtor(getClassDef(basedcl), args);
        SgExpression&                baseobj = mkThisExpForBase(clsdcl, basedcl);
        SgType&                      baseTy  = SG_DEREF( baseobj.get_type() );

        return createMemberCall(baseobj, baseTy, dtor, args, false /* non-virtual call */);
      }

      void execute(CxxTransformStats&)
      {
        newStmt = &mkDtorCall();

        blk.append_statement(newStmt);
      }

    private:
      const SgClassDeclaration& clsdcl;
      const SgBaseClass&        baseclass;
      SgBasicBlock&             blk;

      SgStatement*              newStmt = nullptr;
  };


  /// compares two
  bool sameObject(const SgNode* lhs, const SgNode* rhs)
  {
    return dynamic_cast<const void*>(lhs) == dynamic_cast<const void*>(rhs);
  }

  bool isNormalizedSageNode(const SgLocatedNode& n)
  {
    static const std::string TRANSFORM = "transformation" ;

    ASSERT_not_null(n.get_file_info());

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
    if (!init) return false;

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
        //~ SgScopeStatement*            scope      = si::getEnclosingScope(&var);
        //~ SgExpression&                destructed = SG_DEREF(sb::buildVarRefExp(&var, scope));
        SgExpression&                destructed = mkVarRefExp(var);
        SgExprListExp&               args       = SG_DEREF(sb::buildExprListExp());
        SgMemberFunctionDeclaration& dtorDcl    = obtainGeneratableDtor(clsDef, args);

        // we know the full type, thus we can use a non-virtual call
        dtorCall = &createMemberCall(destructed, varTy, dtorDcl, args, false /* non-virtual call */);

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

  SgVariableDeclaration&
  mkParameter(std::string name, SgType& ty, SgScopeStatement& scope)
  {
    SgVariableDeclaration& sgnode = SG_DEREF(sb::buildVariableDeclaration(name, &ty, nullptr, &scope));
    //~ SgInitializedName&     parm   = SG_DEREF(sgnode.get_variables().at(0));

    // sgnode.set_parent(&scope);
    ASSERT_require(sgnode.get_parent() == &scope);
    ASSERT_not_null(sgnode.get_definingDeclaration());
    ASSERT_not_null(sgnode.get_firstNondefiningDeclaration());
    return sgnode;
  }


  struct CopyAssignmentTransformer
  {
      CopyAssignmentTransformer(SgClassDefinition& classdef, SgMemberFunctionDeclaration* memdcl)
      : clsdef(classdef), fndcl(memdcl)
      {}

      void execute(CxxTransformStats& stat)
      {
        logTrace() << "creating defining operator= .."
                   << std::endl;

        SgClassDeclaration&  clsdcl = SG_DEREF(clsdef.get_declaration());
        SgClassType&         clsTy  = SG_DEREF(clsdcl.get_type());

        if (fndcl == nullptr)
        {
          sg::NotNull<SgType> cnstRf    = sb::buildReferenceType(sb::buildConstType(&clsTy));
          auto                signature =
                [cnstRf](SgFunctionParameterList& lst, SgScopeStatement& scp) -> void
                {
                  SgVariableDeclaration&         decl = mkParameter("that", *cnstRf, scp);
                  SgInitializedNamePtrList&      args = lst.get_args();

                  ASSERT_require(decl.get_variables().size() == 1);
                  sg::NotNull<SgInitializedName> parm = decl.get_variables().front();

                  parm->set_parent(&lst);
                  args.push_back(parm);
                };

          SgType& clsRf = SG_DEREF(sb::buildReferenceType(&clsTy));

          gendcl = fndcl = &mkMemfnDcl(clsdef, "operator=", clsRf, signature);
        }

        ASSERT_require(fndcl && fndcl->get_definingDeclaration() == nullptr);

        gendef = &mkMemfnDef(clsdef, *fndcl);

        // traverse all sub-objects and call their operator= if needed
        const ::ct::ClassAnalysis& classes = GlobalClassAnalysis::get();
        SgInitializedNamePtrList&  parmlst = gendef->get_args();
        ASSERT_require(parmlst.size() == 1);
        SgInitializedName&         thatvar = SG_DEREF(parmlst.front());
        SgFunctionDefinition&      memdef = SG_DEREF(gendef->get_definition());
        SgBasicBlock&              body = SG_DEREF(memdef.get_body());

        // Traverse all bases and either call their (generated) operator= member function
        // or use assignment, if none is present.
        // \note
        //   We mimic assignment operators as generated by: gcc, clang, suncc, and xlc
        //   where virtual base classes are assigned multiple times.
        //   The rational is that this is the interpretation that is more likely to bring out
        //   bugs in the code.
        //   https://eel.is/c++draft/class.copy.assign
        // \todo Make it possible to choose semantics of the Intel compiler.
        //       A design could be similar to constructors where most derived
        //       and base constructors coexist.
        for (const ::ct::InheritanceDesc& inh : classes.at(clsdef).ancestors())
        {
          if (!inh.isDirect()) continue;

          const SgClassDefinition&     basecls   = SG_DEREF(inh.getClass());
          SgMemberFunctionDeclaration* basefn    = findEligibleCopyAssignment(basecls);
          SgClassDeclaration&          basedcl   = SG_DEREF(basecls.get_declaration());
          SgExpression&                self      = mkThisExpForBase(clsdcl, basedcl);
          SgExpression&                thatbase  = mkVarRefForBase(thatvar, basedcl, true /* const */);
          SgStatement*                 subassign = nullptr;

          if (basefn)
          {
            // generate assignment call: static_cast<Base*>(this)->operator=(static_cast<Base&>(that))
            SgType&        selfTy  = SG_DEREF(self.get_type());
            SgExprListExp& args    = SG_DEREF(sb::buildExprListExp(&thatbase));

            subassign = &createMemberCall(self, selfTy, *basefn, args, false /* non-virtual call */);
          }
          else
          {
            // generate C-style assignment: (*static_cast<Base*>(this)) = static_cast<Base&>(that)
            SgExpression& derefSelf = SG_DEREF(sb::buildPointerDerefExp(&self));

            subassign = sb::buildAssignStatement(&derefSelf, &thatbase);
          }

          ASSERT_require(subassign);
          body.append_statement(subassign);
        }

        // Traverse all members and call their operator= if present, otherwise just use
        // the a
        for (const SgInitializedName* mem : classes.at(clsdef).dataMembers())
        {
          SgInitializedName& var = SG_DEREF(const_cast<SgInitializedName*>(mem));
          SgExpression&      thisfld   = mkFieldAccess(clsdcl, var);
          SgClassDefinition* varcls    = getClassDefOpt(var);
          SgExpression&      thatfld   = mkFieldAccess(thatvar, var);
          SgStatement*       subassign = nullptr;

          if (SgMemberFunctionDeclaration* fldfn = varcls ? findEligibleCopyAssignment(*varcls) : nullptr)
          {
            // generate assignment call: this->var.operator=(that.var)
            SgType&        thisfldTy = SG_DEREF(thisfld.get_type());
            SgExprListExp& args      = SG_DEREF(sb::buildExprListExp(&thatfld));

            subassign = &createMemberCall(thisfld, thisfldTy, *fldfn, args, false /* non-virtual call */);
          }
          else
          {
            // generate C-style assignment: this->var = that.var
            subassign = sb::buildAssignStatement(&thisfld, &thatfld);
          }

          ASSERT_require(subassign);
          body.append_statement(subassign);
        }
      }

    private:
      SgClassDefinition&           clsdef;
      SgMemberFunctionDeclaration* fndcl;

      SgMemberFunctionDeclaration* gendcl = nullptr;
      SgMemberFunctionDeclaration* gendef = nullptr;
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
                                transformation_container& transf
                              )
  {
    SgInitializedNamePtrList vars = variableList(n, posInScope);

    for (sg::NotNull<SgInitializedName> var : adapt::reverse(vars))
    {
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

        transformation_container transf;

        SgStatement* prev = pos;
        for (sg::NotNull<SgScopeStatement> curr = blk; curr != limit; curr = &enclosingScope(*curr))
        {
          recordScopedDestructors(*curr, *prev, *blk, *pos, transf);

          prev = curr;
        }

        stat.cnt += transf.size();
        for (AnyTransform& tf : transf)
          tf.execute(stat);
      }

    private:
      ScopeDestructorTransformer() = delete;

      sg::NotNull<SgStatement>      const pos;
      sg::NotNull<SgBasicBlock>     const blk;
      sg::NotNull<SgScopeStatement> const limit;
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
        SgScopeStatement&      scope  = enclosingScope(newexp);
        std::string            nm     = si::generateUniqueVariableName(&scope, "tmpnew");
        SgType&                ty     = SG_DEREF(newexp.get_type());
        SgInitializer*         ini    = nullptr; // set later
        SgVariableDeclaration& vardcl = SG_DEREF(sb::buildVariableDeclaration(nm, &ty, ini, &scope));

        // set varsym
        newAllocStmt = &vardcl;
        varsym       = isSgVariableSymbol(onlyName(vardcl).search_for_symbol_from_symbol_table());
        ASSERT_not_null(varsym);
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
        SgPointerType&            expTy   = SG_DEREF(isSgPointerType(newexp.get_type()));
        SgType&                   allocTy = SG_DEREF(expTy.get_base_type());

        newCtorStmt = &createMemberCallFromConstructorInitializer(varref, allocTy, ini);

        SgStatement*              currentNewStmt = &sg::ancestor<SgStatement>(newexp);

        // execute transformation
        if (newAllocStmt)
        {
          ASSERT_require(newNewExpr == nullptr);
          si::replaceStatement(currentNewStmt, newAllocStmt, true /* move preproc info */);

          currentNewStmt = newAllocStmt;
        }
        else if (newNewExpr)
        {
          // replace new expression with a simplified placement new
          replaceExpression(newexp, SG_DEREF(newNewExpr));
        }

        // insert the initialization (constructor calls)
        si::insertStatement(currentNewStmt, newCtorStmt, false /* insert after */);
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
        SgType&        varTy     = SG_DEREF(varRef.get_type());

        // build new stmt
        SgExprListExp& args      = SG_DEREF( sb::buildExprListExp() );
        SgVarRefExp&   newVarRef = SG_DEREF(si::deepCopy(&varRef));

        // \todo check if dtor is a virtual function
        newDtorStmt = &createMemberCall(newVarRef, varTy, dtor, args, true /* virtual */);

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

    ASSERT_require(parms.size());
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
        ASSERT_require(  SgNodeHelper::isPointerType(parm.get_type())
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

        //~ SgScopeStatement& scope = sg::ancestor<SgScopeStatement>(var);
        //~ SgVarRefExp&      vref  = SG_DEREF(sb::buildVarRefExp(&var, &scope));
        SgVarRefExp&      vref = mkVarRefExp(var);
        SgExpression&     vptr = SG_DEREF(sb::buildAddressOfOp(&vref));

        si::appendExpression(&args, &vptr);

        SgExprStatement&  stmt = SG_DEREF(sb::buildExprStatement(&call));
        SgStatement&      prev = sg::ancestor<SgStatement>(var);

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
        sg::NotNull<SgInitializedName> parm =
           sb::buildInitializedName( RVOReturnStmtTransformer::returnParameterName,
                                     sb::buildPointerType(&ty),
                                     nullptr
                                   );

        /*SgVariableSymbol* sym =*/ si::appendArg(fn.get_parameterList(), parm);

        fnty.set_return_type(sb::buildVoidType());
      }

    private:
      SgFunctionDeclaration& fn;
      SgType&                ty;
  };

  std::string mkFullName(std::string fullName)
  {
    fullName += 'v';
    return fullName;
  }


  struct FullCtorDtorTransformer
  {
      using VirtualBaseContainer = ClassData::VirtualBaseOrderContainer;

      FullCtorDtorTransformer(SgMemberFunctionDeclaration& n, const VirtualBaseContainer& vbases)
      : ctordtor(n), virtualBases(vbases)
      {}

      FullCtorDtorTransformer(FullCtorDtorTransformer&& orig) = default;
      ~FullCtorDtorTransformer() = default;

      FullCtorDtorTransformer& operator=(FullCtorDtorTransformer&&) = delete;
      FullCtorDtorTransformer(const FullCtorDtorTransformer&) = delete;
      FullCtorDtorTransformer& operator=(const FullCtorDtorTransformer&) = delete;

      void execute(CxxTransformStats&)
      {
        using StmtInserter = void (SgBasicBlock::*)(SgStatement*);

        std::string                  fullName = mkFullName(ctordtor.get_name());
        SgClassDefinition&           clsdef   = getClassDefForFunction(ctordtor);
        const bool                   isConstructor = !isDtor(ctordtor);

        sg::NotNull<SgMemberFunctionDeclaration> fulldclNondef =
                  sb::buildNondefiningMemberFunctionDeclaration(
                                                                 fullName,
                                                                 &getReturnType(ctordtor),
                                                                 si::deepCopy(ctordtor.get_parameterList()),
                                                                 &clsdef,
                                                                 nullptr /*decoratorList*/,
                                                                 0 /*functionConstVolatileFlags*/,
                                                                 false /*buildTemplateInstantiation*/,
                                                                 nullptr /*templateArgumentsList*/
                                                               );

        clsdef.append_member(fulldclNondef);

        // create full ctor/dtor
        SgMemberFunctionDeclaration& fulldcl = mkCtorDtorDef(clsdef, *fulldclNondef, !isDtor(ctordtor));
        SgSymbol* sym = fulldcl.search_for_symbol_from_symbol_table();
        ASSERT_require(sym != ctordtor.search_for_symbol_from_symbol_table());


        // rename dtor/ctor

        fulldcl.set_name(fullName);

        SgBasicBlock&                funbdy = getBody(fulldcl);

        // copy the body
        copyStatements(getBody(ctordtor), funbdy);

        SgCtorInitializerList&       ctorlst  = SG_DEREF(ctordtor.get_CtorInitializerList());
        StmtInserter                 inserter = isCtor(ctordtor) ? &SgBasicBlock::prepend_statement
                                                                 : &SgBasicBlock::append_statement;

        // add virtual base class constructor calls to constructor body
        //~ for (const SgClassDefinition* bsecls : adapt::reverse(virtualBases))
        for (int i = virtualBases.size(); i != 0; --i)
        {
          sg::NotNull<const SgClassDefinition> bsecls = virtualBases.at(i-1);

          // create: static_cast<Base*>(this)->Base(args as needed);
          SgConstructorInitializer* ini = getBaseInitializer(*bsecls, ctorlst);
          CtorCallCreator           callCreator{clsdef, SG_DEREF(bsecls->get_declaration()), ini};
          SgStatement&              call   = callCreator.mkCtorCall(isConstructor);

          (funbdy.*inserter)(&call);
        }

        // add new constructor to class
        newDtorCtor = &fulldcl;
        clsdef.append_member(newDtorCtor);
      }

    private:
      SgMemberFunctionDeclaration&   ctordtor;
      const VirtualBaseContainer&    virtualBases;

      SgMemberFunctionDeclaration*   newDtorCtor = nullptr;
  };


  struct FullCtorDtorCallTransformer
  {
      explicit
      FullCtorDtorCallTransformer(SgMemberFunctionRefExp& n)
      : ctorDtorRef(n)
      {}

      static
      bool sameParameterTypes(SgMemberFunctionDeclaration& lhs, SgMemberFunctionDeclaration& rhs)
      {
        // \todo
        return true;
      }

      SgMemberFunctionDeclaration&
      findFullCtorDtor(SgMemberFunctionDeclaration& ctorDtor)
      {
        std::string        fullname = mkFullName(ctorDtor.get_name());
        SgClassDefinition& clsdef   = getClassDefForFunction(ctorDtor);

        for (SgDeclarationStatement* stmt : clsdef.get_members())
        {
          if (SgMemberFunctionDeclaration* cand = isSgMemberFunctionDeclaration(stmt))
            if ((cand->get_name() == fullname) && sameParameterTypes(ctorDtor, *cand))
              return *cand;
        }

        ROSE_ABORT();
      }

      void execute(CxxTransformStats&)
      {
        // find newly generated full constructor
        SgMemberFunctionDeclaration& ctorDtor = SG_DEREF(ctorDtorRef.getAssociatedMemberFunctionDeclaration());
        SgMemberFunctionDeclaration& fullCtorDtor = findFullCtorDtor(ctorDtor);
        SgSymbol*                    fullSym = fullCtorDtor.search_for_symbol_from_symbol_table();

        newCtorDtorRef = sb::buildMemberFunctionRefExp(isSgMemberFunctionSymbol(fullSym), false /*nonvirtual*/, false /*w/o qualifier*/);

        replaceExpression(ctorDtorRef, SG_DEREF(newCtorDtorRef));
      }

    private:
      SgMemberFunctionRefExp& ctorDtorRef;
      SgMemberFunctionRefExp* newCtorDtorRef = nullptr;
  };

  using TypeModifer = std::function<SgType*(SgType*)>;

  SgType* noTypeModifier(SgType* ty) { return ty; }

  TypeModifer typeBuilder(SgMemberFunctionDeclaration& fn)
  {
    TypeModifer                    res    = noTypeModifier;
    const SgDeclarationModifier&   dclmod = fn.get_declarationModifier();
    const SgConstVolatileModifier& mods   = dclmod.get_typeModifier().get_constVolatileModifier();

    if (mods.isConst())
      res = mods.isVolatile() ? sb::buildConstVolatileType : sb::buildConstType;
    else if (mods.isVolatile())
      res = sb::buildVolatileType;

    return res;
  }

  struct ThisParameterTransformer
  {
      explicit
      ThisParameterTransformer(SgMemberFunctionDeclaration& memberFn)
      : memfn(memberFn)
      {}

      SgType& getThisType()
      {
        SgClassDefinition&  clsdef  = getClassDefForFunction(memfn);
        SgClassDeclaration& clsdcl  = SG_DEREF(clsdef.get_declaration());
        SgClassType&        clsty   = SG_DEREF(clsdcl.get_type());
        TypeModifer         modBldr = typeBuilder(memfn);

        return SG_DEREF(modBldr(&clsty));
      }

      void execute(CxxTransformStats&)
      {
        SgType& thisType = getThisType();

        newThisParam = sb::buildInitializedName_nfi("This", &thisType, nullptr);

        // memfn.prepend_arg(newThisParam);
        si::prependArg(memfn.get_parameterList(), newThisParam);
      }

    private:
      SgMemberFunctionDeclaration& memfn;
      SgInitializedName*           newThisParam = nullptr;
  };

  struct ThisPointerTransformer
  {
      ThisPointerTransformer(SgThisExp& thisNode, SgMemberFunctionDeclaration& memberFn)
      : self(thisNode), memfn(memberFn)
      {}

      SgInitializedName& findThisParam()
      {
        SgInitializedNamePtrList& lst = memfn.get_args();
        SgInitializedName&        cand = SG_DEREF(lst.at(0)); // a member function must have at least one argument

        ASSERT_require(cand.get_name() == "This");
        return cand;
      }

      void execute(CxxTransformStats&)
      {
        SgInitializedName& thisParam = findThisParam();

        // was: newThisExpr = sb::buildVarRefExp(&thisParam, memfn.get_definition());
        newThisExpr = &mkVarRefExp(thisParam);
        replaceExpression(self, *newThisExpr);
      }

    private:
      SgThisExp&                   self;
      SgMemberFunctionDeclaration& memfn;
      SgVarRefExp*                 newThisExpr = nullptr;
  };

  struct ThisArgumentTransformer
  {
      using ExprModifier = std::function<SgExpression& (SgExpression&)>;

      static
      SgExpression& noop(SgExpression& exp) { return exp; }

      static
      SgExpression& refToPtr(SgExpression& exp) { return SG_DEREF(sb::buildAddressOfOp(&exp)); }

      ThisArgumentTransformer(SgBinaryOp& exp, SgExprListExp& arguments, ExprModifier modifier)
      : binexp(exp), args(arguments), argModifier(modifier)
      {}

      void execute(CxxTransformStats&)
      {
        newReceiver = si::deepCopy(binexp.get_lhs_operand());
        newFunction = si::deepCopy(binexp.get_rhs_operand());
        newReceiver = &argModifier(SG_DEREF(newReceiver));

        args.prepend_expression(newReceiver);

        //~ std::cerr << binexp.get_parent()->unparseToString()
                  //~ << "\n    =>\n"
                  //~ << std::flush;

        replaceExpression(binexp, SG_DEREF(newFunction));

        //~ std::cerr << newFunction->get_parent()->unparseToString()
                  //~ << std::endl;
      }

    private:
      SgBinaryOp&    binexp;
      SgExprListExp& args;
      ExprModifier   argModifier;

      SgExpression*  newReceiver = nullptr;
      SgExpression*  newFunction = nullptr;
  };

  struct DefaultArgumentTransformer
  {
      DefaultArgumentTransformer(SgFunctionCallExp& callexp, const int argnum, SgFunctionDeclaration& dcl)
      : call(callexp), pos(argnum), func(dcl)
      {}

      sg::NotNull<SgExpression> defaultValue() const
      {
        SgFunctionParameterList& lst = SG_DEREF(func.get_parameterList());
        SgInitializedName&       prm = SG_DEREF(lst.get_args().at(pos));

        return prm.get_initializer();
      }

      void execute(CxxTransformStats&)
      {
        SgExprListExp& arguments = SG_DEREF(call.get_args());

        arguments.append_expression(si::copyExpression(defaultValue()));
      }

    private:
      SgFunctionCallExp&     call;
      const int              pos;
      SgFunctionDeclaration& func;
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

                       for (sg::NotNull<SgBaseClass> cand : cls->get_inheritances())
                       {
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

                       for (sg::NotNull<SgBaseClass> cand : cls->get_inheritances())
                       {
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

    GlobalClassAnalysis::clear();
  }

  // end memoized functors



  void normalizeCtorDef(SgMemberFunctionDeclaration& fun, transformation_container& cont)
  {
    if (!fun.get_definition()) return;

    SgBasicBlock&          blk = getCtorBody(fun);
    SgClassDefinition&     cls = getClassDefForFunction(fun);
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
        SgConstructorInitializer* ini = getBaseInitializer(*base, lst);

        cont.emplace_back(BaseCtorCallTransformer{cls, blk, *base, ini});
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
    // virtual base class initialization is normalized elsewhere
  }

  void normalizeDtorDef(SgMemberFunctionDeclaration& fun, transformation_container& cont)
  {
    if (!fun.get_definition()) return;

    SgBasicBlock&      blk = getCtorBody(fun);
    SgClassDefinition& cls = getClassDefForFunction(fun);

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

    const SgClassDeclaration& clsdcl = SG_DEREF(cls.get_declaration());

    // explicitly destruct all direct non-virtual base classes;
    //   execute the transformations in reverse order
    for (SgBaseClass* base : adapt::reverse(getDirectNonVirtualBases(&cls)))
    {
      cont.emplace_back(BaseDtorCallTransformer{clsdcl, *base, blk});
    }
  }


  SgScopeStatement&
  destructionLimit(SgFunctionDefinition& n)
  {
    sg::NotNull<SgScopeStatement> limit = &enclosingScope(n);

    // if n is a destructor include also the class' scope to destruct
    //   its data members.
    if (isDtor(n))
      return enclosingScope(*limit);

    return *limit;
  }

  /// if n's parent is a function definition, compute limit for function destruction,
  ///   otherwise return n.
  SgScopeStatement&
  destructionLimit(SgBasicBlock& n)
  {
    sg::NotNull<SgScopeStatement> limit = &enclosingScope(n);

    if (SgFunctionDefinition* fundef = isSgFunctionDefinition(limit))
      return destructionLimit(*fundef);

    return *limit;
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

      /// wrapper to access class hierarchy analysis
      const ct::ClassAnalysis&
      classAnalysis() const
      {
        return GlobalClassAnalysis::get();
      }

    private:
      GeneratorBase() = delete;
  };


  template <class SetT, class ElemT>
  inline
  bool alreadyProcessed(SetT& s, const ElemT& e)
  {
    return !s.insert(e).second;
  }

  template <class SetT, class ElemT, class ValT>
  inline
  bool alreadyProcessed(SetT& s, const ElemT& e, ValT v)
  {
    return !s.insert(std::make_pair(e, v)).second;
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

    sg::traverseDispatchedChildren(std::move(tf), n);
  }

  template <class Transformer>
  void descend_collect(Transformer& tf, SgNode& n)
  {
    if (alreadyProcessed(tf.visitedNodes(), &n))
      return;

    tf = sg::traverseDispatchedChildren(std::move(tf), n);
  }


  /// Adds constructors, destructors, operator= as needed and generates/normalizes these functions.
  /// If needed defining declarations for this functions will be generated.
  // \todo currently the incomplete declarations are identified in ideosyncratic
  //       ways. It would be nicer to find a more uniform way (if possible) to
  //       find declarations that need compiler generation.
  struct CxxCtorDtorGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      void handle(SgNode& n);

      void handle(SgCtorInitializerList&) { /* skip */ }

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

        handle(sg::asBaseType(n));
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

        handle(sg::asBaseType(n));
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

        handle(sg::asBaseType(n));
      }

      void handle(SgMemberFunctionRefExp& n)
      {
        // no need to handle SgMemberFunctionRefExp's base class
        // handle(sg::asBaseType(n));

        SgMemberFunctionDeclaration* memdcl = n.getAssociatedMemberFunctionDeclaration();

        if (memdcl == nullptr || memdcl->get_definingDeclaration())
          return;

        // \todo how can we distinguish a compiler generated copy-assignment operator from
        //       a user-defined copy-assignment operator that resides in an external
        //       translation unit?
        //       memdcl->isCompilerGenerated returns false for compiler generated ones

        if (memdcl->get_name() != std::string{"operator="})
          return;

        // check if base classes and all non-trivial members have copy assignment
        SgClassDefinition& clsdef = getClassDefForFunction(*memdcl);

        if (alreadyProcessed(classesWithCopyAssignment, &clsdef, true /* needs copy-assign */))
          return;

        /* bool basesOrMembersRequireCopyAssignment = */ checkCopyAssignInBasesAndMembers(clsdef);
        record(CopyAssignmentTransformer{clsdef, memdcl});
      }

    private:
      void descend(SgNode& n);

      /// check whether a class (or its bases and members) have or need
      /// a copy assignment function.
      /// \return true if the class structure requires a copy assignment operator;
      ///         false otherwise, indicating that standard assignment can be used.
      /// \{
      bool checkCopyAssignInClass(SgClassDefinition& clsdef);
      bool checkCopyAssignInBasesAndMembers(SgClassDefinition& clsdef);
      /// \}

      // tracks if a class has been handled, and whether the base has
      // an implemented copy assignment operator.
      std::map<SgClassDefinition*, bool> classesWithCopyAssignment;
  };


  bool CxxCtorDtorGenerator::checkCopyAssignInClass(SgClassDefinition& clsdef)
  {
    if (alreadyProcessed(classesWithCopyAssignment, &clsdef, false))
      return classesWithCopyAssignment[&clsdef];

    SgMemberFunctionDeclaration* memdcl = findEligibleCopyAssignment(clsdef);

    if (memdcl && memdcl->get_definingDeclaration())
    {
      classesWithCopyAssignment[&clsdef] = true;
      return true;
    }

    const bool basesOrMembersRequireCopyAssign = checkCopyAssignInBasesAndMembers(clsdef);
    const bool thisClassRequiresCopyAssign     = memdcl || basesOrMembersRequireCopyAssign;

    // only generate the copy assignment when a operator= is already present
    //   or if any base or member has a non-trivial copy assignment
    if (thisClassRequiresCopyAssign)
    {
      classesWithCopyAssignment[&clsdef] = true;
      record(CopyAssignmentTransformer{clsdef, memdcl});
    }

    return thisClassRequiresCopyAssign;
  }

  bool CxxCtorDtorGenerator::checkCopyAssignInBasesAndMembers(SgClassDefinition& clsdef)
  {
    bool                       res     = false;
    const ::ct::ClassAnalysis& classes = classAnalysis();

    // for all base classes
    for (const ct::InheritanceDesc& desc : classes.at(&clsdef).ancestors())
    {
      res = res || desc.isVirtual(); // required if there is a virtual base

      if (!desc.isDirect())
        continue;

      const SgClassDefinition& ancestorClass   = SG_DEREF(desc.getClass());
      SgClassDefinition&       ancestorCls     = const_cast<SgClassDefinition&>(ancestorClass);
      const bool               needsCopyAssign = checkCopyAssignInClass(ancestorCls);

      res = res || needsCopyAssign;
    }

    // for all members
    for (const SgInitializedName* datamem : classes.at(&clsdef).dataMembers())
    {
      SgClassDefinition* classdef = getClassDefOpt(SG_DEREF(datamem));

      if (!classdef)
        continue;

      const bool needsCopyAssign = checkCopyAssignInClass(*classdef);

      res = res || needsCopyAssign;
    }

    return res;
  }

  void CxxCtorDtorGenerator::handle(SgNode& n)
  {
    descend(n);
  }

  void CxxCtorDtorGenerator::descend(SgNode& n)
  {
    ::ct::descend_collect(*this, n);
  }


  /// passes over object initialization
  ///   breaks up an object declaration into allocation and initialization
  ///   transformations if A is a non-trivial user defined type:
  ///     A a = A(x, y, z); => A a; a.A(x, y, z);
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

      void handle(SgCtorInitializerList&) { /* skip */ }

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



  /// finds all allocation and deallocations (static and dynamic)
  ///   and replaces the constructors with constructors that also initialize
  ///   virtual base classes.
  /// e.g.,
  ///   struct A : virtual B { A() {} }; A a; a.A();
  ///     =>
  ///   struct A : virtual B { A() {} Av() { this->B(); } }; A a; a.Av();
  /// \details
  ///   Av is the full constructor that initializes both virtual and non-virtual base classes. It needs
  ///   to be called whenever an element of A is allocated. Similarly, ~Av is the full destructor.
  ///   The "normal" constructors A and destructor ~A are only called from a derived type of A.
  struct CxxVirtualBaseCtorDtorGenerator : GeneratorBase
  {
      using NewCtorDtorList = std::unordered_set<const SgMemberFunctionDeclaration*>;

      CxxVirtualBaseCtorDtorGenerator( GeneratorBase::container& transf,
                                       GeneratorBase::node_set& visited
                                     )
      : GeneratorBase(transf, visited), newCtorDtorList()
      {}

      using GeneratorBase::handle;

      void descend(SgNode& n);

      void handle(SgNode& n) { descend(n); }

      void handle(SgCtorInitializerList&) { /* skip */ }

      void handle(SgMemberFunctionRefExp& n)
      {
        using VirtualBaseContainer = ClassData::VirtualBaseOrderContainer;

        SgMemberFunctionDeclaration& memdcl   = SG_DEREF(n.getAssociatedMemberFunctionDeclaration());
        SgMemberFunctionDeclaration* ctordtor = isCtorDtor(memdcl);

        if (!ctordtor) return;

        ClassKeyType                classkey     = &getClassDefForFunction(memdcl);
        const ClassData&            classdesc    = classAnalysis().at(classkey);
        const VirtualBaseContainer& virtualBases = classdesc.virtualBaseClassOrder();

        // no transformation if there is no virtual base class
        if (virtualBases.empty()) return;

        // generate the new ctor dtor impl.
        if (!alreadyProcessed(newCtorDtorList, ctordtor))
        {
          record(FullCtorDtorTransformer{memdcl, virtualBases});
        }

        // replace the reference to the ctor with a reference to the full ctor.
        //   unless this is a base class construction/destruction
        record(FullCtorDtorCallTransformer{n});
      }

    private:
      NewCtorDtorList      newCtorDtorList;
  };


  void CxxVirtualBaseCtorDtorGenerator::descend(SgNode& n)
  {
    ::ct::descend_collect(*this, n);
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

      void handle(SgCtorInitializerList&) { /* skip */ }

      void handle(SgReturnStmt& n)
      {
        if (rvoFunc && !isSgNullExpression(n.get_expression()))
        {
          //~ if (!isSgConstructorInitializer(n.get_expression()))
            //~ std::cerr << ">> " << n.unparseToString()
                      //~ << "\n@" << sg::ancestor<SgFunctionDeclaration>(n).unparseToString()
                      //~ << std::endl;

          ASSERT_not_null(isSgConstructorInitializer(n.get_expression()));
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
      void descendWithCurrBlock(SgNode& n, SgBasicBlock*);

      void handle(SgNode& n)         { descend(n); }

      void handle(SgCtorInitializerList&) { /* skip */ }

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
        ASSERT_not_null(breakScope);
        ASSERT_not_null(currBlk);

        descend(n);
        recordScopeDestructors(n, *currBlk, enclosingScope(*breakScope));
      }

      void handle(SgContinueStmt& n)
      {
        ASSERT_not_null(continueScope);
        ASSERT_not_null(currBlk);

        descend(n);
        recordScopeDestructors(n, *currBlk, enclosingScope(*continueScope));
      }

      void handle(SgReturnStmt& n)
      {
        ASSERT_not_null(functionScope);
        ASSERT_not_null(currBlk);

        descend(n);
        recordScopeDestructors(n, *currBlk, destructionLimit(*functionScope));
      }

      void handle(SgGotoStatement& n)
      {
        ASSERT_not_null(currBlk);

        descend(n);
        recordScopeDestructors(n, *currBlk, enclosingScope(SG_DEREF(n.get_label()).get_scope()));
      }

      void handle(SgBasicBlock& n)
      {
        descendWithCurrBlock(n, &n);

        recordScopeDestructors(n, n, destructionLimit(n));
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

  void CxxObjectDestructionGenerator::descendWithCurrBlock(SgNode& n, SgBasicBlock* blk)
  {
    CxxObjectDestructionGenerator tmp(*this);

    tmp.currBlk = blk;
    ::ct::descend(std::move(tmp), n);
  }

  void CxxObjectDestructionGenerator::loop(SgScopeStatement& n)
  {
    continueScope = breakScope = &n;

    descend(n);
  }


  /// Makes this parameter explicit
  /// \details
  ///   Three transformations are generated:
  ///   - Introduce "This" parameter for non-static member functions as the first parameter
  ///     void x(); => void x(X* This); // assuming x is a member function of X
  ///   - Replace SgThisExp with SgVarRefExp to This
  ///     this->x => This->x
  ///   - Move the receiver object into the argument list when a non-static member function is called
  ///     a->f() => f(a)
  ///     a.f()  => f(&a)
  ///     (a.*f)() => f(&a)
  ///     (a->*f)() => f(a)
  struct CxxThisParameterGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      /// records this only iff n refers to a member function, not a member variable
      void
      recordIfMemberFnCall(SgBinaryOp& n, SgExprListExp& args, ThisArgumentTransformer::ExprModifier mod);

      // recursive tree traversal
      void descend(SgNode& n);

      void handle(SgNode& n)         { descend(n); }

      void handle(SgCtorInitializerList&) { /* skip */ }

      void handle(SgMemberFunctionDeclaration& n)
      {
        if (!si::isStatic(&n))
        {
          record(ThisParameterTransformer{n});

          memfn = &n;
        }

        descend(n);
      }

      void handle(SgThisExp& n)
      {
        record(ThisPointerTransformer{n, SG_DEREF(memfn)});
      }

      void handle(SgFunctionCallExp& n);

    private:
      SgMemberFunctionDeclaration* memfn = nullptr;
  };

  void CxxThisParameterGenerator::descend(SgNode& n)
  {
    ::ct::descend(*this, n);
  }

  void CxxThisParameterGenerator::recordIfMemberFnCall( SgBinaryOp& n,
                                                        SgExprListExp& args,
                                                        ThisArgumentTransformer::ExprModifier mod
                                                      )
  {
    // test if this is a call through a function pointer
    if (!isSgVarRefExp(n.get_rhs_operand()))
      record(ThisArgumentTransformer{n, args, mod});
  }

  void CxxThisParameterGenerator::handle(SgFunctionCallExp& n)
  {
    // find the this expressions first, before the call is updated
    descend(n);

    SgExprListExp& args = SG_DEREF(n.get_args());
    SgExpression*  tgt  = n.get_function();

    if (SgArrowExp* arrow = isSgArrowExp(tgt))
      recordIfMemberFnCall(*arrow, args, ThisArgumentTransformer::noop);
    else if (SgDotExp* dot = isSgDotExp(tgt))
      recordIfMemberFnCall(*dot, args, ThisArgumentTransformer::refToPtr);
    else if (SgArrowStarOp* arrowStar = isSgArrowStarOp(tgt))
      record(ThisArgumentTransformer{*arrowStar, args, ThisArgumentTransformer::noop});
    else if (SgDotStarOp* dotStar = isSgDotStarOp(tgt))
      record(ThisArgumentTransformer{*dotStar, args, ThisArgumentTransformer::refToPtr});
  }

  /// Predicate that returns true if a function declaration defines a default argument
  /// at a give position.
  struct DefinesDefaultValueAt
  {
    bool operator()(const SgFunctionDeclaration* n) const
    {
      SgFunctionParameterList&  paramlst = SG_DEREF(n->get_parameterList());
      SgInitializedNamePtrList& params   = paramlst.get_args();

      return SG_DEREF(params.at(pos)).get_initializer() != nullptr;
    }

    const int pos;
  };

  /// Adds the default argument of function declarations to call sites if needed.
  /// \details
  ///   - given a declaration: void foo(int i, double d = 2.2 * 3.3);
  ///     foo(0) => foo(0, 2.2 * 3.3);
  struct CxxDefaultArgumentGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;
      using DeclarationSet = std::vector<SgFunctionDeclaration*>;

      void reportAmbiguity(int pos, const DeclarationSet& candidates);
      void reportNone(int pos, const SgFunctionDeclaration& callee, const DeclarationSet& decls);

      int numberOfTrueParameters(SgFunctionParameterList& params, SgFunctionType& ty)
      {
        // ty.get_has_ellipses() does not work
        const SgFunctionParameterTypeList& paramTypeList = SG_DEREF(ty.get_argument_list());
        const SgTypePtrList&               tylst = paramTypeList.get_arguments();
        const bool hasEllipsis = tylst.size() && isSgTypeEllipse(tylst.back());
        const int  adj         = hasEllipsis ? 1 : 0;
        const int  len         = params.get_args().size();

        ASSERT_require(adj <= len);
        return len-adj;
      }

      void processCall(SgFunctionCallExp& n);

      // recursive tree traversal
      void descend(SgNode& n);

      void handle(SgNode& n)          { descend(n); }

      DeclarationSet& declarationSet(SgFunctionDeclaration& n)
      {
        sg::NotNull<SgFunctionDeclaration> key = isSgFunctionDeclaration(n.get_firstNondefiningDeclaration());

        return funcs[key];
      }

      void handle(SgClassDefinition& n)
      {
        // calls in classes are processed as if all declarations have been seen.
        //   => delay call processing until the entire body has been handled.
        std::vector<SgFunctionCallExp*>* savedDelayedCalls = delayedCalls;
        std::vector<SgFunctionCallExp*>  calls;

        delayedCalls = &calls;
        descend(n);
        delayedCalls = savedDelayedCalls;

        for (SgFunctionCallExp* callexp : calls)
          processCall(SG_DEREF(callexp));
      }

      void handle(SgFunctionCallExp& n)
      {
        descend(n);

        if (!n.getAssociatedFunctionDeclaration()) return;

        if (delayedCalls)
        {
          delayedCalls->push_back(&n);
          return;
        }

        processCall(n);
      }

      void handle(SgFunctionDeclaration& n)
      {
        descend(n);

        declarationSet(n).push_back(&n);
      }

    private:
      std::unordered_map<SgFunctionDeclaration*, DeclarationSet> funcs;
      std::vector<SgFunctionCallExp*>*                           delayedCalls = nullptr;
  };

  void CxxDefaultArgumentGenerator::processCall(SgFunctionCallExp& n)
  {
    // check if the number of arguments is less than the declared number of parameters
    SgFunctionDeclaration&   callee    = SG_DEREF(n.getAssociatedFunctionDeclaration());
    SgFunctionParameterList& params    = SG_DEREF(callee.get_parameterList());
    SgFunctionType&          fntype    = SG_DEREF(callee.get_type());
    const int                numparams = numberOfTrueParameters(params, fntype);
    SgExprListExp&           args      = SG_DEREF(n.get_args());
    int                      numargs   = args.get_expressions().size();

    // numargs > numparams for functions with ellipsis
    if (numargs >= numparams)
      return;

    // create the transformers
    const DeclarationSet&    declSet = declarationSet(callee);

    for ( ; numargs < numparams; ++numargs)
    {
      DeclarationSet candidates;

      std::copy_if( declSet.begin(), declSet.end(),
                    std::back_inserter(candidates),
                    DefinesDefaultValueAt{numargs}
                  );

      if (candidates.size() == 0) reportNone(numargs, callee, declSet);
      if (candidates.size() > 1)  reportAmbiguity(numargs, candidates);
      ASSERT_require(candidates.size());

      record(DefaultArgumentTransformer{n, numargs, *candidates.back()});
    }
  }

  void CxxDefaultArgumentGenerator::descend(SgNode& n)
  {
    ::ct::descend_collect(*this, n);
  }

  void CxxDefaultArgumentGenerator::reportAmbiguity(int pos, const DeclarationSet& candidates)
  {
    logError() << "Multiple function declarations defining a default value for parameter " << pos
               << " exist for function " << candidates.front()->get_name() << ".\n"
               << "checked " << candidates.size() << '\n'
               << std::endl;
  }

  void CxxDefaultArgumentGenerator::reportNone(int pos, const SgFunctionDeclaration& callee, const DeclarationSet& decls)
  {
    logError() << "No function declarations defining a default value for parameter " << pos
               << " exist for function " << callee.unparseToString() << ".\n"
               << "checked " << decls.size() << '\n'
               << std::endl;
  }


  /// checks if all links to unwanted AST nodes have been removed
  struct CxxNormalizationCheck : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      // recursive tree traversal
      void descend(SgNode& n);

      void reportNode(SgStatement& n, SgLocatedNode& offender);

      void report(SgExpression& n)             { reportNode(sg::ancestor<SgStatement>(n), n); }

      void handle(SgNode& n)                   { descend(n); }

      void handle(SgNewExp& n)                 { /* do not descend */ }

      void handle(SgConstructorInitializer& n) { report(n); }
      void handle(SgThisExp& n)                { report(n); }
  };

  void CxxNormalizationCheck::descend(SgNode& n)
  {
    ::ct::descend(*this, n);
  }

  void CxxNormalizationCheck::reportNode(SgStatement& n, SgLocatedNode& offender)
  {
    logError() << "wanted: " << n.unparseToString()
               << " <" << typeid(offender).name() << "> " << offender.unparseToString()
               << " parent = " << typeid(*n.get_parent()).name()
               << "\n@" << SrcLoc(offender) << " / " << SrcLoc(*isSgLocatedNode(n.get_parent()))
               << " in:\n" << n.get_parent()->unparseToString()
               << std::endl;
  }

  struct CxxCleanCtorInitlistGenerator : GeneratorBase
  {
      using GeneratorBase::GeneratorBase;
      using GeneratorBase::handle;

      // recursive tree traversal
      void descend(SgNode& n);

      void handle(SgNode& n)              { descend(n); }

      void handle(SgCtorInitializerList&) { /* skip */ }

      void handle(SgMemberFunctionDeclaration& n)
      {
        if (SgCtorInitializerList* lst = n.get_CtorInitializerList())
          record(CtorInitListTransformer{*lst});
      }
  };

  void CxxCleanCtorInitlistGenerator::descend(SgNode& n)
  {
    ::ct::descend(*this, n);
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


  using CxxTransformGenerator = std::function<transformation_container(SgNode*, CxxTransformStats&)>;

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

} // anonymous namespace

  //
  // externally visible functions

  void normalizeCtorDtor(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxCtorDtorGenerator>, root, stats);
  }

  void normalizeAllocInitSplit(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxAllocInitSplitGenerator>, root, stats);
  }

  void normalizeDefaultArgument(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxDefaultArgumentGenerator>, root, stats);
  }

  void normalizeRVO(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxRVOGenerator>, root, stats);
  }

  void normalizeObjectDestruction(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxObjectDestructionGenerator>, root, stats);
  }

  void normalizeVirtualBaseCtorDtor(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxVirtualBaseCtorDtorGenerator>, root, stats);
  }

  void normalizeCleanCtorInitlist(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxCleanCtorInitlistGenerator>, root, stats);
  }

  void normalizeThisParameter(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxThisParameterGenerator>, root, stats);
  }

  void normalizationCheck(SgNode* root, CxxTransformStats& stats)
  {
    normalize(computeTransform<CxxNormalizationCheck>, root, stats);
  }

  void normalizeCxx1(Normalization& norm, SgNode* root)
  {
    logInfo() << "Starting C++ normalization. (Phase 1/2)" << std::endl;
    logTrace() << "Not normalizing templates.." << std::endl;

    GlobalClassAnalysis::init(isSgProject(root));

    //~ normalize<CxxCtorDtorGenerator>(root, "terrific C++ ctor/dtor normalizations...");

    logInfo() << "Finished C++ normalization. (Phase 1/2)" << std::endl;
  }

  void normalizeCxx2(Normalization& norm, SgNode* root)
  {
    logInfo() << "Starting C++ normalization. (Phase 2/2)" << std::endl;
    logTrace() << "Not normalizing templates.." << std::endl;

    // create ctor/dtor if needed and not available
    normalize<CxxCtorDtorGenerator>           (root, "awesome C++ ctor/dtor normalizations...");

    // splits allocation and initialization
    normalize<CxxAllocInitSplitGenerator>     (root, "brilliant C++ alloc/init splits...");

    // adds default values to the argument list
    normalize<CxxDefaultArgumentGenerator>    (root, "crucial C++ default argument normalizations...");

    // return value optimization
    normalize<CxxRVOGenerator>                (root, "delightful C++ return value optimizations...");

    // inserts object destruction into blocks
    normalize<CxxObjectDestructionGenerator>  (root, "extraordinary C++ object destruction insertion...");

    // inserts calls to virtual base class constructor and destructor
    //   generates full constructors (those that initialize the virtual bases) and destructors if needed
    //   replaces references to constructors
    normalize<CxxVirtualBaseCtorDtorGenerator>(root, "fantastic C++ ctor/dtor w/ virtual base class normalizations...");

    // last pass that removes AST nodes from the initializer list. They have been moved into the appropriate
    // destructors, and now they are removed so that any subsequent traversal will not see them.
    normalize<CxxCleanCtorInitlistGenerator>  (root, "gigantic C++ ctor initializer list normalizations...");


    normalize<CxxThisParameterGenerator>      (root, "huge C++ this pointer normalizations...");

    // check if any unwanted nodes remain in the AST
    normalize<CxxNormalizationCheck>          (root, "checking AST for unwanted nodes...");

    clearMemoized();
    logInfo() << "Finished C++ normalization. (Phase 2/2)" << std::endl;
  }

  bool cppCreatesTemporaryObject(const SgExpression* n, bool withCplusplus)
  {
    ASSERT_not_null(n);

    if (!withCplusplus)
      return false;

    const SgType&      ty    = SG_DEREF(n->get_type());
    const SgClassType* clsty = isSgClassType(ty.stripType(STRIP_MODIFIER_ALIAS));

/*
    if (clsty)
      std::cerr << "PP22: " << typeid(*clsty).name()
                << " / " << typeid(*n).name()
                << std::endl;
*/
    return clsty && !si::IsTrivial(clsty);
  }

  bool cppNormalizedRequiresReference(const SgType* varTy, const SgExpression* exp)
  {
    ASSERT_not_null(varTy);

    varTy = varTy->stripType(SgType::STRIP_TYPEDEF_TYPE);

    if (isSgReferenceType(varTy))
      return false;

    SgType* beyondModifers = varTy->stripType(SgType::STRIP_MODIFIER_TYPE);

    return isSgClassType(beyondModifers) && !cppCreatesTemporaryObject(exp, true);
  }

  bool cppReturnValueOptimization(const SgReturnStmt* n, bool withCplusplus)
  {
    return withCplusplus && n && isSgConstructorInitializer(n->get_expression());
  }
} // CodeThorn namespace
