/* unparseJovial_statements.C
 * Contains functions that unparse statements
 *
 * FORMATTING WILL BE DONE IN TWO WAYS:
 * 1. using the file_info object to get information from line and column number 
 *    (for original source code)
 * 2. following a specified format that I have specified with indentations of
 *    length TABINDENT (for transformations)
 * 
 * REMEMBER: For types and symbols, we still call the original unparse function
 * defined in sage since they dont have file_info. For expressions,
 * UnparseJovial::unparse is called, and for statements,
 * UnparseJovial::unparseStatement is called.
 *
 */
#include "sage3basic.h"
#include "unparser.h"
#include "sage_support.h"

UnparseJovial::UnparseJovial(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
   }

UnparseJovial::~UnparseJovial()
   {
   }

void 
UnparseJovial::unparseJovialFile(SgSourceFile *sourcefile, SgUnparse_Info& info)
{
  SgGlobal* globalScope = sourcefile->get_globalScope();
  ASSERT_not_null(globalScope);

  // Comments are really important in language translation. Turn them off in the base class
  // so that care can be used to unparse and check comments in Jovial.
  info.set_SkipComments();

  // unparse comments preceding the global scope
  auto preprocInfo = globalScope->get_attachedPreprocessingInfoPtr();
  if (preprocInfo) {
    for (PreprocessingInfo* info : *preprocInfo) {
      if (info->getRelativePosition() == PreprocessingInfo::before) {
        curprint(info->getString());
        curprint("\n");
      }
    }
  }

  curprint("START\n");
  unparseStatement(globalScope, info);

  // unparse comments near end of global scope
  preprocInfo = globalScope->get_attachedPreprocessingInfoPtr();
  if (preprocInfo) {
    for (PreprocessingInfo* info : *preprocInfo) {
      if (info->getRelativePosition() == PreprocessingInfo::after) {
        curprint(info->getString());
        curprint("\n");
      }
    }
  }

  curprint("TERM\n");
}


void
UnparseJovial::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific statements not handled by the base class unparseStatement() member function

     ASSERT_not_null(stmt);

     // unparse comments preceding the statement
     unparseCommentsBefore(stmt, info);

     switch (stmt->variantT())
        {
       // module support
          case V_SgJovialCompoolStatement:     unparseCompoolStmt (stmt, info);     break;
          case V_SgProgramHeaderStatement:     unparseProgHdrStmt (stmt, info);     break;
          case V_SgProcedureHeaderStatement:   unparseProcDeclStmt(stmt, info);     break;
          case V_SgFunctionDefinition:         unparseFuncDefnStmt(stmt, info);     break;

          case V_SgNamespaceDeclarationStatement: unparseNamespaceDeclarationStatement(stmt, info);  break;
          case V_SgNamespaceDefinitionStatement:  unparseNamespaceDefinitionStatement (stmt, info);  break;

       // directives, define

          case V_SgJovialDirectiveStatement:   unparseDirectiveStmt (stmt, info);   break;
          case V_SgJovialDefineDeclaration:    unparseDefineDeclStmt(stmt, info);   break;

       // declarations

          case V_SgEmptyDeclaration:           /* let's ignore it (or print ';') */   break;
          case V_SgEnumDeclaration:            unparseEnumDeclStmt   (stmt, info);    break;
          case V_SgJovialOverlayDeclaration:   unparseOverlayDeclStmt(stmt, info);    break;
          case V_SgJovialTableStatement:       unparseTableDeclStmt  (stmt, info);    break;
          case V_SgVariableDeclaration:        unparseVarDeclStmt    (stmt, info);    break;

       // executable statements, control flow
          case V_SgBasicBlock:                 unparseBasicBlockStmt (stmt, info);  break;
          case V_SgLabelStatement:             unparseLabelStmt      (stmt, info);  break;
          case V_SgJovialLabelDeclaration:     unparseJovialLabelDecl(stmt, info);  break;
          case V_SgForStatement:               unparseForStatement   (stmt, info);  break;
          case V_SgJovialForThenStatement:     unparseJovialForThenStmt(stmt, info);  break;
          case V_SgWhileStmt:                  unparseWhileStmt      (stmt, info);  break;
          case V_SgGotoStatement:              unparseGotoStmt(isSgGotoStatement(stmt), info);  break;
          case V_SgIfStmt:                     unparseIfStmt         (stmt, info);  break;
          case V_SgSwitchStatement:            unparseSwitchStmt     (stmt, info);  break;
          case V_SgCaseOptionStmt:             unparseCaseStmt       (stmt, info);  break;
          case V_SgDefaultOptionStmt:          unparseDefaultStmt    (stmt, info);  break;
          case V_SgBreakStmt:                  unparseBreakStmt      (stmt, info);  break;
          case V_SgTypedefDeclaration:         unparseTypeDefStmt    (stmt, info);  break;

          case V_SgProcessControlStatement:    unparseProcessControlStmt(stmt, info);  break;
          case V_SgReturnStmt:                 unparseReturnStmt     (stmt, info);  break;

          case V_SgExprStatement:              unparseExprStmt       (stmt, info);  break;

          default: {
             cerr << "UnparseJovial::unparseLanguageSpecificStatement: Error: No handler for "
                  <<  stmt->class_name() << ", variant: " << stmt->variantT() << endl;
             ROSE_ABORT();
             break;
          }
        }

     // unparse comments at end of the statement
     unparseCommentsAfter(stmt, info, /*newline*/true);
   }

