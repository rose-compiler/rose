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
 * Unparse_Jovial::unparse is called, and for statements, 
 * Unparse_Jovial::unparseStatement is called.
 *
 */
#include "sage3basic.h"
#include "unparser.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "sage_support.h"


Unparse_Jovial::Unparse_Jovial(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
   }


Unparse_Jovial::~Unparse_Jovial()
   {
   }


void 
Unparse_Jovial::unparseJovialFile(SgSourceFile *sourcefile, SgUnparse_Info& info) 
   {
     SgGlobal* globalScope = sourcefile->get_globalScope();
     ASSERT_not_null(globalScope);

     curprint("START\n");
     unparseStatement(globalScope, info);
     curprint("TERM\n");
   }


void
Unparse_Jovial::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific statements not handled by the base class unparseStatement() member function

     ASSERT_not_null(stmt);

     switch (stmt->variantT())
        {
       // module support
          case V_SgJovialCompoolStatement:     unparseCompoolStmt (stmt, info);     break;
          case V_SgProgramHeaderStatement:     unparseProgHdrStmt (stmt, info);     break;
          case V_SgProcedureHeaderStatement:   unparseProcDeclStmt(stmt, info);     break;
       // case V_SgFunctionDeclaration:        unparseFuncDeclStmt(stmt, info);     break;  /* replaced by SgProcedureHeaderStatement */
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
          case V_SgForStatement:               unparseForStatement   (stmt, info);  break;
          case V_SgJovialForThenStatement:     unparseJovialForThenStmt(stmt, info);  break;
          case V_SgWhileStmt:                  unparseWhileStmt      (stmt, info);  break;
          case V_SgGotoStatement:              unparseGotoStmt       (stmt, info);  break;
          case V_SgIfStmt:                     unparseIfStmt         (stmt, info);  break;
          case V_SgSwitchStatement:            unparseSwitchStmt     (stmt, info);  break;
          case V_SgCaseOptionStmt:             unparseCaseStmt       (stmt, info);  break;
          case V_SgDefaultOptionStmt:          unparseDefaultStmt    (stmt, info);  break;
          case V_SgBreakStmt:                  unparseBreakStmt      (stmt, info);  break;
          case V_SgTypedefDeclaration:         unparseTypeDefStmt    (stmt, info);  break;

          case V_SgStopOrPauseStatement:       unparseStopOrPauseStmt(stmt, info);  break;
          case V_SgReturnStmt:                 unparseReturnStmt     (stmt, info);  break;

          case V_SgExprStatement:              unparseExprStmt       (stmt, info);  break;

#if 0
       // declarations
          case V_SgVariableDefinition:     unparseVarDefnStmt  (stmt, info); break;

       // executable statements, control flow

          case V_SgAssertStmt:             unparseAssertStmt     (stmt, info); break;

          case V_SgContinueStmt:           unparseContinueStmt(stmt, info);     break;



          case V_SgForInitStatement:       unparseForInitStmt(stmt, info);      break;

          case V_SgFunctionParameterList:  unparseFunctionParameterList(stmt, info); break;

          case V_SgUsingDirectiveStatement:            unparseUsingDirectiveStatement (stmt, info);            break;
          case V_SgUsingDeclarationStatement:          unparseUsingDeclarationStatement (stmt, info);          break;
#endif

          default:
            {
               cerr << "Unparse_Jovial::unparseLanguageSpecificStatement: Error: No handler for "
                    <<  stmt->class_name() << ", variant: " << stmt->variantT() << endl;
               ROSE_ASSERT(false);
               break;
            }
        }
   }


//----------------------------------------------------------------------------
//  Unparse_Jovial::DIRECTIVES and DEFINE
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseDirectiveStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);

     SgJovialDirectiveStatement* directive = isSgJovialDirectiveStatement(stmt);
     ROSE_ASSERT(directive);

     std::string content = directive->get_content_string();

     switch (directive->get_directive_type())
        {
        case SgJovialDirectiveStatement::e_compool:
           {
              curprint("!COMPOOL ('");
              curprint(content);
              curprint("');\n");
              break;
           }
        case SgJovialDirectiveStatement::e_reducible:
           {
              curprint("!REDUCIBLE;\n");
              break;
           }
        case SgJovialDirectiveStatement::e_order:
           {
              curprint("!ORDER;\n");
              break;
           }
        default:
           {
              cout << "Warning: SgJovialDirectiveStmt directive type not handled is " << directive->get_directive_type() << endl;
           }
        }
   }

