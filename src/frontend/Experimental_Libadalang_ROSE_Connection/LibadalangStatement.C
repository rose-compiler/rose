#include "sage3basic.h"
#include "sageBuilder.h"
#include "sageInterfaceAda.h"
#include "sageGeneric.h"

#include <numeric>
#include <boost/algorithm/string/predicate.hpp>

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
    int                 range = ada_node_is_null(lal_stmt_list) ? 0 : ada_node_children_count(lal_stmt_list);

    for(int i = 0; i < range; ++i){
      ada_base_entity lal_stmt;
      if(ada_node_child(lal_stmt_list, i, &lal_stmt) != 0){
        handleStmt(&lal_stmt, ctx.scope(blk));
      }
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
    int                 range = ada_node_is_null(lal_handlers) ? 0 : ada_node_children_count(lal_handlers);

    for(int i = 0; i < range; ++i){
      ada_base_entity lal_handler;
      if(ada_node_child(lal_handlers, i, &lal_handler) != 0){
        handleExceptionHandler(&lal_handler, trystmt, ctx.scope(blk));
      }
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
    {
      activeScopes.push_back(&stmtblk);
    }

    blockHandler(lal_stmts, stmtblk, pragmaCtx);

    if (trystmt)
    {
      exhandlerHandler(lal_exceptions, dominantBlock, *trystmt, pragmaCtx);
    }
  }

  /// completes any block with declarative items and exception handlers and pragmas attached
  void completeDeclarationsWithHandledBlock( ada_base_entity* lal_decls,
                                             ada_base_entity* lal_stmts,
                                             ada_base_entity* lal_exceptions,
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
    int             range = ada_node_is_null(lal_decls) ? 0 : ada_node_children_count(lal_decls);

    for(int i =0; i < range; ++i){
         ada_base_entity lal_decl_child;
         if(ada_node_child(lal_decls, i, &lal_decl_child) != 0){
             handleElement(&lal_decl_child, pragmaCtx.scope(dominantBlock));
        }
    }

    completeHandledBlock( lal_stmts,
                          lal_exceptions,
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
    ada_base_entity lal_decls, lal_stmts, lal_exceptions;

    ada_node_kind_enum kind;
    kind = ada_node_kind(lal_decl);

    if(kind == ada_subp_body){
        ada_subp_body_f_decls(lal_decl, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_subp_body_f_stmts(lal_decl, &lal_handled_stmts);
        ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
        ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
    } else if(kind == ada_entry_body){
        ada_entry_body_f_decls(lal_decl, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_entry_body_f_stmts(lal_decl, &lal_handled_stmts);
        ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
        ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
    } else {
        //TODO
        logWarn() << "Unknown kind " << kind << " in completeRoutineBody!\n";
    }

    completeDeclarationsWithHandledBlock( &lal_decls,
                                          &lal_stmts,
                                          &lal_exceptions,
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

  SgExpression&
  getDefaultFunctionExpr(ada_base_entity* lal_element, SgAdaSubroutineType& ty, AstContext ctx)
  {
    SgExpression* res = nullptr;

    if(ada_node_is_null(lal_element)){
      return mkNullExpression();
    }

    ada_node_kind_enum kind = ada_node_kind(lal_element);

    switch (kind)
    {
      case ada_box_expr:
        res = &mkAdaBoxExp();
        break;

      case ada_dotted_name:
      case ada_identifier:
      case ada_string_literal:
        {
          SgFunctionParameterList&  fnparams = SG_DEREF(ty.get_parameterList());
          SgInitializedNamePtrList& parmlst  = fnparams.get_args();
          OperatorCallSupplement::ArgDescList arglist;
          auto argDescExtractor = [](SgInitializedName* ini) -> ArgDesc
                                  {
                                    return { "", ini->get_type() };
                                  };

          arglist.reserve(parmlst.size());
          std::transform(parmlst.begin(), parmlst.end(), std::back_inserter(arglist), argDescExtractor);

          res = &getExpr(lal_element, ctx, OperatorCallSupplement(std::move(arglist), ty.get_return_type()));
          break;
        }

      case ada_null_literal:
        res = sb::buildNullptrValExp();
        break;

      /*case A_Nil_Default:
        res = &mkNullExpression();
        break;*/

      default:
        logError() << "Unhandled kind " << kind << " in getDefaultFunctionExpr!\n";
    }

    return SG_DEREF(res);
  }

  /// Imitates gnatkr (gnatkrunch)
  /// \param package_name The name of the package to reduce
  /// \param krunch_length The length to reduce it to (default 8)
  /// Returns the krunched package name, which is created by splitting \ref package_name
  /// into multiple strings on any ".", "-", or "_" chars, removing said chars, then
  /// repeatedly removing the last char of the longest substr until the combined
  /// length of all remaining substrs is < \ref krunch_length. The substrs are then
  /// recombined and the extension (if present) is appended.
  std::string krunch_package_name(std::string package_name, int krunch_length = 8){
    //Get rid of the extension, if we have it
    std::size_t last_dot_pos = package_name.find_last_of('.');
    std::string extension = "";
    if(last_dot_pos != std::string::npos){
      extension = package_name.substr(last_dot_pos);
      package_name = package_name.substr(0, last_dot_pos);
    }

    //If this starts with ada, gnat, system, or interfaces, remove them and keep note
    std::map<std::string, std::string> special_prefixes = {
      {"ada-", "a-"}, {"gnat-", "g-"}, {"interfaces-", "i-"}, {"system-", "s-"}
    };
    std::string krunched_prefix = "";
    for(const std::pair<std::string, std::string>& pair : special_prefixes){
      if(package_name.find(pair.first, 0) == 0){
          krunched_prefix = pair.second;
          package_name = package_name.substr(package_name.find_first_of("-") + 1, std::string::npos);
          break;
      }
    }
    krunch_length -= krunched_prefix.size();

    //Split the package name by the delimiters (-._)
    std::vector<std::string> split_package_name;
    std::size_t prev = 0, pos;
    int split_length = 0;
    while((pos = package_name.find_first_of(".-_", prev)) != std::string::npos)
    {
      if(pos > prev){
        split_package_name.push_back(package_name.substr(prev, pos-prev));
        split_length += split_package_name.back().size();
      }
      prev = pos+1;
    }
    if(prev < package_name.length()){
      split_package_name.push_back(package_name.substr(prev, std::string::npos));
      split_length += split_package_name.back().size();
    }

    while(split_length > krunch_length){
      //Find the longest substring, and remove its last char
      int longest_substr_pos = 0, longest_substr_len = 0;
      for(int i = 0; i < split_package_name.size(); ++i){
        if(split_package_name.at(i).size() > longest_substr_len){
          longest_substr_pos = i;
          longest_substr_len = split_package_name.at(i).size();
        }
      }
      split_package_name.at(longest_substr_pos).pop_back();

      //decrement split_length
      split_length--;
    }

    std::string krunched_package_name = "";
    for(std::string split_piece : split_package_name){
      krunched_package_name = krunched_package_name + split_piece;
    }

    return krunched_prefix + krunched_package_name + extension;
  }

  // may need special processing for pragmas in body
  using DeferredPragmaBodyCompletion = std::function<void(AstContext::PragmaContainer)>;
  //
  // pragma handling

  void handleExtendSystem(ada_base_entity* lal_element){
    //lal_element should be a list with one child, and that child should be the name of the package
    ada_base_entity lal_package_name;
    ada_node_child(lal_element, 0, &lal_package_name);
    ada_pragma_argument_assoc_f_expr(&lal_package_name, &lal_package_name);
    const std::string lal_package_name_string = getFullName(&lal_package_name);

    logInfo() << "Extending System with package " << lal_package_name_string << std::endl;

    //Get the expected file name from the package name
    std::string package_specification_name = "s-" + krunch_package_name(lal_package_name_string, 6) + ".ads";

    //Iterate through LibadalangTypes, find all types that come from this package, & add them to extendedTypesByName
    for(const std::pair<int, SgDeclarationStatement*>& type_pair : libadalangTypes()){
      SgDeclarationStatement* type_decl = type_pair.second;
      const std::string& type_decl_filename = type_decl->getFilenameString();

      if(type_decl_filename.size() >= package_specification_name.size()
         && boost::algorithm::ends_with(type_decl_filename, package_specification_name)){

        //Get the SgType* & add it to extendedTypesByName
        VariantT type_decl_class = type_decl->variantT();
        SgType* type_to_add = nullptr;
        std::string name_to_add = "NameError";
        switch(type_decl_class){
          case V_SgEnumDeclaration:
            {
              SgEnumDeclaration* enum_decl = isSgEnumDeclaration(type_decl);
              type_to_add = enum_decl->get_type();
              name_to_add = enum_decl->get_name().getString();
              break;
            }
          case V_SgTypedefDeclaration:
            {
              SgTypedefDeclaration* typedef_decl = isSgTypedefDeclaration(type_decl);
              type_to_add = typedef_decl->get_type();
              name_to_add = typedef_decl->get_name().getString();
              break;
            }
          case V_SgClassDeclaration:
            {
              //Records
              SgClassDeclaration* class_decl = isSgClassDeclaration(type_decl);
              type_to_add = class_decl->get_type();
              name_to_add = class_decl->get_name().getString();
              break;
            }
          case V_SgAdaFormalTypeDecl:
            {
              //Generics
              SgAdaFormalTypeDecl* formal_decl = isSgAdaFormalTypeDecl(type_decl);
              //Don't add these; they are part of a generic package/subp
              break;
            }
          default:
            logError() << "Unhandled SgTypeDeclaration subtype " << type_decl_class << " in handleExtendSystem!\n";
            break;
        }

        if(type_to_add != nullptr){
          AdaIdentifier name_to_add_converted{name_to_add};

          const auto& insert_result = extendedTypesByName().insert({name_to_add_converted, type_to_add});

          if(!insert_result.second){
            //Check if the value for this key is the same as what we currently have
            SgType* type_from_map = insert_result.first->second;
            if(type_from_map->get_mangled() != type_to_add->get_mangled()){
              logError() << "Types from " << lal_package_name_string
                         << " would overwrite types already extending System, canceling extension!\n";
              return;
            }
          }
        }
      }
    }
  }

  SgPragmaDeclaration&
  createPragma_common(ada_base_entity* lal_element, SgStatement* stmtOpt, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    if(kind != ada_pragma_node){
      logError() << "createPragma_common called on node of kind " << kind << " (not an ada_pragma_node)!\n";
    }

    if(false){ //TODO Test p_associated_decls more to see if it might be of use in placing pragmas
      ada_ada_node_array associated_decls;
      ada_pragma_node_p_associated_decls(lal_element, &associated_decls);
      for(int i = 0; i < associated_decls->n; ++i){
        ada_base_entity associated_decl = associated_decls->items[i];
        if(!ada_node_is_null(&associated_decl)){
          logInfo() << "  Associated decl " << i << " is at " << dot_ada_full_sloc(&associated_decl) << std::endl;
          logInfo() << "    " << ada_node_kind(&associated_decl) << std::endl;
        }
      }
    }

    //Pragma nodes don't use defining_name, so just hash the ada_pragma_node itself?
    int hash = hash_node(lal_element);

    logKind("ada_pragma_node", hash);

    //Get the name
    ada_base_entity lal_id;
    ada_pragma_node_f_id(lal_element, &lal_id);
    std::string                name = getFullName(&lal_id);

    //Get the args
    ada_base_entity lal_args;
    ada_pragma_node_f_args(lal_element, &lal_args);
    SgExprListExp&             arglist  = mkExprListExp();
    SgPragmaDeclaration&       sgnode   = mkPragmaDeclaration(name, arglist, stmtOpt);

    sgnode.set_parent(&ctx.scope()); // set fictitious parent (will be overwritten when pragma is actually placed)
    //TODO I removed all the stuff about placing pragmas, but it still seems to work? Might need to set parent better.

    std::vector<SgExpression*> args;
    int count = ada_node_children_count(&lal_args);
    for(int i = 0; i < count; ++i){
      ada_base_entity lal_param;
      if(ada_node_child(&lal_args, i, &lal_param) != 0){
        args.push_back(&getArg(&lal_param, ctx.pragmaAspectAnchor(sgnode)));
      }
    }

    arglist.get_expressions().reserve(args.size());
    for (SgExpression* arg : args) arglist.append_expression(arg);

    // \todo do we need to privatize pragmas in the private section?
    attachSourceLocation(sgnode, lal_element, ctx);
    attachSourceLocation(SG_DEREF(sgnode.get_pragma()), lal_element, ctx);
    recordNode(libadalangDecls(), hash, sgnode);

    if(name == "extend_system"){
      //Add all the types in the referenced package to extendedTypesByName()
      handleExtendSystem(&lal_args);
    }

    return sgnode;
  }

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
    ada_base_entity lal_subtype_indication, lal_base_type, lal_type_def;
    ada_derived_type_def_f_subtype_indication(tydef, &lal_subtype_indication);
    ada_type_expr_p_designated_type_decl(&lal_subtype_indication, &lal_base_type);
    ada_type_decl_f_type_def(&lal_base_type, &lal_type_def);

    //Handle the case where lal_base_type is a derived type, not ada_enum_type_def
    ada_node_kind_enum lal_type_def_kind = ada_node_kind(&lal_type_def);
    while(lal_type_def_kind != ada_enum_type_def){
      if(lal_type_def_kind == ada_private_type_def){
        ada_base_type_decl_p_private_completion(&lal_base_type, &lal_base_type);
        ada_type_decl_f_type_def(&lal_base_type, &lal_type_def);
      } else if(lal_type_def_kind == ada_derived_type_def){
        ada_derived_type_def_f_subtype_indication(&lal_type_def, &lal_subtype_indication);
        ada_type_expr_p_designated_type_decl(&lal_subtype_indication, &lal_base_type);
        ada_type_decl_f_type_def(&lal_base_type, &lal_type_def);
      } else {
        logFlaw() << "Unhandled lal_type_def_kind " << lal_type_def_kind << " in processInheritedEnumValues!\n";
        break;
      }

      lal_type_def_kind = ada_node_kind(&lal_type_def);
    }

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
    ada_type_decl_f_type_def(&lal_base_type, &lal_inherited_decl_list);
    ada_enum_type_def_f_enum_literals(&lal_inherited_decl_list, &lal_inherited_decl_list);
    int count = ada_node_children_count(&lal_inherited_decl_list);

    SgType& enumty = SG_DEREF(derivedTypeDcl.get_type());

    // just traverse the IDs, as the elements are not present
    for(int i = 0; i < count ; ++i){
      ada_base_entity lal_inherited_decl;
      if(ada_node_child(&lal_inherited_decl_list, i, &lal_inherited_decl) != 0){
        //TODO How will this work? It isn't a unique node.

        //Get the name
        ada_base_entity lal_defining_name, lal_identifier;
        ada_enum_literal_decl_f_name(&lal_inherited_decl, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);

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
    int lal_element_hash = hash_node(lal_element);
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
          logKind("A_Private_Extension_Definition", lal_element_hash);
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
          logKind("ada_private_type_def", lal_element_hash);
          ada_base_entity lal_has_abstract, lal_has_limited, lal_has_tagged;

          ada_private_type_def_f_has_abstract(lal_element, &lal_has_abstract);
          ada_node_kind_enum lal_has_abstract_kind = ada_node_kind(&lal_has_abstract);
          bool has_abstract = (lal_has_abstract_kind == ada_abstract_present);

          ada_private_type_def_f_has_limited(lal_element, &lal_has_limited);
          ada_node_kind_enum lal_has_limited_kind = ada_node_kind(&lal_has_limited);
          bool has_limited = (lal_has_limited_kind == ada_limited_present);

          ada_private_type_def_f_has_tagged(lal_element, &lal_has_tagged);
          ada_node_kind_enum lal_has_tagged_kind = ada_node_kind(&lal_has_tagged);
          bool has_tagged = (lal_has_tagged_kind == ada_tagged_present);

          setModifiers(dcl, has_abstract, has_limited, has_tagged);
          break;
       }
     case ada_record_type_def:
       {
          logKind("ada_record_type_def", lal_element_hash);
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
  // \note if the type is eventually an enum, we cannot use derived type for the
  //       intermediate declarations. So before deciding what kind of node
  //       shall be created we need to go to the bottom of derived type chains.
  SgDeclarationStatement&
  createOpaqueDecl(const std::string& ident, ada_base_entity* lal_element, AstContext ctx)
  {
    SgScopeStatement&       scope    = ctx.scope();
    bool                    typeview = false;
    SgDeclarationStatement* res      = nullptr;

    //Get the type def
    ada_base_entity lal_type_def;
    ada_type_decl_f_type_def(lal_element, &lal_type_def);
    ada_node_kind_enum lal_type_def_kind = ada_node_kind(&lal_type_def);
    if(ada_node_kind(lal_element) != ada_incomplete_type_decl && ada_node_kind(lal_element) != ada_incomplete_tagged_type_decl){
      typeview = (ada_node_is_null(&lal_type_def) == 0);
    }

    //Get the type this points to
    ada_base_entity lal_base_type_decl;
    ada_base_type_decl_p_full_view(lal_element, &lal_base_type_decl);
    ada_node_kind_enum lal_base_type_kind = ada_node_kind(&lal_base_type_decl);

    //TODO Needs more testing w/enums

    if(lal_base_type_kind == ada_task_type_decl){
      res = &mkAdaTaskTypeDecl(ident, nullptr /* no spec */, scope);
    } else if(lal_base_type_kind == ada_protected_type_decl){
      res = &mkAdaProtectedTypeDecl(ident, nullptr /* no spec */, scope);
    } else if(!typeview || lal_type_def_kind == ada_private_type_def || lal_type_def_kind == ada_derived_type_def){
      //If it isn't an access, we need to find the original type to see if it's a type, record, or enum
      ada_base_entity lal_base_type_def;
      ada_type_decl_f_type_def(&lal_base_type_decl, &lal_base_type_def);
      lal_type_def_kind = ada_node_kind(&lal_base_type_def);

      // if the ultimate base type is an enum, the derived type also shall be an enum.
      // if not an enum, then an intermediate type decl is created.
      // firstLevelIsDerived is set to indicate if the first level was a derived type
      bool firstLevelIsDerived = lal_type_def_kind == ada_derived_type_def;

      if(firstLevelIsDerived){
        //Unfortunately, even if the first level is an ada_derived_type_def, it could still correspond to
        // A_Derived_Record_Extension_Definition. So, check if it has a record extension.
        ada_base_entity lal_record_ext;
        int record_ext_return_check = ada_derived_type_def_f_record_extension(&lal_base_type_def, &lal_record_ext);
        if(record_ext_return_check != 0 && !ada_node_is_null(&lal_record_ext)){
          firstLevelIsDerived = false;
        }
      }

      while(lal_type_def_kind == ada_derived_type_def || lal_type_def_kind == ada_private_type_def){
        ada_base_entity lal_subtype_indication;
        //If this is a derived type, get the type it is derived from
        if(lal_type_def_kind == ada_derived_type_def){
          ada_derived_type_def_f_subtype_indication(&lal_base_type_def, &lal_subtype_indication);
          ASSERT_require(!ada_node_is_null(&lal_subtype_indication));

          logTrace() << "kind = " << ada_node_kind(&lal_subtype_indication)
                     << "   hash = " << hash_node(&lal_subtype_indication)
                     << std::endl;

          ada_type_expr_p_designated_type_decl(&lal_subtype_indication, &lal_base_type_decl);
        } else {
          //For ada_private_type_def, get the type decl
          ada_base_type_decl_p_private_completion(&lal_base_type_decl, &lal_base_type_decl);
        }
        if (!ada_node_is_null(&lal_base_type_decl)) {

          //Check what type lal_base_type_decl is
          ada_node_kind_enum lal_base_type_decl_kind = ada_node_kind(&lal_base_type_decl);
          while(lal_base_type_decl_kind == ada_subtype_decl){
            logInfo() << "ada_subtype_decl found, looping to move past it.\n";
            //ada_subtype_decl does not have a type def, so we need to keep getting the base type until we find a non-subtype
            ada_subtype_decl_f_subtype(&lal_base_type_decl, &lal_subtype_indication);
            ada_type_expr_p_designated_type_decl(&lal_subtype_indication, &lal_base_type_decl);
            lal_base_type_decl_kind = ada_node_kind(&lal_base_type_decl);
          }

          //Found a case in standard (s-osinte.ads) where the designated_type_decl points to the parent in an infinite loop
          //  So, check to make sure the new base_type_def isn't the same as the old
          int old_type_def_hash = hash_node(&lal_base_type_def);
          ada_type_decl_f_type_def(&lal_base_type_decl, &lal_base_type_def);
          ASSERT_require(!ada_node_is_null(&lal_base_type_def));

          if(hash_node(&lal_base_type_def) == old_type_def_hash){
            //If there is an infinite loop, make our best guess
            SgNode& tyrep = getExprType(&lal_base_type_decl, ctx);
            const bool isEnum = si::Ada::baseEnumDeclaration(isSgType(&tyrep)) != nullptr;

            lal_type_def_kind = isEnum ? ada_enum_type_def : ada_private_type_def /* anything */;
          } else {
            //If everything is fine, keep looping
            lal_type_def_kind = ada_node_kind(&lal_base_type_def);
          }
        } else {
          //If we encounter a null node, just give up
          logFlaw() << "Failed to find base type for " << ident << " in createOpaqueDecl!\n";
          ada_base_entity lal_name;

          ada_subtype_indication_f_name(&lal_subtype_indication, &lal_name); //TODO This could use lal_subtype_indication before it is initialized
          SgNode& tyrep = getExprType(&lal_name, ctx);

          const bool isEnum = si::Ada::baseEnumDeclaration(isSgType(&tyrep)) != nullptr;

          lal_type_def_kind = isEnum ? ada_enum_type_def : ada_private_type_def /* anything */;
        }
      }

      if(lal_type_def_kind == ada_enum_type_def){
        res = &mkEnumDecl(ident, scope);
      } else if((lal_type_def_kind == ada_record_type_def) && !firstLevelIsDerived){
        SgClassDeclaration& sgnode = mkRecordDecl(ident, scope);
        if(typeview){
          setParentRecordConstraintIfAvail(sgnode, &lal_type_def, ctx);
        }
        res = &sgnode;
      } else { //TODO not sure this will work for everything
        res = &mkTypeDecl(ident, mkOpaqueType(), scope);
      }
    } else {
      logWarn() << "unhandled opaque type declaration: " << lal_type_def_kind
                << std::endl;
      res = &mkTypeDecl(ident, mkOpaqueType(), scope);
    }

    // \todo put declaration tags on
    SgDeclarationStatement& resdcl = SG_DEREF(res);

    if(typeview){
      setTypeModifiers(resdcl, &lal_type_def);
    } else if(ada_node_kind(lal_element) == ada_incomplete_tagged_type_decl) {
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
      logKind("ada_known_discriminant_part", hash_node(lal_element));
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
    if(primary == nullptr || ada_node_is_null(primary))
    {
      if(secondary != nullptr && !ada_node_is_null(secondary)) logFlaw() << "Unexpected secondary discriminants" << std::endl;

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
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    //Get the name for this type
    ada_base_entity lal_identifier, lal_defining_name;
    ada_base_type_decl_f_name(lal_element, &lal_defining_name);
    ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
    std::string ident = getFullName(&lal_identifier);

    //Get the discriminants
    ada_base_entity         lal_discr;
    if(kind == ada_incomplete_type_decl || kind == ada_incomplete_tagged_type_decl){
      ada_incomplete_type_decl_f_discriminants(lal_element, &lal_discr);
    } else {
      ada_type_decl_f_discriminants(lal_element, &lal_discr);
    }
    ada_base_entity*        second_discr = secondaryDiscriminants(lal_element, &lal_discr, ctx);

    //Get the type def
    ada_base_entity lal_type_def;
    ada_type_decl_f_type_def(lal_element, &lal_type_def);
    ada_node_kind_enum lal_type_def_kind = ada_node_kind(&lal_type_def);

    SgScopeStatement*       parentScope = &ctx.scope();
    SgAdaDiscriminatedTypeDecl* discr = createDiscriminatedDecl_opt(&lal_discr, second_discr, ctx);

    if(discr) {
      parentScope = discr->get_discriminantScope();
    }

    SgScopeStatement&       scope  = SG_DEREF(parentScope);
    SgDeclarationStatement& sgdecl = createOpaqueDecl(ident, lal_element, ctx.scope(scope));
    SgDeclarationStatement* assocdecl = nullptr;

    attachSourceLocation(sgdecl, lal_element, ctx);
    privatize(sgdecl, isPrivate);
    int hash = hash_node(lal_element);
    int decl_hash = hash_node(&lal_defining_name);
    recordNode(libadalangTypes(), hash, sgdecl);
    recordNode(libadalangTypes(), decl_hash, sgdecl);

    if(!discr) {
      ctx.appendStatement(sgdecl);
      assocdecl = &sgdecl;
    } else {
      completeDiscriminatedDecl(hash, lal_element, *discr, sgdecl, isPrivate, ctx);
      assocdecl = discr;
    }

    //If this is a derived type, get the inherited elements
    //TODO Other lal_type_def_kinds may still inherit?
    if(!ada_node_is_null(&lal_type_def) && lal_type_def_kind == ada_derived_type_def){
      TypeData ty = getTypeFoundation(ident, &lal_type_def, ctx);

      processInheritedElementsOfDerivedTypes(ty, SG_DEREF(assocdecl), ctx);
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
        const std::string  name = getFullName(&lal_identifier);
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

    //Get the hash
    int lal_mode_hash = hash_node(lal_mode);

    SgTypeModifier res;

    switch(kind)
    {
      case ada_mode_default:
        Libadalang_ROSE_Translation::logKind("ada_mode_default", lal_mode_hash);
        res.setDefault();
       break;

      case ada_mode_in:
        Libadalang_ROSE_Translation::logKind("ada_mode_in", lal_mode_hash);
        res.setIntent_in();
        break;

      case ada_mode_out:
        Libadalang_ROSE_Translation::logKind("ada_mode_out", lal_mode_hash);
        res.setIntent_out();
        break;

      case ada_mode_in_out:
        Libadalang_ROSE_Translation::logKind("ada_mode_in_out", lal_mode_hash);
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
    if(SgClassDeclaration* recdcl = isSgClassDeclaration(nondefdcl)){
      return mkRecordDecl(*recdcl, def, scope);
    }

    // if nondefdcl is set, it must be a SgClassDeclaration
    if(nondefdcl){
      logFlaw() << name << " is " << typeid(*nondefdcl).name() << ", not a class declaration!"
                << std::endl;
    }

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
          int lal_element_hash = hash_node(lal_element);
          logKind("ada_entry_index_spec", lal_element_hash);

          //Get the subtype
          ada_base_entity lal_subtype;
          ada_entry_index_spec_f_subtype(lal_element, &lal_subtype);

          //Get the name
          ada_base_entity lal_id;
          ada_entry_index_spec_f_id(lal_element, &lal_id);
          ada_defining_name_f_name(&lal_id, &lal_id);

          name = getFullName(&lal_id);
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
     ASSERT_require(pragmas.empty());
  }

  /// Handle a task, and the pragmas associated with it
  /// @{
  std::pair<SgAdaTaskSpec*, DeferredPragmaBodyCompletion>
  getTaskSpec(ada_base_entity* lal_element, AstContext ctx)
  {
    int lal_element_hash = hash_node(lal_element);
    logKind("A_Task_Definition", lal_element_hash);

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
        if(ada_node_child(&public_part, i, &lal_entry) != 0){
          handleElement(&lal_entry, ctx.scope(*nodePtr));
        }
      }
    }

    //Get the private part
    if(!ada_node_is_null(&private_part)){
      ada_declarative_part_f_decls(&private_part, &private_part);

      int range = ada_node_children_count(&private_part);
      for(int i = 0; i < range; ++i){
        ada_base_entity lal_entry;
        if(ada_node_child(&private_part, i, &lal_entry) != 0){
          handleElement(&lal_entry, ctx.scope(*nodePtr), true /* private items */);
        }
      }
    }

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
      logKind("ada_protected_def", hash_node(lal_element));
    }

    //Get the private status
    ada_base_entity lal_private_list;
    ada_protected_def_f_private_part(lal_element, &lal_private_list);
    ada_declarative_part_f_decls(&lal_private_list, &lal_private_list);

    SgAdaProtectedSpec&     sgnode = mkAdaProtectedSpec();
    SgAdaProtectedSpec*     nodePtr  = &sgnode;

    sgnode.set_hasPrivate(!ada_node_is_null(&lal_private_list) && ada_node_children_count(&lal_private_list) > 0);

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

  /// Set of functions to help with various parts of select stmts
  /// @{
  // MS 11/17/2020 : SelectStmtCreator modeled on IfStmtCreator
  // PP 10/12/2021 : modified code to eliminate the need for using scope_npc.
  //                 A block will only be populated after the new node has been connected
  //                 to the AST. This is achieved by returning a lambda function w/o parameters
  //                 (DeferredBodyCompletion) that is invoked after the node has been connected
  //                 to the AST.

    SgAdaSelectAlternativeStmt* handleOrAlternative(SgAdaSelectStmt& sgnode, SgAdaSelectAlternativeStmt* currOrPath, ada_base_entity* lal_element, AstContext ctx){
      // create body of alternative
      SgBasicBlock* block = &mkBasicBlock();

      //Get the guard
      ada_base_entity lal_guard;
      ada_select_when_part_f_cond_expr(lal_element, &lal_guard);
      SgExpression& guard = getExpr_opt(&lal_guard, ctx);

      // instantiate SgAdaSelectAlternativeStmt node
      SgAdaSelectAlternativeStmt& stmt = mkAdaSelectAlternativeStmt(guard, *block);

      if(currOrPath == nullptr)
      {
        sg::linkParentChild(sgnode, stmt, &SgAdaSelectStmt::set_or_path);
      }
      else
      {
        sg::linkParentChild(*currOrPath, stmt, &SgAdaSelectAlternativeStmt::set_next);
      }

      //Get the list of stmts & handle them
      ada_base_entity lal_stmt_list;
      ada_select_when_part_f_stmts(lal_element, &lal_stmt_list);
      int count = ada_node_children_count(&lal_stmt_list);
      for(int i = 0; i < count; ++i){
        ada_base_entity lal_stmt;
        if(ada_node_child(&lal_stmt_list, i, &lal_stmt) != 0){
          handleStmt(&lal_stmt, ctx.scope(*block));
        }
      }

      return &stmt; //Return the stmt so it can be used as the parent for the next or path if necessary
    }

    void handleSelectAlternative(SgAdaSelectStmt& sgnode, ada_base_entity* lal_element, AstContext ctx){
      // create body of alternative
      SgBasicBlock* block = &mkBasicBlock();

      //Get the guard
      ada_base_entity lal_guard;
      ada_select_when_part_f_cond_expr(lal_element, &lal_guard);
      SgExpression& guard = getExpr_opt(&lal_guard, ctx);

      // instantiate SgAdaSelectAlternativeStmt node
      SgAdaSelectAlternativeStmt& stmt = mkAdaSelectAlternativeStmt(guard, *block);

      sg::linkParentChild(sgnode, stmt, &SgAdaSelectStmt::set_select_path);

      //Get the list of stmts & handle them
      ada_base_entity lal_stmt_list;
      ada_select_when_part_f_stmts(lal_element, &lal_stmt_list);
      int count = ada_node_children_count(&lal_stmt_list);
      for(int i = 0; i < count; ++i){
        ada_base_entity lal_stmt;
        if(ada_node_child(&lal_stmt_list, i, &lal_stmt) != 0){
          handleStmt(&lal_stmt, ctx.scope(*block));
        }
      }
    }

    void handleElseAlternative(SgAdaSelectStmt& sgnode, ada_base_entity* lal_element, AstContext ctx){

      SgBasicBlock* block   = &mkBasicBlock();

      sg::linkParentChild(sgnode, *block, &SgAdaSelectStmt::set_else_path);

      //Handle the list of stmts
      int count = ada_node_children_count(lal_element);
      for(int i = 0; i < count; ++i){
        ada_base_entity lal_stmt;
        if(ada_node_child(lal_element, i, &lal_stmt) != 0){
          handleStmt(&lal_stmt, ctx.scope(*block));
        }
      }
    }

    void handleAbortAlternative(SgAdaSelectStmt& sgnode, ada_base_entity* lal_element, AstContext ctx){
      SgBasicBlock* block   = &mkBasicBlock();

      sg::linkParentChild(sgnode, *block, &SgAdaSelectStmt::set_abort_path);

      //Handle the list of stmts
      int count = ada_node_children_count(lal_element);
      for(int i = 0; i < count; ++i){
        ada_base_entity lal_stmt;
        if(ada_node_child(lal_element, i, &lal_stmt) != 0){
          handleStmt(&lal_stmt, ctx.scope(*block));
        }
      }
    }

  ///@}

  /// Reformats a list of Type ptrs to an OperatorCallSupplement::ArgDescList by adding an empty string parameter to each
  OperatorCallSupplement::ArgDescList
  toArgDescList(const SgTypePtrList& typlist)
  {
    OperatorCallSupplement::ArgDescList res;
    auto toArgDesc = [](SgType* ty) -> ArgDesc { return {"", ty}; };

    std::transform(typlist.begin(), typlist.end(), std::back_inserter(res), toArgDesc);
    return res;
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

    SgAdaInheritedFunctionSymbol& sgnode = mkAdaInheritedFunctionSymbol(*fnsym, derivedType, baseType, ctx.scope());
    const auto inserted = inheritedSymbols().insert(std::make_pair(InheritedSymbolKey{fn, &derivedType}, &sgnode));

    ASSERT_require(inserted.second);
  }

  /// Returns the hash for the label referenced by \ref lal_element
  int getLabelRef(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    int lal_element_hash = hash_node(lal_element);

    if(kind == ada_dotted_name)
    {
      logKind("ada_dotted_name", lal_element_hash);
      //Get the suffix
      ada_base_entity lal_suffix;
      ada_dotted_name_f_suffix(lal_element, &lal_suffix);
      return getLabelRef(&lal_suffix, ctx);
    }

    logKind("ada_identifier", lal_element_hash);
    ada_base_entity lal_label_decl;
    ada_expr_p_first_corresponding_decl(lal_element, &lal_label_decl);
    if(ada_node_is_null(&lal_label_decl)){
      //Try p_referenced_decl instead
      ada_name_p_referenced_decl(lal_element, 1, &lal_label_decl);
    }
    return hash_node(&lal_label_decl);
  }

} //End unnamed namespace


/// converts a libadalang parameter declaration to a ROSE parameter (i.e., variable)
///   declaration.
SgVariableDeclaration&
getParm(ada_base_entity* lal_element, AstContext ctx)
{
  ada_node_kind_enum lal_element_kind = ada_node_kind(lal_element);
  if(lal_element_kind != ada_param_spec && lal_element_kind != ada_object_decl){
    logError() << "getParm called with improper node of kind " << lal_element_kind << "!\n";
  }

  //Get the names
  ada_base_entity defining_name_list;
  if(lal_element_kind == ada_param_spec){
    ada_param_spec_f_ids(lal_element, &defining_name_list);
  } else {
    ada_object_decl_f_ids(lal_element, &defining_name_list);
  }

  //Get the aliased status
  ada_base_entity has_aliased;
  if(lal_element_kind == ada_param_spec){
    ada_param_spec_f_has_aliased(lal_element, &has_aliased);
  } else {
    ada_object_decl_f_has_aliased(lal_element, &has_aliased);
  }
  ada_node_kind_enum aliased_status = ada_node_kind(&has_aliased);
  const bool               aliased  = (aliased_status == ada_aliased_present);

  //Get the type of this parameter
  ada_base_entity subtype_indication;
  if(lal_element_kind == ada_param_spec){
    ada_param_spec_f_type_expr(lal_element, &subtype_indication);
  } else {
    ada_object_decl_f_type_expr(lal_element, &subtype_indication);
  }

  //Get the mode
  ada_base_entity lal_mode;
  if(lal_element_kind == ada_param_spec){
    ada_param_spec_f_mode(lal_element, &lal_mode);
  } else {
    ada_object_decl_f_mode(lal_element, &lal_mode);
  }

  SgType&                  basety   = getDeclType(&subtype_indication, ctx);

  SgType&                  parmtype = aliased ? mkAliasedType(basety) : basety;

  SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                       libadalangVars(),
                                                                       &defining_name_list,
                                                                       parmtype,
                                                                       getVarInit(lal_element, &parmtype, ctx)
                                                                     );


  SgVariableDeclaration&   sgnode   = mkParameter(dclnames, getMode(&lal_mode), ctx.scope());

  attachSourceLocation(sgnode, lal_element, ctx);

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
      ada_params_f_params(&param_list, &param_list);
    }

    int count = ada_node_children_count(&param_list);
    for (int i = 0; i < count; ++i)
    {
      ada_base_entity child;

      if(ada_node_child(&param_list, i, &child) == 0){
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

/// Find the first corresponding decl for \ref lal_element, then hash it and search libadalangDecls & adaPkgs
SgDeclarationStatement*
queryDecl(ada_base_entity* lal_element, int defining_name_hash, AstContext /*ctx*/)
{
  ada_node_kind_enum kind = ada_node_kind(lal_element);

  ada_base_entity corresponding_decl;
  ada_expr_p_first_corresponding_decl(lal_element, &corresponding_decl);
  int decl_hash = 0;
  if(!ada_node_is_null(&corresponding_decl)){
    decl_hash = hash_node(&corresponding_decl);
  }

  SgDeclarationStatement* res = findFirst(libadalangDecls(), defining_name_hash, decl_hash);

  if((res == nullptr) && (kind == ada_identifier)){
    res = findFirst(adaPkgs(), defining_name_hash, decl_hash);
  }

  return res;
}

void handleStmt(ada_base_entity* lal_stmt, AstContext ctx, const std::string& lblname)
  {
    //~using PragmaContainer = AstContext::PragmaContainer;

    //Get the kind of this node
    ada_node_kind_enum kind = ada_node_kind(lal_stmt);

    LibadalangText kind_name(kind);
    std::string kind_name_string = kind_name.string_value();
    logTrace() << "handleStmt called on a " << kind_name_string << std::endl;

    if(kind == ada_pragma_node)
    {
      handlePragma(lal_stmt, nullptr, ctx);
      return;
    }

    //Get the hash
    int lal_stmt_hash = hash_node(lal_stmt);

    SgStatement*            assocstmt = nullptr;

    switch(kind){
      case ada_null_stmt:                    // 5.1
        {
          logKind("ada_null_stmt", lal_stmt_hash);

          SgNullStatement& sgnode = mkNullStatement();

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_assign_stmt:             // 5.2
        {
          logKind("ada_assign_stmt", lal_stmt_hash);

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
          logKind("ada_if_stmt", lal_stmt_hash);

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
          logKind("ada_case_stmt", lal_stmt_hash);

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
          logKind("ada_named_stmt", lal_stmt_hash);
          //Get the name
          ada_base_entity lal_decl;
          ada_named_stmt_f_decl(lal_stmt, &lal_decl);
          ada_named_stmt_decl_f_name(&lal_decl, &lal_decl);
          ada_defining_name_f_name(&lal_decl, &lal_decl);
          std::string label_name = getFullName(&lal_decl);

          //Get the stmt
          ada_base_entity lal_named_stmt;
          ada_named_stmt_f_stmt(lal_stmt, &lal_named_stmt);

          //Call handleStmt on the substmt, passing in the name
          handleStmt(&lal_named_stmt, ctx, label_name);

          break;
        }
      case ada_while_loop_stmt:              // 5.5
        {
          logKind("ada_while_loop_stmt", lal_stmt_hash);

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
          logKind("ada_loop_stmt", lal_stmt_hash);

          SgBasicBlock&   block    = mkBasicBlock();
          SgAdaLoopStmt&  sgnode   = mkAdaLoopStmt(block);

          //Get the stmts
          ada_base_entity lal_loop_stmt_list;
          ada_base_loop_stmt_f_stmts(lal_stmt, &lal_loop_stmt_list);

          completeStmt(sgnode, lal_stmt, ctx, lblname);

          int hash = hash_node(lal_stmt);
          recordNode(ctx.labelsAndLoops().libadalangLoops(), hash, sgnode);

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
          logKind("ada_for_loop_stmt", lal_stmt_hash);

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
          logKind("ada_label", lal_stmt_hash);

          //Make a null stmt for this label to attach to
          SgNullStatement& sgnode = mkNullStatement();

          completeStmt(sgnode, lal_stmt, ctx);

          //Add the label
          ada_base_entity lal_label_decl, lal_ident;
          ada_label_f_decl(lal_stmt, &lal_label_decl);
          ada_label_decl_f_name(&lal_label_decl, &lal_ident);
          ada_defining_name_f_name(&lal_ident, &lal_ident);
          std::string label_name = getFullName(&lal_ident);

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
          logKind(kind == ada_decl_block ? "ada_decl_block" : "ada_begin_block", lal_stmt_hash);

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
          ada_base_entity lal_decls, lal_stmts, lal_exceptions;

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
          logKind("ada_exit_stmt", lal_stmt_hash);

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
          logKind("ada_goto_stmt", lal_stmt_hash);
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
          logKind("ada_call_stmt", lal_stmt_hash);

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
          } else if(lal_call_expr_kind == ada_identifier){
            //There are no args

            //Get the name
            lal_call_name = lal_call_expr;
          } else if(lal_call_expr_kind == ada_attribute_ref){
            //Even though this has args, the args are handled by getExpr, so we only need the name.
            lal_call_name = lal_call_expr;
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
          logKind("ada_return_stmt", lal_stmt_hash);

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
          logKind("ada_accept_stmt", lal_stmt_hash);
          //Get the name
          ada_base_entity lal_identifier;
          ada_accept_stmt_f_name(lal_stmt, &lal_identifier);

          //Get the entry_index
          ada_base_entity lal_entry_index_expr;
          ada_accept_stmt_f_entry_index_expr(lal_stmt, &lal_entry_index_expr);

          SgExpression&            entryref = getExpr(&lal_identifier, ctx);
          SgExpression&            idx      = getExpr_opt(&lal_entry_index_expr, ctx);
          SgAdaAcceptStmt&         sgnode   = mkAdaAcceptStmt(entryref, idx);

          //Set the SgFunctionParameterScope to have the correct parent
          //  It initially has the SgAdaAcceptStmt node as its parent, but this is bad since that node isn't a decl
          //  The ASIS frontend sets it to the corresponding SgAdaEntryDecl node somewhere, but I can't figure out where, so I'll just do it here
          ada_base_entity lal_decl_name;
          ada_name_p_referenced_decl(&lal_identifier, 1, &lal_decl_name);
          ada_entry_decl_f_spec(&lal_decl_name, &lal_decl_name);
          ada_entry_spec_f_entry_name(&lal_decl_name, &lal_decl_name);
          int decl_hash = hash_node(&lal_decl_name);
          SgDeclarationStatement& entrydecl = lookupNode(libadalangDecls(), decl_hash);
          SgFunctionParameterScope* parmScope = sgnode.get_parameterScope();
          parmScope->set_parent(&entrydecl);

          completeStmt(sgnode, lal_stmt, ctx);

          //Get the params
          ada_base_entity params;
          ada_accept_stmt_f_params(lal_stmt, &params);
          ada_entry_completion_formal_params_f_params(&params, &params);
          ada_params_f_params(&params, &params);
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
            ada_base_entity lal_stmts, lal_exceptions;
            ada_handled_stmts_f_stmts(&handled_stmts, &lal_stmts);
            ada_handled_stmts_f_exceptions(&handled_stmts, &lal_exceptions);

            completeHandledBlock( &lal_stmts,
                                  &lal_exceptions,
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
      case ada_requeue_stmt:                 // 9.5.4
        {
          logKind("ada_requeue_stmt", lal_stmt_hash);

          //Get the abort status for this stmt
          ada_base_entity lal_has_abort;
          ada_requeue_stmt_f_has_abort(lal_stmt, &lal_has_abort);
          const bool withAbort = (ada_node_kind(&lal_has_abort) == ada_abort_present);

          //Get the call name for this stmt
          ada_base_entity lal_call_name;
          ada_requeue_stmt_f_call_name(lal_stmt, &lal_call_name);

          SgExpression&              expr   = getExpr(&lal_call_name, ctx);
          SgProcessControlStatement& sgnode = mkRequeueStmt(expr, withAbort);

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_delay_stmt:             // 9.6
        {
          logKind("ada_delay_stmt", lal_stmt_hash);

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
          logKind("ada_raise_stmt", lal_stmt_hash);
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
      case ada_select_stmt:    // 9.7.4
        {
          logKind("ada_select_stmt", lal_stmt_hash);

          //Get the various paths that can be taken
          ada_base_entity lal_guard_list, lal_else_list, lal_abort_list;
          ada_select_stmt_f_guards(lal_stmt, &lal_guard_list);
          ada_select_stmt_f_else_stmts(lal_stmt, &lal_else_list);
          ada_select_stmt_f_abort_stmts(lal_stmt, &lal_abort_list);

          //TODO This could also have 2 other values, how do we recognize them?
          //e_selective_accept also works for e_timed_entry & e_conditional_entry, so we don't need to?
          SgAdaSelectStmt& sgnode = ada_node_children_count(&lal_abort_list) ? mkAdaSelectStmt(SgAdaSelectStmt::e_asynchronous) : mkAdaSelectStmt(SgAdaSelectStmt::e_selective_accept);

          completeStmt(sgnode, lal_stmt, ctx);

          //Traverse over each path, and call its respective handler
          int count = ada_node_children_count(&lal_guard_list);
          SgAdaSelectAlternativeStmt* currOrPath = nullptr;
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_guard;
            if(ada_node_child(&lal_guard_list, i, &lal_guard) != 0){
              //The first child is a select alternative, the rest are or alternatives
              if(i == 0){
                handleSelectAlternative(sgnode, &lal_guard, ctx);
              } else {
                currOrPath = handleOrAlternative(sgnode, currOrPath, &lal_guard, ctx);
              }
            }
          }

          if(ada_node_children_count(&lal_else_list) != 0){
            handleElseAlternative(sgnode, &lal_else_list, ctx);
          }

          if(ada_node_children_count(&lal_abort_list) != 0){
            handleAbortAlternative(sgnode, &lal_abort_list, ctx);
          }

          assocstmt = &sgnode;
          break;
        }
      case ada_abort_stmt:                  // 9.8
        {
          logKind("ada_abort_stmt", lal_stmt_hash);

          //Get the list of tasks to abort
          ada_base_entity lal_name_list;
          ada_abort_stmt_f_names(lal_stmt, &lal_name_list);

          std::vector<SgExpression*> aborted;
          int count = ada_node_children_count(&lal_name_list);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_aborted_name;
            if(ada_node_child(&lal_name_list, i, &lal_aborted_name) != 0){
              aborted.push_back(&getExpr(&lal_aborted_name, ctx));
            }
          }

          SgExprListExp&             abortList = mkExprListExp(aborted);
          SgProcessControlStatement& sgnode    = mkAbortStmt(abortList);

          completeStmt(sgnode, lal_stmt, ctx);

          assocstmt = &sgnode;
          break;
        }
      case ada_terminate_alternative:   // 9.7.1
        {
          logKind("ada_terminate_alternative", lal_stmt_hash);

          SgAdaTerminateStmt& sgnode = mkTerminateStmt();

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
      //Clauses
      case ada_with_clause:           // 10.1.2
      case ada_use_type_clause:       // 8.4
      case ada_use_package_clause:    // 8.4
      case ada_attribute_def_clause:  // 13.3
      case ada_record_rep_clause:     // 13.5.1
        {
          handleClause(lal_stmt, ctx);
          return;
        }
      case ada_error_stmt:              // If this node exists, the input code is malformed
      case ada_error_decl:
        {
          //TODO Do we want the frontend to break on malformed code? This could just be a warning.
          logFatal() << "ada_error_stmt encountered!\n";
          sg::report_error("ada_error_stmt encountered!");
          break;
        }
      default:
        {
          logWarn() << "Unhandled statement " << kind_name_string << std::endl;
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

    logKind("ada_exception_handler", hash_node(lal_element));

    //Get the name for this handler
    ada_base_entity lal_name;
    ada_exception_handler_f_exception_name(lal_element, &lal_name);

    std::string name = "";
    if(!ada_node_is_null(&lal_name)){
      ada_defining_name_f_name(&lal_name, &lal_name);
      name = getFullName(&lal_name);
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
  }


namespace {
  SgExpression* createAspect(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_base_entity lal_id, lal_expr;

    ada_aspect_assoc_f_id(lal_element, &lal_id);
    ada_aspect_assoc_f_expr(lal_element, &lal_expr);

    std::string cxx_markname = getFullName(&lal_id);
    const char* markname = cxx_markname.c_str();

    if(ada_node_is_null(&lal_id) || cxx_markname == "")
    {
      logError() << "Unexpected Aspect mark representation: "
         << ada_node_kind(&lal_id) << std::endl;
      return nullptr;
    }

    SgExpression&                sgdefn = getExpr_opt(&lal_expr, ctx);
    SgExpression&                sgnode = SG_DEREF(sb::buildActualArgumentExpression(markname, &sgdefn));

    attachSourceLocation(sgnode, lal_element, ctx);
    return &sgnode;
  }


  void processAspects(ada_base_entity* lal_element, SgDeclarationStatement* sgnode, AstContext ctx)
  {
    //Check that this element is of a kind that can have aspects
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    if(kind < ada_abstract_state_decl || kind > ada_single_task_decl){
      logFlaw() << "processAspects called on node kind " << kind << " (which cannot have aspects)!\n";
      return;
    }

    ada_base_entity lal_aspect_spec;
    ada_basic_decl_f_aspects(lal_element, &lal_aspect_spec);

    //Check if this element has any aspects
    if(ada_node_is_null(&lal_aspect_spec)){
      return;
    }

    //Get the list of aspects
    ada_base_entity lal_aspect_assoc_list_deref;
    ada_aspect_spec_f_aspect_assocs(&lal_aspect_spec, &lal_aspect_assoc_list_deref);

    if (sgnode == nullptr)
    {
      logError() << "found aspects w/o corresponding Sage declaration."
                 << std::endl;
      return;
    }

    // aspects use deferred elaboration, so that they can use
    //   type-bound operations defined later in scope.

    auto deferredAspectCompletion =
        [sgnode, lal_aspect_assoc_list_deref, ctx]()->void
        {
          ada_base_entity lal_aspect_assoc_list = lal_aspect_assoc_list_deref;
          std::vector<SgExpression*> aspects;
          int range = ada_node_children_count(&lal_aspect_assoc_list);
          for(int i = 0; i < range; ++i){
            ada_base_entity lal_aspect_assoc;
            if(ada_node_child(&lal_aspect_assoc_list, i, &lal_aspect_assoc) != 0){
              aspects.push_back(createAspect(&lal_aspect_assoc, ctx.pragmaAspectAnchor(*sgnode)));
            }
          }
          SgExprListExp& asplist = mkExprListExp(aspects);

          sg::linkParentChild(*sgnode, asplist, &SgDeclarationStatement::set_adaAspects);
        };

    ctx.storeDeferredUnitCompletion( std::move(deferredAspectCompletion) );
  }

  //Handle a single type or object decl as part of an instantiation of a generic package or subp
  // \ref lal_element: an ada_generic_formal<type/obj>_decl that the instantiated decl will be based off
  // \ref lal_expr: The value/configuration that the instantiation set for this variable/type
  // \ref ctx: The scope of the instantiated package/subp
  void handleFormalDeclInstantiation(ada_base_entity* lal_element, ada_base_entity* lal_expr, AstContext ctx)
  {
    //TODO: lal_expr can be ada_box_expr, and types can be private. Neither case is handled well at the moment.
    //Get the kind
    ada_node_kind_enum lal_element_kind = ada_node_kind(lal_element);

    //Get the hash
    int lal_element_hash = hash_node(lal_element);

    //Get the decl
    ada_base_entity lal_formal_decl;
    ada_generic_formal_f_decl(lal_element, &lal_formal_decl);

    if(lal_element_kind == ada_generic_formal_type_decl){
      logKind("ada_generic_formal_type_decl", lal_element_hash);
      //Treat this as a subtype declaration, with lal_expr as the original type

      //Get the name
      ada_base_entity lal_identifier;
      ada_base_type_decl_f_name(&lal_formal_decl, &lal_identifier);
      ada_defining_name_f_name(&lal_identifier, &lal_identifier);
      std::string ident = getFullName(&lal_identifier);

      //Get the original type & make a subtype
      SgType& baseType = getDeclType(lal_expr, ctx);
      SgAdaTypeConstraint& range = getConstraint(nullptr, ctx); //I don't think we can have a constraint?
      SgType& subtype = mkAdaSubtype(baseType, range);

      SgScopeStatement&     scope   = ctx.scope();
      SgTypedefDeclaration& sgnode  = mkTypeDecl(ident, subtype, scope);

      //~privatize(sgnode, isPrivate); //Cannot be private
      attachSourceLocation(sgnode, lal_element, ctx);
      ctx.appendStatement(sgnode);
      recordNode(libadalangTypes(), hash_node(&lal_formal_decl), sgnode); //This overwrites the generic's decl
    } else if(lal_element_kind == ada_generic_formal_obj_decl){
      logKind("ada_generic_formal_obj_decl", lal_element_hash);
      //Treat this as a constant declaration, with lal_expr as the value

      //We can't just use handleVarCstDecl b/c we have a different varinit
      SgType& basety = mkConstType(getVarType(&lal_formal_decl, ctx));

      //Get the aliased status
      ada_base_entity lal_has_aliased;
      ada_object_decl_f_has_aliased(lal_element, &lal_has_aliased);

      ada_node_kind_enum aliased_status = ada_node_kind(&lal_has_aliased);
      const bool               aliased  = (aliased_status == ada_aliased_present);
      SgType& varty  = aliased ? mkAliasedType(basety) : basety;

      //Get the names
      ada_base_entity lal_ids;
      ada_object_decl_f_ids(&lal_formal_decl, &lal_ids);

      SgInitializedNamePtrList dclnames = constructInitializedNamePtrList( ctx,
                                                                           libadalangVars(),
                                                                           &lal_ids,
                                                                           basety,
                                                                           &getExpr(lal_expr, ctx)
                                                                         );

      SgVariableDeclaration&   sgnode   = mkVarDecl(dclnames, ctx.scope());

      attachSourceLocation(sgnode, lal_element, ctx);
      ctx.appendStatement(sgnode);
    } else {
      logError() << "Unhandled kind " << lal_element_kind << " in handleFormalDeclInstantiation!\n";
    }
  }

} //end unnamed namespace


//Iterate over an ada_generic_<package/subp>_decl node to create a new node for the instantiated decl
//  We need to do this b/c lal does not create nodes for compiler-generated sections, unlike ASIS.
//  So any references to instantiated packages/subps in the lal tree point back to the generic package/subp,
//  which causes "invalid prefix in selected component" errors in the generated code.
SgDeclarationStatement* createInstantiationDecl(ada_base_entity* lal_element, std::string ident, ada_base_entity* lal_param_list, SgScopeStatement* parent_scope, AstContext ctx){
  //Get the kind of this node
  ada_node_kind_enum kind = ada_node_kind(lal_element);

  //Get the hash
  int lal_element_hash = hash_node(lal_element);

  SgDeclarationStatement* instDecl = nullptr;

  //Process the param list. We'll need to do this for both kinds of decl, so here is good.
  std::vector<std::pair<std::string, ada_base_entity>> named_param_list;
  int lal_param_count = ada_node_children_count(lal_param_list);
  int last_unnamed_param_position = -1;
  for(int i = 0; i < lal_param_count; ++i){
    ada_base_entity lal_param;
    if(ada_node_child(lal_param_list, i, &lal_param) != 0){
      //Check if this param has a name
      std::string name = "";
      ada_base_entity lal_name;
      ada_param_assoc_f_designator(&lal_param, &lal_name);
      if(!ada_node_is_null(&lal_name)){
        name = canonical_text_as_string(&lal_name);
      } else {
        last_unnamed_param_position = i;
      }
      //Get the expr
      ada_base_entity lal_expr;
      ada_param_assoc_f_r_expr(&lal_param, &lal_expr);
      named_param_list.push_back(std::make_pair(name, lal_expr));
    }
  }


  switch(kind)
  {
    case ada_generic_subp_decl:
    {
      logKind("ada_generic_subp_decl", lal_element_hash);
      //Treat this node as an ada_subp_decl

      //Get the subp spec node & whether this decl is overriding another
      ada_base_entity subp_spec;
      ada_base_entity lal_overriding;
      ada_generic_subp_decl_f_subp_decl(lal_element, &subp_spec);
      ada_generic_subp_internal_f_subp_spec(&subp_spec, &subp_spec);
      ada_classic_subp_decl_f_overriding(lal_element, &lal_overriding);

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


      ada_node_kind_enum lal_overriding_kind = ada_node_kind(&lal_overriding);

      const bool              isFunc = subp_kind_kind == ada_subp_kind_function;
      const bool          overriding = (lal_overriding_kind == ada_overriding_overriding); //TODO ada_overriding_unspecified might count?
      SgType&                rettype = isFunc ? getDeclType(&subp_returns, ctx)
                                                : mkTypeVoid();

      const std::string subp_type_report = isFunc ? "function instantiation " : "procedure instantiation ";
      logTrace() << subp_type_report << ident << std::endl;

      SgScopeStatement&      logicalScope = SG_DEREF(parent_scope);
      const bool             renamingAsBody = false; //definedByRenamingID(decl.Corresponding_Body, ctx); //TODO Can lal do this?
      ParameterCompletion    complete{&subp_params, ctx};
      SgFunctionDeclaration& sgnode  = renamingAsBody
                                          ? mkAdaFunctionRenamingDecl(ident, logicalScope, rettype, std::move(complete))
                                          : mkProcedureDecl_nondef(ident, logicalScope, rettype, std::move(complete))
                                          ;

      setAbstractModifier(sgnode, lal_element);
      setOverride(sgnode, overriding);

      //There is no point to recording this node by hash, b/c we'd have to change the hash to be different
      //  from the original generic decl's hash, and no nodes in the lal tree would refer to the changed hash
      /*int hash = hash_node(lal_element);
      int defining_name_hash = hash_node(&lal_defining_name);
      recordNode(libadalangDecls(), hash, sgnode);
      recordNode(libadalangDecls(), defining_name_hash, sgnode);*/

      privatize(sgnode, false);
      //This won't point to a "real" location, but that's okay since it will be marked as compiler-generated
      attachSourceLocation(sgnode, lal_element, ctx);
      ctx.appendStatement(sgnode);

      //TODO This just creates the subp declaration, not the body. Do we need the body?
      // I'm not sure how the instantiated body would differ from the generic body.

      instDecl = &sgnode;

      break;
    }
    case ada_generic_package_decl:
    {
      logKind("ada_generic_package_decl", lal_element_hash);
      //Treat this node as an ada_package_decl

      SgAdaPackageSpecDecl& sgnode       = mkAdaPackageSpecDecl(ident, SG_DEREF(parent_scope));
      SgAdaPackageSpec&     pkgspec      = SG_DEREF(sgnode.get_definition());

      logTrace() << "package instantiation " << ident
                 << std::endl;

      //There is no point to recording this node by hash, b/c we'd have to change the hash to be different
      //  from the original generic decl's hash, and no nodes in the lal tree would refer to the changed hash
      /*int hash = hash_node(&lal_defining_name);
      int hash0 = hash_node(lal_element);
      recordNode(libadalangDecls(), hash, sgnode);
      recordNode(libadalangDecls(), hash0, sgnode);*/

      privatize(sgnode, false);
      //~ attachSourceLocation(pkgspec, lal_element, ctx);
      //This won't point to a "real" location, but that's okay since it will be marked as compiler-generated
      attachSourceLocation(sgnode, lal_element, ctx);
      ctx.appendStatement(sgnode);


      //Get the package internal
      ada_base_entity lal_package_internal;
      ada_generic_package_decl_f_package_decl(lal_element, &lal_package_internal);

      //Process the params into completed declarations
      ada_base_entity lal_formal_decls;
      ada_generic_decl_f_formal_part(lal_element, &lal_formal_decls);
      ada_generic_formal_part_f_decls(&lal_formal_decls, &lal_formal_decls);
      int formal_decl_count = ada_node_children_count(&lal_formal_decls);
      for(int i = 0; i < formal_decl_count; ++i){
        ada_base_entity lal_formal_decl;
        if(ada_node_child(&lal_formal_decls, i, &lal_formal_decl) != 0){
          //Check to see if there is a matching param in lal_param_list. Params can either be positional or named.
          // If they are named, we need to match by this decl's name, if they are positional, we match by the position instead.
          // A param list can have both positional & named, but all positional params need to come first.

          ada_base_entity lal_expr;
          //Check if the param at this decl's position has a name
          if(i <= last_unnamed_param_position){
            //Just use param_list(i)
            lal_expr = named_param_list.at(i).second;

          } else {
            //Get the name of this decl and search param_list for a matching name
            ada_base_entity lal_decl_name;
            ada_generic_formal_f_decl(&lal_formal_decl, &lal_decl_name);
            ada_basic_decl_p_defining_name(&lal_decl_name, &lal_decl_name);
            ada_defining_name_f_name(&lal_decl_name, &lal_decl_name);
            std::string decl_name = canonical_text_as_string(&lal_decl_name);

            auto vecpos = std::find_if(
                named_param_list.begin(),
                named_param_list.end(),
                [&decl_name](std::pair<std::string, ada_base_entity> i) { return i.first == decl_name; });

            if(vecpos == named_param_list.end()){
              //If there wasn't a matching name, use the default for this decl
              //Make sure this isn't a type decl, since type decls cannot have default values
              if(ada_node_kind(&lal_formal_decl) == ada_generic_formal_type_decl){
                logError() << "Instantiation " << ident << " is missing actual " << decl_name << "!\n";
                return nullptr;
              }
              ada_generic_formal_f_decl(&lal_formal_decl, &lal_expr);
              ada_object_decl_f_default_expr(&lal_expr, &lal_expr);
            } else {
              lal_expr = vecpos->second;
            }
          }
          handleFormalDeclInstantiation(&lal_formal_decl, &lal_expr, ctx.scope(pkgspec));
        }
      }


      //TODO: Calling handleElement on the same lal node twice will produce the same hash & overwrite the first record in the maps,
      //  which I thought would be a big problem. This hasn't caused any issues in any example I've run, though. Not sure why.

      // visible items
      {
        ada_base_entity public_part;
        ada_base_package_decl_f_public_part(&lal_package_internal, &public_part);
        ada_declarative_part_f_decls(&public_part, &public_part);

        if(!ada_node_is_null(&public_part)){
          int range = ada_node_children_count(&public_part);
          for(int i = 0; i < range; ++i){
            ada_base_entity lal_child;
            if(ada_node_child(&public_part, i, &lal_child) != 0){
              handleElement(&lal_child, ctx.scope(pkgspec));
            }
          }
        }
      }

      // private items
      {
        ada_base_entity private_part;
        ada_base_package_decl_f_private_part(&lal_package_internal, &private_part);
        ada_declarative_part_f_decls(&private_part, &private_part);

        if(!ada_node_is_null(&private_part)){
          int range = ada_node_children_count(&private_part);
          for(int i = 0; i < range; ++i){
            ada_base_entity lal_child;
            if(ada_node_child(&private_part, i, &lal_child) != 0){
              handleElement(&lal_child, ctx.scope(pkgspec), true /* private items */);
            }
          }

          // a package may contain an empty private section
          pkgspec.set_hasPrivate(true);
        }
      }

      instDecl = &sgnode;

      break;
    }
    default:
    {
      logError() << "Unsupported node kind " << kind << " in createInstantiationDecl!\n";
      break;
    }
  }

  return instDecl;
}

//Forward decl of this func b/c it is used in handleDeclaration
SgScopeStatement* determineParentScope(ada_base_entity* lal_element, AstContext ctx);


void handlePragma(ada_base_entity* lal_element, SgStatement* stmtOpt, AstContext ctx){
  ctx.appendStatement(createPragma_common(lal_element, stmtOpt, ctx));
}

void handleDeclaration(ada_base_entity* lal_element, AstContext ctx, bool isPrivate)
{
  using PragmaContainer = AstContext::PragmaContainer;

  SgDeclarationStatement* assocdecl = nullptr;

  //Get the kind of this node
  ada_node_kind_enum kind;
  kind = ada_node_kind(lal_element);

  LibadalangText kind_name(kind);
  std::string kind_name_string = kind_name.string_value();
  logTrace()   << "handleDeclaration called on a " << kind_name_string << std::endl;

  //Get the hash
  int lal_element_hash = hash_node(lal_element);

  //std::vector<Element_ID> pragmaVector;

  //std::copy(pragmaRange.first, pragmaRange.second, std::back_inserter(pragmaVector));

  switch (kind)
  {

    case ada_package_decl:                    // 7.1(2)
      {
        logKind("ada_package_decl", lal_element_hash);

        //Get the name for this package
        ada_base_entity lal_defining_name, lal_identifier;
        ada_base_package_decl_f_package_name(lal_element, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);

        std::string           ident        = getFullName(&lal_identifier);
        SgScopeStatement*     parent_scope = determineParentScope(&lal_defining_name, ctx);

        SgAdaPackageSpecDecl& sgnode       = mkAdaPackageSpecDecl(ident, SG_DEREF(parent_scope));
        SgAdaPackageSpec&     pkgspec      = SG_DEREF(sgnode.get_definition());

        logTrace() << "package decl " << ident
                   << std::endl;

        int hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), hash, sgnode);

        int hash0 = hash_node(lal_element);
        recordNode(libadalangDecls(), hash0, sgnode);

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
              if(ada_node_child(&public_part, i, &lal_child) != 0){
                handleElement(&lal_child, pragmaCtx.scope(pkgspec));
              }
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
              if(ada_node_child(&private_part, i, &lal_child) != 0){
                handleElement(&lal_child, pragmaCtx.scope(pkgspec), true /* private items */);
              }
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
        logKind("ada_package_body", lal_element_hash);

        //Get the defining name for this package
        ada_base_entity lal_defining_name;
        ada_package_body_f_package_name(lal_element, &lal_defining_name);

        SgScopeStatement*       parent_scope = determineParentScope(&lal_defining_name, ctx);

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
        ada_base_entity lal_decls, lal_stmts, lal_exceptions;

        ada_package_body_f_decls(lal_element, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_package_body_f_stmts(lal_element, &lal_handled_stmts);

        const bool hasBodyStatements = !ada_node_is_null(&lal_handled_stmts);

        if(hasBodyStatements){
          ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
          ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);
        } else {
          logInfo() << "No body statements for this ada_package_body\n";
          ada_package_body_f_stmts(lal_element, &lal_stmts); //Force lal_stmts and lal_exceptions to be null instead of undefined
          ada_package_body_f_stmts(lal_element, &lal_exceptions);
        }

        completeDeclarationsWithHandledBlock( &lal_decls,
                                              &lal_stmts,
                                              &lal_exceptions,
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
        logKind("ada_generic_package_decl", lal_element_hash);

        //Get the name of this package
        ada_base_entity lal_package_internal, lal_defining_name, lal_identifier;
        ada_generic_package_decl_f_package_decl(lal_element, &lal_package_internal);
        ada_base_package_decl_f_package_name(&lal_package_internal, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);
        int hash = hash_node(&lal_defining_name);

        //Get the list of formal decls
        ada_base_entity lal_formal_list;
        ada_generic_decl_f_formal_part(lal_element, &lal_formal_list);
        ada_generic_formal_part_f_decls(&lal_formal_list, &lal_formal_list);

        // create generic declaration
        SgScopeStatement*       logicalScope = determineParentScope(&lal_defining_name, ctx);
        SgAdaGenericDecl&       sgnode     = mkAdaGenericDecl(ident, SG_DEREF(logicalScope));
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
                handleElement(&lal_private_part, pragmaCtx.scope(pkgspec), true /* private items */);
              }
            }
          }
        }

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
               , lal_element_hash
               );

        //Get the name of this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_subp_spec_f_subp_name(&lal_subp_spec, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);

        SgScopeStatement*      logicalScope = determineParentScope(&lal_defining_name, ctx);

        // PP (20/10/21): the assertion does not hold for proc/func defined in their own unit
        //~ ADA_ASSERT (adaname.fullName == adaname.ident);
        SgAdaGenericDecl&      sgnode     = mkAdaGenericDecl(ident, SG_DEREF(logicalScope));
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
        logKind("ada_subp_decl?", lal_element_hash);

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
        ada_base_entity lal_defining_name, lal_identifier;
        ada_subp_spec_f_subp_name(&subp_spec, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);

        ada_node_kind_enum lal_overriding_kind = ada_node_kind(&lal_overriding);

        const bool              isFunc = subp_kind_kind == ada_subp_kind_function;
        const bool          overriding = (lal_overriding_kind == ada_overriding_overriding); //TODO ada_overriding_unspecified might count?
        SgScopeStatement* parent_scope = determineParentScope(&lal_defining_name, ctx);
        std::string              ident = getFullName(&lal_identifier);
        SgType&                rettype = isFunc ? getDeclType(&subp_returns, ctx)
                                                : mkTypeVoid();

        logKind(isFunc ? "A_Function_Declaration" : "A_Procedure_Declaration", lal_element_hash);

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
        int defining_name_hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), hash, sgnode);
        recordNode(libadalangDecls(), defining_name_hash, sgnode);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_null_subp_decl:             // 6.7
    case ada_subp_body:              // 6.3(2)
      {
        logKind("ada_subp_body?", lal_element_hash);

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
        ada_base_entity lal_defining_name, lal_identifier;
        ada_subp_spec_f_subp_name(&subp_spec, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);

        std::string             ident        = getFullName(&lal_identifier);
        const bool              isFunc       = (subp_kind_kind == ada_subp_kind_function);
        SgScopeStatement*       parent_scope = determineParentScope(&lal_defining_name, ctx);
        SgType&                 rettype      = isFunc ? getDeclType(&subp_returns, ctx)
                                                      : mkTypeVoid();

        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl);
        SgDeclarationStatement* ndef   = nullptr;
        SgFunctionDeclaration*  nondef = nullptr;

        //If this isn't the first decl, set up ndef & nondef
        if(!ada_node_is_null(&lal_previous_decl)){
          //First, check if lal_previous_decl has the same unique_identifying_name
          // Sometimes LAL connects to an incorrect previous decl; this is to catch some of those times
          // Only do this check if both the body & decl have valid unique_identifying_names
          bool unique_names_match = true;
          ada_text_type lal_body_unique_name, lal_decl_unique_name;
          int body_return = ada_basic_decl_p_unique_identifying_name(lal_element, &lal_body_unique_name);
          int decl_return = ada_basic_decl_p_unique_identifying_name(&lal_previous_decl, &lal_decl_unique_name);
          if(body_return != 0 && decl_return != 0){
            std::string body_unique_name = dot_ada_text_type_to_string(lal_body_unique_name);
            std::string decl_unique_name = dot_ada_text_type_to_string(lal_decl_unique_name);
            unique_names_match = (body_unique_name == decl_unique_name);
          }
          if(unique_names_match){
            int decl_hash = hash_node(&lal_previous_decl);
            ndef          = findFirst(libadalangDecls(), decl_hash);
            nondef        = getFunctionDeclaration(ndef ? ndef->get_firstNondefiningDeclaration() : nullptr);
          }
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
    case ada_generic_formal_obj_decl: //12.4
      {
        logKind("ada_generic_formal_obj_decl", lal_element_hash);

        ada_base_entity lal_object_decl;
        ada_generic_formal_f_decl(lal_element, &lal_object_decl);

        //Get the constant status
        ada_base_entity lal_has_constant;
        ada_object_decl_f_has_constant(&lal_object_decl, &lal_has_constant);
        ada_node_kind_enum lal_has_constant_kind = ada_node_kind(&lal_has_constant);

        SgDeclarationStatement* sgnode = nullptr;
        //If it is const, call with mkConstType
        if(lal_has_constant_kind == ada_constant_absent){
          sgnode = &getParm(&lal_object_decl, ctx);
        } else {
          sgnode = &handleVarCstDecl(&lal_object_decl, ctx, isPrivate, mkConstType);
        }

        ctx.appendStatement(SG_DEREF(sgnode));
        assocdecl = sgnode;

        break;
      }
    case ada_generic_formal_type_decl:                // 12.5(2)
      {
        logKind("ada_generic_formal_type_decl", lal_element_hash);

        //Get the name for this type
        ada_base_entity lal_type_decl, lal_defining_name, lal_identifier;
        ada_generic_formal_f_decl(lal_element, &lal_type_decl);
        ada_base_type_decl_f_name(&lal_type_decl, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

        std::string          ident = getFullName(&lal_identifier);
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
    case ada_generic_formal_package:             // 12.7(2)
      {
        logKind("ada_generic_formal_package", lal_element_hash);

        //Get the ada_generic_package_instantiation
        ada_base_entity lal_generic_package_instantiation;
        ada_generic_formal_f_decl(lal_element, &lal_generic_package_instantiation);

        //Get the name of the instantiated package
        ada_base_entity lal_defining_name, lal_identifier;
        ada_generic_package_instantiation_f_name(&lal_generic_package_instantiation, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);

        //Get the generic package
        ada_base_entity lal_generic_package, lal_generic_name;
        ada_generic_instantiation_p_designated_generic_decl(&lal_generic_package_instantiation, &lal_generic_package);
        ada_generic_package_decl_f_package_decl(&lal_generic_package, &lal_generic_name);
        ada_base_package_decl_f_package_name(&lal_generic_name, &lal_generic_name);
        SgDeclarationStatement*   basedecl = findFirst(libadalangDecls(), hash_node(&lal_generic_name), hash_node(&lal_generic_package));

        // generic actual part
        ada_base_entity lal_params;
        ada_generic_package_instantiation_f_params(&lal_generic_package_instantiation, &lal_params);
        std::vector<SgExpression*> args;
        int count = ada_node_children_count(&lal_params);
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_param;
          if(ada_node_child(&lal_params, i, &lal_param) != 0){
            args.push_back(&getArg(&lal_param, ctx));
          }
        }
        SgExprListExp& sg_args = mkExprListExp(args);

        // PP (3/31/22): In contrast to the instantiations above (check correctness),
        //               the scope should be the current (generic parameter) scope.
        SgScopeStatement&         scope    = ctx.scope();
        SgAdaFormalPackageDecl&   sgnode   = mkAdaFormalPackageDecl(ident, SG_DEREF(basedecl), sg_args, scope);

        int hash = hash_node(&lal_generic_package_instantiation); //This seems to be the node other nodes will refer back to
        int name_hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), hash, sgnode);
        recordNode(libadalangDecls(), name_hash, sgnode);

        attachSourceLocation(sgnode, lal_element, ctx);
        //~ privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        SgDeclarationStatement* protoDecl = createInstantiationDecl(&lal_generic_package, ident, &lal_params, &scope, ctx.scope(SG_DEREF(sgnode.get_prototypeScope())));
        sgnode.set_prototype(protoDecl);

        assocdecl = &sgnode;
        break;
      }
    case ada_generic_formal_subp_decl:           // 12.6(2)
      {
        //Get whether this is a func or a proc, & the overriding status
        ada_base_entity lal_subp_decl, lal_subp_spec, lal_subp_type, lal_overriding;
        ada_generic_formal_f_decl(lal_element, &lal_subp_decl);
        ada_classic_subp_decl_f_overriding(&lal_subp_decl, &lal_overriding); //TODO overriding isn't needed?
        ada_classic_subp_decl_f_subp_spec(&lal_subp_decl, &lal_subp_spec);
        ada_subp_spec_f_subp_kind(&lal_subp_spec, &lal_subp_type);
        ada_node_kind_enum lal_subp_type_kind = ada_node_kind(&lal_subp_type);

        const bool             isFormalFuncDecl  = (lal_subp_type_kind == ada_subp_kind_function);

        logKind( isFormalFuncDecl ? "A_Formal_Function_Declaration" : "A_Formal_Procedure_Declaration"
               , lal_element_hash
               );

        //Get the name of this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_subp_spec_f_subp_name(&lal_subp_spec, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);

        //Get the params & return type
        ada_base_entity lal_params, lal_return_type;
        ada_subp_spec_f_subp_returns(&lal_subp_spec, &lal_return_type);
        ada_subp_spec_f_subp_params(&lal_subp_spec, &lal_params);

        //Get the default expr
        ada_base_entity lal_default_expr;
        ada_formal_subp_decl_f_default_expr(&lal_subp_decl, &lal_default_expr);

        SgType&                rettype = isFormalFuncDecl ? getDeclType(&lal_return_type, ctx)
                                                          : mkTypeVoid();

        SgScopeStatement&      logicalScope = SG_DEREF(determineParentScope(&lal_defining_name, ctx));
        SgAdaSubroutineType&   funty   = mkAdaSubroutineType(rettype, ParameterCompletion{&lal_params, ctx}, ctx.scope(), false  /*isProtected*/ );
        SgExpression&          defaultInit = getDefaultFunctionExpr(&lal_default_expr, funty, ctx);
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
        logKind("ada_subtype_decl", lal_element_hash);

        //Get the name
        ada_base_entity lal_defining_name, lal_identifier;
        ada_base_type_decl_f_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);

        //Get the subtype indication
        ada_base_entity lal_subtype_indication;
        ada_subtype_decl_f_subtype(lal_element, &lal_subtype_indication);

        int                   decl_hash = hash_node(lal_element);
        int                   name_hash = hash_node(&lal_defining_name);
        const bool            forceSubtype = true;
        SgType&               subtype = getDefinitionType(&lal_subtype_indication, ctx, forceSubtype);
        SgScopeStatement&     scope   = ctx.scope();
        SgTypedefDeclaration& sgnode  = mkTypeDecl(ident, subtype, scope);

        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);
        recordNode(libadalangTypes(), decl_hash, sgnode);
        recordNode(libadalangTypes(), name_hash, sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_number_decl:            // 3.3.2(2), 3.5.6(2)
      {
        logKind("ada_number_decl", lal_element_hash);

        //Get the expr to see what kind it is (int, real, etc.)
        ada_base_entity lal_expr;
        ada_number_decl_f_expr(lal_element, &lal_expr);

        SgType* expected_type = getNumberDeclType(&lal_expr, ctx);

        assocdecl = &handleNumberDecl(lal_element, ctx, isPrivate, SG_DEREF(sb::buildAutoType()), SG_DEREF(expected_type));

        break;
      }
    case ada_protected_body:             // 9.4(7)
      {
        logKind("ada_protected_body", lal_element_hash);

        //Get the defining name of the decl
        ada_base_entity lal_defining_name;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_defining_name);
        if(ada_node_kind(&lal_defining_name) == ada_single_protected_decl){
            ada_single_protected_decl_f_name(&lal_defining_name, &lal_defining_name);
        } else {
            ada_base_type_decl_f_name(&lal_defining_name, &lal_defining_name);
        }
        int decl_hash = hash_node(&lal_defining_name);

        SgAdaProtectedBody&     pobody  = mkAdaProtectedBody();
        SgDeclarationStatement* ndef    = findFirst(libadalangDecls(), decl_hash);
        SgAdaProtectedBodyDecl* nondef  = isSgAdaProtectedBodyDecl(ndef);

        //Element_ID              specID  = queryAsisIDOfDeclaration(decl, A_Protected_Body_Stub, ctx);
        SgDeclarationStatement& podecl  = lookupNode(libadalangDecls(), decl_hash);
        SgScopeStatement&       logicalScope = SG_DEREF(determineParentScope(&lal_defining_name, ctx));
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

        //Call handleElement on each decl
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_decl;
          if(ada_node_child(&lal_decl_list, i, &lal_decl) != 0){
            handleElement(&lal_decl, ctx.scope(pobody));
          }
        }

        assocdecl = &sgnode;
        break;
      }
    case ada_task_body:                  // 9.1(6)
      {
        logKind("ada_task_body", lal_element_hash);

        //Get the name
        ada_base_entity lal_defining_name, lal_identifier;
        ada_task_body_f_name(lal_element, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);

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

        int                     hash      = hash_node(lal_element);
        int                     decl_hash = hash_node(&lal_decl_defining_name);
        std::string             ident     = getFullName(&lal_identifier);
        SgAdaTaskBody&          tskbody   = mkAdaTaskBody();
        SgDeclarationStatement* ndef      = findFirst(libadalangDecls(), decl_hash);
        SgAdaTaskBodyDecl*      nondef    = isSgAdaTaskBodyDecl(ndef);

        //Element_ID              specID  = queryAsisIDOfDeclaration(decl, A_Task_Body_Stub, ctx);
        SgDeclarationStatement& tskdecl = lookupNode(libadalangDecls(), decl_hash); //TODO What is specID?

        SgScopeStatement&       logicalScope = SG_DEREF(determineParentScope(&lal_defining_name, ctx));
        SgAdaTaskBodyDecl&      sgnode  = mkAdaTaskBodyDecl(tskdecl, nondef, tskbody, logicalScope);

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        recordNode(libadalangDecls(), hash, sgnode);

        //Get the stmts, decls, and exceptions for this package
        ada_base_entity lal_decls, lal_stmts, lal_exceptions;

        ada_task_body_f_decls(lal_element, &lal_decls); //lal_decls should now be an ada_declarative_part
        ada_declarative_part_f_decls(&lal_decls, &lal_decls); //lal_decls should now be the list of decls
        ada_base_entity lal_handled_stmts; //This is an intermediary node required to get the stmts and exceptions
        ada_task_body_f_stmts(lal_element, &lal_handled_stmts);
        ada_handled_stmts_f_stmts(&lal_handled_stmts, &lal_stmts);
        ada_handled_stmts_f_exceptions(&lal_handled_stmts, &lal_exceptions);

        completeDeclarationsWithHandledBlock( &lal_decls,
                                              &lal_stmts,
                                              &lal_exceptions,
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
        logKind("ada_entry_body", lal_element_hash);

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

        logKind(isFunc ? "A_Function_Body_Stub" : "A_Procedure_Body_Stub", lal_element_hash);

        //Get the name
        ada_base_entity lal_defining_name, lal_identifier;
        ada_subp_spec_f_subp_name(&subp_spec, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);

        std::string             ident        = getFullName(&lal_identifier);
        SgScopeStatement*       parent_scope = determineParentScope(&lal_defining_name, ctx);
        SgType&                 rettype      = isFunc ? getDeclType(&subp_returns, ctx)
                                                      : mkTypeVoid();

        //This node can have a non-null p_previous_part, but that is only because LAL is weird.
        // Since this node represents an 'is separate' decl, it is always the first defining declaration.
        // So, there is no need to search for a previous decl, because even if we found something it would be wrong.
        SgFunctionDeclaration*  nondef  = nullptr;
        SgScopeStatement&       logicalScope = SG_DEREF(parent_scope);
        SgFunctionDeclaration&  sgnode  = createFunDcl(nondef, ident, logicalScope, rettype, ParameterCompletion{&subp_params, ctx});

        setAdaSeparate(sgnode, true /* separate */);

        int decl_hash = hash_node(lal_element);
        int name_hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), decl_hash, sgnode);
        recordNode(libadalangDecls(), name_hash, sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_task_body_stub:                         // 10.1.3(5)
      {
        logKind("ada_task_body_stub", lal_element_hash);

        //Get the name for this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_task_body_stub_f_name(lal_element, &lal_defining_name);
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);

        //Get the full decl
        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl); //TODO Is this always an ada_single_task_decl?
        ada_single_task_decl_f_task_type(&lal_previous_decl, &lal_previous_decl);
        ada_base_type_decl_f_name(&lal_previous_decl, &lal_previous_decl);

        int               previous_hash = hash_node(&lal_previous_decl);
        int                        hash = hash_node(&lal_defining_name);
        std::string               ident = getFullName(&lal_identifier);
        SgDeclarationStatement& tskdecl = lookupNode(libadalangDecls(), previous_hash);
        SgScopeStatement&       logicalScope = SG_DEREF(determineParentScope(&lal_defining_name, ctx));
        SgAdaTaskBodyDecl&      sgnode  = mkAdaTaskBodyDecl_nondef(tskdecl, logicalScope);

        setAdaSeparate(sgnode, true /* separate */);

        recordNode(libadalangDecls(), hash, sgnode);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_exception_decl:                 // 11.1(2)
      {
        logKind("ada_exception_decl", lal_element_hash);

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

          std::string        ident   = getFullName(&lal_identifier);
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
        logKind("ada_component_decl", lal_element_hash);

        handleVarCstDecl(lal_element, ctx, isPrivate, tyIdentity);

        // assocdecl = &sgnode;
        break;
      }
    case ada_generic_package_renaming_decl:   // 8.5.3(2)
    case ada_package_renaming_decl:           // 8.5.3(2)
      {
        logKind("ada_generic_package_renaming_decl?", lal_element_hash);

        //Get the name for this decl
        ada_base_entity lal_defining_name, lal_identifier;
        if(kind == ada_generic_package_renaming_decl){
            ada_generic_package_renaming_decl_f_name(lal_element, &lal_defining_name);
        } else {
            ada_package_renaming_decl_f_name(lal_element, &lal_defining_name);
        }
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);
        int          hash = hash_node(&lal_defining_name);

        //Get the package we are renaming
        ada_base_entity lal_renames, lal_renamed_object;
        if(kind == ada_generic_package_renaming_decl){
            ada_generic_package_renaming_decl_f_renames(lal_element, &lal_renames);
        } else {
            ada_package_renaming_decl_f_renames(lal_element, &lal_renames);
            ada_renaming_clause_f_renamed_object(&lal_renames, &lal_renames);
        }

        int check_package_return = 1;
        if(kind == ada_generic_package_renaming_decl){
            check_package_return = ada_expr_p_first_corresponding_decl(&lal_renames, &lal_renamed_object);
        } else {
            check_package_return = ada_package_renaming_decl_p_final_renamed_package(lal_element, &lal_renamed_object);
        }

        //LAL_REP_ISSUE: Sometimes, ada_package_renaming_decl and the ada_identifier in its ada_renaming_clause
        // will not link to a real node. The package will exist, other nodes will previous have had p_referenced_decl
        // or p_first_corresponding_decl correctly point to it, but this node & its children will point to null nodes instead.
        // Moreover, lal-dot-tree will display the fields as pointing to the package, but ROSE will not match this.
        // This implies there is a difference in how Libadalang is configured/fed info between lal-dot-tree and ROSE,
        // but I have no idea what it might be at the moment. Ex: c393ao6
        if(check_package_return == 0 || ada_node_is_null(&lal_renamed_object)) {
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

        //Also record this node by the package_decl's hash
        hash = hash_node(lal_element);
        recordNode(libadalangDecls(), hash, sgnode);

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_subp_renaming_decl:         // 8.5.4(2)
      {
        logKind("ada_subp_renaming_decl", lal_element_hash);

        //Get the name of this decl
        ada_base_entity lal_subp_spec, lal_defining_name, lal_identifier;
        ada_base_subp_body_f_subp_spec(lal_element, &lal_subp_spec);
        ada_subp_spec_f_subp_name(&lal_subp_spec, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

        //Get whether this is a func or proc
        ada_base_entity lal_subp_kind;
        ada_subp_spec_f_subp_kind(&lal_subp_spec, &lal_subp_kind);

        //Get the return type (if it exists)
        ada_base_entity lal_subp_returns;
        ada_subp_spec_f_subp_returns(&lal_subp_spec, &lal_subp_returns);

        //Get the list of params
        ada_base_entity lal_subp_params;
        ada_subp_spec_f_subp_params(&lal_subp_spec, &lal_subp_params);

        //Get the overriding status
        ada_base_entity lal_overriding;
        ada_base_subp_body_f_overriding(lal_element, &lal_overriding);

        //Get the renamed object
        ada_base_entity lal_renamed_object;
        ada_subp_renaming_decl_f_renames(lal_element, &lal_renamed_object);
        ada_renaming_clause_f_renamed_object(&lal_renamed_object, &lal_renamed_object);

        const bool                 isFuncRename = (ada_node_kind(&lal_subp_kind) == ada_subp_kind_function);

        SgScopeStatement&          outer     = ctx.scope();
        std::string                ident     = getFullName(&lal_identifier);
        int                        hash      = hash_node(&lal_defining_name);
        SgType&                    rettype   = isFuncRename ? getDeclType(&lal_subp_returns, ctx)
                                                            : mkTypeVoid();
        SgDeclarationStatement* nondefDcl    = nullptr;
        SgAdaFunctionRenamingDecl* nondefFun = nullptr;

        //Search for a previous decl
        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl);
        //If this isn't the first decl, set up nondef
        if(!ada_node_is_null(&lal_previous_decl)){
          int decl_hash = hash_node(&lal_previous_decl);
          nondefDcl = findFirst(libadalangDecls(), decl_hash);
          nondefFun = isSgAdaFunctionRenamingDecl(nondefDcl);
        }

        SgAdaFunctionRenamingDecl& sgnode    = mkAdaFunctionRenamingDecl( ident,
                                                                          outer,
                                                                          rettype,
                                                                          ParameterCompletion{&lal_subp_params, ctx},
                                                                          nondefFun );
        setOverride(sgnode, ada_node_kind(&lal_overriding) == ada_overriding_overriding);
        recordNode(libadalangDecls(), hash, sgnode);

        // find declaration for the thing being renamed
        SgFunctionType&            fntype = SG_DEREF(isSgFunctionType(sgnode.get_type()));
        const SgTypePtrList&       typlist = SG_DEREF(fntype.get_argument_list()).get_arguments();
        OperatorCallSupplement     suppl(toArgDescList(typlist), &rettype);
        SgExpression&              renamedFun = getExpr(&lal_renamed_object, ctx, std::move(suppl));

        sgnode.set_renamed_function(&renamedFun);
        privatize(sgnode, isPrivate);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_generic_subp_renaming_decl: // 8.5.5(2)
      {
        // \todo consider folding the code into generic_package_renaming

        logKind("ada_generic_subp_renaming_decl", lal_element_hash);

        //Get the name of this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_generic_subp_renaming_decl_f_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string  ident = getFullName(&lal_identifier);
        int           hash = hash_node(&lal_defining_name);

        //Get the renamed entity
        ada_base_entity lal_renames;
        ada_generic_subp_renaming_decl_f_renames(lal_element, &lal_renames);

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
    case ada_generic_package_instantiation:        // 12.3(2)
    case ada_generic_subp_instantiation:           // 12.3(2)
      {
        // generic instantiation
        if(kind == ada_generic_package_instantiation){
          logKind("ada_generic_package_instantiation", lal_element_hash);
        } else {
          logKind("ada_generic_subp_instantiation", lal_element_hash);
        }

        //Get the name for this instantiation
        ada_base_entity lal_defining_name, lal_identifier;
        if(kind == ada_generic_package_instantiation){
          ada_generic_package_instantiation_f_name(lal_element, &lal_defining_name);
        } else {
          ada_generic_subp_instantiation_f_subp_name(lal_element, &lal_defining_name);
        }
        ada_name_p_relative_name(&lal_defining_name, &lal_identifier);
        std::string  ident = getFullName(&lal_identifier);
        int           hash = hash_node(&lal_defining_name);

        //Get the decl we are instantiating off of (It's either generic or a renaming of a generic)
        ada_base_entity lal_generic_decl, lal_generic_decl_name;
        int generic_return = 1;
        //First try p_first_corresponding_decl, then p_referenced_decl, then p_designated_generic_decl
        if(kind == ada_generic_package_instantiation){
          ada_generic_package_instantiation_f_generic_pkg_name(lal_element, &lal_generic_decl_name);
        } else {
          ada_generic_subp_instantiation_f_subp_name(lal_element, &lal_generic_decl_name);
        }

        generic_return = ada_expr_p_first_corresponding_decl(&lal_generic_decl_name, &lal_generic_decl);
        if(generic_return == 0 || ada_node_is_null(&lal_generic_decl)){
          generic_return = ada_name_p_referenced_decl(&lal_generic_decl_name, 1, &lal_generic_decl);
        }

        if(generic_return == 0 || ada_node_is_null(&lal_generic_decl)){
          generic_return = ada_generic_instantiation_p_designated_generic_decl(lal_element, &lal_generic_decl);
        }

        if(generic_return == 0 || ada_node_is_null(&lal_generic_decl)){
          logFatal() << "Could not find base decl in Libadalang tree!\n";
        }

        //Get the kind of the decl, and use that to get its defining_name
        ada_node_kind_enum lal_generic_decl_kind = ada_node_kind(&lal_generic_decl);
        if(lal_generic_decl_kind == ada_generic_package_decl){
          ada_generic_package_decl_f_package_decl(&lal_generic_decl, &lal_generic_decl_name);
          ada_base_package_decl_f_package_name(&lal_generic_decl_name, &lal_generic_decl_name);
        } else if(lal_generic_decl_kind == ada_generic_subp_decl){
          ada_generic_subp_decl_f_subp_decl(&lal_generic_decl, &lal_generic_decl_name);
          ada_generic_subp_internal_f_subp_spec(&lal_generic_decl_name, &lal_generic_decl_name);
          ada_subp_spec_f_subp_name(&lal_generic_decl_name, &lal_generic_decl_name);
        } else if(lal_generic_decl_kind == ada_generic_package_renaming_decl){
          ada_generic_package_renaming_decl_f_name(&lal_generic_decl, &lal_generic_decl_name);
        } else if(lal_generic_decl_kind == ada_generic_subp_internal){
          ada_generic_subp_internal_f_subp_spec(&lal_generic_decl, &lal_generic_decl_name);
          ada_subp_spec_f_subp_name(&lal_generic_decl_name, &lal_generic_decl_name);
        } else {
          logError() << "Unhandled base decl kind: " << lal_generic_decl_kind << "!\n";
        }
        int base_decl_hash = hash_node(&lal_generic_decl_name);
        SgDeclarationStatement*   basedecl = findFirst(libadalangDecls(), base_decl_hash);

        if(basedecl == nullptr)
        {
          logFlaw() << "Could not find base decl for ada_generic_package_instantiation!\n";
          logFlaw() << "  lal_generic_decl_kind = " << lal_generic_decl_kind << "\n";
        }

        //Get the params
        ada_base_entity lal_params;
        if(kind == ada_generic_package_instantiation){
          ada_generic_package_instantiation_f_params(lal_element, &lal_params);
        } else {
          ada_generic_subp_instantiation_f_params(lal_element, &lal_params);
        }

        // PP (2/2/22): the base decl can also be a renamed generic declaration
        SgScopeStatement*         logicalScope = determineParentScope(&lal_defining_name, ctx);
        SgAdaGenericInstanceDecl&       sgnode = mkAdaGenericInstanceDecl(ident, SG_DEREF(basedecl), SG_DEREF(logicalScope));

        {
          // generic actual part
          std::vector<SgExpression*> args;
          int count = ada_node_children_count(&lal_params);
          for(int i = 0; i < count; ++i){
            ada_base_entity lal_param;
            if(ada_node_child(&lal_params, i, &lal_param) != 0){
              args.push_back(&getArg(&lal_param, ctx));
            }
          }
          SgExprListExp& args2 = mkExprListExp(args);
          sgnode.set_actual_parameters(&args2);
        }

        recordNode(libadalangDecls(), hash, sgnode);

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        //If lal_generic_decl is a renaming, get the generic decl it renames before calling createInstantiationdecl
        if(lal_generic_decl_kind == ada_generic_package_renaming_decl){
          ada_generic_package_renaming_decl_f_renames(&lal_generic_decl, &lal_generic_decl);
          ada_expr_p_first_corresponding_decl(&lal_generic_decl, &lal_generic_decl);
        } else if(lal_generic_decl_kind == ada_generic_subp_internal){
          ada_generic_instantiation_p_designated_generic_decl(lal_element, &lal_generic_decl);
        }

        //LAL_REP_ISSUE: Libadalang's first_corresponding_decl points to the generic,
        //  whereas ASIS' Corresponding_Declaration points to a compiler generated instance (which Libadalang does not have).
        //  Use createInstantiationDecl to fake having the compiler-generated section.
        // PP (4/1/22): fill in the declaration
        SgDeclarationStatement* instDecl = createInstantiationDecl(&lal_generic_decl, ident, &lal_params, logicalScope, ctx.instantiation(sgnode).scope(SG_DEREF(sgnode.get_instantiatedScope())));

        // mark whole subtree under sgnode.get_instantiatedScope() as instantiated
        si::Ada::setSourcePositionInSubtreeToCompilerGenerated(sgnode.get_instantiatedScope());

        assocdecl = &sgnode;
        break;
      }
    case ada_for_loop_spec:           // 5.5(4)   -> Trait_Kinds
      {
        logKind("ada_for_loop_spec", lal_element_hash);

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

        std::string            ident   = getFullName(&lal_identifier);
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
    case ada_protected_type_decl:             // 9.4(2)
      {
        logKind("ada_protected_type_decl", lal_element_hash);

        //Get the name for this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_base_type_decl_f_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

        //Get & hash the previous decl of this type, if it exists
        ada_base_entity lal_previous_decl;
        ada_basic_decl_p_previous_part_for_decl(lal_element, 1, &lal_previous_decl);
        ada_base_type_decl_f_name(&lal_previous_decl, &lal_previous_decl);
        int previous_hash = 0;
        if(!ada_node_is_null(&lal_previous_decl)){
           previous_hash = hash_node(&lal_previous_decl);
        }

        //Get the discriminants for this decl
        ada_base_entity lal_discr;
        ada_protected_type_decl_f_discriminants(lal_element, &lal_discr);

        auto                        spec    = getProtectedSpecForProtectedType(lal_element, ctx);
        int                         hash    = hash_node(&lal_defining_name);
        std::string                 ident   = getFullName(&lal_identifier);
        SgDeclarationStatement*     incomp  = findFirst(libadalangTypes(), previous_hash);
        SgAdaProtectedTypeDecl*     nondef  = isSgAdaProtectedTypeDecl(incomp);
        SgAdaDiscriminatedTypeDecl* discr   = createDiscriminatedDecl_opt(&lal_discr, 0, ctx);
        SgScopeStatement*           parentScope = &ctx.scope();

        if(discr){
          parentScope = discr->get_discriminantScope();
        }

        SgAdaProtectedTypeDecl& sgdecl  = nondef ? mkAdaProtectedTypeDecl(*nondef, SG_DEREF(spec.first), *parentScope)
                                                 : mkAdaProtectedTypeDecl(ident, spec.first,  *parentScope);

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

        break;
      }
    case ada_task_type_decl:                  // 9.1(2)
      {
        logKind("ada_task_type_decl", lal_element_hash);

        auto                        spec    = getTaskSpec_opt(lal_element, ctx);

        //Get the name
        ada_base_entity defining_name;
        ada_base_type_decl_f_name(lal_element, &defining_name);
        ada_base_entity lal_identifier;
        ada_defining_name_f_name(&defining_name, &lal_identifier);

        //Get the discriminants
        ada_base_entity lal_discr;
        ada_task_type_decl_f_discriminants(lal_element, &lal_discr);

        std::string                 ident  = getFullName(&lal_identifier);

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

         break;
      }
    case ada_single_protected_decl:           // 3.3.1(2):9.4(2)
      {
        logKind("ada_single_protected_decl", lal_element_hash);

        //Get the name of this decl
        ada_base_entity lal_defining_name, lal_identifier;
        ada_single_protected_decl_f_name(lal_element, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
        std::string ident = getFullName(&lal_identifier);

        auto spec = getProtectedSpecForSingleProtected(lal_element, ctx);
        SgAdaProtectedSpecDecl& sgnode = mkAdaProtectedSpecDecl(ident, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);
        int hash = hash_node(&lal_defining_name);
        recordNode(libadalangDecls(), hash, sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_single_task_decl:                // 3.3.1(2):9.1(3)
      {
        logKind("ada_single_task_decl", lal_element_hash);

        //Get the name for this task
        ada_base_entity lal_defining_name, lal_identifier;
        ada_single_task_decl_f_task_type(lal_element, &lal_defining_name);
        ada_base_type_decl_f_name(&lal_defining_name, &lal_defining_name);
        ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

        std::string        ident   = getFullName(&lal_identifier);
        int                hash    = hash_node(&lal_defining_name);
        auto               spec    = getTaskSpecForSingleTask(lal_element, ctx);
        SgAdaTaskSpecDecl& sgnode  = mkAdaTaskSpecDecl(ident, SG_DEREF(spec.first), ctx.scope());

        attachSourceLocation(sgnode, lal_element, ctx);
        privatize(sgnode, isPrivate);
        ctx.appendStatement(sgnode);

        //~ recordNode(libadalangTypes(), hash, sgnode);
        recordNode(libadalangDecls(), hash, sgnode);

        assocdecl = &sgnode;
        break;
      }
    case ada_entry_decl:                     // 9.5.2(2)
      {
        logKind("ada_entry_decl", lal_element_hash);

        //Get the name
        ada_base_entity entry_spec;
        ada_entry_decl_f_spec(lal_element, &entry_spec);
        ada_base_entity defining_name;
        ada_entry_spec_f_entry_name(&entry_spec, &defining_name);
        ada_base_entity lal_identifier;
        ada_defining_name_f_name(&defining_name, &lal_identifier);

        std::string     ident   = getFullName(&lal_identifier);
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
        logKind("ada_object_decl", lal_element_hash);

        //Get the renaming clause, if it exists
        ada_base_entity lal_renaming_clause;
        ada_object_decl_f_renaming_clause(lal_element, &lal_renaming_clause);

        if(!ada_node_is_null(&lal_renaming_clause)){
          //Get the renamed object
          ada_base_entity lal_renamed_identifier;
          ada_renaming_clause_f_renamed_object(&lal_renaming_clause, &lal_renamed_identifier);
          SgExpression& renamed = getExpr(&lal_renamed_identifier, ctx);

          //Get the name of this object
          ada_base_entity lal_defining_name, lal_identifier;
          ada_object_decl_f_ids(lal_element, &lal_defining_name);
          ada_node_child(&lal_defining_name, 0, &lal_defining_name); //renamings can't have identifier lists, so get the single name
          int decl_hash = hash_node(&lal_defining_name);
          ada_defining_name_f_name(&lal_defining_name, &lal_identifier);
          std::string name = getFullName(&lal_identifier);

          //Get the type
          ada_base_entity lal_subtype_indication;
          ada_object_decl_f_type_expr(lal_element, &lal_subtype_indication);
          SgType& ty = getDeclType(&lal_subtype_indication, ctx);

          SgScopeStatement& scope = ctx.scope();

          SgAdaRenamingDecl& sgnode = mkAdaRenamingDecl(name, renamed, ty, scope);

          recordNode(libadalangDecls(), hash_node(lal_element), sgnode);
          recordNode(libadalangDecls(), decl_hash, sgnode);

          attachSourceLocation(sgnode, lal_element, ctx);
          privatize(sgnode, isPrivate);
          ctx.appendStatement(sgnode);
          assocdecl = &sgnode;
          
        } else {
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
        }

        break;
      }
    case ada_incomplete_type_decl:           // 3.2.1(2):3.10(2)
    case ada_incomplete_tagged_type_decl:     //  3.10.1(2)
      {
        logKind( kind == ada_incomplete_type_decl
                        ? "ada_incomplete_type_decl"
                        : "ada_incomplete_tagged_type_decl"
               , lal_element_hash
               );

        //Get the full definition of this type from later on, if it exists
        ada_base_entity lal_full_decl;
        ada_base_type_decl_p_full_view(lal_element, &lal_full_decl);

        if(!ada_node_is_null(&lal_full_decl)){
          // \todo handle pragmas in opaque types
          assocdecl = &handleOpaqueTypes(lal_element, isPrivate, ctx);
        }
        else
        {
          // no definition is available, ... (e.g., in System)
          //Get the name for this type
          ada_base_entity lal_identifier, lal_defining_name;
          ada_base_type_decl_f_name(lal_element, &lal_defining_name);
          ada_defining_name_f_name(&lal_defining_name, &lal_identifier);

          std::string             ident  = getFullName(&lal_identifier);
          SgScopeStatement&       scope  = ctx.scope();
          SgType&                 opaque = mkOpaqueType();
          SgDeclarationStatement& sgnode = mkTypeDecl(ident, opaque, scope);
          int                     hash   = hash_node(&lal_defining_name);

          attachSourceLocation(sgnode, lal_element, ctx);
          privatize(sgnode, isPrivate);
          ctx.appendStatement(sgnode);
          recordNode(libadalangTypes(), hash, sgnode);
          assocdecl = &sgnode;
        }

        break;
      }
    case ada_type_decl:            //3.2.1(3)
      {
        logKind("ada_type_decl", lal_element_hash);
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
        std::string type_name = getFullName(&lal_identifier);
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

  processAspects(lal_element, assocdecl, ctx);
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

//Searches the adaTypes map for a match to the hashes given, and returns the decl of the match if found
SgDeclarationStatement* typeDeclarationFromStandard(int hash1, int hash2){
  SgType*       res = findFirst(adaTypes(), hash1, hash2);

  if (SgNamedType* namedType = isSgNamedType(res)){
    return namedType->get_declaration();
  }

  return nullptr;
}

[[noreturn]]
bool useClauseFatalError(std::string name, AstContext ctx){
  logFatal() << "using unknown package/type: "
    << "'" << name << "': "
    << "\n   in scope type: " << typeid(ctx.scope()).name()
    << std::endl;

  sg::report_error("using unknown package/type");
}

/// Creates a decl for a single package or type name in a use clause
void createUseClause(ada_base_entity* lal_element, map_t<int, SgDeclarationStatement*>& m, AstContext ctx){
 //Get the rightmost node for this name (get the suffix until we don't have ada_dotted_name)
  ada_base_entity lal_name;
  ada_name_p_relative_name(lal_element, &lal_name);

  SgDeclarationStatement*      used = nullptr;
  std::string             full_name = getFullName(lal_element); //TODO This might break for some ada_use_type_clauses

  // Get the hash for the first declaration, and for the defining name of that declaration, if it exists
  ada_base_entity lal_first_decl;
  int decl_hash;
  int decl_name_hash;
  ada_expr_p_first_corresponding_decl(&lal_name, &lal_first_decl);
  if(ada_node_is_null(&lal_first_decl)){
    //Try p_referenced_decl
    ada_name_p_referenced_decl(&lal_name, 1, &lal_first_decl);
  }
  if(ada_node_is_null(&lal_first_decl)){
    decl_hash = 0; //Make sure decl_hash is undefined, even if we don't have a node
    decl_name_hash = 0;
    logFlaw() << "Could not find first decl for use clause!\n";
  } else {
    decl_hash = hash_node(&lal_first_decl);
    //Now get the defining_name node and hash it
    ada_base_entity lal_first_defining_name;
    //Get the kind of the decl
    ada_node_kind_enum decl_kind = ada_node_kind(&lal_first_decl);
    if(decl_kind >= ada_generic_package_internal && decl_kind <= ada_package_decl){
      ada_base_package_decl_f_package_name(&lal_first_decl, &lal_first_defining_name);
      decl_name_hash = hash_node(&lal_first_defining_name);
    } else if(decl_kind >= ada_discrete_base_subtype_decl && decl_kind <= ada_synth_anonymous_type_decl){
      ada_base_type_decl_f_name(&lal_first_decl, &lal_first_defining_name);
      decl_name_hash = hash_node(&lal_first_defining_name);
    } else if(decl_kind == ada_package_renaming_decl){
      ada_package_renaming_decl_f_name(&lal_first_decl, &lal_first_defining_name);
      decl_name_hash = hash_node(&lal_first_defining_name);
    } else if(decl_kind == ada_generic_package_renaming_decl){
      ada_generic_package_renaming_decl_f_name(&lal_first_decl, &lal_first_defining_name);
      decl_name_hash = hash_node(&lal_first_defining_name);
    } else if(decl_kind == ada_generic_package_instantiation){
      ada_generic_package_instantiation_f_name(&lal_first_decl, &lal_first_defining_name);
      decl_name_hash = hash_node(&lal_first_defining_name);
    } else {
      logWarn() << "Could not find defining_name for decl kind " << decl_kind << " in createUseClause()\n";
      decl_name_hash = 0;
    }
  }

  (used != nullptr)
  || (used = findFirst(m, decl_hash, decl_name_hash))
  //|| (used = fromSymbolLookup(usedEl)) //TODO This searches based off name?
  || (used = findFirst(adaPkgs(), decl_hash, decl_name_hash))
  || (used = typeDeclarationFromStandard(decl_hash, decl_name_hash))
  || (useClauseFatalError(full_name, ctx))
  ;

  SgUsingDeclarationStatement& sgnode  = mkUseClause(*used);
  std::size_t                  attrPos = full_name.find("'");

  if(attrPos != std::string::npos){
    // \todo introduce proper flag
    sgnode.set_adaTypeAttribute(full_name.substr(attrPos+1));
  }

  int hash = hash_node(lal_element);
  recordNode(libadalangDecls(), hash, sgnode);
  attachSourceLocation(sgnode, lal_element, ctx);
  ctx.appendStatement(sgnode);
}

/// Creates an SgExpression node for a single name in a with clause
SgExpression& createWithClause(ada_base_entity* lal_element, AstContext ctx){
  //Get the rightmost node for this name (get the suffix until we don't have ada_dotted_name)
  ada_base_entity lal_name;
  ada_name_p_relative_name(lal_element, &lal_name);

  ada_node_kind_enum kind = ada_node_kind(lal_element);
  while(kind == ada_dotted_name){
    ada_dotted_name_f_suffix(&lal_name, &lal_name);
    kind = ada_node_kind(&lal_name);
  }

  SgExpression& sgnode = getExpr(&lal_name, ctx);

  // store source location of the fully qualified name
  attachSourceLocation(sgnode, lal_element, ctx);

  return sgnode;
}

/// Creates an assign op for an enum value
SgAssignOp& itemValuePair(ada_base_entity* lal_element, SgExpression& enumval, AstContext ctx)
{
  ada_base_entity lal_designator;
  ada_node_child(lal_element, 0, &lal_designator);
  SgExpression&  enumitem = getExpr(&lal_designator, ctx);

  return SG_DEREF(sb::buildAssignOp(&enumitem, &enumval));
}

/// Used for ada_enum_rep_clause nodes
/// Creates an assoc of 0 or 1 enum values to a constant
SgExpression& createEnumValue(ada_base_entity* lal_element, AstContext ctx){
  ada_node_kind_enum kind = ada_node_kind(lal_element);
  if(kind != ada_aggregate_assoc){
    logError() << "createEnumValue given node kind " << kind << " (not an ada_aggregate_assoc)!\n";
    return mkNullExpression();
  }
  logKind("ada_aggregate_assoc", hash_node(lal_element));

  //Get the expr for this assoc
  ada_base_entity lal_expr;
  ada_aggregate_assoc_f_r_expr(lal_element, &lal_expr);

  //Get the list of designators for this assoc
  ada_base_entity lal_designator_list;
  ada_aggregate_assoc_f_designators(lal_element, &lal_designator_list);

  int count = ada_node_children_count(&lal_designator_list);
  //If there is more than one designator, something is wrong
  if(count > 1 || count < 0){
    logError() << "createEnumValue has " << count << " designators! Expected 0 or 1.\n";
    return mkNullExpression();
  }

  SgExpression&              enumval = getExpr(&lal_expr, ctx);
  SgExpression&              sgnode = count == 0
                                    ? enumval
                                    : itemValuePair(&lal_designator_list, enumval, ctx);
                                    ;
  attachSourceLocation(sgnode, lal_element, ctx);
  return sgnode;
}

/// Handles an ada_component_clause node
void createComponentClause(ada_base_entity* lal_element, AstContext ctx){
  ada_node_kind_enum kind = ada_node_kind(lal_element);
  if(kind != ada_component_clause){
    logError() << "createComponentClause given node kind " << kind << " (not an ada_component_clause)!\n";
    return;
  }

  logKind("ada_component_clause", hash_node(lal_element));

  //Get the name for this clause
  ada_base_entity lal_name;
  ada_component_clause_f_id(lal_element, &lal_name);

  //Get the position for this clause
  ada_base_entity lal_position;
  ada_component_clause_f_position(lal_element, &lal_position);

  //Get the range for this clause
  ada_base_entity lal_range;
  ada_component_clause_f_range(lal_element, &lal_range);
  ada_range_spec_f_range(&lal_range, &lal_range);

  // \todo use getQualName?
  SgExpression&         field  = getExpr(&lal_name, ctx);
  SgVarRefExp&          fldref = SG_DEREF(isSgVarRefExp(&field));
  SgExpression&         ofs    = getExpr(&lal_position, ctx);
  SgExpression&         rngexp = getDiscreteRange(&lal_range, ctx);
  SgRangeExp*           range  = isSgRangeExp(&rngexp);
  SgAdaComponentClause& sgnode = mkAdaComponentClause(fldref, ofs, SG_DEREF(range));

  //~ recordNode(asisDecls(), el.ID, sgnode);
  attachSourceLocation(sgnode, lal_element, ctx);

  ctx.appendStatement(sgnode);
}

/// Handles a lal node that is a clause
/// ada_component_clause is the exception, it is handled in createComponentClause
void handleClause(ada_base_entity* lal_element, AstContext ctx)
{
  //Get the kind of this node
  ada_node_kind_enum kind;
  kind = ada_node_kind(lal_element);

  LibadalangText kind_name(kind);
  std::string kind_name_string = kind_name.string_value();
  logTrace()   << "handleClause called on a " << kind_name_string << std::endl;

  //Get the hash
  int lal_element_hash = hash_node(lal_element);

  switch(kind){
    case ada_with_clause:                // 10.1.2
      {
        logKind("ada_with_clause", lal_element_hash);

        //Get the list of names for this with clause
        ada_base_entity lal_name_list;
        ada_with_clause_f_packages(lal_element, &lal_name_list);

        //Call createWithClause on each name
        SgExpressionPtrList implst;
        int count = ada_node_children_count(&lal_name_list);
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_with_name;
          if(ada_node_child(&lal_name_list, i, &lal_with_name) != 0){
            implst.push_back(&createWithClause(&lal_with_name, ctx));
          }
        }

        SgImportStatement&  sgnode = mkWithClause(std::move(implst));
        int                   hash = hash_node(lal_element); //There isn't a defining name, so just use the node itself

        attachSourceLocation(sgnode, lal_element, ctx);
        recordNode(libadalangDecls(), hash, sgnode);
        ctx.appendStatement(sgnode);

        //~ recordNonUniqueNode(libadalangDecls(), ???, sgnode);

        break;
      }

    case ada_use_type_clause:            // 8.4
    case ada_use_package_clause:         // 8.4
      {
        const bool typeClause = (kind == ada_use_type_clause);

        logKind(typeClause ? "ada_use_type_clause" : "ada_use_package_clause", lal_element_hash);

        //Get the list of names for this clause
        ada_base_entity lal_name_list;
        if(kind == ada_use_type_clause){
          ada_use_type_clause_f_types(lal_element, &lal_name_list);
        } else {
          ada_use_package_clause_f_packages(lal_element, &lal_name_list);
        }

        //Call createUseClause on each node in the list
        int count = ada_node_children_count(&lal_name_list);
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_use_name;
          if(ada_node_child(&lal_name_list, i, &lal_use_name) != 0){
            createUseClause(&lal_use_name, typeClause ? libadalangTypes() : libadalangDecls(), ctx);
          }
        }

        break;
      }
    case ada_record_rep_clause:           // 13.5.1
      {
        using SageRecordClause = SgAdaRepresentationClause;

        logKind("ada_record_rep_clause", lal_element_hash);

        //Get the name of this clause
        ada_base_entity lal_name;
        ada_record_rep_clause_f_name(lal_element, &lal_name);

        //Get the mod of this clause
        ada_base_entity lal_mod;
        ada_record_rep_clause_f_at_expr(lal_element, &lal_mod);

        //Get the list of components for this clause
        ada_base_entity lal_component_list;
        ada_record_rep_clause_f_components(lal_element, &lal_component_list);

        SgType&                 tyrec      = getDeclType(&lal_name, ctx);
        SgExpression&           modexp     = getExpr_opt(&lal_mod, ctx);
        SageRecordClause&       sgnode     = mkAdaRepresentationClause(tyrec, modexp);
        SgBasicBlock&           components = SG_DEREF(sgnode.get_components());


        // sgnode is not a decl: recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        //Traverse the components
        int count = ada_node_children_count(&lal_component_list);
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_component_clause;
          if(ada_node_child(&lal_component_list, i, &lal_component_clause) != 0){
            createComponentClause(&lal_component_clause, ctx.scope(components));
          }
        }

        break;
      }
    case ada_at_clause:                             // J.7
      {
        using SageRecordClause = SgAdaRepresentationClause;

        logKind("ada_at_clause", lal_element_hash);

        //Get the name for this clause
        ada_base_entity lal_name;
        ada_at_clause_f_name(lal_element, &lal_name);

        //Get the expr for this clause
        ada_base_entity lal_expr;
        ada_at_clause_f_expr(lal_element, &lal_expr);

        // \todo Representation_Clause_Name may not refer to a type but to a variable
        //       (e.g.,rep_sys_address.adb)
        //       consider using expressions as base for AdaRepresentationClause...
        SgType&           ty     = getDeclType(&lal_name, ctx);
        SgExpression&     modexp = getExpr_opt(&lal_expr, ctx);
        SageRecordClause& sgnode = mkAdaRepresentationClause(ty, modexp, true /* at-clause */);

        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        break;
      }
    case ada_attribute_def_clause:           // 13.3
      {
        logKind("ada_attribute_def_clause", lal_element_hash);

        //Get the attribute & expr for this clause
        ada_base_entity lal_attribute, lal_expr;
        ada_attribute_def_clause_f_attribute_expr(lal_element, &lal_attribute);
        ada_attribute_def_clause_f_expr(lal_element, &lal_expr);

        SgAdaAttributeExp&    lenattr = getAttributeExpr(&lal_attribute, ctx);
        SgExpression&         lenexpr = getExpr(&lal_expr, ctx);
        SgAdaAttributeClause& sgnode  = mkAdaAttributeClause(lenattr, lenexpr);

        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        break;
      }
    case ada_enum_rep_clause:     // 13.4
      {
        logKind("ada_enum_rep_clause", lal_element_hash);

        //Get the name for this clause
        ada_base_entity lal_name;
        ada_enum_rep_clause_f_type_name(lal_element, &lal_name);

        //Get the list of associations
        ada_base_entity lal_assoc_list;
        ada_enum_rep_clause_f_aggregate(lal_element, &lal_assoc_list);
        ada_base_aggregate_f_assocs(&lal_assoc_list, &lal_assoc_list);
        std::vector<SgExpression*> assocs;
        int count = ada_node_children_count(&lal_assoc_list);
        for(int i = 0; i < count; ++i){
          ada_base_entity lal_assoc;
          if(ada_node_child(&lal_assoc_list, i, &lal_assoc) != 0){
            assocs.push_back(&createEnumValue(&lal_assoc, ctx));
          }
        }

        SgType&                 enumty   = getDeclType(&lal_name, ctx);
        SgExprListExp&          enumvals = mkExprListExp(assocs);
        SgAdaEnumRepresentationClause& sgnode = mkAdaEnumRepresentationClause(enumty, enumvals);

        attachSourceLocation(sgnode, lal_element, ctx);
        ctx.appendStatement(sgnode);

        break;
      }
    default:
      logWarn() << "unhandled clause kind: " << kind << std::endl;
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
  logKind("A_Definition", hash_node(lal_element));

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

//LAL_REP_ISSUE: Everything to do with inherited subps. LAL does not have Implicit_Inherited_Subprograms,
// so we have to manually locate any subps that used the base type. But, this system currently does not
// account for multiple levels of derivation, each having its own subps. Currently, we only search for
// subps that use the base type, not any middle types.
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

         //Make sure that lal_super_type isn't a subtype
         ada_node_kind_enum lal_super_type_kind = ada_node_kind(&lal_super_type);
         while(lal_super_type_kind == ada_subtype_decl){
           //Get the type this subtype is derived from
           ada_subtype_decl_f_subtype(&lal_super_type, &lal_super_type);
           ada_type_expr_p_designated_type_decl(&lal_super_type, &lal_super_type);
           lal_super_type_kind = ada_node_kind(&lal_super_type);
         }

         //  Get the programs that use the original type
         //Get the name of the type
         ada_text_type lal_unique_identifying_name;
         ada_basic_decl_p_unique_identifying_name(&lal_super_type, &lal_unique_identifying_name);
         std::string unique_identifying_name = dot_ada_text_type_to_string(lal_unique_identifying_name);

         if(baseRootType->get_qualified_name().getString() != unique_identifying_name){
           //In some cases, the baseRootType will be set to an intermediate derived type instead of the true base type.
           // Since this search only looks for subps of the true base type, keeping this incorrect baseRootType will result in
           // the subps being inherited unmodified (b/c we attempt to replace the nonexistent baseRootType with derivedType)
           // To fix this, try to find the correct baseRootType using lal_super_type
           logWarn() << "baseRootType does not match lal_super_type\n";
           ada_base_entity lal_defining_name;
           ada_base_type_decl_f_name(&lal_super_type, &lal_defining_name);
           int name_hash = hash_node(&lal_defining_name);
           int decl_hash = hash_node(&lal_super_type);
           SgClassDeclaration* baseTypeDecl = isSgClassDeclaration(findFirst(libadalangTypes(), name_hash, decl_hash));
           if(baseTypeDecl != nullptr){
             SgNamedType* baseTypeAgain = isSgNamedType(baseTypeDecl->get_type());
             if(baseTypeAgain != nullptr){
                 logInfo() << "Fixed baseRootType\n";
                 baseRootType = baseTypeAgain;
             }
           }
         }

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

namespace {

  /// retrieves the scope of a declaration node
  struct ScopeQuery : sg::DispatchHandler<SgScopeStatement*>
  {
    template <class SageDecl>
    void def(SageDecl& n) { res = n.get_definition(); }

    template <class SageDecl>
    void def(SageDecl* n) { if (n) def(*n); }

    void handle(SgNode& n)                   { SG_UNEXPECTED_NODE(n); }

    // declarations
    void handle(SgAdaPackageSpecDecl& n)     { def(n); }
    void handle(SgAdaPackageBodyDecl& n)     { def(n); }
    void handle(SgFunctionDeclaration& n)    { def(isSgFunctionDeclaration(n.get_definingDeclaration())); }

    void handle(SgAdaGenericDecl& n)
    {
      //~ SgDeclarationStatement* dcl = n.get_declaration();

      //~ if (isSgFunctionDeclaration(dcl)) // \todo correct?
        //~ return def(n);

      //~ res = find(dcl);
      res = find(n.get_declaration());
    }

    void handle(SgAdaGenericInstanceDecl& n)
    {
      SgBasicBlock&       scope   = SG_DEREF(isSgBasicBlock(n.get_instantiatedScope()));
      SgStatementPtrList& stmts   = scope.get_statements();
      SgStatement*        dclstmt = stmts.at(0);

      res = find(dclstmt);
    }

    // renamings
    void handle(SgAdaRenamingDecl& n) { res = find(n.get_renamed()); }
    void handle(SgAdaUnitRefExp& n)   { res = find(n.get_decl()); }

    // others
    void handle(SgBasicBlock& n)      { res = &n; }

    // \todo add handlers as needed
    // ...

    static
    ReturnType find(SgNode*);
  };

  ScopeQuery::ReturnType
  ScopeQuery::find(SgNode* n)
  {
    return sg::dispatch(ScopeQuery{}, n);
  }

} // end anonymous namespace

  //Find the scope specified by a given ada_defining_name node.
  //  If the ada_defining_name_f_name node is ada_dotted_name, find the node for the parent and use that.
  //  E.g., If the defining name is a.b.c, return the scope of a.b
  //  Otherwise, return ctx.scope().
  SgScopeStatement* determineParentScope(ada_base_entity* lal_element, AstContext ctx)
  {
    //Check that this is an ada_defining_name node
    ASSERT_require(ada_node_kind(lal_element) == ada_defining_name);

    //Get the f_name, and check its type
    ada_base_entity lal_name;
    ada_defining_name_f_name(lal_element, &lal_name);
    ada_node_kind_enum lal_name_kind = ada_node_kind(&lal_name);

    if(lal_name_kind == ada_dotted_name){
      //The suffix points to the name of the variable, which we don't want.
      //  We instead want the rightmost name that is not the suffix.
      //  So, take the prefix, then take the suffix if the prefix is ada_dotted_name
      //  ada_dotted_name trees work as below, where o = ada_dotted_name
      //       o
      //      / \
      //     o   d
      //    / \
      //   o   c
      //  / \
      // a   b
      ada_base_entity lal_rightmost_prefix;
      ada_dotted_name_f_prefix(&lal_name, &lal_rightmost_prefix);
      if(ada_node_kind(&lal_rightmost_prefix) == ada_dotted_name){
        ada_dotted_name_f_suffix(&lal_rightmost_prefix, &lal_rightmost_prefix);
      }
      //Now, call queryCorrespondingAstNode on the prefix
      SgNode* scope_decl = queryCorrespondingAstNode(&lal_rightmost_prefix);
      if(scope_decl == nullptr)
      {
        logFatal() << "Unable to find scope/declaration for " << getFullName(&lal_name)
                   << std::endl;
        ASSERT_require(false);
      }
      return ScopeQuery::find(scope_decl);
    } else {
      return &ctx.scope();
    }
  }

  /// returns the ROSE scope of an already converted Asis element \ref elem.
  SgScopeStatement&
  queryScopeOf(int declHash, AstContext)
  {
    if (SgDeclarationStatement* dcl = findFirst(libadalangDecls(), declHash))
      return SG_DEREF(ScopeQuery::find(dcl));

/*
    ADA_ASSERT (elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;

    if (expr.Expression_Kind == A_Selected_Component)
      return queryScopeOfID(expr.Selector, ctx);

    if (expr.Expression_Kind != An_Identifier)
      logError() << "unexpected identifier: " << expr.Expression_Kind;

    ADA_ASSERT (expr.Expression_Kind == An_Identifier);
    logKind("An_Identifier", elem.ID);

    SgNode* dcl = queryCorrespondingAstNode(expr, ctx);

    if (dcl == nullptr)
    {
      logFatal() << "Unable to find scope/declaration for " << expr.Name_Image
                 << std::endl;
      ADA_ASSERT(false);
    }

    return SG_DEREF(ScopeQuery::find(dcl));
*/
    logError() << "unable to retrive scope for node. hash = " << declHash
               << "\n  falling back to Standard package."
               << std::endl;

    return *si::Ada::pkgStandardScope(); // that IS WRONG
  }


} //end Libadalang_ROSE_Translation
