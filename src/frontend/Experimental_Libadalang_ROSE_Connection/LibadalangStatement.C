#include "sage3basic.h"
#include "sageBuilder.h"
#include "sageInterfaceAda.h"
#include "sageGeneric.h"

#include <numeric>

#include "LibadalangStatement.h"

#include "Libadalang_to_ROSE.h"
#include "LibadalangExpression.h"
#include "AdaMaker.h"
#include "LibadalangType.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;



namespace Libadalang_ROSE_Translation
{

  /// gets the body of a function declaration \ref defdcl
  /// \pre defdcl is the defining declaration
  SgBasicBlock& functionBody(SgFunctionDeclaration& defdcl)
  {
    SgFunctionDefinition* def = isSgFunctionDefinition(defdcl.get_definition());

    return SG_DEREF(SG_DEREF(def).get_body());
  }

  /// if \ref isPrivate \ref dcl's accessibility is set to private;
  /// otherwise nothing.
  void
  privatize(SgDeclarationStatement& dcl, bool isPrivate)
  {
    if (!isPrivate) return;

    dcl.get_declarationModifier().get_accessModifier().setPrivate();
  }

  /// completes statements by setting source locations, parent node,
  /// adding labels (if needed)...
  /// @{
  void
  completeStmt(SgStatement& sgnode, ada_base_entity* lal_stmt, AstContext ctx)
  {
    //ADA_ASSERT (elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, lal_stmt, ctx);
    sgnode.set_parent(&ctx.scope());

    //We are handling labels differently
    //SgStatement&      sgn  = labelIfNeeded(sgnode, lal_stmt, ctx);

    ctx.appendStatement(sgnode);
  }

  /// @}

  /// sets the override flag in \ref dcl
  void setOverride(SgDeclarationStatement& dcl, bool isOverride)
  {
    SgDeclarationModifier& sgmod = dcl.get_declarationModifier();

    if (isOverride) sgmod.setOverride(); else sgmod.unsetOverride();
  }

  //~ using TryBlockNodes = std::pair<SgTryStmt*, std::reference_wrapper<SgBasicBlock> >;
  using TryBlockNodes = std::pair<SgTryStmt*, std::reference_wrapper<SgScopeStatement> >;

  SgScopeStatement&
  createBlockIfNeeded(bool newStatementBlock, AstContext ctx)
  {
    if (!newStatementBlock) return ctx.scope();

    SgBasicBlock& newblk = mkBasicBlock();

    ctx.appendStatement(newblk);
    return newblk;
  }

  /// if handlers are present, create an inner try block
  /// \param hasHandlers true if a block has handlers
  /// \param requiresStmtBlock true if a new block is required
  ///        even when no exception handlers are present. (e.g., for package body code section).
  /// \param outer the current scope, in which the try or block will be created.
  /// \returns a pair where second indicates the scope/block in which new statements will reside;
  ///                       first is an optional try statement (can be nullptr if no handlers exist)
  TryBlockNodes
  createTryOrBlockIfNeeded(bool hasHandlers, bool requiresStmtBlock, AstContext ctx)
  {
    if (!hasHandlers) return TryBlockNodes{nullptr, createBlockIfNeeded(requiresStmtBlock, ctx)};

    SgBasicBlock& tryBlock = mkBasicBlock();
    SgTryStmt&    tryStmt  = mkTryStmt(tryBlock);

    ctx.appendStatement(tryStmt);

    return TryBlockNodes{&tryStmt, tryBlock};
  }

  using BlockHandler = std::function<void(ada_base_entity*, SgScopeStatement&, AstContext ctx)>;
  using ExceptionHandlerHandler = std::function<void(ada_base_entity*, SgScopeStatement&, SgTryStmt&, AstContext ctx)>;

  // a simple block handler just traverses the statement list and adds them to the \ref blk.
  void simpleBlockHandler(ada_base_entity* lal_stmt_list, SgScopeStatement& blk, AstContext ctx)
  {
    int                 range = ada_node_children_count(lal_stmt_list);

    for(int i = 0; i < range; i++){
        ada_base_entity lal_stmt;
        ada_node_child(lal_stmt_list, i, &lal_stmt);
        handleStmt(&lal_stmt, ctx.scope(blk));
    }
  }

  // at some point loops, labels, gotos need to be patched up. In this case, we do that at the
  //   end of a routine through the use of the LoopAndLabelManager.
  void routineBlockHandler(ada_base_entity* lal_stmt_list, SgScopeStatement& blk, AstContext ctx)
  {
    //LabelAndLoopManager lblmgr;

    simpleBlockHandler(lal_stmt_list, blk, ctx/*.labelsAndLoops(lblmgr)*/);
  }

  void simpleExceptionBlockHandler(ada_base_entity* lal_handlers, SgScopeStatement& blk, SgTryStmt& trystmt, AstContext ctx)
  {
    int                 range = ada_node_children_count(lal_handlers);

    for(int i = 0; i < range; i++){
        ada_base_entity lal_handler;
        ada_node_child(lal_handlers, i, &lal_handler);
        handleExceptionHandler(&lal_handler, trystmt, ctx.scope(blk));
    }
  }

  // at some point loops, labels, gotos need to be patched up. In this case, we do that at the
  //   end of a routine through the use of the LoopAndLabelManager
  void routineExceptionBlockHandler(ada_base_entity* lal_handlers, SgScopeStatement& blk, SgTryStmt& trystmt, AstContext ctx)
  {
    //LabelAndLoopManager lblmgr;

    simpleExceptionBlockHandler(lal_handlers, blk, trystmt, ctx/*.labelsAndLoops(lblmgr)*/);
  }



  // completes any block with exception handlers and pragmas attached
  void completeHandledBlock( ada_base_entity* lal_stmts,
                             ada_base_entity* lal_exceptions,
                             ada_base_entity* lal_pragmas,
                             BlockHandler blockHandler,
                             ExceptionHandlerHandler exhandlerHandler,
                             SgScopeStatement& dominantBlock,
                             bool requiresStatementBlock,
                             AstContext ctx,
                             AstContext::PragmaContainer pendingPragmas = {} // moved in to capture pragmas seen earlier
                           )
  {
    using ScopeSequence = std::vector<SgScopeStatement*>;

    int               hndlrs    = ada_node_children_count(lal_exceptions);


    TryBlockNodes     trydata   = createTryOrBlockIfNeeded( hndlrs > 0, requiresStatementBlock, ctx.scope(dominantBlock));
    SgTryStmt*        trystmt   = trydata.first;
    SgScopeStatement& stmtblk   = trydata.second;
    AstContext        pragmaCtx = ctx.pragmas(pendingPragmas);
    ScopeSequence     activeScopes = { &dominantBlock };

    if (&dominantBlock != &stmtblk)
      activeScopes.push_back(&stmtblk);

    blockHandler(lal_stmts, stmtblk, pragmaCtx);

    if (trystmt)
    {
      exhandlerHandler(lal_exceptions, dominantBlock, *trystmt, pragmaCtx);

      //computeSourceRangeFromChildren(SG_DEREF(trystmt->get_body()));
      //computeSourceRangeFromChildren(SG_DEREF(trystmt->get_catch_statement_seq_root()));
      //computeSourceRangeFromChildren(*trystmt);
    }

    //processAndPlacePragmas(lal_pragmas, std::move(activeScopes), pragmaCtx.scope(dominantBlock));
  }

  // completes any block with declarative items and exception handlers and pragmas attached
  void completeDeclarationsWithHandledBlock( ada_base_entity* lal_decls,
                                             ada_base_entity* lal_stmts,
                                             ada_base_entity* lal_exceptions,
                                             ada_base_entity* lal_pragmas,
                                             BlockHandler blockHandler,
                                             ExceptionHandlerHandler exhandlerHandler,
                                             SgScopeStatement& dominantBlock,
                                             bool requiresStatementBlock,
                                             AstContext ctx
                                           )
  {
    //pragma stuff
    using PragmaContainer = AstContext::PragmaContainer;
    PragmaContainer pendingPragmas;
    AstContext      pragmaCtx = ctx.pragmas(pendingPragmas);

    //Get the number of decls
    //Assuming lal_decls is a list of some kind, we can just call ada_node_children_count
    int             range = ada_node_children_count(lal_decls);

    for(int i =0; i < range; i++){
         ada_base_entity lal_decl_child;
         ada_node_child(lal_decls, i, &lal_decl_child);
         handleElement(&lal_decl_child, pragmaCtx.scope(dominantBlock));
    }

    completeHandledBlock( lal_stmts,
                          lal_exceptions,
                          lal_pragmas,
                          blockHandler,
                          exhandlerHandler,
                          dominantBlock,
                          requiresStatementBlock,
                          pragmaCtx,
                          std::move(pendingPragmas)
                        );
  }