//----------------------------------------------------------------------------
//  UnparseJovial::DIRECTIVES and DEFINE
//----------------------------------------------------------------------------

void
UnparseJovial::unparseDirectiveStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgJovialDirectiveStatement* directive = isSgJovialDirectiveStatement(stmt);
     ASSERT_not_null(directive);

     std::string content = directive->get_content_string();

     switch (directive->get_directive_type())
        {
        case SgJovialDirectiveStatement::e_compool:
           {
              curprint_indented("!COMPOOL ", info);
              curprint(content);
              curprint(";\n");
              break;
           }
        case SgJovialDirectiveStatement::e_skip:
           {
              curprint_indented("!SKIP", info);
              if (!content.empty()) {
                 curprint(" ");
                 curprint(content);
              }
              curprint(";\n");
              break;
           }
        case SgJovialDirectiveStatement::e_begin:
           {
              curprint_indented("!BEGIN", info);
              if (!content.empty()) {
                 curprint(" ");
                 curprint(content);
              }
              curprint(";\n");
              break;
           }
        case SgJovialDirectiveStatement::e_end:
           {
              curprint_indented("!END;\n", info);
              break;
           }
        case SgJovialDirectiveStatement::e_linkage:
           {
              curprint_indented("!LINKAGE", info);
              if (!content.empty()) {
                 curprint(" ");
                 curprint(content);
              }
              curprint(";\n");
              break;
           }
        case SgJovialDirectiveStatement::e_reducible:
           {
              curprint_indented("!REDUCIBLE;\n", info);
              break;
           }
        case SgJovialDirectiveStatement::e_nolist:
           {
              curprint_indented("!NOLIST;\n", info);
              break;
           }
        case SgJovialDirectiveStatement::e_list:
           {
              curprint_indented("!LIST;\n", info);
              break;
           }
        case SgJovialDirectiveStatement::e_eject:
           {
              curprint_indented("!EJECT;\n", info);
              break;
           }
        case SgJovialDirectiveStatement::e_initialize:
           {
              curprint_indented("!INITIALIZE;\n", info);
              break;
           }
        case SgJovialDirectiveStatement::e_order:
           {
              curprint_indented("!ORDER;\n", info);
              break;
           }
        case SgJovialDirectiveStatement::e_align:
           {
              curprint_indented("!ALIGN ", info);
              curprint(content);
              curprint(";\n");
              break;
           }
        case SgJovialDirectiveStatement::e_always:
           {
              curprint_indented("!ALWAYS'STORE ", info);
              curprint(content);
              curprint(";\n");
              break;
           }
        default:
           {
              cout << "Warning: SgJovialDirectiveStmt directive type not handled is "
                   << directive->get_directive_type() << endl;
           }
        }
   }

void
UnparseJovial::unparseDefineDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgJovialDefineDeclaration* define = isSgJovialDefineDeclaration(stmt);
     ASSERT_not_null(define);

     curprint_indented("DEFINE ", info);
     curprint(define->get_define_string());
     curprint(";\n");
   }

//----------------------------------------------------------------------------
//  UnparseJovial::MODULES
//----------------------------------------------------------------------------

void 
UnparseJovial::unparseCompoolStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgJovialCompoolStatement* compool = isSgJovialCompoolStatement(stmt);
     ASSERT_not_null(compool);

     curprint("COMPOOL ");
     curprint(compool->get_name());
     curprint(";\n");
   }

void
UnparseJovial::unparseProgHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgProgramHeaderStatement* prog = isSgProgramHeaderStatement(stmt);
     ASSERT_not_null(prog);

     curprint("PROGRAM ");
     curprint(prog->get_name());
     curprint(";\n");

     unparseStatement(prog->get_definition(), info);
   }

void
UnparseJovial::unparseProcDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);

     SgBasicBlock* func_body{nullptr};
     SgScopeStatement* param_scope{nullptr};

     SgProcedureHeaderStatement* func = isSgProcedureHeaderStatement(stmt);
     ASSERT_not_null(func);

     bool is_defining_decl = (func->get_declarationModifier().isJovialRef() == false);

     SgFunctionDefinition* func_def = func->get_definition();
     if (func_def) {
        param_scope = func_body = func_def->get_body();
        ASSERT_not_null(func_body);
     }
     else {
        param_scope = func->get_functionParameterScope();
     }
     ASSERT_not_null(param_scope);

  // unparse the declaration modifiers
     if (func->get_declarationModifier().isJovialDef())   curprint("DEF ");
     if (func->get_declarationModifier().isJovialRef())   curprint("REF ");

     curprint("PROC ");
     curprint(func->get_name());

  // unparse the function modifiers
     if (func->get_functionModifier().isRecursive())   curprint(" REC");
     if (func->get_functionModifier().isReentrant())   curprint(" RENT");

  // unparse function arguments
     SgFunctionParameterList* params = func->get_parameterList();
     SgInitializedNamePtrList & args = params->get_args();

     const std::vector<SgInitializedName*> in_params = SageInterface::getInParameters(args);
     const std::vector<SgInitializedName*> out_params = SageInterface::getOutParameters(args);

     curprint("(");

     bool print_comma = false;
     for (SgInitializedName* param : in_params) {
        if (print_comma) curprint(",");
        curprint(param->get_name());
        print_comma = true;
     }

     print_comma = false;
     for (SgInitializedName* param : out_params) {
        if (print_comma) curprint(",");
        else             curprint(":");
        curprint(param->get_name());
        print_comma = true;
     }

     curprint(")");

  // unparse function type
     SgType* type = func->get_type();
     unparseType(type, ninfo);

     curprint(";\n");

     if (is_defining_decl) {
        ASSERT_not_null(func->get_definition());

        info.inc_nestingLevel();
        unparseStatement(func->get_definition(), ninfo);
        info.dec_nestingLevel();
     }
     else {
        for (SgDeclarationStatement* decl : param_scope->getDeclarationList()) {
           if (isSgJovialDirectiveStatement(decl)) {
              unparseStatement(decl, ninfo);
           }
        }

    // There still needs to be at least a BEGIN and END
        curprint_indented("BEGIN\n", info);

        info.inc_nestingLevel();
        for (SgInitializedName* arg : args) {
           SgVariableSymbol* var_sym = SageInterface::lookupVariableSymbolInParentScopes(arg->get_name(), param_scope);
           SgInitializedName* var_init_name = var_sym->get_declaration();
           ASSERT_not_null(var_init_name);
           SgVariableDeclaration* var_decl = isSgVariableDeclaration(var_init_name->get_declaration());
           ASSERT_not_null(var_decl);

           unparseVarDeclStmt(var_decl, info);
        }
        info.dec_nestingLevel();

        curprint_indented("END\n", info);
     } // !is_defining_decl

   }

