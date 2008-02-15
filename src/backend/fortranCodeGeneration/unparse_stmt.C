/* unparse_stmt.C
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
 * Unparser::unparse is called, and for statements, 
 * Unparser::unparseStatement is called.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"
#include "unparser_opt.h"
#include "unparser.h"

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS 0
#define OUTPUT_DEBUGGING_UNPARSE_INFO 0

void
printOutComments ( SgLocatedNode* locatedNode )
   {
  // Debugging function to print out comments in the statements (added by DQ)

     ROSE_ASSERT(locatedNode != NULL);
     AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

     if (comments != NULL)
        {
          printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
          AttachedPreprocessingInfoType::iterator i;
          for (i = comments->begin(); i != comments->end(); i++)
             {
               ROSE_ASSERT ( (*i) != NULL );
               printf ("          Attached Comment (relativePosition=%s): %s",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
             }
        }
       else
        {
          printf ("No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
        }
   }

//-----------------------------------------------------------------------------------
//  void Unparser::unparseStatement
//
//  General function that gets called when unparsing a statement. Then it routes
//  to the appropriate function to unparse each kind of statement.
//-----------------------------------------------------------------------------------
void
Unparser::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT(stmt != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Unparse statement (%p): sage_class_name() = %s \n",stmt,stmt->sage_class_name());
     cur << "\n/* Unparse statement: sage_class_name() = " << stmt->sage_class_name() << " */ \n";
#endif

  // [DT] 3/9/2000 -- Q: Why is this happening?  We probably
  //      need to handle it in a different way than just
  //      ignoring it like this.
  //
  //      Partial Answer: It is happening during recursive
  //      calls to unparseStatement.  We definitely *do*
  //      need to handle this in a different way.  This
  //      results in very incomplete unparsed code.
  //   
  //      A: The recursive calls to unparseStatement are not
  //      sending the root node of the statement but just
  //      the definition or body, etc, of the statement which
  //      must not have the file information.
  //
  //      8/17/2000 -- NOTE: We should be able to assert that
  //      this is not happening now.  See, also, unparser.C.
  //
  
     if (ROSE_DEBUG > 3)
        {
          cout << "In unparseStatement(): getLineNumber(stmt) = "
               << ROSE::getLineNumber(stmt)
               << " getFileName(stmt) = " 
               << ROSE::getFileName(stmt)
            // << " line_to_unparse = " 
            // << line_to_unparse 
               << " cur_index = " 
               << cur_index
               << endl;
        }
  
  // This is part of the original SAGE support for pragmas 
  // I have added pragmas to the grammar so I don't know if this is required anymore.
  /*
     if (!info.inConditional() && stmt->variant() != BASIC_BLOCK_STMT)
        {
          directives(stmt);
        }
  */

     printDebugInfo("-----UNPARSING NEXT STATEMENT-----", TRUE);
     printDebugInfo("statement is in file: ", FALSE); printDebugInfo(getFileName(stmt), TRUE);
     printDebugInfo("statement line number: ", FALSE); printDebugInfo(ROSE::getLineNumber(stmt), TRUE);
  
#if 0
  // Debugging support
     printOutComments (stmt);
#endif

  // Markus Kowarschik: This is the new code to unparse directives before the current statement
     unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);
  
  // check if line_to_unparse is greater than 0. If so, we need to check whether
  // this is the line that we are interested in unparsing. Otherwise, line_to_unparse
  // should be 0, which indicates that we are unparsing all the statements and 
  // directives of the file.
  //
  // [DT] 3/9/2000 -- Changing the following check to accomodate unparsing included files.
  //
  // DQ: Replaced the fix that Danny inserted to debug why unparser is 
  // unparsing declarations from header files in the source files!
     if (ROSE_DEBUG > 5)
          cerr << "Uncommented test for same file while unparsing!" << endl;

     if (line_to_unparse > 0 && !strcmp(getFileName(stmt), getCurOutFileName()))
        {
          if (ROSE::getLineNumber(stmt) != line_to_unparse)
                return ;
        }
  
#if 0
  // the following section is for debugging purposes. Set a breakpoint after the
  // conditional to stop at the line number in the input source file. Modify the
  // value of debugline as needed.
     int debugline = 29;
     if (ROSE::getLineNumber(stmt) == debugline)
        {
          printDebugInfo("stopped at line ", FALSE);
          printDebugInfo(debugline, TRUE);
        }
#endif
  
     char buffer[512];
     sprintf (buffer,"unparseStatement: entering case for %s",stmt->sage_class_name());
     printDebugInfo(buffer,TRUE);

     cur.format(stmt, info, FORMAT_BEFORE_STMT);
     if (repl != 0)  {
        if (repl->unparse_statement(stmt,info, cur))
            return;
     }   

     //MS: insert Attribute unparsing here
     switch (stmt->variant())
        {
          case DECL_STMT:          unparseDeclStmt(stmt, info);         break;
          case SCOPE_STMT:         unparseScopeStmt(stmt, info);        break;
          case FUNC_TBL_STMT:      unparseFuncTblStmt(stmt, info);      break;
          case GLOBAL_STMT:        unparseGlobalStmt(stmt, info);       break;
          case BASIC_BLOCK_STMT:   unparseBasicBlockStmt(stmt, info);   break;
          case IF_STMT:            unparseIfStmt(stmt, info);           break;
          case FOR_STMT:           unparseForStmt(stmt, info);          break; 
          case FUNC_DECL_STMT:     unparseFuncDeclStmt(stmt, info);     break;
          case FUNC_DEFN_STMT:     unparseFuncDefnStmt(stmt, info);     break;
          case MFUNC_DECL_STMT:    unparseMFuncDeclStmt(stmt, info);    break;
          case VAR_DECL_STMT:      unparseVarDeclStmt(stmt, info);      break;
          case VAR_DEFN_STMT:      unparseVarDefnStmt(stmt, info);      break;
          case CLASS_DECL_STMT:    unparseClassDeclStmt(stmt, info);    break;
          case CLASS_DEFN_STMT:    unparseClassDefnStmt(stmt, info);    break;
          case ENUM_DECL_STMT:     unparseEnumDeclStmt(stmt, info);     break;
          case EXPR_STMT:          unparseExprStmt(stmt, info);         break;
          case LABEL_STMT:         unparseLabelStmt(stmt, info);        break;
          case WHILE_STMT:         unparseWhileStmt(stmt, info);        break;
          case DO_WHILE_STMT:      unparseDoWhileStmt(stmt, info);      break;
          case SWITCH_STMT:        unparseSwitchStmt(stmt, info);       break;
          case CASE_STMT:          unparseCaseStmt(stmt, info);         break;
          case TRY_STMT:           unparseTryStmt(stmt, info);          break;
          case CATCH_STMT:         unparseCatchStmt(stmt, info);        break;
          case DEFAULT_STMT:       unparseDefaultStmt(stmt, info);      break;
          case BREAK_STMT:         unparseBreakStmt(stmt, info);        break;
          case CONTINUE_STMT:      unparseContinueStmt(stmt, info);     break;
          case RETURN_STMT:        unparseReturnStmt(stmt, info);       break;
          case GOTO_STMT:          unparseGotoStmt(stmt, info);         break;
          case ASM_STMT:           unparseAsmStmt(stmt, info);          break;
          case SPAWN_STMT:         unparseSpawnStmt(stmt, info);        break;
          case TYPEDEF_STMT:       unparseTypeDefStmt(stmt, info);      break;
          case TEMPLATE_DECL_STMT: unparseTemplateDeclStmt(stmt, info); break;

          case TEMPLATE_INST_DECL_STMT:
               unparseTemplateInstantiationDeclStmt(stmt, info); break;

          case TEMPLATE_INST_FUNCTION_DECL_STMT:
               unparseTemplateInstantiationFunctionDeclStmt(stmt, info); break;

          case TEMPLATE_INST_MEMBER_FUNCTION_DECL_STMT:
               unparseTemplateInstantiationMemberFunctionDeclStmt(stmt, info); break;

          case PRAGMA_DECL:
            // cerr << "WARNING: unparsePragmaDeclStmt not implemented in SAGE 3 (exiting ...)" << endl;
            // This can't be an error since the A++ preprocessor currently processes #pragmas
            // (though we can ignore unparsing them)
            // ROSE_ABORT();

               unparsePragmaDeclStmt(stmt, info);
               break;

          case FOR_INIT_STMT:
               unparseForInitStmt(stmt, info);
               break;

       // Comments could be attached to these statements
          case CATCH_STATEMENT_SEQ:
          case FUNCTION_PARAMETER_LIST:
          case CTOR_INITIALIZER_LIST:
               printf ("Ignore these newly implemented cases (case of %s) \n",stmt->sage_class_name());
               printf ("WARNING: These cases must be implemented so that comments attached to them can be processed \n");
            // ROSE_ABORT();
               break;

          case NAMESPACE_DECLARATION_STMT:       unparseNamespaceDeclarationStatement (stmt, info);      break;
          case NAMESPACE_DEFINITION_STMT:        unparseNamespaceDefinitionStatement (stmt, info);       break;
          case NAMESPACE_ALIAS_DECLARATION_STMT: unparseNamespaceAliasDeclarationStatement (stmt, info); break;
          case USING_DIRECTIVE_STMT:             unparseUsingDirectiveStatement (stmt, info);            break;
          case USING_DECLARATION_STMT:           unparseUsingDeclarationStatement (stmt, info);          break;

          default:
            // printf("Error: default reached in unparseStatement switch statement", TRUE);
               printf("Error: default reached in unparseStatement switch statement (value=%d) \n",stmt->variantT());
               printf ("stmt->sage_class_name() = %s \n",stmt->sage_class_name());
               ROSE_ABORT();
               break;
        }
     cur.format(stmt, info, FORMAT_AFTER_STMT);
  // Markus Kowarschik: This is the new code to unparse directives after the current statement
     unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving unparse statement (%p): sage_class_name() = %s \n",stmt,stmt->sage_class_name());
     cur << "\n/* Bottom of unparseStatement: sage_class_name() = " << stmt->sage_class_name() << " */ \n";
#endif
   }

void
Unparser::unparseNamespaceDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // There is a SgNamespaceDefinition, but it is not unparsed except through the SgNamespaceDeclaration

     SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceDeclaration != NULL);
     cur << "namespace ";
     SgName name = namespaceDeclaration->get_name();
     ROSE_ASSERT (name.str() != NULL);
     if (name.str() != NULL)
          cur << name.str();

     unparseStatement(namespaceDeclaration->get_definition(),info);
   }

void
Unparser::unparseNamespaceDefinitionStatement ( SgStatement* stmt, SgUnparse_Info & info )
   {
     ROSE_ASSERT (stmt != NULL);
     SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(stmt);
     ROSE_ASSERT (namespaceDefinition != NULL);

     cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
     cur << "{";
     cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);

  // unparse all the declarations
     SgDeclarationStatementPtrList & statementList = namespaceDefinition->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = statementList.begin();
     while ( statementIterator != statementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ROSE_ASSERT(currentStatement != NULL);

          bool unparseStatementIntoSourceFile = statementFromFile (currentStatement,getCurOutFileName());

          if (ROSE_DEBUG > 5)
               cout << "unparseStatementIntoSourceFile = " 
                    << ( (unparseStatementIntoSourceFile == true) ? "false" : "false" ) << endl;

          if (unparseStatementIntoSourceFile == true)
             {
               unparseStatement(currentStatement, info);
             }

       // Go to the next statement
          statementIterator++;
        }

     cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
     cur << "}\n";
     cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);
   }

void
Unparser::unparseNamespaceAliasDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceAliasDeclaration != NULL);

     cur << "\nnamespace ";
     ROSE_ASSERT(namespaceAliasDeclaration->get_name().str() != NULL);
     cur << namespaceAliasDeclaration->get_name().str();
     cur << " = ";
     ROSE_ASSERT(namespaceAliasDeclaration->get_namespaceDeclaration() != NULL);
     ROSE_ASSERT(namespaceAliasDeclaration->get_namespaceDeclaration()->get_name().str() != NULL);
     cur << namespaceAliasDeclaration->get_namespaceDeclaration()->get_name().str();
     cur << ";\n";
   }

void
Unparser::unparseUsingDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(stmt);
     ROSE_ASSERT (usingDirective != NULL);

  // DQ (8/26/2004): This should be "using namespace" instead of just "using"
     cur << "\nusing namespace ";
     ROSE_ASSERT(usingDirective->get_namespaceDeclaration() != NULL);
     ROSE_ASSERT(usingDirective->get_namespaceDeclaration()->get_name().str() != NULL);
     printf ("In unparseUsingDirectiveStatement using namespace = %s qualified name = %s \n",
          usingDirective->get_namespaceDeclaration()->get_name().str(),
          usingDirective->get_namespaceDeclaration()->get_qualified_name().str());
  // cur << usingDirective->get_namespaceDeclaration()->get_name().str();
     cur << usingDirective->get_namespaceDeclaration()->get_qualified_name().str();
     cur << ";\n";
   }

void
Unparser::unparseUsingDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(stmt);
     ROSE_ASSERT (usingDeclaration != NULL);

     cur << "\nusing ";

#if 1
  // DQ (9/11/2004): We only save the declaration and get the name by unparsing the declaration
  // Might have to setup info1 to only output the name that we want!
     SgUnparse_Info info1(info);
  // info1.unset_CheckAccess();
  // info1.set_PrintName();
     info1.unset_isWithType();

     SgDeclarationStatement* declarationStatement = usingDeclaration->get_declaration();
     ROSE_ASSERT(declarationStatement != NULL);

     printf ("In unparseUsingDeclarationStatement(): declarationStatement = %s \n",declarationStatement->sage_class_name());
  // unparseStatement(declarationStatement,info1);

  // DQ (9/12/2004): Add the qualification required to resolve the name
     SgScopeStatement* scope = declarationStatement->get_scope();
     cur << scope->get_qualified_name().str() << "::";

  // Handle the different sorts of declarations explicitly since the existing unparse functions for 
  // declarations are not setup for what the using declaration unparser requires.
     switch (declarationStatement->variantT())
        {
          case V_SgVariableDeclaration:
             {
            // get the name of the variable in the declaration
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
               ROSE_ASSERT(variableDeclaration != NULL);
               SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
            // using directives must be issued sepearately for each variable!
               ROSE_ASSERT(variableList.size() == 1);
               SgInitializedName* initializedName = *(variableList.begin());
               ROSE_ASSERT(initializedName != NULL);
               SgName variableName = initializedName->get_name();
               ROSE_ASSERT(variableName.str() != NULL);
               cur << variableName.str();
               break;
             }

          case V_SgNamespaceDeclarationStatement:
             {
               SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(declarationStatement);
               ROSE_ASSERT(namespaceDeclaration != NULL);
               SgName namespaceName = namespaceDeclaration->get_name();
               ROSE_ASSERT(namespaceName.str() != NULL);
               cur << namespaceName.str();
               break;
             }           

          case V_SgFunctionDeclaration:
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(functionDeclaration != NULL);
               SgName functionName = functionDeclaration->get_name();
               ROSE_ASSERT(functionName.str() != NULL);
               cur << functionName.str();
               break;
             }

          case V_SgMemberFunctionDeclaration:
             {
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(memberFunctionDeclaration != NULL);
               SgName memberFunctionName = memberFunctionDeclaration->get_name();
               ROSE_ASSERT(memberFunctionName.str() != NULL);
               cur << memberFunctionName.str();
               break;
             }

          case V_SgClassDeclaration:
             {
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
               ROSE_ASSERT(classDeclaration != NULL);
               SgName className = classDeclaration->get_name();
               ROSE_ASSERT(className.str() != NULL);
               cur << className.str();
               break;
             }

          case V_SgTypedefDeclaration:
             {
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declarationStatement);
               ROSE_ASSERT(typedefDeclaration != NULL);
               SgName typedefName = typedefDeclaration->get_name();
               ROSE_ASSERT(typedefName.str() != NULL);
               cur << typedefName.str();
               break;
             }

          case V_SgTemplateDeclaration:
             {
            // DQ (9/12/2004): This function outputs the default template name which is not correct, we need 
            // to get more information out of EDG about the template name if we are to get this correct in 
            // the future.  This could (and likely will) cause generated code to not compile, but I will 
            // worry about that after we can compile Kull.
               SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(declarationStatement);
               ROSE_ASSERT(templateDeclaration != NULL);
               SgName templateName = templateDeclaration->get_name();
               ROSE_ASSERT(templateName.str() != NULL);
               cur << templateName.str();
               break;
             }

          default:
             {
               printf ("Default reached in unparseUsingDeclarationStatement(): case is not implemented for %s \n",declarationStatement->sage_class_name());
               ROSE_ASSERT(false);
             }
        }

     cur << ";\n";
#else
  // This is the old way of handling using-declarations, but it is not general enough (only handled using of namespaces)!
     ROSE_ASSERT(usingDeclaration->get_namespaceDeclaration() != NULL);
     ROSE_ASSERT(usingDeclaration->get_namespaceDeclaration()->get_name().str() != NULL);

  // printf ("In unparseUsingDeclarationStatement: usingDeclaration->get_namespaceDeclaration()->get_name() = %s \n",usingDeclaration->get_namespaceDeclaration()->get_name().str());
  // printf ("In unparseUsingDeclarationStatement: usingDeclaration->get_name() = %s \n",usingDeclaration->get_name().str());

     cur << usingDeclaration->get_namespaceDeclaration()->get_name().str();
     cur << "::";
     cur << usingDeclaration->get_name().str();
     cur << ";\n";
#endif
   }