  void completeRoutineBody(ada_base_entity* lal_decl, SgBasicBlock& declblk, AstContext ctx)
  {
    //Get the decls, stmts, exceptions, & pragmas of this routine body
    ada_base_entity lal_decls, lal_stmts, lal_exceptions, lal_pragmas;

    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_decl);

    if(kind == ada_subp_body){
        ada_subp_body_f_decls(lal_decl, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_subp_body_f_stmts(lal_decl, &lal_handled_stmts);
        ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
        ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
        //lal_pragmas = ???; //TODO Figure out pragmas
    } else {
        //TODO
    }
    

    completeDeclarationsWithHandledBlock( &lal_decls,
                                          &lal_stmts,
                                          &lal_exceptions,
                                          &lal_pragmas,
                                          routineBlockHandler,
                                          routineExceptionBlockHandler,
                                          declblk,
                                          false /* same block for declarations and statements */,
                                          ctx
                                        );
  }

  SgFunctionDeclaration&
  createFunDef( SgFunctionDeclaration* nondef,
                const std::string& name,
                SgScopeStatement& scope,
                SgType& rettype,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
  {
    // PP (20/1/23): why do we use the nondefining function's scope?
    //               for one, ROSE scope fixup currently unifies the scopes.
    //               see also SCOPE_COMMENT_1 .
    return nondef ? mkProcedureDecl(*nondef, SG_DEREF(nondef->get_scope()), rettype, std::move(complete))
                  : mkProcedureDecl(name,    scope, rettype, std::move(complete));
  }

  SgFunctionDeclaration&
  createFunDcl( SgFunctionDeclaration* nondef,
                const std::string& name,
                SgScopeStatement& scope,
                SgType& rettype,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
  {
    // we use the nondefining function's scope if available
    // see SCOPE_COMMENT_1
    return nondef ? mkProcedureDecl_nondef(*nondef, SG_DEREF(nondef->get_scope()), rettype, std::move(complete))
                  : mkProcedureDecl_nondef(name,    scope, rettype, std::move(complete));
  }

  using TypeModifierFn = std::function<SgType&(SgType&)>;

  SgType& tyIdentity(SgType& ty) { return ty; }

  ada_base_entity*
  secondaryDiscriminants(ada_base_entity* lal_decl, ada_base_entity* first_discriminant, AstContext)
  {
    const bool useThisDecl = (  ada_node_is_null(first_discriminant)
                             );

    if (useThisDecl) return nullptr;
    //TODO Where should the 2nd discr come from?
    return nullptr;
    //Element_Struct& firstDecl = retrieveElem(elemMap(), decl.Corresponding_Type_Declaration);
    //ADA_ASSERT(firstDecl.Element_Kind == A_Declaration);

    //return firstDecl.The_Union.Declaration.Discriminant_Part;
  }

namespace {

  // may need special processing for pragmas in body
  using DeferredPragmaBodyCompletion = std::function<void(AstContext::PragmaContainer)>;

  void
  processInheritedSubroutines( SgNamedType* derivedTy,
                               ada_base_entity* tydef,
                               AstContext ctx
                             )
  {
    /*processInheritedSubroutines( SG_DEREF(derivedTy),
                                 idRange(tydef.Implicit_Inherited_Subprograms),
                                 idRange(tydef.Implicit_Inherited_Declarations),
                                 ctx
                               );*/ //TODO How does Libadalang pass the inherited subps/decls?
  }

  template <class SageTypeDeclStmt>
  void
  processInheritedSubroutines( ada_base_entity* tydef,
                               SageTypeDeclStmt& tyDecl,
                               AstContext ctx
                             )
  {
    processInheritedSubroutines(tyDecl.get_type(), tydef, ctx);
  }

  void
  processInheritedElementsOfDerivedTypes(TypeData& ty, SgDeclarationStatement& dcl, AstContext ctx)
  {
    ada_base_entity* tydef = ty.definitionStruct();

    ada_node_kind_enum kind = ada_node_kind(tydef);

    if (  (kind != ada_derived_type_def)
       )
    {
      return;
    }

    if (SgTypedefDeclaration* derivedTypeDcl = isSgTypedefDeclaration(&dcl))
      processInheritedSubroutines(tydef, *derivedTypeDcl, ctx);
    else if (SgClassDeclaration* classTypeDcl = isSgClassDeclaration(&dcl))
      processInheritedSubroutines(tydef, *classTypeDcl, ctx);
    /*else if (SgEnumDeclaration* derivedEnumDcl = isSgEnumDeclaration(&dcl))
      processInheritedEnumValues(tydef, *derivedEnumDcl, ctx);*/ //TODO
    else if (SgAdaDiscriminatedTypeDecl* discrTypeDcl = isSgAdaDiscriminatedTypeDecl(&dcl))
      processInheritedSubroutines(tydef, *discrTypeDcl, ctx);
    else
      logError() << "Unknown SgNode in processInheritedElementsOfDerivedTypes.\n";
  }

  void completeDiscriminatedDecl( int type_hash,
                                  ada_base_entity* lal_element,
                                  SgAdaDiscriminatedTypeDecl& sgnode,
                                  SgDeclarationStatement& sgdecl,
                                  bool isPrivate,
                                  AstContext ctx
                                )
  {
    sg::linkParentChild(sgnode, sgdecl, &SgAdaDiscriminatedTypeDecl::set_discriminatedDecl);
    // sgdecl.set_scope(sgnode.get_discriminantScope());

    privatize(sgnode, isPrivate);
    recordNode(libadalangTypes(), type_hash, sgdecl, true /* replace */);
    attachSourceLocation(sgnode, lal_element, ctx);

    // \todo link nondef and def SgAdaDiscriminatedTypeDecl
  }

  SgAdaDiscriminatedTypeDecl&
  createDiscriminatedDeclID(ada_base_entity* lal_element, ada_base_entity* secondary, AstContext ctx)
  {
    //ADA_ASSERT (elem.Element_Kind == A_Definition);
    //logKind("A_Definition", elem.ID);

    // many definitions are handled else where
    // here we want to convert the rest that can appear in declarative context

    SgScopeStatement&           scope  = ctx.scope();
    SgAdaDiscriminatedTypeDecl& sgnode = mkAdaDiscriminatedTypeDecl(scope);

    ctx.appendStatement(sgnode);

    if(!ada_node_is_null(lal_element) && ada_node_kind(lal_element) == ada_known_discriminant_part)
    {
      //logKind("A_Known_Discriminant_Part", elem.ID);
      //TODO Figure out discriminants
      /*SgScopeStatement&       scope         = SG_DEREF(sgnode.get_discriminantScope());
      ElemIdRange             discriminants = idRange(def.The_Union.The_Known_Discriminant_Part.Discriminants);
      ElemIdRange             secondaryIDs  = secondaryKnownDiscrimnants(secondary, ctx);
      std::vector<Element_ID> secondaries   = reverseElems(flattenNameLists(secondaryIDs, ctx));

      traverseIDs(discriminants, elemMap(), DiscriminantCreator{sgnode, std::move(secondaries), ctx.scope(scope)});*/
    }
    else
    {
      //ADA_ASSERT (def.Definition_Kind == An_Unknown_Discriminant_Part);

      //logKind("An_Unknown_Discriminant_Part", elem.ID);
    }

    return sgnode;
  }

  /// sets the separate flag in \ref dcl
  void setAdaSeparate(SgDeclarationStatement& dcl, bool isSeparate)
  {
    SgDeclarationModifier& sgmod = dcl.get_declarationModifier();

    if (isSeparate) sgmod.setAdaSeparate(); else sgmod.unsetAdaSeparate();
  }

} //end anonymous namespace

SgAdaDiscriminatedTypeDecl*
createDiscriminatedDeclID_opt(ada_base_entity* primary, ada_base_entity* secondary, AstContext ctx)
{
  if (primary != nullptr && ada_node_is_null(primary))
  {
    if (secondary != nullptr && !ada_node_is_null(secondary)) logFlaw() << "Unexpected secondary discriminants" << std::endl;

    return nullptr;
  }

  return &createDiscriminatedDeclID(primary, secondary, ctx);
}

/// creates an initializer for a variable/parameter declaration if needed
/// \param lst the subset of completed variable declarations
/// \param exp the original initializing expression
/// \param ctx the context
/// \details
///    consider a variable or parameter declaration of the form.
///      a,b : Integer := InitExpr
///    The ROSE AST looks like:
///      int a = InitExpr, int b = InitExpr
SgExpression*
createInit(SgInitializedNamePtrList& lst, SgExpression* exp, AstContext ctx)
{
  // the first variable declarations gets the original initializer
  if ((exp == nullptr) || lst.empty()) return exp;

  // \todo consider rebuilding from the ASIS expression
  exp = si::deepCopy(exp);

  // \todo use a traversal to set all children nodes to compiler generated
  markCompilerGenerated(SG_DEREF(exp));
  return exp;
}

/// creates a sequence of initialized names for all names \ref names.
/// \param m        a map that maintains mappings between Asis IDs and variables/parameters
/// \param names    the list of Asis names
/// \param dcltype  the type of all initialized name
/// \param initexpr the initializer (if it exists) that will be cloned for each
///                 of the initialized names.
SgInitializedNamePtrList
constructInitializedNamePtrList( AstContext ctx,
                                 map_t<int, SgInitializedName*>& m,
                                 ada_base_entity* lal_name_list,
                                 SgType& dcltype,
                                 SgExpression* initexpr,
                                 std::vector<int>& secondaries
                               )
{
  SgInitializedNamePtrList lst;

  int count = ada_node_children_count(lal_name_list);
  for(int i = 0; i < count; ++i)
  {
    ada_base_entity obj;

    if (ada_node_child(lal_name_list, i, &obj) == 0){
      logError() << "Error while getting a child in constructInitializedNamePtrList.\n";
      return lst;
    }
    if(!ada_node_is_null(&obj)){
      //Get the name of this decl
      ada_base_entity    identifier;
      ada_defining_name_f_name(&obj, &identifier);
      ada_symbol_type    p_canonical_text;
      ada_text           ada_canonical_text;
      ada_single_tok_node_p_canonical_text(&identifier, &p_canonical_text);
      ada_symbol_text(&p_canonical_text, &ada_canonical_text);
      const std::string  name = ada_text_to_locale_string(&ada_canonical_text);
      SgExpression*      init = createInit(lst, initexpr, ctx);
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, init);

      attachSourceLocation(dcl, &obj, ctx);

      lst.push_back(&dcl);
      int hash = hash_node(&obj);
      recordNonUniqueNode(m, hash, dcl, true /* overwrite existing entries if needed */);

      //~ logError() << name << " = " << id << std::endl;

      if (!secondaries.empty())
      {
        //~ logError() << name << "' = " << secondaries.back() << std::endl;
        recordNonUniqueNode(m, secondaries.back(), dcl, true /* overwrite existing entries if needed */);
        secondaries.pop_back();
      }
    }
  }

  return lst;
}

SgInitializedNamePtrList
constructInitializedNamePtrList( AstContext ctx,
                                 map_t<int, SgInitializedName*>& m,
                                 ada_base_entity* lal_name_list,
                                 SgType& dcltype,
                                 SgExpression* initexpr
                               )
{
  std::vector<int> dummy;

  return constructInitializedNamePtrList(ctx, m, lal_name_list, dcltype, initexpr, dummy);
}

/// converts a parameter mode to its ROSE representation
SgTypeModifier
getMode(ada_base_entity* lal_mode)
{
  //Get the kind of this node
  ada_node_kind_enum kind;
  kind = ada_node_kind(lal_mode);

  SgTypeModifier res;

  switch(kind)
  {
    case ada_mode_default:
      Libadalang_ROSE_Translation::logKind("ada_mode_default", kind);
      res.setDefault();
      break;

    case ada_mode_in:
      Libadalang_ROSE_Translation::logKind("ada_mode_in", kind);
      res.setIntent_in();
      break;

    case ada_mode_out:
      Libadalang_ROSE_Translation::logKind("ada_mode_out", kind);
      res.setIntent_out();
      break;

    case ada_mode_in_out:
      Libadalang_ROSE_Translation::logKind("ada_mode_in_out", kind);
      res.setIntent_inout();
      break;

    default:
      //ADA_ASSERT(false);
      break;
  }

  return res;
}

/// creates a ROSE expression for an Asis declaration's initializer expression
/// returns null, if no declaration exists.
/// \param expectedType a type that is carried over from a lhs constant declaration
SgExpression*
getVarInit(ada_base_entity* lal_decl, SgType* /*expectedType*/, AstContext ctx)
{
  //Check the kind
  ada_node_kind_enum kind;
  kind = ada_node_kind(lal_decl);

  ada_base_entity default_expr;
  if(kind == ada_object_decl){
    ada_object_decl_f_default_expr(lal_decl, &default_expr);
  } else {
    return nullptr;
  }

  return &getExpr(&default_expr, ctx);

}

SgType&
getVarType(ada_base_entity* lal_decl, AstContext ctx)
{
  /*ADA_ASSERT (  decl.Declaration_Kind == A_Variable_Declaration
             || decl.Declaration_Kind == A_Constant_Declaration
             || decl.Declaration_Kind == A_Component_Declaration
             || decl.Declaration_Kind == A_Deferred_Constant_Declaration
             );*/
  ada_base_entity subtype_indication;
  ada_object_decl_f_type_expr(lal_decl, &subtype_indication);

  return getDefinitionType(&subtype_indication, ctx);
}

  //
  // helper function for combined handling of variables and constant declarations

namespace {
  void
  setModifiers(SgDeclarationStatement& dcl, bool abstract, bool limited, bool tagged)
  {
    SgDeclarationModifier& mod = dcl.get_declarationModifier();

    if (abstract) mod.setAdaAbstract();
    if (limited)  mod.setAdaLimited();
    if (tagged)   mod.setAdaTagged();

    //~ logError() << typeid(dcl).name() << " " << abstract << limited << tagged
               //~ << std::endl;
  }

  void
  setModifiers(SgDeclarationStatement& dcl, const TypeData& info)
  {
    setModifiers(dcl, info.isAbstract(), info.isLimited(), info.isTagged());
  }

  template <class libadalangStruct>
  void
  setModifiers(SgDeclarationStatement& dcl, const libadalangStruct& info)
  {
    setModifiers(dcl, info.Has_Abstract, info.Has_Limited, info.Has_Tagged);
  }

  template <class libadalangStruct>
  void
  setModifiersUntagged(SgDeclarationStatement& dcl, const libadalangStruct& info)
  {
    setModifiers(dcl, info.Has_Abstract, info.Has_Limited, false);
  }

  void
  setAbstractModifier(SgDeclarationStatement& dcl, ada_base_entity* info)
  {
    bool has_abstract = false;
    ada_node_kind_enum kind = ada_node_kind(info);
    ada_base_entity lal_abstract;
    switch(kind){
      case ada_incomplete_tagged_type_decl:
        {
          ada_incomplete_tagged_type_decl_f_has_abstract(info, &lal_abstract);
          ada_node_kind_enum abstract_kind = ada_node_kind(&lal_abstract);
          has_abstract = (abstract_kind == ada_abstract_present);
          break;
        }
      case ada_derived_type_def:
        {
          ada_derived_type_def_f_has_abstract(info, &lal_abstract);
          ada_node_kind_enum abstract_kind = ada_node_kind(&lal_abstract);
          has_abstract = (abstract_kind == ada_abstract_present);
          break;
        }
      case ada_private_type_def:
        {
          ada_private_type_def_f_has_abstract(info, &lal_abstract);
          ada_node_kind_enum abstract_kind = ada_node_kind(&lal_abstract);
          has_abstract = (abstract_kind == ada_abstract_present);
          break;
        }
      case ada_record_type_def:
        {
          ada_record_type_def_f_has_abstract(info, &lal_abstract);
          ada_node_kind_enum abstract_kind = ada_node_kind(&lal_abstract);
          has_abstract = (abstract_kind == ada_abstract_present);
          break;
        }
      case ada_abstract_subp_decl:
        {
          has_abstract = true;
          break;
        }
      default:
        logWarn() << "Node kind " << kind << " in setAbstractModifier has no abstract.\n";
    }
    setModifiers(dcl, has_abstract, false, false);
  }

  SgClassDeclaration&
  createRecordDecl( const std::string& name,
                    SgClassDefinition& def,
                    SgScopeStatement& scope,
                    SgDeclarationStatement* nondefdcl
                  )
  {
    if (SgClassDeclaration* recdcl = isSgClassDeclaration(nondefdcl))
      return mkRecordDecl(*recdcl, def, scope);

    // if nondefdcl is set, it must be a SgClassDeclaration
    if (nondefdcl)
      logFlaw() << name << " " << typeid(*nondefdcl).name() << " not a class declaration!"
                << std::endl;

    //ADA_ASSERT (!nondefdcl);
    return mkRecordDecl(name, def, scope);
  }

  // retrieves package spec declarations, similar to getFunctionDeclaration
  SgAdaPackageSpecDecl* getAdaPackageSpecDecl(SgDeclarationStatement* dcl)
  {
    if (SgAdaPackageSpecDecl* spcdcl  = isSgAdaPackageSpecDecl(dcl))
      return spcdcl;

    if (SgAdaGenericDecl* generic = isSgAdaGenericDecl(dcl))
      return isSgAdaPackageSpecDecl(generic->get_declaration());

    return nullptr;
  }

  /// creates a ROSE declaration depending on the provided type/definition
  struct TypeDeclMaker : sg::DispatchHandler<SgDeclarationStatement*>
  {
      typedef sg::DispatchHandler<SgDeclarationStatement*> base;

      TypeDeclMaker( const std::string& name,
                     SgScopeStatement& scope,
                     TypeData basis,
                     SgDeclarationStatement* incompl
                   )
      : base(nullptr), dclname(name), dclscope(scope), foundation(basis), incomplDecl(incompl)
      {}

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(SgAdaFormalTypeDecl& n)
      {
        //ADA_ASSERT (n.get_scope() == &dclscope);

        res = &n;
      }

      void handle(SgAdaDiscriminatedTypeDecl& n)
      {
        res = &n;
      }

      void handle(SgType& n)
      {
        res = &mkTypeDecl(dclname, n, dclscope);

        if (incomplDecl)
        {
          //~ logError() << dclname << " " << typeid(*res).name() << typeid(*incomplDecl).name()
                     //~ << std::endl;
          res->set_firstNondefiningDeclaration(incomplDecl);
          res->set_definingDeclaration(res);
          incomplDecl->set_definingDeclaration(res);
        }
      }

      void handle(SgAdaDerivedType& n)
      {
        SgEnumDeclaration* enmdcl = si::Ada::baseEnumDeclaration(n);

        if (enmdcl == nullptr)
          return handle(sg::asBaseType(n));

        SgEnumDeclaration& derivedEnum = mkEnumDefn(dclname, dclscope);

        derivedEnum.set_adaParentType(&n);
        res = &derivedEnum;
      }

      void handle(SgEnumDeclaration& n)
      {
        //ADA_ASSERT (n.get_scope() == &dclscope);

        res = &n;
      }

      void handle(SgClassDefinition& n)
      {
        SgClassDeclaration& rec = createRecordDecl(dclname, n, dclscope, incomplDecl);

        setModifiers(rec, foundation);
        res = &rec;
      }

      operator SgDeclarationStatement& () const
      {
        return SG_DEREF(res);
      }

    private:
      std::string             dclname;
      SgScopeStatement&       dclscope;
      TypeData                foundation;
      SgDeclarationStatement* incomplDecl;
  };

  SgDeclarationStatement&
  handleNumVarCstDecl( ada_base_entity* lal_element,
                       AstContext ctx,
                       bool isPrivate,
                       SgType& dclType,
                       SgType* expectedType = nullptr
                     )
  {
    ada_base_entity          ids;
    ada_object_decl_f_ids(lal_element, &ids);
    SgScopeStatement&        scope    = ctx.scope();
    //
    // https://www.adaic.com/resources/add_content/standards/05rm/html/RM-3-3-1.html#S0032
    // $7: Any declaration that includes a defining_identifier_list with more than one defining_identifier
    //     is equivalent to a series of declarations each containing one defining_identifier from the list,
    //     with the rest of the text of the declaration copied for each declaration in the series, in the
    //     same order as the list. The remainder of this International Standard relies on this equivalence;
    //     explanations are given for declarations with a single defining_identifier.
    // => clone the initializing expression instead of creating references to the first one..
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                         libadalangVars(),
                                                                         &ids,
                                                                         dclType,
                                                                         getVarInit(lal_element, expectedType, ctx)
                                                                       );
    SgVariableDeclaration&   sgnode   = mkVarDecl(dclnames, scope);

    attachSourceLocation(sgnode, lal_element, ctx);
    privatize(sgnode, isPrivate);
    ctx.appendStatement(sgnode);
    return sgnode;
  }

  SgDeclarationStatement&
  handleNumberDecl(ada_base_entity* lal_element, AstContext ctx, bool isPrivate, SgType& numty, SgType& expctty)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    return handleNumVarCstDecl(lal_element, ctx, isPrivate, cstty, &expctty);
  }

