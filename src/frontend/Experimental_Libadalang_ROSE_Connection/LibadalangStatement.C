#include "sage3basic.h"
#include "sageBuilder.h"
#include "sageInterfaceAda.h"
#include "sageGeneric.h"

#include <numeric>

#include "LibadalangStatement.h"

#include "Libadalang_to_ROSE.h"
//#include "AdaExpression.h"
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

          /* unused fields:
          */
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

    default:
        logWarn() << "Unhandled statement " << kind << std::endl;
        //ADA_ASSERT (!FAIL_ON_ERROR(ctx));
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

        const bool              isFunc  = (subp_kind_kind == ada_subp_kind_function);
        ada_text_type p_fully_qualified_name; //This will only work for nodes derived from basic_decl
        ada_basic_decl_p_fully_qualified_name(lal_element, &p_fully_qualified_name);
        std::string ident = dot_ada_text_type_to_string(p_fully_qualified_name);
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

        //recordNode(asisDecls(), elem.ID, sgnode);
        //recordNode(asisDecls(), adaname.id(), sgnode);
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

        /* unhandled field
           Declaration_ID                 Body_Block_Statement;
           bool                           Is_Overriding_Declaration;
           bool                           Is_Not_Overriding_Declaration;

         +func:
           bool                           Is_Not_Null_Return

           break;
        */
        assocdecl = &sgnode;
        break;
      }
    default:
      logWarn() << "unhandled declaration kind: " << kind << std::endl;
      //ADA_ASSERT (!FAIL_ON_ERROR(ctx));
  }

  //processAspects(lal_element, decl, assocdecl, ctx);
  //recordPragmasID(std::move(pragmaVector), assocdecl, ctx);
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
  return nullptr;
  //TODO What is this supposed to do? Is it related to the default expr?
  /*if (decl.Declaration_Kind == A_Deferred_Constant_Declaration)
    return nullptr;

  ADA_ASSERT (  decl.Declaration_Kind == A_Variable_Declaration
             || decl.Declaration_Kind == A_Constant_Declaration
             || decl.Declaration_Kind == A_Parameter_Specification
             || decl.Declaration_Kind == A_Real_Number_Declaration
             || decl.Declaration_Kind == An_Integer_Number_Declaration
             || decl.Declaration_Kind == A_Component_Declaration
             || decl.Declaration_Kind == A_Discriminant_Specification
             || decl.Declaration_Kind == A_Formal_Object_Declaration
             );

  //~ logWarn() << "decl.Initialization_Expression = " << decl.Initialization_Expression << std::endl;
  if (decl.Initialization_Expression == 0)
    return nullptr;

  return &getExprID(decl.Initialization_Expression, ctx);*/
}

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
 
} //end Libadalang_ROSE_Translation