void
Unparser::unparseTemplateInstantiationDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (2/29/2004): New function to support templates
     SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(stmt);
     ROSE_ASSERT(templateInstantiationDeclaration != NULL);

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(templateInstantiationDeclaration);
     ROSE_ASSERT(classDeclaration != NULL);

  // Call the unparse function for a class declaration
  // If the template has not been modified then don't output the template specialization 
  // (force the backend compiler to handle the template specialization and instantiation).
     if ( isTransformed (templateInstantiationDeclaration) == true )
        {
       // If the template has been transformed then we have to output the special version 
       // of the template as a template specialization.

       // If a class template has been modified then we need to make sure that all the 
       //      static data members, and 
       //      member functions 
       // are instantiated (on the next pass through the prelinker). The process should 
       // involve a call to the EDG function:
       //      static void set_instantiation_required_for_template_class_members (a_type_ptr class_type)
       // I am not currently sure how to make this happen, but it should involve the *.ti 
       // files (I guess).

       // printf ("Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");

       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // cur << "template <> \n";
          unparseClassDeclStmt(classDeclaration,info);

       // This case is not supported if member functions or static data members are present in the class
       // (could generate code which would compile but not link!).
          printf ("\n\n");
          printf ("WARNING: Transformations on templated classes can currently generate code which \n");
          printf ("         will not compile, since member function and static data members are not \n");
          printf ("         presently generated for the new specialization of the transformed template class. \n");
          printf ("\n\n");
        }
       else
        {
       // If not transformed then we only want to output the template (and usually only the 
       // name of the template specialization) in variable declarations and the like.
       // These locations control the output of the template specialization explicitly 
       // through the SgUnparse_Info object.
          if ( info.outputClassTemplateName() == true )
             {
            // printf ("Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");

            // DQ (9/8/2004):
            // The unparseClassDeclStmt does much more that what we require and is not setup to 
            // handle templates well, it generally works well if the template name required does 
            // not need any qualification (e.g. std::templateName<int>).  Thus don't use the 
            // unparseClassDeclStmt and just output the qualified template name.
            // unparseClassDeclStmt(classDeclaration,info);

            // Output the qualified template name
               cur << templateInstantiationDeclaration->get_qualified_name().str();
             }
#if 0
            else
             {
               printf ("Skipping call to unparse the SgTemplateInstantiationDecl = %p \n",
                    templateInstantiationDeclaration);
             }
#endif
        }
   }

void
Unparser::unparseTemplateInstantiationFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (3/24/2004): New function to support templates
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = 
          isSgTemplateInstantiationFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationFunctionDeclaration != NULL);

     SgFunctionDeclaration* functionDeclaration = 
          isSgFunctionDeclaration(templateInstantiationFunctionDeclaration);
     ROSE_ASSERT(functionDeclaration != NULL);

  // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
  // cur << "template <> \n";
     unparseFuncDeclStmt(functionDeclaration,info);
   }

void
Unparser::unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (3/24/2004): New function to support templates
     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = 
          isSgTemplateInstantiationMemberFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration != NULL);

     SgFunctionDeclaration* memberFunctionDeclaration = 
          isSgMemberFunctionDeclaration(templateInstantiationMemberFunctionDeclaration);
     ROSE_ASSERT(memberFunctionDeclaration != NULL);

  // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
  // cur << "template <> \n";
     unparseMFuncDeclStmt(memberFunctionDeclaration,info);
   }

void
Unparser::unparsePragmaDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(stmt);
     ROSE_ASSERT(pragmaDeclaration != NULL);

     SgPragma* pragma = pragmaDeclaration->get_pragma();
     ROSE_ASSERT(pragma != NULL);

     cur << "#pragma " << pragma->get_pragma() << "\n";

  // printf ("Output the pragma = %s \n",pragma->get_pragma());
  // ROSE_ASSERT (0);
   }


void
Unparser::unparseGlobalStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgGlobal* globalScope = isSgGlobal(stmt);
     ROSE_ASSERT(globalScope != NULL);

  // Setup an iterator to go through all the statements in the top scope of the file.
     SgDeclarationStatementPtrList & globalStatementList = globalScope->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = globalStatementList.begin();
     while ( statementIterator != globalStatementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ROSE_ASSERT(currentStatement != NULL);

//        opt.display("Before statementFromFile");
          bool unparseStatementIntoSourceFile = statementFromFile (currentStatement,getCurOutFileName());

          if (ROSE_DEBUG > 5)
               cout << "unparseStatementIntoSourceFile = " 
                    << ( (unparseStatementIntoSourceFile == true) ? "true" : "false" ) << endl;

//        unparseStatementIntoSourceFile = true;
          if (unparseStatementIntoSourceFile == true)
             {
               if (ROSE_DEBUG > 3)
                  {
                 // (*primary_os)
                    cout << "In run_unparser(): getLineNumber(currentStatement) = "
                         << ROSE::getLineNumber(currentStatement)
                         << " getFileName(currentStatement) = " 
                         << ROSE::getFileName(currentStatement)
                         << " cur_index = " 
                         << cur_index
                         << endl;
                  }

               unparseStatement(currentStatement, info);
             }
            else
             {
            // printf ("Skipped unparsing %s (global declaration) \n",currentStatement->sage_class_name());
             }
          

       // Go to the next statement
          statementIterator++;
        }
   }


void
Unparser::unparseDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // SgDeclarationStatement* decl_stmt = isSgDeclarationStatement(stmt);
  // unparseStatement(decl_stmt, info);
   }

void Unparser::unparseScopeStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgScopeStatement* scope_stmt = isSgScopeStatement(stmt);
     ROSE_ASSERT(scope_stmt != NULL);

     stringstream  out;
     scope_stmt->print_symboltable("UNPARSE", out);
     cur << out.str();
   }

void Unparser::unparseFuncTblStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionTypeTable* functbl_stmt = isSgFunctionTypeTable(stmt);
     ROSE_ASSERT(functbl_stmt != NULL);

     stringstream  out;
     functbl_stmt->print_functypetable(out);
     cur << out.str();
   }

void
Unparser::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ROSE_ASSERT(basic_stmt != NULL);

     cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
     cur << "{";
     cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     while(p != basic_stmt->get_statements().end())
        { 
          ROSE_ASSERT((*p) != NULL);
          unparseStatement((*p), info);
          p++;
        }
     cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
     cur << "}";
     cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
   }

//--------------------------------------------------------------------------------
//  void Unparser::num_stmt_in_block
//
//  returns the number of statements in the basic block
//--------------------------------------------------------------------------------  
int Unparser::num_stmt_in_block(SgBasicBlock* basic_stmt) {
  //counter to keep number of statements in the block
  int num_stmt = 0;
  SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
  while (p != basic_stmt->get_statements().end()) {
    num_stmt++;
    p++;
  }

  return num_stmt;
}

void Unparser::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse if statement \n");
     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     assert (if_stmt != NULL);

     while (if_stmt != 0) {
       SgStatement *tmp_stmt=NULL;
       cur << "if (";
       info.set_inConditional();
       if ( (tmp_stmt = if_stmt->get_conditional()) ) 
          unparseStatement(tmp_stmt, info);
       info.unset_inConditional();
       cur << ") ";
        
       if ( (tmp_stmt = if_stmt->get_true_body()) ) 
        {
       // printf ("Unparse the if true body \n");
       // cur << "\n/* Unparse the if true body */ \n";
          unparseStatement(tmp_stmt, info);
       // cur << "\n/* DONE: Unparse the if true body */ \n";
        }
     
       if ( (tmp_stmt = if_stmt->get_false_body()) )
        {
          cur.format(if_stmt, info, FORMAT_BEFORE_STMT);
          cur << "else ";
          if_stmt = isSgIfStmt(tmp_stmt);
          if (if_stmt == 0) 
             unparseStatement(tmp_stmt, info);
        }
        else 
          if_stmt = 0;
     }
   }

//--------------------------------------------------------------------------------
//  void Unparser::unparseWhereStmt
//
//  This special function unparses where and elsewhere statements. Where 
//  statements are actually represented as for statements in the Sage program
//  tree. Thus, the type of the where_stmt is SgForStatement. The part that
//  we are interested in unparsing is in the initializer statement of the 
//  for statement. In particular, we want to unparse the arguments of the
//  rhs of the initializer. The rhs should be a function call expression.
//  The same applies for elsewhere statements.
//--------------------------------------------------------------------------------
void
Unparser::unparseWhereStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgForStatement* where_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(where_stmt != NULL);

     SgStatement *tmp_stmt;
  // DQ (4/7/2001) we don't want the unparser to depend on the array grammar
  // (so comment this out and introduce the A++ "where" statment in some other way)
#if 0
     if (ArrayClassSageInterface::isROSEWhereStatement(stmt))
        {
          cur << "where (";
        }
       else
        {
       // isROSEElseWhereStatement
          cur << "elsewhere (";
        }
#else
          cur << "elsewhere (";
#endif

     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
  // if(where_stmt->get_init_stmt() != NULL ) {
     if(where_stmt->get_init_stmt().size() > 0 )
        {
          SgStatementPtrList::iterator i=where_stmt->get_init_stmt().begin();
#if USE_SAGE3
          if ((*i) != NULL && (*i)->variant() == EXPR_STMT)
             {
               SgExprStatement* pExprStmt = isSgExprStatement(*i);
#else
          if ((*i).irep() != NULL && (*i).irep()->variant() == EXPR_STMT)
             {
               SgExprStatement* pExprStmt = isSgExprStatement((*i).irep());
#endif
               SgAssignOp* pAssignOp = isSgAssignOp(pExprStmt->get_the_expr());
               if ( pAssignOp != NULL )
                  {
                    SgFunctionCallExp* pFunctionCallExp = isSgFunctionCallExp(pAssignOp->get_rhs_operand());
                    if(pFunctionCallExp != NULL)
                       {
                         if(pFunctionCallExp->get_args())
                            {
                              SgExpressionPtrList& list = pFunctionCallExp->get_args()->get_expressions();
                              SgExpressionPtrList::iterator arg = list.begin();
                              while (arg != list.end())
                                 {
                                   unparseExpression((*arg), newinfo);
                                   arg++;
                                   if (arg != list.end())
                                      {
                                      cur << ",";
                                      }
                                 }
                            }
                       } //pFunctionCallExp != NULL 
                  } //pAssignOp != NULL
             } //(*i).irep() != NULL && (*i).irep()->variant() == EXPR_STMT
        } //where_stmt->get_init_stmt() != NULL

     cur << ")";

     if ( (tmp_stmt = where_stmt->get_loop_body()) )
        {
          unparseStatement(tmp_stmt, info);
        }
       else
        {
          if (!info.SkipSemiColon())
             {
               cur << ";";
             }
        }
   }

void
Unparser::unparseForInitStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (7/11/2004): Added to simplify debugging for everyone (requested by Willcock)

  // printf ("Function not implemented yet! \n");

  // printf ("Unparse for loop initializers \n");
     SgForInitStatement* forInitStmt = isSgForInitStatement(stmt);
     ROSE_ASSERT(forInitStmt != NULL);

     SgStatementPtrList::iterator i = forInitStmt->get_init_stmt().begin();
     while(i != forInitStmt->get_init_stmt().end())
        {
          unparseStatement(*i, info);
          i++;
          if (i != forInitStmt->get_init_stmt().end())
             {
               cur << ", ";
             }
        }

     cur << "; ";
   }

void Unparser::unparseForStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse for loop \n");
     SgForStatement* for_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(for_stmt != NULL);

     SgStatement *tmp_stmt;
     cur << "for (";
     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
     newinfo.set_inConditional();  //set to prevent printing line and file information

     if (for_stmt->get_for_init_stmt())
        unparseStatement(for_stmt->get_for_init_stmt(),newinfo);
     else
        cur << "; ";
     newinfo.unset_inConditional();

     SgExpression *tmp_expr;
     if ( (tmp_expr = for_stmt->get_test_expr()))
          unparseExpression(tmp_expr, info);
     cur << "; ";

     if ( (tmp_expr = for_stmt->get_increment_expr()) )
          unparseExpression(tmp_expr, info);
     cur << ") ";

  // Added support to output the header without the body to support the addition 
  // of more context in the prefix used with the AST Rewrite Mechanism.
  // if ( (tmp_stmt = for_stmt->get_loop_body()) )
     if ( (tmp_stmt = for_stmt->get_loop_body()) && !info.SkipBasicBlock())
        {
       // printf ("Unparse the for loop body \n");
       // cur << "\n/* Unparse the for loop body */ \n";
          unparseStatement(tmp_stmt, info);
       // cur << "\n/* DONE: Unparse the for loop body */ \n";
        }
       else
        {
       // printf ("No for loop body to unparse! \n");
       // cur << "\n/* No for loop body to unparse! */\n";
          if (!info.SkipSemiColon())
             {
               cur << ";";
             }
        }
   }

void
Unparser::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDeclStmt() \n");
  // cur << "/* Inside of Unparser::unparseFuncDeclStmt */";

  // info.display("Inside of unparseFuncDeclStmt()");
#endif

     SgFunctionDeclaration* funcdecl_stmt = isSgFunctionDeclaration(stmt);
     ROSE_ASSERT(funcdecl_stmt != NULL);

#if 0
  // printf ("Inside of Unparser::unparseFuncDeclStmt(): funcdecl_stmt->get_from_template() = %s \n",
  //      funcdecl_stmt->get_from_template() ? "true" : "false");
     if (funcdecl_stmt->get_from_template() == true)
          cur << "/* Unparser comment: Templated Function */";
#endif

  /* EXCEPTION HANDLING: Forward Declarations */
  // DO NOT use the file information sage gives us because the line information
  // refers to the function definition (if it is defined in the same file). Instead, 
  // the line of the forward declaration is set one after the line number of the 
  // previous node or directive. By doing this, any comments around the forward 
  // declaration will come after the declaration, since I'm setting the line number 
  // to be right after the previous thing we unparsed.

  // the following is a HACK. I want to know if this statement is in a header or
  // C++ file. If it is in a header file, then I proceed as normal because header 
  // files provide correct information. If the statement is in a C++ file and is a
  // forward declaration, then I must follow this HACK.
  /* EXCEPTION HANDLING: Forward Declarations */
     SgUnparse_Info ninfo(info);

     if (!funcdecl_stmt->isForward() && funcdecl_stmt->get_definition() && !info.SkipFunctionDefinition())
        {
       // printf ("Not a forward function (normal function) \n");
       // cur << "\n/* Not a forward function (normal function) */\n";
          unparseStatement(funcdecl_stmt->get_definition(), ninfo);
          if (funcdecl_stmt->isExternBrace())
             {
               cur << " }";
             }
        }
       else
        {
       // printf ("Forward function (function prototype) \n");
       // cur << "\n/* Forward function (function prototype) */\n";
          SgClassDefinition *cdefn = isSgClassDefinition(funcdecl_stmt->get_parent());

          if (cdefn && cdefn->get_declaration()->get_class_type()==SgClassDeclaration::e_class)
               ninfo.set_CheckAccess();

       // printf ("Comment out call to get_suppress_atomic(funcdecl_stmt) \n");
#if 0
          if (get_suppress_atomic(funcdecl_stmt))
               ninfo.set_SkipAtomic();   // attributes.h
#endif

       // DQ (10/17/2004): Skip output of class definition for function declaration! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. And by extention 
       // any function declaration!
          ninfo.set_SkipClassDefinition();
          ninfo.set_SkipEnumDefinition();

          printSpecifier(funcdecl_stmt, ninfo);

          ninfo.unset_CheckAccess();
          info.set_access_attribute(ninfo.get_access_attribute());
    
          SgType *rtype=funcdecl_stmt->get_orig_return_type();
          if (!rtype)
               rtype = funcdecl_stmt->get_type()->get_return_type();
          ninfo.set_isTypeFirstPart();

       // output the return type
       // cur << "\n/* output the return type */\n";
          unparseType(rtype, ninfo);

       // output the rest of the function declaration
       // cur << "/* calling unparse_helper */";
          unparse_helper(funcdecl_stmt, ninfo);

          ninfo.set_isTypeSecondPart();
       // cur << "/* output the second part of the type */";
          unparseType(rtype, ninfo);
       // cur << "/* DONE: output the second part of the type */";

       // DQ (4/28/2004): Added support for throw modifier
          if (funcdecl_stmt->get_declarationModifier().isThrow())
             {
               printf ("Output throw modifier (incomplete implementation) \n");
               cur << " throw( /* from unparseFuncDeclStmt() type list output not implemented */ )";
             }

          if (funcdecl_stmt->isForward() && !ninfo.SkipSemiColon())
             {
               cur << ";";
               if (funcdecl_stmt->isExternBrace())
                  {
                    cur << " }";
                  }
             }
        }

#if 0
  // DQ (4/28/2004): Added support for throw modifier
     if (info.SkipClassDefinition() && funcdecl_stmt->get_declarationModifier().isThrow())
        {
          cur << " throw()";
        }
#endif

  // DQ (1/23/03) Added option to support rewrite mechanism (generation of declarations)
     if (info.AddSemiColonAfterDeclaration())
        {
          cur << ";";
        }

  // cur << "/* End of Unparser::unparseFuncDeclStmt */";
  // printf ("End of Unparser::unparseFuncDeclStmt() \n");
   }

// NOTE: Bug in Sage: No file information provided for FuncDeclStmt. 
void
Unparser::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDefnStmt() \n");
     cur << "/* Inside of Unparser::unparseFuncDefnStmt */";