  SgDeclarationStatement&
  handleVarCstDecl( ada_base_entity* lal_element,
                    AstContext ctx,
                    bool isPrivate,
                    TypeModifierFn constMaker
                  )
  {
    SgType& basety = constMaker(getVarType(lal_element, ctx));

    ada_base_entity has_aliased;
    ada_object_decl_f_has_aliased(lal_element, &has_aliased);
    ada_node_kind_enum aliased_status = ada_node_kind(&has_aliased);
    const bool               aliased  = (aliased_status == ada_aliased_present);
    SgType& varty  = aliased ? mkAliasedType(basety) : basety;

    return handleNumVarCstDecl(lal_element, ctx, isPrivate, varty);
  }

  void nothingToComplete(AstContext::PragmaContainer pragmas)
  {
     //ADA_ASSERT(pragmas.empty());
  }

  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpec(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    logKind("A_Task_Definition", kind);

    //Task_Definition_Struct* tasknode = &def.The_Union.The_Task_Definition;
    SgAdaTaskSpec&          sgnode   = mkAdaTaskSpec();
    SgAdaTaskSpec*          nodePtr  = &sgnode;

    // sgnode.set_hasMembers(true);

    //Check if this task has private members
    ada_base_entity task_def;
    ada_task_type_decl_f_definition(lal_element, &task_def);
    ada_base_entity private_part;
    ada_task_def_f_private_part(&task_def, &private_part);
    sgnode.set_hasPrivate(!ada_node_is_null(&private_part));

    //Get the public_part
    ada_base_entity public_part;
    ada_task_def_f_public_part(&task_def, &public_part);
    ada_declarative_part_f_decls(&public_part, &public_part);

    if(!ada_node_is_null(&public_part)){
      int range = ada_node_children_count(&public_part);
      for(int i =0; i < range; i++){
        ada_base_entity lal_entry;
        ada_node_child(&public_part, i, &lal_entry);
        handleElement(&lal_entry, ctx.scope(*nodePtr));
      }
    }

    //Get the private part
    if(!ada_node_is_null(&private_part)){
      ada_declarative_part_f_decls(&private_part, &private_part);

      int range = ada_node_children_count(&private_part);
      for(int i =0; i < range; i++){
        ada_base_entity lal_entry;
        ada_node_child(&private_part, i, &lal_entry);
        handleElement(&lal_entry, ctx.scope(*nodePtr), true /* private items */);
      }
    }

    /*auto deferred = [ctx,nodePtr,&task_def](AstContext::PragmaContainer pragmas) -> void
                    {
                      AstContext pragmaCtx = ctx.pragmas(pragmas);

                      // visible items
                      {
                        //Get the public_part
                        ada_base_entity public_part;
                        ada_task_def_f_public_part(&task_def, &public_part);
                        ada_declarative_part_f_decls(&public_part, &public_part);

                        if(!ada_node_is_null(&public_part)){
                          int range = ada_node_children_count(&public_part);
                          for(int i =0; i < range; i++){
                             ada_base_entity lal_entry;
                             ada_node_child(&public_part, i, &lal_entry);
                             handleElement(&lal_entry, pragmaCtx.scope(*nodePtr));
                          }
                        }
                      }

                      // private items
                      {
                        //Get the private_part
                        ada_base_entity private_part;
                        ada_task_def_f_private_part(&task_def, &private_part);
                        ada_declarative_part_f_decls(&private_part, &private_part);

                        if(!ada_node_is_null(&private_part)){
                          int range = ada_node_children_count(&private_part);
                          for(int i =0; i < range; i++){
                             ada_base_entity lal_entry;
                             ada_node_child(&private_part, i, &lal_entry);
                             handleElement(&lal_entry, pragmaCtx.scope(*nodePtr), true /* private items *//*);
                          }
                        }
                      }

                      //placePragmas({ nodePtr }, pragmaCtx.scope(*nodePtr)); //TODO pragmas
                    };*/

    //return std::make_pair(&sgnode, deferred);
    return std::make_pair(&sgnode, nothingToComplete);
  }

  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpec_opt(ada_base_entity* lal_element, AstContext ctx)
  {
    if(lal_element == nullptr || ada_node_is_null(lal_element)){
      return std::make_pair(&mkAdaTaskSpec(), nothingToComplete);
    }

    return getTaskSpec(lal_element, ctx);
  }
} //End unnamed namespace