void
Unparse_Jovial::unparseDefineDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgJovialDefineDeclaration* define = isSgJovialDefineDeclaration(stmt);
     ROSE_ASSERT(define);

     curprint("DEFINE ");
     curprint(define->get_define_string());
     curprint(";\n");
   }

//----------------------------------------------------------------------------
//  Unparse_Jovial::MODULES
//----------------------------------------------------------------------------

void 
Unparse_Jovial::unparseCompoolStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgJovialCompoolStatement* compool = isSgJovialCompoolStatement(stmt);
     ROSE_ASSERT(compool);

     curprint("COMPOOL ");
     curprint(compool->get_name());
     curprint(";\n");
   }

void
Unparse_Jovial::unparseProgHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);

     SgProgramHeaderStatement* prog = isSgProgramHeaderStatement(stmt);
     ROSE_ASSERT(prog);

     curprint("PROGRAM ");
     curprint(prog->get_name());
     curprint(";\n");

     unparseStatement(prog->get_definition(), ninfo);
   }

void
Unparse_Jovial::unparseProcDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);

     SgProcedureHeaderStatement* func = isSgProcedureHeaderStatement(stmt);
     ROSE_ASSERT(func);

     bool isDefiningDeclaration = (func->get_declarationModifier().isJovialRef() == false);

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

     if (args.size() > 0)
        {
           bool firstOutParam = false;
           bool foundOutParam = false;

           curprint("(");

           int i = 0;
           foreach(SgInitializedName* arg, args)
              {
              // TODO - Change temporary hack of using storage modifier isMutable to represent an out parameter
                 if (arg->get_storageModifier().isMutable() && foundOutParam == false)
                    {
                       firstOutParam = true;
                       foundOutParam = true;
                       curprint(":");
                    }

              // Don't output comma if this is the first out parameter
                 if (i++ > 0 && firstOutParam == false) curprint(",");
                 firstOutParam = false;

                 curprint(arg->get_name());
              }
           curprint(")");
        }

  // unparse function type
     SgType* type = func->get_type();
     unparseType(type, ninfo);

     curprint(";\n");

     if (isDefiningDeclaration)
        {
           ROSE_ASSERT(func->get_definition());

           info.inc_nestingLevel();
           unparseStatement(func->get_definition(), ninfo);
           info.dec_nestingLevel();
        }
     else
        {
           // There still needs to be at least a BEGIN and END
           info.inc_nestingLevel();
           curprint_indented("BEGIN\n", info);

           info.inc_nestingLevel();
           foreach(SgInitializedName* arg, args)
              {
                 curprint( ws_prefix(info.get_nestingLevel()) );
                 curprint("ITEM ");
                 curprint(arg->get_name());
                 curprint(" ");
                 unparseType(arg->get_type(), ninfo);
                 curprint(" ;\n");
              }
           info.dec_nestingLevel();

           curprint_indented("END\n", info);
           info.dec_nestingLevel();
        }
   }