#endif

     SgFunctionDefinition* funcdefn_stmt = isSgFunctionDefinition(stmt);
     ROSE_ASSERT(funcdefn_stmt != NULL);

  // Unparse any comments of directives attached to the SgFunctionParameterList
     ROSE_ASSERT (funcdefn_stmt->get_declaration() != NULL);
     if (funcdefn_stmt->get_declaration()->get_parameterList() != NULL)
         unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::before);

     info.set_SkipFunctionDefinition();
     SgStatement *declstmt = funcdefn_stmt->get_declaration();

  // DQ (3/24/2004): Need to permit SgMemberFunctionDecl and SgTemplateInstantiationMemberFunctionDecl
  // if (declstmt->variant() == MFUNC_DECL_STMT)

  // DQ (5/8/2004): Any generated specialization needed to use the 
  // C++ syntax for explicit specification of specializations.
  // if (isSgTemplateInstantiationMemberFunctionDecl(declstmt) != NULL)
  //      cur << "template<> ";

     if ( isSgMemberFunctionDeclaration(declstmt) != NULL )
        unparseMFuncDeclStmt( declstmt, info);
     else
        unparseFuncDeclStmt( declstmt, info);

     info.unset_SkipFunctionDefinition();
     SgUnparse_Info ninfo(info);
  
  // now the body of the function
     if (funcdefn_stmt->get_body())
          unparseStatement(funcdefn_stmt->get_body(), ninfo);
       else
        {
          cur << "{}";

       // DQ (9/22/2004): I think this is an error!
          printf ("Error: Should be an error to not have a function body in the AST \n");
          ROSE_ASSERT(false);
        }

  // Unparse any comments of directives attached to the SgFunctionParameterList
     unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::after);
   }

void
Unparser::unparseMFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgMemberFunctionDeclaration* mfuncdecl_stmt = isSgMemberFunctionDeclaration(stmt);
     ROSE_ASSERT(mfuncdecl_stmt != NULL);

#if 0
     printf ("Inside of Unparser::unparseMFuncDeclStmt(): mfuncdecl_stmt->get_from_template() = %s \n",
          mfuncdecl_stmt->get_from_template() ? "true" : "false");
     if (mfuncdecl_stmt->get_from_template() == true)
          cur << "/* Unparser comment: Templated Member Function */";
#endif

  // Unparse any comments of directives attached to the SgCtorInitializerList
     if (mfuncdecl_stmt->get_CtorInitializerList() != NULL)
         unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), info, PreprocessingInfo::before);

  /* EXCEPTION HANDLING: Member Forward Declarations in *.C file */
  // DO NOT use the file information sage gives us because the line information
  // refers to the function definition (if it is defined in the same file). Instead, 
  // the line of the forward declaration is set one after the line number of the 
  // previous node or directive. By doing this, any comments around the forward 
  // declaration will come after the declaration, since I'm setting the line number 
  // to be right after the previous thing we unparsed.
  
  // the following is a HACK. I want to know if this statement is in a header or
  // C++ file. If it is in a header file, then I proceed as normal. If the
  // statement is in a C++ file and is a forward declaration, then I must 
  // follow this HACK. When the Sage bug of forward declaration is fixed, remove
  // this code.
  /* EXCEPTION HANDLING: Member Forward Declarations in *.C file */

     if ( !mfuncdecl_stmt->isForward() && mfuncdecl_stmt->get_definition() && !info.SkipFunctionDefinition() )
        {
       // printf ("Unparsing special case of non-forward, valid definition and !skip function definition \n");

          unparseStatement(mfuncdecl_stmt->get_definition(), info);

          if (mfuncdecl_stmt->isExternBrace())
             {
               cur.format(mfuncdecl_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
               cur << " }";
               cur.format(mfuncdecl_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
             }
        } 
       else 
        {
       // printf ("Normal case for unparsing member function declarations! \n");

       // DQ (5/8/2004): Any generated specialization needed to use the 
       // C++ syntax for explicit specification of specializations.
          if (isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt) != NULL)
               cur << "template<> ";

          ROSE_ASSERT (mfuncdecl_stmt->get_parent() != NULL);
          SgClassDefinition *parent_class = isSgClassDefinition(mfuncdecl_stmt->get_parent());

       // printf ("Commented out call to get_suppress_atomic(mfuncdecl_stmt) \n");
#if 0
          if (get_suppress_atomic(mfuncdecl_stmt))
               info.set_SkipAtomic();
#endif

          if (parent_class && parent_class->get_declaration()->get_class_type() == SgClassDeclaration::e_class) 
               info.set_CheckAccess();
          printDebugInfo("entering printSpecifier1", TRUE);
          printSpecifier1(mfuncdecl_stmt, info);

          printDebugInfo("entering printSpecifier2", TRUE);
          printSpecifier2(mfuncdecl_stmt, info);
          info.unset_CheckAccess();

          SgUnparse_Info ninfo(info);
          SgType *rtype = NULL;

       // DQ (10/17/2004): Added code to form skipping enum definitions.
       // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. 
          ninfo.set_SkipClassDefinition();
          ninfo.set_SkipEnumDefinition();

       // if (!(mfuncdecl_stmt->isConstructor() || mfuncdecl_stmt->isDestructor() || mfuncdecl_stmt->isConversion()))
          if ( !( mfuncdecl_stmt->get_specialFunctionModifier().isConstructor() || 
                  mfuncdecl_stmt->get_specialFunctionModifier().isDestructor()  ||
                  mfuncdecl_stmt->get_specialFunctionModifier().isConversion() ) )
             {
            // printf ("In unparser: NOT a constructor, destructor or conversion operator \n");

               if (mfuncdecl_stmt->get_orig_return_type() != NULL)
                    rtype = mfuncdecl_stmt->get_orig_return_type();
                 else
                    rtype = mfuncdecl_stmt->get_type()->get_return_type();

               ninfo.set_isTypeFirstPart();
               ninfo.set_SkipClassSpecifier();
               unparseType(rtype, ninfo);
               ninfo.unset_SkipClassSpecifier();
            // printf ("In unparser: DONE with NOT a constructor, destructor or conversion operator \n");
             }
            else
             {
            // DQ (9/17/2004): What can we assume about the return type of a constructor, destructor, or conversion operator?
               ROSE_ASSERT(mfuncdecl_stmt->get_orig_return_type() != NULL);
               ROSE_ASSERT(mfuncdecl_stmt->get_type()->get_return_type() != NULL);
             }

          ROSE_ASSERT (mfuncdecl_stmt != NULL);

       // printf ("In unparser: parent_class = %p \n",parent_class);
       // printf ("In unparser: mfuncdecl_stmt->get_name() = %s \n",mfuncdecl_stmt->get_name().str());

          if (parent_class)
             {
            /* in a class structure */

               ROSE_ASSERT (mfuncdecl_stmt->get_name().str() != NULL);
               cur <<  mfuncdecl_stmt->get_name().str() ;
             }
            else
             {
            /* at global scope,  s::f() */
               SgName scopename;

               ROSE_ASSERT (mfuncdecl_stmt->get_scope() != NULL);
               ROSE_ASSERT (mfuncdecl_stmt->get_scope()->get_declaration() != NULL);

               scopename = mfuncdecl_stmt->get_scope()->get_declaration()->get_qualified_name();
               ROSE_ASSERT (scopename.str() != NULL);

            // printf ("In unparseMFuncDeclStmt(): mfuncdecl_stmt->get_scope() = %p = %s \n",mfuncdecl_stmt->get_scope(),mfuncdecl_stmt->get_scope()->sage_class_name());
            // printf ("In unparseMFuncDeclStmt(): mfuncdecl_stmt->get_scope()->get_declaration() = %p = %s \n",mfuncdecl_stmt->get_scope()->get_declaration(),mfuncdecl_stmt->get_scope()->get_declaration()->sage_class_name());
            // printf ("In unparseMFuncDeclStmt(): mfuncdecl_stmt->get_scope()->get_declaration() = %p scopename = %s \n",mfuncdecl_stmt->get_scope()->get_declaration(),scopename.str());

               cur <<  scopename.str();
               cur <<  "::";
               cur << mfuncdecl_stmt->get_name().str();
             }

          SgUnparse_Info ninfo2(info);

       // DQ (10/17/2004): Skip output of enum and class definitions for return type! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. 
          ninfo2.set_SkipClassDefinition();
          ninfo2.set_SkipEnumDefinition();

          ninfo2.set_inArgList();
          SgName tmp_name;

       // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. 
          ninfo2.set_SkipClassDefinition();

          cur << "(";
       // Unparse the function arguments
          unparseFunctionArgs(mfuncdecl_stmt,ninfo2);
          cur << ")";

          if (rtype != NULL)
             {
               SgUnparse_Info ninfo3(ninfo);
               ninfo3.set_isTypeSecondPart();

               unparseType(rtype, ninfo3);
             }

          SgMemberFunctionType *mftype = isSgMemberFunctionType(mfuncdecl_stmt->get_type());
          if (!info.SkipFunctionQualifier() && mftype )
             {
               if (mftype->isConstFunc())
                  {
                    cur << " const";
                  }
               if (mftype->isVolatileFunc())
                  {
                    cur << " volatile";
                  }
             }
#if 1
       // DQ (4/28/2004): Added support for restrict modifier
          if (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict())
             {
               cur << " restrict";
             }

       // DQ (4/28/2004): Added support for throw modifier
          if (mfuncdecl_stmt->get_declarationModifier().isThrow())
             {
            // Unparse SgThrow
            // unparseThrowExp(mfuncdecl_stmt->get_throwExpression,info);
               printf ("Incomplete implementation of throw specifier on function \n");
               cur << " throw( /* from unparseMFuncDeclStmt() type list output not implemented */ )";
             }
#endif

       // if (mfuncdecl_stmt->isPure())
          if (mfuncdecl_stmt->get_functionModifier().isPureVirtual())
             {
               cur << " = 0";
             }

          if (mfuncdecl_stmt->isForward() && !info.SkipSemiColon())
             {
               cur << ";";
               if (mfuncdecl_stmt->isExternBrace())
                  {
                    cur << " }";
                  }
             }
            else
             {
               int first = 1;
               SgInitializedNamePtrList::iterator p = mfuncdecl_stmt->get_ctors().begin();

               while (p != mfuncdecl_stmt->get_ctors().end())
                  {
                    if (first)
                       {
                         cur << " : ";
                         first = 0;
                       }
                      else
                       {
                         cur << ", ";
                       }
                    ROSE_ASSERT ((*p) != NULL);
                    cur <<  (*p)->get_name().str();

                    unparseExpression((*p)->get_initializer(), info);
                    p++;
                  }
             }
        }

  // DQ (1/23/03) Added option to support rewrite mechanism (generation of declarations)
     if (info.AddSemiColonAfterDeclaration())
        {
          cur << ";";
        }

  // Unparse any comments of directives attached to the SgCtorInitializerList
     if (mfuncdecl_stmt->get_CtorInitializerList() != NULL)
        unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), info, PreprocessingInfo::after);
   }

void
Unparser::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Inside of unparseVarDeclStmt(%p) \n",stmt);
  // cur << "/* Inside of unparseVarDeclStmt() */ \n";

     SgVariableDeclaration* vardecl_stmt = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(vardecl_stmt != NULL);

#if 0
     vardecl_stmt->get_declarationModifier().display("Called from unparseVarDeclStmt()");
#endif

#if 0
     printf ("In unparseVarDeclStmt() \n");
  // printf ("   isPrivate()         = %s \n",vardecl_stmt->isPrivate()         ? "true" : "false");
  // printf ("   isProtected()       = %s \n",vardecl_stmt->isProtected()       ? "true" : "false");
  // printf ("   isPublic()          = %s \n",vardecl_stmt->isPublic()          ? "true" : "false");
  // printf ("   isInline()          = %s \n",vardecl_stmt->isInline()          ? "true" : "false");
  // printf ("   isVirtual()         = %s \n",vardecl_stmt->isVirtual()         ? "true" : "false");
  // printf ("   isAtomic()          = %s \n",vardecl_stmt->isAtomic()          ? "true" : "false");
  // printf ("   isStatic()          = %s \n",vardecl_stmt->isStatic()          ? "true" : "false");
  // printf ("   isExtern()          = %s \n",vardecl_stmt->isExtern()          ? "true" : "false");
     printf ("   isExternBrace()     = %s \n",vardecl_stmt->isExternBrace()     ? "true" : "false");
  // printf ("   isAuto()            = %s \n",vardecl_stmt->isAuto()            ? "true" : "false");
  // printf ("   isRegister()        = %s \n",vardecl_stmt->isRegister()        ? "true" : "false");
  // printf ("   isFriend()          = %s \n",vardecl_stmt->isFriend()          ? "true" : "false");
  // printf ("   isGlobalClass()     = %s \n",vardecl_stmt->isGlobalClass()     ? "true" : "false");
  // printf ("   isConstructor()     = %s \n",vardecl_stmt->isConstructor()     ? "true" : "false");
  // printf ("   isDestructor()      = %s \n",vardecl_stmt->isDestructor()      ? "true" : "false");
  // printf ("   isConversion()      = %s \n",vardecl_stmt->isConversion()      ? "true" : "false");
     printf ("   isForward()         = %s \n",vardecl_stmt->isForward()         ? "true" : "false");
     printf ("   isNameOnly()        = %s \n",vardecl_stmt->isNameOnly()        ? "true" : "false");
  // printf ("   SkipElaborateType() = %s \n",vardecl_stmt->SkipElaborateType() ? "true" : "false");
  // printf ("   isOperator()        = %s \n",vardecl_stmt->isOperator()        ? "true" : "false");
  // printf ("   isPure()            = %s \n",vardecl_stmt->isPure()            ? "true" : "false");
  // printf ("   isSpecialization()  = %s \n",vardecl_stmt->isSpecialization()  ? "true" : "false");
  // printf ("   isSecondary()       = %s \n",vardecl_stmt->isSecondary()       ? "true" : "false");