void
UnparseJovial::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionDefinition* funcdef = isSgFunctionDefinition(stmt);
     ASSERT_not_null(funcdef);

  // unparse the body of the function
     if (funcdef->get_body())
        {
          unparseStatement(funcdef->get_body(), info);
        }
   }

void
UnparseJovial::unparseNamespaceDeclarationStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceDeclarationStatement* decl = isSgNamespaceDeclarationStatement(stmt);
     ASSERT_not_null(decl);

     SgNamespaceDefinitionStatement* defn = decl->get_definition();
     ASSERT_not_null(defn);

     unparseNamespaceDefinitionStatement(defn, info);
   }

void
UnparseJovial::unparseNamespaceDefinitionStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceDefinitionStatement* namespace_defn = isSgNamespaceDefinitionStatement(stmt);
     ASSERT_not_null(namespace_defn);

     const SgDeclarationStatementPtrList& declarations = namespace_defn->get_declarations();

     info.inc_nestingLevel();
     for (SgStatement* namespace_stmt : declarations)
        {
           unparseStatement(namespace_stmt, info);
        }
     info.dec_nestingLevel();
   }


//----------------------------------------------------------------------------
//  UnparseJovial::<executable statements, control flow>
//----------------------------------------------------------------------------

void
UnparseJovial::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* block = isSgBasicBlock(stmt);
     ASSERT_not_null(block);
     ASSERT_not_null(block->get_parent());

     SgSwitchStatement* switch_stmt = isSgSwitchStatement(block->get_parent());

     int block_size = block->get_statements().size();

  // allow one declaration to be unparsed without BEGIN and END (except for switch stmts)
     if (block_size > 1 || switch_stmt) {
       curprint_indented("BEGIN\n", info);
     }

     info.inc_nestingLevel();
     for (SgStatement* block_stmt : block->get_statements()) {
       unparseStatement(block_stmt, info);
     }
     info.dec_nestingLevel();

     if (block_size > 1 || switch_stmt) {
       curprint_indented("END", info);
     }
   }

// Jovial label statements similar to C/C++
void UnparseJovial::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  auto labelStmt = isSgLabelStatement(stmt);
  ASSERT_not_null(labelStmt);

  switch (labelStmt->get_label_type())
  {
    case SgLabelStatement::e_jovial_label_decl:
      curprint_indented("LABEL ", info);
      curprint (labelStmt->get_label() + ";\n");
      return;
    case SgLabelStatement::e_jovial_label_def:
      curprint_indented("DEF LABEL ", info);
      curprint (labelStmt->get_label() + ";\n");
      return;
    case SgLabelStatement::e_jovial_label_ref:
      curprint_indented("REF LABEL ", info);
      curprint (labelStmt->get_label() + ";\n");
      return;
    default: ;
  }

  curprint (string(labelStmt->get_label().str()) + ":");
  unp->cur.insert_newline(1);

  if (labelStmt->get_statement() != nullptr) {
    SgStatement* sgStmt = labelStmt->get_statement();
    ASSERT_not_null(sgStmt);
    UnparseLanguageIndependentConstructs::unparseStatement(sgStmt, info);
  }
}

// Jovial statement name declarations allow labels to be (const) variables!
void UnparseJovial::unparseJovialLabelDecl(SgStatement* stmt, SgUnparse_Info& info)
{
  auto labelDecl = isSgJovialLabelDeclaration(stmt);
  ASSERT_not_null(labelDecl);
  ASSERT_require2(labelDecl->get_label_type() != SgJovialLabelDeclaration::e_unknown, "Jovial label declaration type is unknown\n");

  if (labelDecl->get_label_type() == SgJovialLabelDeclaration::e_jovial_label_decl) {
    curprint_indented("LABEL ", info);
  }
  else if (labelDecl->get_label_type() == SgJovialLabelDeclaration::e_jovial_label_def) {
    curprint_indented("DEF LABEL ", info);
  }
  else if (labelDecl->get_label_type() == SgJovialLabelDeclaration::e_jovial_label_ref) {
    curprint_indented("REF LABEL ", info);
  }

  curprint (labelDecl->get_label() + ";");
  unp->cur.insert_newline(1);
}