// Deprecated (Jovial will always have a function body so SgProcedureHeaderStatement)
#if 0
void
Unparse_Jovial::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);

     SgFunctionDeclaration* func = isSgFunctionDeclaration(stmt);
     ROSE_ASSERT(func);

     bool isDefiningDeclaration = (func->get_definition() != NULL);

  // This will likely need to be changed.  It may work for compool files but likely not for jovial files.
     if (isDefiningDeclaration)  curprint("DEF PROC ");
     else                        curprint("REF PROC ");

     curprint(func->get_name());

  // unparse the function modifiers
     if      (func->get_functionModifier().isRecursive())    curprint(" REC");
     else if (func->get_functionModifier().isReentrant())    curprint(" RENT");

  // unparse function arguments
     SgFunctionParameterList* params = func->get_parameterList();
     SgInitializedNamePtrList & args = params->get_args();

     if (args.size() > 0)
        {
           bool firstOutParam = false;
           bool foundOutParam = false;

           curprint("(");

           int i = 0;
           foreach(SgInitializedName* arg, args)
              {
              // TODO - Change temporary hack of using storage modifier isMutable to represent an out parameter
                 if (arg->get_storageModifier().isMutable() && foundOutParam == false)
                    {
                       firstOutParam = true;
                       foundOutParam = true;
                       curprint(" : ");
                    }

              // Don't output comma if this is the first out parameter
                 if (i++ > 0 && firstOutParam == false) curprint(",");
                 firstOutParam = false;

                 curprint(arg->get_name());
              }
           curprint(")");
        }

  // unparse function type
     SgType* type = func->get_type();
     unparseType(type, ninfo);

     curprint(";\n");

     if (isDefiningDeclaration)
        {
           unparseStatement(func->get_definition(), ninfo);
        }
     else
        {
           // There still needs to be at least a BEGIN and END
           curprint_indented("BEGIN\n", ninfo);
           foreach(SgInitializedName* arg, args)
              {
              // TODO: at some point a table type will need to be unparsed here
                 SgJovialTableType* table_type = isSgJovialTableType(type);
                 ROSE_ASSERT(table_type == NULL);

                 curprint("    ITEM ");
                 curprint(arg->get_name());
                 curprint(" ");
                 unparseType(arg->get_type(), ninfo);
                 curprint(" ;\n");
              }
           curprint_indented("END\n", ninfo);
        }
   }
#endif

void
Unparse_Jovial::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
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
Unparse_Jovial::unparseNamespaceDeclarationStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceDeclarationStatement* decl = isSgNamespaceDeclarationStatement(stmt);
     ASSERT_not_null(decl);

     SgNamespaceDefinitionStatement* defn = decl->get_definition();
     ASSERT_not_null(defn);

     unparseNamespaceDefinitionStatement(defn, info);
   }

void
Unparse_Jovial::unparseNamespaceDefinitionStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceDefinitionStatement* namespace_defn = isSgNamespaceDefinitionStatement(stmt);
     ASSERT_not_null(namespace_defn);

     const SgDeclarationStatementPtrList& declarations = namespace_defn->get_declarations();

     info.inc_nestingLevel();
     foreach(SgStatement* namespace_stmt, declarations)
        {
           unparseStatement(namespace_stmt, info);
        }
     info.dec_nestingLevel();
   }


//----------------------------------------------------------------------------
//  Unparse_Jovial::<executable statements, control flow>
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* block = isSgBasicBlock(stmt);
     ASSERT_not_null(block);

     int block_size = block->get_statements().size();

  // allow one declaration to be unparsed without BEGIN and END
     if (block_size > 1)
        {
           curprint_indented("BEGIN\n", info);
        }

     info.inc_nestingLevel();
     foreach(SgStatement* block_stmt, block->get_statements())
        {
           unparseStatement(block_stmt, info);
        }
     info.dec_nestingLevel();

     if (block_size > 1)
        {
           curprint_indented("END\n", info);
        }
   }

void Unparse_Jovial::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ASSERT_not_null(label_stmt);

     curprint (string(label_stmt->get_label().str()) + ":");
     unp->cur.insert_newline(1);

     if (label_stmt->get_statement() != NULL) {
        SgStatement* sg_stmt = label_stmt->get_statement();
        ROSE_ASSERT(sg_stmt);
        UnparseLanguageIndependentConstructs::unparseStatement(sg_stmt, info);
     }
   }