#endif

  // 3 types of output
  //    var1=2, var2=3 (enum list)
  //    int var1=2, int var2=2 (arg list)
  //    int var1=2, var2=2 ; (vardecl list)
  // must also allow for this
  //    void (*set_foo)()=doo

  // Build a new SgUnparse_Info object to represent formatting options for this statement
     SgUnparse_Info ninfo(info);

  // ninfo.display ("At top of Unparser::unparseVarDeclStmt");

     int inClass = false;
     SgName inCname;

  // Check to see if this is an object defined within a class
     if (vardecl_stmt->get_parent() == NULL)
        {
          vardecl_stmt->get_file_info()->display("Found NULL parent for SgVariableDeclaration");
        }
     ROSE_ASSERT (vardecl_stmt->get_parent() != NULL);
     SgClassDefinition *cdefn = isSgClassDefinition(vardecl_stmt->get_parent());
     if (cdefn != NULL)
        {
          if(cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
               ninfo.set_CheckAccess();
          inClass = true;
          inCname = isSgClassDefinition(vardecl_stmt->get_parent())->get_declaration()->get_name();
        }

  // printf ("Calling printSpecifier1() \n");
     printDebugInfo("entering printSpecifier1", TRUE);
     printSpecifier1(vardecl_stmt, ninfo); 
  // printf ("DONE: Calling printSpecifier1() \n");

  // Save the input information
     SgUnparse_Info saved_ninfo(ninfo); 
  // this call has been moved below, after we indent
  // printSpecifier2(vardecl_stmt, ninfo);

  // Setup the SgUnparse_Info object for this statement
     ninfo.unset_CheckAccess();
     info.set_access_attribute(ninfo.get_access_attribute());
     SgInitializedNamePtrList::iterator p = vardecl_stmt->get_variables().begin();

     SgName tmp_name;
     SgType *tmp_type        = NULL;
  // SgPointerMemberType *pm_type = NULL;
     SgInitializer *tmp_init = NULL;

     SgInitializedName *decl_item      = NULL;
     SgInitializedName *prev_decl_item = NULL;

     cdefn = NULL;

     if (ninfo.inEnumDecl())
          ninfo.unset_isWithType();
       else
          ninfo.set_isWithType();

#define REWRITE_MACRO_HANDLING
#ifdef REWRITE_MACRO_HANDLING
  // DQ (9/17/2003) feature required for rewrite mechanism (macro handling)

  // Search for special coded declaration which are specific to the 
  // handling of MACROS in ROSE.  variable declarations of a specific form:
  //      char * rose_macro_declaration_1 = "<macro string>";
  // are unparsed as:
  //      macro string
  // to permit macros to be passed unevaluated through the transformation 
  // process (unevaluated in the intermediate files generated to process 
  // the transformations)

     if (p != vardecl_stmt->get_variables().end())
        {
          SgName cname;
          SgInitializedName *decl_item = *p;
          SgInitializer     *tmp_init  = NULL;

          SgName  tmp_name = decl_item->get_name();
          SgType *tmp_type = decl_item->get_type();

          if(!vardecl_stmt->isForward() && !isSgFunctionType(tmp_type))
             {
               ROSE_ASSERT (decl_item != NULL);
               //if (decl_item->get_named_item() != NULL)
                //    tmp_init = decl_item->get_named_item()->get_initializer();
               //else 
                    tmp_init = decl_item->get_initializer();  

               if (tmp_init != NULL) {
                    SgAssignInitializer *assignInitializer = isSgAssignInitializer(tmp_init);
                    if (assignInitializer != NULL)
                       {
                         SgStringVal *stringValueExpression = isSgStringVal(assignInitializer->get_operand());
                         if (stringValueExpression != NULL)
                            {
                              const char* targetString = "ROSE-MACRO-CALL:";
                              int targetStringLength = strlen(targetString);
                              if (strncmp(stringValueExpression->get_value(),targetString,targetStringLength) == 0)
                                 {
                                // unparse the string without the surrounding quotes and with a new line at the end
                                   char* remainingString = stringValueExpression->get_value()+targetStringLength;
                                // printf ("Specify a MACRO: remainingString = %s \n",remainingString);
                                   string tempRemainingString = remainingString;
                                   string quoteProcessedString = 
                                        StringUtility::copyEdit (tempRemainingString,"\\\"","\"");
                                // cur << "\n" << remainingString << "\n";
                                   cur << "\n" << quoteProcessedString.c_str() << "\n";
                                   return;
                                 }
                            }
                       }
                  }
             }
        }
#endif

  // DQ (10/5/2004): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current variable declaration (e.g. struct A { int x; } a;).  In this case we have to output the base type with
  // its definition.
     bool outputTypeDefinition = vardecl_stmt->get_variableDeclarationContainsBaseTypeDefiningDeclaration();
  // printf ("outputTypeDefinition = %s \n",(outputTypeDefinition == true) ? "true" : "false");

     if (p != vardecl_stmt->get_variables().end())
        {
          bool first = true;

          while(true)
             {
               SgName cname;
               decl_item = *p;

            // printf ("In unparseVarDeclStmt(): cname = decl_item->get_name() = %s \n",decl_item->get_name().str());

               tmp_name = decl_item->get_name();
               tmp_type = decl_item->get_type();

            // printf ("tmp_type = %p = %s \n",tmp_type,tmp_type->sage_class_name());

#if 1
               SgClassType* classType = isSgClassType(tmp_type);
            // printf ("classType = %p \n",classType);
               if (classType != NULL)
                  {
                    ROSE_ASSERT(classType->get_declaration() != NULL);
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                    ROSE_ASSERT(classDeclaration != NULL);
                 // printf ("classDeclaration = %p isForward = %s \n",classDeclaration,(classDeclaration->isForward() == true) ? "true" : "false");
                  }
#endif

            // XXX this should not be here,
            // from the definition of sage2 struction, forward function
            // declaration should not mix with regular variable like this
            // int x, *y, f(int); 
            // But, so far, it is possible to construct such structure
            // but information as to it is a SgFunctionDeclaration is
            // lost in the code

            // printf ("vardecl_stmt->isForward()  = %s \n",(vardecl_stmt->isForward()  == true) ? "true" : "false");
            // printf ("isSgFunctionType(tmp_type) = %s \n",(isSgFunctionType(tmp_type) != NULL) ? "true" : "false");
               if(!vardecl_stmt->isForward() && !isSgFunctionType(tmp_type))
                  {
                    ROSE_ASSERT (decl_item != NULL);
                    //if (decl_item->get_named_item() != NULL) 
                    //     tmp_init = decl_item->get_named_item()->get_initializer();
                    //else
                         tmp_init = decl_item->get_initializer();

                    prev_decl_item = decl_item->get_prev_decl_item();
                  }

            // printf ("prev_decl_item = %p \n",prev_decl_item);

            /* exception handling : for ptr_to_class_data (::) */
            /* if it is a class data and also a static and not in the
               class scope, might need this 'int P::pvar=10;' */
               if (prev_decl_item != NULL)
                  {
                    SgDeclarationStatement *decl = prev_decl_item->get_declaration();
#if 0
                    SgDeclarationStatement *decl = NULL;
                    if (outputTypeDefinition == true)
                       {
                      // printf ("prev_decl_item != NULL: Use the defining declaration as a basis for the variable declaration \n");
                         decl = prev_decl_item->get_declaration()->get_definingDeclaration();
                         ROSE_ASSERT(decl != NULL);
                       }
                      else
                       {
                      // printf ("prev_decl_item != NULL: Use what is surely a non-defining declaration as a basis for the variable declaration \n");
                         decl = prev_decl_item->get_declaration();
                         ROSE_ASSERT(decl != NULL);
                       }
#endif
                 // printf ("decl = %p \n",decl);

                 /* also check for case where decl is defined in a vardecl */
                 // if (decl && decl->isStatic())
                    if (decl && decl->get_declarationModifier().get_storageModifier().isStatic())
                       {
                         int v = GLOBAL_STMT;
                      // SgStatement *vparent = decl->get_parent();
                         ROSE_ASSERT (decl->get_parent() != NULL);
                         SgStatement *vparent = isSgStatement(decl->get_parent());
                         if (vparent)
                              v=vparent->variant();
                         if (v==VAR_DECL_STMT || v==TYPEDEF_STMT)
                            {
                              ROSE_ASSERT (vparent->get_parent() != NULL);
                              cdefn=isSgClassDefinition(vparent->get_parent());
                            }
                           else
                              if( v==CLASS_DEFN_STMT)
                                   cdefn=isSgClassDefinition(vparent);
                       }
	
                    if(cdefn && (!inClass || (inClass && cdefn->get_declaration()->get_name()!=inCname)))
                       {
                         cname = cdefn->get_declaration()->get_qualified_name() << "::";
                       }
                  }

               cname << tmp_name.str();

            // printf ("cname = %s \n",cname.str());

               SgNamedType *namedType = isSgNamedType(tmp_type->findBaseType());
            // SgDeclarationStatement* declStmt = (namedType) ? namedType->get_declaration() : NULL;
               SgDeclarationStatement* declStmt = NULL;

            // printf ("namedType = %p \n",namedType);
               if (namedType != NULL)
                  {
                 // DQ (10/5/2004): This controls the unparsing of the class definition
                 // when unparsing the type within this variable declaration.
                    if (outputTypeDefinition == true)
                       {
                      // printf ("Use the defining declaration as a basis for the variable declaration \n");
                         ROSE_ASSERT(namedType->get_declaration() != NULL);
                         declStmt = namedType->get_declaration()->get_definingDeclaration();
                      // printf ("outputTypeDefinition == true: declStmt = %p \n",declStmt);
                         if (declStmt == NULL)
                            {
#if 0
                              printf ("namedType->get_declaration() = %p = %s \n",
                                   namedType->get_declaration(),namedType->get_declaration()->sage_class_name());
#endif
                           // it is likely an enum declaration which does not yet use the defing vs. non-defining mechanisms
                           // so just set it to the currently available declaration (since for enums there is only one
                              declStmt = namedType->get_declaration();
                            }
                         ROSE_ASSERT(declStmt != NULL);
                         ROSE_ASSERT(declStmt->isForward() == false);

                      // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
                         ROSE_ASSERT(ninfo.SkipClassDefinition() == false);
                         ROSE_ASSERT(ninfo.SkipEnumDefinition()  == false);
                         ROSE_ASSERT(ninfo.SkipDefinition()      == false);
                       }
                      else
                       {
                      // printf ("Use what is surely a non-defining declaration as a basis for the variable declaration \n");
                         declStmt = namedType->get_declaration();
                         ROSE_ASSERT(declStmt != NULL);
                      // printf ("outputTypeDefinition == false: declStmt = %p \n",declStmt);
                      // Only implement test for class declarations
                         if (isSgClassDeclaration(declStmt) != NULL)
                            {
                           // DQ (10/8/2004): If this is not a enum then expect a forward declaration!
                           // Enum declarations are an exception since forward declarations of enums 
                           // are not permitted in C and C++ according to the standard!
                              if (declStmt->isForward() == false)
                                 {
                                   printf ("Warning about declStmt = %p = %s \n",declStmt,declStmt->sage_class_name());
                                   declStmt->get_file_info()->display("Warning: declStmt->isForward() == false (should be true)");
                                 }
                              ROSE_ASSERT(declStmt->isForward() == true);
                            }

                      // DQ (10/5/2004): If this is a non-defining declaration then skip the definition
                      // ninfo.set_SkipClassDefinition();
                         ninfo.set_SkipDefinition();
                         ROSE_ASSERT(ninfo.SkipClassDefinition() == true);
                         ROSE_ASSERT(ninfo.SkipEnumDefinition() == true);
                       }
                  }

            // printf ("declStmt = %p \n",declStmt);

            // printf ("Calling printSpecifier2 \n");
               printDebugInfo("entering printSpecifier2", TRUE);
               printSpecifier2(vardecl_stmt, saved_ninfo);

#if 0
               printf ("first = %s \n",(first == true) ? "true" : "false");
               printf ("declStmt = %p \n",declStmt);
               printf ("isSgTypedefDeclaration(declStmt) = %p \n",isSgTypedefDeclaration(declStmt));
#endif
            // XXX have to recheck this once the front end can do multiple variable
            // within a single statement
            // if ( (first == true) && (declStmt != NULL) && (!isSgTypedefDeclaration(declStmt) != NULL) )
               if ( (first == true) && (declStmt != NULL) && (isSgTypedefDeclaration(declStmt) == NULL) )
                  {
                    SgUnparse_Info ninfo2(ninfo);

                 // printf ("At TOP: ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");

#if 0
                 // DQ (9/28/2004): Now that we have explicit defining and nondefining 
                 // declarations we don't need this, it is redundant and wrong!

                 // DQ (5/6/03) Comment this out since it causes the unparsing
                 // of the member fields in a class to skip the ";" (is it needed?)
                 // ninfo2.set_SkipSemiColon();
                    printf ("At TOP: vardecl_stmt->isNameOnly() = %s \n",(vardecl_stmt->isNameOnly() == true) ? "true" : "false");
                    if (vardecl_stmt->isNameOnly())
                       {
                         ninfo2.set_SkipDefinition();
                       }
#endif

                 // if (vardecl_stmt->SkipElaborateType())
                    if (vardecl_stmt->skipElaborateType())
                       {
                         ninfo2.set_SkipClassSpecifier();
                       }

                 // printf ("After isNameOnly() test: ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");

                 // Don't know why is this here
                 // SgClassDeclaration* classDecl = isSgClassDeclaration(declStmt);

// Turned this off to test fix to anonymous typedef
// #if ANONYMOUS_TYPEDEF_FIX
#if 0
                    if (classDecl != NULL)
                       {
                      // This might help handle the case of "struct a { int b; } c;"
                         printf ("Case of a class declaration (get the definition so it can be unparsed) \n");

                      // this piece of code should get us the non-forward declaration(if
                      // there is one... otherwise it gives us a forward declaration).
                         ROSE_ASSERT(classDecl->get_definition());
                         declStmt = classDecl->get_definition()->get_declaration();
                       }
#endif

#if 0
                 // DQ (9/28/2004): Now that we have defining and non-defining declarations we should be able to simplify this code!


                 // DQ (3/25/2003)
                 // Do avoid infinite recursion in processing:
                 // struct Xtag { Xtag* next; } X;
                 // by detecting the use of structure tags that don't require unparsing the structures
                 // All structure tags are places into a list and the list is searched, if the tag is found
                 // then unparsing the type representing the tag is only unparsed to generate the type name
                 // only (not the full class definition).
                    bool structureTagBeingProcessed = true;
                    if ( (classDecl != NULL) &&
                         ( (namedType != NULL) &&
                           (namedType->get_autonomous_declaration() == FALSE) ) )
                       {
                         structureTagBeingProcessed = find(ninfo2.getStructureTagList().begin(),
                                                           ninfo2.getStructureTagList().end(),namedType)
                                                      != ninfo2.getStructureTagList().end();

                      // Only add the tag if it is NOT being processed
                         if (structureTagBeingProcessed == FALSE)
                            {
                           // debugging (structures requiring this depth are likely an internal error)
                              ROSE_ASSERT (ninfo2.getStructureTagList().size() < 100);

                              ninfo2.addStructureTag(namedType);
                            }
                       }

                    SgUnparse_Info ninfo3(ninfo2);
                    ninfo3.set_inEmbeddedDecl();

                 // DQ (9/6/2004): Set the list of scopes required to qualify the name when output
                 // ninfo3.set_listOfScopeSymbols(vardecl_stmt->get_name_qualifier_list());

                 // DQ (3/18/2004): Force output of class template names
                    ninfo3.set_outputClassTemplateName();

                 // Need to check if the structure tag has been used previously in the definition of any
                 // current autonomous class being defined (that we are currently unparsing).
                 // If it is, then we don't want to unparse the class definition again since this leads to
                 // infinite recursion.  The best fix is to add a statck in the inherited attribute 
                 // (SgUnparse_Info) and place the qualified name of the structure tags 
                 // there and test the type name of any variable declaration against this list 
                 // to see if it has already been defined if it is not an autonomous declaration.
                 // An alternative might be to use a list of pointers tto types (if types are 
                 // ALWAYS shared).

                    printf ("START: calling unparseStatement \n");
                    cur << "\n/* START: output using statementInfo */ \n";

                 // Testing the output of the class definition instead of the class declaration
                    if ( (classDecl != NULL) &&
                         ( (namedType != NULL) && 
                           (namedType->get_autonomous_declaration() == FALSE) ) )
                       {
                         cur << "\n/* branch: classDecl != NULL */ \n";
                         if (structureTagBeingProcessed == false)
                            {
                              cur << "\n/* branch: structureTagBeingProcessed == false */ \n";
                           // SgUnparse_Info statementInfo;
                              SgUnparse_Info statementInfo(ninfo3);

                           // DQ (7/19/2003) skip the semi-colon in "struct XYZ; * abc;"
                           // Might need to check if this appears in the right position before skipping the semi-colon
//                            statementInfo.set_SkipSemiColon();

#if 0
                              printf ("Calling the default SgUnparse_Info constructor needs to be fixed \n");
                              ROSE_ABORT();
#endif
                              unparseStatement(classDecl->get_definition(), statementInfo);
                              cur << "\n/* branch: DONE: unparseStatemente */ \n";
                            }
                           else
                            {
                              cur << "\n/* branch: structureTagBeingProcessed == true */ \n";
                              SgUnparse_Info statementInfo(ninfo3);
                              statementInfo.display("statementInfo in unparsing of variable declaration");
                              statementInfo.set_SkipClassDefinition();

                           // DQ (7/19/2003) skip the semi-colon in "struct XYZ; * abc;"
                           // Might need to check if this appears in the right position before skipping the semi-colon
                              statementInfo.set_SkipSemiColon();

                           // statementInfo.getStructureTagList() = ninfo3.getStructureTagList();
                              cur << "\n/* START: output using statementInfo */ \n";
                              unparseStatement(classDecl, statementInfo);
                              cur << "\n/* END: output using statementInfo */ \n";
                            }
                       }
                      else
                       {
                         cur << "\n/* branch: classDecl == NULL */ \n";
                         printf ("Generic case of calling unparseStatement \n");
                         SgEnumDeclaration* enumDecl   = isSgEnumDeclaration(declStmt);
                         SgClassDeclaration* classDecl  = isSgClassDeclaration(declStmt);
                         if ( (enumDecl != NULL) || (classDecl != NULL) )
                            {
                           // cur << "\n/* branch: skip semi-colon */ \n";
                           // printf ("Skip the training semi-colon \n");
                              ninfo3.set_SkipSemiColon();
                            }
                           else
                            {
                           // cur << "\n/* branch: output semi-colon */ \n";
                            }

                         unparseStatement(declStmt, ninfo3);
                       }

                    cur << "\n/* END: output using statementInfo */ \n";
                    printf ("END: calling unparseStatement \n");

                    ninfo2.set_SkipBaseType(); // base is done in above stmt
#endif

                    ninfo2.set_isTypeFirstPart();

                 // printf ("ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");
                 // cur << "\n/* START: output using unparseType (1st part) */ \n";
                 // printf ("Calling 1st part of unparseType for %s \n",tmp_type->sage_class_name());

                    unparseType(tmp_type, ninfo2);

                 // cur << "\n/* END: output using unparseType (1st part) */ \n";
                 // printf ("In unparseVarDeclStmt(): cname = %s \n",cname.str() == NULL ? "NULL" : cname.str());

#if 1
                 // DQ (10/6/2004): Changed this back to the previous ordering so that we could handle test2004_104.C
                 // DQ (9/28/2004): Output the variable name after the first and second parts of the type!
                    if (cname.str() != NULL)
                         cur <<  cname.str();
#endif

                    ninfo2.set_isTypeSecondPart();

                 // cur << "\n/* START: output using unparseType (2nd part) */ \n";
                 // printf ("Calling 2nd part of unparseType for %s \n",tmp_type->sage_class_name());
                 // printf ("ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");

#if 0
                 // Example of what the 2nd part of the type is (for static declaration):
                 // Within "X *X::staticArray [10];" the "[10]" is the 2nd part!
                    SgNamedType* tempNamedType = isSgNamedType(tmp_type);
                    if ( (tempNamedType != NULL) && (tempNamedType->get_autonomous_declaration() == FALSE) )
                       {
                      // Need to generate the "{ int b; }" before
                      // the name and to generate: struct { int b; } B;
                      // printf ("&&&&& Skipping the call to unparse the 2nd part: unparseType(tmp_type, ninfo2); \n");
                      // unparseType(tmp_type, ninfo2);
                       }
                      else
                       {
                         if (structureTagBeingProcessed == false)
                            {
                           // printf ("Calling unparseType for %s \n",tmp_type->sage_class_name());
                              unparseType(tmp_type, ninfo2);
                            }
                           else
                            {
                           // Unparsing the type here would cause it to be generated after
                           // the name and would generate: struct { int b; } B { int b; };
                           // But for an array type we have to unparse the second part of the
                           // type as in: class Btag { static Btag* b[10]; }; Btag* B::b[10];
                           // The "[10]" represents the 2nd part of the type in "Btag* B::b[10];"
                              if (isSgArrayType(tmp_type) != NULL)
                                   unparseType(tmp_type, ninfo2);
                            }
                       }
#else
#if 1
                 // DQ (9/28/2004): Now that we have defining and non-defining declarations we should be able to simplify this code!
                    if (isSgArrayType(tmp_type) != NULL)
                       {
                      // printf ("This is an array type so use special handling ... \n");
                         unparseType(tmp_type, ninfo2);
                       }
#else
                    unparseType(tmp_type, ninfo2);
#endif
#endif

#if 0
                 // DQ (10/6/2004): Changed this back to the previous ordering so that we could handle test2004_104.C
                 // DQ (9/28/2004): Output the variable name after the first and second parts of the type!
                    if (cname.str() != NULL)
                         cur <<  cname.str();
#endif
                 // cur << "\n/* END: output using unparseType (2nd part) */ \n";
                  }
                 else
                  {
                 // Handle the remaining declared variables in this branch
                 // printf ("Handle the remaining declared variables in this branch: tmp_type = %p \n",tmp_type);
                    if (tmp_type != NULL)
                       {
                      // printf ("tmp_type = %p = %s namedType = %p \n",tmp_type,tmp_type->sage_class_name(),namedType);
#if 0
                      // DQ (10/15/2004): If we let the unparseTypedefType output the qualified name this this code becomes redundent!
                         if (namedType != NULL)
                            {
                              printf ("Call namedType->get_qualified_name() \n");
                           // SgName theName = namedType->get_qualified_name();
                              SgName theName = namedType->get_declaration()->get_scope()->get_qualified_name();
                              printf ("DONE: Call namedType->get_qualified_name() = %s \n",theName.str());

                              if (!theName.is_null())
                                 {
                                   printf ("namedType = %p = %s theName = %s \n",namedType,namedType->sage_class_name(),theName.str());
                                   cur <<  theName.str() << "::";
                                 }
                                else
                                 {
                                // DQ (10/14/2004): If the qualified name is empty then the non qualified name should be empty too!
                                   printf ("theName.is_null() == true \n");
                                   SgName nonQualifiedName = namedType->get_name();
                                   printf ("nonQualifiedName = %s \n",nonQualifiedName.is_null() ? "EMPTY NONQUALIFIED NAME" : nonQualifiedName.str());
                                   ROSE_ASSERT(nonQualifiedName.is_null() == true);
                                 }
                            }
#endif
                         ninfo.set_isTypeFirstPart();
#if 0
                         printf ("Handle the remaining declared variables: calling unparseType for 1st part %s \n",
                              tmp_type->sage_class_name());
#endif
                         unparseType(tmp_type, ninfo);

                      // DQ (1/19/2004): test2003_30.C demonstrates an error here (null name string)
                         if (cname.str() != NULL)
                            {
                           // There is a valid name string so put it out
                              cur <<  cname.str();
                            }
                           else
                            {
                           // DQ (9/19/2004): Added support for type "..." (which fixes a bug in test2004_91.C)
                           // There is no valid name string in this case which is OK, since this might be 
                           // a declaration using the SgTypeEllipse (e.g. in a "catch(...) {}" statement or 
                           // an unnamed field in a structure (e.g. "struct X { bool b; unsigned int :16; };")
                           // cur <<  "NO_NAME_FOUND_IN_UNPARSER";
                            }

#if 0
                         printf ("Handle the remaining declared variables: calling unparseType for 2nd part %s \n",
                              tmp_type->sage_class_name());
#endif
                         ninfo.set_isTypeSecondPart();
                         unparseType(tmp_type, ninfo);
                       }
                      else
                       {
                         cur <<  cname.str() ;
                       }
                  }


            // Mark that we are no longer processing the first entry 
            // (first variable in a declaration containing multiple "," separated names)
               first = false;

            // cur << "\n/* Handle initializers (if any) */ \n";

            // Unparse the initializers if any exist
            // if(tmp_init)
               if ( (tmp_init != NULL) && !ninfo.SkipInitializer())
                  {
                    if ( tmp_init->variant() == ASSIGN_INIT || tmp_init->variant() == AGGREGATE_INIT )
                       {
                         cur << " = ";
                       }

                    SgUnparse_Info statementInfo(ninfo);
                    statementInfo.set_SkipClassDefinition();

                 // statementInfo.display("Debugging the initializer (set SkipClassDefinition");

                 // printf ("Unparse the initializer \n");
                 // cur << "/* Unparse the initializer */ \n";
                 // unparseExpression(tmp_init, ninfo);
                    unparseExpression(tmp_init, statementInfo);
                 // printf ("DONE: Unparse the initializer \n");
                 // cur << "/* DONE: Unparse the initializer */ \n";
                  }

               p++;

               if (p != vardecl_stmt->get_variables().end())
                  {
                    if (!ninfo.inArgList())
                         ninfo.set_SkipBaseType();
                    cur << ",";
                  }
                 else
                    break;
             }
        }

  // cur << "\n/* Handle bit fields (if any) */ \n";

  // Bit fields appear as "a_bit_field autonomous_tag_decl:1;"
     SgVariableDefinition *defn = vardecl_stmt->get_definition();
     if (defn != NULL)
        {
          SgUnsignedLongVal *bitfield = vardecl_stmt->get_definition()->get_bitfield();
          if (bitfield != NULL)
             {
               cur << ":";
               unparseExpression(bitfield, ninfo);
             }
        }

  // ninfo.display("Close off the statement with a \";\"?");

  // cur << "\n/* START: Close off the statement with a \";\" and brace */ \n";

  // Close off the statement with a ";"
     if(!ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.SkipSemiColon())
        {
          cur << ";";
#if 0
       // DQ (4/29/2004): Removed to fix bug
          if (vardecl_stmt->isExternBrace())
             {
               cur <<  " }";
             }
#endif
        }

  // cur << "\n/* END: Close off the statement with a \";\" and brace */ \n";

  // printf ("Leaving unparseVarDeclStmt() \n");
  // cur << "/* Leaving unparseVarDeclStmt() */ \n";
   }