void
UnparseJovial::unparseForStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // The SgForStatement is used for the Jovial for statements like:
  //
  //     FOR ivar:0 by 1 while ivar<25;
  //
  // This choice was made so that it could be treated like a C for statement.
  // Other forms of the Jovial ForStatement will require different Sage nodes.
  //
     SgForStatement* for_stmt = isSgForStatement(stmt);
     ASSERT_not_null(for_stmt);
     ASSERT_not_null(for_stmt->get_for_init_stmt());
     ASSERT_not_null(for_stmt->get_test());
     ASSERT_not_null(for_stmt->get_increment());
     ASSERT_not_null(for_stmt->get_loop_body());

     curprint("FOR ");

     SgForInitStatement* for_init_stmt = isSgForInitStatement(for_stmt->get_for_init_stmt());
     ASSERT_not_null(for_init_stmt);

     SgStatementPtrList init_list = for_init_stmt->get_init_stmt();
     SgExprStatement* init_stmt = isSgExprStatement(init_list[0]);
     ASSERT_not_null(init_stmt);

     SgAssignOp* init_expr = isSgAssignOp(init_stmt->get_expression());
     ASSERT_not_null(init_expr);

  // variable
     unparseExpression(init_expr->get_lhs_operand_i(), info);

  // initial value
     curprint(":");
     unparseExpression(init_expr->get_rhs_operand_i(), info);

  // increment
     curprint(" BY ");
     unparseExpression(for_stmt->get_increment(), info);

  // while condition
     SgExprStatement* test_stmt = isSgExprStatement(for_stmt->get_test());
     ASSERT_not_null(test_stmt);

     if ( ! isSgNullExpression(test_stmt->get_expression()) )
        {
           curprint(" WHILE ");
           unparseExpression(test_stmt->get_expression(), info);
        }

     curprint(";");
     unp->cur.insert_newline(1);

  // for body
     unparseStatement(for_stmt->get_loop_body(), info);
     unp->cur.insert_newline(1);
   }

void
UnparseJovial::unparseJovialForThenStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // The SgJovialForThenStatement is used for Jovial for statements like:
  //
  //    FOR ivar:0 THEN 3 WHILE ivar<25;
  //
     SgJovialForThenStatement* for_stmt = isSgJovialForThenStatement(stmt);
     ASSERT_not_null(for_stmt);
     ASSERT_not_null(for_stmt->get_initialization());
     ASSERT_not_null(for_stmt->get_while_expression());
     ASSERT_not_null(for_stmt->get_by_or_then_expression());
     ASSERT_not_null(for_stmt->get_loop_body());

     curprint_indented("FOR ", info);

     SgAssignOp* init_expr = isSgAssignOp(for_stmt->get_initialization());
     ASSERT_not_null(init_expr);

  // variable
     unparseExpression(init_expr->get_lhs_operand_i(), info);

  // initial value
     curprint(":");
     unparseExpression(init_expr->get_rhs_operand_i(), info);

     switch (for_stmt->get_loop_statement_type())
        {
        case SgJovialForThenStatement::e_for_while_stmt:
           {
              if (!isSgNullExpression(for_stmt->get_while_expression()))
                 {
                    curprint(" WHILE ");
                    unparseExpression(for_stmt->get_while_expression(), info);
                 }
              break;
           }
        case SgJovialForThenStatement::e_for_while_then_stmt:
           {
              if (!isSgNullExpression(for_stmt->get_while_expression()))
                 {
                    curprint(" WHILE ");
                    unparseExpression(for_stmt->get_while_expression(), info);
                 }
              if (!isSgNullExpression(for_stmt->get_by_or_then_expression()))
                 {
                    curprint(" THEN ");
                    unparseExpression(for_stmt->get_by_or_then_expression(), info);
                 }
              break;
           }
        case SgJovialForThenStatement::e_for_while_by_stmt:
           {
              if (!isSgNullExpression(for_stmt->get_while_expression()))
                 {
                    curprint(" WHILE ");
                    unparseExpression(for_stmt->get_while_expression(), info);
                 }
              if (!isSgNullExpression(for_stmt->get_by_or_then_expression()))
                 {
                    curprint(" BY ");
                    unparseExpression(for_stmt->get_by_or_then_expression(), info);
                 }
              break;
           }
        case SgJovialForThenStatement::e_for_then_while_stmt:
           {
              if (!isSgNullExpression(for_stmt->get_by_or_then_expression()))
                 {
                    curprint(" THEN ");
                    unparseExpression(for_stmt->get_by_or_then_expression(), info);
                 }
              if (!isSgNullExpression(for_stmt->get_while_expression()))
                 {
                    curprint(" WHILE ");
                    unparseExpression(for_stmt->get_while_expression(), info);
                 }
              break;
           }
        case SgJovialForThenStatement::e_for_by_while_stmt:
           {
              if (!isSgNullExpression(for_stmt->get_by_or_then_expression()))
                 {
                    curprint(" BY ");
                    unparseExpression(for_stmt->get_by_or_then_expression(), info);
                 }
              if (!isSgNullExpression(for_stmt->get_while_expression()))
                 {
                    curprint(" WHILE ");
                    unparseExpression(for_stmt->get_while_expression(), info);
                 }
              break;
           }
        case SgJovialForThenStatement::e_for_only_stmt:
           {
              break;
           }
        default:
           {
              cout << "Warning: In Jovial unparser, SgJovialForThenStatement::loop_statement_type not handled is "
                   << for_stmt->get_loop_statement_type() << endl;
           }
        }

     curprint(";");
     unp->cur.insert_newline(1);

  // Don't unparse control letters (variable declarations are compiler generated)
     SgBasicBlock* loop_body = isSgBasicBlock(for_stmt->get_loop_body());
     ROSE_ASSERT(loop_body);

  // Loop body
  //
  // Due to weird construction (basic block containing a basic block) the basic block for the loop
  // may be the last statement. The other statement (if present) will be the compiler generated control
  // variable declaration. However a SimpleStatement for the loop body won't have the extra basic block.
  //
  // In any case, comments may be attached before outer block loop body (not to actual loop body, see note
  // above about about weird construction and code below finding loop_body).
     unparseCommentsBefore(loop_body, info);

     if (loop_body->get_statements().size() == 1) {
        loop_body = isSgBasicBlock(for_stmt->get_loop_body()->get_statements()[0]);
     }
     else if (loop_body->get_statements().size() == 2) {
     // Degenerate case of a null loop body (see rose-issue-rc-42b.jov), don't unparse variable declaration
        SgVariableDeclaration* var_decl = isSgVariableDeclaration(for_stmt->get_loop_body()->get_statements()[0]);
        if (var_decl) SageInterface::removeStatement(var_decl);
        loop_body = isSgBasicBlock(for_stmt->get_loop_body()->get_statements()[1]);
     }

     if (!loop_body) {
        loop_body = isSgBasicBlock(for_stmt->get_loop_body());
     }
     ROSE_ASSERT(loop_body);

     unparseStatement(loop_body, info);
     unp->cur.insert_newline(1);
   }