/// converts an Asis parameter declaration to a ROSE paramter (i.e., variable)
///   declaration.
SgVariableDeclaration&
getParm(ada_base_entity* lal_param_spec, AstContext ctx)
{

  ada_base_entity defining_name_list;
  ada_param_spec_f_ids(lal_param_spec, &defining_name_list);

  ada_base_entity has_aliased;
  ada_param_spec_f_has_aliased(lal_param_spec, &has_aliased);
  ada_node_kind_enum aliased_status = ada_node_kind(&has_aliased);
  const bool               aliased  = (aliased_status == ada_aliased_present);

  ada_base_entity subtype_indication;
  ada_param_spec_f_type_expr(lal_param_spec, &subtype_indication);
  SgType&                  basety   = getDeclType(&subtype_indication, ctx);

  SgType&                  parmtype = aliased ? mkAliasedType(basety) : basety;

  SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                       libadalangVars(),
                                                                       &defining_name_list,
                                                                       parmtype,
                                                                       getVarInit(lal_param_spec, &parmtype, ctx)
                                                                     );

  ada_base_entity mode;
  ada_param_spec_f_mode(lal_param_spec, &mode);
  SgVariableDeclaration&   sgnode   = mkParameter(dclnames, getMode(&mode), ctx.scope());

  attachSourceLocation(sgnode, lal_param_spec, ctx);
  /* unused fields:
  */
  return sgnode;
}