void
Unparse_Jovial::unparseForStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // The SgForStatement is used for the Jovial for statements like:
  //
  //     FOR ivar:0 by 1 while ivar<25;
  //
  // This choice was made so that it could be treated like a C for statement.
  // Other forms of the Jovial ForStatement will require different Sage nodes.
  //
     SgForStatement* for_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(for_stmt);
     ROSE_ASSERT(for_stmt->get_for_init_stmt());
     ROSE_ASSERT(for_stmt->get_test());
     ROSE_ASSERT(for_stmt->get_increment());
     ROSE_ASSERT(for_stmt->get_loop_body());

     curprint("FOR ");

     SgForInitStatement* for_init_stmt = isSgForInitStatement(for_stmt->get_for_init_stmt());
     ROSE_ASSERT(for_init_stmt);

     SgStatementPtrList init_list = for_init_stmt->get_init_stmt();
     SgExprStatement* init_stmt = isSgExprStatement(init_list[0]);
     ROSE_ASSERT(init_stmt);

     SgAssignOp* init_expr = isSgAssignOp(init_stmt->get_expression());
     ROSE_ASSERT(init_expr);

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
     ROSE_ASSERT(test_stmt);

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
Unparse_Jovial::unparseJovialForThenStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // The SgJovialForThenStatement is used for Jovial for statements like:
  //
  //    FOR ivar:0 THEN 3 WHILE ivar<25;
  //
     SgJovialForThenStatement* for_stmt = isSgJovialForThenStatement(stmt);
     ROSE_ASSERT(for_stmt);
     ROSE_ASSERT(for_stmt->get_initialization());
     ROSE_ASSERT(for_stmt->get_then_expression());
     ROSE_ASSERT(for_stmt->get_while_expression());
     ROSE_ASSERT(for_stmt->get_loop_body());

     curprint("FOR ");

     SgAssignOp* init_expr = isSgAssignOp(for_stmt->get_initialization());
     ROSE_ASSERT(init_expr);

  // variable
     unparseExpression(init_expr->get_lhs_operand_i(), info);

  // initial value
     curprint(":");
     unparseExpression(init_expr->get_rhs_operand_i(), info);

  // then increment
     curprint(" THEN ");
     unparseExpression(for_stmt->get_then_expression(), info);

  // while condition
     if ( ! isSgNullExpression(for_stmt->get_while_expression()) )
        {
           curprint(" WHILE ");
           unparseExpression(for_stmt->get_while_expression(), info);
        }

     curprint(";");
     unp->cur.insert_newline(1);

  // for body
     unparseStatement(for_stmt->get_loop_body(), info);
     unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
     ROSE_ASSERT(while_stmt);
     ROSE_ASSERT(while_stmt->get_body());
     ROSE_ASSERT(while_stmt->get_condition());

  // condition
     curprint("WHILE ");
     info.set_inConditional(); // prevent printing line and file info

     SgExprStatement* condition_stmt = isSgExprStatement(while_stmt->get_condition());
     ROSE_ASSERT(condition_stmt);

     unparseExpression(condition_stmt->get_expression(), info);
     info.unset_inConditional();
     curprint(";");
     unp->cur.insert_newline(1);

     unparseStatement(while_stmt->get_body(), info);
     unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
     ASSERT_not_null(goto_stmt);
     ASSERT_not_null(goto_stmt->get_label());

     curprint (string("GOTO " ) + goto_stmt->get_label()->get_label().str());
     curprint (string(";"));
     unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     ASSERT_not_null(if_stmt);
     ROSE_ASSERT(if_stmt->get_conditional());

  // condition
     curprint_indented("IF (", info);
     info.set_inConditional();

     SgExprStatement* expressionStatement = isSgExprStatement(if_stmt->get_conditional());
     unparseExpression(expressionStatement->get_expression(), info);

     info.unset_inConditional();
     curprint(") ;\n");

  // true body
     ROSE_ASSERT(if_stmt->get_true_body());
     unparseStatement(if_stmt->get_true_body(), info);

  // false body
     if (if_stmt->get_false_body() != NULL) {
        curprint_indented("ELSE\n", info);
        unparseStatement(if_stmt->get_false_body(), info);
     }
   }

void
Unparse_Jovial::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
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
Unparse_Jovial::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
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
Unparse_Jovial::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
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
Unparse_Jovial::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info)
  {
 // This IR node is compiler generated for no FALLTHRU option in CaseAlternative rule.
 // It should not be unparsed, unparseCaseOptionStmt and unparseDefaultStmt will
 // unparse the FALLTHRU keyward as needed.
  }

