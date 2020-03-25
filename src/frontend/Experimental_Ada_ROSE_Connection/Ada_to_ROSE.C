#include <numeric>
#include <type_traits>
#include <sstream>

#include "sage3basic.h"
#include "rose_config.h"
#include "sageGeneric.h"
#include "sageBuilder.h"
#include "Ada_to_ROSE_translation.h"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Ada_ROSE_Translation
{

namespace // local declarations
{
  //
  // loggers
  //   \todo replace streams with Sawyer
  std::ostream&
  logInfo()
  {
    return std::cerr << "[INFO] ";
  }

  std::ostream&
  logWarn()
  {
    return std::cerr << "[WARN] ";
  }

  //
  // utilities
  template <class U, class V>
  U conv(const V& val)
  {
    U                 res;
    std::stringstream buf;

    buf << val;
    buf >> res;

    return res;
  }

  //
  // declaration store and retrieval

  std::map<int, SgInitializedName*>      asisVars;  ///< stores a mapping from element id
                                                    ///  to constructed ROSE nodes.

  std::map<int, SgInitializedName*>      asisExcps; ///< stores a mapping from element id
                                                    ///  to an ADA exception decl. node.

  std::map<int, SgDeclarationStatement*> asisDecls; ///< stores a mapping from element id
                                                    ///  to constructed ROSE nodes.
                                                    ///  functions.. (anything else)

  std::map<int, SgDeclarationStatement*> asisTypes; ///< stores a mapping from element id
                                                    ///  to constructed ROSE nodes.

  std::map<int, SgStatement*>            asisLoops; ///< stores a mapping from element id
                                                    ///  to constructed loops (needed for exit statements).

  std::map<std::string, SgType*>         adaTypes;  ///< stores a mapping from a name to an ADA standard type
                                                    ///  e.g., Integer.

  template <class KeyT, class DclT, class ValT>
  void
  recordDcl(std::map<KeyT, DclT*>& m, KeyT key, ValT& val)
  {
    ROSE_ASSERT(m.find(key) == m.end());

    //~ logInfo() << ">el# " << key << std::endl;
    m[key] = &val;
  }
  
  template <class KeyT, class DclT>
  DclT&
  retrieveDcl(const std::map<KeyT, DclT*>& m, KeyT key)
  {
    typename std::map<KeyT, DclT*>::const_iterator pos = m.find(key);

    ROSE_ASSERT(pos != m.end());
    return *(pos->second);
  }
  
  template <class KeyT, class DclT, class Alt>
  DclT&
  retrieveDclIfAvail(const std::map<KeyT, DclT*>& m, KeyT key, Alt alt)
  {
    if (key == -1) return alt(); 
    
    return retrieveDcl(m, key);
  }


  template <class KeyT, class DclT>
  DclT*
  findDcl(const std::map<KeyT, DclT*>& m, KeyT key)
  {
    typename std::map<KeyT, DclT*>::const_iterator pos = m.find(key);

    if (pos == m.end())
      return nullptr;

    return pos->second;
  }

  /// base case when a declaration is not in the map
  template <class KeyT, class DclT>
  DclT*
  findFirst(const std::map<KeyT, DclT*>&)
  {
    return nullptr;
  }

  /// tries a number of keys to find a declaration from map @ref m
  template <class KeyT, class DclT, class Key0T, class... KeysT>
  DclT*
  findFirst(const std::map<KeyT, DclT*>& m, Key0T key0, KeysT... keys)
  {
    DclT* dcl = findDcl(m, key0);

    return dcl ? dcl : findFirst(m, keys...);
  }


  //
  // retrieve from ASIS map

  template <class ElemT>
  ElemT*
  retrieveAsOpt(ASIS_element_id_to_ASIS_MapType& map, int key)
  {
    //~ logInfo() << "key: " << key << std::endl;
    ASIS_element_id_to_ASIS_MapType::iterator pos = map.find(key);

    if (pos != map.end())
      return reinterpret_cast<ElemT*>((*pos).second);

    return NULL;
  }

  template <class ElemT>
  ElemT&
  retrieveAs(ASIS_element_id_to_ASIS_MapType& map, int key)
  {
    return SG_DEREF(retrieveAsOpt<ElemT>(map, key));
  }


  //
  // some ADA AST node maker functions
  // - currently everything is mapped back to the existing ROSE nodes
  //   this needs to be reconfigured once we decide which AST nodes to introduce

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

  /// sets the symbols defining decl
  void link_decls(SgFunctionSymbol& funcsy, SgFunctionDeclaration& func)
  {
    SgFunctionDeclaration& sdcl = sg::deref(funcsy.get_declaration());

    sdcl.set_definingDeclaration(&func);
    func.set_firstNondefiningDeclaration(&sdcl);

    // \todo \pp is sdcl == func allowed in ROSE/C++?
  }

  SgBasicBlock&
  mkBasicBlock()
  {
    SgBasicBlock& bdy = sg::deref(sb::buildBasicBlock());

    markCompilerGenerated(bdy);
    return bdy;
  }

  SgWhileStmt&
  mkWhileStmt(SgExpression& cond, SgBasicBlock& body)
  {
    SgWhileStmt& stmt = SG_DEREF( sb::buildWhileStmt(&cond, &body) );

    markCompilerGenerated(stmt);
    return stmt;
  }

  SgForStatement&
  mkLoopStmt(SgBasicBlock& body)
  {
    SgStatement&    init = SG_DEREF( sb::buildNullStatement() );
    SgStatement&    test = SG_DEREF( sb::buildNullStatement() );
    SgExpression&   incr = SG_DEREF( sb::buildNullExpression() );
    SgForStatement& stmt = SG_DEREF( sb::buildForStatement(&init, &test, &incr, &body) );

    markCompilerGenerated(stmt);
    return stmt;
  }

  SgNamespaceDeclarationStatement&
  mkPackage(std::string name, SgScopeStatement& scope)
  {
    SgNamespaceDeclarationStatement& nsdcl = SG_DEREF(sb::buildNamespaceDeclaration(name, &scope));
    SgNamespaceDefinitionStatement&  nsdef = SG_DEREF(sb::buildNamespaceDefinition(&nsdcl));

    nsdcl.set_parent(&scope);
    nsdef.set_parent(&nsdcl);
    nsdcl.set_definition(&nsdef);

    // set if first definition
    if (nsdef.get_global_definition() == NULL)
    {
      //~ SgNamespaceDeclarationStatement& globdcl = SG_DEREF(sb::buildNamespaceDeclaration(name, &scope));
      SgNamespaceDefinitionStatement&  globdef = SG_DEREF(sb::buildNamespaceDefinition(&nsdcl));

      //~ globdcl.set_parent(&scope);
      globdef.set_parent(&scope);
      globdef.set_global_definition(&globdef);
      nsdef.set_global_definition(&globdef);
    }

    return nsdcl;
  }

  SgFunctionDeclaration&
  mkProcedure(std::string nm, SgScopeStatement& scope)
  {
    SgType&                   ty  = SG_DEREF(sb::buildVoidType());
    SgFunctionParameterList&  lst = SG_DEREF(sb::buildFunctionParameterList());
    SgFunctionDeclaration&    dcl = SG_DEREF(sb::buildNondefiningFunctionDeclaration(nm, &ty, &lst, &scope, NULL));
    SgFunctionParameterScope& psc = SG_DEREF(new SgFunctionParameterScope(dummyFileInfo()));

    markCompilerGenerated(lst);
    markCompilerGenerated(dcl);
    markCompilerGenerated(psc);
    dcl.set_functionParameterScope(&psc);
    psc.set_parent(&dcl);

    return dcl;
  }

  SgFunctionDefinition&
  mkProcedureDefNode(SgFunctionDeclaration& dcl)
  {
    SgFunctionDefinition& def = *new SgFunctionDefinition(&dcl, NULL);

    dcl.set_definition(&def);
    def.set_parent(&dcl);
    markCompilerGenerated(def);
    return def;
  }

  std::pair<SgFunctionDeclaration*, SgBasicBlock*>
  mkProcedureDef(std::string nm, SgScopeStatement& scope)
  {
    SgFunctionDeclaration& proc   = mkProcedure(nm, scope);
    SgSymbol*              baseSy = proc.search_for_symbol_from_symbol_table();
    SgFunctionSymbol&      funcSy = *SG_ASSERT_TYPE(SgFunctionSymbol, baseSy);

    link_decls(funcSy, proc);
    proc.set_definingDeclaration(&proc);
    proc.unsetForward();

    SgFunctionDefinition&  pdef   = mkProcedureDefNode(proc);
    SgBasicBlock&          body   = mkBasicBlock();

    pdef.set_body(&body);
    body.set_parent(&pdef);

    return std::make_pair(&proc, &body);
  }

  SgStatement&
  mkNullStmt()
  {
    SgStatement& sgnode = SG_DEREF(sb::buildNullStatement());

    markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgInitializedName&
  mkInitializedName(const std::string& varname, SgType& vartype, SgExpression* varexpr)
  {
    SgAssignInitializer* varinit = varexpr ? sb::buildAssignInitializer(varexpr) : NULL;
    SgInitializedName&   sgnode = SG_DEREF( sb::buildInitializedName(varname, &vartype, varinit) );

    if (varinit)
      markCompilerGenerated(*varinit);

    markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgVariableDeclaration&
  mkParameter( const std::vector<SgInitializedName*>& parms,
               SgTypeModifier parmmode,
               SgScopeStatement& scope
             )
  {
    SgVariableDeclaration&    parmDecl = SG_DEREF( new SgVariableDeclaration(dummyFileInfo()) );
    SgInitializedNamePtrList& names    = parmDecl.get_variables();
    SgDeclarationModifier&    declMods = parmDecl.get_declarationModifier();

    names.insert(names.end(), parms.begin(), parms.end());
    declMods.get_typeModifier() = parmmode;

    markCompilerGenerated(parmDecl);
    si::fixVariableDeclaration(&parmDecl, &scope);
    parmDecl.set_parent(&scope);
    parmDecl.set_definingDeclaration(NULL);

    return parmDecl;
  }


  SgVariableDeclaration&
  mkVarDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope)
  {
    SgVariableDeclaration&    vardcl = SG_DEREF( new SgVariableDeclaration(dummyFileInfo()) );
    SgInitializedNamePtrList& names  = vardcl.get_variables();

    names.insert(names.end(), vars.begin(), vars.end());

    markCompilerGenerated(vardcl);
    si::fixVariableDeclaration(&vardcl, &scope);
    vardcl.set_parent(&scope);
    return vardcl;
  }

  SgVariableDeclaration&
  mkExceptionDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope)
  {
    // \todo
    // currently, exceptions are variables with type "Exception"
    //   revise after ROSE representation of ADA exceptions is revised
    SgVariableDeclaration&    vardcl = SG_DEREF( new SgVariableDeclaration(dummyFileInfo()) );
    SgInitializedNamePtrList& names  = vardcl.get_variables();

    names.insert(names.end(), vars.begin(), vars.end());

    markCompilerGenerated(vardcl);
    si::fixVariableDeclaration(&vardcl, &scope);
    vardcl.set_parent(&scope);
    return vardcl;
  }


  SgExpression&
  mkExceptionRef(SgInitializedName& exception, SgScopeStatement& scope)
  {
    // \todo
    // replace and refer to referenced declaration
    SgExpression& sgnode = SG_DEREF( sb::buildVarRefExp(&exception, &scope) );

    markCompilerGenerated(sgnode);
    return sgnode;
  }


  SgStatement&
  mkRaiseStmt(SgExpression& raised)
  {
    SgExpression&    raiseop = SG_DEREF( sb::buildThrowOp(&raised, SgThrowOp::throw_expression ) );
    SgExprStatement& sgnode  = SG_DEREF( sb::buildExprStatement(&raiseop) );

    markCompilerGenerated(raiseop);
    markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgSwitchStatement&
  mkSwitchStmt(SgExpression& ctrl, SgBasicBlock& blk)
  {
    // \todo
    // maybe it would be good to have a separate case statement for ADA
    //   as ADA is a bit more restrictive in its switch case syntax compared to C++.
    SgSwitchStatement& sgnode = SG_DEREF( sb::buildSwitchStatement(&ctrl, &blk) );

    markCompilerGenerated(sgnode);
    return sgnode;
  }


  SgTypedefDeclaration&
  mkSubtypeType(const std::string& name, SgType& ty, SgScopeStatement& scope)
  {
    SgTypedefDeclaration& sgnode = SG_DEREF( sb::buildTypedefDeclaration(name, &ty, &scope) );

    markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgTypedefType&
  mkTyperef(SgDeclarationStatement& dcl)
  {
    SgTypedefType& sgnode = SG_DEREF( new SgTypedefType(&dcl, dcl.search_for_symbol_from_symbol_table()) );

    return sgnode;
  }

  SgImportStatement&
  mkWithClause(const std::vector<SgExpression*>& imported)
  {
    SgImportStatement&   sgnode = SG_DEREF( new SgImportStatement(dummyFileInfo()) );
    SgExpressionPtrList& lst    = sgnode.get_import_list();

    lst.insert(lst.end(), imported.begin(), imported.end());

    markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgExpression&
  mkSelectedComponent(SgExpression& prefix, SgExpression& selector)
  {
    SgExpression& sgnode = SG_DEREF( sb::buildDotExp(&prefix, &selector) );

    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgAdaExitStmt&
  mkAdaExitStmt(SgStatement& loop, SgExpression& condition, bool explicitLoopName = true)
  {
    SgAdaExitStmt& sgnode = SG_DEREF(new SgAdaExitStmt(&loop, &condition, explicitLoopName));

    markCompilerGenerated(sgnode);
    return sgnode;
  }


  //
  // auxiliary structs and functions
  struct AstContext
  {
    AstContext(SgScopeStatement& s)
    : scope_(&s)
    {}

    SgScopeStatement& scope() const { return *scope_; }

    AstContext scope(SgScopeStatement& s) const
    {
      AstContext tmp(*this);

      tmp.scope_ = &s;
      return tmp;
    }

    SgScopeStatement* scope_;
  };

  template <class T>
  struct Range : std::pair<T, T>
  {
    Range(T lhs, T rhs)
    : std::pair<T, T>(lhs, rhs)
    {}

    bool empty() const { return this->first == this->second; }
    int  size()  const { return this->second - this->first; }
  };

  struct UnitIdRange : Range<Unit_ID_Ptr>
  {
    typedef Unit_Struct value_type;

    UnitIdRange(Unit_ID_Ptr lhs, Unit_ID_Ptr rhs)
    : Range<Unit_ID_Ptr>(lhs, rhs)
    {}
  };

  struct ElemIdRange : Range<Element_ID_Ptr>
  {
    typedef Element_Struct value_type;

    ElemIdRange(Element_ID_Ptr lhs, Element_ID_Ptr rhs)
    : Range<Element_ID_Ptr>(lhs, rhs)
    {}
  };


  template <class T>
  struct range_types {};

  template <>
  struct range_types<Element_ID_List>
  {
    typedef ElemIdRange type;
  };

  template <>
  struct range_types<Unit_ID_Array_Struct>
  {
    typedef UnitIdRange type;
  };

  template <class P, class F>
  void traverse(P* first, P* limit, F functor)
  {
    while (first != limit)
    {
      functor(*first);

      first = first->Next;
    }
  }


  template <class ElemT, class Ptr, class F>
  F
  traverseIDs(Ptr first, Ptr limit, ASIS_element_id_to_ASIS_MapType& map, F func)
  {
    while (first != limit)
    {
      func(retrieveAs<ElemT>(map, *first));

      ++first;
    }

    return func;
  }

  template <class Range, class F>
  F
  traverseIDs(Range range, ASIS_element_id_to_ASIS_MapType& map, F functor)
  {
    return traverseIDs<typename Range::value_type>(range.first, range.second, map, functor);
  }

  template <class Lst>
  typename range_types<Lst>::type
  idRange(Lst lst)
  {
    typedef typename range_types<Lst>::type RangeType;

    return RangeType(lst.IDs, lst.IDs + lst.Length);
  }

  void handleElement(Element_Struct& elem, AstContext ctx);
  void handleStmt(Element_Struct& elem, AstContext ctx);

  struct ElemCreator
  {
      explicit
      ElemCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        handleElement(elem, ctx);
      }

      AstContext ctx;
  };

  struct StmtCreator
  {
      explicit
      StmtCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        handleStmt(elem, ctx);
      }

      AstContext ctx;
  };

  SgExpression&
  getArg(Element_Struct& elem, AstContext ctx);

  struct ArglistCreator
  {
      explicit
      ArglistCreator(AstContext astctx)
      : ctx(astctx), args()
      {}

      void operator()(Element_Struct& elem)
      {
        args.push_back(&getArg(elem, ctx));
      }

      operator SgExprListExp& ()
      {
        return SG_DEREF(sb::buildExprListExp(args));
      }

      AstContext                 ctx;
      std::vector<SgExpression*> args;
  };

  //
  // pre declarations

  SgVariableDeclaration&
  getParm(Element_Struct& elem, AstContext ctx);

  SgExpression&
  getExprID(Element_ID el, AstContext ctx);

  SgExpression&
  getExprID_opt(Element_ID el, AstContext ctx);

  SgExpression&
  getExpr(Element_Struct& elem, AstContext ctx);

  std::string
  getName(Element_Struct& elem, AstContext ctx);

  //
  // Creator classes

  struct ParmlistCreator
  {
      ParmlistCreator(SgFunctionDeclaration& func, AstContext astctx)
      : ctx(astctx), args( SG_DEREF(func.get_parameterList()).get_args() )
      {}

      void operator()(Element_Struct& elem)
      {
        SgVariableDeclaration&    decl  = getParm(elem, ctx);
        SgInitializedNamePtrList& parms = decl.get_variables();

        args.insert(args.end(), parms.begin(), parms.end());
      }

      AstContext                ctx;
      SgInitializedNamePtrList& args;
  };

  struct NameCreator
  {
      typedef std::pair<std::string, Element_ID> result_type;
      typedef std::vector<result_type>           result_container;

      explicit
      NameCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        names.push_back(result_type(getName(elem, ctx), elem.ID));
      }

      operator result_container() { return names; }

      AstContext       ctx;
      result_container names;
  };

  struct WithClauseElemCreator
  {
      explicit
      WithClauseElemCreator(AstContext astctx)
      : ctx(astctx), elems()
      {}

      void operator()(Element_Struct& el)
      {
        ROSE_ASSERT(el.Element_Kind == An_Expression);

        elems.push_back(&getExpr(el, ctx));
      }

      operator std::vector<SgExpression*> () &&
      {
        return std::move(elems);
      }

      AstContext                 ctx;
      std::vector<SgExpression*> elems;
  };

  struct IfStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      explicit
      IfStmtCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void commonBranch(Path_Struct& path, SgExpression* cond = NULL)
      {
        SgBasicBlock& block     = mkBasicBlock();
        ElemIdRange   thenStmts = idRange(path.Sequence_Of_Statements);

        traverseIDs(thenStmts, asisMap, StmtCreator(ctx.scope(block)));
        branches.push_back(branch_type(cond, &block));
      }

      void conditionedBranch(Path_Struct& path)
      {
        commonBranch(path, &getExprID(path.Condition_Expression, ctx));
      }

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case An_If_Path:
            {
              ROSE_ASSERT(branches.size() == 0);
              conditionedBranch(path);
              break;
            }

          case An_Elsif_Path:
            {
              ROSE_ASSERT(branches.size() != 0);
              conditionedBranch(path);
              break;
            }

          case An_Else_Path:
            {
              ROSE_ASSERT(branches.size() != 0);
              commonBranch(path);
              break;
            }

          default:
            ROSE_ASSERT(false);
        }
      }

      static
      SgStatement*
      createIfStmt(SgStatement* elsePath, branch_type thenPath)
      {
        ROSE_ASSERT(thenPath.first && thenPath.second);

        return sb::buildIfStmt(thenPath.first, thenPath.second, elsePath);
      }

      operator SgStatement&()
      {
        SgStatement* elseStmt = NULL;

        if (!branches.back().first)
        {
          elseStmt = branches.back().second;
          branches.pop_back();
        }

        return SG_DEREF( std::accumulate(branches.rbegin(), branches.rend(), elseStmt, createIfStmt) );
      }

      AstContext       ctx;
      branch_container branches;
  };


  //
  // some(?) functions

  SgType&
  getExprType(Expression_Struct& typeEx, AstContext ctx)
  {
    ROSE_ASSERT(typeEx.Expression_Kind == An_Identifier);

    if (SgDeclarationStatement* dcl = findFirst(asisTypes, typeEx.Corresponding_Name_Definition))
      return mkTyperef(*dcl);
      
    SgType* ty = findFirst(adaTypes, std::string(typeEx.Name_Image));
    
    if (!ty)
    {
      logWarn() << "unknown type: " << typeEx.Name_Image << std::endl;
      
      ROSE_ASSERT(false);
    }

    return SG_DEREF(ty);
  }

  SgType&
  getAccessType(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == An_Access_Definition);

    Access_Definition_Struct& access = def.The_Union.The_Access_Definition;

    switch (access.Access_Definition_Kind)
    {
      case An_Anonymous_Access_To_Variable:            // [...] access subtype_mark
      case An_Anonymous_Access_To_Constant:            // [...] access constant subtype_mark
      case An_Anonymous_Access_To_Procedure:           // access procedure
      case An_Anonymous_Access_To_Protected_Procedure: // access protected procedure
      case An_Anonymous_Access_To_Function:            // access function
      case An_Anonymous_Access_To_Protected_Function:  // access protected function
        {
          logInfo() << "ak: " << access.Access_Definition_Kind << std::endl;
          break;
        }

      case Not_An_Access_Definition:                   // An unexpected element
      default:
        logWarn() << "ak? " << access.Access_Definition_Kind << std::endl;
        ROSE_ASSERT(false);
    }

    ROSE_ASSERT(false);
    return SG_DEREF(sb::buildVoidType());
  }

  SgType&
  getDeclType(Element_Struct& elem, AstContext ctx)
  {
    if (elem.Element_Kind == An_Expression)
      return getExprType(elem.The_Union.Expression, ctx);

    ROSE_ASSERT(elem.Element_Kind == A_Definition);
    Definition_Struct& def = elem.The_Union.Definition;

    if (def.Definition_Kind == An_Access_Definition)
      return getAccessType(def, ctx);

    ROSE_ASSERT(false);
    return SG_DEREF(sb::buildVoidType());
  }

  SgType&
  getDeclTypeID(Element_ID id, AstContext ctx)
  {
    return getDeclType(retrieveAs<Element_Struct>(asisMap, id), ctx);
  }

  SgType&
  getVarType(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               );

    Element_Struct& elem = retrieveAs<Element_Struct>(asisMap, decl.Object_Declaration_View);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&         def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Subtype_Indication);

    Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;

    return getDeclTypeID(subtype.Subtype_Mark, ctx);
  }

  SgType&
  getTypedeclType(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT( decl.Declaration_Kind == An_Ordinary_Type_Declaration );

    SgType*                 res  = NULL;
    Element_Struct&         elem = retrieveAs<Element_Struct>(asisMap, decl.Type_Declaration_View);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&      def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Type_Definition);

    /* unused fields:
       Definition_Struct
         bool                           Has_Null_Exclusion;
    */
    Type_Definition_Struct& typenode = def.The_Union.The_Type_Definition;

    switch (typenode.Type_Kind)
    {
      case A_Derived_Type_Definition:              // 3.4(2)     -> Trait_Kinds
        {
          Element_Struct&            derivedTypeElem = retrieveAs<Element_Struct>(asisMap, typenode.Parent_Subtype_Indication);
          ROSE_ASSERT(derivedTypeElem.Element_Kind == A_Definition);

          Definition_Struct&         derivedTypeDef  = derivedTypeElem.The_Union.Definition;
          ROSE_ASSERT(derivedTypeDef.Definition_Kind == A_Subtype_Indication);
          /*
             unused fields: (derivedTypeDef)
                Declaration_List     Implicit_Inherited_Declarations;
          */

          Subtype_Indication_Struct& derivedSubtype = derivedTypeDef.The_Union.The_Subtype_Indication;
          /*
             unused fields: (derivedSubtype)
                bool       Has_Null_Exclusion;
                Constraint Subtype_Constraint;
          */

          res = &getDeclTypeID(derivedSubtype.Subtype_Mark, ctx);
          break;
        }

      case A_Floating_Point_Definition:            // 3.5.7(2)
        {
          /*
             unused fields:
                Expression           Digits_Expression;
                Range_Constraint     Real_Range_Constraint;
          */
          res = sb::buildFloatType();
          break;
        }

      case Not_A_Type_Definition:                  // An unexpected element
      case A_Derived_Record_Extension_Definition:  // 3.4(2)     -> Trait_Kinds
      case An_Enumeration_Type_Definition:         // 3.5.1(2)
      case A_Signed_Integer_Type_Definition:       // 3.5.4(3)
      case A_Modular_Type_Definition:              // 3.5.4(4)
      case A_Root_Type_Definition:                 // 3.5.4(14):  3.5.6(3)
      case An_Ordinary_Fixed_Point_Definition:     // 3.5.9(3)
      case A_Decimal_Fixed_Point_Definition:       // 3.5.9(6)
      case An_Unconstrained_Array_Definition:      // 3.6(2)
      case A_Constrained_Array_Definition:         // 3.6(2)
      case A_Record_Type_Definition:               // 3.8(2)     -> Trait_Kinds
      case A_Tagged_Record_Type_Definition:        // 3.8(2)     -> Trait_Kinds
      //  //|A2005 start
      case An_Interface_Type_Definition:           // 3.9.4      -> Interface_Kinds
      //  //|A2005 end
      case An_Access_Type_Definition:              // 3.10(2)    -> Access_Type_Kinds
      default:
        {
          res = sb::buildVoidType();
          logWarn() << "unhandled type kind " << typenode.Type_Kind << std::endl;
        }
    }

    return SG_DEREF(res);
  }


  SgExpression&
  getOperator(Expression_Struct& expr, AstContext ctx)
  {
    ROSE_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    std::string operstring = "UnknownOperator";

    switch (expr.Operator_Kind)
    {
      case An_And_Operator:                   // and
        operstring = "and";
        break;

      case An_Or_Operator:                    // or
        operstring = "or";
        break;

      case An_Xor_Operator:                   // xor
        operstring = "xor";
        break;

      case An_Equal_Operator:                 // =
        operstring = "=";
        break;

      case A_Not_Equal_Operator:              // /=
        operstring = "/=";
        break;

      case A_Less_Than_Operator:              // <
        operstring = "<";
        break;

      case A_Less_Than_Or_Equal_Operator:     // <=
        operstring = "<=";
        break;

      case A_Greater_Than_Operator:           // >
        operstring = ">";
        break;

      case A_Greater_Than_Or_Equal_Operator:  // >=
        operstring = ">=";
        break;

      case A_Plus_Operator:                   // +
        operstring = "+";
        break;

      case A_Minus_Operator:                  // -
        operstring = "-";
        break;

      case A_Concatenate_Operator:            // &
        operstring = "&";
        break;

      case A_Unary_Plus_Operator:             // +
        operstring = "+";
        break;

      case A_Unary_Minus_Operator:            // -
        operstring = "-";
        break;

      case A_Multiply_Operator:               // *
        operstring = "*";
        break;

      case A_Divide_Operator:                 // /
        operstring = "/";
        break;

      case A_Mod_Operator:                    // mod
        operstring = "mod";
        break;

      case A_Rem_Operator:                    // rem
        operstring = "rem";
        break;

      case An_Exponentiate_Operator:          // **
        operstring = "**";
        break;

      case An_Abs_Operator:                   // abs
        operstring = "abs";
        break;

      case A_Not_Operator:                    // not
        operstring = "not";
        break;

      case Not_An_Operator:                   // An unexpected element
      default:
        ROSE_ASSERT(false);
        break;
    }

    return SG_DEREF(sb::buildOpaqueVarRefExp(operstring, &ctx.scope()));
  }


  SgExpression&
  getEnumLiteral(Expression_Struct& expr, AstContext ctx)
  {
    ROSE_ASSERT(expr.Expression_Kind == An_Enumeration_Literal);

    Element_Struct* typedcl = retrieveAsOpt<Element_Struct>(asisMap, expr.Corresponding_Expression_Type_Definition);

    ROSE_ASSERT (!typedcl);

    std::string   enumstr(expr.Name_Image);
    SgExpression* res = NULL;

    if (enumstr == "True")
      res = sb::buildBoolValExp(1);
    else if (enumstr == "False")
      res = sb::buildBoolValExp(0);

    return SG_DEREF( res );
  }


  typedef std::pair<std::string, int> NameKeyPair;

  NameKeyPair
  singleName(Declaration_Struct& decl, AstContext ctx)
  {
    ElemIdRange     range = idRange(decl.Names);
    ROSE_ASSERT(range.size() == 1);

    Element_Struct& dclname = retrieveAs<Element_Struct>(asisMap, *range.first);

    return NameKeyPair(getName(dclname, ctx), dclname.ID);
  }


  SgExpression&
  getExpr(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;
    SgExpression*      res  = NULL;

    switch (expr.Expression_Kind)
    {
      case Not_An_Expression:                         // An unexpected element
        {
          ROSE_ASSERT(false);
          break;
        }

      case An_Identifier:                             // 4.1
        {
          if (SgInitializedName* var = findFirst(asisVars, expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
          {
            res = sb::buildVarRefExp(var, &ctx.scope());
          }
          else if (SgDeclarationStatement* dcl = findFirst(asisDecls, expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
          {
            SgFunctionDeclaration* fundcl = isSgFunctionDeclaration(dcl);
            ROSE_ASSERT(fundcl);

            res = sb::buildFunctionRefExp(fundcl);
          }
          else if (SgInitializedName* exc = findFirst(asisExcps, expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
          {
            res = &mkExceptionRef(*exc, ctx.scope());
          }
          else
          {
            logWarn() << "ADDING opaque name reference: "
                      << expr.Name_Image
                      << std::endl;

            res = sb::buildOpaqueVarRefExp(expr.Name_Image, &ctx.scope());
          }

          /* unused fields: (Expression_Struct)
               ** depends on the branch
               Defining_Name_ID      Corresponding_Generic_Element;
          */
          break;
        }

      case A_Function_Call:                           // 4.1
        {
          logInfo() << "function call "
                    << expr.Is_Prefix_Notation << " "
                    << expr.Is_Prefix_Call
                    << std::endl;

  #if FAST_FUNCTION_RESOLUTION
          SgDeclarationStatement& dcl    = retrieveDeclaration(expr.Corresponding_Called_Function);
          SgFunctionDeclaration*  fundcl = isSgFunctionDeclaration(&dcl);

          ROSE_ASSERT(fundecl);
          SgFunctionRefExp&       target = SG_DEREF(sb::buildFunctionRefExp(fundcl));
  #endif /* FAST_FUNCTION_RESOLUTION */

          SgExpression&           target = getExprID(expr.Prefix, ctx);
          ElemIdRange             range  = idRange(expr.Function_Call_Parameters);
          SgExprListExp&          arglst = traverseIDs(range, asisMap, ArglistCreator(ctx));

          res = sb::buildFunctionCallExp(&target, &arglst);

          /* unused fields:
             Expression_Struct
               Expression_ID         Prefix;
               bool                  Is_Prefix_Call;
               bool                  Is_Generalized_Reference;
               bool                  Is_Dispatching_Call;
               bool                  Is_Call_On_Dispatching_Operation;
          */
          break;
        }

      case An_Integer_Literal:                        // 2.4
        {
          res = sb::buildIntVal(conv<int>(expr.Value_Image));
          /* unused fields: (Expression_Struct)
               enum Attribute_Kinds  Attribute_Kind
          */
          break;
        }

      case A_Character_Literal:                       // 4.1
        {
          res = sb::buildCharVal(*expr.Name_Image);
          /* unused fields: (Expression_Struct)
               Defining_Name_ID      Corresponding_Name_Definition;
               Defining_Name_List    Corresponding_Name_Definition_List;
               Element_ID            Corresponding_Name_Declaration;
               Defining_Name_ID      Corresponding_Generic_Element
          */
          break;
        }

      case A_Real_Literal:                            // 2.4.1
        {
          res = sb::buildDoubleVal(conv<long double>(expr.Value_Image));
          /* unused fields: (Expression_Struct)
               enum Attribute_Kinds  Attribute_Kind;
          */
          break;
        }

      case An_Operator_Symbol:                        // 4.1
        {
          res = &getOperator(expr, ctx);
          /* unused fields:
             Expression_Struct
             char                 *Name_Image;
             Defining_Name_ID      Corresponding_Name_Definition;
             Defining_Name_List    Corresponding_Name_Definition_List;
             Element_ID            Corresponding_Name_Declaration;
             Defining_Name_ID      Corresponding_Generic_Element;
          */
          break;
        }

      case An_Enumeration_Literal:                    // 4.1
        {
          res = &getEnumLiteral(expr, ctx);
          /* unused fields: (Expression_Struct)
             char                 *Name_Image;
             Defining_Name_ID      Corresponding_Name_Definition;
             Defining_Name_List    Corresponding_Name_Definition_List;
             Element_ID            Corresponding_Name_Declaration;
             Defining_Name_ID      Corresponding_Generic_Element;
          */
          break;
        }

      case A_Selected_Component:                      // 4.1.3
        {
          SgExpression& prefix   = getExprID(expr.Prefix, ctx);
          SgExpression& selector = getExprID(expr.Selector, ctx);

          res = &mkSelectedComponent(prefix, selector);
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case An_And_Then_Short_Circuit:                 // 4.4
        {
          SgExpression& lhs = getExprID(expr.Short_Circuit_Operation_Left_Expression, ctx);
          SgExpression& rhs = getExprID(expr.Short_Circuit_Operation_Right_Expression, ctx);

          res = sb::buildAndOp(&lhs, &rhs);
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case An_Or_Else_Short_Circuit:                  // 4.4
        {
          SgExpression& lhs = getExprID(expr.Short_Circuit_Operation_Left_Expression, ctx);
          SgExpression& rhs = getExprID(expr.Short_Circuit_Operation_Right_Expression, ctx);

          res = sb::buildOrOp(&lhs, &rhs);
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case A_Parenthesized_Expression:                // 4.4
        {
          // \todo can we ignore parenthesis in ADA

          res = &getExprID(expr.Expression_Parenthesized, ctx);
          /* unused fields: (Expression_Struct)
          */
          break;
        }
      case A_Type_Conversion:                         // 4.6
        {
          SgExpression& exp = getExprID(expr.Converted_Or_Qualified_Expression, ctx);
          SgType&       ty  = getDeclTypeID(expr.Converted_Or_Qualified_Subtype_Mark, ctx);
          
          res = sb::buildCastExp(&exp, &ty);
          /* unused fields: (Expression_Struct)
               Expression_ID         Predicate;
          */
          break;
        }

      case A_Box_Expression:                          // Ada 2005 4.3.1(4): 4.3.3(3:6)
      case A_String_Literal:                          // 2.6

      case An_Explicit_Dereference:                   // 4.1

      case An_Indexed_Component:                      // 4.1.1
      case A_Slice:                                   // 4.1.2
      case An_Attribute_Reference:                    // 4.1.4  -> Attribute_Kinds
      case A_Record_Aggregate:                        // 4.3
      case An_Extension_Aggregate:                    // 4.3
      case A_Positional_Array_Aggregate:              // 4.3
      case A_Named_Array_Aggregate:                   // 4.3

      case An_In_Membership_Test:                     // 4.4  Ada 2012
      case A_Not_In_Membership_Test:                  // 4.4  Ada 2012

      case A_Null_Literal:                            // 4.4
      case A_Raise_Expression:                        // 4.4 Ada 2012 (AI12-0022-1)

      case A_Qualified_Expression:                    // 4.7
      case An_Allocation_From_Subtype:                // 4.8
      case An_Allocation_From_Qualified_Expression:   // 4.8

      case A_Case_Expression:                         // Ada 2012
      case An_If_Expression:                          // Ada 2012
      case A_For_All_Quantified_Expression:           // Ada 2012
      case A_For_Some_Quantified_Expression:          // Ada 2012
      default:
        logWarn() << "unhandled expression: " << expr.Expression_Kind << std::endl;
        res = sb::buildIntVal();
    }

    return SG_DEREF(res);
  }

  SgExpression&
  getExprID(Element_ID el, AstContext ctx)
  {
    return getExpr(retrieveAs<Element_Struct>(asisMap, el), ctx);
  }
  
  SgExpression&
  getExprID_opt(Element_ID el, AstContext ctx)
  {
    if (el == -1) return SG_DEREF( sb::buildNullExpression() );
    
    return getExprID(el, ctx);
  }


  SgExpression&
  getArg(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Association);

    Association_Struct& assoc      = elem.The_Union.Association;
    ROSE_ASSERT(assoc.Association_Kind == A_Parameter_Association);

    SgExpression&       arg        = getExprID(assoc.Actual_Parameter, ctx);
    Element_Struct*     formalParm = retrieveAsOpt<Element_Struct>(asisMap, assoc.Formal_Parameter);

    if (!formalParm) return arg;

    ROSE_ASSERT(formalParm->Element_Kind == An_Expression);

    Expression_Struct&  formalName = formalParm->The_Union.Expression;
    ROSE_ASSERT(formalName.Expression_Kind == An_Identifier);

    return SG_DEREF(sb::buildActualArgumentExpression(formalName.Name_Image, &arg));
  }

  SgTypeModifier
  handleModes(Mode_Kinds asisKind)
  {
    SgTypeModifier res;

    switch (asisKind)
    {
      case A_Default_In_Mode:
        res.setDefault();
        break;

      case An_In_Mode:
        res.setIntent_in();
        break;

      case An_Out_Mode:
        res.setIntent_out();
        break;

      case An_In_Out_Mode:
        res.setIntent_inout();
        break;

      case Not_A_Mode:
      default:
        ROSE_ASSERT(false);
        break;
    }

    return res;
  }


  SgExpression*
  getVarInit(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Parameter_Specification
               || decl.Declaration_Kind == A_Real_Number_Declaration
               || decl.Declaration_Kind == An_Integer_Number_Declaration
               );

    Element_Struct* elem = retrieveAsOpt<Element_Struct>(asisMap, decl.Initialization_Expression);

    return elem ? &getExpr(*elem, ctx) : NULL;
  }


  SgExpression*
  cloneNonNull(SgExpression* exp)
  {
    if (exp == NULL) return NULL;

    return si::deepCopy(exp);
  }

  SgInitializedNamePtrList
  constructInitializedNamePtrList( std::map<int, SgInitializedName*>& m,
                                   const NameCreator::result_container& names,
                                   SgType& dcltype,
                                   SgExpression* initexpr = NULL
                                 )
  {
    SgInitializedNamePtrList lst;
    int                      num      = names.size();

    for (int i = 0; i < num; ++i)
    {
      const std::string& name = names.at(i).first;
      Element_ID         id   = names.at(i).second;
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, cloneNonNull(initexpr));

      lst.push_back(&dcl);
      recordDcl(m, id, dcl);
    }

    return lst;
  }

  SgVariableDeclaration&
  getParm(Element_Struct& elem, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ROSE_ASSERT(elem.Element_Kind == A_Declaration);

    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    ROSE_ASSERT(asisDecl.Declaration_Kind == A_Parameter_Specification);

    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);
    name_container           names    = traverseIDs(range, asisMap, NameCreator(ctx));
    SgType&                  parmtype = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
    SgExpression*            initexpr = getVarInit(asisDecl, ctx);
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars, names, parmtype, initexpr);

    delete initexpr;

    /* unused fields:
         bool                           Has_Aliased;
         Mode_Kinds                     Mode_Kind;
         bool                           Has_Null_Exclusion;
    */

    return mkParameter(dclnames, handleModes(asisDecl.Mode_Kind), ctx.scope());
  }

  //
  // types

  typedef SgType& (*TypeModiferFn) (SgType&);

  SgType& tyIdentity(SgType& ty) { return ty; }

  SgType& tyConstify(SgType& ty) { return SG_DEREF(sb::buildConstType(&ty)); }


  //
  // helper function for combined handling of variables and constant declarations

  void
  handleNumVarCstDecl(Declaration_Struct& decl, AstContext ctx, SgType& dcltype)
  {
    typedef NameCreator::result_container name_container;

    SgExpression*            initexp  = getVarInit(decl, ctx);
    ElemIdRange              range    = idRange(decl.Names);
    name_container           names    = traverseIDs(range, asisMap, NameCreator(ctx));
    SgScopeStatement&        scope    = ctx.scope();
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars, names, dcltype, initexp);

    delete initexp;

    scope.append_statement(&mkVarDecl(dclnames, scope));
  }

  void
  handleNumberDecl(Declaration_Struct& decl, AstContext ctx, SgType& numty)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    handleNumVarCstDecl(decl, ctx, cstty);
  }

  void
  handleVarCstDecl(Declaration_Struct& dcl, AstContext ctx, TypeModiferFn tyModifier)
  {
    handleNumVarCstDecl(dcl, ctx, tyModifier(getVarType(dcl, ctx)));
  }

  struct DeclareSubtype
  {
    DeclareSubtype(SgType& tyrep, AstContext astctx)
    : ty(tyrep), scope(astctx.scope())
    {}

    void operator()(NameCreator::result_container::value_type nameelem)
    {
      const std::string&    name = nameelem.first;
      Element_ID            id   = nameelem.second;
      SgTypedefDeclaration& dcl  = mkSubtypeType(name, ty, scope);

      scope.append_statement(&dcl);
      recordDcl(asisTypes, id, dcl);
    }

    SgType&           ty;
    SgScopeStatement& scope;
    // AstContext        ctx;
  };


  void handleDeclaration(Element_Struct& elem, AstContext ctx)
  {
    Declaration_Struct& decl = elem.The_Union.Declaration;

    switch (decl.Declaration_Kind)
    {
      case A_Package_Declaration:                    // 7.1(2)
        {
          logInfo() << "package decl" << std::endl;
          ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

          traverseIDs(range, asisMap, ElemCreator(ctx));

          range = idRange(decl.Private_Part_Declarative_Items);
          ROSE_ASSERT((!range.empty()) == decl.Is_Private_Present);

          traverseIDs(range, asisMap, ElemCreator(ctx));
          break;
        }

      case A_Package_Body_Declaration:               // 7.2(2)
        {
          logInfo() << "package body decl (unhandled)" << std::endl;
          break;
        }

      case A_Function_Declaration:                   // 6.1(4)   -> Trait_Kinds
      case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
        {
          logInfo() << (decl.Declaration_Kind == A_Function_Declaration ? "function" : "procedure") << " decl"
                    << std::endl;

          SgScopeStatement&      outer   = ctx.scope();
          NameKeyPair            adaname = singleName(decl, ctx);
          SgFunctionDeclaration& procdcl = mkProcedure(adaname.first, outer);

          recordDcl(asisDecls, elem.ID, procdcl);

          ElemIdRange            range   = idRange(decl.Parameter_Profile);
          AstContext             parmctx = ctx.scope(SG_DEREF(procdcl.get_functionParameterScope()));

          traverseIDs(range, asisMap, ParmlistCreator(procdcl, parmctx));

          outer.append_statement(&procdcl);

          /* unhandled fields

             bool                          Has_Abstract
             bool                          Is_Overriding_Declaration
             bool                          Is_Not_Overriding_Declaration
             bool                          Is_Dispatching_Operation
             Declaration_ID                Corresponding_Declaration
             Declaration_ID                Corresponding_Body
             Declaration_ID                Corresponding_Subprogram_Derivation
             Type_Definition_ID            Corresponding_Type

           +func:
             bool                          Is_Not_Null_Return
             Element_ID                    Result_Profile
             Declaration_ID                Corresponding_Equality_Operator

             break;
          */

          break;
        }

      case A_Function_Body_Declaration:              // 6.3(2)
      case A_Procedure_Body_Declaration:             // 6.3(2)
        {
          typedef std::pair<SgFunctionDeclaration*, SgBasicBlock*> ProcDefPair;

          logInfo() << (decl.Declaration_Kind == A_Function_Body_Declaration ? "function" : "procedure") << " body decl"
                    << std::endl;

          SgScopeStatement&      outer   = ctx.scope();
          NameKeyPair            adaname = singleName(decl, ctx);
          ProcDefPair            defpair = mkProcedureDef(adaname.first, outer);

          SgFunctionDeclaration& proc    = SG_DEREF(defpair.first);
          SgBasicBlock&          blk     = SG_DEREF(defpair.second);

          recordDcl(asisDecls, elem.ID, proc);

          ElemIdRange            range = idRange(decl.Parameter_Profile);

          traverseIDs(range, asisMap, ParmlistCreator(proc, ctx));
          outer.append_statement(&proc);

          AstContext             inner  = ctx.scope(blk);

          {
            ElemIdRange range = idRange(decl.Body_Declarative_Items);

            traverseIDs(range, asisMap, ElemCreator(inner));
          }

          {
            ElemIdRange range = idRange(decl.Body_Statements);

            traverseIDs(range, asisMap, StmtCreator(inner));
          }

          /* unhandled fields

             Element_ID                     Body_Block_Statement

           +func:
             bool                           Is_Not_Null_Return
             Element_ID                     Result_Profile

             break;
          */
          break;
        }

      case An_Ordinary_Type_Declaration:            // 3.2.1(3)
        {
          typedef NameCreator::result_container name_container;

          logInfo() << "Ordinary Type "
                    << "\n  abstract: " << decl.Has_Abstract
                    << "\n  limited: " << decl.Has_Limited
                    << std::endl;
          ElemIdRange    range = idRange(decl.Names);
          name_container names = traverseIDs(range, asisMap, NameCreator(ctx));
          SgType&        ty    = getTypedeclType(decl, ctx);

          std::for_each(names.begin(), names.end(), DeclareSubtype(ty, ctx));

          ROSE_ASSERT(names.size() == 1);
          logInfo() << "  name: " << names.front().first
                    << std::endl;

          /* unused fields
                    bool                           Has_Abstract;
                    bool                           Has_Limited;
                    Definition_ID                  Discriminant_Part;
                    Declaration_ID                 Corresponding_Type_Declaration;
                    Declaration_ID                 Corresponding_Type_Partial_View;
                    Declaration_ID                 Corresponding_First_Subtype;
                    Declaration_ID                 Corresponding_Last_Constraint;
                    Declaration_ID                 Corresponding_Last_Subtype;
          */
          break;
        }

      case A_Subtype_Declaration:                    // 3.2.2(2)
        {
          
          /* unused fields:
                Declaration_ID                 Corresponding_First_Subtype;
                Declaration_ID                 Corresponding_Last_Constraint;
                Declaration_ID                 Corresponding_Last_Subtype;
          */
          break;
        }
        
      case A_Variable_Declaration:                   // 3.3.1(2) -> Trait_Kinds
        {
          handleVarCstDecl(decl, ctx, tyIdentity);
          /* unused fields:
               bool                           Has_Aliased;
          */
          break;
        }

      case An_Integer_Number_Declaration:            // 3.3.2(2)
        {
          handleNumberDecl(decl, ctx, SG_DEREF(sb::buildIntType()));

          /* unused fields:
          */
          break;
        }
      
      case A_Constant_Declaration:
        {
          handleVarCstDecl(decl, ctx, tyConstify);
          /* unused fields:
               bool                           Has_Aliased;
          */
          break;
        }
      
      case A_Real_Number_Declaration:                // 3.5.6(2)
        {
          handleNumberDecl(decl, ctx, SG_DEREF(sb::buildFloatType()));

          /* unused fields:
           */
          break;
        }

      case An_Exception_Declaration:                 // 11.1(2)
        {
          typedef NameCreator::result_container name_container;

          ElemIdRange              range    = idRange(decl.Names);
          name_container           names    = traverseIDs(range, asisMap, NameCreator(ctx));
          SgScopeStatement&        scope    = ctx.scope();
          SgType&                  excty    = SG_DEREF( sb::buildOpaqueType("Exception", &scope) );
          SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisExcps, names, excty);

          scope.append_statement(&mkExceptionDecl(dclnames, scope));
          break;
        }


      case Not_A_Declaration:                       // An unexpected element
        {
          logWarn() << "Not a declaration" << std::endl;
          ROSE_ASSERT(false);
          break;
        }

      case A_Parameter_Specification:                // 6.1(15)  -> Trait_Kinds
        {
          // handled in getParm
          ROSE_ASSERT(false);
          break;
        }

      case A_Task_Type_Declaration:                  // 9.1(2)
      case A_Protected_Type_Declaration:             // 9.4(2)
      case An_Incomplete_Type_Declaration:           // 3.2.1(2):3.10(2)
      case A_Tagged_Incomplete_Type_Declaration:     //  3.10.1(2)
      case A_Private_Type_Declaration:               // 3.2.1(2):7.3(2) -> Trait_Kinds
      case A_Private_Extension_Declaration:          // 3.2.1(2):7.3(3) -> Trait_Kinds
      case A_Deferred_Constant_Declaration:          // 3.3.1(6):7.4(2) -> Trait_Kinds
      case A_Single_Task_Declaration:                // 3.3.1(2):9.1(3)
      case A_Single_Protected_Declaration:           // 3.3.1(2):9.4(2)
      case An_Enumeration_Literal_Specification:     // 3.5.1(3)
      case A_Discriminant_Specification:             // 3.7(5)   -> Trait_Kinds
      case A_Component_Declaration:                  // 3.8(6)
      case A_Loop_Parameter_Specification:           // 5.5(4)   -> Trait_Kinds
      case A_Generalized_Iterator_Specification:     // 5.5.2    -> Trait_Kinds
      case An_Element_Iterator_Specification:        // 5.5.2    -> Trait_Kinds
      case A_Return_Variable_Specification:          // 6.5
      case A_Return_Constant_Specification:          // 6.5
      case A_Null_Procedure_Declaration:             // 6.7
      case An_Expression_Function_Declaration:       // 6.8
      case An_Object_Renaming_Declaration:           // 8.5.1(2)
      case An_Exception_Renaming_Declaration:        // 8.5.2(2)
      case A_Package_Renaming_Declaration:           // 8.5.3(2)
      case A_Procedure_Renaming_Declaration:         // 8.5.4(2)
      case A_Function_Renaming_Declaration:          // 8.5.4(2)
      case A_Generic_Package_Renaming_Declaration:   // 8.5.5(2)
      case A_Generic_Procedure_Renaming_Declaration: // 8.5.5(2)
      case A_Generic_Function_Renaming_Declaration:  // 8.5.5(2)
      case A_Task_Body_Declaration:                  // 9.1(6)
      case A_Protected_Body_Declaration:             // 9.4(7)
      case An_Entry_Declaration:                     // 9.5.2(2)
      case An_Entry_Body_Declaration:                // 9.5.2(5)
      case An_Entry_Index_Specification:             // 9.5.2(2)
      case A_Procedure_Body_Stub:                    // 10.1.3(3)
      case A_Function_Body_Stub:                     // 10.1.3(3)
      case A_Package_Body_Stub:                      // 10.1.3(4)
      case A_Task_Body_Stub:                         // 10.1.3(5)
      case A_Protected_Body_Stub:                    // 10.1.3(6)
      case A_Choice_Parameter_Specification:         // 11.2(4)
      case A_Generic_Procedure_Declaration:          // 12.1(2)
      case A_Generic_Function_Declaration:           // 12.1(2)
      case A_Generic_Package_Declaration:            // 12.1(2)
      case A_Package_Instantiation:                  // 12.3(2)
      case A_Procedure_Instantiation:                // 12.3(2)
      case A_Function_Instantiation:                 // 12.3(2)
      case A_Formal_Object_Declaration:              // 12.4(2)  -> Mode_Kinds
      case A_Formal_Type_Declaration:                // 12.5(2)
      case A_Formal_Incomplete_Type_Declaration:
      case A_Formal_Procedure_Declaration:           // 12.6(2)
      case A_Formal_Function_Declaration:            // 12.6(2)
      case A_Formal_Package_Declaration:             // 12.7(2)
      case A_Formal_Package_Declaration_With_Box:    // 12.7(3)
      default:
        logWarn() << "unhandled declaration kind: " << decl.Declaration_Kind << std::endl;
    }
  }

  void handleClause(Element_Struct& elem, AstContext ctx)
  {
    Clause_Struct& clause = elem.The_Union.Clause;

    switch (clause.Clause_Kind)
    {
      case Not_A_Clause:                 // An unexpected element
        {
          logWarn() << "Not a clause" << std::endl;
          ROSE_ASSERT(false);
          break;
        }

      case A_With_Clause:                // 10.1.2
        {
          typedef std::vector<SgExpression*> ExprVec;

          ElemIdRange        range = idRange(clause.Clause_Names);
          ExprVec            uselst = traverseIDs(range, asisMap, WithClauseElemCreator(ctx));
          SgImportStatement& withClause = mkWithClause(uselst);

          ctx.scope().append_statement(&withClause);
          /* unused fields:
              bool   Has_Limited
          */
          break;
        }

      case A_Use_Package_Clause:         // 8.4
      case A_Use_Type_Clause:            // 8.4
      case A_Use_All_Type_Clause:        // 8.4: Ada 2012
      case A_Representation_Clause:      // 13.1     -> Representation_Clause_Kinds
      case A_Component_Clause:           // 13.5.1
      default:
        logWarn() << "unhandled clause kind: " << clause.Clause_Kind << std::endl;
    }
  }

  void handleElement(Element_Struct& elem, AstContext ctx)
  {
    switch (elem.Element_Kind)
    {
      case Not_An_Element:            // Nil_Element
        {
          logWarn() << "Not and Element (nil)" << std::endl;
          break;
        }

      case A_Declaration:             // Asis.Declarations
        {
          handleDeclaration(elem, ctx);
          break;
        }

      case A_Clause:                  // Asis.Clauses
        {
          handleClause(elem, ctx);
          break;
        }

      case A_Defining_Name:           // Asis.Declarations
        {
          // handled by getName
          ROSE_ASSERT(false);
          break;
        }

      case A_Definition:              // Asis.Definitions
      case A_Path:                    // Asis.Statements
      case A_Pragma:                  // Asis.Elements
      case An_Expression:             // Asis.Expressions
      case An_Association:            // Asis.Expressions
      case A_Statement:               // Asis.Statements
      case An_Exception_Handler:      // Asis.Statements
      default:
        logWarn() << "Unhandled element " << elem.Element_Kind << std::endl;
    }
  }


  void handleStmt(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    logInfo() << "a statement (in progress)" << std::endl;

    Statement_Struct& stmt = elem.The_Union.Statement;

    switch (stmt.Statement_Kind)
    {
      case A_Null_Statement:                    // 5.1
        {
          ctx.scope().append_statement(&mkNullStmt());
          /* unused fields:
          */
          break;
        }

      case An_Assignment_Statement:             // 5.2
        {
          SgExpression& lhs    = getExprID(stmt.Assignment_Variable_Name, ctx);
          SgExpression& rhs    = getExprID(stmt.Assignment_Expression, ctx);
          SgExpression& assign = SG_DEREF(sb::buildAssignOp(&lhs, &rhs));
          SgStatement&  sgnode = SG_DEREF(sb::buildExprStatement(&assign));

          ctx.scope().append_statement(&sgnode);
          /* unused fields:
          */
          break;
        }

      case A_Procedure_Call_Statement:          // 6.4
        {
          SgDeclarationStatement& tgt    = retrieveDcl(asisDecls, stmt.Corresponding_Called_Entity);
          SgFunctionDeclaration*  fundcl = isSgFunctionDeclaration(&tgt);
          SgFunctionRefExp&       funref = SG_DEREF(sb::buildFunctionRefExp(fundcl));
          ElemIdRange             range  = idRange(stmt.Call_Statement_Parameters);
          SgExprListExp&          arglst = traverseIDs(range, asisMap, ArglistCreator(ctx));
          SgStatement&            sgnode = SG_DEREF(sb::buildFunctionCallStmt(&funref, &arglst));

          ctx.scope().append_statement(&sgnode);
          /* unused fields:
              bool        Is_Prefix_Notation
              bool        Is_Dispatching_Call
              bool        Is_Call_On_Dispatching_Operation
              Element_ID  Called_Name
              Declaration Corresponding_Called_Entity_Unwound
          */
          break;
        }

      case An_If_Statement:                     // 5.3
        {
          ElemIdRange               range  = idRange(stmt.Statement_Paths);
          SgStatement&              sgnode = traverseIDs(range, asisMap, IfStmtCreator(ctx));

          ctx.scope().append_statement(&sgnode);
          /* unused fields:
          */
          break;
        }

      case A_Case_Statement:                    // 5.4
        {
          SgExpression&      caseexpr = getExprID(stmt.Case_Expression, ctx);
          SgBasicBlock&      casebody = mkBasicBlock();
          SgSwitchStatement& sgnode   = mkSwitchStmt(caseexpr, casebody);
          ElemIdRange        range    = idRange(stmt.Statement_Paths);

          traverseIDs(range, asisMap, ElemCreator(ctx.scope(casebody)));

          ctx.scope().append_statement(&sgnode);
          /* unused fields:
          */
          break;
        }

      case A_While_Loop_Statement:              // 5.5
        {
          SgExpression& cond      = getExprID(stmt.While_Condition, ctx);
          SgBasicBlock& block     = mkBasicBlock();
          ElemIdRange   loopStmts = idRange(stmt.Loop_Statements);
          SgWhileStmt&  sgnode    = mkWhileStmt(cond, block);

          recordDcl(asisLoops, elem.ID, sgnode);
          traverseIDs(loopStmts, asisMap, StmtCreator(ctx.scope(block)));
          ctx.scope().append_statement(&sgnode);
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Defining_Name_ID          Statement_Identifier;
                Element_ID                Corresponding_End_Name;
          */
          break;
        }

        case A_Loop_Statement:                    // 5.5
        {
          SgBasicBlock& block     = mkBasicBlock();
          ElemIdRange   loopStmts = idRange(stmt.Loop_Statements);
          SgStatement&  sgnode    = mkLoopStmt(block);
          
          recordDcl(asisLoops, elem.ID, sgnode);
          traverseIDs(loopStmts, asisMap, StmtCreator(ctx.scope(block)));
          ctx.scope().append_statement(&sgnode);
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
                Defining_Name_ID          Statement_Identifier;
                bool                      Is_Name_Repeated;
          */
          break;
        }
#if 0
      case A_For_Loop_Statement:                // 5.5
        {
          SgBasicBlock&      block     = mkBasicBlock();
          ElemIdRange        loopStmts = idRange(stmt.Loop_Statements);
          SgInitializedName* var       = findFirst(asisVars, stmt.For_Loop_Parameter_Specification);
          ROSE_ASSERT(var);

          SgStatement&       sgnode    = mkForLoopStmt(*var, range, block);

          recordDcl(asisLoops, elem.ID, sgnode);
          traverseIDs(loopStmts, asisMap, StmtCreator(ctx.scope(block)));
          ctx.scope().append_statement(&sgnode);

          /* unused fields:
               Pragma_Element_ID_List Pragmas;
               Element_ID             Corresponding_End_Name;
               Defining_Name_ID       Statement_Identifier;
          */
          break;
        }
#endif


      case A_Block_Statement:                   // 5.6
        {
          SgBasicBlock& sgnode    = mkBasicBlock();
          ElemIdRange   blkDecls  = idRange(stmt.Block_Declarative_Items);
          ElemIdRange   blkStmts  = idRange(stmt.Block_Statements);

          traverseIDs(blkDecls, asisMap, StmtCreator(ctx.scope(sgnode)));
          traverseIDs(blkStmts, asisMap, StmtCreator(ctx.scope(sgnode)));

          ctx.scope().append_statement(&sgnode);
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Defining_Name_ID          Statement_Identifier;
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
                bool                      Is_Declare_Block
                Exception_Handler_List    Block_Exception_Handlers;
          */
          break;
        }

      case A_Return_Statement:                  // 6.5
        {
          Element_Struct* elem   = retrieveAsOpt<Element_Struct>(asisMap, stmt.Return_Expression);
          SgExpression*   retval = elem ? &getExpr(*elem, ctx) : NULL;
          SgStatement&    sgnode = SG_DEREF( sb::buildReturnStmt(retval) );

          ctx.scope().append_statement(&sgnode);
          /* unused fields:
          */
          break;
        }

      case A_Raise_Statement:                   // 11.3
        {
          SgExpression&   raised = getExprID(stmt.Raised_Exception, ctx);
          SgStatement&    sgnode = mkRaiseStmt(raised);

          ctx.scope().append_statement(&sgnode);
          /* unused fields:
               Expression_ID          Associated_Message;
          */
          break;
        }

      case An_Exit_Statement:                   // 5.7
        {
          SgStatement&  exitedLoop    = retrieveDclIfAvail(asisLoops, stmt.Corresponding_Loop_Exited, mkNullStmt); 
          SgExpression& exitCondition = getExprID_opt(stmt.Exit_Condition, ctx);
          SgStatement&  sgnode        = mkAdaExitStmt(exitedLoop, exitCondition, true);
          
          ctx.scope().append_statement(&sgnode);
          /* unused fields:
               Expression_ID          Exit_Loop_Name;
               Expression_ID          Exit_Condition;
               Expression_ID          Corresponding_Loop_Exited;
          */
          break;
        }
      
      case Not_A_Statement:                     // An unexpected element
        {
          ROSE_ASSERT(false);
          break;
        }

      
      
      case A_Goto_Statement:                    // 5.8

      //|A2005 start
      case An_Extended_Return_Statement:        // 6.5
      //|A2005 end

      case An_Accept_Statement:                 // 9.5.2
      case An_Entry_Call_Statement:             // 9.5.3
      case A_Requeue_Statement:                 // 9.5.4
      case A_Requeue_Statement_With_Abort:      // 9.5.4
      case A_Delay_Until_Statement:             // 9.6
      case A_Delay_Relative_Statement:          // 9.6
      case A_Terminate_Alternative_Statement:   // 9.7.1
      case A_Selective_Accept_Statement:        // 9.7.1
      case A_Timed_Entry_Call_Statement:        // 9.7.2
      case A_Conditional_Entry_Call_Statement:  // 9.7.3
      case An_Asynchronous_Select_Statement:    // 9.7.4
      case An_Abort_Statement:                  // 9.8
      case A_Code_Statement:                    // 13.8 assembly
      default:
        logWarn() << "Unhandled statement " << stmt.Statement_Kind << std::endl;
    }
  }

  std::string
  getName(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Defining_Name);

    return std::string(elem.The_Union.Defining_Name.Defining_Name_Image);
  }

  void handleElementID(Element_ID id, AstContext ctx)
  {
    handleElement(retrieveAs<Element_Struct>(asisMap, id), ctx);
  }

  void handleUnit(Unit_Struct& adaUnit, AstContext ctx);

  // ADA

  struct UnitCreator
  {
      explicit
      UnitCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Unit_Struct& adaUnit)
      {
        handleUnit(adaUnit, ctx);
      }

      void operator()(Unit_Struct_List_Struct& adaUnit)
      {
        (*this)(adaUnit.Unit);
      }

      AstContext ctx;
  };

  struct PrnUnitHeader
  {
    explicit
    PrnUnitHeader(Unit_Struct& adaUnit)
    : unit(adaUnit)
    {}

    Unit_Struct& unit;
  };

  std::ostream& operator<<(std::ostream& os, PrnUnitHeader obj)
  {
    os << "\n " << obj.unit.ID                  << " (ID)"
       << "\n " << obj.unit.Unit_Full_Name      << " (Unit_Full_Name)"
       << "\n " << obj.unit.Unique_Name         << " (Unique_Name)"
       << "\n " << obj.unit.Text_Name           << " (Text_Name)"
       << "\n " << obj.unit.Object_Name         << " (Object_Name)"
       << "\n " << obj.unit.Exists              << " (Exists)"
       << "\n " << obj.unit.Can_Be_Main_Program << " (Can_Be_Main_Program)"
       << "\n " << obj.unit.Is_Body_Required    << " (Is_Body_Required)"
       << "\n " << obj.unit.Unit_Declaration    << " (Unit_Declaration)"
       ;

    return os;
  }


  void handleUnit(Unit_Struct& adaUnit, AstContext ctx)
  {
    // dispatch based on unit kind
    switch (adaUnit.Unit_Kind)
    {
      case A_Function:
      case A_Procedure:
        {
          logInfo() << "A " << (adaUnit.Unit_Kind == A_Function ? "function" : "procedure")
                    << PrnUnitHeader(adaUnit)
                    << "\n " << adaUnit.Corresponding_Parent_Declaration << " (Corresponding_Parent_Declaration)"
                    << "\n " << adaUnit.Corresponding_Body << " (Corresponding_Body)"
                    << std::endl;

          ElemIdRange                    elemRange = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(elemRange, asisMap, ElemCreator(ctx));
          handleElementID(adaUnit.Unit_Declaration, ctx);
          /* break; */
          break;
        }

      case A_Function_Body:
      case A_Procedure_Body:
        {
          logInfo() << "A " << (adaUnit.Unit_Kind == A_Function_Body ? "function" : "procedure") << " body"
                    << PrnUnitHeader(adaUnit)
                    << std::endl;
          /*
          == Moved handling of units to element processing ==

          SgScopeStatement&      outer   = ctx.scope();
          ProcDefPair            defpair = mkProcedureDef(adaUnit.Unit_Full_Name, outer);
          SgFunctionDeclaration& proc    = SG_DEREF(defpair.first);
          SgBasicBlock&          blk     = SG_DEREF(defpair.second);
          UnitIdRange            range   = idRange(adaUnit.Subunits);

          outer.append_statement(&proc);
          traverseIDs(range, asisMap, UnitCreator(blk));
          */

          ElemIdRange                    elemRange = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(elemRange, asisMap, ElemCreator(ctx));
          handleElementID(adaUnit.Unit_Declaration, ctx);
          /* break; */
          break;
        }

      case A_Package_Body:
      case A_Package:
        {
          logInfo() << "A package"
                    << PrnUnitHeader(adaUnit)
                    << std::endl;

          SgScopeStatement&                outer = ctx.scope();
          SgNamespaceDeclarationStatement& nsdcl = mkPackage(adaUnit.Unit_Full_Name, outer);
          SgNamespaceDefinitionStatement&  nsdef = SG_DEREF(nsdcl.get_definition());

          outer.append_statement(&nsdcl);

          ElemIdRange                      elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange                      unitRange = idRange(adaUnit.Corresponding_Children);

          traverseIDs(elemRange, asisMap, ElemCreator(ctx));
          traverseIDs(unitRange, asisMap, UnitCreator(ctx.scope(nsdef)));

          handleElementID(adaUnit.Unit_Declaration, ctx.scope(nsdef));
          /* break; */
          break;
        }

      case Not_A_Unit:
        {
          logWarn() << "Not a Unit" << std::endl;
          ROSE_ASSERT(false);
        }

      case A_Package_Instance:
      case A_Generic_Package:

      case A_Generic_Procedure:
      case A_Generic_Function:

      case A_Procedure_Instance:
      case A_Function_Instance:

      case A_Procedure_Renaming:
      case A_Function_Renaming:
      case A_Package_Renaming:

      case A_Generic_Procedure_Renaming:
      case A_Generic_Function_Renaming:
      case A_Generic_Package_Renaming:

      //  A unit interpreted only as the completion of a function: or a unit
      //  interpreted as both the declaration and body of a library
      //  function. Reference Manual 10.1.4(4)

      case A_Procedure_Body_Subunit:
      case A_Function_Body_Subunit:
      case A_Package_Body_Subunit:
      case A_Task_Body_Subunit:
      case A_Protected_Body_Subunit:

      case A_Nonexistent_Declaration:
      //  A unit that does not exist but is:
      //    1) mentioned in a with clause of another unit or:
      //    2) a required corresponding library_unit_declaration
      case A_Nonexistent_Body:
      //  A unit that does not exist but is:
      //     1) known to be a corresponding subunit or:
      //     2) a required corresponding library_unit_body
      case A_Configuration_Compilation:
      //  Corresponds to the whole content of a compilation with no
      //  compilation_unit, but possibly containing comments, configuration
      //  pragmas, or both. Any Context can have at most one unit of
      //  case A_Configuration_Compilation kind. A unit of
      //  case A_Configuration_Compilation does not have a name. This unit
      //  represents configuration pragmas that are "in effect".
      //
      //  GNAT-specific note: In case of GNAT the requirement to have at most
      //  one unit of case A_Configuration_Compilation kind does not make sense: in
      //  GNAT compilation model configuration pragmas are contained in
      //  configuration files, and a compilation may use an arbitrary number
      //  of configuration files. That is, (Elements representing) different
      //  configuration pragmas may have different enclosing compilation units
      //  with different text names. So in the ASIS implementation for GNAT a
      //  Context may contain any number of units of
      //  case A_Configuration_Compilation kind
      case An_Unknown_Unit:

      default:;
        logWarn() << "unit kind unhandled: " << adaUnit.Unit_Kind << std::endl;
        //~ ROSE_ASSERT(false);
    }
  }
  
  void initializeAdaTypes()
  {
    adaTypes[std::string("Integer")]   = sb::buildIntType();  
    adaTypes[std::string("Character")] = sb::buildCharType();  
    
    // \todo items
    adaTypes[std::string("Float")]     = sb::buildFloatType();  // Float is a subtype of Real
    adaTypes[std::string("Positive")]  = sb::buildIntType();    // Positive is a subtype of int  
    adaTypes[std::string("Natural")]   = sb::buildIntType();    // Natural is a subtype of int  
    adaTypes[std::string("Boolean")]   = sb::buildBoolType();   // Boolean is an enumeration of True and False  
  }
}

void secondConversion(Nodes_Struct& headNodes, SgSourceFile* file)
{
  ROSE_ASSERT(file);

  initializeAdaTypes();

  Unit_Struct_List_Struct* adaLimit = 0;
  Unit_Struct_List_Struct* adaUnit  = headNodes.Units;
  SgGlobal&                astScope = SG_DEREF(file->get_globalScope());

  traverse(adaUnit, adaLimit, UnitCreator(AstContext(astScope)));

  generateDOT(&astScope, "adaTypedAst");

  file->set_processedToIncludeCppDirectivesAndComments(false);
}



}