void ParameterCompletion::operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope)
{
  if(!ada_node_is_null(range)){
    ada_base_entity param_list;
    ada_params_f_params(range, &param_list);
    int count = ada_node_children_count(&param_list);
    for (int i = 0; i < count; ++i)
    {
      ada_base_entity child;

      if (ada_node_child(&param_list, i, &child) == 0){
        logError() << "Error while getting a child in ParameterCompletion.\n";
        return;
      }
      if(!ada_node_is_null(&child)){
        SgVariableDeclaration& decl = getParm(&child, ctx.scope(parmscope));
        // in Ada multiple parameters can be declared
        //   within a single declaration.
        for (SgInitializedName* parm : decl.get_variables())
          lst.append_arg(parm);
      }
    }
  }
}

SgDeclarationStatement*
queryDecl(ada_base_entity* lal_element, AstContext /*ctx*/)
{
  ada_node_kind_enum kind = ada_node_kind(lal_element);

  ada_base_entity corresponding_decl;
  ada_expr_p_first_corresponding_decl(lal_element, &corresponding_decl);
  int decl_hash = hash_node(&corresponding_decl);

  SgDeclarationStatement* res = findFirst(libadalangDecls(), decl_hash); //, expr.Corresponding_Name_Definition);

  if((res == nullptr) && (kind == ada_identifier))
  {
    //res = findFirst(adaPkgs(), AdaIdentifier{expr.Name_Image}); //TODO adaPkgs
  }

  return res;
}

void handleStmt(ada_base_entity* lal_stmt, AstContext ctx)
  {
    //TODO Add more nodes
    using PragmaContainer = AstContext::PragmaContainer;
    
    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_stmt);

    ada_text kind_name;
    ada_kind_name(kind, &kind_name);
    std::string kind_name_string = ada_text_to_locale_string(&kind_name);
    logTrace()   << "handleStmt called on a " << kind_name_string << std::endl;

    /*if (elem.Element_Kind == A_Declaration)
    {
      handleDeclaration(elem, ctx);
      return;
    }

    if (elem.Element_Kind == A_Clause)
    {
      handleClause(elem, ctx);
      return;
    }

    if (elem.Element_Kind == A_Pragma)
    {
      handlePragma(elem, nullptr, ctx);
      return;
    }*/

    //~ logError() << ":: " << elem.Element_Kind << std::endl;
    //ADA_ASSERT (elem.Element_Kind == A_Statement);

    SgStatement*            assocstmt = nullptr;
    //Statement_Struct&       stmt = elem.The_Union.Statement; //Same as lal_stmt now
    //ElemIdRange             pragmaRange  = idRange(stmt.Corresponding_Pragmas);
    //std::vector<Element_ID> pragmaVector;

    //std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));

    switch(kind){
      case ada_null_stmt:                    // 5.1
        {
          //logKind("ada_null_stmt", kind);

          SgNullStatement& sgnode = mkNullStatement();

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_assign_stmt:             // 5.2
        {
          logKind("ada_assign_stmt", kind);

          ada_base_entity lal_dest, lal_expr;
          ada_assign_stmt_f_dest(lal_stmt, &lal_dest);
          ada_assign_stmt_f_expr(lal_stmt, &lal_expr);

          SgExpression&    lhs    = getExpr(&lal_dest, ctx);
          SgExpression&    rhs    = getExpr(&lal_expr, ctx);
          SgExprStatement& sgnode = mkAssignStmt(lhs, rhs);
          SgExpression&    assign = SG_DEREF(sgnode.get_expression());

          attachSourceLocation(assign, lal_stmt, ctx);
          completeStmt(sgnode, lal_stmt, ctx);
          assocstmt = &sgnode;
          break;
        }
      case ada_label:
        {
          //logKind("ada_label", kind);

          //Make a null stmt for this label to attach to
          SgNullStatement& sgnode = mkNullStatement();

          completeStmt(sgnode, lal_stmt, ctx);

          //Add the label
          ada_base_entity lal_ident;
          ada_label_f_decl(lal_stmt, &lal_ident);
          ada_label_decl_f_name(&lal_ident, &lal_ident);
          ada_defining_name_f_name(&lal_ident, &lal_ident);
          ada_symbol_type p_canonical_text;
          ada_text ada_canonical_text;
          ada_single_tok_node_p_canonical_text(&lal_ident, &p_canonical_text);
          ada_symbol_text(&p_canonical_text, &ada_canonical_text);
          std::string label_name = ada_text_to_locale_string(&ada_canonical_text);
          ada_destroy_text(&ada_canonical_text);

          //This code is similar to labelIfNeeded/labelStmt
          SgLabelStatement& sgn     = mkLabelStmt(label_name, sgnode, ctx.scope());
          attachSourceLocation(sgn, lal_stmt, ctx);
          ctx.appendStatement(sgn);

          assocstmt = &sgnode;
          break;
        }
      case ada_return_stmt:
        {
          logKind("ada_return_stmt", kind);
          ada_base_entity return_expr;
          ada_return_stmt_f_return_expr(lal_stmt, &return_expr);
          SgExpression*   retval = ada_node_is_null(&return_expr) ? nullptr : &getExpr(&return_expr, ctx);
          SgReturnStmt&   sgnode = mkReturnStmt(retval);

          completeStmt(sgnode, lal_stmt, ctx);
          assocstmt = &sgnode;
          break;
        }
      case ada_accept_stmt:                 // 9.5.2
      case ada_accept_stmt_with_stmts:
        {
          logKind("ada_accept_stmt", kind);
          ada_base_entity lal_identifier;
          ada_accept_stmt_f_name(lal_stmt, &lal_identifier);
          SgExpression&            entryref = getExpr(&lal_identifier, ctx);
          SgExpression&            idx      = getExpr_opt(nullptr, ctx); //TODO What is Accept_Entry_Index?
          SgAdaAcceptStmt&         sgnode   = mkAdaAcceptStmt(entryref, idx);

          completeStmt(sgnode, lal_stmt, ctx);

          ada_base_entity params;
          ada_accept_stmt_f_params(lal_stmt, &params);
          ada_entry_completion_formal_params_f_params(&params, &params);
          SgFunctionParameterList& paramlst = SG_DEREF(sgnode.get_parameterList());
          AstContext               parmctx  = ctx.scope(SG_DEREF(sgnode.get_parameterScope()));

          int param_count = ada_node_is_null(&params) ? 0 : ada_node_children_count(&params);
          for(int i = 0; i < param_count; i++){
            ada_base_entity param;
            ada_node_child(&params, i, &param);
            SgVariableDeclaration& decl = getParm(&param, parmctx);
            for(SgInitializedName* parm : decl.get_variables())
              paramlst.append_arg(parm);
          }

          ada_base_entity handled_stmts;
          ada_accept_stmt_with_stmts_f_stmts(lal_stmt, &handled_stmts);

          if(kind == ada_accept_stmt || ada_node_is_null(&handled_stmts))
          {
            SgStatement&           noblock = mkNullStatement();

            sg::linkParentChild(sgnode, noblock, &SgAdaAcceptStmt::set_body);
          }
          else
          {
            SgBasicBlock&          block   = mkBasicBlock();

            sg::linkParentChild(sgnode, as<SgStatement>(block), &SgAdaAcceptStmt::set_body);

           //Get the stmts, exceptions, and pragmas for this package
            ada_base_entity lal_stmts, lal_exceptions, lal_pragmas;
            ada_handled_stmts_f_stmts(&handled_stmts, &lal_stmts);
            ada_handled_stmts_f_exceptions(&handled_stmts, &lal_exceptions);
            //TODO lal_pragmas

            completeHandledBlock( &lal_stmts,
                                  &lal_exceptions,
                                  &lal_pragmas,
                                  simpleBlockHandler,
                                  simpleExceptionBlockHandler,
                                  block,
                                  false /* no separate block is needed */,
                                  ctx
                                );
          }

          assocstmt = &sgnode;
          break;
        }
      default:
        {
          logWarn() << "Unhandled statement " << kind << std::endl;
          //ADA_ASSERT (!FAIL_ON_ERROR(ctx));
        }
    }

    //recordPragmasID(std::move(pragmaVector), assocstmt, ctx);
  }