void Unparse_Jovial::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
      SgTypedefDeclaration* typedef_decl = isSgTypedefDeclaration(stmt);
      ROSE_ASSERT(typedef_decl);

      curprint("TYPE ");

      SgName name = typedef_decl->get_name();
      curprint(name.str());

      SgType* base_type = typedef_decl->get_base_type();
      ROSE_ASSERT(base_type);
      curprint(" ");
      unparseType(base_type, info);
      curprint(";");
      unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseStopOrPauseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgStopOrPauseStatement* sp_stmt = isSgStopOrPauseStatement(stmt);
     ASSERT_not_null(sp_stmt);

     SgStopOrPauseStatement::stop_or_pause_enum kind = sp_stmt->get_stop_or_pause();

     if (kind == SgStopOrPauseStatement::e_stop)
        {
          curprint_indented("STOP ", info);
          unparseExpression(sp_stmt->get_code(), info);
          curprint(";\n");
        }
     else if (kind == SgStopOrPauseStatement::e_exit)
        {
          curprint_indented("EXIT;\n", info);
        }
     else if (kind == SgStopOrPauseStatement::e_abort)
        {
          curprint_indented("ABORT;\n", info);
        }
     else
        {
          cerr << "Unparse_Jovial::unparseStopOrPauseStmt: unknown statement enum "
               <<  kind << endl;
          ROSE_ASSERT(false);
        }
   }

void
Unparse_Jovial::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
      SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
      ASSERT_not_null(return_stmt);

      curprint("RETURN ;");
      unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_decl = isSgEnumDeclaration(stmt);
     ASSERT_not_null(enum_decl);

     SgName enum_name = enum_decl->get_name();
     SgType* field_type = enum_decl->get_field_type();

     curprint("TYPE ");
     curprint(enum_name.str());
     curprint(" STATUS");

     if (field_type) {
        SgTypeInt* int_type = isSgTypeInt(field_type);
        ASSERT_not_null(int_type);
        curprint(" ");
        SgExpression* kind_expr = int_type->get_type_kind();
        ASSERT_not_null(kind_expr);
        unparseExpression(kind_expr, info);
     }

     unp->cur.insert_newline(1);

     curprint("(");
     unp->cur.insert_newline(1);

     int n = enum_decl->get_enumerators().size();
     foreach(SgInitializedName* init_name, enum_decl->get_enumerators())
        {
           std::string name = init_name->get_name().str();
           name.replace(0, 3, "V(");
           name.append(")");

           SgAssignInitializer* assign_expr = isSgAssignInitializer(init_name->get_initializer());
           ASSERT_not_null(assign_expr);
           SgEnumVal* enum_val = isSgEnumVal(assign_expr->get_operand());
           ASSERT_not_null(enum_val);

           curprint("  ");
           curprint(tostring(enum_val->get_value()));
           curprint(name);
           if (--n > 0) curprint(",");
           unp->cur.insert_newline(1);
        }

     curprint(");");
     unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseOverlayDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
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
Unparse_Jovial::unparseTableDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
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

      curprint("TYPE ");
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
     if (table_type->get_structure_specifier() == SgJovialTableType::e_parallel) {
        curprint("PARALLEL ");
     }
     else if (table_type->get_structure_specifier() == SgJovialTableType::e_tight) {
        curprint("T ");
        if (table_type->get_bits_per_entry() > 0) {
           std::string value = Rose::StringUtility::numberToString(table_type->get_bits_per_entry());
           curprint(value);
           curprint(" ");
        }
     }

  // WordsPerEntry
     if (table_decl->get_has_table_entry_size())
        {
        // TODO - fix ROSETTA so this doesn't depend on NULL for entry size, has_table_entry_size should be table_entry_enum (or some such)
           if (table_decl->get_table_entry_size() != NULL)
              {
                 curprint("W ");
                 unparseExpression(table_decl->get_table_entry_size(), info);
              }
           else curprint("V");
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

  // Unparse body if present
     if (table_def->get_members().size() > 0)
        {
           curprint("BEGIN");
           unp->cur.insert_newline(1);

           foreach(SgDeclarationStatement* item_decl, table_def->get_members())
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
                       // do nothing for a null declaration (may want to unparse ";\n")
                    }
                 else cerr << "WARNING UNIMPLEMENTED: Unparse of table member not a variable declaration \n";
              }

           unp->cur.insert_newline(1);
           curprint("END");
           unp->cur.insert_newline(1);
        }
   }