void
UnparseJovial::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
     ASSERT_not_null(while_stmt);
     ASSERT_not_null(while_stmt->get_body());
     ASSERT_not_null(while_stmt->get_condition());

  // condition
     curprint_indented("WHILE ", info);
     info.set_inConditional(); // prevent printing line and file info

     SgExprStatement* condition_stmt = isSgExprStatement(while_stmt->get_condition());
     ASSERT_not_null(condition_stmt);

     unparseExpression(condition_stmt->get_expression(), info);
     info.unset_inConditional();
     curprint(";");
     unp->cur.insert_newline(1);

     unparseStatement(while_stmt->get_body(), info);
     unp->cur.insert_newline(1);
   }

void
UnparseJovial::unparseGotoStmt(SgGotoStatement* stmt, SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);
  ASSERT_not_null(stmt->get_label());

  curprint_indented(string("GOTO " ) + stmt->get_label()->get_label().str(), info);
  curprint(string(";"));
  unp->cur.insert_newline(1);
}

void
UnparseJovial::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     ASSERT_not_null(if_stmt);
     ASSERT_not_null(if_stmt->get_conditional());

  // condition
     curprint_indented("IF (", info);
     info.set_inConditional();

     SgExprStatement* expressionStatement = isSgExprStatement(if_stmt->get_conditional());
     unparseExpression(expressionStatement->get_expression(), info);

     info.unset_inConditional();
     curprint(") ;\n");

  // true body
     ASSERT_not_null(if_stmt->get_true_body());
     unparseStatement(if_stmt->get_true_body(), info);

  // false body
     if (if_stmt->get_false_body() != NULL) {
        curprint_indented("ELSE\n", info);
        unparseStatement(if_stmt->get_false_body(), info);
     }
   }

void
UnparseJovial::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
  {
 // Sage node corresponding to Jovial CaseStatement;
    SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
    ASSERT_not_null(switch_stmt);

    curprint_indented("CASE ", info);

    SgExprStatement* expressionStatement = isSgExprStatement(switch_stmt->get_item_selector());
    ASSERT_not_null(expressionStatement);
    unparseExpression(expressionStatement->get_expression(), info);

    curprint(";\n");

    if (switch_stmt->get_body())
      {
         unparseStatement(switch_stmt->get_body(), info);
      }
  }

void
UnparseJovial::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
  {
 // Sage node corresponding to Jovial CaseAlternative rule
    SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
    ASSERT_not_null(case_stmt);

    curprint_indented("(", info);
    unparseExpression(case_stmt->get_key(), info);
    curprint("):\n");

    if (case_stmt->get_body())
      {
         unparseStatement(case_stmt->get_body(), info);
      }
    if (case_stmt->get_has_fall_through())
      {
         curprint_indented("FALLTHRU\n", info);
      }
  }

void 
UnparseJovial::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
  {
 // Sage node corresponding to Jovial DefaultOption rule
    SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
    ASSERT_not_null(default_stmt);

    curprint_indented("(DEFAULT):\n", info);

    if (default_stmt->get_body())
      {
         unparseStatement(default_stmt->get_body(), info);
      }
    if (default_stmt->get_has_fall_through())
      {
         curprint_indented("FALLTHRU\n", info);
      }
  }

void
UnparseJovial::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info)
  {
 // This IR node is compiler generated for no FALLTHRU option in CaseAlternative rule.
 // It should not be unparsed, unparseCaseOptionStmt and unparseDefaultStmt will
 // unparse the FALLTHRU keyward as needed.
  }

void UnparseJovial::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
      SgTypedefDeclaration* typedef_decl = isSgTypedefDeclaration(stmt);
      ASSERT_not_null(typedef_decl);

      curprint_indented("TYPE ", info);

      SgName name = typedef_decl->get_name();
      curprint(name.str());

      SgType* base_type = typedef_decl->get_base_type();
      ASSERT_not_null(base_type);
      curprint(" ");
      unparseType(base_type, info);
      curprint(";");
      unp->cur.insert_newline(1);
   }