void
Unparser::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
     ROSE_ASSERT(vardefn_stmt != NULL);

  // DQ: (9/17/2003)
  // Although I have not seen it in any of our tests of ROSE the SgVariableDefinition
  // does appear to be used in the declaration of bit fields!  Note the comment at the
  // end of the unparseVarDeclStmt() function where the bit field is unparsed! Though 
  // it appears that the unparseVarDefnStmt is not required to the unparsing of the 
  // bit field, so this function is never called!

  // DQ: added 3/25/2001
     printf("unparseVarDefnStmt not implemented (should it be?) \n");
     ROSE_ABORT();

  // assert(0);
   }

#if 0
void
Unparser::unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info)
   {
     ROSE_ASSERT (templateInstantiationDeclaration != NULL);

     cur << templateInstantiationDeclaration->get_templateName().str();
     SgTemplateArgumentPtrListPtr templateArgListPtr = templateInstantiationDeclaration->get_templateArguments();
     if (templateArgListPtr != NULL)
        {
       // printf ("templateArgListPtr->size() = %d \n",templateArgListPtr->size());
          cur << "< ";
          SgTemplateArgumentPtrList::const_iterator i = templateArgListPtr->begin();
          while (i != templateArgListPtr->end())
             {
            // printf ("templateArgList element *i = %s \n",(*i)->sage_class_name());
               unparseTemplateArgument(*i,info);
               i++;
               if (i != templateArgListPtr->end())
                    cur << " , ";
             }
          cur << " > ";
        }
   }
#endif



void
Unparser::initializeDeclarationsFromParent ( 
   SgDeclarationStatement* declarationStatement,
   SgClassDefinition* & cdefn,
   SgNamespaceDefinitionStatement* & namespaceDefn )
   {
  // DQ (10/14/2004): This function represents a refactoring of code to support the output of class declarations 
  // within unparseClassDeclStmt() and unparseClassType().  The purpose is to determine the enclosing scope of the
  // declaration so that we can decide if name qualification is required.

  // DQ (9/10/2004): Error checking
     if (declarationStatement->get_parent() == NULL)
        {
          printf ("In unparseClassDeclaration: declarationStatement is a %s \n",declarationStatement->sage_class_name());
          declarationStatement->get_file_info()->display("location of classdecl_stmt");
        }
     ROSE_ASSERT (declarationStatement->get_parent() != NULL);

  // printf ("classdecl_stmt->get_parent() = %s \n",classdecl_stmt->get_parent()->sage_class_name());
     SgStatement *cparent = isSgStatement(declarationStatement->get_parent());

     ROSE_ASSERT (cparent != NULL);

  // printf ("In Unparser::initializeDeclarationsFromParent(): cparent = %p = %s \n",cparent,cparent->sage_class_name());

  // I don't think that we need this case!
     if (cparent == NULL)
        {
       // DQ (7/20/2004): This demonates a current bug in the EDG/Sage 
       // connection (I think) so leave this message in here.
          printf ("Warning: In unparseClassDeclStmt classdecl_stmt->get_parent() = %s \n",
               declarationStatement->get_parent()->sage_class_name());
          cdefn         = NULL;
          namespaceDefn = NULL;
       // ROSE_ASSERT (false);
        }
       else
        {
          switch(cparent->variantT())
             {
               case V_SgGlobal:
                  {
                 // This is nothing to do in this case (inless we wanted to insert "::" everywhere!
                 // printf ("This is in global scope: cparent = %s \n",cparent->sage_class_name());
                    ROSE_ASSERT(cdefn == NULL);
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                 // This is the case of a type declaration hidden in a typedef declaration (e.g. "typedef struct X xType;") hidden
                 // in a class definition or a namespace!

                 // DQ (9/9/2004): I previously thought this case was never visited, but this is incorrect!
                 // This case is used to handle "typedef doubleArray doubleSerialArray;" in A++_notemplates.h with test2001_12.C

                 // printf ("In unparseClassDeclStmt(): Case of searching for class definition in a typedef declaration \n");
                    ROSE_ASSERT (cparent->get_parent() != NULL);
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(cparent);
                    ROSE_ASSERT (typedefDeclaration != NULL);
                    ROSE_ASSERT (typedefDeclaration->get_name().str() != NULL);
                 // printf ("typedefDeclaration->get_name() = %s \n",typedefDeclaration->get_name().str());
                 // printf ("In unparseClassDeclStmt(): cparent = %s \n",cparent->sage_class_name());
                 // printf ("In unparseClassDeclStmt(): cparent->get_parent() = %s \n",cparent->get_parent()->sage_class_name());
                 // cdefn = isSgClassDefinition(cparent->get_parent());

                 // DQ (10/16/2004): When the parent of the declaration is a SgTypedefDeclaration then get the scope 
                 // of the typedef and see if it is a SgClassDefinition or SgNamespaceDefinitionStatement and initialize 
                 // the approporiate input reference pointer.
                    SgScopeStatement* scope = typedefDeclaration->get_scope();
                    cdefn         = isSgClassDefinition(scope);
                    namespaceDefn = isSgNamespaceDefinitionStatement(scope);
                 // printf ("SgTypedefDeclaration case: cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
                    break;
                  }

               case V_SgVariableDeclaration:
                  {
                 // This is the case of a type declaration hidden in a variable declaration (e.g. "struct X *x;") hidden
                 // in a class definition or a namespace! Note that the definition of the struct is not required.

                 // printf ("In unparseClassDeclStmt(): Case of searching for class definition in a variable declaration \n");

                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(cparent);
                    ROSE_ASSERT (variableDeclaration != NULL);

                    ROSE_ASSERT (cparent->get_parent() != NULL);
                 // printf ("In unparseClassDeclStmt(): cparent = %s \n",cparent->sage_class_name());
                 // printf ("In unparseClassDeclStmt(): cparent->get_parent() = %s \n",cparent->get_parent()->sage_class_name());
                 // cdefn = isSgClassDefinition(cparent->get_parent());

                 // DQ (10/16/2004): When the parent of the declaration is a SgTypedefDeclaration then get the scope 
                 // of the typedef and see if it is a SgClassDefinition or SgNamespaceDefinitionStatement and initialize 
                 // the approporiate input reference pointer.
                    SgScopeStatement* scope = variableDeclaration->get_scope();
                    cdefn         = isSgClassDefinition(scope);
                    namespaceDefn = isSgNamespaceDefinitionStatement(scope);
                 // printf ("SgVariableDeclaration case: cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);

#if 0
                 // DQ (9/28/2004): this fails for test2003_06.C and I'm not sure that we should assert that it is true!
                    ROSE_ASSERT (cdefn != NULL);
                    ROSE_ASSERT (cdefn->get_declaration() != NULL);
                    ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
#endif
                    break;
                  }

            // DQ (10/19/2004): Added case of V_SgTemplateInstantiationDefn
               case V_SgTemplateInstantiationDefn:
               case V_SgClassDefinition:
                  {
                 // This is the trival case: here the type declaration is not hidden inside of a typedef or variable declaration
                    cdefn = isSgClassDefinition(cparent);
                    ROSE_ASSERT (cdefn != NULL);
                    ROSE_ASSERT (cdefn->get_declaration() != NULL);
                 // ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
                    break;
                  }

            // DQ (9/8/2004): Added namespace support!
               case V_SgNamespaceDefinitionStatement:
                  {
                 // This is the trival case: here the type declaration is not hidden inside of a typedef or variable declaration
                    namespaceDefn = isSgNamespaceDefinitionStatement(cparent);
                    ROSE_ASSERT (cdefn == NULL);
                    ROSE_ASSERT (namespaceDefn != NULL);
                    ROSE_ASSERT (namespaceDefn->get_namespaceDeclaration() != NULL);
                 // ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
                    break;
                  }

            // DQ (10/17/2004): Added this case, but I am not sure it should be required!
               case V_SgBasicBlock:
                  {
                 // This is nothing to do in this case!
                    printf ("This is in basic block scope: cparent = %s \n",cparent->sage_class_name());
                    ROSE_ASSERT(cdefn == NULL);
                    break;
                  }


            // DQ (10/17/2004): Added this case, but I am not sure it should be required!
               case V_SgFunctionParameterList:
                  {
                 // This is nothing to do in this case!
                    printf ("This is a SgFunctionParameterList scope: cparent = %s \n",cparent->sage_class_name());
                    ROSE_ASSERT(cdefn == NULL);
                    break;
                  }

               default:
                  {
                 // I don't think there any any other places where type declarations can be hidden.
                    printf ("In initializeDeclarationsFromParent(): Default case = %s \n",cparent->sage_class_name());
                    ROSE_ASSERT(false);
                    break;
                  }
             }
        }
   }


void
Unparser::unparseClassDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDeclaration* classdecl_stmt = isSgClassDeclaration(stmt);
     ROSE_ASSERT(classdecl_stmt != NULL);

#if 0
     printf ("Inside of Unparser::unparseClassDeclStmt(): classdecl_stmt->get_from_template() = %s \n",
          classdecl_stmt->get_from_template() ? "true" : "false");
     if (classdecl_stmt->get_from_template() == true)
          cur << "/* Unparser comment: Templated Class Declaration Function */";
     Sg_File_Info* classDeclarationfileInfo = classdecl_stmt->get_file_info();
     ROSE_ASSERT ( classDeclarationfileInfo != NULL );
     cur << "\n/* file: " << classDeclarationfileInfo->get_filename() << " line: " << classDeclarationfileInfo->get_line() << " col: " << classDeclarationfileInfo->get_col() << " */ \n";
#endif

  // info.display("Inside of unparseClassDeclStmt");

  // printf ("At top of unparseClassDeclStmt name = %s \n",classdecl_stmt->get_name().str());

#if 0
  /* EXCEPTION HANDLING: Forward Class Declarations */ 
  // DO NOT use the file information sage gives us because the line information
  // refers to the class definition (if it is defined in the same file). Instead, 
  // the line of the forward declaration is set one after the line number of the 
  // previous node or directive. By doing this, any comments around the forward 
  // declaration will come after the declaration, since I'm setting the line number 
  // to be right after the previous thing we unparsed.
  
  // the following is a HACK. I want to know if this statement is in a header or
  // C++ file. If it is in a header file, then I proceed as normal. If the
  // statement is in a C++ file and is a forward declaration, then I must 
  // follow this HACK. When the Sage bug of forward declaration is fixed, remove
  // this code.
     if ( classdecl_stmt->get_file_info() != NULL ) // [DT] 3/13/2000
        {
          if (strstr(classdecl_stmt->get_file_info()->get_filename(), ".C") != NULL)
             {
               if (classdecl_stmt->isForward())
                  {
#ifndef UNPARSER_IS_READ_ONLY
                    if (prevnode == NULL)
                       {
                         if (prevdir_line_num > 0) 
                              classdecl_stmt->get_file_info()->set_line(prevdir_line_num + 1);
                      // place this at the top (line 1) 
                           else
                              classdecl_stmt->get_file_info()->set_line(1); 
                       }
                      else
                       {
                         if (prevnode->get_file_info()->get_line() > prevdir_line_num) 
                              classdecl_stmt->get_file_info()->set_line(prevnode->get_file_info()->get_line() + 1);
                           else
                              classdecl_stmt->get_file_info()->set_line(prevdir_line_num + 1);
                       }
#endif
                  }
             }
        }
  /* EXCEPTION HANDLING: Forward Class Declarations */ 
#endif

#if 0
     printf ("In Unparser::unparseClassDeclStmt(): classdecl_stmt = %p isForward() = %s info.SkipClassDefinition() = %s name = %s \n",
          classdecl_stmt,(classdecl_stmt->isForward() == true) ? "true" : "false",
          (info.SkipClassDefinition() == true) ? "true" : "false",classdecl_stmt->get_name().str());
