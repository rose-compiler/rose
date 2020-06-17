#include "sage3basic.h"

#include <numeric>
#include <type_traits>
#include <sstream>

#include "Diagnostics.h"
#include "rose_config.h"
#include "sageGeneric.h"
#include "sageBuilder.h"
#include "Ada_to_ROSE_translation.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Ada_ROSE_Translation
{

namespace // local declarations
{
  const bool PRODUCTION_CODE = false;
  
  //
  // loggers
  Sawyer::Message::Facility adalogger;

  inline
  auto logTrace() -> decltype(adalogger[Sawyer::Message::TRACE])
  {
    return adalogger[Sawyer::Message::TRACE];
  }
  
  inline
  auto logInfo() -> decltype(adalogger[Sawyer::Message::INFO])
  {
    return adalogger[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(adalogger[Sawyer::Message::WARN])
  {
    return adalogger[Sawyer::Message::WARN];
  }

  inline
  auto logFatal() -> decltype(adalogger[Sawyer::Message::FATAL])
  {
    return adalogger[Sawyer::Message::FATAL];
  }

  void logInit() 
  {
    adalogger = Sawyer::Message::Facility("ADA-Frontend", Rose::Diagnostics::destination);
  }
  
#if 0
  inline
  std::ostream& logTrace()
  {
    return std::cerr << "[TRACE] ";
  }
  
  inline 
  std::ostream logInfo()
  {
    return std::cerr << "[INFO] ";
  }

  inline
  std::ostream& logWarn()
  {
    return std::cerr << "[WARN] ";
  }
  
  inline
  std::ostream& logFatal()
  {
    return std::cerr << "[FATAL] ";
  }
  
  void logInit() {}
#endif 

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

  template <class U, class V>
  U& as(V& obj)
  { 
    U& res = obj;
    
    return res;
  }

  //
  // declaration store and retrieval
  
  template <class KeyNode, class SageNode>
  using map_t = std::map<KeyNode, SageNode>;

  map_t<int, SgInitializedName*>      asisVars;  ///< stores a mapping from element id
                                                 ///  to constructed ROSE nodes.

  map_t<int, SgInitializedName*>      asisExcps; ///< stores a mapping from element id
                                                 ///  to an ADA exception decl. node.

  map_t<int, SgDeclarationStatement*> asisDecls; ///< stores a mapping from element id
                                                 ///  to constructed ROSE nodes.
                                                 ///  functions.. (anything else)

  map_t<int, SgDeclarationStatement*> asisUnits; ///< stores a mapping from unit id
                                                 ///  to constructed ROSE nodes.

  map_t<int, SgDeclarationStatement*> asisTypes; ///< stores a mapping from element id
                                                 ///  to constructed ROSE nodes.

  map_t<int, SgStatement*>            asisLoops; ///< stores a mapping from element id
                                                 ///  to constructed loops (needed for exit statements).

  map_t<std::string, SgType*>         adaTypes;  ///< stores a mapping from a name to an ADA standard type
                                                 ///  e.g., Integer.

  /// records a node (value) @ref val with key @ref key in map @ref m. 
  template <class KeyT, class DclT, class ValT>
  void
  recordNode(map_t<KeyT, DclT*>& m, KeyT key, ValT& val, bool unique = true)
  {
    ROSE_ASSERT(!unique || m.find(key) == m.end());

    //~ logInfo() << ">el# " << key << std::endl;
    m[key] = &val;
  }
  
  /// retrieves a node from map @ref m with key @ref key.
  template <class KeyT, class DclT>
  DclT&
  retrieveNode(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    typename map_t<KeyT, DclT*>::const_iterator pos = m.find(key);

    ROSE_ASSERT(pos != m.end());
    return *(pos->second);
  }
  
  /// retrieves a node from map @ref m with key @ref key, under the
  ///   assumption that it is of real type TgtT.
  template <class TgtT, class KeyT, class DclT>
  TgtT&
  retrieveNodeAs(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    DclT& node = retrieveNode(m, key);

    return SG_ASSERT_TYPE(TgtT, node);
  }
  
  /// retrieves a node from map @ref m with key @ref key, only if key is valid.
  /// returns the result of calling @ref alt otherwise (i.e., alt()).
  template <class KeyT, class DclT, class Alt>
  inline
  DclT&
  retrieveNodeIfAvail(const map_t<KeyT, DclT*>& m, KeyT key, Alt alt)
  {
    if (key == -1) return alt(); 
    
    return retrieveNode(m, key);
  }

  /// retrieves a node from map @ref m with key @ref key if key exists.
  /// returns nullptr otherwise.
  template <class KeyT, class DclT>
  DclT*
  findNode(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    typename map_t<KeyT, DclT*>::const_iterator pos = m.find(key);

    if (pos == m.end())
      return nullptr;

    return pos->second;
  }

  /// base case when a declaration is not in the map
  template <class KeyT, class DclT>
  DclT*
  findFirst(const map_t<KeyT, DclT*>&)
  {
    return nullptr;
  }

  /// tries a number of keys to find a declaration from map @ref m
  /// returns nullptr if none of the keys can be found.
  template <class KeyT, class DclT, class Key0T, class... KeysT>
  DclT*
  findFirst(const map_t<KeyT, DclT*>& m, Key0T key0, KeysT... keys)
  {
    DclT* dcl = findNode(m, key0);

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

    return nullptr;
  }

  template <class ElemT>
  ElemT&
  retrieveAs(ASIS_element_id_to_ASIS_MapType& map, int key)
  {
    return SG_DEREF(retrieveAsOpt<ElemT>(map, key));
  }


  //
  // Some ADA AST node maker functions
  //

  //
  // file info objects

  Sg_File_Info& mkFileInfo()
  {
    return SG_DEREF( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  }
  
  Sg_File_Info& mkFileInfo(const std::string& file, int line, int col)
  {
    Sg_File_Info& sgnode = SG_DEREF( new Sg_File_Info(file, line, col) );
    
    sgnode.setOutputInCodeGeneration(); 
    return sgnode;
  }
  
/*
  Sg_File_Info& mkFileInfo(const Sg_File_Info& orig)
  {
    return SG_DEREF(new Sg_File_Info(orig));
  }
  
  void copyFileInfo(const SgLocatedNode& src, SgLocatedNode& tgt)
  {
    tgt.set_file_info       (&mkFileInfo(SG_DEREF(src.get_file_info())));
    tgt.set_startOfConstruct(&mkFileInfo(SG_DEREF(src.get_startOfConstruct())));
    tgt.set_endOfConstruct  (&mkFileInfo(SG_DEREF(src.get_endOfConstruct())));
  }
*/

  void markCompilerGenerated(SgLocatedNode& n)
  {
    n.set_file_info       (&mkFileInfo());
    n.set_startOfConstruct(&mkFileInfo());
    n.set_endOfConstruct  (&mkFileInfo());
  }
  
  std::string
  canonicalString(const char* s) 
  { 
    // \todo string representations could be possibly shared..
    return std::string(s); 
  }
  
  void attachSourceLocation(SgLocatedNode& n, Element_Struct& elem)
  {
    Source_Location_Struct& loc  = elem.Source_Location;
    std::string             unit = canonicalString(loc.Unit_Name);
    
    //~ delete n.get_file_info();
    //~ delete n.get_startOfConstruct();
    //~ delete n.get_endOfConstruct();
    
    n.set_file_info       (&mkFileInfo(unit, loc.First_Line, loc.First_Column));
    n.set_startOfConstruct(&mkFileInfo(unit, loc.First_Line, loc.First_Column));
    n.set_endOfConstruct  (&mkFileInfo(unit, loc.Last_Line,  loc.Last_Column));
  }

  /// sets the symbols defining decl
  void linkDecls(SgFunctionSymbol& funcsy, SgFunctionDeclaration& func)
  {
    SgFunctionDeclaration& sdcl = SG_DEREF(funcsy.get_declaration());
    
    // defining and first non-defining must differ
    ROSE_ASSERT(&sdcl != &func);

    sdcl.set_definingDeclaration(&func);
    func.set_firstNondefiningDeclaration(&sdcl);
  }
  
  template <class SageParent, class SageChild>
  void linkParentChild(SageParent& parent, SageChild& child, void (SageParent::*setter)(SageChild*))
  {
    (parent.*setter)(&child);
    child.set_parent(&parent);
  }
  
  template <class SageNode, class ... Args>
  SageNode&
  mkBareNode(Args... args)
  {
    SageNode& sgnode = SG_DEREF(new SageNode(args...));

    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  template <class SageNode, class ... Args>
  SageNode&
  mkTypeNode(Args... args)
  {
    return SG_DEREF(SageNode::createType(args...));
  }
    

  //
  // Type Makers
  
  SgAdaRangeConstraint&
  mkAdaRangeConstraint(SgRangeExp& range) // \todo pass in range expressions
  {
    return SG_DEREF( new SgAdaRangeConstraint(&range) );
  }
  
  SgAdaSubtype&
  mkAdaSubtype(SgType& superty, SgAdaTypeConstraint& constr)
  {
    return mkTypeNode<SgAdaSubtype>(&superty, &constr);
  }
  
  SgAdaFloatType&
  mkAdaFloatType(SgExpression& digits, SgAdaRangeConstraint& range)
  {
    return mkTypeNode<SgAdaFloatType>(&digits, &range);
  }
  
  SgTypedefType&
  mkTypedefType(SgTypedefDeclaration& dcl)
  {
    return mkTypeNode<SgTypedefType>(&dcl);
  }
  
  SgDeclType&
  mkExceptionType(SgExpression& n)
  {
    return mkTypeNode<SgDeclType>(&n);
  }
  
  SgTypeTuple&
  mkTypeUnion(const std::vector<SgType*>& elemtypes)
  {
    SgTypeTuple&   sgnode = mkTypeNode<SgTypeTuple>();
    SgTypePtrList& lst    = sgnode.get_types();
    
    lst.insert(lst.end(), elemtypes.begin(), elemtypes.end());
    return sgnode;
  }
  
  SgClassType&
  mkRecordType(SgClassDeclaration& dcl)
  {
    return mkTypeNode<SgClassType>(&dcl);
  }
  
  SgAdaTaskType&
  mkAdaTaskType(SgAdaTaskTypeDecl& dcl)
  {
    SgAdaTaskType& sgnode = mkTypeNode<SgAdaTaskType>(&dcl);
    
    sgnode.set_decl(&dcl);
    return sgnode;
  }
  
  SgFunctionType& mkAdaEntryType(SgFunctionParameterList& lst)
  {
    // \todo build a real entry type
    return SG_DEREF(sb::buildFunctionType(sb::buildVoidType(), &lst));
  }
  
  //
  // Statement Makers

  SgStatement&
  mkRaiseStmt(SgExpression& raised)
  {
    SgExpression&    raiseop = SG_DEREF( sb::buildThrowOp(&raised, SgThrowOp::throw_expression ) );
    SgExprStatement& sgnode  = SG_DEREF( sb::buildExprStatement(&raiseop) );

    markCompilerGenerated(raiseop);
    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgBasicBlock&
  mkBasicBlock()
  {
    SgBasicBlock& sgnode = SG_DEREF( sb::buildBasicBlock() );

    markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgWhileStmt&
  mkWhileStmt(SgExpression& cond, SgBasicBlock& body)
  {
    SgWhileStmt& sgnode = SG_DEREF( sb::buildWhileStmt(&cond, &body) );

    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgAdaLoopStmt&
  mkLoopStmt(SgBasicBlock& body)
  {
    SgAdaLoopStmt&  sgnode = SG_DEREF( new SgAdaLoopStmt() ); 
    
    linkParentChild(sgnode, body, &SgAdaLoopStmt::set_body);

    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgForStatement&
  mkForStatement(SgBasicBlock& body)
  {
    SgStatement&    test   = SG_DEREF( sb::buildNullStatement() );
    SgExpression&   incr   = SG_DEREF( sb::buildNullExpression() );
    SgForStatement& sgnode = SG_DEREF( sb::buildForStatement(nullptr, &test, &incr, &body) ); 

    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgImportStatement&
  mkWithClause(const std::vector<SgExpression*>& imported)
  {
    SgImportStatement&   sgnode = SG_DEREF( new SgImportStatement(&mkFileInfo()) );
    SgExpressionPtrList& lst    = sgnode.get_import_list();

    lst.insert(lst.end(), imported.begin(), imported.end());

    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgAdaExitStmt&
  mkAdaExitStmt(SgStatement& loop, SgExpression& condition, bool explicitLoopName)
  {
    SgAdaExitStmt& sgnode = SG_DEREF(new SgAdaExitStmt(&loop, &condition, explicitLoopName));

    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgSwitchStatement&
  mkAdaCaseStmt(SgExpression& selector, SgBasicBlock& body)
  {
    // \todo
    // maybe it would be good to have a separate case statement for ADA
    //   as ADA is a bit more restrictive in its switch case syntax compared to C++    
    SgSwitchStatement& sgnode = SG_DEREF(sb::buildSwitchStatement(&selector, &body));
    
    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgLabelStatement&
  mkLabelStmt(std::string n, SgStatement& stmt, SgScopeStatement& encl)
  {
    SgLabelStatement& sgnode = SG_DEREF( sb::buildLabelStatement(n, &stmt, &encl) );

    linkParentChild(sgnode, stmt, &SgLabelStatement::set_statement);
    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }

  SgStatement&
  mkNullStmt()
  {
    SgStatement& sgnode = SG_DEREF(sb::buildNullStatement());

    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgTryStmt&
  mkTryStmt(SgBasicBlock& blk)
  {
    SgTryStmt& sgnode = SG_DEREF(sb::buildTryStmt(&blk));

    markCompilerGenerated(SG_DEREF(sgnode.get_catch_statement_seq_root()));
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  

  //
  // Declaration Makers
  
  SgTypedefDeclaration&
  mkTypeDecl(const std::string& name, SgType& ty, SgScopeStatement& scope)
  {
    SgTypedefDeclaration& sgnode = SG_DEREF( sb::buildTypedefDeclaration(name, &ty, &scope) );

    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgClassDeclaration& 
  mkRecordDecl(const std::string& name, SgClassDefinition& def, SgScopeStatement& scope)
  {
    SgClassDeclaration& nondef = SG_DEREF( sb::buildNondefiningClassDeclaration_nfi( name,   
                                                                                     SgClassDeclaration::e_struct,
                                                                                     &scope,
                                                                                     false /* template instance */,
                                                                                     nullptr /* template parameter list */ 
                                                                                   ));

    SgClassDeclaration& sgnode = SG_DEREF( sb::buildNondefiningClassDeclaration_nfi( name,   
                                                                                     SgClassDeclaration::e_struct,
                                                                                     &scope,
                                                                                     false /* template instance */,
                                                                                     nullptr /* template parameter list */ 
                                                                                   ));
    
    linkParentChild(sgnode, def, &SgClassDeclaration::set_definition);
    sgnode.unsetForward();
    sgnode.set_definingDeclaration(&sgnode);
    nondef.set_definingDeclaration(&sgnode);
    sgnode.set_firstNondefiningDeclaration(&nondef);
    nondef.set_firstNondefiningDeclaration(&nondef);
    return sgnode;
  }

  SgAdaPackageSpecDecl&
  mkAdaPackageSpecDecl(std::string name, SgScopeStatement& scope)
  {
    SgAdaPackageSpec&     pkgdef = mkBareNode<SgAdaPackageSpec>();
    SgAdaPackageSpecDecl& sgnode = SG_DEREF( new SgAdaPackageSpecDecl(name, nullptr) );

    sgnode.set_parent(&scope);
    sgnode.set_firstNondefiningDeclaration(&sgnode);
    
    linkParentChild(sgnode, pkgdef, &SgAdaPackageSpecDecl::set_definition);
    
    // add the symbol to the table
    ROSE_ASSERT(!scope.symbol_exists(name));
    scope.insert_symbol(name, new SgAdaPackageSymbol(&sgnode));
    
    /*
    // set if first definition
    if (nsdef.get_global_definition() == nullptr)
    {
      SgNamespaceDefinitionStatement&  globdef = SG_DEREF(sb::buildNamespaceDefinition(&nsdcl));

      globdef.set_parent(&scope);
      globdef.set_global_definition(&globdef);
      nsdef.set_global_definition(&globdef);
    }
    */

    markCompilerGenerated(pkgdef);
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgAdaPackageBodyDecl&
  mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl, SgScopeStatement& scope)
  {
    SgAdaPackageBody&     pkgbody = SG_DEREF( new SgAdaPackageBody() );
    SgAdaPackageBodyDecl& sgnode  = SG_DEREF( new SgAdaPackageBodyDecl(specdcl.get_name(), &pkgbody) );

    pkgbody.set_parent(&sgnode);
    sgnode.set_parent(&scope);
    
    SgAdaPackageSpec&     pkgspec = SG_DEREF( specdcl.get_definition() );
    
    pkgspec.set_body(&pkgbody);
    pkgbody.set_spec(&pkgspec);
    
    // \todo make sure assertion holds
    // ROSE_ASSERT(scope.symbol_exists(specdcl.get_name()));
/*
    if (!scope.symbol_exists(specdcl.get_name()))
      scope.insert_symbol(specdcl.get_name(), new SgAdaPackageSymbol(&sgnode));
*/
    
/*
    nsdcl.set_parent(&scope);
    linkParentChild(nsdcl, nsdef, &SgNamespaceDeclarationStatement::set_definition);
    
    // set if first definition
    if (nsdef.get_global_definition() == nullptr)
    {
      SgNamespaceDefinitionStatement&  globdef = SG_DEREF(sb::buildNamespaceDefinition(&nsdcl));

      globdef.set_parent(&scope);
      globdef.set_global_definition(&globdef);
      nsdef.set_global_definition(&globdef);
    }
*/
    markCompilerGenerated(pkgbody);
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgAdaTaskTypeDecl&
  mkAdaTaskTypeDecl(std::string name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
  {
    SgAdaTaskTypeDecl& sgnode = SG_DEREF(new SgAdaTaskTypeDecl(name, &spec));
    
    scope.insert_symbol(name, new SgAdaTaskSymbol(&sgnode));
    
    spec.set_parent(&sgnode);
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  struct TaskDeclInfo
  {
    std::string    name;
    SgAdaTaskSpec* spec;
  };
  
  struct ExtractTaskDeclinfo : sg::DispatchHandler<TaskDeclInfo>
  {
    template <class SageTaskDecl>
    void handleTaskDecl(SageTaskDecl& n)
    {
      res.name = n.get_name();
      res.spec = n.get_definition();
    }
    
    void handle(SgNode& n)            { SG_UNEXPECTED_NODE(n); }
    void handle(SgAdaTaskSpecDecl& n) { handleTaskDecl(n); }
    void handle(SgAdaTaskTypeDecl& n) { handleTaskDecl(n); }
  };
  
  
  SgAdaTaskBodyDecl&
  mkAdaTaskBodyDecl(SgDeclarationStatement& tskdecl, SgAdaTaskBody& tskbody, SgScopeStatement& scope)
  {
    //~ SgAdaPackageBody&     pkgbody = SG_DEREF( new SgAdaPackageBody() );
    TaskDeclInfo       specinfo = sg::dispatch(ExtractTaskDeclinfo(), &tskdecl);
    SgAdaTaskBodyDecl& sgnode   = SG_DEREF( new SgAdaTaskBodyDecl(specinfo.name, &tskbody) );

    tskbody.set_parent(&sgnode);
    sgnode.set_parent(&scope);
    
    SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );
    
    tskspec.set_body(&tskbody);
    tskbody.set_spec(&tskspec);
    
    ROSE_ASSERT(scope.symbol_exists(specinfo.name));
    return sgnode;
  }
  
  SgAdaTaskBodyDecl&
  mkAdaTaskBodyDecl(std::string name, SgAdaTaskBody& tskbody, SgScopeStatement& scope)
  {
    //~ SgAdaPackageBody&     pkgbody = SG_DEREF( new SgAdaPackageBody() );
    SgAdaTaskBodyDecl& sgnode   = SG_DEREF( new SgAdaTaskBodyDecl(name, &tskbody) );

    tskbody.set_parent(&sgnode);
    sgnode.set_parent(&scope);
    
    /*
    SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );
    
    tskspec.set_body(&tskbody);
    tskbody.set_spec(&tskspec);
    */
    
    //~ ROSE_ASSERT(scope.symbol_exists(specinfo.name));
    scope.insert_symbol(name, new SgAdaTaskSymbol(&sgnode));
    return sgnode;
  }
  
  SgAdaTaskSpec&
  mkAdaTaskSpec() { return mkBareNode<SgAdaTaskSpec>(); }
  
  SgAdaTaskBody&
  mkAdaTaskBody() { return mkBareNode<SgAdaTaskBody>(); }
  
  typedef SgScopeStatement& (*ProcScopeMaker)(SgFunctionDeclaration&);
  
  SgScopeStatement&
  mkProcDef(SgFunctionDeclaration& dcl)
  {
    SgFunctionDefinition& sgnode = SG_DEREF(new SgFunctionDefinition(&dcl, nullptr));
    SgBasicBlock&         body   = mkBasicBlock();

    linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_definition);
    linkParentChild(sgnode, body, &SgFunctionDefinition::set_body);
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgScopeStatement&
  mkProcDecl(SgFunctionDeclaration& dcl)
  {
    SgFunctionParameterScope& sgnode = SG_DEREF(new SgFunctionParameterScope(&mkFileInfo()));
    
    linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_functionParameterScope);
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgFunctionParameterList&
  mkFunctionParameterList()
  {
    SgFunctionParameterList& sgnode = SG_DEREF(sb::buildFunctionParameterList());
    
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  template <class ParamCompletion>
  SgFunctionDeclaration&
  mkProcedure(std::string nm, SgScopeStatement& scope, SgType& retty, ParamCompletion complete, ProcScopeMaker scopeMaker = mkProcDecl)
  {
    SgFunctionParameterList& lst       = mkFunctionParameterList();
    SgFunctionDeclaration&   sgnode    = SG_DEREF(sb::buildNondefiningFunctionDeclaration(nm, &retty, &lst, &scope, nullptr));
    SgScopeStatement&        parmScope = scopeMaker(sgnode); 

    complete(lst, parmScope);
    ROSE_ASSERT(sgnode.get_type() != nullptr);     
    
    markCompilerGenerated(sgnode);
    return sgnode;
  }

  template <class ParamCompletion>
  SgFunctionDeclaration&
  mkProcedureDef(SgFunctionDeclaration& ndef, SgScopeStatement& scope, SgType& retty, ParamCompletion complete)
  {
    SgFunctionDeclaration& sgnode = mkProcedure(ndef.get_name(), scope, retty, complete, mkProcDef);
    SgSymbol*              baseSy = sgnode.search_for_symbol_from_symbol_table();
    SgFunctionSymbol&      funcSy = *SG_ASSERT_TYPE(SgFunctionSymbol, baseSy);

    linkDecls(funcSy, sgnode);
    sgnode.set_definingDeclaration(&sgnode);
    sgnode.unsetForward();

    return sgnode;
  }

  template <class ParamCompletion>
  SgFunctionDeclaration&
  mkProcedureDef(std::string nm, SgScopeStatement& scope, SgType& retty, ParamCompletion complete)
  {
    SgFunctionDeclaration& ndef   = mkProcedure(nm, scope, retty, complete);
    
    return mkProcedureDef(ndef, scope, retty, complete);
  }
  
  
  template <class ParamCompletion>
  SgAdaEntryDecl&
  mkAdaEntryDecl(std::string name, SgScopeStatement& scope, ParamCompletion complete) 
  {
    //~ SgFunctionParameterList&  lst    = mkFunctionParameterList();
    SgAdaEntryDecl&           sgnode = SG_DEREF(new SgAdaEntryDecl(name, NULL /* entry type */, NULL /* definition */));
    SgFunctionParameterList&  lst    = SG_DEREF(sgnode.get_parameterList());
    SgFunctionParameterScope& psc    = SG_DEREF(new SgFunctionParameterScope(&mkFileInfo()));
    
    ROSE_ASSERT(sgnode.get_functionParameterScope() == nullptr);
    linkParentChild<SgFunctionDeclaration>(sgnode, psc, &SgFunctionDeclaration::set_functionParameterScope);
    
    complete(lst, psc);
    
    SgFunctionType&           funty  = mkAdaEntryType(lst);
    
    sgnode.set_type(&funty);
    
    //~ ROSE_ASSERT(sgnode.get_parameterList() == nullptr);
    //~ linkParentChild<SgFunctionDeclaration>(sgnode, lst, &SgFunctionDeclaration::set_parameterList);

    // not used 
    ROSE_ASSERT(sgnode.get_parameterList_syntax() == nullptr);
    
    //~ SgFunctionSymbol*         funsy  = scope.find_symbol_by_type_of_function<SgAdaEntryDecl>(name, &funty, NULL, NULL);
    SgFunctionSymbol*         funsy  = scope.find_symbol_by_type_of_function<SgFunctionDeclaration>(name, &funty, NULL, NULL);
    
    ROSE_ASSERT(funsy == nullptr); 
    funsy = new SgFunctionSymbol(&sgnode);
    
    scope.insert_symbol(name, funsy);
    sgnode.set_scope(&scope);
    sgnode.set_definingDeclaration(&sgnode);
    sgnode.unsetForward();
    
    markCompilerGenerated(psc);
    markCompilerGenerated(lst);
    markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgAdaAcceptStmt&
  mkAdaAcceptStmt(SgExpression& ref, SgExpression& idx)
  {
    SgAdaAcceptStmt&          sgnode = mkBareNode<SgAdaAcceptStmt>();
    SgFunctionParameterScope& psc    = SG_DEREF(new SgFunctionParameterScope(&mkFileInfo()));
    SgFunctionParameterList&  lst    = mkFunctionParameterList();
    
    ROSE_ASSERT(sgnode.get_parameterScope() == nullptr);
    linkParentChild(sgnode, psc, &SgAdaAcceptStmt::set_parameterScope);
    
    ROSE_ASSERT(sgnode.get_parameterList() == nullptr);
    linkParentChild(sgnode, lst, &SgAdaAcceptStmt::set_parameterList);
    
    linkParentChild(sgnode, ref, &SgAdaAcceptStmt::set_entry);
    linkParentChild(sgnode, idx, &SgAdaAcceptStmt::set_index);
    
    markCompilerGenerated(psc);
    return sgnode;
  }
  
  SgVariableDeclaration&
  mkVarDecl(SgInitializedName& var, SgScopeStatement& scope);
  
  SgCatchOptionStmt&
  mkExceptionHandler(SgInitializedName& parm, SgBasicBlock& body)
  {
    SgCatchOptionStmt&     sgnode = SG_DEREF( sb::buildCatchOptionStmt(nullptr, &body) );
    SgVariableDeclaration& exparm = mkVarDecl(parm, sgnode);
    
    linkParentChild(sgnode, exparm, &SgCatchOptionStmt::set_condition);
    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgInitializedName&
  mkInitializedName(const std::string& varname, SgType& vartype, SgExpression* varexpr)
  {
    SgAssignInitializer* varinit = varexpr ? sb::buildAssignInitializer(varexpr) : nullptr;
    SgInitializedName&   sgnode = SG_DEREF( sb::buildInitializedName(varname, &vartype, varinit) );
    
    //~ sgnode.set_type(&vartype);

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
    SgVariableDeclaration&    parmDecl = SG_DEREF( new SgVariableDeclaration(&mkFileInfo()) );
    SgInitializedNamePtrList& names    = parmDecl.get_variables();
    SgDeclarationModifier&    declMods = parmDecl.get_declarationModifier();

    // insert initialized names and set the proper declaration node
    std::for_each( parms.begin(), parms.end(),
                   [&parmDecl, &names](SgInitializedName* prm)->void
                   {
                     prm->set_definition(&parmDecl);
                     names.push_back(prm);
                   }
                 );
    declMods.get_typeModifier() = parmmode;

    markCompilerGenerated(parmDecl);
    si::fixVariableDeclaration(&parmDecl, &scope);
    parmDecl.set_parent(&scope);
    
    ROSE_ASSERT(parmDecl.get_definingDeclaration() == nullptr);
    ROSE_ASSERT(parmDecl.get_firstNondefiningDeclaration() == nullptr);
    
    parmDecl.set_firstNondefiningDeclaration(&parmDecl);
    return parmDecl;
  }

  template <class Const_iterator>
  SgVariableDeclaration&
  mkVarDecl(Const_iterator aa, Const_iterator zz, SgScopeStatement& scope)
  {
    SgVariableDeclaration&    vardcl = SG_DEREF( new SgVariableDeclaration(&mkFileInfo()) );
    SgInitializedNamePtrList& names  = vardcl.get_variables();

    names.insert(names.end(), aa, zz);

    markCompilerGenerated(vardcl);
    si::fixVariableDeclaration(&vardcl, &scope);
    vardcl.set_parent(&scope);
    
    ROSE_ASSERT(vardcl.get_definingDeclaration() == nullptr);
    ROSE_ASSERT(vardcl.get_firstNondefiningDeclaration() == nullptr);
    vardcl.set_firstNondefiningDeclaration(&vardcl);
    
    return vardcl;
  }

  SgVariableDeclaration&
  mkVarDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope)
  {
    return mkVarDecl(vars.begin(), vars.end(), scope);
  }
  
  SgVariableDeclaration&
  mkVarDecl(SgInitializedName& var, SgScopeStatement& scope)
  {
    SgInitializedName*  alias = &var;
    SgInitializedName** aa    = &alias;
     
    return mkVarDecl(aa, aa+1, scope);
  }

  SgVariableDeclaration&
  mkExceptionDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope)
  {
    // \todo
    // currently, exceptions are variables with type "Exception"
    //   revise after ROSE representation of ADA exceptions is revised
    SgVariableDeclaration&    vardcl = SG_DEREF( new SgVariableDeclaration(&mkFileInfo()) );
    SgInitializedNamePtrList& names  = vardcl.get_variables();

    names.insert(names.end(), vars.begin(), vars.end());

    markCompilerGenerated(vardcl);
    si::fixVariableDeclaration(&vardcl, &scope);
    vardcl.set_parent(&scope);
    return vardcl;
  }
  
  SgBaseClass&
  mkRecordParent(SgClassDeclaration& n)
  {
    if (!n.get_definingDeclaration())
    {
      logWarn() << "no defining declaration for base class: " << n.get_name() 
                << std::endl;
    }
    
    return SG_DEREF(new SgBaseClass(&n, true /* direct base */));
  }
  
  
  //
  // Expression Makers
  
  SgRangeExp&
  mkRangeExp(SgExpression& start, SgExpression& end)
  {
    SgRangeExp& sgnode = SG_DEREF( sb::buildRangeExp(&start) );
    
    sgnode.set_end(&end);
    
    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgRangeExp&
  mkRangeExp()
  {
    SgExpression& start  = SG_DEREF( sb::buildNullExpression() );
    SgExpression& end    = SG_DEREF( sb::buildNullExpression() );
    
    return mkRangeExp(start, end);
  }
  
  SgExpression&
  mkOthersExp()
  {
    return SG_DEREF(sb::buildNullExpression());
  }

  SgExpression&
  mkExceptionRef(SgInitializedName& exception, SgScopeStatement& scope)
  {
    // \todo
    // replace and refer to referenced declaration
    SgExpression& sgnode = SG_DEREF( sb::buildVarRefExp(&exception, &scope) );

    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  

  SgExpression&
  mkSelectedComponent(SgExpression& prefix, SgExpression& selector)
  {
    SgExpression& sgnode = SG_DEREF( sb::buildDotExp(&prefix, &selector) );

    //~ markCompilerGenerated(sgnode);
    return sgnode;
  }
  
  SgRemOp* 
  buildRemOp(SgExpression* lhs, SgExpression* rhs)
  {
    return &mkBareNode<SgRemOp>(lhs, rhs, nullptr);
  }
  
  SgAbsOp* 
  buildAbsOp(SgExpression* op)
  {
    return &mkBareNode<SgAbsOp>(op, nullptr);
  }
  
  template <class SageValue>
  SageValue& mkValue(const char* img)
  {
    typedef decltype(std::declval<SageValue>().get_value()) rose_rep_t;
    
    SageValue& sgnode = SG_DEREF(new SageValue(conv<rose_rep_t>(img), img));
    
    markCompilerGenerated(sgnode);
    return sgnode;
  }



  //
  // auxiliary classes and functions

  /// \brief resolves all goto statements to labels
  ///        at the end of procedures or functions.
  struct LabelManager
  {
      LabelManager() = default;
      
      /// patch gotos with target (a label statement)
      ///   at the end of a procudure / function.
      ~LabelManager()
      {
        for (GotoContainer::value_type el : gotos)
        {
          el.first->set_label(&retrieveNode(labels, el.second));
        }
      }
      
      /// records a new labeled statement @ref lblstmt with key @ref id.
      void label(Element_ID id, SgLabelStatement& lblstmt)
      {
        SgLabelStatement*& mapped = labels[id];
         
        ROSE_ASSERT(mapped == nullptr);
        mapped = &lblstmt;
      }
      
      /// records a new goto statement @ref gotostmt with label key @ref id.
      void gotojmp(Element_ID id, SgGotoStatement& gotostmt)
      {
        gotos.emplace_back(&gotostmt, id);
      }
    
    private:
      typedef std::map<Element_ID, SgLabelStatement*>               LabelContainer;
      typedef std::vector<std::pair<SgGotoStatement*, Element_ID> > GotoContainer;
          
      LabelContainer labels;
      GotoContainer  gotos;         
    
      LabelManager(const LabelManager&)            = delete;
      LabelManager(LabelManager&&)                 = delete;
      LabelManager& operator=(const LabelManager&) = delete;
      LabelManager& operator=(LabelManager&&)      = delete;
  };


  struct AstContext
  {
      explicit
      AstContext(SgScopeStatement& s)
      : the_scope(&s), all_labels(nullptr)
      {}
  
      SgScopeStatement& scope()  const { return *the_scope; }
      LabelManager&     labels() const { return SG_DEREF(all_labels); }
  
      // no-parent-check: sets scope without parent check
      //   e.g., when the parent node is built after the fact (e.g., if statements)
      AstContext scope_npc(SgScopeStatement& s) const
      {
        AstContext tmp(*this);
  
        tmp.the_scope = &s;
        return tmp;
      }
  
      AstContext scope(SgScopeStatement& s) const
      {
        ROSE_ASSERT(s.get_parent());
        
        return scope_npc(s);
      }
      
      AstContext labels(LabelManager& lm) const
      {
        AstContext tmp(*this);
  
        tmp.all_labels = &lm;
        return tmp;
      }
    
    private:
      SgScopeStatement* the_scope;
      LabelManager*     all_labels;
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

  void handleElement(Element_Struct& elem, AstContext ctx, bool isPrivate = false);
  void handleStmt(Element_Struct& elem, AstContext ctx);
  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx);
  
  void handleElementID(Element_ID id, AstContext ctx)
  {
    handleElement(retrieveAs<Element_Struct>(asisMap, id), ctx);
  }

  /// if @ref isPrivate @ref dcl's accessibility is set to private;
  /// otherwise nothing.
  void
  privatize(SgDeclarationStatement& dcl, bool isPrivate)
  {
    if (!isPrivate) return;
     
    dcl.get_declarationModifier().get_accessModifier().setPrivate();
  }


  struct ElemCreator
  {
      explicit
      ElemCreator(AstContext astctx, bool privateItems = false)
      : ctx(astctx), privateElems(privateItems)
      {}

      void operator()(Element_Struct& elem)
      {
        handleElement(elem, ctx, privateElems);
      }
      
    private:
      AstContext ctx;
      bool       privateElems;
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
  
  struct ExHandlerCreator
  {
      ExHandlerCreator(AstContext astctx, SgTryStmt& tryStatement)
      : ctx(astctx), tryStmt(tryStatement)
      {}
    
      void operator()(Element_Struct& elem)
      {
        handleExceptionHandler(elem, tryStmt, ctx);
      }
    
      AstContext ctx;
      SgTryStmt& tryStmt;
  };
  
  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx);

  SgNode&
  getExprType(Expression_Struct& elem, AstContext ctx);
  
  struct HandlerTypeCreator
  {
      HandlerTypeCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        ROSE_ASSERT(elem.Element_Kind == An_Expression);
        
        Expression_Struct& asisexpr  = elem.The_Union.Expression; 
        SgInitializedName* exception = isSgInitializedName(&getExprType(asisexpr, ctx));
        SgExpression&      exref     = mkExceptionRef(SG_DEREF(exception), ctx.scope());
        SgType&            extype    = mkExceptionType(exref);
        
        lst.push_back(&extype);
      }
        
      operator SgType&() const
      {
        ROSE_ASSERT(lst.size() > 0);
        
        if (lst.size() == 1) 
          return SG_DEREF(lst[0]);
        
        return mkTypeUnion(lst);
      } 
      
      AstContext           ctx;
      std::vector<SgType*> lst;
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
      
      operator std::vector<SgExpression*> () &&
      {
        return std::move(args);
      }

    private:
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
  
  SgAdaRangeConstraint&
  getRangeConstraint(Element_ID el, AstContext ctx);
  
  //
  // Creator classes

  struct ParmlistCreator
  {
      ParmlistCreator(SgFunctionParameterList& parms, AstContext astctx)
      : parmlist(parms), ctx(astctx)
      {}
    
      // CallableDeclaration is either derived from function declaration, 
      // or an accept statement.
      template <class CallableDeclaration>
      ParmlistCreator(CallableDeclaration& callable, AstContext astctx)
      : ParmlistCreator(SG_DEREF(callable.get_parameterList()), astctx)
      {}
      
      void operator()(Element_Struct& elem)
      {
        SgVariableDeclaration&    decl = getParm(elem, ctx);
        SgInitializedNamePtrList& args = parmlist.get_args();
        
        for (SgInitializedName* parm : decl.get_variables())
        {
          parm->set_parent(&parmlist);
          args.push_back(parm);
        }
      }
      
    private:
      SgFunctionParameterList& parmlist;
      AstContext               ctx;
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

    private:
      AstContext       ctx;
      result_container names;
  };
  
  SgRangeExp& 
  getDiscreteRange(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Discrete_Range);
    
    SgRangeExp*            res = nullptr;
    Discrete_Range_Struct& range = def.The_Union.The_Discrete_Range; 
    
    switch (range.Discrete_Range_Kind)
    {
      case A_Discrete_Simple_Expression_Range:    // 3.6.1, 3.5
        {
          SgExpression& lb = getExprID(range.Lower_Bound, ctx);
          SgExpression& ub = getExprID(range.Upper_Bound, ctx);
          
          res = &mkRangeExp(lb, ub);
          break;
        }

      case A_Discrete_Subtype_Indication:         // 3.6.1(6), 3.2.2
      case A_Discrete_Range_Attribute_Reference:  // 3.6.1, 3.5
      case Not_A_Discrete_Range:                  // An unexpected element
      default: 
        logWarn() << "Unhandled range: " << range.Discrete_Range_Kind << std::endl;
        res = &mkRangeExp();
        ROSE_ASSERT(!PRODUCTION_CODE);  
    }
    
    return SG_DEREF(res);
  }
  
  SgExpression& 
  getDefinitionExpr(Definition_Struct& def, AstContext ctx)
  {
    SgExpression* res = nullptr;
    
    switch (def.Definition_Kind)
    {
      case A_Discrete_Range:
        res = &getDiscreteRange(def, ctx);
        break; 
        
      case An_Others_Choice:
        res = &mkOthersExp();
        break;
      
      default:
        logWarn() << "Unhandled definition: " << def.Definition_Kind << std::endl;
        res = sb::buildNullExpression();
        ROSE_ASSERT(!PRODUCTION_CODE);  
    }
    
    return SG_DEREF(res);
  }

  struct ExprListCreator
  {
      explicit
      ExprListCreator(AstContext astctx)
      : ctx(astctx), elems()
      {}

      void operator()(Element_Struct& el)
      {
        SgExpression* res = nullptr;
        
        if (el.Element_Kind == An_Expression)
          res = &getExpr(el, ctx);
        else if (el.Element_Kind == A_Definition)
          res = &getDefinitionExpr(el.The_Union.Definition, ctx);

        ROSE_ASSERT(res);
        elems.push_back(res);
      }

#if __cplusplus < 201103L
      operator std::vector<SgExpression*> () const
      {
        return elems;
      }
#else      
      operator std::vector<SgExpression*> () &&
      {
        return std::move(elems);
      }
#endif /* C++11 */

    private:
      AstContext                 ctx;
      std::vector<SgExpression*> elems;

      ExprListCreator() = delete;
  };
  
  
  struct IfStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      explicit
      IfStmtCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void commonBranch(Path_Struct& path, SgExpression* cond = nullptr)
      {
        SgBasicBlock& block     = mkBasicBlock();
        ElemIdRange   thenStmts = idRange(path.Sequence_Of_Statements);

        traverseIDs(thenStmts, asisMap, StmtCreator(ctx.scope_npc(block)));
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
        SgStatement* elseStmt = nullptr;

        if (!branches.back().first)
        {
          elseStmt = branches.back().second;
          branches.pop_back();
        }

        return SG_DEREF( std::accumulate(branches.rbegin(), branches.rend(), elseStmt, createIfStmt) );
      }
      
    private:
      AstContext       ctx;
      branch_container branches;
  };
  
  struct CaseStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      CaseStmtCreator(AstContext astctx, SgSwitchStatement& caseStmt)
      : ctx(astctx), caseNode(caseStmt)
      {}

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case A_Case_Path:
            {
              ElemIdRange                caseChoices = idRange(path.Case_Path_Alternative_Choices);
              std::vector<SgExpression*> choices     = traverseIDs(caseChoices, asisMap, ExprListCreator(ctx));
              ElemIdRange                caseBlock   = idRange(path.Sequence_Of_Statements);
              
              // \todo reconsider the "reuse" of SgCommaOp
              //   SgCommaOp is only used to separate discrete choices in case-when
              ROSE_ASSERT(choices.size());
              SgExpression&              caseCond    = SG_DEREF( std::accumulate( choices.begin()+1, choices.end(), 
                                                                                  choices.front(), 
                                                                                  sb::buildCommaOpExp
                                                                                ));
              
              SgBasicBlock&              block       = mkBasicBlock();
              SgCaseOptionStmt*          sgnode      = sb::buildCaseOptionStmt(&caseCond, &block);
              
              attachSourceLocation(SG_DEREF(sgnode), elem); 
              sgnode->set_has_fall_through(false);
              caseNode.append_case(sgnode);
              traverseIDs(caseBlock, asisMap, StmtCreator(ctx.scope(block)));
              break;
            }

          default:
            ROSE_ASSERT(false);
        }
      }

    private:
      AstContext         ctx;
      SgSwitchStatement& caseNode; // \todo change to SgAdaCaseStmt
  };
  


  //
  // some(?) functions
  
  struct MakeTyperef : sg::DispatchHandler<SgType*>
  {
    typedef sg::DispatchHandler<SgType*> base;
    
    MakeTyperef()
    : base()
    {}
    
    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }
    
    void handle(SgType& n)               { res = &n; }
    void handle(SgTypedefDeclaration& n) { res = &mkTypedefType(n); }
    void handle(SgClassDeclaration& n)   { res = &mkRecordType(n); }
    void handle(SgAdaTaskTypeDecl& n)    { res = &mkAdaTaskType(n); }
  };
  
  
  SgNode&
  getExprType(Expression_Struct& typeEx, AstContext ctx)
  {
    SgNode* res = nullptr;
    
    switch (typeEx.Expression_Kind)
    {
      case An_Identifier:
        {
          // is it a type?
          // typeEx.Corresponding_Name_Declaration ?
          res = findFirst(asisTypes, typeEx.Corresponding_Name_Definition);
          
          if (!res)
          {
            // is it an exception?
            // typeEx.Corresponding_Name_Declaration ?
            res = findFirst(asisExcps, typeEx.Corresponding_Name_Definition);
          }
          
          if (!res)
          {
            // is it a predefined Ada type?
            res = findFirst(adaTypes, std::string(typeEx.Name_Image));
          }
          
          if (!res)
          {
            // what is it?
            logWarn() << "unknown type name: " << typeEx.Name_Image << std::endl;
          
            ROSE_ASSERT(!PRODUCTION_CODE);
            res = sb::buildVoidType();
          }
          
          break /* counted in getExpr */;
        }
        
      case A_Selected_Component:
        {
          res = &getExprTypeID(typeEx.Selector, ctx);
          break /* counted in getExpr */;
        }
      
      default:
        logWarn() << "Unknown type expression: " << typeEx.Expression_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
        res = sb::buildVoidType();
    }

    return SG_DEREF(res);
  }
  
  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(asisMap, tyid);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);
    
    return getExprType(elem.The_Union.Expression, ctx);
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
          logWarn() << "ak: " << access.Access_Definition_Kind << std::endl;
          break;
        }

      case Not_An_Access_Definition: /* break; */ // An unexpected element
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
    {
      SgNode& basenode = getExprType(elem.The_Union.Expression, ctx);
      SgType* res      = sg::dispatch(MakeTyperef(), &basenode);
          
      return SG_DEREF(res);
    }

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
  getDefinitionTypeID(Element_ID defid, AstContext ctx);
  
  SgType&
  getDefinitionType(Definition_Struct& def, AstContext ctx)
  {
    SgType* res = nullptr;
    
    switch (def.Definition_Kind)
    {
      case A_Subtype_Indication:
        {
          Subtype_Indication_Struct& subtype   = def.The_Union.The_Subtype_Indication;
          
          res = &getDeclTypeID(subtype.Subtype_Mark, ctx);
          
          // \todo if there is no subtype constraint, shall we produce
          //       a subtype w/ NoConstraint, or leave the original type?
          if (subtype.Subtype_Constraint)
          {
            SgAdaTypeConstraint& range = getRangeConstraint(subtype.Subtype_Constraint, ctx);
            
            res = &mkAdaSubtype(SG_DEREF(res), range);
          }
          
          /* unused fields:
                bool       Has_Null_Exclusion;
          */
          break;
        }
        
      case A_Component_Definition:
        {
          Component_Definition_Struct& component = def.The_Union.The_Component_Definition;
          
          res = &getDefinitionTypeID(component.Component_Definition_View, ctx);
          break;
        }
      
      default:
        logWarn() << "Unhandled type definition: " << def.Definition_Kind << std::endl;
        res = sb::buildVoidType();
        ROSE_ASSERT(!PRODUCTION_CODE);  
    }
    
    return SG_DEREF(res);
  }
  
  SgType&
  getDefinitionTypeID(Element_ID defid, AstContext ctx)
  {
    Element_Struct&     elem = retrieveAs<Element_Struct>(asisMap, defid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);
    
    return getDefinitionType(elem.The_Union.Definition, ctx);
  }
  
  SgClassDeclaration&
  getParentRecordDecl(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Subtype_Indication);
    
    Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;
    ROSE_ASSERT (subtype.Subtype_Constraint == 0);
    
    Element_Struct&            subelem = retrieveAs<Element_Struct>(asisMap, subtype.Subtype_Mark);
    ROSE_ASSERT(subelem.Element_Kind == An_Expression);
    
    SgNode*                    basenode = &getExprType(subelem.The_Union.Expression, ctx);
    SgClassDeclaration*        res = isSgClassDeclaration(basenode);
    
    return SG_DEREF(res);
  }

  SgClassDeclaration&
  getParentRecordDeclID(Element_ID defid, AstContext ctx)
  {
    Element_Struct&     elem = retrieveAs<Element_Struct>(asisMap, defid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);
    
    return getParentRecordDecl(elem.The_Union.Definition, ctx);
  }

  SgType&
  getVarType(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Component_Declaration
               );

    return getDefinitionTypeID(decl.Object_Declaration_View, ctx);
  }
  
  
  SgClassDefinition& 
  getRecordBody(Record_Definition_Struct& rec, AstContext ctx)
  {
    SgClassDefinition&        sgnode = SG_DEREF( sb::buildClassDefinition() );
    
    ElemIdRange               components = idRange(rec.Record_Components);
    //~ ElemIdRange               implicits  = idRange(rec.Implicit_Components);
    
    traverseIDs(components, asisMap, ElemCreator(ctx.scope_npc(sgnode)));

    // how to represent implicit components
    //~ traverseIDs(implicits, asisMap, ElemCreator(ctx.scope_npc(sgnode)));
    
    /* unused nodes:
         Record_Component_List Implicit_Components
    */    
    return sgnode;
  }
  
  SgClassDefinition& 
  getRecordBodyID(Element_ID recid, AstContext ctx)
  {
    Element_Struct&           elem = retrieveAs<Element_Struct>(asisMap, recid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&        def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Record_Definition);
    
    return getRecordBody(def.The_Union.The_Record_Definition, ctx);
  }
  
  
  struct TypeFundamental
  {
    SgNode* n;
    bool    hasAbstract;
    bool    hasLimited;
    bool    hasTagged;
  };

  TypeFundamental
  getTypeFoundation(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT( decl.Declaration_Kind == An_Ordinary_Type_Declaration );

    TypeFundamental         res{nullptr, false, false, false};
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
          /*
             unused fields: (derivedTypeDef)
                Declaration_List     Implicit_Inherited_Declarations;
          */

          res.n = &getDefinitionTypeID(typenode.Parent_Subtype_Indication, ctx);
          break;
        }

      case A_Derived_Record_Extension_Definition:  // 3.4(2)     -> Trait_Kinds
        {
          SgClassDefinition&  def    = getRecordBodyID(typenode.Record_Definition, ctx);
          SgClassDeclaration& basecl = getParentRecordDeclID(typenode.Parent_Subtype_Indication, ctx);
          SgBaseClass&        parent = mkRecordParent(basecl);
          
          def.append_inheritance(&parent);
          
          /*
          Declaration_List     Implicit_Inherited_Declarations;
          Declaration_List     Implicit_Inherited_Subprograms;
          Declaration          Corresponding_Parent_Subtype;
          Declaration          Corresponding_Root_Type;
          Declaration          Corresponding_Type_Structure;
          Expression_List      Definition_Interface_List;
          */
          res.n = &def;
          break;
        }
        
      case A_Floating_Point_Definition:            // 3.5.7(2)
        {
          SgExpression&         digits     = getExprID_opt(typenode.Digits_Expression, ctx);
          SgAdaRangeConstraint& constraint = getRangeConstraint(typenode.Real_Range_Constraint, ctx);
          
          res.n = &mkAdaFloatType(digits, constraint);
          break;
        }
        
      case A_Tagged_Record_Type_Definition:        // 3.8(2)     -> Trait_Kinds
        {
          SgClassDefinition& def = getRecordBodyID(typenode.Record_Definition, ctx);
                    
          //~ logInfo() << "tagged ? " << typenode.Has_Tagged << std::endl;
          
          /* unused fields:
                bool                 Has_Private;
                bool                 Has_Tagged;
                Declaration_List     Corresponding_Type_Operators;
          */
          res = TypeFundamental{&def, typenode.Has_Abstract, typenode.Has_Limited, true};
          break; 
        }
      

      case Not_A_Type_Definition: /* break; */     // An unexpected element
      case An_Enumeration_Type_Definition:         // 3.5.1(2)
      case A_Signed_Integer_Type_Definition:       // 3.5.4(3)
      case A_Modular_Type_Definition:              // 3.5.4(4)
      case A_Root_Type_Definition:                 // 3.5.4(14):  3.5.6(3)
      case An_Ordinary_Fixed_Point_Definition:     // 3.5.9(3)
      case A_Decimal_Fixed_Point_Definition:       // 3.5.9(6)
      case An_Unconstrained_Array_Definition:      // 3.6(2)
      case A_Constrained_Array_Definition:         // 3.6(2)
      case A_Record_Type_Definition:               // 3.8(2)     -> Trait_Kinds
      //  //|A2005 start
      case An_Interface_Type_Definition:           // 3.9.4      -> Interface_Kinds
      //  //|A2005 end
      case An_Access_Type_Definition:              // 3.10(2)    -> Access_Type_Kinds
      default:
        {
          logWarn() << "unhandled type kind " << typenode.Type_Kind << std::endl;
          ROSE_ASSERT(!PRODUCTION_CODE);
          res.n = sb::buildVoidType();
        }
    }

    ROSE_ASSERT(res.n);
    return res;
  }
  
  
  SgAdaTaskSpec& 
  getTaskSpec(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Type_Declaration);
    
    SgAdaTaskSpec&          sgnode = mkAdaTaskSpec();
    
    if (decl.Type_Declaration_View == 0) 
      return sgnode;
        
    sgnode.set_hasMembers(true);
    
    // Definition_ID Discriminant_Part    
    Element_Struct&         elem = retrieveAs<Element_Struct>(asisMap, decl.Type_Declaration_View);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&      def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Task_Definition);
    
    Task_Definition_Struct& tasknode = def.The_Union.The_Task_Definition;

    // visible items
    {
      ElemIdRange range = idRange(tasknode.Visible_Part_Items);

      traverseIDs(range, asisMap, ElemCreator(ctx.scope_npc(sgnode)));
    }
    
    // private items
    {
      ElemIdRange range = idRange(tasknode.Private_Part_Items);
      ROSE_ASSERT((!range.empty()) == tasknode.Is_Private_Present);

      traverseIDs(range, asisMap, ElemCreator(ctx.scope_npc(sgnode), true /* private items */));
    }
    
    /* unused fields: (Task_Definition_Struct)
         bool                  Has_Task;
    */
    return sgnode;
  }
  
  
  SgAdaTaskBody& 
  getTaskBody(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Body_Declaration);
    
    SgAdaTaskBody& sgnode = mkAdaTaskBody();
    ElemIdRange    decls = idRange(decl.Body_Declarative_Items);
    ElemIdRange    stmts = idRange(decl.Body_Statements);

    traverseIDs(decls, asisMap, StmtCreator(ctx.scope_npc(sgnode)));
    traverseIDs(stmts, asisMap, StmtCreator(ctx.scope_npc(sgnode)));
    
    return sgnode;
  }


  typedef SgExpression* (*mk_wrapper_fun)();

  // homogeneous return types instead of covariant ones
  template <class R, R* (*mkexp) (SgExpression*, SgExpression*)>
  SgExpression* mk2_wrapper()
  {
    return mkexp(nullptr, nullptr);
  }
  
  // homogeneous return types instead of covariant ones
  template <class R, R* (*mkexp) (SgExpression*)>
  SgExpression* mk1_wrapper()
  {
    return mkexp(nullptr);
  }
  
  template <class R, R* (*mkexp) (SgExpression*)>
  SgExpression* mk_rem_wrapper()
  {
    return mkexp(nullptr);
  }

/*
    SgExpression* mkCall(SgExpression* callee, SgExpression* args)
    {
      SgExprListExp* lst = SG_ASSERT_TYPE(SgExprListExp, args);

      return sb::buildFunctionCallExp(callee, lst);
    }
*/
    
  SgExpression&
  getOperator(Expression_Struct& expr, AstContext ctx)
  {
    typedef std::map<Operator_Kinds, mk_wrapper_fun> binary_maker_map_t;
    
    static const binary_maker_map_t binary_maker_map 
                     = { { An_And_Operator,                  mk2_wrapper<SgBitAndOp,         sb::buildBitAndOp> },         /* break; */
                         { An_Or_Operator,                   mk2_wrapper<SgBitOrOp,          sb::buildBitOrOp> },          /* break; */
                         { An_Xor_Operator,                  mk2_wrapper<SgBitXorOp,         sb::buildBitXorOp> },         /* break; */
                         { An_Equal_Operator,                mk2_wrapper<SgEqualityOp,       sb::buildEqualityOp> },       /* break; */
                         { A_Not_Equal_Operator,             mk2_wrapper<SgNotEqualOp,       sb::buildNotEqualOp> },       /* break; */
                         { A_Less_Than_Operator,             mk2_wrapper<SgLessThanOp,       sb::buildLessThanOp> },       /* break; */
                         { A_Less_Than_Or_Equal_Operator,    mk2_wrapper<SgLessOrEqualOp,    sb::buildLessOrEqualOp> },    /* break; */
                         { A_Greater_Than_Operator,          mk2_wrapper<SgGreaterThanOp,    sb::buildGreaterThanOp> },    /* break; */
                         { A_Greater_Than_Or_Equal_Operator, mk2_wrapper<SgGreaterOrEqualOp, sb::buildGreaterOrEqualOp> }, /* break; */
                         { A_Plus_Operator,                  mk2_wrapper<SgAddOp,            sb::buildAddOp> },            /* break; */
                         { A_Minus_Operator,                 mk2_wrapper<SgSubtractOp,       sb::buildSubtractOp> },       /* break; */
                         { A_Concatenate_Operator,           mk2_wrapper<SgConcatenationOp,  sb::buildConcatenationOp> },  /* break; */
                         { A_Unary_Plus_Operator,            mk1_wrapper<SgUnaryAddOp,       sb::buildUnaryAddOp> },       /* break; */
                         { A_Unary_Minus_Operator,           mk1_wrapper<SgMinusOp,          sb::buildMinusOp> },          /* break; */
                         { A_Multiply_Operator,              mk2_wrapper<SgMultiplyOp,       sb::buildMultiplyOp> },       /* break; */
                         { A_Divide_Operator,                mk2_wrapper<SgDivideOp,         sb::buildDivideOp> },         /* break; */
                         { A_Mod_Operator,                   mk2_wrapper<SgModOp,            sb::buildModOp> },            /* break; */
                         { A_Rem_Operator,                   mk2_wrapper<SgRemOp,            buildRemOp> },                /* break; */
                         { An_Exponentiate_Operator,         mk2_wrapper<SgPowerOp,          sb::buildPowerOp> },          /* break; */
                         { An_Abs_Operator,                  mk1_wrapper<SgAbsOp,            buildAbsOp> },                /* break; */
                         { A_Not_Operator,                   mk1_wrapper<SgNotOp,            sb::buildNotOp> },            /* break; */
                       };
    
    ROSE_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    binary_maker_map_t::const_iterator pos = binary_maker_map.find(expr.Operator_Kind);
    
    if (pos != binary_maker_map.end())
      return SG_DEREF(pos->second());
    
    ROSE_ASSERT(expr.Operator_Kind != Not_An_Operator); /* break; */
    
    /* unused fields:
         Defining_Name_ID      Corresponding_Name_Definition;
         Defining_Name_List    Corresponding_Name_Definition_List; // Only >1 if the expression in a pragma is ambiguous
         Element_ID            Corresponding_Name_Declaration; // Decl or stmt
         Defining_Name_ID      Corresponding_Generic_Element;
    */
    return SG_DEREF(sb::buildOpaqueVarRefExp(expr.Name_Image, &ctx.scope()));
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
  
  struct AdaCallBuilder : sg::DispatchHandler<SgExpression*>
  {
    typedef sg::DispatchHandler<SgExpression*> base;
    
    AdaCallBuilder(ElemIdRange params, AstContext astctx)
    : base(nullptr), range(params), ctx(astctx)
    {}
    
    ArglistCreator computeArguments()
    {
      return traverseIDs(range, asisMap, ArglistCreator(ctx));
    }
    
    void mkCall(SgExpression& n)
    {
      SgExprListExp& arglst = computeArguments();

      res = sb::buildFunctionCallExp(&n, &arglst);
    }
    
    void handle(SgNode& n)       { SG_UNEXPECTED_NODE(n); }
    
    // default
    void handle(SgExpression& n) { mkCall(n); }
    
    void handle(SgUnaryOp& n) 
    {
      // computed target ?
      if (n.get_operand() != nullptr)
      {
        mkCall(n);
        return;
      }
      
      ROSE_ASSERT(range.size() == 1);
      std::vector<SgExpression*> args = computeArguments();
      
      ROSE_ASSERT(args.size() == 1);
      n.set_operand(args[0]);
      res = &n;
    }
    
    void handle(SgBinaryOp& n) 
    {
      // lhs and rhs must be null or not-null
      ROSE_ASSERT((n.get_lhs_operand() == nullptr) == (n.get_rhs_operand() == nullptr));
      
      // computed target ?
      if (n.get_lhs_operand() != nullptr)
      {
        mkCall(n);
        return;
      }
      
      ROSE_ASSERT(range.size() == 2);
      std::vector<SgExpression*> args = computeArguments();
      
      ROSE_ASSERT(args.size() == 2);
      n.set_lhs_operand(args[0]);
      n.set_rhs_operand(args[1]);
      res = &n;
    }
    
    ElemIdRange range;
    AstContext  ctx;
  };
  

  SgExpression&
  getExpr(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    bool               withParen = false;
    Expression_Struct& expr      = elem.The_Union.Expression;
    SgExpression*      res       = NULL;

    switch (expr.Expression_Kind)
    {
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
          logTrace() << "function call "
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
          
          // distinguish between operators and calls
          res = sg::dispatch(AdaCallBuilder(range, ctx), &target); 

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
          res = &mkValue<SgIntVal>(expr.Value_Image);
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
          // \todo remove _opt once the asis connection fills in the list
          SgExpression& lhs = getExprID_opt(expr.Short_Circuit_Operation_Left_Expression, ctx);
          SgExpression& rhs = getExprID_opt(expr.Short_Circuit_Operation_Right_Expression, ctx);

          res = sb::buildOrOp(&lhs, &rhs);
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case A_Parenthesized_Expression:                // 4.4
        {
          withParen = true;
          
          // \todo remove _opt when the asis connector implements A_Parenthesized_Expression
          res = &getExprID_opt(expr.Expression_Parenthesized, ctx);
          
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
      case Not_An_Expression: /* break; */            // An unexpected element
      default:
        logWarn() << "unhandled expression: " << expr.Expression_Kind << std::endl;
        res = sb::buildIntVal();
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
    
    attachSourceLocation(SG_DEREF(res), elem);
    res->set_need_paren(withParen);
    return *res;
  }

  SgExpression&
  getExprID(Element_ID el, AstContext ctx)
  {
    return getExpr(retrieveAs<Element_Struct>(asisMap, el), ctx);
  }
  
  SgExpression&
  getExprID_opt(Element_ID el, AstContext ctx)
  {
    if (el == -1) 
    {
      logWarn() << "unintialized expression id -1" << std::endl;
      return SG_DEREF( sb::buildNullExpression() );
    }
    
    return el == 0 ? SG_DEREF( sb::buildNullExpression() ) 
                   : getExprID(el, ctx)
                   ;
  }
  
  SgAdaRangeConstraint&
  getRangeConstraint(Element_ID el, AstContext ctx)
  {
    if (el == -1) 
    {
      logWarn() << "Uninitialized element [range constraint]" << std::endl;
      return mkAdaRangeConstraint(mkRangeExp());
    }
    
    SgAdaRangeConstraint* res = nullptr;
    Element_Struct&       elem = retrieveAs<Element_Struct>(asisMap, el);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);
    
    Definition_Struct&    def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Constraint);
    
    Constraint_Struct&    constraint = def.The_Union.The_Constraint;
    
    switch (constraint.Constraint_Kind)
    {
      case A_Simple_Expression_Range:             // 3.2.2: 3.5(3)
        {
          SgExpression& lb       = getExprID(constraint.Lower_Bound, ctx);
          SgExpression& ub       = getExprID(constraint.Upper_Bound, ctx);
          SgRangeExp&   rangeExp = mkRangeExp(lb, ub);
          
          res = &mkAdaRangeConstraint(rangeExp);
          break;
        }
        
      case A_Range_Attribute_Reference:           // 3.5(2)
        {
          SgExpression& expr     = getExprID(constraint.Range_Attribute, ctx);
          SgRangeExp&   rangeExp = SG_DEREF(isSgRangeExp(&expr));
          
          res = &mkAdaRangeConstraint(rangeExp);
          break;
        }
      
      case Not_A_Constraint: /* break; */         // An unexpected element
      case A_Digits_Constraint:                   // 3.2.2: 3.5.9
      case A_Delta_Constraint:                    // 3.2.2: J.3
      case An_Index_Constraint:                   // 3.2.2: 3.6.1
      case A_Discriminant_Constraint:             // 3.2.2
      default:
        logWarn() << "Unhandled constraint: " << constraint.Constraint_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
        res = &mkAdaRangeConstraint(mkRangeExp());
    }
    
    return SG_DEREF(res); 
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

      case Not_A_Mode: /* break; */
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
               || decl.Declaration_Kind == A_Component_Declaration
               );
               
    //~ logWarn() << "decl.Initialization_Expression = " << decl.Initialization_Expression << std::endl;
    if (decl.Initialization_Expression == 0) return nullptr;

    return &getExprID_opt(decl.Initialization_Expression, ctx);
  }


  SgExpression*
  cloneNonNull(SgExpression* exp)
  {
    if (exp == nullptr) return nullptr;

    return si::deepCopy(exp);
  }
  
  typedef std::unique_ptr<SgExpression> GuardedExpression;

  SgInitializedNamePtrList
  constructInitializedNamePtrList( std::map<int, SgInitializedName*>& m,
                                   const NameCreator::result_container& names,
                                   SgType& dcltype,
                                   GuardedExpression initexpr = nullptr
                                 )
  {
    SgInitializedNamePtrList lst;
    int                      num      = names.size();

    for (int i = 0; i < num; ++i)
    {
      const std::string& name = names.at(i).first;
      Element_ID         id   = names.at(i).second;
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, cloneNonNull(initexpr.get()));
      
      attachSourceLocation(dcl, retrieveAs<Element_Struct>(asisMap, id));

      lst.push_back(&dcl);
      recordNode(m, id, dcl, false /* key is non-unique (non-defining/defining declarations) */);
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
    GuardedExpression        initexpr{getVarInit(asisDecl, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars, names, parmtype, std::move(initexpr));
    SgVariableDeclaration&   sgnode   = mkParameter(dclnames, handleModes(asisDecl.Mode_Kind), ctx.scope());
    
    attachSourceLocation(sgnode, elem);
    /* unused fields:
         bool                           Has_Aliased;
         bool                           Has_Null_Exclusion;
    */
    return sgnode;
  }

  //
  // types

  typedef SgType& (*TypeModiferFn) (SgType&);

  SgType& tyIdentity(SgType& ty) { return ty; }

  SgType& tyConstify(SgType& ty) { return SG_DEREF(sb::buildConstType(&ty)); }


  //
  // helper function for combined handling of variables and constant declarations

  void
  handleNumVarCstDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& dcltype, Element_Struct& elem)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange              range    = idRange(decl.Names);
    name_container           names    = traverseIDs(range, asisMap, NameCreator(ctx));
    SgScopeStatement&        scope    = ctx.scope();
    GuardedExpression        initexp{getVarInit(decl, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars, names, dcltype, std::move(initexp));
    SgVariableDeclaration&   vardcl   = mkVarDecl(dclnames, scope);

    attachSourceLocation(vardcl, elem);
    privatize(vardcl, isPrivate);
    scope.append_statement(&vardcl);
    
    ROSE_ASSERT(vardcl.get_parent() == &scope);
  }

  void
  handleNumberDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& numty, Element_Struct& elem)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    handleNumVarCstDecl(decl, ctx, isPrivate, cstty, elem);
  }

  void
  handleVarCstDecl(Declaration_Struct& dcl, AstContext ctx, bool isPrivate, TypeModiferFn tyModifier, Element_Struct& elem)
  {
    handleNumVarCstDecl(dcl, ctx, isPrivate, tyModifier(getVarType(dcl, ctx)), elem);
  }
  
  struct MakeDeclaration : sg::DispatchHandler<SgDeclarationStatement*>
  {
    typedef sg::DispatchHandler<SgDeclarationStatement*> base;
    
    MakeDeclaration(const std::string& name, SgScopeStatement& scope, TypeFundamental basis)
    : base(), dclname(name), dclscope(scope), foundation(basis)
    {}
    
    void handle(SgNode& n)            { SG_UNEXPECTED_NODE(n); } 
    void handle(SgType& n)            { res = &mkTypeDecl(dclname, n, dclscope); }
    
    void handle(SgClassDefinition& n) 
    { 
      SgClassDeclaration&    rec = mkRecordDecl(dclname, n, dclscope); 
      SgDeclarationModifier& mod = rec.get_declarationModifier();
      
      if (foundation.hasAbstract) mod.setAdaAbstract();
      if (foundation.hasLimited)  mod.setAdaLimited();
      if (foundation.hasTagged)   mod.setAdaTagged();
      
      res = &rec;
    }
    
    std::string       dclname;
    SgScopeStatement& dclscope;
    TypeFundamental   foundation;
  };

  struct DeclareType
  {
    DeclareType(TypeFundamental what, AstContext astctx, bool privateItems)
    : foundation(what), scope(astctx.scope()), privateElems(privateItems)
    {}

    void operator()(NameCreator::result_container::value_type nameelem)
    {
      const std::string&      name = nameelem.first;
      Element_ID              id   = nameelem.second;
      SgDeclarationStatement* dcl = sg::dispatch(MakeDeclaration(name, scope, foundation), foundation.n);
      ROSE_ASSERT(dcl);

      privatize(*dcl, privateElems);
      scope.append_statement(dcl);
      recordNode(asisTypes, id, *dcl);
      ROSE_ASSERT(dcl->get_parent() == &scope);
    }

    TypeFundamental   foundation;
    SgScopeStatement& scope;
    bool              privateElems;
    // AstContext        ctx;
  };
  
  struct ParameterCompletion
  {
      ParameterCompletion(ElemIdRange paramrange, AstContext astctx)
      : range(paramrange), ctx(astctx)
      {}
      
      void operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope)
      {
        traverseIDs(range, asisMap, ParmlistCreator(lst, ctx.scope(parmscope)));
      }
    
    private:
      ElemIdRange range;
      AstContext  ctx;
      
      ParameterCompletion() = delete;
  };
  
/*  
  struct DeclareTaskType
  {
    DeclareTaskType(SgAdaTaskSpec& taskspec, AstContext astctx, bool privateItems)
    : spec(taskspec), scope(astctx.scope()), privateElems(privateItems)
    {}

    void operator()(NameCreator::result_container::value_type nameelem)
    {
      const std::string& name = nameelem.first;
      Element_ID         id   = nameelem.second;
      SgAdaTaskTypeDecl& dcl  = mkAdaTaskTypeDecl(name, spec);

      privatize(dcl, privateElems);
      scope.append_statement(&dcl);
      recordNode(asisTypes, id, dcl);
    }

    SgAdaTaskSpec&    spec;
    SgScopeStatement& scope;
    bool              privateElems;
  };
*/

  SgBasicBlock& getFunctionBody(SgFunctionDeclaration& defdcl)
  {
    SgFunctionDefinition& def = SG_ASSERT_TYPE(SgFunctionDefinition, SG_DEREF(defdcl.get_definition()));
    
    return SG_DEREF(def.get_body());
  }
  
  void setOverride(SgDeclarationModifier& sgmod, bool isOverride)
  {
    if (isOverride) 
      sgmod.setOverride();            
    else
      sgmod.unsetOverride();
  }
  
  typedef std::pair<SgTryStmt*, SgBasicBlock*> TryBlockNodes;
          
  TryBlockNodes
  createTryBlockIfNeeded(bool hasHandlers, SgBasicBlock& outer)
  {
    if (!hasHandlers) return TryBlockNodes(nullptr, &outer);
    
    SgBasicBlock& tryBlock = mkBasicBlock();
    SgTryStmt&    tryStmt  = mkTryStmt(tryBlock);
    
    //~ link_parent_child(outer, as<SgStatement>(tryStmt), SgBasicBlock::append_statement);
    outer.append_statement(&tryStmt);
    ROSE_ASSERT(tryStmt.get_parent() == &outer);
    
    return TryBlockNodes(&tryStmt, &tryBlock);
  }


  void handleDeclaration(Element_Struct& elem, AstContext ctx, bool isPrivate = false)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Declaration);
    
    Declaration_Struct& decl = elem.The_Union.Declaration;

    switch (decl.Declaration_Kind)
    {
      case A_Package_Declaration:                    // 7.1(2)
        {
          logTrace() << "package decl" << std::endl;
          SgScopeStatement&     outer   = ctx.scope();
          NameKeyPair           adaname = singleName(decl, ctx);
          SgAdaPackageSpecDecl& sgnode  = mkAdaPackageSpecDecl(adaname.first, outer);
          SgAdaPackageSpec&     pkgspec = SG_DEREF(sgnode.get_definition());
          
          //~ recordNode(asisUnits, adaUnit.ID, pkgdecl);
          recordNode(asisDecls, elem.ID, sgnode);

          privatize(sgnode, isPrivate);
          attachSourceLocation(sgnode, elem);
          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);
          
          // visible items
          {
            ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

            traverseIDs(range, asisMap, ElemCreator(ctx.scope(pkgspec)));
          }
          
          // private items
          {
            ElemIdRange range = idRange(decl.Private_Part_Declarative_Items);
            ROSE_ASSERT((!range.empty()) == decl.Is_Private_Present);
  
            traverseIDs(range, asisMap, ElemCreator(ctx.scope(pkgspec), true /* private items */));
          }

          /* unused nodes:
                 Pragma_Element_ID_List         Pragmas;
                 Element_ID                     Corresponding_End_Name;
                 bool                           Is_Name_Repeated;
                 Declaration_ID                 Corresponding_Declaration;
           */
          break;
        }

      case A_Package_Body_Declaration:               // 7.2(2)
        {          
          logTrace() << "package body decl" << std::endl;
          
          SgScopeStatement&     outer   = ctx.scope();
          Element_ID            specID  = decl.Corresponding_Declaration;
          SgAdaPackageSpecDecl& specdcl = retrieveNodeAs<SgAdaPackageSpecDecl>(asisDecls, specID);
          SgAdaPackageBodyDecl& sgnode  = mkAdaPackageBodyDecl(specdcl, outer);
          SgAdaPackageBody&     pkgbody = SG_DEREF(sgnode.get_definition());

          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);
          
          // declarative items
          {
            ElemIdRange range = idRange(decl.Body_Declarative_Items);
          
            traverseIDs(range, asisMap, ElemCreator(ctx.scope(pkgbody)));
          }
          
          // statements
          {
            ElemIdRange range = idRange(decl.Body_Statements);
          
            traverseIDs(range, asisMap, ElemCreator(ctx.scope(pkgbody)));
          }
          
          /*
           * unused nodes:
                 Pragma_Element_ID_List         Pragmas;
                 Element_ID                     Corresponding_End_Name;
                 Exception_Handler_List         Body_Exception_Handlers;
                 Declaration_ID                 Body_Block_Statement;
                 bool                           Is_Name_Repeated;
                 bool                           Is_Subunit;
                 Declaration_ID                 Corresponding_Body_Stub;
           */
          break;
        }

      case A_Function_Declaration:                   // 6.1(4)   -> Trait_Kinds
      case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
        {
          logTrace() << (decl.Declaration_Kind == A_Function_Declaration ? "function" : "procedure") << " decl"
                     << std::endl;

          const bool             isFunc  = decl.Declaration_Kind == A_Function_Declaration;
          SgScopeStatement&      outer   = ctx.scope();
          NameKeyPair            adaname = singleName(decl, ctx);
          ElemIdRange            range   = idRange(decl.Parameter_Profile);
          SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                  : SG_DEREF(sb::buildVoidType());
          SgFunctionDeclaration& sgnode  = mkProcedure(adaname.first, outer, rettype, ParameterCompletion(range, ctx));
          
          setOverride(sgnode.get_declarationModifier(), decl.Is_Overriding_Declaration);
          recordNode(asisDecls, elem.ID, sgnode);

          privatize(sgnode, isPrivate);
          attachSourceLocation(sgnode, elem);
          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);

          /* unhandled fields

             bool                          Has_Abstract
             bool                          Is_Not_Overriding_Declaration
             bool                          Is_Dispatching_Operation
             Declaration_ID                Corresponding_Declaration
             Declaration_ID                Corresponding_Body
             Declaration_ID                Corresponding_Subprogram_Derivation
             Type_Definition_ID            Corresponding_Type

           +func:
             bool                          Is_Not_Null_Return
             Declaration_ID                Corresponding_Equality_Operator

             break;
          */

          break;
        }

      case A_Function_Body_Declaration:              // 6.3(2)
      case A_Procedure_Body_Declaration:             // 6.3(2)
        {
          logTrace() << (decl.Declaration_Kind == A_Function_Body_Declaration ? "function" : "procedure") << " body decl"
                     << std::endl;

          const bool             isFunc  = decl.Declaration_Kind == A_Function_Body_Declaration;
          SgScopeStatement&      outer   = ctx.scope();
          NameKeyPair            adaname = singleName(decl, ctx);
          ElemIdRange            params  = idRange(decl.Parameter_Profile);
          SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                  : SG_DEREF(sb::buildVoidType());
          SgFunctionDeclaration& sgnode  = mkProcedureDef(adaname.first, outer, rettype, ParameterCompletion(params, ctx));
          SgBasicBlock&          declblk = getFunctionBody(sgnode);
          
          recordNode(asisDecls, elem.ID, sgnode);
          privatize(sgnode, isPrivate);
          attachSourceLocation(sgnode, elem);
          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);

          ElemIdRange            hndlrs  = idRange(decl.Body_Exception_Handlers);
          logWarn() << "block ex handlers: " << hndlrs.size() << std::endl;
          
          TryBlockNodes          trydata = createTryBlockIfNeeded(hndlrs.size() > 0, declblk);
          SgTryStmt*             trystmt = trydata.first;
          SgBasicBlock&          stmtblk = SG_DEREF(trydata.second);
          
          {
            ElemIdRange range = idRange(decl.Body_Declarative_Items);

            traverseIDs(range, asisMap, ElemCreator(ctx.scope(declblk)));
          }

          {
            LabelManager lblmgr;
            ElemIdRange  range = idRange(decl.Body_Statements);

            traverseIDs(range, asisMap, StmtCreator(ctx.scope(stmtblk).labels(lblmgr)));
          }
          
          if (trystmt)
          {
            traverseIDs(hndlrs, asisMap, ExHandlerCreator(ctx.scope(declblk), SG_DEREF(trystmt)));
          }

          /* unhandled field
             Declaration_ID                 Body_Block_Statement;

           +func:
             bool                           Is_Not_Null_Return
             
             break;
          */
          break;
        }

      case An_Ordinary_Type_Declaration:            // 3.2.1(3)
        {
          typedef NameCreator::result_container name_container;

          logTrace() << "Ordinary Type "
                     << "\n  abstract: " << decl.Has_Abstract
                     << "\n  limited: " << decl.Has_Limited
                     << std::endl;
          ElemIdRange     range = idRange(decl.Names);
          name_container  names = traverseIDs(range, asisMap, NameCreator(ctx));
          TypeFundamental ty    = getTypeFoundation(decl, ctx);

          ROSE_ASSERT(ctx.scope().get_parent());
          std::for_each(names.begin(), names.end(), DeclareType(ty, ctx, isPrivate));

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


      //~ case A_Subtype_Declaration:                    // 3.2.2(2)
        //~ {
          
          //~ /* unused fields:
                //~ Declaration_ID                 Corresponding_First_Subtype;
                //~ Declaration_ID                 Corresponding_Last_Constraint;
                //~ Declaration_ID                 Corresponding_Last_Subtype;
          //~ */
          //~ break ;
        //~ }
        
      case A_Variable_Declaration:                   // 3.3.1(2) -> Trait_Kinds
        {
          handleVarCstDecl(decl, ctx, isPrivate, tyIdentity, elem);
          /* unused fields:
               bool                           Has_Aliased;
          */
          break;
        }

      case An_Integer_Number_Declaration:            // 3.3.2(2)
        {
          handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildIntType()), elem);

          /* unused fields:
          */
          break;
        }
      
      case A_Constant_Declaration:
        {
          handleVarCstDecl(decl, ctx, isPrivate, tyConstify, elem);
          /* unused fields:
               bool                           Has_Aliased;
          */
          break;
        }
      
      case A_Real_Number_Declaration:                // 3.5.6(2)
        {
          handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildFloatType()), elem);

          /* unused fields:
           */
          break;
        }

      case A_Loop_Parameter_Specification:           // 5.5(4)   -> Trait_Kinds
        {
          ROSE_ASSERT(!isPrivate);
          
          NameKeyPair            adaname = singleName(decl, ctx);           
          SgType&                vartype = SG_DEREF( sb::buildIntType() ); // \todo 
          SgInitializedName&     loopvar = mkInitializedName(adaname.first, vartype, nullptr);
          SgScopeStatement&      scope   = ctx.scope();
               
          recordNode(asisVars, adaname.second, loopvar);
          
          SgVariableDeclaration& sgnode  = mkVarDecl(loopvar, scope);
          
          attachSourceLocation(sgnode, elem);
          scope.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &scope);
          
          /* unused fields:
               bool                           Has_Reverse;
               Discrete_Subtype_Definition_ID Specification_Subtype_Definition;
           */
          break;
        }
      
      case A_Parameter_Specification:                // 6.1(15)  -> Trait_Kinds
        {
          // handled in getParm
          // break;
          ROSE_ASSERT(false);
        }
        
      case A_Task_Type_Declaration:                  // 9.1(2)
        {
          SgAdaTaskSpec&     spec    = getTaskSpec(decl, ctx);
          NameKeyPair        adaname = singleName(decl, ctx);
          SgAdaTaskTypeDecl& sgnode  = mkAdaTaskTypeDecl(adaname.first, spec, ctx.scope());

          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          ctx.scope().append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
          recordNode(asisTypes, adaname.second, sgnode);
          recordNode(asisDecls, adaname.second, sgnode);

          /* unused fields:
               bool                           Has_Task;
               Element_ID                     Corresponding_End_Name;
               Definition_ID                  Discriminant_Part;
               Definition_ID                  Type_Declaration_View;
               Declaration_ID                 Corresponding_Type_Declaration;
               Declaration_ID                 Corresponding_Type_Partial_View;
               Declaration_ID                 Corresponding_First_Subtype;
               Declaration_ID                 Corresponding_Last_Constraint;
               Declaration_ID                 Corresponding_Last_Subtype;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
               Declaration_ID                 Corresponding_Body;
               Expression_List                Declaration_Interface_List;
           */
           break;
        }
      
      case A_Task_Body_Declaration:                  // 9.1(6)
        {
          SgAdaTaskBody&          tskbody = getTaskBody(decl, ctx);
          NameKeyPair             adaname = singleName(decl, ctx);
          Element_ID              declID  = decl.Corresponding_Declaration;
          SgDeclarationStatement* tskdecl = findNode(asisDecls, declID);
          
          // \todo \review not sure why a task body could be independently created
          SgAdaTaskBodyDecl&      sgnode  = tskdecl ? mkAdaTaskBodyDecl(*tskdecl, tskbody, ctx.scope())
                                                    : mkAdaTaskBodyDecl(adaname.first, tskbody, ctx.scope());
          
          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          ctx.scope().append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
          recordNode(asisDecls, adaname.second, sgnode);
          
          /* unused fields:
               bool                           Has_Task;
               Pragma_Element_ID_List         Pragmas;
               Element_ID                     Corresponding_End_Name;
               Exception_Handler_List         Body_Exception_Handlers;
               Declaration_ID                 Body_Block_Statement;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
               bool                           Is_Subunit;
               Declaration_ID                 Corresponding_Body_Stub;
          */
          break;
        }
        
      case An_Entry_Declaration:                     // 9.5.2(2)
        {
          NameKeyPair     adaname = singleName(decl, ctx);
          ElemIdRange     range   = idRange(decl.Parameter_Profile);
          SgAdaEntryDecl& sgnode  = mkAdaEntryDecl(adaname.first, ctx.scope(), ParameterCompletion(range, ctx));
                    
          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          ctx.scope().append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
          recordNode(asisDecls, adaname.second, sgnode);

          /* unused fields:
               bool                           Is_Overriding_Declaration;
               bool                           Is_Not_Overriding_Declaration;
               Declaration_ID                 Corresponding_Body;
               Discrete_Subtype_Definition_ID Entry_Family_Definition;
          */
          break;
        }
      
      case An_Exception_Declaration:                 // 11.1(2)
        {
          typedef NameCreator::result_container name_container;

          ElemIdRange              range    = idRange(decl.Names);
          name_container           names    = traverseIDs(range, asisMap, NameCreator(ctx));
          SgScopeStatement&        scope    = ctx.scope();
          SgType&                  excty    = retrieveNode(adaTypes, std::string("Exception"));
          SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisExcps, names, excty);
          SgVariableDeclaration&   sgnode   = mkExceptionDecl(dclnames, scope); 

          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          scope.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &scope);
          break;
        }

      case A_Component_Declaration:                  // 3.8(6)
        {
          handleVarCstDecl(decl, ctx, isPrivate, tyIdentity, elem);
          /* unused clause:
                Pragma_Element_ID_List         Corresponding_Pragmas;
                Element_ID_List                Aspect_Specifications;
                Representation_Clause_List     Corresponding_Representation_Clauses;
                bool                           Has_Aliased;
          */
          break;
        }
        
      case A_Choice_Parameter_Specification:         // 11.2(4)
        {
          // handled in handleExceptionHandler
          ROSE_ASSERT(false);
          break;
        }

      case Not_A_Declaration: /* break; */           // An unexpected element
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
      case A_Protected_Body_Declaration:             // 9.4(7)
      case An_Entry_Body_Declaration:                // 9.5.2(5)
      case An_Entry_Index_Specification:             // 9.5.2(2)
      case A_Procedure_Body_Stub:                    // 10.1.3(3)
      case A_Function_Body_Stub:                     // 10.1.3(3)
      case A_Package_Body_Stub:                      // 10.1.3(4)
      case A_Task_Body_Stub:                         // 10.1.3(5)
      case A_Protected_Body_Stub:                    // 10.1.3(6)
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
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }

  void handleClause(Element_Struct& elem, AstContext ctx)
  {
    Clause_Struct& clause = elem.The_Union.Clause;

    switch (clause.Clause_Kind)
    {
      case A_With_Clause:                // 10.1.2
        {
          typedef std::vector<SgExpression*> ExprVec;

          ElemIdRange        range  = idRange(clause.Clause_Names);
          ExprVec            uselst = traverseIDs(range, asisMap, ExprListCreator(ctx));
          SgImportStatement& sgnode = mkWithClause(uselst);

          attachSourceLocation(sgnode, elem);
          ctx.scope().append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
          /* unused fields:
              bool   Has_Limited
          */
          break;
        }

      case Not_A_Clause: /* break; */    // An unexpected element
      case A_Use_Package_Clause:         // 8.4
      case A_Use_Type_Clause:            // 8.4
      case A_Use_All_Type_Clause:        // 8.4: Ada 2012
      case A_Representation_Clause:      // 13.1     -> Representation_Clause_Kinds
      case A_Component_Clause:           // 13.5.1
      default:
        logWarn() << "unhandled clause kind: " << clause.Clause_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }

  void handleElement(Element_Struct& elem, AstContext ctx, bool isPrivate)
  {
    switch (elem.Element_Kind)
    {
        case A_Declaration:             // Asis.Declarations
        {
          handleDeclaration(elem, ctx, isPrivate);
          break;
        }

      case A_Clause:                  // Asis.Clauses
        {
          // currently only handles with clauses
          handleClause(elem, ctx);
          break;
        }

      case A_Defining_Name:           // Asis.Declarations
        {
          // handled by getName
          ROSE_ASSERT(false);
          break;
        }

      case A_Statement:               // Asis.Statements
        {
          // handled in StmtCreator
          ROSE_ASSERT(false);
          break;
        }
        
      case An_Expression:             // Asis.Expressions
        {
          // handled by getExpr 
          ROSE_ASSERT(false);
          break;
        }
      
      case An_Exception_Handler:      // Asis.Statements
        {
          // handled by handleExceptionHandler 
          ROSE_ASSERT(false);
          break;          
        }
        
      case A_Definition:              // Asis.Definitions
        {
          // records (one of many definitions) are handled by getRecordBody
          Definition_Struct& def = elem.The_Union.Definition;
          
          logWarn() << "Unhandled element " << elem.Element_Kind 
                    << "\n  definition kind: " << def.Definition_Kind 
                    << std::endl;
          
          ROSE_ASSERT(false && !PRODUCTION_CODE);
          break;
        }
        
      case Not_An_Element: /* break; */ // Nil_Element
      case A_Path:                    // Asis.Statements
      case A_Pragma:                  // Asis.Elements
      case An_Association:            // Asis.Expressions
      default:
        logWarn() << "Unhandled element " << elem.Element_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }
  
  /// labels a statement with a block label or a sequence of labels (if needed)
  /// @{
  SgStatement&
  labelIfNeeded(SgStatement& stmt, std::string lblname, Defining_Name_ID lblid, AstContext ctx)
  {
    ROSE_ASSERT(lblid > 0);
    
    SgNode&           parent  = SG_DEREF(stmt.get_parent());
    SgLabelStatement& sgn     = mkLabelStmt(lblname, stmt, ctx.scope());
    Element_Struct&   lblelem = retrieveAs<Element_Struct>(asisMap, lblid);
    
    //~ copyFileInfo(stmt, sgn);
    attachSourceLocation(sgn, lblelem);
    sgn.set_parent(&parent);
    ctx.labels().label(lblid, sgn);
    
    ROSE_ASSERT(stmt.get_parent() == &sgn);
    return sgn; 
  }
  
  template <class SageAdaStmt>
  SgStatement&
  labelIfNeeded(SageAdaStmt& stmt, Defining_Name_ID lblid, AstContext ctx)
  {
    ROSE_ASSERT(lblid >= 0);
    
    if (!lblid) return stmt;
    
    Element_Struct& lblelem = retrieveAs<Element_Struct>(asisMap, lblid);
    std::string     lblname = getName(lblelem, ctx);
    
    stmt.set_string_label(lblname);
    return labelIfNeeded(stmt, lblname, lblid, ctx);
  }
  
  SgStatement&
  labelIfNeeded(SgStatement& stmt, Statement_Struct& adastmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange    range  = idRange(adastmt.Label_Names);
    name_container names  = traverseIDs(range, asisMap, NameCreator(ctx));
    SgStatement*   sgnode = std::accumulate( names.rbegin(), names.rend(),
                                             &stmt,
                                             [&](SgStatement* labeled, name_container::value_type& el) -> SgStatement*
                                             {
                                               return &labelIfNeeded(SG_DEREF(labeled), el.first, el.second, ctx);
                                             } 
                                           );
        
    return SG_DEREF(sgnode);
  } 
  
  /// @}


  /// completes statements by setting source locations, parent node, 
  /// adding labels (if needed)...
  /// @{
  void
  completeStmt(SgStatement& sgnode, Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Statement);
    
    attachSourceLocation(sgnode, elem);
    sgnode.set_parent(&ctx.scope());
    
    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgnode, stmt, ctx);
    
    ctx.scope().append_statement(&sgn);
    ROSE_ASSERT(sgn.get_parent() == &ctx.scope());
  }
  
  template <class SageAdaStmt>
  void
  completeStmt(SageAdaStmt& sgnode, Element_Struct& elem, AstContext ctx, Defining_Name_ID lblid)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Statement);
    
    attachSourceLocation(sgnode, elem);
    sgnode.set_parent(&ctx.scope());
    
    SgStatement&      sgn0 = labelIfNeeded(sgnode, lblid, ctx);
    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgn0, stmt, ctx);
    
    ctx.scope().append_statement(&sgn);
    ROSE_ASSERT(sgn.get_parent() == &ctx.scope());
  }
  
  /// @}
  
  NameCreator::result_container
  queryDeclNames(Declaration_ID id, AstContext ctx)
  {
    if (id == 0) return NameCreator::result_container();
    
    Element_Struct& elem = retrieveAs<Element_Struct>(asisMap, id);
    
    ROSE_ASSERT(elem.Element_Kind == A_Declaration);
    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    
    ROSE_ASSERT((asisDecl.Declaration_Kind == A_Choice_Parameter_Specification));
               
    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);
    
    return traverseIDs(range, asisMap, NameCreator(ctx));
  }
  
  
  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;
    
    ROSE_ASSERT(elem.Element_Kind == An_Exception_Handler);
    
    Exception_Handler_Struct& ex      = elem.The_Union.Exception_Handler;
    name_container            names   = queryDeclNames(ex.Choice_Parameter_Specification, ctx);
    
    if (names.size() == 0)
    {
      names.push_back(std::make_pair(std::string(), elem.ID));
    }
    
    ROSE_ASSERT (names.size() == 1);
    ElemIdRange              tyRange = idRange(ex.Exception_Choices);
    SgType&                  extypes = traverseIDs(tyRange, asisMap, HandlerTypeCreator(ctx));
    SgInitializedNamePtrList lst     = constructInitializedNamePtrList(asisVars, names, extypes); 
    SgBasicBlock&            body    = mkBasicBlock();
    
    ROSE_ASSERT(lst.size() == 1);
    SgCatchOptionStmt& 	     sgnode  = mkExceptionHandler(SG_DEREF(lst[0]), body);
    ElemIdRange              range   = idRange(ex.Handler_Statements);
    
    logWarn() << "catch handler" << std::endl;
    linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);

    traverseIDs(range, asisMap, StmtCreator(ctx.scope(body)));
          
    /* unused fields:
         Pragma_Element_ID_List Pragmas;
    */
  }
  
  void handleStmt(Element_Struct& elem, AstContext ctx)
  {
    logTrace() << "a statement (in progress) " << elem.Element_Kind << std::endl;
    
    // declarations are statements too
    if (elem.Element_Kind == A_Declaration)
    {
      handleDeclaration(elem, ctx);
      return;
    }
    
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    Statement_Struct& stmt = elem.The_Union.Statement;

    switch (stmt.Statement_Kind)
    {
      case A_Null_Statement:                    // 5.1
        {
          completeStmt(mkNullStmt(), elem, ctx);
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

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_If_Statement:                     // 5.3
        {
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgStatement& sgnode = traverseIDs(range, asisMap, IfStmtCreator(ctx));

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_Case_Statement:                    // 5.4
        {
          SgExpression&      caseexpr = getExprID(stmt.Case_Expression, ctx);
          SgBasicBlock&      casebody = mkBasicBlock();
          SgSwitchStatement& sgnode   = mkAdaCaseStmt(caseexpr, casebody);
          ElemIdRange        range    = idRange(stmt.Statement_Paths);

          traverseIDs(range, asisMap, CaseStmtCreator(ctx.scope(casebody), sgnode));
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_While_Loop_Statement:              // 5.5
        {
          SgExpression& cond     = getExprID(stmt.While_Condition, ctx);
          SgBasicBlock& block    = mkBasicBlock();
          ElemIdRange   adaStmts = idRange(stmt.Loop_Statements);
          SgWhileStmt&  sgnode   = mkWhileStmt(cond, block);
          
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);
          
          recordNode(asisLoops, elem.ID, sgnode);
          traverseIDs(adaStmts, asisMap, StmtCreator(ctx.scope(block)));
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
          */
          break;
        }

        case A_Loop_Statement:                    // 5.5
        {
          SgBasicBlock&  block    = mkBasicBlock();
          ElemIdRange    adaStmts = idRange(stmt.Loop_Statements);
          SgAdaLoopStmt& sgnode   = mkLoopStmt(block);
          
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);
          
          recordNode(asisLoops, elem.ID, sgnode);
          traverseIDs(adaStmts, asisMap, StmtCreator(ctx.scope(block)));
          
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
          */
          break;
        }

      case A_For_Loop_Statement:                // 5.5
        {
          SgBasicBlock&       block  = mkBasicBlock();
          SgForStatement&     sgnode = mkForStatement(block);
          Element_Struct&     forvar = retrieveAs<Element_Struct>(asisMap, stmt.For_Loop_Parameter_Specification);
          SgForInitStatement& forini = SG_DEREF( sb::buildForInitStatement(sgnode.getStatementList()) );

          linkParentChild(sgnode, forini, &SgForStatement::set_for_init_stmt);
          handleDeclaration(forvar, ctx.scope_npc(sgnode));
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);
          ROSE_ASSERT(sgnode.getStatementList().size() <= 1 /* \todo should be 0 */);
          
          // \todo this swap is needed, otherwise the variable declaration ends 
          //       up in the body instead of the initializer.. ???
          std::swap(forini.get_init_stmt(), block.get_statements());
          
          ElemIdRange         loopStmts = idRange(stmt.Loop_Statements);

          recordNode(asisLoops, elem.ID, sgnode);
          traverseIDs(loopStmts, asisMap, StmtCreator(ctx.scope(block)));

          /* unused fields:
               Pragma_Element_ID_List Pragmas;
               Element_ID             Corresponding_End_Name;
          */
          break;
        }

      case A_Block_Statement:                   // 5.6
        {
          SgBasicBlock& sgnode   = mkBasicBlock();
          ElemIdRange   blkDecls = idRange(stmt.Block_Declarative_Items);
          ElemIdRange   blkStmts = idRange(stmt.Block_Statements);
          ElemIdRange   exHndlrs = idRange(stmt.Block_Exception_Handlers);
          logWarn() << "block ex handlers: " << exHndlrs.size() << std::endl;
          
          TryBlockNodes trydata  = createTryBlockIfNeeded(exHndlrs.size() > 0, sgnode);
          SgTryStmt*    tryblk   = trydata.first;
          SgBasicBlock& block    = SG_DEREF(trydata.second);
          
          completeStmt(block, elem, ctx, stmt.Statement_Identifier);
          traverseIDs(blkDecls, asisMap, StmtCreator(ctx.scope(sgnode)));
          traverseIDs(blkStmts, asisMap, StmtCreator(ctx.scope(block)));
          
          if (tryblk)
          {
            traverseIDs(exHndlrs, asisMap, ExHandlerCreator(ctx.scope(sgnode), SG_DEREF(tryblk)));
          }
          
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
                bool                      Is_Declare_Block;
          */
          break;
        }

      case An_Exit_Statement:                   // 5.7
        {
          SgStatement&  exitedLoop    = retrieveNode(asisLoops, stmt.Corresponding_Loop_Exited); 
          SgExpression& exitCondition = getExprID_opt(stmt.Exit_Condition, ctx);
          const bool    loopIsNamed   = stmt.Exit_Loop_Name > 0;
          SgStatement&  sgnode        = mkAdaExitStmt(exitedLoop, exitCondition, loopIsNamed);
          
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Exit_Loop_Name;
          */
          break;
        }
      
      case A_Goto_Statement:                    // 5.8
        {
          SgGotoStatement& sgnode = SG_DEREF( sb::buildGotoStatement() );
          
          ctx.labels().gotojmp(stmt.Goto_Label, sgnode);
          
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
                Statement_ID           Corresponding_Destination_Statement;
          */
          break;
        }
      
      case A_Procedure_Call_Statement:          // 6.4
        {
          SgDeclarationStatement& tgt    = retrieveNode(asisDecls, stmt.Corresponding_Called_Entity);
          SgFunctionDeclaration*  fundcl = isSgFunctionDeclaration(&tgt);
          SgFunctionRefExp&       funref = SG_DEREF(sb::buildFunctionRefExp(fundcl));
          ElemIdRange             range  = idRange(stmt.Call_Statement_Parameters);
          SgExprListExp&          arglst = traverseIDs(range, asisMap, ArglistCreator(ctx));
          SgStatement&            sgnode = SG_DEREF(sb::buildFunctionCallStmt(&funref, &arglst));

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
              bool        Is_Prefix_Notation
              bool        Is_Dispatching_Call
              bool        Is_Call_On_Dispatching_Operation
              Element_ID  Called_Name
              Declaration Corresponding_Called_Entity_Unwound
          */
          break;
        }

      case A_Return_Statement:                  // 6.5
        {
          Element_Struct* exprel = retrieveAsOpt<Element_Struct>(asisMap, stmt.Return_Expression);
          SgExpression*   retval = exprel ? &getExpr(*exprel, ctx) : NULL;
          SgStatement&    sgnode = SG_DEREF( sb::buildReturnStmt(retval) );

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }
      
      case An_Accept_Statement:                 // 9.5.2
        {
          SgExpression&           entryref = getExprID(stmt.Accept_Entry_Direct_Name, ctx);
          SgExpression&           idx      = getExprID_opt(stmt.Accept_Entry_Index, ctx);
          SgAdaAcceptStmt&        sgnode   = mkAdaAcceptStmt(entryref, idx); 
          
          completeStmt(sgnode, elem, ctx);
        
          ElemIdRange             params   = idRange(stmt.Accept_Parameters);
          AstContext              parmctx  = ctx.scope(SG_DEREF(sgnode.get_parameterScope()));

          traverseIDs(params, asisMap, ParmlistCreator(sgnode, parmctx));
          
          ElemIdRange             stmts   = idRange(stmt.Accept_Body_Statements);
          
          if (stmts.empty())
          {
            SgStatement&          noblock = mkNullStmt();
            
            linkParentChild(sgnode, noblock, &SgAdaAcceptStmt::set_body);
          }
          else
          {
            SgBasicBlock&         block   = mkBasicBlock();
            
            linkParentChild(sgnode, as<SgStatement>(block), &SgAdaAcceptStmt::set_body);
            traverseIDs(stmts, asisMap, StmtCreator(parmctx.scope(block)));
          }
          
          /* unused fields:
              Pragma_Element_ID_List       Pragmas;
              Element_ID                   Corresponding_End_Name;
              bool                         Is_Name_Repeated;
              Statement_List               Accept_Body_Exception_Handlers;
              Declaration_ID               Corresponding_Entry;
          */
          break;
        }

      case A_Raise_Statement:                   // 11.3
        {
          SgExpression&   raised = getExprID(stmt.Raised_Exception, ctx);
          SgStatement&    sgnode = mkRaiseStmt(raised);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Associated_Message;
          */
          break;
        }
        
        
      //|A2005 start
      case An_Extended_Return_Statement:        // 6.5
      //|A2005 end

      case Not_A_Statement: /* break; */        // An unexpected element
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
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }

  std::string
  getName(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Defining_Name);

    return std::string(elem.The_Union.Defining_Name.Defining_Name_Image);
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
          logTrace() << "A " << (adaUnit.Unit_Kind == A_Function ? "function" : "procedure")
                     << PrnUnitHeader(adaUnit)
                     << "\n " << adaUnit.Corresponding_Parent_Declaration << " (Corresponding_Parent_Declaration)"
                     << "\n " << adaUnit.Corresponding_Body << " (Corresponding_Body)"
                     << std::endl;

          ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(range, asisMap, ElemCreator(ctx));
          handleElementID(adaUnit.Unit_Declaration, ctx);
          
          /* unused optional elems:
               Unit_ID             Corresponding_Parent_Declaration
               Unit_ID             Corresponding_Declaration;
               Unit_ID             Corresponding_Body;
               Unit_List           Subunits;
          */          
          /* break; */
          break;
        }

      case A_Function_Body:
      case A_Procedure_Body:
        {
          logTrace() << "A " << (adaUnit.Unit_Kind == A_Function_Body ? "function" : "procedure") << " body"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;
          ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(range, asisMap, ElemCreator(ctx));
          handleElementID(adaUnit.Unit_Declaration, ctx);
          
          /* unused optional elems:
               Unit_ID             Corresponding_Parent_Declaration;
               Unit_ID             Corresponding_Declaration;
               Unit_List           Subunits;
          */
          
          /* break; */
          break;
        }

      case A_Package:
        {
          logTrace() << "A package"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;
/*
          SgScopeStatement&     outer     = ctx.scope();
          SgAdaPackageSpecDecl& pkgdecl   = mkAdaPackageSpecDecl(adaUnit.Unit_Full_Name, outer);
          SgAdaPackageSpec&     pkgspec   = SG_DEREF(pkgdecl.get_definition());
          
          recordNode(asisUnits, adaUnit.ID, pkgdecl);

          outer.append_statement(&pkgdecl);
*/
          ElemIdRange           elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange           unitRange = idRange(adaUnit.Corresponding_Children);

          if (elemRange.size() || unitRange.size())
          {
            logWarn() << "A package"
                      << PrnUnitHeader(adaUnit)
                      << "  elems# " << elemRange.size() << std::endl
                      << "   subs# " << unitRange.size() << std::endl;

            traverseIDs(elemRange, asisMap, ElemCreator(ctx));
            traverseIDs(unitRange, asisMap, UnitCreator(ctx));
            //~ traverseIDs(unitRange, asisMap, UnitCreator(ctx.scope(pkgspec)));
          }

          //~ handleElementID(adaUnit.Unit_Declaration, ctx.scope(pkgspec));
          handleElementID(adaUnit.Unit_Declaration, ctx);
          
          /* unused fields:                 
          */
          break;
        }
        
      case A_Package_Body:
        {
          logTrace() << "A package body"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;
                     
          //~ SgScopeStatement&     outer   = ctx.scope();
          //~ Unit_ID               specID  = adaUnit.Corresponding_Declaration;
          //~ SgAdaPackageSpecDecl& specdcl = retrieveNodeAs<SgAdaPackageSpecDecl>(asisUnits, specID);
          //~ SgAdaPackageBodyDecl& pkgdecl = mkAdaPackageBodyDecl(specdcl, outer);
          //~ SgAdaPackageBody&     pkgbody = SG_DEREF(pkgdecl.get_definition());

          //~ outer.append_statement(&pkgdecl);

          ElemIdRange           elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange           unitRange = idRange(adaUnit.Corresponding_Children);
          
          if (elemRange.size() || unitRange.size())
          {
            logWarn() << "A package body "
                      << PrnUnitHeader(adaUnit)
                      << "  elems# " << elemRange.size() << std::endl
                      << "   subs# " << unitRange.size() << std::endl;

            traverseIDs(elemRange, asisMap, ElemCreator(ctx));
            traverseIDs(unitRange, asisMap, UnitCreator(ctx));
            //~ traverseIDs(unitRange, asisMap, UnitCreator(ctx.scope(pkgbody)));
          }

          handleElementID(adaUnit.Unit_Declaration, ctx);
          break;
        }

      case Not_A_Unit: /* break; */
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

      default:
        logWarn() << "unit kind unhandled: " << adaUnit.Unit_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }
  
  void initializeAdaTypes(SgGlobal& global)
  {
    SgAdaPackageSpec& hiddenScope = mkBareNode<SgAdaPackageSpec>();
    
    hiddenScope.set_parent(&global);
    
    adaTypes[std::string("Integer")]   = sb::buildIntType();  
    adaTypes[std::string("Character")] = sb::buildCharType();  
    
    // \todo items
    adaTypes[std::string("Float")]     = sb::buildFloatType();  // Float is a subtype of Real
    adaTypes[std::string("Positive")]  = sb::buildIntType();    // Positive is a subtype of int  
    adaTypes[std::string("Natural")]   = sb::buildIntType();    // Natural is a subtype of int  
    adaTypes[std::string("Boolean")]   = sb::buildBoolType();   // Boolean is an enumeration of True and False
    
    adaTypes[std::string("Exception")] = sb::buildOpaqueType("Exception", &hiddenScope);  
  }
}

void secondConversion(Nodes_Struct& headNodes, SgSourceFile* file)
{
  ROSE_ASSERT(file);

  logInit();
  logInfo() << "Building ROSE AST .." << std::endl;

  Unit_Struct_List_Struct* adaLimit = 0;
  Unit_Struct_List_Struct* adaUnit  = headNodes.Units;
  SgGlobal&                astScope = SG_DEREF(file->get_globalScope());

  initializeAdaTypes(astScope);
  traverse(adaUnit, adaLimit, UnitCreator(AstContext(astScope)));

  logTrace() << "Generating DOT file: " << "adaTypedAst.dot" << std::endl;
  generateDOT(&astScope, "adaTypedAst");

  file->set_processedToIncludeCppDirectivesAndComments(false);
  logInfo() << "Building ROSE AST done" << std::endl;
}



}