void
UnparseJovial::unparseProcessControlStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgProcessControlStatement* pc_stmt = isSgProcessControlStatement(stmt);
     ASSERT_not_null(pc_stmt);

     SgProcessControlStatement::control_enum kind = pc_stmt->get_control_kind();

     if (kind == SgProcessControlStatement::e_stop)
        {
          curprint_indented("STOP ", info);
          unparseExpression(pc_stmt->get_code(), info);
          curprint(";\n");
        }
     else if (kind == SgProcessControlStatement::e_exit)
        {
          curprint_indented("EXIT;\n", info);
        }
     else if (kind == SgProcessControlStatement::e_abort)
        {
          curprint_indented("ABORT;\n", info);
        }
     else
        {
          cerr << "UnparseJovial::unparseProcessControlStmt: unknown statement enum "
               <<  kind << endl;
          ROSE_ABORT();
        }
   }

void
UnparseJovial::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
      SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
      ASSERT_not_null(return_stmt);

      curprint("RETURN ;");
      unp->cur.insert_newline(1);
   }

void
UnparseJovial::unparseEnumBody(SgEnumDeclaration* enum_decl, SgUnparse_Info& info)
{
   ASSERT_not_null(enum_decl);

   SgEnumDeclaration* def_decl = isSgEnumDeclaration(enum_decl->get_definingDeclaration());
   ASSERT_not_null(def_decl);

   SgType* field_type = enum_decl->get_field_type();
   if (field_type) {
      SgTypeInt* int_type = isSgTypeInt(field_type);
      ASSERT_not_null(int_type);
      curprint(" ");
      SgExpression* kind_expr = int_type->get_type_kind();
      ASSERT_not_null(kind_expr);
      unparseExpression(kind_expr, info);
   }

   unp->cur.insert_newline(1);

   curprint_indented("(\n", info);
   info.inc_nestingLevel();

   int n = def_decl->get_enumerators().size();
   for (SgInitializedName* init_name : def_decl->get_enumerators()) {
      std::string name = init_name->get_name().str();
      name.replace(0, 3, "V(");
      name.append(")");

      SgAssignInitializer* assign_expr = isSgAssignInitializer(init_name->get_initializer());
      ASSERT_not_null(assign_expr);
      SgExpression* assign_op = assign_expr->get_operand();
      ASSERT_not_null(assign_op);

      // unparse comments preceding the expression
      auto preprocInfo = assign_op->get_attachedPreprocessingInfoPtr();
      if (preprocInfo) {
        for (PreprocessingInfo* info : *preprocInfo) {
          if (info->getRelativePosition() == PreprocessingInfo::before) {
            curprint(info->getString());
          }
        }
      }

      // unparse enum value
      if (auto enum_val = isSgEnumVal(assign_op)) {
        curprint_indented(tostring(enum_val->get_value()), info);
      }
      else {
        // there might be a better way to indent the expression
        curprint_indented("", info);
        unparseExpression(assign_op, info);
      }

      // unparse enum name
      curprint(name);
      if (--n > 0) curprint(",");

      // unparse comments succeeding the expression
       if (preprocInfo) {
         for (PreprocessingInfo* info : *preprocInfo) {
          if (info->getRelativePosition() == PreprocessingInfo::after) {
            curprint(info->getString());
          }
         }
       }

      unp->cur.insert_newline(1);
   }
   info.dec_nestingLevel();

   curprint_indented(")", info);
}

void
UnparseJovial::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_decl = isSgEnumDeclaration(stmt);
     ASSERT_not_null(enum_decl);

     SgName enum_name = enum_decl->get_name();

     curprint_indented("TYPE ", info);
     curprint(enum_name.str());
     curprint(" STATUS");

     unparseEnumBody(enum_decl, info);
     curprint(";\n");
   }

void
UnparseJovial::unparseOverlayDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
      SgJovialOverlayDeclaration* overlay_decl = isSgJovialOverlayDeclaration(stmt);
      ASSERT_not_null(overlay_decl);

      SgExprListExp* overlay = overlay_decl->get_overlay();
      SgExpression*  address = overlay_decl->get_address();

      curprint_indented("OVERLAY ", info);

      if (!isSgNullExpression(address))
         {
            curprint("POS (");
            unparseExpression(address, info);
            curprint(") ");
         }

      unparseOverlayExpr(overlay, info);
      curprint(";\n");
   }

