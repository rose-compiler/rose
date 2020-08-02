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
namespace adapt = boost::adaptors;

namespace CodeThorn
{
  // internal use (does not exclude templates)
  void normalizeCxx(SgNode* root);
  
namespace 
{
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
  
#if 0  
  //
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
#endif
    
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
  auto logTrace() -> decltype(Normalization::logger[Sawyer::Message::TRACE])
  {
    return Normalization::logger[Sawyer::Message::TRACE];  
  }
  
  //
  // transformation wrappers
  
  // borrowed from XPlacer
  struct BaseTransform
  {
      virtual ~BaseTransform() = default;
      BaseTransform()          = default;

      virtual void execute() = 0;
      
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
      PolyTransform(const ConcreteTransform& concreteTf)
      : tf(concreteTf)
      {}
      
      explicit
      PolyTransform(ConcreteTransform&& concreteTf)
      : tf(std::move(concreteTf))
      {}

      void execute() ROSE_OVERRIDE
      {
        tf.execute();
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
      // wrapping value ctors
      template <class ConcreteTransform>
      AnyTransform(const ConcreteTransform& a)
      : tf(new PolyTransform<ConcreteTransform>(a))
      {}
  
      template <class ConcreteTransform>
      AnyTransform(ConcreteTransform&& a)
      : tf(new PolyTransform<ConcreteTransform>(std::move(a)))
      {}
  
      AnyTransform(AnyTransform&& other)            = default;
      AnyTransform& operator=(AnyTransform&& other) = default;
      ~AnyTransform()                               = default;
  
      void execute() { tf->execute(); }      

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
    void handle(SgArrayType& n)    
    { 
      res = check(n.get_base_type());
    }
    
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
      
      auto endseq     = n.get_variables().end();
      bool nottrivial = endseq != std::find_if( n.get_variables().begin(), endseq,
                                             [](SgInitializedName* x) -> bool
                                             {
                                               ROSE_ASSERT(x);
                                               return !TriviallyDestructible::check(x->get_type());                                    
                                             }
                                           );
      
      if (nottrivial) nontrivial();
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
  
  struct SameClassDef
  {
    explicit
    SameClassDef(SgBaseClass& base)
    : classdef(getClassDef(SG_DEREF(base.get_base_class())))
    {}
    
    bool operator()(SgInitializedName* cand)
    {
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
  
  //
  // C++ normalizing transformers
  
  SgMemberFunctionDeclaration* 
  obtainDefaultCtorIfAvail(SgType& ty);
  
  /// inserts the initialization of a member variable into a block (i.e., ctor body).
  /// \note 
  ///   the ast representation deviates from standard C++
  ///   e.g., for struct S { S() {}; std::string s; };
  ///     varrefexp(s) = initializer-expr 
  ///   is inserted into the block
  struct VarCtorInserter
  {
      VarCtorInserter(SgBasicBlock& where, SgInitializedName& what, SgInitializer* how)
      : blk(where), var(what), ini(how) 
      {}
      
      SgInitializer* mkDefaultInitializer() 
      {
        SgType&                      varty = SG_DEREF(var.get_type());
        SgMemberFunctionDeclaration* ctor  = obtainDefaultCtorIfAvail(varty);
        
        if (ctor == nullptr)
        {
          // return an empty assign initializer, if the type does not have a ctor
          return sb::buildAssignInitializer(sb::buildNullExpression(), &varty);
        }
        
        SgClassDefinition&           clsdef = getClassDef(*ctor); 
        SgClassDeclaration*          clazz  = isSgClassDeclaration(clsdef.get_parent());
      
        ROSE_ASSERT(clazz);
        return sb::buildConstructorInitializer( ctor,
                                                sb::buildExprListExp(), 
                                                SgClassType::createType(clazz),
                                                false /* need name */,
                                                false /* need qualifier */,
                                                false /* need parenthesis after name */,
                                                false /* associated class unknown */
                                              );
      } 
      
      void execute() 
      {
        ini = ini ? si::deepCopy(ini) : mkDefaultInitializer();
        
        SgVariableSymbol* varsym = SG_ASSERT_TYPE(SgVariableSymbol, var.search_for_symbol_from_symbol_table());         
        SgVarRefExp*      varref = sb::buildVarRefExp(varsym);
        SgAssignOp*       assign = sb::buildAssignOp(varref, ini);
        
        blk.prepend_statement(sb::buildExprStatement(assign));
      }
      
    private:
      SgBasicBlock&      blk;
      SgInitializedName& var;
      SgInitializer*     ini;
  };
  
  
  bool isConvertibleTo(SgExpression& expr, SgInitializedName& parm)
  {
    // \todo implement 
    return true;
  }
  
  bool parametersHaveDefaultValues(SgMemberFunctionDeclaration*, SgInitializedNamePtrList& parms, size_t from)
  {
    const size_t eoparams = parms.size();
    
    for (size_t i = from; i < eoparams; ++i)
    {
      SgInitializedName& parm = SG_DEREF(parms.at(i));
      
      if (!parm.get_initializer())
        return false;
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
        SgInitializedNamePtrList& 	 parms = fplst.get_args();
        SgExpressionPtrList& 	       exprs = args->get_expressions();
        
        if (exprs.size() > parms.size()) 
          return false;
        
        // \todo we need to ONLY check that mem is a ctor, dtor, cctor, or mctor (nothing else)
        for (size_t i = 0; i < exprs.size(); ++i)
        {
          if (!isConvertibleTo(SG_DEREF(exprs.at(i)), SG_DEREF(parms.at(i))))
            return false;
        }
        
        return parametersHaveDefaultValues(mem, parms, exprs.size());
      }

      static
      result      
      find(SgClassDefinition& clsdef, const std::string& n, SgExprListExp* args = nullptr)
      {
        SgDeclarationStatementPtrList&          lst = clsdef.get_members();
        SgDeclarationStatementPtrList::iterator zzz = lst.end();
        
        return result(std::find_if(lst.begin(), zzz, FindFunction(n, args)), zzz);
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

  Sg_File_Info* dummyFileInfo()
  {
    return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  }

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
    constexpr unsigned char STRIP_MODIFIER_ALIAS = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_TYPEDEF_TYPE; 
    
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


  struct BaseCtorInserter
  {
      BaseCtorInserter(SgBasicBlock& where, SgBaseClass& what, SgInitializer* how)
      : blk(where), baseclass(what), ini(how) 
      {}
      
      SgInitializer* mkDefaultInitializer() const 
      {
        SgExprListExp&               args  = SG_DEREF( sb::buildExprListExp() );
        SgClassDeclaration&          clazz = SG_DEREF( baseclass.get_base_class() );
        SgMemberFunctionDeclaration& ctor  = obtainGeneratableCtor(getClassDef(clazz), args);      
  
        return sb::buildConstructorInitializer( &ctor,
                                                &args, 
                                                SgClassType::createType(&clazz),
                                                false /* need name */,
                                                false /* need qualifier */,
                                                false /* need parenthesis after name */,
                                                false /* associated class unknown */
                                              );
      } 
      
      void execute() 
      {
        ini = ini ? si::deepCopy(ini) : mkDefaultInitializer();
        
        blk.prepend_statement(sb::buildExprStatement(ini));
      }
    
    private:  
      SgBasicBlock&  blk;
      SgBaseClass&   baseclass;
      SgInitializer* ini;
  };
  
  SgFunctionSymbol& 
  get_symbol(SgFunctionDeclaration& fundecl)
  {
    SgSymbol& symbl = SG_DEREF( fundecl.search_for_symbol_from_symbol_table() );
    
    return SG_ASSERT_TYPE(SgFunctionSymbol, symbl);
  }
  
  struct BaseDtorInserter
  {
      BaseDtorInserter(SgBasicBlock& where, SgBaseClass& what)
      : blk(where), baseclass(what) 
      {}
      
      SgStatement* mkDtorCall() const
      {
        SgExprListExp&               args  = SG_DEREF( sb::buildExprListExp() );
        SgClassDeclaration&          clazz = SG_DEREF( baseclass.get_base_class() );
        SgMemberFunctionDeclaration& dtor  = obtainGeneratableDtor(getClassDef(clazz), args);
        SgFunctionSymbol&            symbl = get_symbol(dtor);
        SgFunctionCallExp&           call  = SG_DEREF( sb::buildFunctionCallExp(&symbl, &args) );
  
        return sb::buildExprStatement(&call);
      } 
      
      void execute() { blk.append_statement(mkDtorCall()); }
    
    private:  
      SgBasicBlock& blk;
      SgBaseClass&  baseclass;
  };
  
  template <class SageNode, class SageChild>
  void set_child(SageNode& parent, void (SageNode::*setter) (SageChild*), SageChild& child)
  {
    (parent.*setter)(&child);
    child.set_parent(&parent);
  }
  
  
  struct DtorCallCreator : sg::DispatchHandler<SgStatement*>
  {
    typedef sg::DispatchHandler<SgStatement*> base;
    
    DtorCallCreator(SgExpression& expr)
    : base(), elem(expr)
    {}
    
    void descend(SgNode* n) { res = sg::dispatch(*this, n); }
 
    SgForStatement&
    createLoopOverArray(SgArrayType& arrty)
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
      
      set_child(sgnode, &SgForStatement::set_test, ltstmt);
      
      // increment
      SgExpression&          varinc  = SG_DEREF( sb::buildVarRefExp(&var) );
      SgExpression&          inc     = SG_DEREF( sb::buildPlusPlusOp(&varinc) );
      
      set_child(sgnode, &SgForStatement::set_increment, inc);
      
      // index expression + body recursion
      SgExpression&          varidx  = SG_DEREF( sb::buildVarRefExp(&var) );
      SgExpression&          indexed = SG_DEREF( sb::buildPntrArrRefExp(&elem, &varidx) );
      SgStatement*           bdy     = sg::dispatch(DtorCallCreator(indexed), arrty.get_base_type());
      
      set_child(sgnode, &SgForStatement::set_loop_body, SG_DEREF(bdy));
      
      // done 
      return sgnode;
    }
    
    void handle(SgNode& n)         { SG_UNEXPECTED_NODE(n); }
    
    void handle(SgModifierType& n) { descend(n.get_base_type()); }
    void handle(SgTypedefType& n)  { descend(n.get_base_type()); }
    
    void handle(SgClassType& n)
    {
      SgClassDefinition&           clsdef   = SG_DEREF( getClassDefOpt(n) );
      SgExprListExp&               args     = SG_DEREF( sb::buildExprListExp() );
      SgMemberFunctionDeclaration& dtordcl  = obtainGeneratableDtor(clsdef, args);
      SgMemberFunctionSymbol*      mfunsym  = SG_ASSERT_TYPE(SgMemberFunctionSymbol, dtordcl.search_for_symbol_from_symbol_table());
      SgMemberFunctionRefExp&      mfunref  = SG_DEREF( sb::buildMemberFunctionRefExp( mfunsym,
                                                                                       false /* \todo virtual call */,
                                                                                       false /* need qualifier */
                                                                                     ));
      SgDotExp&                    callee   = SG_DEREF( sb::buildDotExp(&elem, &mfunref) );
      SgFunctionCallExp&           callexp  = SG_DEREF( sb::buildFunctionCallExp(&callee, &args) );
      
      res = sb::buildExprStatement(&callexp);
    }
    
    void handle(SgArrayType& n)
    { 
      res = &createLoopOverArray(n);
    }
    
    SgExpression& elem;
  };
  
  struct VarDtorInserter
  {
      VarDtorInserter(SgBasicBlock& where, SgInitializedName& what)
      : blk(where), var(what)
      {}
      
      void execute() 
      {
        logWarn() << "in " << var.get_name() << std::endl;
        
        SgExpression& destructed = SG_DEREF( sb::buildVarRefExp(&var, nullptr) );
        SgStatement*  dtorcall   = sg::dispatch(DtorCallCreator(destructed), var.get_type());
        ROSE_ASSERT(dtorcall);
        
        //~ logInfo() << "destructing " << var.get_name()
                  //~ << " // " << SrcLoc(var)
                  //~ << std::endl;        
        blk.prepend_statement(dtorcall);
      }
      
    private:
      SgBasicBlock&      blk;
      SgInitializedName& var;
  };
  

  struct CtorInitListClearer
  {
      explicit
      CtorInitListClearer(SgCtorInitializerList& n)
      : ctorlst(n)
      {}
      
      void execute() 
      {
        // \todo delete elements before removing them from the list
        ctorlst.get_ctors().clear();
      }
      
    private:
      SgCtorInitializerList& ctorlst;
  };
  
  
  struct ConstructorGenerator
  {
      explicit
      ConstructorGenerator(SgMemberFunctionDeclaration& nondefiningCtor)
      : ctor(nondefiningCtor)
      {}
      
      void execute()
      {
        // is it already done?
        if (ctor.get_definingDeclaration()) return;

        // \todo cannot yet handle SgTemplateInstantiationMemberFunctionDecl
        if (isSgTemplateInstantiationMemberFunctionDecl(&ctor))
        {
          logWarn() << "Definition for SgTemplateInstantiationMemberFunctionDecl not generated: "
                    << ctor.get_name()
                    << std::endl;
          return;
        }
        
        SgClassDefinition&           clsdef  = sg::ancestor<SgClassDefinition>(ctor);
        SgMemberFunctionDeclaration& ctordef = mkCtorDtorDef(clsdef, ctor, true /* ctor */);

        clsdef.prepend_member(&ctordef);
        
        // initialize members
        normalizeCxx(&ctordef);
      }
    
    private:
      SgMemberFunctionDeclaration& ctor;
  };
  
  struct DestructorGenerator
  {
      explicit
      DestructorGenerator(SgClassDefinition& clsdef)
      : cls(clsdef)
      {}
      
      void execute()
      {
        logInfo() << "create dtor: " << nameCtorDtor(cls, false) 
                  << " // " << SrcLoc(cls)
                  << std::endl;
        
        SgMemberFunctionDeclaration& dtor = createCtorDtor(cls, false /* dtor */);
        
        // destruct member variables that have class type
        normalizeCxx(&dtor);
      }
    
    private:
      SgClassDefinition& cls;
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
        SgInitializedNamePtrList& lst = dcl->get_variables();
        
        ROSE_ASSERT(lst.size() == 1 && lst[0]);  
        res.push_back(lst[0]);
      }
    }
    
    return std::move(res);
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
                                                   
                       return std::move(res);
                     } 
                   );
  
  void clearMemoized()
  {
    logInfo() << getDirectNonVirtualBases.hits() 
              << " <hits -- size> " 
              << getDirectNonVirtualBases.size()
              << " getDirectNonVirtualBases - cache\n" 
              << getMemberVars.hits() 
              << " <hits -- size> " 
              << getMemberVars.size() 
              << " getMemberVars - cache\n" 
              << std::endl;
    
    getMemberVars.clear();
    getDirectNonVirtualBases.clear();
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
    for (SgInitializedName* var : adapt::reverse(getMemberVars(&cls)))
    {
      SgInitializer* ini = getMemberInitializer(*var, lst);
      
      cont.emplace_back(VarCtorInserter(blk, *var, ini));
    } 
    
    // explicitly construct all direct non-virtual bases;
    //   execute the transformations in reverse order
    for (SgBaseClass* base : adapt::reverse(getDirectNonVirtualBases(&cls)))
    {
      SgInitializer* ini = getBaseInitializer(*base, lst);
      
      cont.emplace_back(BaseCtorInserter(blk, *base, ini));
    } 
      
    // the initializer list is emptied.
    //   while it is not part of the ICFG, its nodes would be seen by
    //   the normalization.
    cont.emplace_back(CtorInitListClearer(lst));
  }
  
  void normalizeDtorDef(SgMemberFunctionDeclaration& fun, transformation_container& cont)
  {
    if (!fun.get_definition()) return; 
    
    SgBasicBlock&      blk = getCtorBody(fun);
    SgClassDefinition& cls = getClassDef(fun);
    
    // explicitly destruct all member variables of class type;
    //   execute the transformations in reverse order
    for (SgInitializedName* var : adapt::reverse(getMemberVars(&cls)))
    {
      SgType* varty = var->get_type();
      
      if (!TriviallyDestructible::check(varty)) 
      {
        //~ logInfo() << "nontrivial: " << var->get_name() << " " << varty->get_mangled() 
                  //~ << std::endl;
        
        cont.emplace_back(VarDtorInserter(blk, *var));
      }
    } 
    
    // explicitly destruct all direct non-virtual base classes;
    //   execute the transformations in reverse order
    for (SgBaseClass* base : adapt::reverse(getDirectNonVirtualBases(&cls)))
    {
      cont.emplace_back(BaseDtorInserter(blk, *base));
    }
  }
  
  struct CxxTransformer // : ExcludeTemplates
  {
      typedef transformation_container container;
      
      explicit
      CxxTransformer(container& transformations)
      : cont(transformations)
      {}
      
      void handle(SgNode&) {}
      
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
          return;
        }
          
        cont.emplace_back(ConstructorGenerator(*ctor));
      }
      