#endif

     if (!classdecl_stmt->isForward() && classdecl_stmt->get_definition() && !info.SkipClassDefinition())
        {
          SgUnparse_Info ninfox(info);

          ninfox.unset_SkipSemiColon();

       // printf ("Calling unparseStatement(classdecl_stmt->get_definition(), ninfox); for %s \n",classdecl_stmt->get_name().str());
          unparseStatement(classdecl_stmt->get_definition(), ninfox);

          if (!info.SkipSemiColon())
             {
               cur <<  ";";
             }
        }
       else
        {
          if (!info.inEmbeddedDecl())
             {
               SgUnparse_Info ninfo(info);
               if (classdecl_stmt->get_parent() == NULL)
                  {
                    printf ("classdecl_stmt->isForward() = %s \n",(classdecl_stmt->isForward() == true) ? "true" : "false");
                  }
               ROSE_ASSERT (classdecl_stmt->get_parent() != NULL);
               SgClassDefinition *cdefn = isSgClassDefinition(classdecl_stmt->get_parent());

               if(cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
                    ninfo.set_CheckAccess();

            // DQ (8/19/2004): Removed functions using old attribute mechanism (old CC++ mechanism)
            // printf ("Commented out get_suppress_global(classdecl_stmt) \n");
            // if (get_suppress_global(classdecl_stmt))
            //      ninfo.set_SkipGlobal(); //attributes.h

               printDebugInfo("entering printSpecifier", TRUE);
               printSpecifier(classdecl_stmt, ninfo);
               info.set_access_attribute(ninfo.get_access_attribute());
             }

          info.unset_inEmbeddedDecl();

          if (!info.SkipClassSpecifier())
             {
               switch (classdecl_stmt->get_class_type()) 
                  {
                    case SgClassDeclaration::e_class : 
                       {
                         cur << "class ";
                         break;
                       }
                    case SgClassDeclaration::e_struct :
                       {
                         cur << "struct ";
                         break;
                       }
                    case SgClassDeclaration::e_union :
                       {
                         cur << "union ";
                         break;
                       }
                  }
              }

       /* have to make sure if it needs qualifier or not */

       // since this declaration might be a friend or forward declaration,
       // we need to get its real "declaration" via its p_definition  
          SgClassDefinition  *cdefn = NULL;

       // DQ (9/8/2004): Added these two assertions
          ROSE_ASSERT(classdecl_stmt->get_definition() != NULL);
          ROSE_ASSERT(classdecl_stmt->get_definition()->get_declaration() != NULL);
          SgClassDeclaration *cdecl = classdecl_stmt->get_definition()->get_declaration();
//        SgClassDeclaration *cdecl = NULL;
//        if (classdecl_stmt->get_definition() != NULL)
//             cdecl = classdecl_stmt->get_definition()->get_declaration();

       // DQ (9/8/2004): Added namespace support!
          SgNamespaceDefinitionStatement  *namespaceDefn = NULL;
       // SgNamespaceDeclarationStatement *namespaceDecl = classdecl_stmt->get_definition()->get_declaration();
#if 0
          printf ("Figure out if we are in a namespace, if so, then we need a qualifier! \n");
          printf ("classdecl_stmt = %p  cdecl = %p namespaceDefn = %p \n",classdecl_stmt,cdecl,namespaceDefn);
#endif
       // This test is independent of if the class declaration occurs in a namespace!
          if (classdecl_stmt != cdecl)
             {
            // This is not the defining declaration
               printf ("This is not the defining declaration \n");

            // DQ (9/24/2004): Allow cdecl to be NULL to allow classdecl_stmt->get_definition() to be 
            // NULL to avoid numerous SgClassDefinition objects where there was no defining declaration!
            // if (!cdecl->isFriend())
            // if ( !cdecl->get_declarationModifier().isFriend() )
               if ( (cdecl != NULL) && (!cdecl->get_declarationModifier().isFriend()) )
                  {
                    ROSE_ASSERT (cdecl->get_parent() != NULL);
                    cdefn         = isSgClassDefinition(cdecl->get_parent());
                    namespaceDefn = isSgNamespaceDefinitionStatement(cdecl->get_parent());
                 // ROSE_ASSERT (cdefn != NULL || namespaceDefn != NULL);
                  }
              // else, default to global or current scope (without qualification)
             }
            else
             {
#if 0
               printf ("This MIGHT be the defining declaration  cdecl = %p namespaceDefn = %p \n",cdecl,namespaceDefn);
               printf ("classdecl_stmt->get_declarationModifier().isFriend() = %s \n",
                    classdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
               printf ("classdecl_stmt->isForward() = %s \n",classdecl_stmt->isForward() ? "true" : "false");
#endif

               ROSE_ASSERT(cdefn == NULL);

            // DQ: Rule out any cases where we would not want to unparse the name as a qualified name
            // class declarations failing this test are unparsed without any qualification.
            // if ( ! classdecl_stmt->isFriend() && ! classdecl_stmt->isForward())
               if ( ! classdecl_stmt->get_declarationModifier().isFriend() && 
                    ! classdecl_stmt->isForward() )
                  {
                 // ck if defined in var decl
                 // SgStatement *cparent = classdecl_stmt->get_parent();
#if 1
                    initializeDeclarationsFromParent (classdecl_stmt,cdefn,namespaceDefn );
#else
                 // DQ (9/10/2004): Error checking
                    if (classdecl_stmt->get_parent() == NULL)
                       {
                         printf ("In unparseClassDeclaration: classdecl_stmt is a %s \n",classdecl_stmt->sage_class_name());
                         classdecl_stmt->get_file_info()->display("location of classdecl_stmt");
                       }
                    ROSE_ASSERT (classdecl_stmt->get_parent() != NULL);

                 // printf ("classdecl_stmt->get_parent() = %s \n",classdecl_stmt->get_parent()->sage_class_name());
                    SgStatement *cparent = isSgStatement(classdecl_stmt->get_parent());

                 // DQ (7/20/2004): test2001_20.C generates a cparent == NULL
                 // DQ (7/19/2004): Added to enforce that parent of class declaration is a SgStatement
                 // ROSE_ASSERT (cparent != NULL);

                    if (cparent == NULL)
                       {
                      // DQ (7/20/2004): This demonates a current bug in the EDG/Sage 
                      // connection (I think) so leave this message in here.
                         printf ("Warning: In unparseClassDeclStmt classdecl_stmt->get_parent() = %s \n",
                              classdecl_stmt->get_parent()->sage_class_name());
                         cdefn         = NULL;
                         namespaceDefn = NULL;
                      // ROSE_ASSERT (false);
                       }
                      else
                       {
                         switch(cparent->variantT())
                            {
                              case V_SgGlobal:
                                 {
                                // printf ("This is in global scope: cparent = %s \n",cparent->sage_class_name());
                                   ROSE_ASSERT(cdefn == NULL);
                                   break;
                                 }

                              case V_SgTypedefDeclaration:
                                 {
                                // DQ (9/9/2004): I previously thought this case was never visited, but this is incorrect!
                                // This case is used to handle "typedef doubleArray doubleSerialArray;" in A++_notemplates.h with test2001_12.C

                                // printf ("In unparseClassDeclStmt(): Case of searching for class definition in a typedef declaration \n");
#if 0
                                // DQ (9/9/2004): Note that classes referenced in typedefs are unparsed as types (by the 
                                // unparseClassType function) and are not handled by this function!  This has always been 
                                // the case and the logic here to handle typedefs is simply misleading.

                                   printf ("ERROR: This case is handled by the unparseClassType function called by the unparseTypedefStatement \n");
                                   cparent->get_file_info()->display("ERROR: This case is handled by the unparseClassType function called by the unparseTypedefStatement");
                                   ROSE_ASSERT(false);
#endif

                                   SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(cparent);
                                   ROSE_ASSERT (cparent->get_parent() != NULL);
                                // printf ("In unparseClassDeclStmt(): cparent = %s \n",cparent->sage_class_name());
                                // printf ("In unparseClassDeclStmt(): cparent->get_parent() = %s \n",cparent->get_parent()->sage_class_name());
                                // cdefn = isSgClassDefinition(cparent->get_parent());
                                   ROSE_ASSERT (typedefDeclaration->get_declaration() != NULL);
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(typedefDeclaration->get_declaration());
                                   ROSE_ASSERT (classDeclaration != NULL);
                                   ROSE_ASSERT (classDeclaration->get_definition() != NULL);
#if 1
                                // DQ (9/8/2004): Not sure why this is commented out since I thought the point was to set cdefn!
                                   cdefn = isSgClassDefinition(classDeclaration->get_definition());
                                   ROSE_ASSERT (cdefn != NULL);
                                   ROSE_ASSERT (cdefn->get_declaration() != NULL);
                                   ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
#endif
                                   break;
                                 }

                              case V_SgVariableDeclaration:
                                 {
                                // printf ("In unparseClassDeclStmt(): Case of searching for class definition in a variable declaration \n");

                                   SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(cparent);
                                   ROSE_ASSERT (variableDeclaration != NULL);

                                   ROSE_ASSERT (cparent->get_parent() != NULL);
                                // printf ("In unparseClassDeclStmt(): cparent = %s \n",cparent->sage_class_name());
                                // printf ("In unparseClassDeclStmt(): cparent->get_parent() = %s \n",cparent->get_parent()->sage_class_name());
                                   cdefn = isSgClassDefinition(cparent->get_parent());
#if 0
                                // DQ (9/28/2004): this fails for test2003_06.C and I'm not sure that we should assert that it is true!
                                   ROSE_ASSERT (cdefn != NULL);
                                   ROSE_ASSERT (cdefn->get_declaration() != NULL);
                                   ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
#endif
                                   break;
                                 }

                              case V_SgClassDefinition:
                                 {
                                   cdefn = isSgClassDefinition(cparent);
                                   ROSE_ASSERT (cdefn != NULL);
                                   ROSE_ASSERT (cdefn->get_declaration() != NULL);
                                // ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
                                   break;
                                 }

                           // DQ (9/8/2004): Added namespace support!
                              case V_SgNamespaceDefinitionStatement:
                                 {
                                   namespaceDefn = isSgNamespaceDefinitionStatement(cparent);
                                   ROSE_ASSERT (cdefn == NULL);
                                   ROSE_ASSERT (namespaceDefn != NULL);
                                   ROSE_ASSERT (namespaceDefn->get_namespaceDeclaration() != NULL);
                                // ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
                                   break;
                                 }

                              default:
                                 {
                                   printf ("UnparseClassDeclaratation: Default case = %s \n",cparent->sage_class_name());
                                   break;
                                 }
                            }
                       }
#endif
                  } // else, default to global or current scope (without qualification)
             }

       // If the current class declaration is in another class definition (cdefn), then it must be a SgNameType
          SgNamedType *ptype = NULL;
          if (cdefn != NULL)
             {
               ptype = isSgNamedType(cdefn->get_declaration()->get_type());

            // DQ (9/8/2004): This should be true (verify it)
               ROSE_ASSERT(ptype != NULL);
             }

       // printf ("cdefn = %p ptype = %p info.get_current_context() = %p \n",cdefn,ptype,info.get_current_context());

       // bool outputQualifiedName = !((ptype == NULL) || (info.get_current_context() == ptype));
       // bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype));
          ROSE_ASSERT(namespaceDefn == NULL || namespaceDefn->get_namespaceDeclaration() != NULL);
          bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype)) ||
                                     ((namespaceDefn != NULL) && (info.get_current_namespace() != namespaceDefn->get_namespaceDeclaration()));
       // printf ("outputQualifiedName = %s \n",outputQualifiedName ? "true" : "false");

       // cur << " /* Before name in Unparser::unparseClassDeclStmt */ \n";

       // DQ (7/19/2004): Simplified condition to make it more readable
       // if (!ptype || (info.get_current_context() == ptype))
       // if (ptype == NULL || (info.get_current_context() == ptype))
          if (outputQualifiedName == false)
             {
            // printf ("***** This branch should output an UNqualified name? \n");
            // cur << " /* " << classdecl_stmt->get_name().str() << " */ \n";
            // cur << " /* classdecl_stmt->get_from_template() = " << classdecl_stmt->get_from_template() << " */ \n";
               if (classdecl_stmt->get_from_template() == true)
                  {
#if 0
                    printf ("classdecl_stmt->sage_class_name() = %s \n",classdecl_stmt->sage_class_name());
                    printf ("Generate a bogus template class name for now (to force template instatiation) class name = %s \n",classdecl_stmt->get_name().str());
                 // cur << classdecl_stmt->get_name().str() << "<int> ";
#endif

                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(classdecl_stmt);
                    ROSE_ASSERT (templateInstantiationDeclaration != NULL);

                 // DQ (3/21/2004): After modification of the get_name function this code is simplified.
                    cur << templateInstantiationDeclaration->get_name().str() << " ";

#if 0
                    const SgTemplateParameterPtrList & templateParameterList = classdecl_stmt->get_name().get_TemplateParams();
                    printf ("templateParameterList.size() = %d \n",templateParameterList.size());
                    for (SgNodePtrList::const_iterator i = templateParameterList.begin(); i != templateParameterList.end(); i++)
                       {
                         printf ("templateArgList element *i = %s \n",(*i)->sage_class_name());
                       }
#endif
                  }
                 else
                  {
                 // printf ("Output a normal name: class name = %s \n",classdecl_stmt->get_name().str());
                    cur << classdecl_stmt->get_name().str() << " ";
                  }
             }
            else
             {
            // printf ("***** This branch should output a QUALIFIED name? \n");

            // DQ (9/8/2004): This should be true now (however, not now with the more complex namespace test)!

            // DQ (9/9/2004): Exclusive OR these pointers
               ROSE_ASSERT ( (ptype != NULL) || (namespaceDefn != NULL) );
               ROSE_ASSERT ( (ptype == NULL) || (namespaceDefn == NULL) );

            // Handle case of class
               if (ptype != NULL)
                  {
                    ROSE_ASSERT (cdefn != NULL);
                    ROSE_ASSERT (cdefn->get_declaration() != NULL);

                 // DQ (9/8/2004): This should also be true
                    ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().is_null() == false);
                    ROSE_ASSERT (cdefn->get_declaration()->get_qualified_name().str() != NULL);

                 // DQ (9/9/2004): Not sure this comment is correct, qualified names are often required!
#if 0
                 // When using C mode the qualified_name of the parent class is always defined, 
                 // but when using C++ mode the parent class's qualified_name is not defined.
                 // But I don't think that the unparser needs to define the X::Y syntax and 
                 // that defining the Y name is enough (at least in many places).
                    if (cdefn->get_declaration()->get_qualified_name().is_null() == false)
                       {
                      // printf ("cdefn->get_declaration()->get_qualified_name().str() = %s \n",
                      //      cdefn->get_declaration()->get_qualified_name().str());
                         cur << cdefn->get_declaration()->get_qualified_name().str() << "::";
                       }
                      else
                       {
                         printf ("Warning: In unparser, qualified class name is NULL (this might be an error) \n");
                       }
#else
                 // printf ("Output a qualified class name: class name = %s \n",cdefn->get_declaration()->get_qualified_name().str());
                    cur << cdefn->get_declaration()->get_qualified_name().str() << "::";
#endif
                  }

            // Handle case of namespace
               if (namespaceDefn != NULL)
                  {
                    ROSE_ASSERT (namespaceDefn != NULL);
                    SgNamespaceDeclarationStatement* namespaceDeclaration = namespaceDefn->get_namespaceDeclaration();
                    ROSE_ASSERT (namespaceDeclaration != NULL);
                    cur << namespaceDeclaration->get_qualified_name().str() << "::";
                  }

                cur << classdecl_stmt->get_name().str() << " ";
             }

       // cur << "/* After name in Unparser::unparseClassDeclStmt */ \n";

          if (classdecl_stmt->isForward() && !info.SkipSemiColon())
             {
               cur << ";";
               if (classdecl_stmt->isExternBrace())
                  {
                    cur << " }";
                  }
             }
        }
   }

void
Unparser::unparseClassDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Inside of unparseClassDefnStmt \n");
  // cur << "/* Inside of unparseClassDefnStmt */ \n";

     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ROSE_ASSERT(classdefn_stmt != NULL);

     SgUnparse_Info ninfo(info);

  // cur << "/* Print out class declaration */ \n";

     ninfo.set_SkipClassDefinition();

  // DQ (7/19/2003) skip the output of the semicolon
     ninfo.set_SkipSemiColon();

  // printf ("Calling unparseClassDeclStmt = %p isForward = %s from unparseClassDefnStmt = %p \n",
  //      classdefn_stmt->get_declaration(),(classdefn_stmt->get_declaration()->isForward() == true) ? "true" : "false",classdefn_stmt);
     unparseClassDeclStmt( classdefn_stmt->get_declaration(), ninfo);

  // DQ (7/19/2003) unset the specification to skip the output of the semicolon
     ninfo.unset_SkipSemiColon();

     ninfo.unset_SkipClassDefinition();

  // cur << "/* END: Print out class declaration */ \n";

     SgNamedType *saved_context = ninfo.get_current_context();
     ninfo.set_current_context(classdefn_stmt->get_declaration()->get_type());

  // cur << "/* Print out inheritance */ \n";

  // print out inheritance
     SgBaseClassList::iterator p = classdefn_stmt->get_inheritances().begin();
  // int tmp_spec = 0;
     SgClassDeclaration *tmp_decl;
     if (p != classdefn_stmt->get_inheritances().end())
        {
          cur << ": ";
          while(true)
             {
            // DQ (4/25/2004): Use the new modifier interface
            // tmp_spec = (*p).get_base_specifier();
               SgBaseClassModifier & baseClassModifier = (*p).get_baseClassModifier();
               tmp_decl = (*p).get_base_class();
            // specifier
            // if (tmp_spec & SgDeclarationStatement::e_virtual)
            // if (tmp_spec & SgDeclarationStatement::e_virtual)
               if (baseClassModifier.isVirtual())
                  {
                    cur << "virtual ";
                  }
            // if (tmp_spec & SgDeclarationStatement::e_public)
               if (baseClassModifier.get_accessModifier().isPublic())
                  {
                    cur << "public ";
                  }
            // if (tmp_spec & SgDeclarationStatement::e_private)
               if (baseClassModifier.get_accessModifier().isPrivate())
                  {
                    cur << "private ";
                  }
            // if (tmp_spec & SgDeclarationStatement::e_protected)
               if (baseClassModifier.get_accessModifier().isProtected())
                  {
                    cur << "protected ";
                  }
  
            // base name
               cur << tmp_decl->get_name().str();
               p++;
               if (p != classdefn_stmt->get_inheritances().end())
                  {
                    cur << ",";
                  }
                 else
                    break;
             }
        }