void
Unparse_Jovial::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
     ASSERT_not_null(vardecl);
  
     SgInitializedNamePtrList::iterator p = vardecl->get_variables().begin();

  // Jovial has only one variable per declaration
     unparseVarDecl(vardecl, *p, info);
   }

void
Unparse_Jovial::unparseVarDecl(SgStatement* stmt, SgInitializedName* initializedName, SgUnparse_Info& info)
   {
     SgName name         = initializedName->get_name();
     SgType* type        = initializedName->get_type();
     SgInitializer* init = initializedName->get_initializer();
     ASSERT_not_null(type);

     info.set_inVarDecl();

  // pretty printing
     curprint( ws_prefix(info.get_nestingLevel()) );

     bool is_block = false;
     SgClassDeclaration* type_decl = isSgClassDeclaration(type->getAssociatedDeclaration());
     if (type_decl) {
        is_block = (type_decl->get_class_type() == SgClassDeclaration::e_jovial_block);
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
     if (var_decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst())
        {
           curprint("CONSTANT ");
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

     unparseType(type, info);

  // OptStructureSpecifier
     if (table_type)
        {
           if (table_type->get_structure_specifier() == SgJovialTableType::e_parallel) {
              curprint("PARALLEL ");
           }
           else if (table_type->get_structure_specifier() == SgJovialTableType::e_tight) {
              curprint("T ");
              if (table_type->get_bits_per_entry() > 0) {
                 std::string value = Rose::StringUtility::numberToString(table_type->get_bits_per_entry());
                 curprint(value);
                 curprint(" ");
              }
           }
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
           SgDeclarationStatement* def_decl = var_decl->get_baseTypeDefiningDeclaration();
           ASSERT_not_null(def_decl);

           SgJovialTableStatement* table_decl = dynamic_cast<SgJovialTableStatement*>(def_decl);
           ASSERT_not_null(table_decl);

        // WordsPerEntry for anonymous table declarations
           if (table_decl->get_has_table_entry_size())
              {
                 // TODO - fix ROSETTA so this doesn't depend on NULL for entry size, has_table_entry_size should be table_entry_enum (or some such)
                 if (table_decl->get_table_entry_size() != NULL)
                    {
                       curprint("W ");
                       unparseExpression(table_decl->get_table_entry_size(), info);
                    }
                 else curprint("V");
              }

           SgClassDefinition* table_def = table_decl->get_definition();
           ASSERT_not_null(table_def);

           if (table_def->get_members().size() > 0)
              {
                 curprint(";\n");

                 info.inc_nestingLevel();
                 curprint_indented("BEGIN\n", info);

                 info.inc_nestingLevel();
                 foreach(SgDeclarationStatement* item_decl, table_def->get_members())
                    {
                       if (isSgVariableDeclaration(item_decl))
                          {
                             unparseVarDeclStmt(item_decl, info);
                          }
                       else if (isSgEmptyDeclaration(item_decl))
                          {
                             // do nothing for a null declaration (may want to unparse ";\n")
                          }
                       else cerr << "WARNING UNIMPLEMENTED: Unparse of table member not a variable declaration \n";
                    }
                 info.dec_nestingLevel();

                 curprint_indented("END\n", info);
                 info.dec_nestingLevel();
              }
        }
     else
        {
           curprint(";\n");
        }

     info.unset_inVarDecl();
   }

void
Unparse_Jovial::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ASSERT_not_null(expr_stmt);
     ASSERT_not_null(expr_stmt->get_expression());

  // pretty printing
     curprint( ws_prefix(info.get_nestingLevel()) );

     unparseExpression(expr_stmt->get_expression(), info);

     unp->u_sage->curprint_newline();
   }