void
UnparseJovial::unparseTableDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This unparses a table type declaration not a table variable declaration
  //
     SgJovialTableStatement* table_decl = isSgJovialTableStatement(stmt);
     ASSERT_not_null(table_decl);

     bool is_block = (table_decl->get_class_type() == SgClassDeclaration::e_jovial_block);

     SgJovialTableStatement* defining_decl = isSgJovialTableStatement(table_decl->get_definingDeclaration());
     ASSERT_not_null(isSgJovialTableStatement(defining_decl));

     SgClassDefinition* table_def = defining_decl->get_definition();
     ASSERT_not_null(table_def);

     SgName table_name = table_decl->get_name();

      SgType* type = table_decl->get_type();
      ASSERT_not_null(type);

      SgJovialTableType* table_type = isSgJovialTableType(type);
      ASSERT_not_null(table_type);

      curprint_indented("TYPE ", info);
      curprint(table_name);

      if (is_block) curprint(" BLOCK ");
      else          curprint(" TABLE ");

   // Table DimensionList
      SgExprListExp* dim_info = table_type->get_dim_info();
     if (dim_info != NULL)
        {
           unparseDimInfo(dim_info, info);
        }

  // OptStructureSpecifier
     using StructureSpecifier = SgJovialTableType::StructureSpecifier;
     if (table_type->get_structure_specifier() == StructureSpecifier::e_parallel) {
        curprint("PARALLEL ");
     }
     else if (table_type->get_structure_specifier() == StructureSpecifier::e_tight) {
        curprint("T ");
        if (table_type->get_bits_per_entry() > 0) {
           std::string value = Rose::StringUtility::numberToString(table_type->get_bits_per_entry());
           curprint(value);
           curprint(" ");
        }
     }

   // Like option
      if (table_decl->get_has_like_option()) {
         curprint("LIKE ");
         curprint(table_decl->get_like_table_name());
         curprint(" ");
      }

  // WordsPerEntry
     using WordsPerEntry = SgJovialTableStatement::WordsPerEntry;
     if (table_decl->get_words_per_entry() == WordsPerEntry::e_fixed_length) {
        curprint("W ");
        if (table_decl->get_has_table_entry_size()) {
           unparseExpression(table_decl->get_table_entry_size(), info);
        }
     }
     else if (table_decl->get_words_per_entry() == WordsPerEntry::e_variable_length) {
        curprint("V");
     }

  // Unparse base type or base class name if present
  //
     SgType* table_base_type = table_type->get_base_type();
     bool has_base_type  = (table_base_type != NULL);

     SgBaseClassPtrList base_class_list = table_def->get_inheritances();
     bool has_base_class = (base_class_list.size() > 0);

     if (has_base_type)
        {
           unparseType(table_base_type, info);
        }
     else if (has_base_class)
        {
           ROSE_ASSERT (base_class_list.size() == 1);
           SgBaseClass* base_class = base_class_list[0];
           ASSERT_not_null(base_class);
           SgClassDeclaration* base_class_decl = base_class->get_base_class();
           ASSERT_not_null(base_class_decl);

           curprint(base_class_decl->get_name());
        }

     if (!is_block)
        {
           // BLOCKs don't need the semicolon!
           curprint(";");
        }
     unp->cur.insert_newline(1);

  // Unparse table body
     unparseTableBody(table_def, info);
   }

void
UnparseJovial::unparseTableBody(SgClassDefinition* table_def, SgUnparse_Info& info)
   {
     ASSERT_not_null(table_def);

  // Unparse body if present
     if (table_def->get_members().size() > 0)
        {
           info.inc_nestingLevel();
           curprint_indented("BEGIN\n", info);

           info.inc_nestingLevel();
           for (SgDeclarationStatement* item_decl : table_def->get_members())
              {
                 if (isSgVariableDeclaration(item_decl))
                    {
                       unparseVarDeclStmt(item_decl, info);
                    }
                 else if (SgJovialDirectiveStatement* directive = isSgJovialDirectiveStatement(item_decl))
                    {
                       unparseDirectiveStmt(directive, info);
                    }
                 else if (isSgEmptyDeclaration(item_decl))
                    {
                       // must unparse ';' because "BEGIN ; END" appears in code
                       curprint_indented(";\n", info);

                    }
                 else cerr << "WARNING UNIMPLEMENTED: Unparse of unknown table member type \n";
              }
           info.dec_nestingLevel();

           unp->cur.insert_newline(1);
           curprint_indented("END\n", info);
           info.dec_nestingLevel();
        }
   }

void
UnparseJovial::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
     ASSERT_not_null(vardecl);
  
     SgInitializedNamePtrList::iterator p = vardecl->get_variables().begin();

  // Jovial has only one variable per declaration
     unparseVarDecl(vardecl, *p, info);
   }