      void handle(SgMemberFunctionDeclaration& n)
      {
        if (n.get_specialFunctionModifier().isConstructor())
        {
          normalizeCtorDef(n, cont);
          return;
        }
        
        if (isDtor(n))
        {
          normalizeDtorDef(n, cont);
          return;
        }
      }
      
      void handle(SgClassDefinition& n)
      {
        typedef std::pair<bool, SgMemberFunctionDeclaration*> trivial_result_t;
        
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
          
          cont.emplace_back(DestructorGenerator(n));
          return;
        }
      }
    
    private:
      container& cont;
  };
  
  template <class SetT, class ElemT>
  inline
  bool alreadyProcessed(SetT& s, const ElemT& e)
  {
    return !s.insert(e).second;
  }
} // anonymous namespace

  // externally visible function
  void normalizeCxx(Normalization& norm, SgNode* root)
  {
    typedef std::set<SgNode*> NodeSet;
    //~ typedef std::unordered_set<SgNode*> NodeSet;
        
    logInfo() << "Starting C++ normalization." << std::endl;
        
    CxxTransformer::container transformations;
    NodeSet                   knownNodes;
    RoseAst                   ast(root);
    
    logTrace() << "Not normalizing templates.." << std::endl;
    //~ ast.setWithTemplates(false);
    for (auto i=ast.begin(); i!=ast.end(); ++i)
    { 
      // only traverse nodes one and if they are not templated 
      if (Normalization::isTemplateNode(*i) || alreadyProcessed(knownNodes, *i))
      {
        i.skipChildrenOnForward();
        continue;
      } 
      
      // never seen and not a template    
      sg::dispatch(CxxTransformer(transformations), *i);
    }
    
    logInfo() << "Found " << transformations.size() << " terrific top-level transformations..." << std::endl;
    
    for (AnyTransform& tf : transformations) 
      tf.execute();

    clearMemoized();
    logInfo() << "Finished C++ normalization." << std::endl; 
  }

  // for secondary transformations
  void normalizeCxx(SgNode* root)
  {
    CxxTransformer::container transformations;
    
    for (SgNode* n : RoseAst(root))
      sg::dispatch(CxxTransformer(transformations), n);
    
    for (AnyTransform& tf : transformations) 
      tf.execute();
  }

} // CodeThorn namespace