#if 0
  // cur << "\n/* After specification of base classes unparse the declaration body */\n";
     printf ("After specification of base classes unparse the declaration body  info.SkipBasicBlock() = %s \n",
          (info.SkipBasicBlock() == true) ? "true" : "false");
#endif

  // DQ (9/28/2004): Turn this back on as the only way to prevent this from being unparsed!
  // DQ (11/22/2003): Control unparsing of the {} part of the definition
  // if ( !info.SkipBasicBlock() )
     if ( info.SkipBasicBlock() == false )
        {
       // cur << "\n/* Unparsing class definition within unparseClassDefnStmt */\n";

          ninfo.set_isUnsetAccess();
          cur.format(classdefn_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
          cur << "{";
          cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

          SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();

          while(pp != classdefn_stmt->get_members().end())
             {
               unparseStatement((*pp), ninfo);
               pp++;
             }

          cur.format(classdefn_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
          cur << "}";
          cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
        }

     ninfo.set_current_context(saved_context);

  // cur << "/* Leaving unparseClassDefnStmt */ \n";
  // printf ("Leaving unparseClassDefnStmt \n");
   }

// never seen this function called yet
void
Unparser::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_stmt = isSgEnumDeclaration(stmt);
     ROSE_ASSERT(enum_stmt != NULL);

  // info.display("Called inside of unparseEnumDeclStmt()");

  if (!info.inEmbeddedDecl()) {
    SgClassDefinition *cdefn=isSgClassDefinition(enum_stmt->get_parent());
    if(cdefn && cdefn->get_declaration()->get_class_type()==SgClassDeclaration::e_class)
      info.set_CheckAccess();
    printDebugInfo("entering printSpecifier", TRUE);
    printSpecifier(enum_stmt, info);
    info.unset_CheckAccess();
    cur << "enum " << enum_stmt->get_name().str() << " ";
  } else { 
    SgClassDefinition *cdefn = NULL;

 // DQ (9/9/2004): Put this message in place at least for now!
    printf ("Need logic to handle enums defined in namespaces and which require qualification \n");
    
    // ck if defined within a var decl
    int v=GLOBAL_STMT;
//  SgStatement *cparent=enum_stmt->get_parent();
    SgStatement *cparent = isSgStatement(enum_stmt->get_parent());
    v=cparent->variant();
    if(v==VAR_DECL_STMT || v==TYPEDEF_STMT)
      cdefn=isSgClassDefinition(cparent->get_parent());
    else if( v==CLASS_DEFN_STMT)
      cdefn=isSgClassDefinition(cparent);
    
    if(cdefn) {
      SgNamedType *ptype=isSgNamedType(cdefn->get_declaration()->get_type());
      if(!ptype || (info.get_current_context() == ptype)) {
	cur << "enum " <<  enum_stmt->get_name().str() << " ";
      }
      else { // add qualifier of current types to the name
	SgName nm= cdefn->get_declaration()->get_qualified_name();
	if(!nm.is_null()) {
	  cur << nm.str() << "::" << enum_stmt->get_name().str() << " ";
	}
	else { 
	  cur << "enum " << enum_stmt->get_name().str() << " ";
	}
      }
    } else {
      cur << "enum " << enum_stmt->get_name().str() << " ";
    }
  }

  // if (!info.SkipDefinition()
     if (!info.SkipEnumDefinition()
    /* [BRN] 4/19/2002 --  part of the fix in unparsing var decl including enum definition */
                  || enum_stmt->get_embedded())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inEnumDecl();
          SgInitializer *tmp_init=NULL;
          SgName tmp_name;
    
          SgInitializedNamePtrList::iterator p = enum_stmt->get_enumerators().begin();
  
          if (p != enum_stmt->get_enumerators().end())
             {
               cur << "{"; 
               while (1)
                  {
                    ROSE_ASSERT((*p) != NULL);
                    tmp_name=(*p)->get_name();
                    tmp_init=(*p)->get_initializer();
                    cur << tmp_name.str();
                    if(tmp_init)
                       {
                         cur << "=";
                         unparseExpression(tmp_init, ninfo);
                       }
                    p++;
                    if (p != enum_stmt->get_enumerators().end())
                       {
                         cur << ",";
                       }
                      else
                         break; 
                  }
               cur << "}";
             }

          if (!info.SkipSemiColon())
             {
            // cur << " /* output a ; */ ";
               cur << ";";
               if (enum_stmt->isExternBrace())
                  {
                    cur << " }";
                  }
             }

       /* [BRN] 4/19/2002 -- part of fix in unparsing var decl including enum definition */
          if (enum_stmt->get_embedded())
             {
               cur << " ";
             }
          enum_stmt->set_embedded(false);
       /* [BRN] end */
        } /* if */
   }

void Unparser::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgExprStatement* expr_stmt = isSgExprStatement(stmt);
  ROSE_ASSERT(expr_stmt != NULL);

  SgUnparse_Info newinfo(info);

// Expression are another place where a class definition should NEVER be unparsed
  newinfo.set_SkipClassDefinition();

  if(expr_stmt->get_the_expr())
     {
       printDebugInfo(getSgVariant(expr_stmt->get_the_expr()->variant()), TRUE); 
       unparseExpression(expr_stmt->get_the_expr(), newinfo);
     }
    else
       assert(false);

  if(newinfo.inVarDecl()) {
    cur << ",";
  }
  else if (!newinfo.inConditional() && !newinfo.SkipSemiColon()) {
    cur << ";";
  }
}

void Unparser::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
  ROSE_ASSERT(label_stmt != NULL);

  cur << label_stmt->get_label().str() << ":";
}

void Unparser::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
  ROSE_ASSERT(while_stmt != NULL);

  cur << "while" << "(";
  info.set_inConditional();
  
  unparseStatement(while_stmt->get_condition(), info);
  info.unset_inConditional();
  cur << ")";
  if(while_stmt->get_body()) 
    unparseStatement(while_stmt->get_body(), info);
  else if (!info.SkipSemiColon()) { cur << ";"; }

}

void Unparser::unparseDoWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgDoWhileStmt* dowhile_stmt = isSgDoWhileStmt(stmt);
  ROSE_ASSERT(dowhile_stmt != NULL);

  cur << "do ";
  unparseStatement(dowhile_stmt->get_body(), info);
  cur << "while " << "(";
  SgUnparse_Info ninfo(info);
  ninfo.set_inConditional();

  //we need to keep the properties of the prevnode (The next prevnode will set the
  //line back to where "do" was printed) 
// SgLocatedNode* tempnode = prevnode;

  unparseStatement(dowhile_stmt->get_condition(), ninfo);
  ninfo.unset_inConditional();
  cur << ")"; 
  if (!info.SkipSemiColon()) { cur << ";"; }
}

void
Unparser::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
  
     ROSE_ASSERT(switch_stmt != NULL);

     cur << "switch(";
     unparseExpression(switch_stmt->get_item_selector(), info);
     cur << ")";

  // DQ (11/5/2003): Support for skipping basic block added to support 
  //                 prefix generation for AST Rewrite Mechanism
  // if(switch_stmt->get_body())
     if ( (switch_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(switch_stmt->get_body(), info);
   }

void
Unparser::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
     ROSE_ASSERT(case_stmt != NULL);

     cur << "case ";
     unparseExpression(case_stmt->get_key(), info);
     cur << ":";

  // if(case_stmt->get_body())
     if ( (case_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(case_stmt->get_body(), info);
   }

void Unparser::unparseTryStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTryStmt* try_stmt = isSgTryStmt(stmt);
     ROSE_ASSERT(try_stmt != NULL);

     cur << "try ";
  
     unparseStatement(try_stmt->get_body(), info);
  
     SgStatementPtrList::iterator i=try_stmt->get_catch_statement_seq().begin();
     while (i != try_stmt->get_catch_statement_seq().end())
        {
          unparseStatement(*i, info);
          i++;
        }
   }

void Unparser::unparseCatchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCatchOptionStmt* catch_statement = isSgCatchOptionStmt(stmt);
     ROSE_ASSERT(catch_statement != NULL);

     cur << "catch " << "(";
     if (catch_statement->get_condition())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inVarDecl();

       // DQ (5/6/2004): this does not unparse correctly if the ";" is included
          ninfo.set_SkipSemiColon();
          ninfo.set_SkipClassSpecifier();
          unparseStatement(catch_statement->get_condition(), ninfo);
        }

     cur << ")";
  // if (catch_statement->get_condition() == NULL) prevnode = catch_statement;

     unparseStatement(catch_statement->get_body(), info);
   }

void Unparser::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
     ROSE_ASSERT(default_stmt != NULL);

     cur << "default:";
  // if(default_stmt->get_body()) 
     if ( (default_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(default_stmt->get_body(), info);
   }

void Unparser::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
  ROSE_ASSERT(break_stmt != NULL);

  cur << "break; ";
}

void Unparser::unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgContinueStmt* continue_stmt = isSgContinueStmt(stmt);
  ROSE_ASSERT(continue_stmt != NULL);

  cur << "continue; ";
}

void
Unparser::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
     ROSE_ASSERT(return_stmt != NULL);

     cur << "return ";
     SgUnparse_Info ninfo(info);

     if (return_stmt->get_return_expr())
        {
          unparseExpression(return_stmt->get_return_expr(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
               cur << ";";
        }
   }

void
Unparser::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
  ROSE_ASSERT(goto_stmt != NULL);

  cur << "goto " << goto_stmt->get_label()->get_label().str();
  if (!info.SkipSemiColon()) { cur << ";"; }
}

//never seen this function called yet
void Unparser::unparseAsmStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgAsmStmt* asm_stmt = isSgAsmStmt(stmt);
  ROSE_ASSERT(asm_stmt != NULL);
  cur << "asm(";
  unparseExpression(asm_stmt->get_expr(), info);
  cur << ")";
  if (!info.SkipSemiColon()) { cur << ";"; }
}

//never seen this function called yet
void Unparser::unparseSpawnStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgSpawnStmt* spawn_stmt = isSgSpawnStmt(stmt);
  ROSE_ASSERT(spawn_stmt != NULL);
  
  cur << "spawn ";
  
  unparseExpression(spawn_stmt->get_the_func(), info);
  if (!info.SkipSemiColon()) cur << ";";
}

void
Unparser::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTypedefDeclaration* typedef_stmt = isSgTypedefDeclaration(stmt);
     ROSE_ASSERT(typedef_stmt != NULL);

  // printf ("In unparseTypeDefStmt() \n");

  // DQ (10/5/2004): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current variable declaration (e.g. struct A { int x; } a;).  In this case we have to output the base type with
  // its definition.
     bool outputTypeDefinition = typedef_stmt->get_typedefBaseTypeContainsDefiningDeclaration();
  // printf ("outputTypeDefinition = %s \n",(outputTypeDefinition == true) ? "true" : "false");

     if (!info.inEmbeddedDecl())
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* NOT an embeddedDeclaration */\n";
#endif
          SgClassDefinition *cdefn = isSgClassDefinition(typedef_stmt->get_parent());
          if (cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
               info.set_CheckAccess();
          printDebugInfo("entering printSpecifier", TRUE);
          printSpecifier(typedef_stmt, info);
          info.unset_CheckAccess();
        }
       else
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Found an embeddedDeclaration */\n";
#endif
        }
     
     SgUnparse_Info ninfo(info);

  // DQ (10/5/2004): This controls the unparsing of the class definition
  // when unparsing the type within this variable declaration.
     if (outputTypeDefinition == true)
        {
       // printf ("Output the full definition as a basis for the typedef base type \n");
       // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
          ROSE_ASSERT(ninfo.SkipClassDefinition() == false);
        }
       else
        {
       // printf ("Skip output of the full definition as a basis for the typedef base type \n");
       // DQ (10/5/2004): If this is a non-defining declaration then skip the definition
          ninfo.set_SkipClassDefinition();
          ROSE_ASSERT(ninfo.SkipClassDefinition() == true);
        }

  // Note that typedefs of function pointers and member function pointers 
  // are quite different from ordinary typedefs and so should be handled
  // separately.

  // First look for a pointer to a function
     SgPointerType* pointerToType = isSgPointerType(typedef_stmt->get_base_type());

     SgFunctionType* functionType = NULL;
     if (pointerToType != NULL)
          functionType = isSgFunctionType(pointerToType->get_base_type());

  // SgPointerMemberType* pointerToMemberType = isSgPointerMemberType(typedef_stmt->get_base_type());

  // DQ (9/15/2004): Added to support typedefs of member pointers
     SgMemberFunctionType* pointerToMemberFunctionType = isSgMemberFunctionType(typedef_stmt->get_base_type());

#if 0
     printf ("In unparseTypedef: functionType                = %p \n",functionType);
  // printf ("In unparseTypedef: pointerToMemberType         = %p \n",pointerToMemberType);
     printf ("In unparseTypedef: pointerToMemberFunctionType = %p \n",pointerToMemberFunctionType);