void
UnparseJovial::unparseVarDecl(SgStatement* stmt, SgInitializedName* initializedName, SgUnparse_Info& info)
   {
     SgName name         = initializedName->get_name();
     SgType* type        = initializedName->get_type();
     SgInitializer* init = initializedName->get_initializer();
     ASSERT_not_null(type);

     SgModifierType* modifier_type = isSgModifierType(type);

     info.set_inVarDecl();

  // pretty printing
     curprint( ws_prefix(info.get_nestingLevel()) );

     bool is_block = false;
     SgClassDeclaration* type_decl = isSgClassDeclaration(type->getAssociatedDeclaration());
     if (type_decl) {
        is_block = (type_decl->get_class_type() == SgClassDeclaration::e_jovial_block);

     // Type could be an SgModifierType, for tables and blocks, save trouble and unwrap here
        if (modifier_type)
           {
              type = modifier_type->get_base_type();
              ASSERT_not_null(type);
           }
     }

     bool type_has_base_type = false;
     SgJovialTableType* table_type = isSgJovialTableType(type);
     if (table_type)
        {
           if (table_type->get_base_type()) type_has_base_type = true;
        }

     SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt);
     ASSERT_not_null(var_decl);

     if (var_decl->get_declarationModifier().isJovialDef())
        {
           curprint("DEF ");
        }
     if (var_decl->get_declarationModifier().isJovialRef())
        {
           curprint("REF ");
        }
     if (modifier_type)
        {
           if (modifier_type->get_typeModifier().get_constVolatileModifier().isConst()) {
             curprint("CONSTANT ");
           }
        }

     switch (type->variantT())
        {
          case V_SgJovialTableType:
             if (is_block) {
                curprint("BLOCK ");
             } else {
                curprint("TABLE ");
             }
             curprint(name.str());
             curprint(" ");
             break;
          default:
             curprint("ITEM ");
             curprint(name.str());
             curprint(" ");
        }

  // OptAllocationSpecifier
     if (var_decl->get_declarationModifier().isJovialStatic())
        {
           curprint("STATIC ");
        }

     SgNamedType* named_type = isSgNamedType(type);
     if (named_type && !var_decl->get_variableDeclarationContainsBaseTypeDefiningDeclaration())
        {
        // If a base type defining declaration then name prefix will be "_table_of_" or "_anon_typeof_"
           curprint(named_type->get_name());
        }
     else
        {
           unparseType(type, info);
        }

  // OptPackingSpecifier
     if      (var_decl->get_declarationModifier().get_storageModifier().isPackingNone())  curprint("N ");
     else if (var_decl->get_declarationModifier().get_storageModifier().isPackingMixed()) curprint("M ");
     else if (var_decl->get_declarationModifier().get_storageModifier().isPackingDense()) curprint("D ");

  // Unparse the LocationSpecifier if present
     if (var_decl->get_bitfield() != NULL)
        {
           SgExpression* bitfield = var_decl->get_bitfield();
           SgExprListExp* sg_location_specifier = isSgExprListExp(bitfield);
           ASSERT_not_null(sg_location_specifier);

           SgExpressionPtrList & location_exprs = sg_location_specifier->get_expressions();
           ROSE_ASSERT(location_exprs.size() == 2);

           curprint(" POS(");
           unparseExpression(location_exprs[0], info);
           curprint(",");
           unparseExpression(location_exprs[1], info);
           curprint(")");
        }

  // WordsPerEntry (for anonymous table declarations)
     SgJovialTableStatement* table_decl = nullptr;
     using WordsPerEntry = SgJovialTableStatement::WordsPerEntry;
     if (!type_has_base_type && var_decl->get_variableDeclarationContainsBaseTypeDefiningDeclaration())
        {
        // typedefs (e.g., TYPE utype U) also have a base_type (in this case U)
           table_decl = dynamic_cast<SgJovialTableStatement*>(var_decl->get_baseTypeDefiningDeclaration());
           if (table_decl) {
              if (table_decl->get_words_per_entry() == WordsPerEntry::e_fixed_length) {
                 curprint("W ");
                 if (table_decl->get_has_table_entry_size()) {
                    unparseExpression(table_decl->get_table_entry_size(), info);
                 }
              }
              else if (table_decl->get_words_per_entry() == WordsPerEntry::e_variable_length) {
                 curprint("V");
              }
           }
        }

  // Initialization
     if (init != NULL)
        {
           curprint(" = ");
           SgInitializer* initializer = isSgInitializer(init);
           ASSERT_not_null(initializer);
           unparseExpression(initializer, info);
        }

  // Unparse anonymous type declaration body if present
     if (!type_has_base_type && var_decl->get_variableDeclarationContainsBaseTypeDefiningDeclaration())
        {
          if (table_decl) {
            SgClassDefinition* table_def = table_decl->get_definition();
            ASSERT_not_null(table_def);

            curprint(";\n");

         // Unparse table body
            unparseTableBody(table_def, info);
          }
          else {
            // anonymous status type
            curprint(";");
          }
        }
     else
        {
           curprint(";");
           unparseCommentsAfter(stmt, info);
           curprint("\n");
        }

     info.unset_inVarDecl();
   }

void
UnparseJovial::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ASSERT_not_null(expr_stmt);

     SgExpression* expr = expr_stmt->get_expression();
     ASSERT_not_null(expr);

  // pretty printing
     curprint( ws_prefix(info.get_nestingLevel()) );

     unparseExpression(expr, info);

  // This is needed because SgAssignOp prints the ";" on its own
  // and function call is an expression so it can't add the ";" itself.
     if (isSgFunctionCallExp(expr)) {
        curprint(";");
     }
   }

void
UnparseJovial::unparseCommentsBefore(SgStatement* stmt, SgUnparse_Info& info)
{
  // unparse comments preceding the statement
  const AttachedPreprocessingInfoType* preprocInfo = stmt->get_attachedPreprocessingInfoPtr();
  if (preprocInfo) {
    for (PreprocessingInfo* info : *preprocInfo) {
      if (info->getRelativePosition() == PreprocessingInfo::before) {
        curprint(info->getString());
        curprint("\n");
      }
    }
  }
}

void
UnparseJovial::unparseCommentsAfter(SgStatement* stmt, SgUnparse_Info& info, bool newline)
{
  // unparse comments after the statement
  const AttachedPreprocessingInfoType* preprocInfo = stmt->get_attachedPreprocessingInfoPtr();
  if (preprocInfo) {
    for (PreprocessingInfo* info : *preprocInfo) {
      auto pos = info->getRelativePosition();
      if (pos == PreprocessingInfo::end_of || pos == PreprocessingInfo::after) {
        if (newline) {
          newline = false;
          if (pos == PreprocessingInfo::end_of) {
            curprint(" "); // pad end_of_stmt from comment
          }
          else {
            curprint("\n");
          }
        }
        curprint(info->getString());
        curprint("\n");
      }
    }
  }
  if (newline) curprint("\n");
}
