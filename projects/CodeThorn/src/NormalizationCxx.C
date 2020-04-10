#include "sage3basic.h"
#include "sageBuilder.h"
#include "sageGeneric.h"
#include "sageInterface.h"
#include "RoseAst.h"

#include <memory>

#include "Normalization.h"
#include "NormalizationCxx.h"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace CodeThorn
{
  // internal use (does not exclude templates)
  void normalizeCxx(SgNode* root);
  
namespace 
{
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
  
  struct CxxTransformInitializer
  {
      explicit
      CxxTransformInitializer(transformation_container& transformations)
      : cont(transformations)
      {}
      
      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }
      
      void handle(SgAssignInitializer& n) {}
      void handle(SgConstructorInitializer& n) {}
    
    private:
      transformation_container& cont;
  };
  
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
  getInitializerFromCtorList(SgInitializedName& var, SgCtorInitializerList& ctorini)
  {
    SgInitializedNamePtrList&                lst = ctorini.get_ctors();
    SgInitializedNamePtrList::const_iterator pos = std::find_if(lst.begin(), lst.end(), SameName(var)); 
        
    return (pos != lst.end()) ? (*pos)->get_initializer() : nullptr;
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
  getInitializerFromCtorList(SgBaseClass& base, SgCtorInitializerList& ctorini)
  {
    SgInitializedNamePtrList&                lst = ctorini.get_ctors();
    SgInitializedNamePtrList::const_iterator pos = std::find_if(lst.begin(), lst.end(), SameClassDef(base)); 
        
    return (pos != lst.end()) ? (*pos)->get_initializer() : nullptr;
  }
  
  struct VarCtorInserter
  {
      VarCtorInserter(SgInitializedName& what, SgInitializer* how, SgBasicBlock& where)
      : var(&what), ini(how), blk(&where)
      {}
      
      SgInitializer* mkDefaultInitializer() 
      {
        // \todo once we build missing constructor bodies, this should become
        //       SgConstructorInitializer.
        return sb::buildAssignInitializer(sb::buildNullExpression(), var->get_type());
      } 
      
      void execute() 
      {
        ini = ini ? si::deepCopy(ini) : mkDefaultInitializer();
        
        SgVariableSymbol* varsym = SG_ASSERT_TYPE(SgVariableSymbol, var->search_for_symbol_from_symbol_table());         
        SgVarRefExp*      varref = sb::buildVarRefExp(varsym);
        SgAssignOp*       assign = sb::buildAssignOp(varref, ini);
        
        blk->prepend_statement(sb::buildExprStatement(assign));
      }
      
    private:
      SgInitializedName* var;
      SgInitializer*     ini;
      SgBasicBlock*      blk;
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
      FindFunction(const std::string& name, SgExprListExp& ctorargs)
      : args(ctorargs), funname(name)
      {}
  
      bool operator()(SgDeclarationStatement* dcl)
      { 
        SgMemberFunctionDeclaration* mem = isSgMemberFunctionDeclaration(dcl);
       
        if (!mem || (funname != std::string(mem->get_name()))) 
          return false;
      
        SgFunctionParameterList&     fplst = SG_DEREF(mem->get_parameterList());
        SgInitializedNamePtrList& 	 parms = fplst.get_args();
        SgExpressionPtrList& 	       exprs = args.get_expressions();
        
        if (exprs.size() > parms.size()) 
          return false;
        
        // \todo we need to ONLY check that mem is a ctor, cctor, or mctor (nothing else)
        for (size_t i = 0; i < exprs.size(); ++i)
        {
          if (!isConvertibleTo(SG_DEREF(exprs.at(i)), SG_DEREF(parms.at(i))))
            return false;
        }
        
        return parametersHaveDefaultValues(mem, parms, exprs.size());
      }
    
    private:
      SgExprListExp&    args;
      const std::string funname;
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

  SgMemberFunctionDeclaration&
  mkDctor(SgClassDefinition& scope)
  {
    SgName                       nm  = SG_DEREF(scope.get_declaration()).get_name();
    SgType&                      ty  = SG_DEREF(sb::buildVoidType());
    SgFunctionParameterList&     lst = SG_DEREF(sb::buildFunctionParameterList());
    SgMemberFunctionDeclaration& dcl = SG_DEREF(sb::buildNondefiningMemberFunctionDeclaration(nm, &ty, &lst, &scope));
    SgFunctionParameterScope&    psc = SG_DEREF(new SgFunctionParameterScope(dummyFileInfo()));

    markCompilerGenerated(lst);
    markCompilerGenerated(dcl);
    markCompilerGenerated(psc);

    dcl.get_specialFunctionModifier().setConstructor();
    dcl.set_functionParameterScope(&psc);
    psc.set_parent(&dcl);
    dcl.set_firstNondefiningDeclaration(&dcl);

    return dcl;
  }

  SgMemberFunctionDeclaration&
  mkDctorDef(SgClassDefinition& scope, SgMemberFunctionDeclaration& nondef)
  {
    ROSE_ASSERT(nondef.get_definingDeclaration() == nullptr);
    
    SgName                       nm  = nondef.get_name();
    SgType&                      ty  = SG_DEREF(nondef.get_orig_return_type());
    SgFunctionParameterList&     lst = SG_DEREF(sb::buildFunctionParameterList());
    SgMemberFunctionDeclaration& dcl = SG_DEREF(sb::buildDefiningMemberFunctionDeclaration(nm, &ty, &lst, &scope, nullptr, false, 0, &nondef, nullptr));
    SgFunctionParameterScope&    psc = SG_DEREF(new SgFunctionParameterScope(dummyFileInfo()));
    
    ROSE_ASSERT(nondef.get_definingDeclaration() != nullptr);
    ROSE_ASSERT(dcl.get_parent() != nullptr);
    ROSE_ASSERT(dcl.get_definition() != nullptr);
    ROSE_ASSERT(dcl.get_CtorInitializerList() != nullptr);
    ROSE_ASSERT(dcl.get_functionParameterScope() == nullptr);

    dcl.get_specialFunctionModifier().setConstructor();
    dcl.set_functionParameterScope(&psc);
    
    psc.set_parent(&dcl);
    
    return dcl;
  }
  
  SgMemberFunctionDeclaration& 
  createCtor(SgClassDeclaration& clsdcl, SgExprListExp& ctorargs)
  {
    SgClassDefinition&           clsdef  = getClassDef(clsdcl);
    SgMemberFunctionDeclaration& nondef  = mkDctor(clsdef);
    SgMemberFunctionDeclaration& ctordef = mkDctorDef(clsdef, nondef);

    clsdef.prepend_member(&ctordef);
    return ctordef;    
  }

  /// obtains a reference to a compiler generatable constructor in class \ref clazz that can take ctorargs.
  /// If none is defined, an empty constructor is added to the class
  SgMemberFunctionDeclaration& 
  obtainGeneratableFunction(SgClassDeclaration& clazz, const std::string& n, SgExprListExp& ctorargs)
  {
    typedef SgDeclarationStatementPtrList::iterator iterator;
    
    ROSE_ASSERT(ctorargs.get_expressions().size() == 0); // \todo handle copy and move ctors

    // SageInterface's getDefaultConstructor is not used b/c
    //   it finds the last constructor in scope, not necessarily the default ctor
    //~ SgMemberFunctionDeclaration*   dctor    = si::getDefaultConstructor(&clazz);
     
    ROSE_ASSERT(ctorargs.get_expressions().size() == 0); // \todo handle copy and move ctors

    SgClassDefinition&             classdef = getClassDef(clazz);
    SgDeclarationStatementPtrList& members  = classdef.get_members();
    
    // \todo Currently, this code only considers in-class ctor declarations.
    //       To be correct, we would need to also find all out-of-class ctor declarations
    //       and check against their default arguments.
    iterator                       pos      = std::find_if( members.begin(), members.end(), 
                                                            FindFunction(n, ctorargs)
                                                          );
    
    return pos != members.end() ? SG_DEREF( isSgMemberFunctionDeclaration(*pos) )
                                : createCtor(clazz, ctorargs)
                                ; 
  }
  
  SgMemberFunctionDeclaration& 
  obtainGeneratableCtor(SgClassDeclaration& clazz, SgExprListExp& ctorargs)
  {
    return obtainGeneratableFunction(clazz, clazz.get_name(), ctorargs);
  }

  SgMemberFunctionDeclaration& 
  obtainGeneratableDtor(SgClassDeclaration& clazz, SgExprListExp& ctorargs)
  {
    std::string dtorname = std::string("~") + clazz.get_name();
    
    return obtainGeneratableFunction(clazz, dtorname, ctorargs);
  }

  struct BaseCtorInserter
  {
      BaseCtorInserter(SgBaseClass& what, SgInitializer* how, SgBasicBlock& where)
      : base(&what), ini(how), blk(&where)
      {}
      
      SgInitializer* mkDefaultInitializer() const 
      {
        SgExprListExp&               args  = SG_DEREF( sb::buildExprListExp() );
        SgClassDeclaration&          clazz = SG_DEREF( base->get_base_class() );
        SgMemberFunctionDeclaration& ctor  = obtainGeneratableCtor(clazz, args);      
  
        return sb::buildConstructorInitializer( &ctor /* currently not available */,
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
        
        blk->prepend_statement(sb::buildExprStatement(ini));
      }
    
    private:  
      SgBaseClass*   base;
      SgInitializer* ini;
      SgBasicBlock*  blk;
  };
  
  SgFunctionSymbol& 
  get_symbol(SgFunctionDeclaration& fundecl)
  {
    SgSymbol& symbl = SG_DEREF( fundecl.search_for_symbol_from_symbol_table() );
    
    return SG_ASSERT_TYPE(SgFunctionSymbol, symbl);
  }
  
  struct BaseDtorInserter
  {
      BaseDtorInserter(SgBaseClass& what, SgBasicBlock& where)
      : base(&what), blk(&where)
      {}
      
      SgStatement* mkDtorCall() const
      {
        SgExprListExp&               args  = SG_DEREF( sb::buildExprListExp() );
        SgClassDeclaration&          clazz = SG_DEREF( base->get_base_class() );
        SgMemberFunctionDeclaration& dtor  = obtainGeneratableDtor(clazz, args);
        SgFunctionSymbol&            symbl = get_symbol(dtor);
        SgFunctionCallExp&           call  = SG_DEREF( sb::buildFunctionCallExp(&symbl, &args) );
  
        return sb::buildExprStatement(&call);
      } 
      
      void execute() { blk->append_statement(mkDtorCall()); }
    
    private:  
      SgBaseClass*   base;
      SgBasicBlock*  blk;
  };
  
  
  struct CtorInitListClearer
  {
      explicit
      CtorInitListClearer(SgCtorInitializerList& n)
      : ctorlst(&n)
      {}
      
      void execute() 
      {
        // \todo delete elements before removing them from the list
        ctorlst->get_ctors().clear();
      }
      
    private:
      SgCtorInitializerList* ctorlst;
  };
  
  
  struct ConstructorGenerator
  {
      ConstructorGenerator(SgMemberFunctionDeclaration& nondefiningCtor)
      : ctor(&nondefiningCtor)
      {}
      
      void execute()
      {
        // is it already done?
        if (ctor->get_definingDeclaration()) return;

        // \todo cannot yet handle SgTemplateInstantiationMemberFunctionDecl
        if (isSgTemplateInstantiationMemberFunctionDecl(ctor))
        {
          logWarn() << "Definition for SgTemplateInstantiationMemberFunctionDecl not generated: "
                    << ctor->get_name()
                    << std::endl;
          return;
        }
        
        SgClassDefinition&           clsdef  = sg::ancestor<SgClassDefinition>(*ctor);
        SgMemberFunctionDeclaration& ctordef = mkDctorDef(clsdef, *ctor);

        clsdef.prepend_member(&ctordef);
        
        // initialize members
        normalizeCxx(&ctordef);
      }
    
    private:
      SgMemberFunctionDeclaration* ctor;
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
    return (  n.get_definingDeclaration() != nullptr
           || isGenerateableCtor(n)
           );
  }

  
  /// returns the canonical node of a construction (builder function + components).
  /// \details
  ///   for each unique combination of builder function and components the
  ///   canonical node is returned.
  ///   On the first invocation, the canonical node is built (using builder(args...))
  ///   and memoized. On later invocations, the memoized node is returned.
  /// \tparam BuilderFn the type of the builder function
  /// \tparam Args      an argument pack consisting of less-than comparable components
  /// \param  func      the function (MUST be a functor, not a function)
  /// \param  args      the arguments to func
  /// \return the result obtained by func(args...)
  template <class Fn, class... Args>
  auto cacheFunction(Fn func, Args... args) -> decltype(func(args...))&
  {
    typedef decltype(func(args...))                            return_t;
    typedef std::map<std::tuple<Args...>, return_t>            result_cache_t;
    typedef std::pair<typename result_cache_t::iterator, bool> emplace_result_t;
  
    static result_cache_t cache;
    
    std::tuple<Args...>               desc(args...);
    typename result_cache_t::iterator pos = cache.find(desc);

    if (pos != cache.end())
      return pos->second;
    
    emplace_result_t                  res = cache.emplace(desc, func(args...));
      
    ROSE_ASSERT(res.second);
    return res.first->second;
  }

  
  const std::vector<SgInitializedName*>&
  getMemberVars(SgClassDefinition& cls)
  {
    struct Extractor
    {
      const std::vector<SgInitializedName*>
      operator()(const SgClassDefinition* cls)
      {
        std::vector<SgInitializedName*> res;
        
        for (SgDeclarationStatement* cand : cls->get_members())
        {
          SgVariableDeclaration* dcl = isSgVariableDeclaration(cand);
    
          if (dcl && !si::isStatic(dcl))      
            res.push_back( si::getFirstInitializedName(dcl) );
        }
        
        return std::move(res);
      }
    };

    return cacheFunction(Extractor(), &cls);
  }
  
  const std::vector<SgBaseClass*>&
  getDirectNonVirtualBases(SgClassDefinition& cls)
  {
    struct Extractor
    {
      const std::vector<SgBaseClass*>
      operator()(const SgClassDefinition* cls)
      {
        std::vector<SgBaseClass*> res;
        
        for (SgBaseClass* cand : cls->get_inheritances())
        {
          ROSE_ASSERT(cand);
                         
          if (cand->get_isDirectBaseClass() && !isVirtualBase(*cand))
            res.push_back(cand);
        }
                                     
        return std::move(res);
      }
    };
    
    return cacheFunction(Extractor(), &cls);
  }
  
  void normalizeCtorDef(SgMemberFunctionDeclaration& fun, transformation_container& cont)
  {
    typedef const std::vector<SgInitializedName*> member_container_t;
    typedef const std::vector<SgBaseClass*>       base_container_t;
    
    if (!fun.get_definition()) return;
    
    SgBasicBlock&          blk = getCtorBody(fun);
    SgClassDefinition&     cls = getClassDef(fun);
    SgCtorInitializerList& lst = SG_DEREF( fun.get_CtorInitializerList() );
    
    {
      member_container_t&  members = getMemberVars(cls);
      
      // explicitly initialize all member variables;
      //   execute the transformations in reverse order
      std::transform( members.rbegin(), members.rend(),
                      std::back_inserter(cont),
                      [&](SgInitializedName* var) -> AnyTransform
                      {
                        ROSE_ASSERT(var);
                        
                        SgInitializer* ini = getInitializerFromCtorList(*var, lst);
        
                        if (!ini) ini = var->get_initializer();
                        
                        return VarCtorInserter(*var, ini, blk);
                      } 
                    );
    }
    
    {
      base_container_t&    bases = getDirectNonVirtualBases(cls);
      
      // explicitly construct all direct non-virtual base classes;
      //   execute the transformations in reverse order
      std::transform( bases.rbegin(), bases.rend(),
                      std::back_inserter(cont),
                      [&](SgBaseClass* base) -> AnyTransform
                      {
                        ROSE_ASSERT(base);
                        
                        SgInitializer* ini = getInitializerFromCtorList(*base, lst);
                        
                        return BaseCtorInserter(*base, ini, blk);
                      } 
                    );
    }
      
    // the initializer list is emptied.
    //   while it is not part of the ICFG, its nodes would be seen by
    //   the normalization.
    cont.emplace_back(CtorInitListClearer(lst));
  }
  
  void normalizeDtorDef(SgMemberFunctionDeclaration& fun, transformation_container& cont)
  {
    //~ if (!fun.get_definition()) return;
    
    //~ SgBasicBlock&          blk = getCtorBody(fun);
    //~ SgClassDefinition&     cls = getClassDef(fun);
    
    // explicitly deconstruct all member variables;
    //   execute the transformations in reverse order
    //~ for (int i = cls.get_members().size(); i > 0; --i)
    //~ {
      //~ SgVariableDeclaration* dcl = isSgVariableDeclaration(cls.get_members().at(i-1));
  
      //~ if (!dcl || si::isStatic(dcl)) continue;
      
      //~ SgInitializedName& var = SG_DEREF( si::getFirstInitializedName(dcl) );
      
      //~ cont.emplace_back(VarDtorInserter(var, blk));
    //~ }
    
    // explicitly call all direct base class dtors (excl. virtual base classes)
    //   execute the transformations in reverse order
    //~ for (int i = cls.get_inheritances().size(); i > 0; --i)
    //~ {
      //~ SgBaseClass& base = SG_DEREF(cls.get_inheritances().at(i-1));
      
      //~ if (!base.get_isDirectBaseClass() || isVirtualBase(base)) continue;
      
      //~ cont.emplace_back(BaseDtorInserter(base, blk));
    //~ }
  }
  
  struct CxxTransformer
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
          return;
          
        cont.emplace_back(ConstructorGenerator(*ctor));
      }
      
      void handle(SgMemberFunctionDeclaration& n)
      {
        if (n.get_specialFunctionModifier().isConstructor())
        {
          normalizeCtorDef(n, cont);
          return;
        }
        
        if (n.get_specialFunctionModifier().isDestructor())
        {
          normalizeDtorDef(n, cont);
          return;
        }
      }
    
    private:
      container& cont;
  };
} // anonymous namespace

  void normalizeCxx(Normalization& norm, SgNode* root)
  {
    logInfo() << "Starting C++ normalization." << std::endl;
        
    CxxTransformer::container transformations;
    size_t                    templateCount = 0;
    RoseAst                   ast(root);
    
    for (auto i=ast.begin(); i!=ast.end(); ++i)
    {
      if (norm.isTemplateNode(*i)) 
      {
        i.skipChildrenOnForward();
        ++templateCount;
        continue;
      }
      
      sg::dispatch(CxxTransformer(transformations), *i);
    }
    
    if (templateCount) logWarn() << "Skipped " << templateCount << " templates " << std::endl; 
    logInfo() << "Found " << transformations.size() << " terrific top-level transformations..." << std::endl;
    
    for (AnyTransform& tf : transformations) 
      tf.execute();

    logInfo() << "Finished C++ normalization." << std::endl; 
  }

  void normalizeCxx(SgNode* root)
  {
    CxxTransformer::container transformations;
    
    for (SgNode* n : RoseAst(root))
      sg::dispatch(CxxTransformer(transformations), n);
    
    for (AnyTransform& tf : transformations) 
      tf.execute();
  }

} // CodeThorn namespace
