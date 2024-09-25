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

  /// Creates a new label stmt
  SgLabelStatement&
  labelStmt(SgStatement& stmt, std::string lblname, ada_base_entity* lal_element, AstContext ctx)
  {
    SgNode&           parent  = SG_DEREF(stmt.get_parent());
    SgLabelStatement& sgn     = mkLabelStmt(lblname, stmt, ctx.scope());

    //~ copyFileInfo(stmt, sgn);
    attachSourceLocation(sgn, lal_element, ctx);
    sgn.set_parent(&parent);
    //ctx.labelsAndLoops().label(lblid, sgn);

    return sgn;
  }

  /// completes statements by setting source locations, parent node,
  /// adding labels (if needed)...
  /// @{
  void
  completeStmt(SgStatement& sgnode, ada_base_entity* lal_element, AstContext ctx)
  {
    //ADA_ASSERT (elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, lal_element, ctx);
    sgnode.set_parent(&ctx.scope());

    //We are handling labels differently
    //SgStatement&      sgn  = labelIfNeeded(sgnode, lal_element, ctx);

    ctx.appendStatement(sgnode);
  }

  template <class SageScopeStmt>
  void
  completeStmt(SageScopeStmt& sgnode, ada_base_entity* lal_element, AstContext ctx, const std::string& lblname)
  {
    attachSourceLocation(sgnode, lal_element, ctx);
    sgnode.set_parent(&ctx.scope());

    //SgStatement&      sgn0 = labelIfNeeded(sgnode, lblid, ctx);
    //Statement_Struct& stmt = elem.The_Union.Statement;
    //SgStatement&      sgn  = labelIfNeeded(sgn0, stmt, ctx);
    if(lblname != ""){
      logInfo() << "In completeStmt, adding label " << lblname << ".\n";
      sgnode.set_string_label(lblname);
      SgStatement& sgn = labelStmt(sgnode, lblname, lal_element, ctx);
      ctx.appendStatement(sgn);
      return;
    }

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

  /// a simple block handler just traverses the statement list and adds them to the \ref blk.
  void simpleBlockHandler(ada_base_entity* lal_stmt_list, SgScopeStatement& blk, AstContext ctx)
  {
    int                 range = ada_node_children_count(lal_stmt_list);

    for(int i = 0; i < range; ++i){
        ada_base_entity lal_stmt;
        ada_node_child(lal_stmt_list, i, &lal_stmt);
        handleStmt(&lal_stmt, ctx.scope(blk));
    }
  }

  /// at some point loops, labels, gotos need to be patched up. In this case, we do that at the
  ///   end of a routine through the use of the LoopAndLabelManager.
  void routineBlockHandler(ada_base_entity* lal_stmt_list, SgScopeStatement& blk, AstContext ctx)
  {
    LabelAndLoopManager lblmgr;

    simpleBlockHandler(lal_stmt_list, blk, ctx.labelsAndLoops(lblmgr));
  }

  /// Calls handleExceptionHandler for each excp in a block
  void simpleExceptionBlockHandler(ada_base_entity* lal_handlers, SgScopeStatement& blk, SgTryStmt& trystmt, AstContext ctx)
  {
    int                 range = ada_node_children_count(lal_handlers);

    for(int i = 0; i < range; ++i){
        ada_base_entity lal_handler;
        ada_node_child(lal_handlers, i, &lal_handler);
        handleExceptionHandler(&lal_handler, trystmt, ctx.scope(blk));
    }
  }

  /// at some point loops, labels, gotos need to be patched up. In this case, we do that at the
  ///   end of a routine through the use of the LoopAndLabelManager
  void routineExceptionBlockHandler(ada_base_entity* lal_handlers, SgScopeStatement& blk, SgTryStmt& trystmt, AstContext ctx)
  {
    LabelAndLoopManager lblmgr;

    simpleExceptionBlockHandler(lal_handlers, blk, trystmt, ctx.labelsAndLoops(lblmgr));
  }

  /// completes any block with exception handlers and pragmas attached
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

  /// completes any block with declarative items and exception handlers and pragmas attached
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
    int             range = ada_node_children_count(lal_decls); //TODO This works if lal_decls is uninitialized, but I'm not sure why?

    for(int i =0; i < range; ++i){
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

  /// Completes any routine body with  declarative items and stmts and exception handlers and pragmas attached
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
        //lal_pragmas = ???; //TODO pragmas
    } else if(kind == ada_entry_body){
        ada_entry_body_f_decls(lal_decl, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_entry_body_f_stmts(lal_decl, &lal_handled_stmts);
        ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
        ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
        //lal_pragmas = ???; //TODO pragmas
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

  /// Create a function definition
  /// \param nondef pointer to the defining decl, null if this is the defining decl
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

  /// Create a function declaration
  /// \param nondef pointer to the defining decl, null if this is the defining decl
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

  /// Find any additional discriminants for \ref lal_decl
  ada_base_entity*
  secondaryDiscriminants(ada_base_entity* lal_decl, ada_base_entity* first_discriminant, AstContext)
  {
    const bool useThisDecl = (  ada_node_is_null(first_discriminant) == 0
                             );

    if (useThisDecl) return nullptr;
    //TODO Where should the 2nd discr come from?
    logWarn() << "1st discr is null, don't have 2nd discr\n";
    return nullptr;
    //Element_Struct& firstDecl = retrieveElem(elemMap(), decl.Corresponding_Type_Declaration);
    //ADA_ASSERT(firstDecl.Element_Kind == A_Declaration);

    //return firstDecl.The_Union.Declaration.Discriminant_Part;
  }

namespace {

  // may need special processing for pragmas in body
  using DeferredPragmaBodyCompletion = std::function<void(AstContext::PragmaContainer)>;

  // following proc is public and located at the bottom of the file
  //   void
  //   processInheritedSubroutines( SgNamedType& derivedType,
  //                                ada_base_entity* tydef,
  //                                AstContext ctx
  //                              )

  template <class SageTypeDeclStmt>
  void
  processInheritedSubroutines( ada_base_entity* tydef,
                               SageTypeDeclStmt& tyDecl,
                               AstContext ctx
                             )
  {
    processInheritedSubroutines(SG_DEREF(tyDecl.get_type()), tydef, ctx);
  }

  std::tuple<SgEnumDeclaration*, SgAdaRangeConstraint*>
  getBaseEnum(SgType* baseTy)
  {

    SgAdaRangeConstraint* constraint = nullptr;
    SgEnumDeclaration*    basedecl   = nullptr;

    if (SgAdaDerivedType* deriveTy = isSgAdaDerivedType(baseTy))
    {
      SgType* ty = deriveTy->get_base_type();

      if (SgAdaSubtype* subTy = isSgAdaSubtype(ty))
      {
        ty = subTy->get_base_type();
        constraint = isSgAdaRangeConstraint(subTy->get_constraint());
      }

      basedecl = si::Ada::baseEnumDeclaration(ty);
    }

    if (basedecl == nullptr)
    {
      logFlaw() << "basedecl == nullptr: " << typeid(*baseTy).name() << std::endl;
    }

    if (SgEnumDeclaration* realdecl = isSgEnumDeclaration(basedecl->get_definingDeclaration()))
      basedecl = realdecl;

    return { basedecl, constraint };
  }

  /// Like processInheritedSubroutines, but specifically for enumeration values
  /// Currently inherits all enum values, ignoring range restrictions (ASIS does this too?)
  void
  processInheritedEnumValues( ada_base_entity* tydef,
                              SgEnumDeclaration& derivedTypeDcl,
                              AstContext ctx
                            )
  {
    //Get the base type
    ada_base_entity lal_subtype_indication, lal_base_type;
    ada_derived_type_def_f_subtype_indication(tydef, &lal_subtype_indication);
    ada_type_expr_p_designated_type_decl(&lal_subtype_indication, &lal_base_type);

    //We process the subps in a different function, so just handle the decls

    using BaseTuple = std::tuple<SgEnumDeclaration*, SgAdaRangeConstraint*>;

    BaseTuple          baseInfo = getBaseEnum(derivedTypeDcl.get_adaParentType());
    SgEnumDeclaration& origDecl = SG_DEREF(std::get<0>(baseInfo));

    // do not process derivations from Standard.*Char type.
    //   the reason is that the derived enums will have its symbols
    //   injected into the new scope as alias symbols, but for
    //   character based enums, the symbols are not physically present.
    if ( si::Ada::characterBaseType(origDecl.get_type()) )
      return;

    //Get the range of inherited decls
    ada_base_entity lal_inherited_decl_list;
    ada_type_decl_f_type_def(&lal_inherited_decl_list, &lal_inherited_decl_list);
    ada_enum_type_def_f_enum_literals(&lal_inherited_decl_list, &lal_inherited_decl_list);
    int count = ada_node_children_count(&lal_inherited_decl_list);

    // just traverse the IDs, as the elements are not present
    for(int i = 0; i < count ; ++i){
      ada_base_entity lal_inherited_decl;
      if(ada_node_child(&lal_inherited_decl_list, i, &lal_inherited_decl) != 0){
        //TODO How will this work? It isn't a unique node.
        SgType& enumty = SG_DEREF(derivedTypeDcl.get_type());

        //Get the name
        ada_base_entity lal_defining_name, lal_identifier;
        ada_enum_literal_decl_f_name(&lal_inherited_decl, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);

        // \todo name.ident could be a character literal, such as 'c'
        //       since SgEnumDeclaration only accepts SgInitializedName as enumerators
        //       SgInitializedName are created with the name 'c' instead of character constants.
        SgExpression&       repval = getEnumRepresentationValue(&lal_inherited_decl, i, ctx);
        SgInitializedName&  sgnode = mkEnumeratorDecl(derivedTypeDcl, ident, enumty, repval);

        attachSourceLocation(sgnode, &lal_inherited_decl, ctx);
        //~ sg::linkParentChild(enumdcl, sgnode, &SgEnumDeclaration::append_enumerator);
        derivedTypeDcl.append_enumerator(&sgnode);
      }
    }
  }

   /// Links a derived class with its parent class, if said parent exists
  void
  setParentRecordConstraintIfAvail(SgClassDeclaration& sgnode, ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    if(kind != ada_derived_type_def){
      return;
    }

    ada_base_entity lal_subtype_indication;
    ada_derived_type_def_f_subtype_indication(lal_element, &lal_subtype_indication);
    SgBaseClass& pardcl = getParentType(&lal_subtype_indication, ctx);

    sg::linkParentChild(sgnode, pardcl, &SgClassDeclaration::set_adaParentType);
  }

  /// Sets the abstract, limited, and tagged modifiers for a decl
  /// @{
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

  /// @}

  /// Determines & sets the abstract modifier for a decl, sets limited & tagged modifiers to false
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

  /// Determines & sets the abstract, limited, & tagged modifiers for a decl
  void
  setTypeModifiers(SgDeclarationStatement& dcl, ada_base_entity* lal_element)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    switch(kind)
    {
      /*case A_Private_Type_Definition:
        {
          logKind("A_Private_Type_Definition");

          setModifiersUntagged(dcl, def.The_Union.The_Private_Type_Definition);
          break;
        }*/
      case ada_derived_type_def:
        {
          logKind("A_Private_Extension_Definition", kind);
          ada_base_entity lal_has_abstract, lal_has_limited;

          ada_derived_type_def_f_has_abstract(lal_element, &lal_has_abstract);
          ada_node_kind_enum lal_has_abstract_kind = ada_node_kind(&lal_has_abstract);
          bool has_abstract = (lal_has_abstract_kind == ada_abstract_present);

          ada_derived_type_def_f_has_limited(lal_element, &lal_has_limited);
          ada_node_kind_enum lal_has_limited_kind = ada_node_kind(&lal_has_limited);
          bool has_limited = (lal_has_limited_kind == ada_limited_present);

          setModifiers(dcl, has_abstract, has_limited, false);
          break;
        }
     case ada_private_type_def:
       {
          logKind("ada_private_type_def", kind);
          ada_base_entity lal_has_abstract, lal_has_limited;

          ada_private_type_def_f_has_abstract(lal_element, &lal_has_abstract);
          ada_node_kind_enum lal_has_abstract_kind = ada_node_kind(&lal_has_abstract);
          bool has_abstract = (lal_has_abstract_kind == ada_abstract_present);

          ada_private_type_def_f_has_limited(lal_element, &lal_has_limited);
          ada_node_kind_enum lal_has_limited_kind = ada_node_kind(&lal_has_limited);
          bool has_limited = (lal_has_limited_kind == ada_limited_present);

          setModifiers(dcl, has_abstract, has_limited, false);
          break;
       }
     case ada_record_type_def:
       {
          logKind("ada_record_type_def", kind);
          ada_base_entity lal_has_abstract, lal_has_limited, lal_has_tagged;

          ada_record_type_def_f_has_abstract(lal_element, &lal_has_abstract);
          ada_node_kind_enum lal_has_abstract_kind = ada_node_kind(&lal_has_abstract);
          bool has_abstract = (lal_has_abstract_kind == ada_abstract_present);

          ada_record_type_def_f_has_limited(lal_element, &lal_has_limited);
          ada_node_kind_enum lal_has_limited_kind = ada_node_kind(&lal_has_limited);
          bool has_limited = (lal_has_limited_kind == ada_limited_present);

          ada_record_type_def_f_has_tagged(lal_element, &lal_has_tagged);
          ada_node_kind_enum lal_has_tagged_kind = ada_node_kind(&lal_has_tagged);
          bool has_tagged = (lal_has_tagged_kind == ada_tagged_present);

          setModifiers(dcl, has_abstract, has_limited, has_tagged);
          break;
       }
     /* case A_Tagged_Private_Type_Definition:
        {
          logKind("A_Tagged_Private_Type_Definition");

          setModifiers(dcl, def.The_Union.The_Tagged_Private_Type_Definition);
          break;
        }*/
      default:
        logWarn() << "Unknown type declaration view: " << kind
                  << std::endl;
    }
  }

  /// Handles any inheritance for a derived type \ref dcl
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

    if (SgTypedefDeclaration* derivedTypeDcl = isSgTypedefDeclaration(&dcl)){
      processInheritedSubroutines(tydef, *derivedTypeDcl, ctx);
    } else if (SgClassDeclaration* classTypeDcl = isSgClassDeclaration(&dcl)){
      processInheritedSubroutines(tydef, *classTypeDcl, ctx);
    } else if (SgEnumDeclaration* derivedEnumDcl = isSgEnumDeclaration(&dcl)){
      processInheritedEnumValues(tydef, *derivedEnumDcl, ctx);
      processInheritedSubroutines(tydef, *derivedEnumDcl, ctx);
    } else if (SgAdaDiscriminatedTypeDecl* discrTypeDcl = isSgAdaDiscriminatedTypeDecl(&dcl)) {
      processInheritedSubroutines(tydef, *discrTypeDcl, ctx);
    } else {
      logError() << "Unknown SgNode in processInheritedElementsOfDerivedTypes.\n";
    }
  }

  /// Creates a public forward decl for a private type/enum
  SgDeclarationStatement&
  createOpaqueDecl(const std::string& ident, ada_base_entity* lal_element, AstContext ctx)
  {
    SgScopeStatement&       scope = ctx.scope();
    bool                    typeview = false;
    SgDeclarationStatement* res = nullptr;

    //Get the type def
    ada_base_entity lal_type_def;
    ada_type_decl_f_type_def(lal_element, &lal_type_def);
    ada_node_kind_enum lal_type_def_kind = ada_node_kind(&lal_type_def);
    typeview = (ada_node_is_null(&lal_type_def) == 0);

    //TODO Needs more testing w/enums, records

    if(lal_type_def_kind == ada_derived_type_def){ //If it's derived, make an access
      SgClassDeclaration& sgnode = mkRecordDecl(ident, scope);

      if(typeview){
        setParentRecordConstraintIfAvail(sgnode, &lal_type_def, ctx);
      }

      res = &sgnode;
    } else if(lal_type_def_kind == ada_private_type_def){ //If it isn't an access, we need to find the original type to see if it's a type or an enum
      ada_base_entity lal_base_type_decl, lal_base_type_def;
      ada_base_type_decl_p_full_view(lal_element, &lal_base_type_decl);
      ada_type_decl_f_type_def(&lal_base_type_decl, &lal_base_type_def);
      lal_type_def_kind = ada_node_kind(&lal_base_type_def);
      while(lal_type_def_kind == ada_derived_type_def){
        //If this is a derived type, get the type it is derived from
        ada_base_entity lal_subtype_indication;
        ada_derived_type_def_f_subtype_indication(&lal_base_type_def, &lal_subtype_indication);
        ada_type_expr_p_designated_type_decl(&lal_subtype_indication, &lal_base_type_decl);
        ada_type_decl_f_type_def(&lal_base_type_decl, &lal_base_type_def);
        lal_type_def_kind = ada_node_kind(&lal_base_type_def);
      }

      if(lal_type_def_kind == ada_enum_type_def){
        res = &mkEnumDecl(ident, scope);
      } else {
        res = &mkTypeDecl(ident, mkOpaqueType(), scope);
      }
    } else {
      logWarn() << "unhandled opaque type declaration: " << lal_type_def_kind
                << std::endl;
      res = &mkTypeDecl(ident, mkOpaqueType(), scope);
    }

    // \todo put declaration tags on
    SgDeclarationStatement& resdcl = SG_DEREF(res);

    if (typeview){
      setTypeModifiers(resdcl, &lal_type_def);
    } else if(true /*TODO decl.Declaration_Kind == A_Tagged_Incomplete_Type_Declaration*/) {
      setModifiers(resdcl, false /*abstract*/, false /*limited*/, true /*tagged*/);
    }

    return resdcl;
  }

  /// Links & records \ref sgnode, replacing the previous non-discriminated decl if it exists
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

  /// converts a libadalang parameter declaration to a ROSE parameter (i.e., variable)
  ///   declaration.
  SgVariableDeclaration&
  getDiscriminant(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    if(kind != ada_discriminant_spec){
      logError() << "getDicriminant called on non discriminant kind " << kind << std::endl;
    }

    //Get the ids and type_expr
    ada_base_entity lal_names, lal_type_expr;
    ada_discriminant_spec_f_ids(lal_element, &lal_names);
    ada_discriminant_spec_f_type_expr(lal_element, &lal_type_expr);

    SgType&                  basety   = getDeclType(&lal_type_expr, ctx);
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                         libadalangVars(),
                                                                         &lal_names,
                                                                         basety,
                                                                         getVarInit(lal_element, &basety, ctx)
                                                                       );
    SgVariableDeclaration&   sgnode   = mkVarDecl(dclnames, ctx.scope());

    attachSourceLocation(sgnode, lal_element, ctx);

    return sgnode;
  }

  /// Creates a discriminated decl with the list of discriminants in \ref lal_element
  SgAdaDiscriminatedTypeDecl&
  createDiscriminatedDecl(ada_base_entity* lal_element, ada_base_entity* secondary, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    // many definitions are handled else where
    // here we want to convert the rest that can appear in declarative context

    SgScopeStatement&           scope  = ctx.scope();
    SgAdaDiscriminatedTypeDecl& sgnode = mkAdaDiscriminatedTypeDecl(scope);
    SgAdaParameterList&         params = SG_DEREF(sgnode.get_discriminants());

    ctx.appendStatement(sgnode);

    if(!ada_node_is_null(lal_element) && kind == ada_known_discriminant_part)
    {
      logKind("ada_known_discriminant_part", kind);
      //Get the list of discrs
      ada_base_entity lal_discr_list;
      ada_known_discriminant_part_f_discr_specs(lal_element, &lal_discr_list);

      //Iterate over the discrs
      int count = ada_node_children_count(&lal_discr_list);
      for(int i = 0; i < count; ++i){
        ada_base_entity lal_discr_spec;
        if(ada_node_child(&lal_discr_list, i, &lal_discr_spec) != 0){
          params.append_parameter(&getDiscriminant(&lal_discr_spec, ctx));
        }
      }

    } else if(secondary != nullptr && !ada_node_is_null(secondary)){
      //TODO Check if this works for secondary discrs
      int count = ada_node_children_count(secondary);
      for(int i = 0; i < count; ++i){
        ada_base_entity lal_discr_spec;
        if(ada_node_child(secondary, i, &lal_discr_spec) != 0){
          params.append_parameter(&getDiscriminant(&lal_discr_spec, ctx));
        }
      }
    } else {
      logWarn() << "createDiscriminatedDecl given non-discr kind " << kind << std::endl;
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

  /// If \ref primary or \ref secondary exist, creates a discriminated decl using them, otherwise returns nullptr
  SgAdaDiscriminatedTypeDecl*
  createDiscriminatedDecl_opt(ada_base_entity* primary, ada_base_entity* secondary, AstContext ctx)
  {
    if (primary != nullptr && ada_node_is_null(primary))
    {
      if (secondary != nullptr && !ada_node_is_null(secondary)) logFlaw() << "Unexpected secondary discriminants" << std::endl;

      return nullptr;
    }

    return &createDiscriminatedDecl(primary, secondary, ctx);
  }

  /// handles incomplete (but completed) and private types
  SgDeclarationStatement&
  handleOpaqueTypes( ada_base_entity* lal_element, //Should be an ada_type_decl
                     bool isPrivate,
                     AstContext ctx
                   )
  {

    //Get the name for this type
    ada_base_entity lal_identifier;
    ada_base_type_decl_f_name(lal_element, &lal_identifier);
    ada_defining_name_f_name(&lal_identifier, &lal_identifier);
    std::string ident = canonical_text_as_string(&lal_identifier);

    //Get the discriminants
    ada_base_entity             lal_discr;
    ada_type_decl_f_discriminants(lal_element, &lal_discr);
    ada_base_entity*            second_discr = secondaryDiscriminants(lal_element, &lal_discr, ctx);

    //Get the type def
    ada_base_entity lal_type_def;
    ada_type_decl_f_type_def(lal_element, &lal_type_def);
    ada_node_kind_enum lal_type_def_kind = ada_node_kind(&lal_type_def);

    SgScopeStatement*       parentScope = &ctx.scope();
    SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDecl_opt(&lal_discr, second_discr, ctx);

    if (discr)
    {
      parentScope = discr->get_discriminantScope();
    }

    SgScopeStatement&       scope  = SG_DEREF(parentScope);
    SgDeclarationStatement& sgdecl = createOpaqueDecl(ident, lal_element, ctx.scope(scope));
    SgDeclarationStatement* assocdecl = nullptr;

    attachSourceLocation(sgdecl, lal_element, ctx);
    privatize(sgdecl, isPrivate);
    int hash = hash_node(lal_element);
    recordNode(libadalangTypes(), hash, sgdecl);

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

    //If this is a derived type, get the inherited elements
    //TODO Other lal_type_def_kinds may still inherit?
    if(lal_type_def_kind == ada_derived_type_def){ 
      SgDeclarationStatement* tydcl = discr ? discr : &sgdecl;

      TypeData ty = getTypeFoundation(ident, &lal_type_def, ctx);

      processInheritedElementsOfDerivedTypes(ty, *tydcl, ctx);
    }

    return SG_DEREF(assocdecl);
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
  createInit(SgInitializedNamePtrList& lst, SgExpression* exp)
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
  /// \param m        a map that maintains mappings between libadalang hashes and variables/parameters
  /// \param names    the list of libadalang names
  /// \param dcltype  the type of all initialized name
  /// \param initexpr the initializer (if it exists) that will be cloned for each
  ///                 of the initialized names.
  /// @{
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

    if(ada_node_is_null(lal_name_list)){
      //Add a dummy blank name to the list & return
      const std::string name = "";
      SgExpression*      init = createInit(lst, initexpr);
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, init);
     //attachSourceLocation(dcl, &lal_obj, ctx);
      lst.push_back(&dcl);
      return lst;
    }

    int count = ada_node_children_count(lal_name_list);
    //Iterate over the list of defining names
    for(int i = 0; i < count; ++i)
    {
      ada_base_entity lal_obj;

      if (ada_node_child(lal_name_list, i, &lal_obj) == 0){
        logError() << "Error while getting a child in constructInitializedNamePtrList.\n";
        return lst;
      }
      if(!ada_node_is_null(&lal_obj)){
        //Get the name of this decl
        ada_base_entity    lal_identifier;
        ada_defining_name_f_name(&lal_obj, &lal_identifier);
        const std::string  name = canonical_text_as_string(&lal_identifier);
        SgExpression*      init = createInit(lst, initexpr);
        SgInitializedName& dcl  = mkInitializedName(name, dcltype, init);

        attachSourceLocation(dcl, &lal_obj, ctx);

        lst.push_back(&dcl);
        int hash = hash_node(&lal_obj);
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
  /// @}

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
        logWarn() << "Unknown mode " << kind << " in getMode.\n";
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
    //Get the default expr based on the kind
    if(kind == ada_object_decl){
      ada_object_decl_f_default_expr(lal_decl, &default_expr);
    } else if(kind == ada_param_spec){
      ada_param_spec_f_default_expr(lal_decl, &default_expr);
    } else if(kind ==ada_component_decl){
      ada_component_decl_f_default_expr(lal_decl, &default_expr);
    } else if(kind == ada_number_decl){
      ada_number_decl_f_expr(lal_decl, &default_expr);
    } else if(kind == ada_discriminant_spec){
      ada_discriminant_spec_f_default_expr(lal_decl, &default_expr);
    } else {
      logWarn() << "getVarInit called with unhandled kind: " << kind << std::endl;
      return nullptr;
    }

    if(ada_node_is_null(&default_expr)){
      return nullptr;
    }

    return &getExpr(&default_expr, ctx);
  }

  /// Get the type of a variable/object decl
  SgType&
  getVarType(ada_base_entity* lal_decl, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_decl);
    ada_base_entity subtype_indication;
    if(kind == ada_object_decl){
      ada_object_decl_f_type_expr(lal_decl, &subtype_indication);
    } else if(kind == ada_component_decl){
      ada_component_decl_f_component_def(lal_decl, &subtype_indication);
      ada_component_def_f_type_expr(&subtype_indication, &subtype_indication);
    } else {
      logWarn() << "Unhandled kind " << kind << " in getVarType.\n";
    }

    return getDefinitionType(&subtype_indication, ctx);
  }

namespace {
  /// Creates a record decl
  ///   \param nondefdcl full decl for this record, nullptr if this is the complete decl
  SgClassDeclaration&
  createRecordDecl( const std::string& name,
                    SgClassDefinition& def,
                    SgScopeStatement& scope,
                    SgDeclarationStatement* nondefdcl
                  )
  {
    if(SgClassDeclaration* recdcl = isSgClassDeclaration(nondefdcl))
      return mkRecordDecl(*recdcl, def, scope);

    // if nondefdcl is set, it must be a SgClassDeclaration
    if (nondefdcl)
      logFlaw() << name << " " << typeid(*nondefdcl).name() << " not a class declaration!"
                << std::endl;

    //ADA_ASSERT (!nondefdcl);
    return mkRecordDecl(name, def, scope);
  }

  // returns a function declaration statement for a declaration statement
  //   checks if the function is an Ada generic function, where the declaration
  //   is hidden under an SgAdaGenericDecl.
  SgFunctionDeclaration* getFunctionDeclaration(SgDeclarationStatement* dcl)
  {
    // PP: 2/6/22 refactored function out of handleDeclaration
    if (SgFunctionDeclaration* fndcl  = isSgFunctionDeclaration(dcl))
      return fndcl;

    // MS: 7/26/2021 This point may be reached for both regular and
    // generic subprograms.  If it is a generic subprogram that was
    // declared, then we'll need to get the declaration out of the
    // SgAdaGenericDecl node.
    if (SgAdaGenericDecl* generic = isSgAdaGenericDecl(dcl))
      return isSgFunctionDeclaration(generic->get_declaration());

    return nullptr;
  }

  /// retrieves package spec declarations, similar to getFunctionDeclaration
  SgAdaPackageSpecDecl* getAdaPackageSpecDecl(SgDeclarationStatement* dcl)
  {
    if (SgAdaPackageSpecDecl* spcdcl  = isSgAdaPackageSpecDecl(dcl))
      return spcdcl;

    if (SgAdaGenericDecl* generic = isSgAdaGenericDecl(dcl))
      return isSgAdaPackageSpecDecl(generic->get_declaration());

    return nullptr;
  }

  /// handles an entry specification
  struct EntryIndexCompletion
  {
      EntryIndexCompletion(ada_base_entity* lal_entry_spec, AstContext astctx)
      : lal_element(lal_entry_spec), ctx(astctx)
      {}

      SgInitializedName& operator()(SgScopeStatement& scope)
      {
        std::string     name;
        SgType*         ty   = &mkTypeVoid();

        if(!ada_node_is_null(lal_element))
        {
          ada_node_kind_enum kind = ada_node_kind(lal_element);
          logKind("ada_entry_index_spec", kind);

          //Get the subtype
          ada_base_entity lal_subtype;
          ada_entry_index_spec_f_subtype(lal_element, &lal_subtype);

          //Get the name
          ada_base_entity lal_id;
          ada_entry_index_spec_f_id(lal_element, &lal_id);
          ada_defining_name_f_name(&lal_id, &lal_id);

          name = canonical_text_as_string(&lal_id);
          ty   = &getDefinitionType(&lal_subtype, ctx);
        }

        SgInitializedName&     ini = mkInitializedName(name, SG_DEREF(ty));
        SgVariableDeclaration& var = mkVarDecl(ini, scope);

        if (!ada_node_is_null(lal_element))
        {
          attachSourceLocation(var, lal_element, ctx);
        }

        return ini;
      }

    private:
      ada_base_entity*  lal_element;
      AstContext        ctx;
  };

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

  /// Create a decl for a var with one or more defining names & a return type
  SgDeclarationStatement&
  handleNumVarCstDecl( ada_base_entity* lal_element,
                       AstContext ctx,
                       bool isPrivate,
                       SgType& dclType,
                       SgType* expectedType = nullptr
                     )
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    //Get the list of defining names
    ada_base_entity          ids;
    SgScopeStatement&        scope    = ctx.scope();
    if(kind == ada_object_decl){
      ada_object_decl_f_ids(lal_element, &ids);
    } else if(kind == ada_component_decl){
      ada_component_decl_f_ids(lal_element, &ids);
    } else if(kind == ada_number_decl){
      ada_number_decl_f_ids(lal_element, &ids);
    }
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

  /// Create a const decl
  SgDeclarationStatement&
  handleNumberDecl(ada_base_entity* lal_element, AstContext ctx, bool isPrivate, SgType& numty, SgType& expctty)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    return handleNumVarCstDecl(lal_element, ctx, isPrivate, cstty, &expctty);
  }

  /// Create a decl for a var which may be aliased
  SgDeclarationStatement&
  handleVarCstDecl( ada_base_entity* lal_element,
                    AstContext ctx,
                    bool isPrivate,
                    TypeModifierFn constMaker
                  )
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    SgType& basety = constMaker(getVarType(lal_element, ctx));

    //Get the aliased status
    ada_base_entity has_aliased;
    if(kind == ada_object_decl){
      ada_object_decl_f_has_aliased(lal_element, &has_aliased);
    } else if(kind == ada_component_decl){
      ada_component_decl_f_component_def(lal_element, &has_aliased);
      ada_component_def_f_has_aliased(&has_aliased, &has_aliased);
    }
    ada_node_kind_enum aliased_status = ada_node_kind(&has_aliased);
    const bool               aliased  = (aliased_status == ada_aliased_present);
    SgType& varty  = aliased ? mkAliasedType(basety) : basety;

    return handleNumVarCstDecl(lal_element, ctx, isPrivate, varty);
  }

  void nothingToComplete(AstContext::PragmaContainer pragmas)
  {
     //ADA_ASSERT(pragmas.empty());
  }

  /// Handle a task, and the pragmas associated with it
  /// @{
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
      for(int i = 0; i < range; ++i){
        ada_base_entity lal_entry;
        ada_node_child(&public_part, i, &lal_entry);
        handleElement(&lal_entry, ctx.scope(*nodePtr));
      }
    }

    //Get the private part
    if(!ada_node_is_null(&private_part)){
      ada_declarative_part_f_decls(&private_part, &private_part);

      int range = ada_node_children_count(&private_part);
      for(int i = 0; i < range; ++i){
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
                          for(int i =0; i < range; ++i){
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
                          for(int i =0; i < range; ++i){
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

  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpecForSingleTask(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    if(kind != ada_single_task_decl){
      logError() << "getTaskSpecForSingleTask given node of kind " << kind << "!\n";
    }

    //Get the definition
    ada_base_entity lal_definition;
    ada_single_task_decl_f_task_type(lal_element, &lal_definition);

    return getTaskSpec_opt(&lal_definition, ctx);
  }
  /// @}

  /// @{
  // protected objects
  //
  // protected objects are represented as ClassDeclaration and ClassDefinition

  std::pair<SgAdaProtectedSpec*, DeferredPragmaBodyCompletion>
  getProtectedSpec(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    if(kind != ada_protected_def){
      logFlaw() << "getProtectedSpec called with kind = " << kind << std::endl;
    } else {
      logKind("ada_protected_def", kind);
    }

    //Get the private status
    ada_base_entity lal_private_list;
    ada_protected_def_f_private_part(lal_element, &lal_private_list);
    ada_declarative_part_f_decls(&lal_private_list, &lal_private_list);

    SgAdaProtectedSpec&     sgnode = mkAdaProtectedSpec();
    SgAdaProtectedSpec*     nodePtr  = &sgnode;

    sgnode.set_hasPrivate(!ada_node_is_null(&lal_private_list) && ada_node_children_count(&lal_private_list) > 0);

    /*auto deferred = [ctx,nodePtr,protectedNode](AstContext::PragmaContainer pragmas) -> void
                    {
                      AstContext pragmaCtx = ctx.pragmas(pragmas);

                      // visible items
                      {
                        //Get the public part
                        ada_base_entity lal_public_part;
                        ada_protected_def_f_public_part(lal_element, &lal_public_part);
                        ada_declarative_part_f_decls(&lal_public_part, &lal_public_part);
                        int count = ada_node_children_count(&lal_public_part);

                        //Call getElement on each public decl
                        for(int i = 0; i < count; ++i){
                          ada_base_entity lal_decl;
                          if(ada_node_child(&lal_public_part, i, &lal_decl) != 0){
                            handleElement(&lal_decl, pragmaCtx.scope(*nodePtr));
                          }
                        }
                      }

                      // private items
                      {
                        //Get the private part
                        ada_base_entity lal_private_part;
                        ada_protected_def_f_private_part(lal_element, &lal_private_part);
                        ada_declarative_part_f_decls(&lal_private_part, &lal_private_part);
                        int count = ada_node_children_count(&lal_private_part);

                        //Call getElement on each private decl
                        for(int i = 0; i < count; ++i){
                          ada_base_entity lal_decl;
                          if(ada_node_child(&lal_private_part, i, &lal_decl) != 0){
                            handleElement(&lal_decl, pragmaCtx.scope(*nodePtr), true /* private items *//*);
                          }
                        }
                      }

                      placePragmas({ nodePtr }, pragmaCtx.scope(*nodePtr));
                    };*/ //TODO pragmas

    //Get the public part
    ada_base_entity lal_public_part;
    ada_protected_def_f_public_part(lal_element, &lal_public_part);
    ada_declarative_part_f_decls(&lal_public_part, &lal_public_part);
    int count = ada_node_children_count(&lal_public_part);

    //Call getElement on each public decl
    for(int i = 0; i < count; ++i){
      ada_base_entity lal_decl;
      if(ada_node_child(&lal_public_part, i, &lal_decl) != 0){
        handleElement(&lal_decl, ctx.scope(*nodePtr));
      }
    }

    //Get the private part
    ada_base_entity lal_private_part;
    ada_protected_def_f_private_part(lal_element, &lal_private_part);
    ada_declarative_part_f_decls(&lal_private_part, &lal_private_part);
    count = ada_node_children_count(&lal_private_part);

    //Call getElement on each private decl
    for(int i = 0; i < count; ++i){
      ada_base_entity lal_decl;
      if(ada_node_child(&lal_private_part, i, &lal_decl) != 0){
        handleElement(&lal_decl, ctx.scope(*nodePtr), true /* private items */);
      }
    }

    auto deferred = nothingToComplete;

    return std::make_pair(&sgnode, deferred);
  }

  std::pair<SgAdaProtectedSpec*, DeferredPragmaBodyCompletion>
  getProtectedSpecForProtectedType(ada_base_entity* lal_element, AstContext ctx)
  {
    //Get the definition
    ada_base_entity lal_definition;
    ada_protected_type_decl_f_definition(lal_element, &lal_definition);

    if(ada_node_is_null(&lal_definition)){
      return std::make_pair(&mkAdaProtectedSpec(), nothingToComplete);
    }

    return getProtectedSpec(&lal_definition, ctx);
  }


  std::pair<SgAdaProtectedSpec*, DeferredPragmaBodyCompletion>
  getProtectedSpecForSingleProtected(ada_base_entity* lal_element, AstContext ctx)
  {
    //Get the definition
    ada_base_entity lal_definition;
    ada_single_protected_decl_f_definition(lal_element, &lal_definition);

    return getProtectedSpec(&lal_definition, ctx);
  }
  /// @}

  /// Create a node for one path of a case stmt, and add it to \ref caseNode
  void createCaseStmtPath(ada_base_entity* lal_element, SgSwitchStatement& caseNode, AstContext ctx)
  {
    //Get the choices
    ada_base_entity lal_choices;
    ada_case_stmt_alternative_f_choices(lal_element, &lal_choices);

    //Get the stmts
    ada_base_entity lal_stmts;
    ada_case_stmt_alternative_f_stmts(lal_element, &lal_stmts);


    SgStatement*  sgnode      = nullptr;
    SgBasicBlock& block       = mkBasicBlock();

    //Handle the choices
    int num_choices = ada_node_children_count(&lal_choices);

    //Check if this is a when others choice
    bool when_others = false;
    if(num_choices == 1){
      ada_base_entity lal_solo_choice;
      ada_node_child(&lal_choices, 0, &lal_solo_choice);
      ada_node_kind_enum lal_solo_choice_kind = ada_node_kind(&lal_solo_choice);
      when_others = (lal_solo_choice_kind == ada_others_designator);
    }

    if(when_others){
      SgDefaultOptionStmt* whenOthersNode = &mkWhenOthersPath(block);

      caseNode.append_default(whenOthersNode);
      sgnode = whenOthersNode;
    } else {
      //If this isn't a "when others", get the options for ths path
      std::vector<SgExpression*> choices;
      for(int i = 0; i < num_choices; ++i){
        ada_base_entity lal_choice;
        if(ada_node_child(&lal_choices, i, &lal_choice) !=0){
          ada_node_kind_enum lal_choice_kind = ada_node_kind(&lal_choice);
          if(lal_choice_kind == ada_bin_op){ //TODO Is this right? What makes a def vs an expr?
            choices.push_back(&getDefinitionExpr(&lal_choice, ctx));
          } else {
            choices.push_back(&getExpr(&lal_choice, ctx));
          }
        }
      }

      // \todo reconsider the "reuse" of SgCommaOp
      //   SgCommaOp is only used to separate discrete choices in case-when
      SgExpression&              caseCond  = mkChoiceExpIfNeeded(std::move(choices));
      SgCaseOptionStmt*          whenNode  = &mkWhenPath(caseCond, block);

      caseNode.append_case(whenNode);
      sgnode = whenNode;
    }

    //Handle the stmts
    int num_stmts = ada_node_children_count(&lal_stmts);
    for(int i = 0; i < num_stmts; ++i){
      ada_base_entity lal_stmt;
      if(ada_node_child(&lal_stmts, i, &lal_stmt) != 0){
        handleStmt(&lal_stmt, ctx.scope(block));
      }
    }

    attachSourceLocation(*sgnode, lal_element, ctx);
  }

  /// Handles the stmts for one branch of an if stmt
  void ifStmtCommonBranch(ada_base_entity* lal_path, SgIfStmt* ifStmt, AstContext ctx, void (SgIfStmt::*branchSetter)(SgStatement*))
  {
    SgBasicBlock& block     = mkBasicBlock();

    sg::linkParentChild(SG_DEREF(ifStmt), static_cast<SgStatement&>(block), branchSetter);

    int count = ada_node_children_count(lal_path);
    for(int i = 0; i < count; ++i){
      ada_base_entity lal_stmt;
      if(ada_node_child(lal_path, i, &lal_stmt) != 0){
        handleStmt(&lal_stmt, ctx.scope(block));
      }
    }
  }

  /// Handles the condition, then stmts for one branch of an if stmt
  void ifStmtConditionedBranch(ada_base_entity* lal_condition, ada_base_entity* lal_path, SgIfStmt* ifStmt, AstContext ctx)
  {
    SgExpression& condExpr = getExpr(lal_condition, ctx);
    SgStatement&  condStmt = mkExprStatement(condExpr);

    sg::linkParentChild(SG_DEREF(ifStmt), condStmt, &SgIfStmt::set_conditional);
    ifStmtCommonBranch(lal_path, ifStmt, ctx, &SgIfStmt::set_true_body);
  }

  /// Creates a new symbol for \ref derivedType based on a symbol from \ref baseType
  void createInheritedSymbol(ada_base_entity* lal_element, SgNamedType& baseType, SgNamedType& derivedType, AstContext ctx)
  {
    logTrace() << "In createInheritedSymbol\n";
    int                           hash   = hash_node(lal_element);
    SgDeclarationStatement*       fndcl  = findFirst(libadalangDecls(), hash);
    SgFunctionDeclaration*        fn     = isSgFunctionDeclaration(fndcl);

    if (fn == nullptr)
    {
      logFlaw() << "unable to find function with hash " << hash << std::endl;
      return;
    }

    SgFunctionSymbol*             fnsym  = findFirst(inheritedSymbols(), std::make_pair(fn, &baseType));
    if (fnsym == nullptr) fnsym = isSgFunctionSymbol(fn->search_for_symbol_from_symbol_table());

    if (fnsym == nullptr)
    {
      logFlaw() << "unable to find (derived) function symbol for " << fn->get_name() << std::endl;
      return;
    }

    SgDeclarationStatement const* symdcl = fnsym->get_declaration();
    if (symdcl && (symdcl->get_firstNondefiningDeclaration() != fn->get_firstNondefiningDeclaration()))
    {
      logFlaw() << fn->get_name() << " sym/dcl mismatch"
                << std::endl;
    }

    SgAdaInheritedFunctionSymbol& sgnode = mkAdaInheritedFunctionSymbol(*fnsym, derivedType, ctx.scope());
    const auto inserted = inheritedSymbols().insert(std::make_pair(InheritedSymbolKey{fn, &derivedType}, &sgnode));

    ROSE_ASSERT(inserted.second);
  }

  /// Returns the hash for the label referenced by \ref lal_element
  int getLabelRef(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    if(kind == ada_dotted_name)
    {
      logKind("ada_dotted_name", kind);
      //Get the suffix
      ada_base_entity lal_suffix;
      ada_dotted_name_f_suffix(lal_element, &lal_suffix);
      return getLabelRef(&lal_suffix, ctx);
    }

    logKind("ada_identifier", kind);
    ada_base_entity lal_label_decl;
    ada_expr_p_first_corresponding_decl(lal_element, &lal_label_decl);
    return hash_node(&lal_label_decl);
  }

} //End unnamed namespace


/// converts a libadalang parameter declaration to a ROSE parameter (i.e., variable)
///   declaration.
SgVariableDeclaration&
getParm(ada_base_entity* lal_param_spec, AstContext ctx)
{
  logTrace() << "getParm called\n";

  ada_base_entity defining_name_list;
  ada_param_spec_f_ids(lal_param_spec, &defining_name_list);

  ada_base_entity has_aliased;
  ada_param_spec_f_has_aliased(lal_param_spec, &has_aliased);
  ada_node_kind_enum aliased_status = ada_node_kind(&has_aliased);
  const bool               aliased  = (aliased_status == ada_aliased_present);

  //Get the type of this parameter
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

  return sgnode;
}

/// Add the parameters in range to \ref lst
void ParameterCompletion::operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope)
{
  if(!ada_node_is_null(range)){
    ada_node_kind_enum range_kind = ada_node_kind(range);
    ada_base_entity param_list;
    if(range_kind == ada_params){
      ada_params_f_params(range, &param_list);
    } else if(range_kind == ada_entry_completion_formal_params){
      ada_entry_completion_formal_params_f_params(range, &param_list);
    }

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

/// Find the first corresponding decl for \ref lal_element
SgDeclarationStatement*
queryDecl(ada_base_entity* lal_element, int defining_name_hash, AstContext /*ctx*/)
{
  ada_node_kind_enum kind = ada_node_kind(lal_element);

  ada_base_entity corresponding_decl;
  ada_expr_p_first_corresponding_decl(lal_element, &corresponding_decl);
  int decl_hash = hash_node(&corresponding_decl);

  SgDeclarationStatement* res = findFirst(libadalangDecls(), decl_hash, defining_name_hash);

  if((res == nullptr) && (kind == ada_identifier))
  {
    res = findFirst(adaPkgs(), decl_hash, defining_name_hash);
  }

  return res;
}

void handleStmt(ada_base_entity* lal_stmt, AstContext ctx, const std::string& lblname)
  {
    //~using PragmaContainer = AstContext::PragmaContainer;
    
    //Get the kind of this node
    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_stmt);

    LibadalangText kind_name(kind);
    std::string kind_name_string = kind_name.string_value();
    logTrace() << "handleStmt called on a " << kind_name_string << std::endl;

    SgStatement*            assocstmt = nullptr;
    //Statement_Struct&       stmt = elem.The_Union.Statement; //Same as lal_stmt now
    //ElemIdRange             pragmaRange  = idRange(stmt.Corresponding_Pragmas);
    //std::vector<Element_ID> pragmaVector;

    //std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));

    switch(kind){
      case ada_null_stmt:                    // 5.1
        {
          logKind("ada_null_stmt", kind);

          SgNullStatement& sgnode = mkNullStatement();

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_assign_stmt:             // 5.2
        {
          logKind("ada_assign_stmt", kind);

          //Get the destination & expr to assign
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
      case ada_if_stmt:                     // 5.3
        {
          logKind("ada_if_stmt", kind);

          SgIfStmt&   sgnode = mkIfStmt();
          SgIfStmt*   ifStmt = &sgnode;

          completeStmt(sgnode, lal_stmt, ctx, lblname);

          //Get the first if stmt
          ada_base_entity lal_condition;
          ada_base_entity lal_path;
          ada_if_stmt_f_cond_expr(lal_stmt, &lal_condition);
          ada_if_stmt_f_then_stmts(lal_stmt, &lal_path);
          ifStmtConditionedBranch(&lal_condition, &lal_path, ifStmt, ctx);

          //Get any elsifs
          ada_base_entity lal_elsif_list;
          ada_if_stmt_f_alternatives(lal_stmt, &lal_elsif_list);
          int count = ada_node_children_count(&lal_elsif_list);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_alternative;
            if(ada_node_child(&lal_elsif_list, i, &lal_alternative) != 0){
              ada_elsif_stmt_part_f_cond_expr(&lal_alternative, &lal_condition);
              ada_elsif_stmt_part_f_stmts(&lal_alternative, &lal_path);
              SgIfStmt& cascadingIf = mkIfStmt(true /* elsif */);

              sg::linkParentChild( SG_DEREF(ifStmt),
                                   static_cast<SgStatement&>(cascadingIf),
                                   &SgIfStmt::set_false_body
                                 );
              ifStmt = &cascadingIf;
              ifStmtConditionedBranch(&lal_condition, &lal_path, ifStmt, ctx);
            }
          }

          //Get the else
          ada_if_stmt_f_else_stmts(lal_stmt, &lal_path);
          count = ada_node_children_count(&lal_path);
          if(count > 0){
            ifStmtCommonBranch(&lal_path, ifStmt, ctx, &SgIfStmt::set_false_body);
          }

          assocstmt = &sgnode;
          break;
        }
      case ada_case_stmt:                    // 5.4
        {
          logKind("ada_case_stmt", kind);

          //Get the case_expr
          ada_base_entity lal_case_expr;
          ada_case_stmt_f_expr(lal_stmt, &lal_case_expr);

          //Get the alternatives
          ada_base_entity lal_alternatives;
          ada_case_stmt_f_alternatives(lal_stmt, &lal_alternatives);

          SgExpression&      caseexpr = getExpr(&lal_case_expr, ctx);
          SgBasicBlock&      casebody = mkBasicBlock();
          SgSwitchStatement& sgnode   = mkAdaCaseStmt(caseexpr, casebody);

          completeStmt(sgnode, lal_stmt, ctx, lblname);

          int count = ada_node_children_count(&lal_alternatives);

          //Handle each alternative
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_case_stmt_alternative;
            if(ada_node_child(&lal_alternatives, i, &lal_case_stmt_alternative) != 0){
              createCaseStmtPath(&lal_case_stmt_alternative, sgnode, ctx.scope(casebody));
            }
          }

          assocstmt = &sgnode;
          break;
        }
      case ada_named_stmt:
        {
          logKind("ada_named_stmt", kind);
          //Get the name
          ada_base_entity lal_decl;
          ada_named_stmt_f_decl(lal_stmt, &lal_decl);
          ada_named_stmt_decl_f_name(&lal_decl, &lal_decl);
          ada_defining_name_f_name(&lal_decl, &lal_decl);
          std::string label_name = canonical_text_as_string(&lal_decl);

          //Get the stmt
          ada_base_entity lal_named_stmt;
          ada_named_stmt_f_stmt(lal_stmt, &lal_named_stmt);

          //Call handleStmt on the substmt, passing in the name
          handleStmt(&lal_named_stmt, ctx, label_name);

          break;
        }
      case ada_while_loop_stmt:              // 5.5
        {
          logKind("ada_while_loop_stmt", kind);

          //Get the spec
          ada_base_entity lal_spec;
          ada_base_loop_stmt_f_spec(lal_stmt, &lal_spec);
          ada_while_loop_spec_f_expr(&lal_spec, &lal_spec);

          //Get the stmts
          ada_base_entity lal_loop_stmt_list;
          ada_base_loop_stmt_f_stmts(lal_stmt, &lal_loop_stmt_list);

          SgExpression&   cond     = getExpr(&lal_spec, ctx);
          SgBasicBlock&   block    = mkBasicBlock();
          SgWhileStmt&    sgnode   = mkWhileStmt(cond, block);
          int             hash     = hash_node(lal_stmt);

          completeStmt(sgnode, lal_stmt, ctx, lblname);
          recordNode(ctx.labelsAndLoops().libadalangLoops(), hash, sgnode);

          /*PragmaContainer pendingPragmas;
          AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

          traverseIDs(adaStmts, elemMap(), StmtCreator{pragmaCtx.scope(block)});
          processAndPlacePragmas(stmt.Pragmas, { &block }, pragmaCtx); // pragmaCtx.scope(block) ?*/ //TODO Pragmas
          //Call handleStmt for each stmt in the loop
          int count = ada_node_children_count(&lal_loop_stmt_list);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_loop_stmt;
            if(ada_node_child(&lal_loop_stmt_list, i, &lal_loop_stmt) != 0){
              handleStmt(&lal_loop_stmt, ctx.scope(block));
            }
          }

          assocstmt = &sgnode;
          break;
        }
      case ada_loop_stmt:                    // 5.5
        {
          logKind("ada_loop_stmt", kind);

          SgBasicBlock&   block    = mkBasicBlock();
          SgAdaLoopStmt&  sgnode   = mkAdaLoopStmt(block);

          //Get the stmts
          ada_base_entity lal_loop_stmt_list;
          ada_base_loop_stmt_f_stmts(lal_stmt, &lal_loop_stmt_list);

          completeStmt(sgnode, lal_stmt, ctx, lblname);

          int hash = hash_node(lal_stmt);
          recordNode(ctx.labelsAndLoops().libadalangLoops(), hash, sgnode);

          /*PragmaContainer pendingPragmas;
          AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

          traverseIDs(adaStmts, elemMap(), StmtCreator{pragmaCtx.scope(block)});
          processAndPlacePragmas(stmt.Pragmas, { &block }, pragmaCtx); // pragmaCtx.scope(block) ?*/ //TODO Pragmas
          //Call handleStmt for each stmt in the loop
          int count = ada_node_children_count(&lal_loop_stmt_list);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_loop_stmt;
            if(ada_node_child(&lal_loop_stmt_list, i, &lal_loop_stmt) != 0){
              handleStmt(&lal_loop_stmt, ctx.scope(block));
            }
          }

          assocstmt = &sgnode;
          break;
        }
      case ada_for_loop_stmt:                // 5.5
        {
          logKind("ada_for_loop_stmt", kind);

          SgBasicBlock&          block  = mkBasicBlock();
          SgForStatement&        sgnode = mkForStatement(block);

          //Get the for loop's iter var
          ada_base_entity lal_loop_spec, lal_for_var;
          ada_base_loop_stmt_f_spec(lal_stmt, &lal_loop_spec);
          ada_for_loop_spec_f_var_decl(&lal_loop_spec, &lal_for_var);

          //Get the direction of this loop
          ada_base_entity lal_has_reverse;
          ada_for_loop_spec_f_has_reverse(&lal_loop_spec, &lal_has_reverse);
          ada_node_kind_enum lal_has_reverse_kind = ada_node_kind(&lal_has_reverse);
          bool isForwardLoop = (lal_has_reverse_kind == ada_reverse_absent);

          //Get the stmts
          ada_base_entity lal_loop_stmt_list;
          ada_base_loop_stmt_f_stmts(lal_stmt, &lal_loop_stmt_list);

          SgForInitStatement&    forini = SG_DEREF( sb::buildForInitStatement(sgnode.getStatementList()) );

          attachSourceLocation(forini, &lal_for_var, ctx);
          sg::linkParentChild(sgnode, forini, &SgForStatement::set_for_init_stmt);
          completeStmt(sgnode, lal_stmt, ctx, lblname);
          handleDeclaration(&lal_loop_spec, ctx.scope(sgnode), false);

          // this swap is needed, b/c SgForInitStatement is not a scope
          // and when the loop variable declaration is created, the declaration
          // is pushed to the wrong statement list.
          std::swap(forini.get_init_stmt(), block.get_statements());

          SgVariableDeclaration* inductionVar = isSgVariableDeclaration(forini.get_init_stmt().front());
          SgExpression&          direction    = mkForLoopIncrement(isForwardLoop, SG_DEREF(inductionVar));
          sg::linkParentChild(sgnode, direction, &SgForStatement::set_increment);

          // test is not strictly necessary; added for convenience
          SgStatement&           test    = mkForLoopTest(isForwardLoop, SG_DEREF(inductionVar));
          sg::linkParentChild(sgnode, test, &SgForStatement::set_test);

          int hash = hash_node(lal_stmt);
          recordNode(ctx.labelsAndLoops().libadalangLoops(), hash, sgnode);

          /*PragmaContainer pendingPragmas;
          AstContext      pragmaCtx = ctx.pragmas(pendingPragmas);

          // loop body
          {
            ElemIdRange            loopStmts = idRange(stmt.Loop_Statements);

            traverseIDs(loopStmts, elemMap(), StmtCreator{pragmaCtx.scope(block)});
          }

          processAndPlacePragmas(stmt.Pragmas, { &block }, pragmaCtx); // pragmaCtx.scope(block) ?*/ //TODO pragmas
          //Call handleStmt for each stmt in the loop
          int count = ada_node_children_count(&lal_loop_stmt_list);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_loop_stmt;
            if(ada_node_child(&lal_loop_stmt_list, i, &lal_loop_stmt) != 0){
              handleStmt(&lal_loop_stmt, ctx.scope(block));
            }
          }

          assocstmt = &sgnode;
          break;
        }
      case ada_label:
        {
          logKind("ada_label", kind);

          //Make a null stmt for this label to attach to
          SgNullStatement& sgnode = mkNullStatement();

          completeStmt(sgnode, lal_stmt, ctx);

          //Add the label
          ada_base_entity lal_label_decl, lal_ident;
          ada_label_f_decl(lal_stmt, &lal_label_decl);
          ada_label_decl_f_name(&lal_label_decl, &lal_ident);
          ada_defining_name_f_name(&lal_ident, &lal_ident);
          std::string label_name = canonical_text_as_string(&lal_ident);

          //This code is similar to labelIfNeeded/labelStmt
          SgLabelStatement& sgn     = mkLabelStmt(label_name, sgnode, ctx.scope());
          attachSourceLocation(sgn, lal_stmt, ctx);
          ctx.appendStatement(sgn);

          //Record this label
          int hash = hash_node(&lal_label_decl);
          ctx.labelsAndLoops().label(hash, sgn);

          assocstmt = &sgnode;
          break;
        }
      case ada_begin_block:
      case ada_decl_block:                   // 5.6
        {
          logKind(kind == ada_decl_block ? "ada_decl_block" : "ada_begin_block", kind);

          SgBasicBlock& sgnode = mkBasicBlock();

          int hash = hash_node(lal_stmt);
          recordNode(libadalangBlocks(), hash, sgnode);

          //If this block is a named stmt, we also want to record it under its name
          if(lblname != ""){
            //Get the defining name & its hash; to do this, we go up to the ada_named_stmt, then get the name from there
            ada_base_entity lal_defining_name;
            ada_ada_node_parent(lal_stmt, &lal_defining_name);
            //Quick sanity check
            ada_node_kind_enum lal_parent_kind  = ada_node_kind(&lal_defining_name);
            if(lal_parent_kind != ada_named_stmt){
              logError() << "Block stmt has lblname, but parent is of kind " << lal_parent_kind << ", not ada_named_stmt!\n";
              break;
            }
            ada_named_stmt_f_decl(&lal_defining_name, &lal_defining_name);
            ada_named_stmt_decl_f_name(&lal_defining_name, &lal_defining_name);
            int name_hash = hash_node(&lal_defining_name);
            recordNode(libadalangBlocks(), name_hash, sgnode);
          }

          completeStmt(sgnode, lal_stmt, ctx, lblname);

          //Get the decls, stmts, exceptions, & pragmas of this block body
          ada_base_entity lal_decls, lal_stmts, lal_exceptions, lal_pragmas;

          if(kind == ada_decl_block){
            ada_decl_block_f_decls(lal_stmt, &lal_decls); //lal_decls should now be an ada_declarative_part
            ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
            ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
            ada_decl_block_f_stmts(lal_stmt, &lal_handled_stmts);
            ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
            ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
          } else {
            ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
            ada_begin_block_f_stmts(lal_stmt, &lal_handled_stmts);
            ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
            ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
          }

          completeDeclarationsWithHandledBlock( &lal_decls,
                                                &lal_stmts,
                                                &lal_exceptions,
                                                &lal_pragmas,
                                                simpleBlockHandler,
                                                simpleExceptionBlockHandler,
                                                sgnode,
                                                false /* same block for declarations and statements */,
                                                ctx
                                              );

          assocstmt = &sgnode;
          break;
        }
      case ada_exit_stmt:                   // 5.7
        {
          logKind("ada_exit_stmt", kind);

          //Determine the loop we are trying to exit
          ada_base_entity lal_loop_name;
          ada_exit_stmt_f_loop_name(lal_stmt, &lal_loop_name);
          int loop_hash;
          if(ada_node_is_null(&lal_loop_name)){
            //We don't have a name, so just go up the tree until we find a loop
            ada_base_entity lal_exit_parent;
            ada_ada_node_parent(lal_stmt, &lal_exit_parent);
            ada_node_kind_enum lal_exit_parent_kind = ada_node_kind(&lal_exit_parent);
            while(lal_exit_parent_kind != ada_loop_stmt && lal_exit_parent_kind != ada_for_loop_stmt && lal_exit_parent_kind != ada_while_loop_stmt){
              ada_ada_node_parent(&lal_exit_parent, &lal_exit_parent);
              lal_exit_parent_kind = ada_node_kind(&lal_exit_parent);
            } 
            loop_hash = hash_node(&lal_exit_parent);
          } else {
            //Figure out which loop this name matches
            ada_base_entity lal_loop;
            ada_expr_p_first_corresponding_decl(&lal_loop_name, &lal_loop);
            ada_ada_node_parent(&lal_loop, &lal_loop); //I hope named loops are always represented by ada_named_stmt, so this works
            ada_named_stmt_f_stmt(&lal_loop, &lal_loop);
            loop_hash = hash_node(&lal_loop);
          }

          //Get the exit condition
          ada_base_entity lal_exit_cond;
          ada_exit_stmt_f_cond_expr(lal_stmt, &lal_exit_cond);
          SgStatement&  exitedLoop    = lookupNode(ctx.labelsAndLoops().libadalangLoops(), loop_hash);
          SgExpression& exitCondition = getExpr_opt(&lal_exit_cond, ctx);
          const bool    loopIsNamed   = ada_node_is_null(&lal_loop_name) == 0;
          SgStatement&  sgnode        = mkAdaExitStmt(exitedLoop, exitCondition, loopIsNamed);

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_goto_stmt:                    // 5.8
        {
          logKind("ada_goto_stmt", kind);
          SgGotoStatement& sgnode = SG_DEREF( sb::buildGotoStatement() );

          //Get the label we will goto
          ada_base_entity lal_label;
          ada_goto_stmt_f_label_name(lal_stmt, &lal_label);

          ctx.labelsAndLoops().gotojmp(getLabelRef(&lal_label, ctx), sgnode);

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      //case An_Entry_Call_Statement:             // 9.5.3
      case ada_call_stmt:          // 6.4
        {
          logKind("ada_call_stmt", kind);

          //Get the call_expr node
          ada_base_entity lal_call_expr;
          ada_call_stmt_f_call(lal_stmt, &lal_call_expr);

          ada_node_kind_enum lal_call_expr_kind = ada_node_kind(&lal_call_expr);

          //We can't pass ada_base_entity as a func arg, so we will pass the pointers instead
          std::vector<ada_base_entity*> lal_args;
          std::vector<ada_base_entity> lal_args_backend;

          ada_base_entity lal_call_name;

          if(lal_call_expr_kind == ada_call_expr){
            //Get the args
            ada_base_entity lal_arg_list;
            ada_call_expr_f_suffix(&lal_call_expr, &lal_arg_list);
            int count = ada_node_children_count(&lal_arg_list);
            lal_args.resize(count);
            lal_args_backend.resize(count);
            for(int i = 0; i < count; ++i){
                if(ada_node_child(&lal_arg_list, i, &lal_args_backend.at(i)) == 0){
                  logError() << "Error while getting a child in handleStmt.\n";
                }
                lal_args.at(i) = &lal_args_backend.at(i);
            }

            //Get the name of the call
            ada_call_expr_f_name(&lal_call_expr, &lal_call_name);         
          } else if(lal_call_expr_kind == ada_dotted_name){
            //Get the arg (there can only be 1)
            lal_args.resize(1);
            lal_args_backend.resize(1);
            ada_dotted_name_f_prefix(&lal_call_expr, &lal_args_backend.at(0));
            lal_args.at(0) = &lal_args_backend.at(0);

            //Get the name
            ada_dotted_name_f_suffix(&lal_call_expr, &lal_call_name);
          } else {
            logFlaw() << "Unhandled call expr kind " << lal_call_expr_kind << " in handleStmt for ada_call_stmt.\n";
          }

          // oocall indicates if code uses object-oriented syntax: x.init instead of init(x)
          const bool       oocall = (lal_call_expr_kind == ada_dotted_name);
          SgExpression&    call   = createCall(&lal_call_name, lal_args, true /* prefix call */, oocall, ctx);
          SgExprStatement& sgnode = SG_DEREF(sb::buildExprStatement(&call));

          attachSourceLocation(call, lal_stmt, ctx);
          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_return_stmt:
        {
          logKind("ada_return_stmt", kind);

          //Get the expr
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
          //Get the name
          ada_base_entity lal_identifier;
          ada_accept_stmt_f_name(lal_stmt, &lal_identifier);
          SgExpression&            entryref = getExpr(&lal_identifier, ctx);
          SgExpression&            idx      = getExpr_opt(nullptr, ctx); //TODO What is Accept_Entry_Index?
          SgAdaAcceptStmt&         sgnode   = mkAdaAcceptStmt(entryref, idx);

          completeStmt(sgnode, lal_stmt, ctx);

          //Get the params
          ada_base_entity params;
          ada_accept_stmt_f_params(lal_stmt, &params);
          ada_entry_completion_formal_params_f_params(&params, &params);
          SgFunctionParameterList& paramlst = SG_DEREF(sgnode.get_parameterList());
          AstContext               parmctx  = ctx.scope(SG_DEREF(sgnode.get_parameterScope()));

          //If there are params, add them to paramlst
          int param_count = ada_node_is_null(&params) ? 0 : ada_node_children_count(&params);
          for(int i = 0; i < param_count; ++i){
            ada_base_entity param;
            if(ada_node_child(&params, i, &param) != 0){
              SgVariableDeclaration& decl = getParm(&param, parmctx);
              for(SgInitializedName* parm : decl.get_variables()){
                paramlst.append_arg(parm);
              }
            }
          }

          //Get the stmts, if they exist
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
      case ada_delay_stmt:             // 9.6
        {
          logKind("ada_delay_stmt", kind);

          //Get the until status of this node
          ada_base_entity lal_has_until;
          ada_delay_stmt_f_has_until(lal_stmt, &lal_has_until);
          bool has_until = (ada_node_kind(&lal_has_until) == ada_until_present);

          //Get the delay expression
          ada_base_entity lal_expr;
          ada_delay_stmt_f_expr(lal_stmt, &lal_expr);

          SgExpression&   delayexpr = getExpr(&lal_expr, ctx);
          SgAdaDelayStmt& sgnode    = mkAdaDelayStmt(delayexpr, !has_until);

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_raise_stmt:                   // 11.3
        {
          logKind("ada_raise_stmt", kind);
          //Get the exception
          ada_base_entity lal_exception;
          ada_raise_stmt_f_exception_name(lal_stmt, &lal_exception);

          //Get the message
          ada_base_entity lal_message;
          ada_raise_stmt_f_error_message(lal_stmt, &lal_message);

          SgExpression&   raised = getExpr_opt(&lal_exception, ctx);
          SgExpression*   msg    = !ada_node_is_null(&lal_message) ? &getExpr(&lal_message, ctx)
                                                                   : nullptr;
          SgStatement&    sgnode = mkRaiseStmt(raised, msg);

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      //Declarations //TODO Add more decls?
      case ada_entry_body:
        {
          handleDeclaration(lal_stmt, ctx, false);
          return;
        }
      default:
        {
          logWarn() << "Unhandled statement " << kind << std::endl;
          //ADA_ASSERT (!FAIL_ON_ERROR(ctx));
        }
    }

    //recordPragmasID(std::move(pragmaVector), assocstmt, ctx);
  }

  void handleExceptionHandler(ada_base_entity* lal_element, SgTryStmt& tryStmt, AstContext ctx)
  {
    //~using PragmaContainer = AstContext::PragmaContainer;

    //Get the kind of this node
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    if(kind != ada_exception_handler){
        logWarn() << "handleExceptionHandler given " << kind << std::endl;
    }

    logKind("ada_exception_handler", kind);
    
    //Get the name for this handler
    ada_base_entity lal_name;
    ada_exception_handler_f_exception_name(lal_element, &lal_name);

    std::string name = "";
    if(!ada_node_is_null(&lal_name)){
      ada_defining_name_f_name(&lal_name, &lal_name);
      name = canonical_text_as_string(&lal_name);
    }

    //Get the types of exceptions handled
    ada_base_entity lal_exception_choices;
    ada_exception_handler_f_handled_exceptions(lal_element, &lal_exception_choices);

    //Get the stmts
    ada_base_entity lal_stmts;
    ada_exception_handler_f_stmts(lal_element, &lal_stmts);
    SgType&                 extypes  = createExHandlerType(&lal_exception_choices, ctx);
    SgInitializedName&      initName = mkInitializedName(name, extypes, nullptr);
    SgBasicBlock&           body     = mkBasicBlock();

    if(!ada_node_is_null(&lal_name)){
      attachSourceLocation(initName, &lal_name, ctx);
    }

    SgCatchOptionStmt&       sgnode  = mkExceptionHandler(initName, body, tryStmt);

    sg::linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);
    sgnode.set_parent(tryStmt.get_catch_statement_seq_root());

    /*PragmaContainer pendingPragmas;
    AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

    traverseIDs(range, elemMap(), StmtCreator{pragmaCtx.scope(body)});*/ //TODO Pragmas
    //Handle the stmts for this excp
    int count = ada_node_children_count(&lal_stmts);
    for(int i = 0; i < count; ++i){
      ada_base_entity lal_stmt;
      if(ada_node_child(&lal_stmts, i, &lal_stmt) != 0){
        handleStmt(&lal_stmt, ctx.scope(body));
      }
    }

    computeSourceRangeFromChildren(body);
    attachSourceLocation(sgnode, lal_element, ctx);
    //processAndPlacePragmas(ex.Pragmas, { &body }, pragmaCtx); // pragmaCtx.scope(body) ?
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

  LibadalangText kind_name(kind);
  std::string kind_name_string = kind_name.string_value();
  logTrace()   << "handleDeclaration called on a " << kind_name_string << std::endl;
  
  //std::vector<Element_ID> pragmaVector;

  //std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));

  switch (kind)
  {

    case ada_package_decl:                    // 7.1(2)
      {
        logKind("ada_package_decl", kind);

        //Get the name for this package
        ada_base_entity lal_defining_name, lal_identifier;
        ada_base_package_decl_f_package_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

        std::string           ident        = canonical_text_as_string(&lal_identifier);
        SgScopeStatement*     parent_scope = &ctx.scope();

        SgAdaPackageSpecDecl& sgnode       = mkAdaPackageSpecDecl(ident, SG_DEREF(parent_scope));
        SgAdaPackageSpec&     pkgspec      = SG_DEREF(sgnode.get_definition());

        logTrace() << "package decl " << ident
                   << std::endl;

        int hash = hash_node(&lal_defining_name);
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
            for(int i = 0; i < range; ++i){
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
            for(int i = 0; i < range; ++i){
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

        std::string             ident        = canonical_text_as_string(&lal_identifier);
        SgScopeStatement*       parent_scope = &ctx.scope();

        //Get the decl for this package body
        ada_base_entity previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &previous_decl); //TODO imprecise fallback
        //Get the ada_defining_name node, since that is what we record in the map
        ada_base_package_decl_f_package_name(&previous_decl, &previous_decl);
        int decl_hash = hash_node(&previous_decl);
        SgDeclarationStatement& declnode = lookupNode(libadalangDecls(), decl_hash);
        SgAdaPackageSpecDecl*   specdcl  = getAdaPackageSpecDecl(&declnode);

        SgDeclarationStatement* ndef    = findFirst(libadalangDecls(), decl_hash);
        SgAdaPackageBodyDecl*   nondef  = isSgAdaPackageBodyDecl(ndef);

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
    case ada_generic_package_decl:            // 12.1(2)
      {
        logKind("ada_generic_package_decl", kind);

        //Get the name of this package
        ada_base_entity lal_package_internal, lal_defining_name, lal_identifier;
        ada_generic_package_decl_f_package_decl(lal_element, &lal_package_internal);
        ada_base_package_decl_f_package_name(&lal_package_internal, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);
        int hash = hash_node(&lal_defining_name);

        //Get the list of formal decls
        ada_base_entity lal_formal_list;
        ada_generic_decl_f_formal_part(lal_element, &lal_formal_list);
        ada_generic_formal_part_f_decls(&lal_formal_list, &lal_formal_list);

        // create generic declaration
        SgScopeStatement&       logicalScope = ctx.scope();
        SgAdaGenericDecl&       sgnode     = mkAdaGenericDecl(ident, logicalScope);
        SgAdaGenericDefn&       gen_defn   = SG_DEREF(sgnode.get_definition());

        // create package in the scope of the generic
        //~ SgAdaPackageSpecDecl&   pkgnode    = mkAdaPackageSpecDecl(adaname.ident, logicalScope);
        SgAdaPackageSpecDecl&   pkgnode    = mkAdaPackageSpecDecl(ident, gen_defn);
        SgAdaPackageSpec&       pkgspec    = SG_DEREF(pkgnode.get_definition());

        // set declaration component of generic decl to package decl
        sgnode.set_declaration(&pkgnode);
        pkgnode.set_parent(&gen_defn);

        // record ID to sgnode mapping
        recordNode(libadalangDecls(), hash, sgnode);

        // should private be set on the generic or on the package?
        //~ privatize(pkgnode, isPrivate);
        privatize(sgnode, isPrivate);
        //~ attachSourceLocation(pkgspec, elem, ctx);
        //~ attachSourceLocation(pkgnode, elem, ctx);
        //~ attachSourceLocation(gen_defn, elem, ctx);
        attachSourceLocation(sgnode, lal_element, ctx);

        ctx.appendStatement(sgnode);

        // generic formal part: this must be done first so the types defined in
        // the generic formal part exist when the package definition is processed.
        {
          int count = ada_node_children_count(&lal_formal_list);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_formal_part;
            if(ada_node_child(&lal_formal_list, i, &lal_formal_part) != 0){
              handleElement(&lal_formal_part, ctx.scope(gen_defn));
            }
          }
        }

        // can the formal part also have pragmas?
        PragmaContainer pendingPragmas;
        AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

        // visible items
        {
          ada_base_entity lal_public_list;
          ada_base_package_decl_f_public_part(&lal_package_internal, &lal_public_list);
          if(!ada_node_is_null(&lal_public_list)){
            ada_declarative_part_f_decls(&lal_public_list, &lal_public_list);
            int count = ada_node_children_count(&lal_public_list);
            for(int i = 0; i < count; ++i){
              ada_base_entity lal_public_part;
              if(ada_node_child(&lal_public_list, i, &lal_public_part) != 0){
                handleElement(&lal_public_part, pragmaCtx.scope(pkgspec));
              }
            }
          }
        }

        // private items
        {
          ada_base_entity lal_private_list;
          ada_base_package_decl_f_private_part(&lal_package_internal, &lal_private_list);
          if(!ada_node_is_null(&lal_private_list)){
            ada_declarative_part_f_decls(&lal_private_list, &lal_private_list);
            int count = ada_node_children_count(&lal_private_list);
            for(int i = 0; i < count; ++i){
              ada_base_entity lal_private_part;
              if(ada_node_child(&lal_private_list, i, &lal_private_part) != 0){
                handleElement(&lal_private_part, pragmaCtx.scope(pkgspec), true);
              }
            }
          }
        }

        //processAndPlacePragmas(decl.Pragmas, { &pkgspec }, pragmaCtx.scope(pkgspec)); //TODO Pragmas

        assocdecl = &sgnode;
        break;
      }
    case ada_generic_subp_decl:          // 12.1(2)
      {
        //Get whether this is a func or a proc
        ada_base_entity lal_subp_spec, lal_subp_type;
        ada_generic_subp_decl_f_subp_decl(lal_element, &lal_subp_spec);
        ada_generic_subp_internal_f_subp_spec(&lal_subp_spec, &lal_subp_spec); 
        ada_subp_spec_f_subp_kind(&lal_subp_spec, &lal_subp_type);
        ada_node_kind_enum lal_subp_type_kind = ada_node_kind(&lal_subp_type);

        const bool             isFunc  = (lal_subp_type_kind == ada_subp_kind_function);

        logKind( isFunc
                    ? "A_Generic_Function_Declaration"
                    : "A_Generic_Procedure_Declaration"
               , kind
               );

        //Get the name of this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_subp_spec_f_subp_name(&lal_subp_spec, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);

        SgScopeStatement&      logicalScope = ctx.scope();

        // PP (20/10/21): the assertion does not hold for proc/func defined in their own unit
        //~ ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaGenericDecl&      sgnode     = mkAdaGenericDecl(ident, logicalScope);
        SgAdaGenericDefn&      gen_defn   = SG_DEREF(sgnode.get_definition());

        // PP (20/10/21): use the logical scope
        //    the logical scope is the parent package in the package structure
        //    this could be different from the physical parent, for example when
        //    the generic proc/func forms its own subpackage.
        //~ outer.insert_symbol(adaname.ident, &mkBareNode<SgAdaGenericSymbol>(&sgnode));
        ctx.appendStatement(sgnode);

        // generic formal part
        {
          //Get the formal part
          ada_base_entity lal_formal_part;
          ada_generic_decl_f_formal_part(lal_element, &lal_formal_part);
          ada_generic_formal_part_f_decls(&lal_formal_part, &lal_formal_part);

          int count = ada_node_children_count(&lal_formal_part);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_formal_decl;
            if(ada_node_child(&lal_formal_part, i, &lal_formal_decl) != 0){
              handleElement(&lal_formal_decl, ctx.scope(gen_defn));
            }
          }
        }

        //Get the params & return type
        ada_base_entity lal_params, lal_return_type;
        ada_subp_spec_f_subp_returns(&lal_subp_spec, &lal_return_type);
        ada_subp_spec_f_subp_params(&lal_subp_spec, &lal_params);

        SgType&                rettype = isFunc ? getDeclType(&lal_return_type, ctx)
                                                : mkTypeVoid();

        // PP (10/20/21): changed scoping for packages and procedures/functions
        //                the generic proc/func is declared in the logical parent scope
        // was: SgFunctionDeclaration&  fundec     = mkProcedureDecl_nondef(adaname.fullName, gen_defn, rettype, ParameterCompletion{params, ctx});
        SgFunctionDeclaration&  fundec     = mkProcedureDecl_nondef( ident,
                                                                     gen_defn, //~ logicalScope,
                                                                     rettype,
                                                                     ParameterCompletion{&lal_params, ctx}
                                                                   );

        sgnode.set_declaration(&fundec);
        fundec.set_parent(&gen_defn);

        setOverride(fundec, false); //TODO Can this kind be overriding?

        int hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), hash, sgnode);

        // should private be set on the generic or on the proc?
        //~ privatize(fundec, isPrivate);
        privatize(sgnode, isPrivate);

        //~ attachSourceLocation(fundec, elem, ctx);
        attachSourceLocation(sgnode, lal_element, ctx);
        //~ attachSourceLocation(gen_defn, elem, ctx);

        assocdecl = &sgnode;
        break;
      }
    case ada_subp_decl:                   // 6.1(4)   -> Trait_Kinds
    case ada_abstract_subp_decl:
    //case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
      {
        logKind("ada_subp_decl?", kind);

        //Get the subp spec node & whether this decl is overriding another
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

        ada_node_kind_enum lal_overriding_kind = ada_node_kind(&lal_overriding);

        const bool              isFunc = subp_kind_kind == ada_subp_kind_function;
        const bool          overriding = (lal_overriding_kind == ada_overriding_overriding); //TODO ada_overriding_unspecified might count?
        SgScopeStatement* parent_scope = &ctx.scope();
        std::string              ident = canonical_text_as_string(&lal_identifier);
        SgType&                rettype = isFunc ? getDeclType(&subp_returns, ctx)
                                                : mkTypeVoid();

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
    case ada_null_subp_decl:             // 6.7
    case ada_subp_body:              // 6.3(2)
    //case A_Procedure_Body_Declaration:             // 6.3(2)
      {
        logKind("ada_subp_body?", kind);

        //Get the subp spec node
        ada_base_entity subp_spec;
        if(kind == ada_subp_body || kind == ada_null_subp_decl){
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

        std::string             ident        = canonical_text_as_string(&lal_identifier);
        const bool              isFunc       = (subp_kind_kind == ada_subp_kind_function);
        SgScopeStatement*       parent_scope = &ctx.scope();
        SgType&                 rettype      = isFunc ? getDeclType(&subp_returns, ctx)
                                                      : mkTypeVoid();

        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl);
        SgDeclarationStatement* ndef   = nullptr;
        SgFunctionDeclaration*  nondef = nullptr;

        //If this isn't the first decl, set up ndef & nondef
        if(!ada_node_is_null(&lal_previous_decl)){
          int decl_hash = hash_node(&lal_previous_decl);
          ndef          = findFirst(libadalangDecls(), decl_hash);
          nondef        = getFunctionDeclaration(ndef ? ndef->get_firstNondefiningDeclaration() : nullptr);
        }

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
        if(kind != ada_null_subp_decl) {
          completeRoutineBody(lal_element, declblk, ctx);
        }

        assocdecl = &sgnode;
        break;
      }
    case ada_generic_formal_type_decl:                // 12.5(2)
      {
        logKind("ada_generic_formal_type_decl", kind);

        //Get the name for this type
        ada_base_entity lal_type_decl, lal_defining_name, lal_identifier;
        ada_generic_formal_f_decl(lal_element, &lal_type_decl);
        ada_base_type_decl_f_name(&lal_type_decl, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

        std::string          ident = canonical_text_as_string(&lal_identifier);
        FormalTypeData          ty = getFormalTypeFoundation(ident, &lal_type_decl, ctx);
        SgScopeStatement&    scope = ctx.scope();
        int                   hash = hash_node(&lal_defining_name);

        //Get the previous def, if there is one
        ada_base_entity lal_previous_def;
        SgDeclarationStatement* nondef = nullptr;
        ada_base_type_decl_p_previous_part(&lal_type_decl, 1, &lal_previous_def);
        if(!ada_node_is_null(&lal_previous_def)){
          logInfo() << ada_node_kind(&lal_previous_def) << std::endl;
          ada_base_type_decl_f_name(&lal_previous_def, &lal_previous_def);
          int def_hash = hash_node(&lal_previous_def);
          nondef = findFirst(libadalangTypes(), def_hash);
        }

        SgDeclarationStatement& sgnode = ty.sageNode();

        setModifiers(sgnode, ty.isAbstract(), ty.isLimited(), ty.isTagged());

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);

        if (/*SgAdaDiscriminatedTypeDecl* discr =*/ isSgAdaDiscriminatedTypeDecl(&sgnode))
          /* do nothing */;
        else
          ctx.appendStatement(sgnode);

        recordNode(libadalangTypes(), hash, sgnode, nondef != nullptr);

        if(ty.inheritsRoutines()){
          processInheritedSubroutines(SG_DEREF(si::getDeclaredType(&sgnode)), ty.definitionStruct(), ctx);
        }

        assocdecl = &sgnode;
        break;
      }
    case ada_generic_formal_subp_decl:           // 12.6(2)
      {
        //Get whether this is a func or a proc, & the overriding status
        ada_base_entity lal_subp_spec, lal_subp_type, lal_overriding;
        ada_generic_formal_f_decl(lal_element, &lal_subp_spec);
        ada_classic_subp_decl_f_overriding(&lal_subp_spec, &lal_overriding); //TODO overriding isn't needed?
        ada_classic_subp_decl_f_subp_spec(&lal_subp_spec, &lal_subp_spec); 
        ada_subp_spec_f_subp_kind(&lal_subp_spec, &lal_subp_type);
        ada_node_kind_enum lal_subp_type_kind = ada_node_kind(&lal_subp_type);

        const bool             isFormalFuncDecl  = (lal_subp_type_kind == ada_subp_kind_function);

        logKind( isFormalFuncDecl ? "A_Formal_Function_Declaration" : "A_Formal_Procedure_Declaration"
               , kind
               );

        //Get the name of this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_subp_spec_f_subp_name(&lal_subp_spec, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);

        //Get the params & return type
        ada_base_entity lal_params, lal_return_type;
        ada_subp_spec_f_subp_returns(&lal_subp_spec, &lal_return_type);
        ada_subp_spec_f_subp_params(&lal_subp_spec, &lal_params);

        SgType&                rettype = isFormalFuncDecl ? getDeclType(&lal_return_type, ctx)
                                                          : mkTypeVoid();

        SgScopeStatement&      logicalScope = ctx.scope();
        SgAdaSubroutineType&   funty   = mkAdaSubroutineType(rettype, ParameterCompletion{&lal_params, ctx}, ctx.scope(), false  /*isProtected*/ );
        SgExpression&          defaultInit = mkNullExpression();//getDefaultFunctionExpr(lal_element, funty, ctx); TODO Find this in the lal tree
        SgAdaRenamingDecl&     sgnode  = mkAdaRenamingDecl(ident, defaultInit, funty, logicalScope);

        int hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), hash, sgnode);

        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_subtype_decl:                    // 3.2.2(2)
      {
        logKind("ada_subtype_decl", kind);

        //Get the name
        ada_base_entity lal_identifier;
        ada_base_type_decl_f_name(lal_element, &lal_identifier);
        ada_defining_name_f_name(&lal_identifier, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);

        //Get the subtype indication
        ada_base_entity lal_subtype_indication;
        ada_subtype_decl_f_subtype(lal_element, &lal_subtype_indication);

        int                   hash = hash_node(lal_element);
        const bool            forceSubtype = true;
        SgType&               subtype = getDefinitionType(&lal_subtype_indication, ctx, forceSubtype);
        SgScopeStatement&     scope   = ctx.scope();
        SgTypedefDeclaration& sgnode  = mkTypeDecl(ident, subtype, scope);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);
        recordNode(libadalangTypes(), hash, sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_number_decl:            // 3.3.2(2), 3.5.6(2)
      {
        logKind("ada_number_decl", kind);

        //Get the expr to see what kind it is (int, real, etc.)
        ada_base_entity lal_expr;
        ada_number_decl_f_expr(lal_element, &lal_expr);

        SgType* expected_type = getNumberDeclType(&lal_expr);

        assocdecl = &handleNumberDecl(lal_element, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), SG_DEREF(expected_type));

        break;
      }
    case ada_protected_body:             // 9.4(7)
      {
        logKind("ada_protected_body", kind);

        //Get the defining name of the decl
        ada_base_entity lal_defining_name;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_defining_name);
        ada_single_protected_decl_f_name(&lal_defining_name, &lal_defining_name);
        int decl_hash = hash_node(&lal_defining_name);

        SgAdaProtectedBody&     pobody  = mkAdaProtectedBody();
        SgDeclarationStatement* ndef    = findFirst(libadalangDecls(), decl_hash);
        SgAdaProtectedBodyDecl* nondef  = isSgAdaProtectedBodyDecl(ndef);

        //Element_ID              specID  = queryAsisIDOfDeclaration(decl, A_Protected_Body_Stub, ctx);
        SgDeclarationStatement& podecl  = lookupNode(libadalangDecls(), decl_hash);
        SgScopeStatement&       logicalScope = SG_DEREF(&ctx.scope());
        SgAdaProtectedBodyDecl& sgnode  = mkAdaProtectedBodyDecl(podecl, nondef, pobody, logicalScope);

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        ada_base_entity lal_body_name;
        ada_protected_body_f_name(lal_element, &lal_body_name);
        int body_hash = hash_node(&lal_body_name);

        recordNode(libadalangDecls(), body_hash, sgnode);

        //Get the decls
        ada_base_entity lal_decl_list;
        ada_protected_body_f_decls(lal_element, &lal_decl_list);
        ada_declarative_part_f_decls(&lal_decl_list, &lal_decl_list);
        int count = ada_node_children_count(&lal_decl_list);

        //Call handleStmt on each decl
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_decl;
          if(ada_node_child(&lal_decl_list, i, &lal_decl) != 0){
            handleStmt(&lal_decl, ctx.scope(pobody));
          }
        }

        /*PragmaContainer pendingPragmas;
        AstContext      pragmaCtx  = ctx.pragmas(pendingPragmas);

        {
          ElemIdRange decls = idRange(decl.Protected_Operation_Items);

          traverseIDs(decls, elemMap(), StmtCreator{pragmaCtx.scope(pobody)});
        }

        processAndPlacePragmas(decl.Pragmas, { &pobody }, pragmaCtx.scope(pobody));*/ //TODO pragmas

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

        //Get the hash for the decl
        ada_base_entity p_decl_part, lal_decl_defining_name;
        ada_body_node_p_decl_part(lal_element, 1, &p_decl_part);
        ada_node_kind_enum decl_kind = ada_node_kind(&p_decl_part);
        if(decl_kind == ada_single_task_decl){
          ada_single_task_decl_f_task_type(&p_decl_part, &lal_decl_defining_name);
          ada_base_type_decl_f_name(&lal_decl_defining_name, &lal_decl_defining_name);
        } else if(decl_kind == ada_task_type_decl){
          ada_base_type_decl_f_name(&p_decl_part, &lal_decl_defining_name);
        } else {
          logError() << "ada_task_body has decl of unhandled kind: " << kind << "!\n";
        }

        int                          hash = hash_node(lal_element);
        int                     decl_hash = hash_node(&lal_decl_defining_name);
        std::string             ident     = canonical_text_as_string(&lal_identifier);
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
    case ada_entry_body:                // 9.5.2(5)
      {
        logKind("ada_entry_body", kind);

        //Get the defining name for the ada_entry_decl
        ada_base_entity lal_decl_name;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_decl_name);
        ada_entry_decl_f_spec(&lal_decl_name, &lal_decl_name);
        ada_entry_spec_f_entry_name(&lal_decl_name, &lal_decl_name);
        int decl_hash = hash_node(&lal_decl_name);

        //Get the barrier
        ada_base_entity lal_barrier;
        ada_entry_body_f_barrier(lal_element, &lal_barrier);

        //Get the defining name
        ada_base_entity lal_defining_name;
        ada_entry_body_f_entry_name(lal_element, &lal_defining_name);
        int body_hash = hash_node(&lal_defining_name);

        //Get the params
        ada_base_entity lal_params;
        ada_entry_body_f_params(lal_element, &lal_params);

        //Get the index spec
        ada_base_entity lal_index_spec;
        ada_entry_body_f_index_spec(lal_element, &lal_index_spec);

        SgDeclarationStatement* sagedcl  = findFirst(libadalangDecls(), decl_hash);
        SgAdaEntryDecl&         entrydcl = SG_DEREF(isSgAdaEntryDecl(sagedcl));

        //~ SgScopeStatement&       logicalScope = adaname.parent_scope();
        //~ SgAdaEntryDecl&         sgnode  = mkAdaEntryDef(entrydcl, logicalScope, ParameterCompletion{range, ctx});
        //ElemIdRange             secondaryIDs = secondaryParameters(decl, ctx); TODO secondary params?
        //std::vector<Element_ID> secondaries  = reverseElems(flattenNameLists(secondaryIDs, ctx));

        // PP (1/20/23): *SCOPE_COMMENT_1
        //               replace outer with entrydcl.get_scope()
        //               note, this will have entries use the same scope handling as
        //                     functions and procedures.
        //                     Not sure if this is correct, because by giving a definition
        //                     in the body the scope of a spec, will make symbols
        //                     in the body invisible, unless the physical scope is used
        //                     for lookup.
        SgAdaEntryDecl&         sgnode  = mkAdaEntryDefn( entrydcl,
                                                          SG_DEREF(entrydcl.get_scope()), // was: ctx.scope(),
                                                          ParameterCompletion{&lal_params, ctx},
                                                          EntryIndexCompletion{&lal_index_spec, ctx}
                                                        );

        recordNode(libadalangDecls(), body_hash, sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);

        ctx.appendStatement(sgnode);

        SgFunctionDefinition&   fndef   = SG_DEREF(sgnode.get_definition());
        SgBasicBlock&           declblk = SG_DEREF(fndef.get_body());

        //~ if (isInvalidId(decl.Entry_Barrier))
        //~ logError() << "Entry_Barrier-id " << decl.Entry_Barrier << std::endl;

        SgExpression&           barrier = getExpr_opt(&lal_barrier, ctx.scope(fndef));

        sg::linkParentChild(sgnode, barrier, &SgAdaEntryDecl::set_entryBarrier);
        completeRoutineBody(lal_element, declblk, ctx);

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

        std::string             ident        = canonical_text_as_string(&lal_identifier);
        SgScopeStatement*       parent_scope = &ctx.scope();
        SgType&                 rettype      = isFunc ? getDeclType(&subp_returns, ctx)
                                                      : mkTypeVoid();

        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl);
        SgDeclarationStatement* ndef    = nullptr;
        SgFunctionDeclaration*  nondef  = nullptr;

        //If this isn't the first decl, set up nondef
        if(!ada_node_is_null(&lal_previous_decl)){
          int decl_hash = hash_node(&lal_previous_decl);
          ndef          = findFirst(libadalangDecls(), decl_hash);
          nondef        = getFunctionDeclaration(ndef);
        }

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
    case ada_exception_decl:                 // 11.1(2)
      {
        logKind("ada_exception_decl", kind);

        //Get a list of names
        ada_base_entity lal_excp_names;
        ada_exception_decl_f_ids(lal_element, &lal_excp_names);

        //Get if this is a rename
        ada_base_entity lal_renames;
        ada_exception_decl_f_renames(lal_element, &lal_renames);

        SgScopeStatement&        scope    = ctx.scope();
        SgType&                  excty    = lookupNode(adaTypesByName(), AdaIdentifier{"Exception"});
        if(ada_node_is_null(&lal_renames)){
          SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(ctx, libadalangExcps(), &lal_excp_names, excty, nullptr);
          SgVariableDeclaration&   sgnode   = mkExceptionDecl(dclnames, scope);

          attachSourceLocation(sgnode, lal_element, ctx);
          privatize(sgnode, isPrivate);
          ctx.appendStatement(sgnode);
          assocdecl = &sgnode;
          break;
        } else {
          //TODO Asis assumes only 1 name here, is that true?
          //Get the only? name
          ada_base_entity lal_identifier;
          ada_exception_decl_f_ids(lal_element, &lal_identifier);
          ada_node_child(&lal_identifier, 0, &lal_identifier);
          int hash = hash_node(&lal_identifier);
          ada_defining_name_f_name(&lal_identifier, &lal_identifier);

          //Get the name of the renamed excp
          ada_base_entity lal_renamed_object;
          ada_renaming_clause_f_renamed_object(&lal_renames, &lal_renamed_object);

          std::string        ident   = canonical_text_as_string(&lal_identifier);
          SgExpression&      renamed = getExpr(&lal_renamed_object, ctx);

          SgAdaRenamingDecl& sgnode  = mkAdaRenamingDecl(ident, renamed, excty, scope);

          recordNode(libadalangDecls(), hash, sgnode);

          attachSourceLocation(sgnode, lal_element, ctx);
          privatize(sgnode, isPrivate);
          ctx.appendStatement(sgnode);
          assocdecl = &sgnode;
          break;
        }
      }
    case ada_component_decl:                  // 3.8(6)
      {
        logKind("ada_component_decl", kind);

        handleVarCstDecl(lal_element, ctx, isPrivate, tyIdentity);

        // assocdecl = &sgnode;
        break;
      }
    case ada_package_renaming_decl:           // 8.5.3(2)
      {
        logKind("ada_package_renaming_decl", kind);

        //Get the name for this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_package_renaming_decl_f_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);
        int hash = hash_node(&lal_defining_name);

        //Get the renamed package
        ada_base_entity lal_renames;
        ada_package_renaming_decl_f_renames(lal_element, &lal_renames);
        ada_renaming_clause_f_renamed_object(&lal_renames, &lal_renames);

        //Check if the package we are renaming exists in the lal AST
        ada_base_entity lal_renamed_package;
        ada_package_renaming_decl_p_final_renamed_package(lal_element, &lal_renamed_package);

        if(ada_node_is_null(&lal_renamed_package))
        {
          logWarn() << "skipping unknown package renaming: " << ident << std::endl;
          return;
        }

/*
        SgDeclarationStatement* aliased = &getAliasedID(decl.Renamed_Entity, ctx);

        if (SgAdaGenericDecl* gendcl = isSgAdaGenericDecl(aliased))
          aliased = gendcl->get_declaration();
*/
        SgExpression&           renamed = getExpr(&lal_renames, ctx);
        SgScopeStatement&       scope   = ctx.scope();
        SgType&                 pkgtype = mkTypeVoid();
        SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(ident, renamed, pkgtype, scope);

        recordNode(libadalangDecls(), hash, sgnode);

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_for_loop_spec:           // 5.5(4)   -> Trait_Kinds
      {
        logKind("ada_for_loop_spec", kind);

        //Get the var decl
        ada_base_entity lal_for_var;
        ada_for_loop_spec_f_var_decl(lal_element, &lal_for_var);
        ada_for_loop_var_decl_f_id(&lal_for_var, &lal_for_var); //Now a defining_name

        //Get the var type
        ada_base_entity lal_iter_expr;
        ada_for_loop_spec_f_iter_expr(lal_element, &lal_iter_expr);

        //Get the name
        ada_base_entity lal_identifier;
        ada_defining_name_f_name(&lal_for_var, &lal_identifier);

        std::string            ident   = canonical_text_as_string(&lal_identifier);
        SgExpression&          range   = getDefinitionExpr(&lal_iter_expr, ctx);
        SgType&                vartype = si::Ada::typeOfExpr(range).typerep_ref();
        SgInitializedName&     loopvar = mkInitializedName(ident, vartype, &range);
        SgScopeStatement&      scope   = ctx.scope();
        int                    hash    = hash_node(&lal_for_var);

        recordNode(libadalangVars(), hash, loopvar);

        SgVariableDeclaration& sgnode  = mkVarDecl(loopvar, scope);

        attachSourceLocation(loopvar, lal_element, ctx); // correct ?
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_task_type_decl:                  // 9.1(2)
      {
        logKind("ada_task_type_decl", kind);

        auto                        spec    = getTaskSpec_opt(lal_element, ctx);

        //Get the name
        ada_base_entity defining_name;
        ada_base_type_decl_f_name(lal_element, &defining_name);
        ada_base_entity lal_identifier;
        ada_defining_name_f_name(&defining_name, &lal_identifier);

        //Get the discriminants
        ada_base_entity lal_discr;
        ada_task_type_decl_f_discriminants(lal_element, &lal_discr);

        std::string                 ident  = canonical_text_as_string(&lal_identifier);

        //Set up nondef if this isn't the first decl
        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl);
        int                         hash      = hash_node(&defining_name);
        SgDeclarationStatement*     ndef      = nullptr;
        SgAdaTaskTypeDecl*          nondef    = nullptr;

        if(!ada_node_is_null(&lal_previous_decl)){
          int decl_hash = hash_node(&lal_previous_decl);
          ndef          = findFirst(libadalangTypes(), decl_hash);
          nondef        = isSgAdaTaskTypeDecl(ndef);
        }

        SgScopeStatement*           parentScope = &ctx.scope();
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDecl_opt(&lal_discr, 0, ctx);

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

        //AstContext::PragmaContainer taskPragmas = splitOfPragmas(pragmaVector, taskDeclPragmas, ctx); //TODO Pragmas

        //spec.second(std::move(taskPragmas)); // complete the body

         break;
      }
    case ada_single_protected_decl:           // 3.3.1(2):9.4(2)
      {
        logKind("ada_single_protected_decl", kind);

        //Get the name of this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_single_protected_decl_f_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = canonical_text_as_string(&lal_identifier);

        auto spec = getProtectedSpecForSingleProtected(lal_element, ctx);
        SgAdaProtectedSpecDecl& sgnode = mkAdaProtectedSpecDecl(ident, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        int hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), hash, sgnode);

        /*AstContext::PragmaContainer protectedPragmas = splitOfPragmas(pragmaVector, protectedDeclPragmas, ctx);

        spec.second(std::move(protectedPragmas)); // complete the body*/ //TODO pragmas

        assocdecl = &sgnode;
        break;
      }
    case ada_single_task_decl:                // 3.3.1(2):9.1(3)
      {
        logKind("ada_single_task_decl", kind);

        //Get the name for this task
        ada_base_entity lal_defining_name, lal_identifier;
        ada_single_task_decl_f_task_type(lal_element, &lal_defining_name);
        ada_base_type_decl_f_name(&lal_defining_name, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

        std::string        ident   = canonical_text_as_string(&lal_identifier);
        int                 hash   = hash_node(&lal_defining_name);
        auto               spec    = getTaskSpecForSingleTask(lal_element, ctx);
        SgAdaTaskSpecDecl& sgnode  = mkAdaTaskSpecDecl(ident, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        //~ recordNode(libadalangTypes(), hash, sgnode);
        recordNode(libadalangDecls(), hash, sgnode);

        //AstContext::PragmaContainer taskPragmas = splitOfPragmas(pragmaVector, taskDeclPragmas, ctx); //TODO Pragmas

        //spec.second(std::move(taskPragmas)); // complete the body

        assocdecl = &sgnode;
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

        std::string     ident   = canonical_text_as_string(&lal_identifier);
        int              hash   = hash_node(&defining_name);

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

        //Get the constant status
        ada_base_entity lal_has_constant;
        ada_object_decl_f_has_constant(lal_element, &lal_has_constant);
        ada_node_kind_enum lal_has_constant_kind = ada_node_kind(&lal_has_constant);

        //If it is const, call with mkConstType
        if(lal_has_constant_kind == ada_constant_absent){
          assocdecl = &handleVarCstDecl(lal_element, ctx, isPrivate, tyIdentity);
        } else {
          assocdecl = &handleVarCstDecl(lal_element, ctx, isPrivate, mkConstType);
        }

        break;
      }
    case ada_type_decl:
      {
        logKind("ada_type_decl", kind);
        //Get the type definition
        ada_base_entity lal_type_def;
        ada_type_decl_f_type_def(lal_element, &lal_type_def);

        ada_node_kind_enum type_def_kind = ada_node_kind(&lal_type_def);

        logTrace() << "Ordinary Type\n";

        bool has_with_private = false;

        //Get/print abstract, limited, & private if possible
        if(type_def_kind == ada_derived_type_def){
          ada_base_entity lal_has_abstract;
          ada_derived_type_def_f_has_abstract(&lal_type_def, &lal_has_abstract);
          ada_node_kind_enum abstract_status = ada_node_kind(&lal_has_abstract);
          const bool            has_abstract = (abstract_status == ada_abstract_present);

          ada_base_entity lal_has_limited;
          ada_derived_type_def_f_has_limited(&lal_type_def, &lal_has_limited);
          ada_node_kind_enum limited_status = ada_node_kind(&lal_has_limited);
          const bool            has_limited = (limited_status == ada_limited_present);

          ada_base_entity lal_has_with_private;
          ada_derived_type_def_f_has_with_private(&lal_type_def, &lal_has_with_private);
          ada_node_kind_enum lal_has_with_private_kind = ada_node_kind(&lal_has_with_private);
          if(lal_has_with_private_kind == ada_with_private_present){
            has_with_private = true; 
          }

          logTrace() << "  abstract: " << has_abstract
                     << "\n  limited: " << has_limited
                     << "\n  private: " << has_with_private
                     << std::endl;
        } else if (type_def_kind == ada_private_type_def){
          has_with_private = true;

          ada_base_entity lal_has_abstract;
          ada_private_type_def_f_has_abstract(&lal_type_def, &lal_has_abstract);
          ada_node_kind_enum abstract_status = ada_node_kind(&lal_has_abstract);
          const bool            has_abstract = (abstract_status == ada_abstract_present);

          ada_base_entity lal_has_limited;
          ada_private_type_def_f_has_limited(&lal_type_def, &lal_has_limited);
          ada_node_kind_enum limited_status = ada_node_kind(&lal_has_limited);
          const bool            has_limited = (limited_status == ada_limited_present);

          logTrace() << "  abstract: " << has_abstract
                     << "\n  limited: " << has_limited
                     << "\n  private: " << has_with_private
                     << std::endl;
        }

        //If it matches A_Private_Extension_Declaration/A_Private_Type_Declaration from ASIS, do this instead
        if(has_with_private){
          assocdecl = &handleOpaqueTypes(lal_element, isPrivate, ctx);
          break;
        }

        //Get the name of the type
        ada_base_entity lal_defining_name, lal_identifier;
        ada_base_type_decl_f_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string type_name = canonical_text_as_string(&lal_identifier);
        SgScopeStatement*           parentScope = &ctx.scope();
        ada_base_entity             lal_discr;
        ada_type_decl_f_discriminants(lal_element, &lal_discr);
        ada_base_entity*            second_discr = secondaryDiscriminants(lal_element, &lal_discr, ctx);
        SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDecl_opt(&lal_discr, second_discr, ctx);

        //Get the previous part of this decl, if it exists
        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl); //TODO imprecise fallback
        SgDeclarationStatement* nondef    = nullptr;
        if(!ada_node_is_null(&lal_previous_decl)){
          int previous_hash = hash_node(&lal_previous_decl);
          nondef = findFirst(libadalangTypes(), previous_hash);
        }

        if(discr)
        {
          parentScope = discr->get_discriminantScope();
        }
        SgScopeStatement&       scope     = SG_DEREF(parentScope);
        TypeData                ty        = getTypeFoundation(type_name, &lal_type_def, ctx.scope(scope));
        int                     type_hash = hash_node(lal_element);
        int                     name_hash = hash_node(&lal_defining_name);
        SgDeclarationStatement& sgdecl    = sg::dispatch(TypeDeclMaker{type_name, scope, ty, nondef}, &ty.sageNode());

        privatize(sgdecl, isPrivate);
        //Record this type with hashes for both the ada_type_decl node and ada_defining_name node, since it may be searched for by either
        recordNode(libadalangTypes(), type_hash, sgdecl, nondef != nullptr);
        recordNode(libadalangTypes(), name_hash, sgdecl, nondef != nullptr);
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

/// Creates a choice statement from an ada_variant node
void createVariant(ada_base_entity* lal_element, AstContext ctx)
{
  //Get the choices/components for this path
  ada_base_entity lal_choices, lal_components;
  ada_variant_f_choices(lal_element, &lal_choices);

  ada_variant_f_components(lal_element, &lal_components);
  ada_component_list_f_components(&lal_components, &lal_components);

  //Process the choices
  SgExpressionPtrList exprlst;
  int num_choices = ada_node_children_count(&lal_choices);
  for(int i = 0; i < num_choices; ++i){
    ada_base_entity lal_choice;
    if(ada_node_child(&lal_choices, i, &lal_choice) !=0){
      ada_node_kind_enum lal_choice_kind = ada_node_kind(&lal_choice);
      if(lal_choice_kind == ada_bin_op || lal_choice_kind == ada_others_designator){
        exprlst.push_back(&getDefinitionExpr(&lal_choice, ctx));
      } else {
        exprlst.push_back(&getExpr(&lal_choice, ctx));
      }
    }
  }

  SgExprListExp&      choicelst = mkExprListExp(exprlst);
  SgAdaVariantWhenStmt& sgnode  = mkAdaVariantWhenStmt(choicelst);
  SgAdaUnscopedBlock& blk = SG_DEREF(sgnode.get_components());

  ctx.appendStatement(sgnode);

  //Handle the components
  int num_components = ada_node_children_count(&lal_components);
  for(int i = 0; i < num_components; ++i){
    ada_base_entity lal_component;
    if(ada_node_child(&lal_components, i, &lal_component) != 0){
      handleElement(&lal_component, ctx.unscopedBlock(blk));
    }
  }
}

/// Handles an ada_variant_part node
void handleVariant(ada_base_entity* lal_element, AstContext ctx)
{
  //Get the discriminant name
  ada_base_entity lal_discr_name;
  ada_variant_part_f_discr_name(lal_element, &lal_discr_name);

  //Get the list of variants
  ada_base_entity lal_variant_list;
  ada_variant_part_f_variant(lal_element, &lal_variant_list);

  SgExpression&       discrExpr = getExpr(&lal_discr_name, ctx);
  SgAdaVariantDecl&   sgnode    = mkAdaVariantDecl(discrExpr);
  SgAdaUnscopedBlock& blk       = SG_DEREF(sgnode.get_variants());

  ctx.appendStatement(sgnode);

  //Add each variant to the block
  int count = ada_node_children_count(&lal_variant_list);
  for(int i = 0; i < count; ++i){
    ada_base_entity lal_variant;
    if(ada_node_child(&lal_variant_list, i, &lal_variant) != 0){
      createVariant(&lal_variant, ctx.unscopedBlock(blk));
    }
  }
}

void handleDefinition(ada_base_entity* lal_element, AstContext ctx){
  ada_node_kind_enum kind = ada_node_kind(lal_element);
  logKind("A_Definition", kind);

  // many definitions are handled else where
  // here we want to convert the rest that can appear in declarative context

  switch(kind)
  {
    case ada_null_component_decl:                 // 3.8(4)
      {
        SgDeclarationStatement& sgnode = mkNullDecl();

        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);
        break;
      }
    case ada_variant_part:                   // 3.8.1(2)
      {
        handleVariant(lal_element, ctx);
        break;
      }
    default:
      logFlaw() << "unhandled definition kind: " << kind << std::endl;
      break;
  }
}

void
processInheritedSubroutines( SgNamedType& derivedType,
                             ada_base_entity* tydef,
                             AstContext ctx
                           )
{
  ada_base_entity lal_tydef_deref = *tydef;
  // use a deferred unit completion, so that all information of records
  //   (i.e., detailed inheritance relationship) have been seen when
  //   the inherited subroutines are processed.
  auto deferredSubRoutineProcessing =
       [&derivedType, lal_tydef_deref, ctx]() -> void
       {
         SgType*      baseType     = si::Ada::baseType(derivedType);
         SgType*      baseRootRaw  = si::Ada::typeRoot(baseType).typerep();
         SgNamedType* baseRootType = isSgNamedType(baseRootRaw);

         if (baseRootType == nullptr)
         {
           // if baseRootRaw != nullptr
           //   it will correspond to a universal type in package standard.
           //   -> not an error
           if (baseRootRaw == nullptr)
             logFlaw() << "unable to find any base-root for " << derivedType.get_name()
                       << " / base = " << baseType
                       << std::endl;
           return;
         }

         //~ logWarn() << "drv: " << derivedType.get_name() << " / " << baseRootType->get_name()
                   //~ << std::endl;
         //Figure out what the subprograms to inherit are
         //Get the original type
         ada_base_entity lal_tydef = lal_tydef_deref;
         ada_base_entity lal_super_type;
         ada_derived_type_def_f_subtype_indication(&lal_tydef, &lal_super_type);
         ada_type_expr_p_designated_type_decl(&lal_super_type, &lal_super_type);

         //  Get the programs that use the original type
         //Get the name of the type
         ada_text_type lal_unique_identifying_name;
         ada_basic_decl_p_unique_identifying_name(&lal_super_type, &lal_unique_identifying_name);
         std::string unique_identifying_name = dot_ada_text_type_to_string(lal_unique_identifying_name);

         ada_base_entity lal_node_list;
         ada_ada_node_parent(&lal_super_type, &lal_node_list);
         int count = ada_node_children_count(&lal_node_list);
         for(int i = 0; i < count; ++i){
           ada_base_entity lal_stmt;
           if(ada_node_child(&lal_node_list, i, &lal_stmt) != 0){
             //Check if this is a subp_decl that uses the original type
             ada_node_kind_enum lal_stmt_kind = ada_node_kind(&lal_stmt);
             if(lal_stmt_kind == ada_subp_decl){
               ada_text_type lal_subp_unique_name;
               ada_basic_decl_p_unique_identifying_name(&lal_stmt, &lal_subp_unique_name);
               std::string subp_unique_name = dot_ada_text_type_to_string(lal_subp_unique_name);
               if(subp_unique_name.find(unique_identifying_name) != std::string::npos){
                 createInheritedSymbol(&lal_stmt, *baseRootType, derivedType, ctx);
               }
             }
           }
         }

         /*if (!declarations.empty()) //TODO decls? Can decls be inherited?
         {
           // fields will be added later during the fixup pass: FixupAstSymbolTablesToSupportAliasedSymbols
           // \todo also implement fixup for discriminants and enumerations..
           logInfo() << "A derived/extension record type's implicit declaration is not empty: "
                     << derivedType.get_name()
                     << std::endl;
         }*/
       };

  ctx.storeDeferredUnitCompletion(std::move(deferredSubRoutineProcessing));
}
 
} //end Libadalang_ROSE_Translation