//TODO Convert this
void handleExceptionHandler(ada_base_entity* lal_exception, SgTryStmt& tryStmt, AstContext ctx)
  {
    using PragmaContainer = AstContext::PragmaContainer;

    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_exception);

    if(kind != ada_exception_handler){
        logWarn() << "handleExceptionHandler given " << kind << std::endl;
    }

    //ADA_ASSERT (elem.Element_Kind == An_Exception_Handler);

    //logKind("An_Exception_Handler", elem.ID);
    /*name_container            names   = queryDeclNames(ex.Choice_Parameter_Specification, ctx);

    if(names.size() == 0)
    {
      // add an unnamed exception handler
      names.emplace_back(std::string{}, std::string{}, ctx.scope(), elem);
    }

    ADA_ASSERT (names.size() == 1);
    ElemIdRange              tyRange = idRange(ex.Exception_Choices);
    SgType&                  extypes = traverseIDs(tyRange, elemMap(), ExHandlerTypeCreator{ctx});
    SgInitializedNamePtrList lst     = constructInitializedNamePtrList(ctx, asisVars(), names, extypes, nullptr);
    SgBasicBlock&            body    = mkBasicBlock();

    ADA_ASSERT (lst.size() == 1);
    SgCatchOptionStmt&       sgnode  = mkExceptionHandler(SG_DEREF(lst[0]), body, tryStmt);
    ElemIdRange              range   = idRange(ex.Handler_Statements);

    sg::linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);
    sgnode.set_parent(tryStmt.get_catch_statement_seq_root());

    PragmaContainer pendingPragmas;
    AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

    traverseIDs(range, elemMap(), StmtCreator{pragmaCtx.scope(body)});

    computeSourceRangeFromChildren(body);
    attachSourceLocation(sgnode, elem, pragmaCtx);
    processAndPlacePragmas(ex.Pragmas, { &body }, pragmaCtx); // pragmaCtx.scope(body) ?*/
    /* unused fields:
    */
  }