#endif

  // DQ (9/22/2004): It is not clear why we need to handle this case with special code.
  // We are only putting out the return type fo the function type (for functions or member functions).
  // It seems that the reason we handle function pointers separately is that typedefs of non function 
  // pointers could include the complexity of class declarations with definitions and separating the 
  // code for function pointers allows for easier debugging.  When typedefs of defining class 
  // declarations is fixed we might be able to unify these separate cases.

  // This handles pointers to functions and member function (but not pointers to members!)
  // if ( (functionType != NULL) || (pointerToMemberType != NULL) )
     if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) )
        {
       // Newly implemented case of typedefs for function and member function pointers
       // printf ("case of typedefs for function and member function pointers \n");
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Case of typedefs for function and member function pointers */\n";
#endif
          ninfo.set_SkipFunctionQualifier();
          cur << "typedef ";

       // DQ (9/22/2004): This conditional handles the output of the function's (or member function's)
       // return type.  This would not be required if we would output the return type completely for a 
       // function type. I'm unclear why we don't output the complete return type in the case of putting 
       // out the function type and doing so is likely the fix for function pointers where the function 
       // return type is a pointer (since part of the return type (the "*") is output twice, which is a bug).

       // DQ (9/22/2004): I think that this can be removed! Allow the unparsing of the first part of the function type to
       // put out the function return type (instead of putting it out explicitly!)
#if 0
       // DQ (9/15/2004): pointers to member functions were previously confused with pointers 
       // to anything (so pointers to member data were not handled properly (pointers to member 
       // data were handled as pointers to member functions (and failed)).
       // if (pointerToMemberType != NULL)
          if (pointerToMemberFunctionType != NULL)
             {
            // Case typedef of a member function pointer
            // ROSE_ASSERT (pointerToMemberType != NULL);
               ROSE_ASSERT (pointerToMemberFunctionType != NULL);
               ROSE_ASSERT (functionType == NULL);

            // start of code for non-functionPointer case
               SgMemberFunctionDeclaration* memberFunctionDeclarationStatement =
                    isSgMemberFunctionDeclaration(typedef_stmt->get_declaration());

            // DQ (9/15/2004): This is not true for pointer to member data (also had to be fixed in EDG/Sage connection code!)
               ROSE_ASSERT (memberFunctionDeclarationStatement != NULL);

            // DQ (9/15/2004): This should always be true
            // if (memberFunctionDeclarationStatement != NULL)
                  {
                    SgType* functionReturnType = memberFunctionDeclarationStatement->get_orig_return_type();
                    ROSE_ASSERT (functionReturnType != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
                    cur << "\n/* Output functionReturnType Case typedef of a member function pointer */\n";
#endif
                    unparseType(functionReturnType, ninfo);
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
                    cur << "\n/* Done: Output functionReturnType */\n";
#endif
                    cur << " "; 
                    ninfo.set_isWithType();
                    ninfo.set_SkipBaseType();

                 // DQ (9/15/2004): Moved from outside of conditional scope (below) since 
                 // memberFunctionDeclarationStatement can be NULL (e.g. for pointer to member data)
                 // Now initialize the functionType pointer
                    functionType = memberFunctionDeclarationStatement->get_type();
                  }

               ROSE_ASSERT (functionType != NULL);

            // end of code for non-functionPointer case
             }
            else
             {
            // Case typedef of a non-member function pointer

            // ROSE_ASSERT (pointerToMemberType == NULL);
               ROSE_ASSERT (pointerToMemberFunctionType == NULL);
               ROSE_ASSERT (functionType != NULL);

               SgFunctionDeclaration* functionDeclarationStatement =
                    isSgFunctionDeclaration(typedef_stmt->get_declaration());
               ROSE_ASSERT (functionDeclarationStatement != NULL);

            // DQ (9/15/2004): This should always be true
            // if (functionDeclarationStatement != NULL)
                  {
                    SgType* functionReturnType = functionDeclarationStatement->get_orig_return_type();
                    ROSE_ASSERT (functionReturnType != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
                    cur << "\n/* Output functionReturnType Case typedef of a non-member function pointer */\n";
#endif
                    unparseType(functionReturnType, ninfo);
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
                    cur << "\n/* Done: Output functionReturnType Case typedef of a non-member function pointer */\n";
#endif
                    cur << " "; 
                    ninfo.set_isWithType();
                    ninfo.set_SkipBaseType();
                  }

               ROSE_ASSERT (functionType != NULL);
             }
#else
       // printf ("Skipping the explicit output of the return type for function and member function pointers (allow unparseFunctionType to output this \n");
#endif

       // Specify that only the first part of the type shold be unparsed 
       // (this will permit the introduction of the name into the member
       // function pointer declaration)
          ninfo.set_isTypeFirstPart();

#if OUTPUT_DEBUGGING_UNPARSE_INFO
          cur << "\n/* " << ninfo.displayString("After return Type now output the base type (first part then second part)") << " */\n";
#endif

       // The base type contains the function po9inter type
          SgType *btype = typedef_stmt->get_base_type();
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Output base type (first part) */\n";
#endif
          unparseType(btype, ninfo);
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Done: Output base type (first part) */\n";
#endif
          cur << typedef_stmt->get_name().str();

       // Now unparse the second part of the typedef
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Output base type (second part) */\n";
#endif
          ninfo.set_isTypeSecondPart();
          unparseType(btype, ninfo);
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Done: Output base type (second part) */\n";
#endif

#if 0
       // DQ (9/15/2004): Not clear what to do here!
          printf ("In unparser: Not clear if parens are required for pointer to member data or just pointer to member function \n");
       // This is a temp fix because it seems that declarations of functions are output with
       // the "(...)" arguments while member function declarations are not (need to fix this).
          if (pointerToMemberType != NULL)
//        if ( (pointerToMemberType != NULL) || (pointerToMemberFunctionType != NULL) )
             {
            // output a "(" before the comma separate argument list of types
               cur << "\n/* In unparseTypeDefStmt(): start of argument list */\n";
               cur << "(";

               ROSE_ASSERT (functionType != NULL);

            // Now unparse the function arguments
               SgTypePtrList::iterator p = functionType->get_arguments().begin();
               while(p != functionType->get_arguments().end())
                  {
                    printf ("In unparseTypeDefStmt: output the arguments \n");
                    unparseType(*p, ninfo);
                    p++;
                    if (p != functionType->get_arguments().end())
                       {
                              cur << ", "; 
                       }
                  }

            // output a closing ")" after the comma separate argument list of types
               cur << ")";
               cur << "\n/* In unparseTypeDefStmt(): end of argument list */\n";
             }
#else
       // printf ("In unparseTypeDefStmt(): skipped unparsing member function arguments since it appears to be redundent! \n");
#endif

          if (!info.SkipSemiColon())
             {
               cur << ";";
             }
        }
       else
        {
       // previously implemented case of unparsing the typedef does not handle 
       // function pointers properly (so they are handled explicitly above!)

       // printf ("Not a typedef for a function type or member function type \n");
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Output function pointers (Not a typedef for a function type or member function type) */\n";
#endif
          ninfo.set_SkipFunctionQualifier();
          cur << "typedef ";

          ninfo.set_SkipSemiColon();
          SgType *btype = typedef_stmt->get_base_type();

#if 0
       // DQ (10/6/2004): Get rid of this now that we have a better way to controling the output of definitions!

       // todo: should allow multiple names
       // DQ (3/25/2003)
       // Do avoid infinite recursion in processing:
       // typedef struct Xtag { Xtag* next; } X;
       // by detecting the use of structure tags that don't require unparsing the structures
       // All structure tags are places into a list and the list is searched, if the tag is found
       // then unparsing the type representing the tag is only unparsed to generate the type name
       // only (not the full class definition).
          SgNamedType *namedType = isSgNamedType(btype);

          if (namedType != NULL)
               printf ("namedType->get_autonomous_declaration() = %s \n",namedType->get_autonomous_declaration() ? "true" : "false");
            else
               printf ("namedType == NULL \n");

       // DQ (10/6/2004): Get rid of this now that we have a better way to controling the output of definitions!
       // SgDeclarationStatement* declStmt = (namedType) ? namedType->get_declaration() : NULL;
          bool structureTagBeingProcessed = true;
          if ( (btype != NULL) &&
               ( (namedType != NULL) &&
                 (namedType->get_autonomous_declaration() == FALSE) ) )
             {
               structureTagBeingProcessed = find(ninfo.getStructureTagList().begin(),
                                                 ninfo.getStructureTagList().end(),namedType)
                                            != ninfo.getStructureTagList().end();

               if (structureTagBeingProcessed == false)
                    ninfo.addStructureTag(namedType);
             }

       // printf ("structureTagBeingProcessed = %s \n",structureTagBeingProcessed ? "true" : "false");
#endif

#if 0
       // DQ (7/19/2004):
       // When is the declaration pointer valid? Answer: Maybe nowhere!
       // When this is available it should be output by the unparsing of the type
          printf ("typedef_stmt->get_declaration() = %s \n",
               (typedef_stmt->get_declaration() != NULL) ?
                  typedef_stmt->get_declaration()->sage_class_name() : "NULL POINTER");
          printf ("Before unparseStatement (unpase the declaration in the typedef ) \n");
          cur << "\n/* Before unparseStatement (unpase the declaration in the typedef) */\n";
          if (typedef_stmt->get_declaration() != NULL)
             {
            // DQ (7/19/2004): just get the name, the class definition is output in the unparsing of the type 
            // unparseStatement(typedef_stmt->get_declaration(), ninfo);
               printf ("Skipping the output of the class definition, just get the name \n");
               SgUnparse_Info localInfo(ninfo);
//             localInfo.set_SkipClassDefinition();
//             unparseStatement(typedef_stmt->get_declaration(), localInfo);
               cur << " ";
//             ninfo.set_isWithType();
//             ninfo.set_SkipBaseType();
             }
          cur << "\n/* After unparseStatement (unpase the declaration in the typedef) */\n";
          printf ("After unparseStatement (unpase the declaration in the typedef) \n");
#endif

          ninfo.set_isTypeFirstPart();

       // printf ("Before first part of type (type = %s) \n",btype->sage_class_name());
       // ninfo.display ("Before first part of type in unparseTypeDefStmt()");

       // cur << "\n/* Before first part of type */\n";
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Output base type (first part) */\n";
#endif
          unparseType(btype, ninfo);
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Done: Output base type (second part) */\n";
#endif
       // cur << "\n/* After first part of type */\n";
       // printf ("After first part of type \n");

#if 1
       // DQ (10/7/2004): Moved the output of the name to before the output of the second part of the type
       // to handle the case of "typedef A* A_Type[10];" (see test2004_104.C).

       // The name of the type (X, in the following example) has to appear after the 
       // declaration. Example: struct { int a; } X;
          if (typedef_stmt->get_name().str() != NULL)
               cur << typedef_stmt->get_name().str();
            else
               cur << "NO NAME SPECIFIED IN unparseTypeDefStmt()";
#endif

          ninfo.set_isTypeSecondPart();

       // printf ("Before 2nd part of type \n");
       // cur << "\n/* Before second part of type */\n";
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Output base type (second part) */\n";
#endif

       // DQ (10/6/2004): Get rid of this now that we have a better way to controling the output of definitions!
       // if (structureTagBeingProcessed == false)
       //      unparseType(btype, ninfo);
          unparseType(btype, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* Done: Output base type (second part) */\n";
#endif
       // cur << "\n/* After second part of type */\n";
       // printf ("After 2nd part of type \n");

#if 0
       // The name of the type (X, in the following example) has to appear after the 
       // declaration. Example: struct { int a; } X;
          if (typedef_stmt->get_name().str() != NULL)
               cur << typedef_stmt->get_name().str();
            else
               cur << "NO NAME SPECIFIED IN unparseTypeDefStmt()";
#endif

#if 0
       // DQ (10/17/2004): This is now an error since the output of types has been unified and rewritten!
       // DQ (9/16/2004): If this is a pointer to a member then the unparseMemberPointerType 
       // function introduced an opening "(" which we must now close!
          if ( isSgPointerMemberType(btype) != NULL )
             {
               printf ("Skipping the output of the final \")\" \n");
            // cur << ")";
             }
#endif
          if (!info.SkipSemiColon())
             {
               cur << ";";
             }
        }

  // info.display ("At base of unparseTypeDefStmt()");
   }

// never seen this function called yet
void
Unparser::unparseTemplateDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // By commenting this out we avoid the template declaration 
  // that is placed at the top of any A++ application.

     SgTemplateDeclaration* template_stmt = isSgTemplateDeclaration(stmt);
     ROSE_ASSERT(template_stmt != NULL);

  // printf ("In unparseTemplateDeclStmt(template_stmt = %p) \n",template_stmt);
  // template_stmt->get_declarationModifier().display("In unparseTemplateDeclStmt()");

  // SgUnparse_Info ninfo(info);

  // Check to see if this is an object defined within a class
     ROSE_ASSERT (template_stmt->get_parent() != NULL);
     SgClassDefinition *cdefn = isSgClassDefinition(template_stmt->get_parent());
     if (cdefn != NULL)
        {
          if (cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
             {
               info.set_CheckAccess();
             }
        }

  // SgUnparse_Info saved_ninfo(ninfo);
  // this call has been moved below, after we indent
  // printSpecifier2(vardecl_stmt, ninfo);

  // Setup the SgUnparse_Info object for this statement
  // ninfo.unset_CheckAccess();
  // info.set_access_attribute(ninfo.get_access_attribute());

  // info.display("In unparseTemplateDeclStmt()");

  // Output access modifiers
     printSpecifier1(template_stmt, info);

  // printf ("template_stmt->get_string().str() = %s \n",template_stmt->get_string().str());

  // DQ (1/21/2004): Use the string class to simplify the previous version of the code
     string templateString = template_stmt->get_string().str();

  // DQ (4/29/2004): Added support for "export" keyword (not supported by g++ yet)
     if (template_stmt->get_declarationModifier().isExport())
          cur << "export ";

  // printf ("template_stmt->get_template_kind() = %d \n",template_stmt->get_template_kind());
     switch (template_stmt->get_template_kind())
        {
          case SgTemplateDeclaration::e_template_class :
          case SgTemplateDeclaration::e_template_m_class :
          case SgTemplateDeclaration::e_template_function :
          case SgTemplateDeclaration::e_template_m_function :
          case SgTemplateDeclaration::e_template_m_data :
             {
               cur << "\n" << templateString;
               break;
             }
          case SgTemplateDeclaration::e_template_none :
            // printf ("Do we need this extra \";\"? \n");
            // cur << templateString << ";";
               printf ("Error: SgTemplateDeclaration::e_template_none found (not sure what to do here) \n");
               ROSE_ASSERT (false);
               break;
          default:
               printf ("Error: default reached \n");
               ROSE_ASSERT (false);
        }
   }
 
void
Unparser::unparseAttachedPreprocessingInfo(
   SgStatement* stmt,
   SgUnparse_Info& info,
   PreprocessingInfo::RelativePositionType whereToUnparse)
   {
#if 0
  // Debugging added by DQ
     printf ("In Unparser::unparseAttachedPreprocessingInfo(%s): whereToUnparse = %s \n",
          stmt->sage_class_name(),
          (whereToUnparse == PreprocessingInfo::before) ? "before" : "after");
     printOutComments (stmt);
#endif

  // Get atached preprocessing info
     AttachedPreprocessingInfoType *prepInfoPtr= stmt->getAttachedPreprocessingInfo();

     if (!prepInfoPtr)
        {
       // There's no preprocessing info attached to the current statement
       // printf ("No comments or CPP directives associated with this statement ... \n");
          return;
        }

  // If we are skiping BOTH comments and CPP directives then there is nothing to do
     if ( info.SkipComments() && info.SkipCPPDirectives() )
        {
       // There's no preprocessing info attached to the current statement
       // printf ("Skipping output or comments and CPP directives \n");
          return;
        }

#if 0
     info.display("In Unparser::unparseAttachedPreprocessingInfo()");
#endif

  // Traverse the container of PreprocessingInfo objects
     AttachedPreprocessingInfoType::iterator i;
     for(i = prepInfoPtr->begin(); i != prepInfoPtr->end(); ++i)
        {
       // i ist a pointer to the current prepInfo object, print current preprocessing info
       // Assert that i points to a valid preprocssingInfo object
          ROSE_ASSERT ((*i) != NULL);
          ROSE_ASSERT ((*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration);
          ROSE_ASSERT ((*i)->getRelativePosition() == PreprocessingInfo::before || 
                       (*i)->getRelativePosition() == PreprocessingInfo::after);

#if 0
          printf ("Stored comment: (*i)->getRelativePosition() = %s (*i)->getString() = %s \n",
               ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
               (*i)->getString());
#endif

       // Check and see if the info object would indicate that the statement would 
       // be printed, if not then don't print the comments associated with it.
       // These might have to be handled on a case by case basis.
       // bool infoSaysGoAhead = !info.SkipDefinition();
          bool infoSaysGoAhead = !info.SkipEnumDefinition()  &&
                                 !info.SkipClassDefinition() &&
                                 !info.SkipFunctionDefinition();

#if 0
          printf ("infoSaysGoAhead = %s \n",infoSaysGoAhead ? "true" : "false");
#endif

#if 0
          static int counter = 0;
          counter++;
          if (counter > 3)
          {
             printf ("Exiting in Unparser::unparseAttachedPreprocessingInfo() \n");
             ROSE_ABORT();
          }
#endif

// DQ (2/5/2003):
// The old directive handling allows all the test codes to parse properly, but
// is not sufficent for handling the A++ transformations which are more complex.
// I am trying to recover the old way which permitted all the test codes to work
// before I fix it to be consistant with how it must work for the A++ transformation
// to work properly.  This is work that was unfinished by Markus K. summer 2002.
// (Though he did do a great job not enough infor was provided in SAGE from EDG 
// to finish it (I think)).

// DQ (2/18/2003): Work to allow all CPP directives to be unparsed correctly on a statement
//                 by statement basis has been completed, tested, and checked in.

          if (infoSaysGoAhead && (*i)->getRelativePosition() == whereToUnparse)
             {
               cur.format(stmt, info, FORMAT_BEFORE_DIRECTIVE);
               if (opt.get_unparse_includes_opt() == true)
                  {
                 // If we are unparsing the include files then we can simplify the 
                 // CPP directive processing and unparse them all as comments!
                 // Comments can also be unparsed as comments (I think!).
                    cur <<  "// " << (*i)->getString();
                  }
                 else
                  {
               switch ( (*i)->getTypeOfDirective() )
                  {
                 // All #include directives are unparsed so that we can make the 
                 // output codes a similar as possible to the input codes. This also
                 // simplifies the debugging. On the down side it sets up a chain of 
                 // problems that force us to unparse most of the other directives 
                 // which makes the unparsing a bit more complex.
                    case PreprocessingInfo::CpreprocessorIncludeDeclaration:
                         if ( !info.SkipComments() )
                            {
                              if (opt.get_unparse_includes_opt() == true)
                                   cur << "// " << (*i)->getString();
                                else
                                   cur << (*i)->getString();
                            }
                         break;

                 // Comments don't have to be further commented
                    case PreprocessingInfo::C_StyleComment:
                    case PreprocessingInfo::CplusplusStyleComment:
                         if ( !info.SkipComments() )
                            {
                              cur << (*i)->getString();
                            }
                         break;

                 // extern declarations must be handled as comments since 
                 // the EDG frontend strips them away
                    case PreprocessingInfo::ClinkageSpecificationStart:
                    case PreprocessingInfo::ClinkageSpecificationEnd:
                         if ( !info.SkipComments() )
                            {
                              if (opt.get_unparse_includes_opt() == true)
                                   cur <<  string("// ") << (*i)->getString();
                                else
                                   cur << (*i)->getString();
                            }
                         break;

                 // Must unparse these because they could hide a #define 
                 // directive which would then be seen e.g.
                 //      #if 0
                 //      #define printf parallelPrintf
                 //      #endif
                 // So because we unparse the #define we must unparse 
                 // the #if, #ifdef, #else, and #endif directives.
                 // line declarations should also appear in the output 
                 // to permit the debugger to see the original code
                    case PreprocessingInfo::CpreprocessorIfdefDeclaration:
                    case PreprocessingInfo::CpreprocessorIfndefDeclaration:
                    case PreprocessingInfo::CpreprocessorIfDeclaration:
                    case PreprocessingInfo::CpreprocessorElseDeclaration:
                    case PreprocessingInfo::CpreprocessorElifDeclaration:
                    case PreprocessingInfo::CpreprocessorEndifDeclaration:
                    case PreprocessingInfo::CpreprocessorLineDeclaration:
                         if ( !info.SkipComments() )
                            {
                              if (opt.get_unparse_includes_opt() == true)
                                   cur << "// " << (*i)->getString();
                                else
                                   cur << (*i)->getString();
                            }
                         break;

                 // Comment out these declarations where they occur because we don't need
                 // them (they have already been evaluated by the front-end and would be
                 // redundent).
                    case PreprocessingInfo::CpreprocessorErrorDeclaration:
                    case PreprocessingInfo::CpreprocessorEmptyDeclaration:
                         if ( !info.SkipCPPDirectives() )
                              cur << "// " << (*i)->getString() ;
                         break;

                 // We skip commenting out these cases for the moment
                 // We must unparse these since they could control the path 
                 // taken in header files included separately e.g.
                 //      #define OPTIMIZE_ME
                 //      // optimization.h could include two paths dependent on the value of OPTIMIZE_ME
                 //      #include "optimization.h"
                    case PreprocessingInfo::CpreprocessorDefineDeclaration:
                    case PreprocessingInfo::CpreprocessorUndefDeclaration:
                         if ( !info.SkipCPPDirectives() )
                            {
                              if (opt.get_unparse_includes_opt() == true)
                                   cur << "//" << (*i)->getString();
                                else
                                   cur << (*i)->getString();
                            }
                         break;

                    case PreprocessingInfo::CpreprocessorUnknownDeclaration:
                         printf ("Error: CpreprocessorUnknownDeclaration found \n");
                         ROSE_ABORT();
                         break;

                    default:
                         printf ("Error: default reached in switch in Unparser::unparseAttachedPreprocessingInfo()\n");
                         ROSE_ABORT();
                  }
                  }

             }
              cur.format(stmt, info, FORMAT_AFTER_DIRECTIVE);
        }
   }

 // EOF