void handleDeclaration(ada_base_entity* lal_element, AstContext ctx, bool isPrivate)
{
  using PragmaContainer = AstContext::PragmaContainer;

  //ADA_ASSERT (elem.Element_Kind == A_Declaration);
  //logKind("A_Declaration", elem.ID);

  SgDeclarationStatement* assocdecl = nullptr;

  //Get the kind of this node
  ada_node_kind_enum kind;
  kind = ada_node_kind(lal_element);

  ada_text kind_name;
  ada_kind_name(kind, &kind_name);
  std::string kind_name_string = ada_text_to_locale_string(&kind_name);
  logTrace()   << "handleDeclaration called on a " << kind_name_string << std::endl;
  
  //Declaration_Struct&     decl = elem.The_Union.Declaration; //decl is equivalent to lal_element
  //ElemIdRange             pragmaRange  = idRange(decl.Corresponding_Pragmas);
  //std::vector<Element_ID> pragmaVector;

  //std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));

  switch (kind)
  {

    case ada_package_decl:                    // 7.1(2)
      {
        logKind("ada_package_decl", kind);

        //Get the name for this package
        ada_base_entity lal_identifier;
        ada_base_package_decl_f_package_name(lal_element, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);
        std::string ident = ada_text_to_locale_string(&ada_canonical_text);

        SgScopeStatement*       parent_scope = &ctx.scope();

        SgAdaPackageSpecDecl& sgnode  = mkAdaPackageSpecDecl(ident, SG_DEREF(parent_scope));
        SgAdaPackageSpec&     pkgspec = SG_DEREF(sgnode.get_definition());

        logTrace() << "package decl " << ident
                   << std::endl;

        int hash = hash_node(lal_element);
        recordNode(libadalangDecls(), hash, sgnode);

        privatize(sgnode, isPrivate);
        //~ attachSourceLocation(pkgspec, lal_element, ctx);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        PragmaContainer pendingPragmas;
        AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

        // visible items
        {
          ada_base_entity public_part;
          ada_base_package_decl_f_public_part(lal_element, &public_part);
          ada_declarative_part_f_decls(&public_part, &public_part);

          if(!ada_node_is_null(&public_part)){
            int range = ada_node_children_count(&public_part);
            for(int i = 0; i < range; i++){
              ada_base_entity lal_child;
              ada_node_child(&public_part, i, &lal_child);
              handleElement(&lal_child, pragmaCtx.scope(pkgspec));
            }
          }
        }

        // private items
        {
          ada_base_entity private_part;
          ada_base_package_decl_f_private_part(lal_element, &private_part);
          ada_declarative_part_f_decls(&private_part, &private_part);

          if(!ada_node_is_null(&private_part)){
            int range = ada_node_children_count(&private_part);
            for(int i = 0; i < range; i++){
              ada_base_entity lal_child;
              ada_node_child(&private_part, i, &lal_child);
              handleElement(&lal_child, pragmaCtx.scope(pkgspec), true /* private items */);
            }

            // a package may contain an empty private section
            pkgspec.set_hasPrivate(true);
          }
        }

        // \todo consider:
        // currently, missing built-in function declarations are generated when needed
        //   a more principled implementation could generate them here, but they still would
        //   need to be identified when needed.
        // generateBuiltinFunctionsOnTypes(ctx.scope(pkgspec));

        //processAndPlacePragmas(decl.Pragmas, { &pkgspec }, pragmaCtx.scope(pkgspec));

        assocdecl = &sgnode;
        break;
      }
    case ada_package_body:               // 7.2(2)
      {
        logKind("ada_package_body", kind);

        //Get the name for this package
        ada_base_entity lal_identifier;
        ada_package_body_f_package_name(lal_element, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);
        std::string ident = ada_text_to_locale_string(&ada_canonical_text);

        SgScopeStatement*       parent_scope = &ctx.scope();

        ada_base_entity previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &previous_decl); //TODO imprecise fallback
        int decl_hash = hash_node(&previous_decl);
        SgDeclarationStatement& declnode = lookupNode(libadalangDecls(), decl_hash);
        SgAdaPackageSpecDecl*   specdcl  = getAdaPackageSpecDecl(&declnode);
        //SgAdaPackageSpecDecl*   specdcl  = &mkAdaPackageSpecDecl(ident, SG_DEREF(parent_scope));

        //SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Declaration, decl.Corresponding_Body_Stub);
        //SgFunctionDeclaration*  nondef  = getFunctionDeclaration(ndef ? ndef->get_firstNondefiningDeclaration() : nullptr);
        SgAdaPackageBodyDecl*   nondef = nullptr; //For now, just assume that this is the first declaration

        // when this is an implementation of a stub, use the scope of the stub, instead of the global scope
        SgScopeStatement&       logicalScope = nondef ? SG_DEREF(nondef->get_scope())
                                                      : SG_DEREF(parent_scope);

        SgAdaPackageBodyDecl&   sgnode  = mkAdaPackageBodyDecl(SG_DEREF(specdcl), nondef, logicalScope);
        SgAdaPackageBody&       pkgbody = SG_DEREF(sgnode.get_definition());

        int hash = hash_node(lal_element);
        recordNode(libadalangDecls(), hash, sgnode);

        sgnode.set_scope(specdcl->get_scope());
        //~ attachSourceLocation(pkgbody, elem, ctx);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        //Get the stmts, decls, exceptions, and pragmas for this package
        ada_base_entity lal_decls, lal_stmts, lal_exceptions, lal_pragmas;

        ada_package_body_f_decls(lal_element, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_package_body_f_stmts(lal_element, &lal_handled_stmts);
        ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
        ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
        //TODO lal_pragmas

        const bool hasBodyStatements = !ada_node_is_null(&lal_handled_stmts);

        completeDeclarationsWithHandledBlock( &lal_decls,
                                              &lal_stmts,
                                              &lal_exceptions,
                                              &lal_pragmas,
                                              routineBlockHandler,
                                              routineExceptionBlockHandler,
                                              pkgbody,
                                              hasBodyStatements /* create new block for statements if needed */,
                                              ctx
                                            );

        assocdecl = &sgnode;
        break;
      }
    case ada_subp_decl:                   // 6.1(4)   -> Trait_Kinds
    case ada_abstract_subp_decl:
    //case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
      {
        //Get the subp spec node
        ada_base_entity subp_spec;
        ada_base_entity lal_overriding;
        if(kind == ada_subp_decl || kind == ada_abstract_subp_decl){
          ada_classic_subp_decl_f_subp_spec(lal_element, &subp_spec);
          ada_classic_subp_decl_f_overriding(lal_element, &lal_overriding);
        } else {
          //TODO
        }

        //Determine if this is a function or procedure
        ada_base_entity subp_kind;
        ada_subp_spec_f_subp_kind(&subp_spec, &subp_kind);
        ada_node_kind_enum subp_kind_kind = ada_node_kind(&subp_kind);

        //Get the params for if this is a function
        ada_base_entity subp_params;
        ada_subp_spec_f_subp_params(&subp_spec, &subp_params);

        //Get the return type for if this is a function
        ada_base_entity subp_returns;
        ada_subp_spec_f_subp_returns(&subp_spec, &subp_returns);

        //Get the name
        ada_base_entity lal_identifier;
        ada_subp_spec_f_subp_name(&subp_spec, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);

        ada_node_kind_enum lal_overriding_kind = ada_node_kind(&lal_overriding);

        const bool              isFunc = subp_kind_kind == ada_subp_kind_function;
        const bool          overriding = (lal_overriding_kind == ada_overriding_overriding); //TODO ada_overriding_unspecified might count?
        SgScopeStatement* parent_scope = &ctx.scope();
        std::string              ident = ada_text_to_locale_string(&ada_canonical_text);
        //ElemIdRange             params = idRange(decl.Parameter_Profile);
        SgType&                rettype = isFunc ? getDeclType(&subp_returns, ctx)
                                                : mkTypeVoid();

        ada_destroy_text(&ada_canonical_text);
        logKind(isFunc ? "A_Function_Declaration" : "A_Procedure_Declaration", kind);

        SgScopeStatement&      logicalScope = SG_DEREF(parent_scope);
        const bool             renamingAsBody = false; //definedByRenamingID(decl.Corresponding_Body, ctx); //TODO Can lal do this?
        ParameterCompletion    complete{&subp_params, ctx};
        SgFunctionDeclaration& sgnode  = renamingAsBody
                                            ? mkAdaFunctionRenamingDecl(ident, logicalScope, rettype, std::move(complete))
                                            : mkProcedureDecl_nondef(ident, logicalScope, rettype, std::move(complete))
                                            ;

        setAbstractModifier(sgnode, lal_element);
        setOverride(sgnode, overriding);
        int hash = hash_node(lal_element);
        recordNode(libadalangDecls(), hash, sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    //case ada_null_subp_decl (maybe):             // 6.7
    case ada_subp_body:              // 6.3(2)
    //case A_Procedure_Body_Declaration:             // 6.3(2)
      {
        /*if (decl.Declaration_Kind == A_Function_Declaration)
          logKind("A_Function_Body_Declaration", elem.ID);
        else if (decl.Declaration_Kind == A_Procedure_Body_Declaration)
          logKind("A_Procedure_Body_Declaration", elem.ID);
        else
          logKind("A_Null_Procedure_Declaration", elem.ID);*/

        //Get the subp spec node
        ada_base_entity subp_spec;
        if(kind == ada_subp_body){
            ada_base_subp_body_f_subp_spec(lal_element, &subp_spec);
        } else {
            //TODO
        }

        //Determine if this is a function or procedure
        ada_base_entity subp_kind;
        ada_subp_spec_f_subp_kind(&subp_spec, &subp_kind);
        ada_node_kind_enum subp_kind_kind = ada_node_kind(&subp_kind);

        //Get the params for if this is a function
        ada_base_entity subp_params;
        ada_subp_spec_f_subp_params(&subp_spec, &subp_params);

        //Get the return type for if this is a function
        ada_base_entity subp_returns;
        ada_subp_spec_f_subp_returns(&subp_spec, &subp_returns);

        //Get the name
        ada_base_entity lal_identifier;
        ada_subp_spec_f_subp_name(&subp_spec, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);
        std::string ident = ada_text_to_locale_string(&ada_canonical_text);

        const bool              isFunc  = (subp_kind_kind == ada_subp_kind_function);
        SgScopeStatement*       parent_scope = &ctx.scope();
        //ElemIdRange             params  = idRange(usableParameterProfile(decl, ctx)); 
        SgType&                 rettype = isFunc ? getDeclType(&subp_returns, ctx)
                                                 : mkTypeVoid();

        //SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Declaration, decl.Corresponding_Body_Stub);
        //SgFunctionDeclaration*  nondef  = getFunctionDeclaration(ndef ? ndef->get_firstNondefiningDeclaration() : nullptr);
        SgFunctionDeclaration*  nondef = nullptr; //For now, just assume that this is the first declaration
        //ADA_ASSERT(!ndef || nondef); // ndef => nondef

        //~ logError() << "proc body: " << nondef << std::endl;

        // SCOPE_COMMENT_1: the logical scope is only used, if nondef is nullptr
        //   createFunDef chooses the scope as needed.
        SgScopeStatement&       logicalScope = SG_DEREF(parent_scope);
        SgFunctionDeclaration&  sgnode  = createFunDef(nondef, ident, logicalScope, rettype, ParameterCompletion{&subp_params, ctx});
        SgBasicBlock&           declblk = functionBody(sgnode);

        int hash = hash_node(lal_element);
        recordNode(libadalangDecls(), hash, sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        attachSourceLocation(declblk, lal_element, ctx); // experimental
        ctx.appendStatement(sgnode);

        // PP 2/6/22: Since a null procedure does not have any body,
        //            there should be no pragmas to process.
        //            Pragmas on the declaration are processed by
        //            the parent scope.
        if (kind != ada_null_subp_decl) {
          completeRoutineBody(lal_element, declblk, ctx);
        }

        assocdecl = &sgnode;
        break;
      }
    case ada_task_body:                  // 9.1(6)
      {
        logKind("ada_task_body", kind);

        //Get the name
        ada_base_entity lal_identifier;
        ada_task_body_f_name(lal_element, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);

        //Get the hash for the decl
        ada_base_entity p_decl_part;
        ada_body_node_p_decl_part(lal_element, 1, &p_decl_part);

        int                          hash = hash_node(lal_element);
        int                     decl_hash = hash_node(&p_decl_part);
        std::string             ident     = ada_text_to_locale_string(&ada_canonical_text);
        SgAdaTaskBody&          tskbody   = mkAdaTaskBody();
        SgDeclarationStatement* ndef      = findFirst(libadalangDecls(), decl_hash);
        SgAdaTaskBodyDecl*      nondef    = isSgAdaTaskBodyDecl(ndef);

        //Element_ID              specID  = queryAsisIDOfDeclaration(decl, A_Task_Body_Stub, ctx);
        SgDeclarationStatement& tskdecl = lookupNode(libadalangDecls(), decl_hash); //TODO What is specID?

        SgScopeStatement&       logicalScope = SG_DEREF(&ctx.scope());
        SgAdaTaskBodyDecl&      sgnode  = mkAdaTaskBodyDecl(tskdecl, nondef, tskbody, logicalScope);

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        recordNode(libadalangDecls(), hash, sgnode);

        //Get the stmts, decls, exceptions, and pragmas for this package
        ada_base_entity lal_decls, lal_stmts, lal_exceptions, lal_pragmas;

        ada_task_body_f_decls(lal_element, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_task_body_f_stmts(lal_element, &lal_handled_stmts);
        ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
        ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
        //TODO lal_pragmas

        completeDeclarationsWithHandledBlock( &lal_decls,
                                              &lal_stmts,
                                              &lal_exceptions,
                                              &lal_pragmas,
                                              routineBlockHandler,
                                              routineExceptionBlockHandler,
                                              tskbody,
                                              false /* same block for declarations and statements */,
                                              ctx
                                            );

        assocdecl = &sgnode;
        break;
      }
    case ada_subp_body_stub:                     // 10.1.3(3)
      {
        //Get the subp spec node
        ada_base_entity subp_spec;
        if(kind == ada_subp_body_stub){
            ada_subp_body_stub_f_subp_spec(lal_element, &subp_spec);
        } else {
            //TODO
        }

        //Determine if this is a function or procedure
        ada_base_entity subp_kind;
        ada_subp_spec_f_subp_kind(&subp_spec, &subp_kind);
        ada_node_kind_enum subp_kind_kind = ada_node_kind(&subp_kind);
        const bool isFunc  = (subp_kind_kind == ada_subp_kind_function);

        //Get the params for if this is a function
        ada_base_entity subp_params;
        ada_subp_spec_f_subp_params(&subp_spec, &subp_params);

        //Get the return type for if this is a function
        ada_base_entity subp_returns;
        ada_subp_spec_f_subp_returns(&subp_spec, &subp_returns);

        logKind(isFunc ? "A_Function_Body_Stub" : "A_Procedure_Body_Stub", kind);

        //Get the name
        ada_base_entity lal_identifier;
        ada_subp_spec_f_subp_name(&subp_spec, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);

        std::string             ident        = ada_text_to_locale_string(&ada_canonical_text);
        SgScopeStatement*       parent_scope = &ctx.scope();
        SgType&                 rettype      = isFunc ? getDeclType(&subp_returns, ctx)
                                                      : mkTypeVoid();

        /*SgDeclarationStatement* ndef    = findFirst(asisDecls(), decl.Corresponding_Declaration);
        SgFunctionDeclaration*  nondef  = getFunctionDeclaration(ndef);*/
        SgFunctionDeclaration*  nondef = nullptr; //For now, just assume that this is the first declaration

        SgScopeStatement&       logicalScope = SG_DEREF(parent_scope);
        SgFunctionDeclaration&  sgnode  = createFunDcl(nondef, ident, logicalScope, rettype, ParameterCompletion{&subp_params, ctx});

        setAdaSeparate(sgnode, true /* separate */);

         int hash = hash_node(lal_element);
        recordNode(libadalangDecls(), hash, sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_task_type_decl:                  // 9.1(2)
      {
        logKind("ada_task_type_decl", kind);

        auto                        spec    = getTaskSpec_opt(lal_element, ctx); //TODO

        //Get the name
        ada_base_entity defining_name;
        ada_base_type_decl_f_name(lal_element, &defining_name);
        ada_base_entity lal_identifier;
        ada_defining_name_f_name(&defining_name, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);

        //Get the discriminants
        ada_base_entity lal_discr;
        ada_task_type_decl_f_discriminants(lal_element, &lal_discr);

        std::string                 ident  = ada_text_to_locale_string(&ada_canonical_text);

        int                         hash   = hash_node(lal_element);
        /*SgDeclarationStatement*     ndef   = findFirst(libadalangTypes(), hash);
        SgAdaTaskTypeDecl*          nondef = isSgAdaTaskTypeDecl(ndef);*/
        SgAdaTaskTypeDecl*          nondef = nullptr; //For now, just assume that this is the first declaration

        SgScopeStatement*           parentScope = &ctx.scope();
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(&lal_discr, 0, ctx);

        if (discr)
        {
          parentScope = discr->get_discriminantScope();
        }

        SgAdaTaskTypeDecl& sgdecl  = nondef ? mkAdaTaskTypeDecl(*nondef, SG_DEREF(spec.first), *parentScope)
                                            : mkAdaTaskTypeDecl(ident, spec.first,  *parentScope);

        attachSourceLocation(sgdecl, lal_element, ctx);
        privatize(sgdecl, isPrivate);

        recordNode(libadalangTypes(), hash, sgdecl, nondef != nullptr);
        recordNode(libadalangDecls(), hash, sgdecl);

        if (!discr)
        {
          ctx.appendStatement(sgdecl);
          assocdecl = &sgdecl;
        }
        else
        {
          completeDiscriminatedDecl(hash, lal_element, *discr, sgdecl, isPrivate, ctx);
          assocdecl = discr;
        }

        //AstContext::PragmaContainer taskPragmas = splitOfPragmas(pragmaVector, taskDeclPragmas, ctx);

        //spec.second(std::move(taskPragmas)); // complete the body

         break;
      }
    case ada_entry_decl:                     // 9.5.2(2)
      {
        logKind("ada_entry_decl", kind);

        //Get the name
        ada_base_entity entry_spec;
        ada_entry_decl_f_spec(lal_element, &entry_spec);
        ada_base_entity defining_name;
        ada_entry_spec_f_entry_name(&entry_spec, &defining_name);
        ada_base_entity lal_identifier;
        ada_defining_name_f_name(&defining_name, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);

        std::string     ident   = ada_text_to_locale_string(&ada_canonical_text);
        int              hash   = hash_node(lal_element);

        //Get the params
        ada_base_entity entry_params;
        ada_entry_spec_f_entry_params(&entry_spec, &entry_params);

        //Get the family type
        ada_base_entity family_type;
        ada_entry_spec_f_family_type(&entry_spec, &family_type);

        SgType&         idxTy  = getDefinitionType_opt(&family_type, ctx);
        SgAdaEntryDecl& sgnode = mkAdaEntryDecl(ident, ctx.scope(), ParameterCompletion{&entry_params, ctx}, idxTy);

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        // the entry call links back to the declaration ID
        recordNode(libadalangDecls(), hash, sgnode);
        
        assocdecl = &sgnode;
        break;
      }
    case ada_object_decl:
      {
        logKind("ada_object_decl", kind);

        assocdecl = &handleVarCstDecl(lal_element, ctx, isPrivate, tyIdentity);
        /* unused fields:
        */
        break;
      }
    case ada_type_decl:
      {
        logKind("ada_type_decl", kind);
        //Get the type definition
        ada_base_entity lal_type_def;
        ada_type_decl_f_type_def(lal_element, &lal_type_def);

        ada_node_kind_enum type_def_kind = ada_node_kind(&lal_type_def);

        logTrace() << "Ordinary Type ";

        if(type_def_kind == ada_derived_type_def){
          //Get abstract & limited from the type def
          ada_base_entity lal_has_abstract;
          ada_derived_type_def_f_has_abstract(&lal_type_def, &lal_has_abstract);
          ada_node_kind_enum abstract_status = ada_node_kind(&lal_has_abstract);
          const bool            has_abstract = (abstract_status == ada_abstract_present);

          ada_base_entity lal_has_limited;
          ada_derived_type_def_f_has_limited(&lal_type_def, &lal_has_limited);
          ada_node_kind_enum limited_status = ada_node_kind(&lal_has_limited);
          const bool            has_limited = (limited_status == ada_limited_present);

          logTrace() << "\n  abstract: " << has_abstract
                     << "\n  limited: " << has_limited
                     << std::endl;
        }

        //Get the name of the type
        ada_base_entity lal_identifier;
        ada_base_type_decl_f_name(lal_element, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        ada_symbol_type p_canonical_text;
        ada_text ada_canonical_text;
        ada_single_tok_node_p_canonical_text(&lal_identifier, &p_canonical_text);
        ada_symbol_text(&p_canonical_text, &ada_canonical_text);
        std::string type_name = ada_text_to_locale_string(&ada_canonical_text);
        logTrace() << "  name: " << type_name << std::endl;
        SgScopeStatement*           parentScope = &ctx.scope();
        ada_base_entity             lal_discr;
        ada_type_decl_f_discriminants(lal_element, &lal_discr);
        ada_base_entity*            second_discr = secondaryDiscriminants(lal_element, &lal_discr, ctx);
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDeclID_opt(&lal_discr, second_discr, ctx);

        if(discr)
        {
          parentScope = discr->get_discriminantScope();
        }
        logInfo() << "After discr\n";
        SgScopeStatement&       scope     = SG_DEREF(parentScope);
        TypeData                ty        = getTypeFoundation(type_name, &lal_type_def, ctx.scope(scope));
        int                     type_hash = hash_node(lal_element);
        SgDeclarationStatement* nondef    = findFirst(libadalangTypes(), type_hash);
        SgDeclarationStatement& sgdecl    = sg::dispatch(TypeDeclMaker{type_name, scope, ty, nondef}, &ty.sageNode());

        privatize(sgdecl, isPrivate);
        recordNode(libadalangTypes(), type_hash, sgdecl, nondef != nullptr);
        attachSourceLocation(sgdecl, lal_element, ctx);

        if (!discr)
        {
          //ADA_ASSERT(&ctx.scope() == parentScope);
          ctx.appendStatement(sgdecl);

          assocdecl = &sgdecl;
        }
        else
        {
          completeDiscriminatedDecl(type_hash, lal_element, *discr, sgdecl, isPrivate, ctx);

          assocdecl = discr;
        }

        processInheritedElementsOfDerivedTypes(ty, discr ? *discr : sgdecl, ctx);

        break;
      }
    default:
      logWarn() << "unhandled declaration kind: " << kind << std::endl;
      //ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }

  //processAspects(lal_element, decl, assocdecl, ctx);
  //recordPragmasID(std::move(pragmaVector), assocdecl, ctx);
}
 
} //end Libadalang_ROSE_Translation
