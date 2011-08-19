/* unparseJava_statements.C
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
 * Unparse_Java::unparse is called, and for statements, 
 * Unparse_Java::unparseStatement is called.
 *
 */
#include "sage3basic.h"
#include "unparser.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

using namespace std;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS  0
#define OUTPUT_DEBUGGING_UNPARSE_INFO        0

// Output the class name and function names as we unparse (for debugging)
#define OUTPUT_DEBUGGING_CLASS_NAME    0
#define OUTPUT_DEBUGGING_FUNCTION_NAME 0
#define OUTPUT_HIDDEN_LIST_DATA 0


Unparse_Java::Unparse_Java(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
  // Nothing to do here!
   }

Unparse_Java::~Unparse_Java()
   {
  // Nothing to do here!
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::unparseOneElemConInit
//
//  This function is used to only unparse constructor initializers with one 
//  element in their argument list. Because there is only one argument in the
//  list, the parenthesis around the arguments are not unparsed (to control
//  the excessive printing of parenthesis). 
//-----------------------------------------------------------------------------------
void 
Unparse_Java::unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info)
   {
     ROSE_ASSERT(con_init != NULL);
  /* code inserted from specification */

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("\n\nInside of Unparse_Java::unparseOneElemConInit (%p) \n",con_init);
     curprint( "\n /* Inside of Unparse_Java::unparseOneElemConInit */ \n");
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving of Unparse_Java::unparseOneElemConInit \n\n\n");
     curprint( "\n /* Leaving of Unparse_Java::unparseOneElemConInit */ \n");
#endif
   }

void
Unparse_Java::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function

     ROSE_ASSERT(stmt != NULL);

     curprint_indented("", info);
     switch (stmt->variantT())
        {
       // DQ (3/14/2011): Need to move the Java specific unparse member functions from the base class to this function.

       // declarations
       // case V_SgInterfaceStatement:     unparseInterfaceStmt(stmt, info); break;
          case V_SgVariableDeclaration:    unparseVarDeclStmt  (stmt, info); break;
          case V_SgVariableDefinition:     unparseVarDefnStmt  (stmt, info); break;

       // executable statements, control flow
          case V_SgBasicBlock:             unparseBasicBlockStmt (stmt, info); break;
          case V_SgIfStmt:                 unparseIfStmt         (stmt, info); break;

          case V_SgWhileStmt:              unparseWhileStmt      (stmt, info); break;
          case V_SgSwitchStatement:        unparseSwitchStmt     (stmt, info); break;
          case V_SgCaseOptionStmt:         unparseCaseStmt       (stmt, info); break;
          case V_SgDefaultOptionStmt:      unparseDefaultStmt    (stmt, info); break;
          case V_SgBreakStmt:              unparseBreakStmt      (stmt, info); break;
          case V_SgLabelStatement:         unparseLabelStmt      (stmt, info); break;
          case V_SgGotoStatement:          unparseGotoStmt       (stmt, info); break;
          case V_SgReturnStmt:             unparseReturnStmt     (stmt, info); break;

          case V_SgForStatement:           unparseForStmt(stmt, info);          break; 
          case V_SgFunctionDeclaration:    unparseFuncDeclStmt(stmt, info);     break;
          case V_SgFunctionDefinition:     unparseFuncDefnStmt(stmt, info);     break;
          case V_SgMemberFunctionDeclaration: unparseMFuncDeclStmt(stmt, info); break;

          case V_SgClassDeclaration:       unparseClassDeclStmt(stmt, info);    break;
          case V_SgClassDefinition:        unparseClassDefnStmt(stmt, info);    break;
          case V_SgEnumDeclaration:        unparseEnumDeclStmt(stmt, info);     break;
          case V_SgExprStatement:          unparseExprStmt(stmt, info);         break;

          case V_SgDoWhileStmt:            unparseDoWhileStmt(stmt, info);      break;

          case V_SgTryStmt:                unparseTryStmt(stmt, info);          break;
          case V_SgCatchOptionStmt:        unparseCatchStmt(stmt, info);        break;

          case V_SgContinueStmt:           unparseContinueStmt(stmt, info);     break;

       // case V_SgAsmStmt:                unparseAsmStmt(stmt, info);          break;

          case V_SgTypedefDeclaration:     unparseTypeDefStmt(stmt, info);      break;
          case V_SgTemplateDeclaration:    unparseTemplateDeclStmt(stmt, info); break;

          case V_SgTemplateInstantiationDecl:               unparseTemplateInstantiationDeclStmt(stmt, info); break;
          case V_SgTemplateInstantiationFunctionDecl:       unparseTemplateInstantiationFunctionDeclStmt(stmt, info); break;
          case V_SgTemplateInstantiationMemberFunctionDecl: unparseTemplateInstantiationMemberFunctionDeclStmt(stmt, info); break;
          case V_SgTemplateInstantiationDirectiveStatement: unparseTemplateInstantiationDirectiveStmt(stmt, info); break;

          case V_SgForInitStatement:                   unparseForInitStmt(stmt, info); break;

          case V_SgCatchStatementSeq:     // CATCH_STATEMENT_SEQ:
          case V_SgFunctionParameterList:  unparseFunctionParameterList(stmt, info); break;
          case V_SgCtorInitializerList:   // CTOR_INITIALIZER_LIST:
#if PRINT_DEVELOPER_WARNINGS
             printf ("Ignore these newly implemented cases (case of %s) \n",stmt->sage_class_name());
             printf ("WARNING: These cases must be implemented so that comments attached to them can be processed \n");
#endif
          // ROSE_ABORT();
             break;

          case V_SgNamespaceDeclarationStatement:      unparseNamespaceDeclarationStatement (stmt, info);      break;
          case V_SgNamespaceDefinitionStatement:       unparseNamespaceDefinitionStatement (stmt, info);       break;
          case V_SgNamespaceAliasDeclarationStatement: unparseNamespaceAliasDeclarationStatement (stmt, info); break;
          case V_SgUsingDirectiveStatement:            unparseUsingDirectiveStatement (stmt, info);            break;
          case V_SgUsingDeclarationStatement:          unparseUsingDeclarationStatement (stmt, info);          break;

          case V_SgTemplateInstantiationDefn:          unparseClassDefnStmt(stmt, info);   break;

       // DQ (4/16/2011): Added Java specific IR node for "import" statements.
          case V_SgJavaImportStatement:                unparseImportDeclarationStatement(stmt, info); break;

          default:
             {
               printf("Unparse_Java::unparseLanguageSpecificStatement: Error: No handler for %s (variant: %d)\n",stmt->sage_class_name(), stmt->variantT());
               ROSE_ASSERT(false);
               break;
             }
        }

        bool printSemicolon = true;
        switch (stmt->variantT()) {
            case V_SgClassDeclaration:
            case V_SgClassDefinition:
            case V_SgMemberFunctionDeclaration:
            case V_SgFunctionDefinition:
            case V_SgFunctionParameterList:
            case V_SgBasicBlock:
                printSemicolon = false;
        }
        if (printSemicolon) curprint(";");

        bool printNewline = true;
        switch (stmt->variantT()) {
            case V_SgFunctionParameterList:
                printNewline = false;
        }
        if (printNewline) unp->cur.insert_newline();
   }

void
Unparse_Java::unparseNestedStatement(SgStatement* stmt, SgUnparse_Info& info) {
    info.inc_nestingLevel();
    unparseStatement(stmt, info);
    info.dec_nestingLevel();
}

void
Unparse_Java::curprint_indented(const string str, SgUnparse_Info& info) const {
    unp->cur.insert_newline(0, 4 * info.get_nestingLevel());
    curprint(str);
}

void
Unparse_Java::unparseImportDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // There is a SgNamespaceDefinition, but it is not unparsed except through the SgNamespaceDeclaration

     SgJavaImportStatement* importDeclaration = isSgJavaImportStatement(stmt);
     ROSE_ASSERT (importDeclaration != NULL);
     curprint ( string("import "));

  // This can be an empty string (in the case of an unnamed namespace)
     SgName name = importDeclaration->get_path();
     curprint ( name.str());
     curprint ("; ");
   }

void
Unparse_Java::unparseName(SgName name, SgUnparse_Info& info)
   {
        curprint(name.getString());
   }

void
Unparse_Java::unparseNamespaceDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // There is a SgNamespaceDefinition, but it is not unparsed except through the SgNamespaceDeclaration

     SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceDeclaration != NULL);
     curprint ( string("namespace "));

  // This can be an empty string (in the case of an unnamed namespace)
     SgName name = namespaceDeclaration->get_name();
     curprint ( name.str());

     unparseStatement(namespaceDeclaration->get_definition(),info);
   }

void
Unparse_Java::unparseNamespaceDefinitionStatement ( SgStatement* stmt, SgUnparse_Info & info )
   {
     ROSE_ASSERT (stmt != NULL);
     SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(stmt);
     ROSE_ASSERT (namespaceDefinition != NULL);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (namespaceDefinition);
#endif

     SgUnparse_Info ninfo(info);

     ROSE_ASSERT(namespaceDefinition->get_namespaceDeclaration() != NULL);
     SgNamespaceDeclarationStatement *saved_namespace = ninfo.get_current_namespace();

#if 0
     if (saved_namespace != NULL)
          printf ("In unparseNamespaceDefinitionStatement(): saved_namespace = %p = %s \n",saved_namespace,saved_namespace->class_name().c_str());
#endif

     ninfo.set_current_namespace(NULL);
     ninfo.set_current_namespace(namespaceDefinition->get_namespaceDeclaration());

     unp->cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
     curprint ( string("{"));
     unp->cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);

  // unparse all the declarations
     SgDeclarationStatementPtrList & statementList = namespaceDefinition->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = statementList.begin();
     while ( statementIterator != statementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ROSE_ASSERT(currentStatement != NULL);

          unparseStatement(currentStatement, ninfo);

       // Go to the next statement
          statementIterator++;
        }

     unparseAttachedPreprocessingInfo(namespaceDefinition, info, PreprocessingInfo::inside);

     unp->cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
     curprint ( string("}\n"));
     unp->cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);

     ninfo.set_current_namespace(NULL);
     ninfo.set_current_namespace(saved_namespace);

#if 0
     if (saved_namespace != NULL)
          printf ("In unparseNamespaceDefinitionStatement(): reset saved_namespace = %p = %s \n",saved_namespace,saved_namespace->class_name().c_str());
#endif
   }

void
Unparse_Java::unparseNamespaceAliasDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceAliasDeclaration != NULL);

     curprint ( string("\nnamespace "));
     curprint ( namespaceAliasDeclaration->get_name().str());
     curprint ( string(" = "));
     ROSE_ASSERT(namespaceAliasDeclaration->get_namespaceDeclaration() != NULL);
     curprint ( namespaceAliasDeclaration->get_namespaceDeclaration()->get_name().str());
     curprint ( string(";\n"));
   }

void
Unparse_Java::unparseUsingDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(stmt);
     ROSE_ASSERT (usingDirective != NULL);
   }

void
Unparse_Java::unparseUsingDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(stmt);
     ROSE_ASSERT (usingDeclaration != NULL);

     curprint ( string("\nusing "));

     curprint ( string(";\n"));
   }

void 
Unparse_Java::unparseTemplateInstantiationDirectiveStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(stmt);
     ROSE_ASSERT(templateInstantiationDirective != NULL);

     SgDeclarationStatement* declarationStatement = templateInstantiationDirective->get_declaration();
     ROSE_ASSERT(declarationStatement != NULL);
   }


void
Unparse_Java::unparseTemplateInstantiationDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(stmt);
     ROSE_ASSERT(templateInstantiationDeclaration != NULL);

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(templateInstantiationDeclaration);
     ROSE_ASSERT(classDeclaration != NULL);

#if OUTPUT_DEBUGGING_CLASS_NAME
     printf ("Inside of unparseTemplateInstantiationDeclStmt() stmt = %p/%p name = %s  templateName = %s transformed = %s/%s prototype = %s compiler-generated = %s compiler-generated and marked for output = %s \n",
          classDeclaration,templateInstantiationDeclaration,
          templateInstantiationDeclaration->get_name().str(),
          templateInstantiationDeclaration->get_templateName().str(),
          unp->isTransformed (templateInstantiationDeclaration) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true) ? "true" : "false");
#endif

   }



void
Unparse_Java::unparseTemplateInstantiationFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = 
          isSgTemplateInstantiationFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationFunctionDeclaration != NULL);
     ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_file_info() != NULL);

     SgFunctionDeclaration* functionDeclaration = 
          isSgFunctionDeclaration(templateInstantiationFunctionDeclaration);

     ROSE_ASSERT(functionDeclaration != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_NAME
     printf ("Inside of unparseTemplateInstantiationFunctionDeclStmt() name = %s  transformed = %s prototype = %s static = %s compiler generated = %s transformed = %s output = %s \n",
       // templateInstantiationFunctionDeclaration->get_name().str(),
          templateInstantiationFunctionDeclaration->get_qualified_name().str(),
          isTransformed (templateInstantiationFunctionDeclaration) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true) ? "true" : "false");
#endif
   }

void
Unparse_Java::unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // Rules for output of member templates functions:
  //  1) When we unparse the template declaration as a string EDG removes the member 
  //     function definitions so we are forced to output all template member functions.
  //  2) If the member function is specified outside of the class then we don't have to
  //     explicitly output the instantiation.

     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = 
          isSgTemplateInstantiationMemberFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_NAME
     printf ("Inside of unparseTemplateInstantiationMemberFunctionDeclStmt() = %p name = %s  transformed = %s prototype = %s static = %s compiler generated = %s transformation = %s output = %s \n",
       // templateInstantiationMemberFunctionDeclaration->get_templateName().str(),
          templateInstantiationMemberFunctionDeclaration,
          templateInstantiationMemberFunctionDeclaration->get_qualified_name().str(),
          isTransformed (templateInstantiationMemberFunctionDeclaration) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true) ? "true" : "false");
#endif
   }


void
Unparse_Java::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ROSE_ASSERT(basic_stmt != NULL);

     curprint ("{");
     unp->cur.insert_newline();
     foreach (SgStatement* stmt, basic_stmt->get_statements()) {
         unparseNestedStatement(stmt, info);
         unp->cur.insert_newline();
     }
     curprint_indented ("}", info);
   }


// Determine how many "else {}"'s an outer if that has an else clause needs to
// prevent dangling if problems
static size_t countElsesNeededToPreventDangling(SgStatement* s) {
  // The basic rule here is that anything that has a defined end marker
  // (i.e., cannot end with an unmatched if statement) returns 0, everything
  // else (except if) gets the correct number of elses from its body
  switch (s->variantT()) {
    case V_SgCaseOptionStmt: return countElsesNeededToPreventDangling(isSgCaseOptionStmt(s)->get_body());
    case V_SgCatchStatementSeq: {
      SgCatchStatementSeq* cs = isSgCatchStatementSeq(s);
      const SgStatementPtrList& seq = cs->get_catch_statement_seq();
      ROSE_ASSERT (!seq.empty());
      return countElsesNeededToPreventDangling(seq.back());
    }
    case V_SgDefaultOptionStmt: return countElsesNeededToPreventDangling(isSgCaseOptionStmt(s)->get_body());
    case V_SgLabelStatement: return countElsesNeededToPreventDangling(isSgLabelStatement(s)->get_statement());
    case V_SgCatchOptionStmt: return countElsesNeededToPreventDangling(isSgCatchOptionStmt(s)->get_body());
    case V_SgForStatement: return countElsesNeededToPreventDangling(isSgForStatement(s)->get_loop_body());
    case V_SgIfStmt: {
      SgIfStmt* ifs = isSgIfStmt(s);
      if (ifs->get_false_body() != NULL) {
        return 0;
      } else {
        return countElsesNeededToPreventDangling(ifs->get_true_body()) + 1;
      }
    }
    case V_SgWhileStmt: return countElsesNeededToPreventDangling(isSgWhileStmt(s)->get_body());
    case V_SgSwitchStatement: ROSE_ASSERT(isSgBasicBlock(isSgSwitchStatement(s)->get_body())); return 0;
    default: return 0;
  }
}

void Unparse_Java::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {

  // printf ("Unparse if statement stmt = %p \n",stmt);

     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     assert (if_stmt != NULL);

     while (if_stmt != NULL)
        {
          SgStatement *tmp_stmt = NULL;
          curprint ( string("if ("));
          SgUnparse_Info testInfo(info);
          testInfo.set_SkipSemiColon();
          testInfo.set_inConditional();
       // info.set_inConditional();
          if ( (tmp_stmt = if_stmt->get_conditional()) )
             {
            // Unparse using base class function so we get any required comments and CPP directives.
            // unparseStatement(tmp_stmt, testInfo);
               UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, testInfo);
             }
          testInfo.unset_inConditional();
          curprint ( string(") "));

          if ( (tmp_stmt = if_stmt->get_true_body()) ) 
             {
            // printf ("Unparse the if true body \n");
            // curprint ( string("\n/* Unparse the if true body */ \n") );
               unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);

            // Unparse using base class function so we get any required comments and CPP directives.
            // unparseStatement(tmp_stmt, info);
               UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, info);

               unp->cur.format(tmp_stmt, info, FORMAT_AFTER_NESTED_STATEMENT);
            // curprint ( string("\n/* DONE: Unparse the if true body */ \n") );
             }

          if ( (tmp_stmt = if_stmt->get_false_body()) )
             {
               size_t elsesNeededForInnerIfs = countElsesNeededToPreventDangling(if_stmt->get_true_body());
               for (size_t i = 0; i < elsesNeededForInnerIfs; ++i) {
                 curprint ( string(" else {}") ); // Ensure this else does not match an inner if statement
               }
               unp->cur.format(if_stmt, info, FORMAT_BEFORE_STMT);
               curprint ( string("else "));
               if_stmt = isSgIfStmt(tmp_stmt);
               if (if_stmt == NULL) {
                 unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);

              // curprint ( string("\n/* Unparse the if false body */ \n") );
              // Unparse using base class function so we get any required comments and CPP directives.
              // unparseStatement(tmp_stmt, info);
                 UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, info);
              // curprint ( string("\n/* DONE: Unparse the if false body */ \n") );

                 unp->cur.format(tmp_stmt, info, FORMAT_AFTER_NESTED_STATEMENT);
               }
             }
            else
             {
               if_stmt = NULL;
             }

          if (if_stmt != NULL)
               unparseAttachedPreprocessingInfo(if_stmt, info, PreprocessingInfo::before);
        }
   }

void
Unparse_Java::unparseInitializedName(SgInitializedName* init_name, SgUnparse_Info& info) {
    unparseType(init_name->get_type(), info);
    unparseName(init_name->get_name(), info);

    if (init_name->get_initializer() != NULL) {
        curprint(" ");
        unparseExpression(init_name->get_initializer(), info);
    }
}

void
Unparse_Java::unparseForInitStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse for loop initializers \n");
     SgForInitStatement* forInitStmt = isSgForInitStatement(stmt);
     ROSE_ASSERT(forInitStmt != NULL);

     SgStatementPtrList::iterator i = forInitStmt->get_init_stmt().begin();

     SgUnparse_Info newinfo(info);

     while(i != forInitStmt->get_init_stmt().end())
        {
       // curprint(" /* unparseForInitStmt: " + (*i)->class_name() + " */ ");
          unparseStatement(*i, newinfo);
          i++;

       // After unparsing the first variable declaration with the type 
       // we want to unparse the rest without the base type.
          newinfo.set_SkipBaseType();

          if (i != forInitStmt->get_init_stmt().end())
             {
               curprint ( string(", "));
             }
        }

     curprint ( string("; "));
   }

void
Unparse_Java::unparseForStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse for loop \n");
     SgForStatement* for_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(for_stmt != NULL);

     curprint ( string("for ("));
     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
     newinfo.set_inConditional();  // set to prevent printing line and file information

  // curprint(" /* initializer */ ");
     SgStatement *tmp_stmt = for_stmt->get_for_init_stmt();
  // curprint(" /* initializer: " + tmp_stmt->class_name() + " */ ");
  // ROSE_ASSERT(tmp_stmt != NULL);
     if (tmp_stmt != NULL)
        {
          unparseStatement(tmp_stmt,newinfo);
        }
       else
        {
          printf ("Warning in unparseForStmt(): for_stmt->get_for_init_stmt() == NULL \n");
          curprint ( string("; "));
        }
     newinfo.unset_inConditional();

     SgStatement *test_stmt = for_stmt->get_test();
     ROSE_ASSERT(test_stmt != NULL);
  // if ( test_stmt != NULL )
     SgUnparse_Info testinfo(info);
     testinfo.set_SkipSemiColon();
     testinfo.set_inConditional();
  // printf ("Output the test in the for statement format testinfo.inConditional() = %s \n",testinfo.inConditional() ? "true" : "false");
     unparseStatement(test_stmt, testinfo);

     curprint ( string("; "));

  // curprint ( string(" /* increment */ ";
  // SgExpression *increment_expr = for_stmt->get_increment_expr();
     SgExpression *increment_expr = for_stmt->get_increment();
     ROSE_ASSERT(increment_expr != NULL);
     if ( increment_expr != NULL )
          unparseExpression(increment_expr, info);
     curprint ( string(") "));

  // Added support to output the header without the body to support the addition 
  // of more context in the prefix used with the AST Rewrite Mechanism.
  // if ( (tmp_stmt = for_stmt->get_loop_body()) )

     SgStatement* loopBody = for_stmt->get_loop_body();
     ROSE_ASSERT(loopBody != NULL);
  // printf ("loopBody = %p         = %s \n",loopBody,loopBody->class_name().c_str());
  // printf ("info.SkipBasicBlock() = %s \n",info.SkipBasicBlock() ? "true" : "false");

  // if ( (tmp_stmt = for_stmt->get_loop_body()) && !info.SkipBasicBlock())
     if ( (loopBody != NULL) && !info.SkipBasicBlock())
        {
       // printf ("Unparse the for loop body \n");
       // curprint ( string("\n/* Unparse the for loop body */ \n";
       // unparseStatement(tmp_stmt, info);

          unp->cur.format(loopBody, info, FORMAT_BEFORE_NESTED_STATEMENT);
          unparseStatement(loopBody, info);
          unp->cur.format(loopBody, info, FORMAT_AFTER_NESTED_STATEMENT);
       // curprint ( string("\n/* DONE: Unparse the for loop body */ \n";
        }
       else
        {
       // printf ("No for loop body to unparse! \n");
       // curprint ( string("\n/* No for loop body to unparse! */ \n";
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
   }


void
Unparse_Java::unparseExceptionSpecification(const SgTypePtrList& exceptionSpecifierList, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseExceptionSpecification() \n");
#endif

     curprint ( string(" throw("));
     if (!exceptionSpecifierList.empty())
        {
          SgTypePtrList::const_iterator i = exceptionSpecifierList.begin();
          while (i != exceptionSpecifierList.end())
             {
            // Handle class type as a special case to make sure the names are always output (see test2004_91.C).
            // unparseType(*i,info);
            // printf ("Note: Type found in function throw specifier type = %p = %s \n",*i,i->class_name().c_str());

               ROSE_ASSERT(*i != NULL);
               unp->u_type->unparseType(*i,info);

               i++;
               if (i != exceptionSpecifierList.end())
                  curprint ( string(","));
             }
        }
       else
        {
       // There was no exception specification list of types
        }
     curprint ( string(")"));
   }


void
Unparse_Java::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT(!"Unexpected function declaration. All java functions should be SgMemberFunctions");
   }


// NOTE: Bug in Sage: No file information provided for FuncDeclStmt. 
void
Unparse_Java::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionDefinition* funcdefn_stmt = isSgFunctionDefinition(stmt);
     ROSE_ASSERT(funcdefn_stmt != NULL);

     unparseStatement(funcdefn_stmt->get_body(), info);
   }

void
Unparse_Java::unparseFunctionParameterList(SgStatement* stmt, SgUnparse_Info& info)
{
    SgFunctionParameterList* param_list = isSgFunctionParameterList(stmt);
    ROSE_ASSERT(param_list != NULL);

    SgInitializedNamePtrList& names = param_list->get_args();
    SgInitializedNamePtrList::iterator name_it;
    for (name_it = names.begin(); name_it != names.end(); name_it++) {
        if (name_it != names.begin())
            curprint(", ");
        SgInitializedName* iname = *name_it;
        unparseType(iname->get_type(), info);
        curprint(" ");
        unparseName(iname->get_name(), info);
    }
}

string
Unparse_Java::trimGlobalScopeQualifier ( string qualifiedName )
   {
     string s = qualifiedName;
     size_t subStringLocationOfScopeQualifier = s.find("::");
  // printf ("Location of member function substring = %d \n",subStringLocationOfScopeQualifier);
     if (subStringLocationOfScopeQualifier == 0)
        {
       // printf ("Found global scope qualifier at start of function or member function name qualification \n");
          s.replace(s.find("::"),2,"");

       // reset the string in scopename!
          qualifiedName = s.c_str();
        }

     return qualifiedName;
   }


void
Unparse_Java::unparseMFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgMemberFunctionDeclaration* mfuncdecl_stmt = isSgMemberFunctionDeclaration(stmt);
     ROSE_ASSERT(mfuncdecl_stmt != NULL);

     //TODO should there be forward declarations or nondefining declarations?
     if (mfuncdecl_stmt->isForward()) {
         //cout << "unparser: skipping forward mfuncdecl: "
         //   << mfuncdecl_stmt->get_qualified_name().getString()
         //   << endl;
         return;
     } else if (mfuncdecl_stmt->get_definition() == NULL) {
         cout << "unparser: skipping nondefining mfuncdecl: "
              << mfuncdecl_stmt->get_qualified_name().getString()
              << endl;
         return;
     }

     unparseDeclarationModifier(mfuncdecl_stmt->get_declarationModifier(), info);

     //TODO remove when specialFxnModifier.isConstructor works
     bool constructor = mfuncdecl_stmt->get_specialFunctionModifier().isConstructor();
     bool name_match = mfuncdecl_stmt->get_name() == mfuncdecl_stmt->get_associatedClassDeclaration()->get_name();
     if (name_match && !constructor) {
         cout << "unparser: method " << mfuncdecl_stmt->get_qualified_name().getString()
              << " should be marked isConstructor" << endl;
         constructor = true;
     }

     // unparse type, unless this a constructor
     if (! constructor) {
         unparseType(mfuncdecl_stmt->get_type()->get_return_type(), info);
     }

     unparseName(mfuncdecl_stmt->get_name(), info);
     curprint("(");
     unparseStatement(mfuncdecl_stmt->get_parameterList(), info);
     curprint(") ");
     unparseStatement(mfuncdecl_stmt->get_definition(), info);

#if OUTPUT_DEBUGGING_FUNCTION_NAME
     printf ("Inside of unparseMFuncDeclStmt() name = %s  transformed = %s prototype = %s \n",
         mfuncdecl_stmt->get_qualified_name().str(),
         isTransformed (mfuncdecl_stmt) ? "true" : "false",
         (mfuncdecl_stmt->get_definition() == NULL) ? "true" : "false");
#endif
   }

void
Unparse_Java::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDeclaration* vardecl_stmt = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(vardecl_stmt != NULL);

     unparseDeclarationModifier(vardecl_stmt->get_declarationModifier(), info);
     foreach (SgInitializedName* init_name, vardecl_stmt->get_variables())
         unparseInitializedName(init_name, info);
   }


void
Unparse_Java::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
     ROSE_ASSERT(vardefn_stmt != NULL);
   }

void
Unparse_Java::initializeDeclarationsFromParent ( 
   SgDeclarationStatement* declarationStatement,
   SgClassDefinition* & cdefn,
   SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport )
   {
     ROSE_ASSERT(declarationStatement != NULL);
   }


void
Unparse_Java::unparseClassDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDeclaration* classdecl_stmt = isSgClassDeclaration(stmt);
     ROSE_ASSERT(classdecl_stmt != NULL);

     unparseDeclarationModifier(classdecl_stmt->get_declarationModifier(), info);

     curprint("class ");
     unparseName(classdecl_stmt->get_name(), info);
     //TODO inheritance
     unparseStatement(classdecl_stmt->get_definition(), info);
   }

void
Unparse_Java::unparseClassDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ROSE_ASSERT(classdefn_stmt != NULL);

     curprint(" {");
     unp->cur.insert_newline();
     foreach (SgDeclarationStatement* child, classdefn_stmt->get_members()) {
         unparseNestedStatement(child, info);
     }
     curprint_indented("}", info);
   }


void
Unparse_Java::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_stmt = isSgEnumDeclaration(stmt);
     ROSE_ASSERT(enum_stmt != NULL);

   }

void
Unparse_Java::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ROSE_ASSERT(expr_stmt != NULL);

     unparseExpression(expr_stmt->get_expression(), info);
   }

void Unparse_Java::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ROSE_ASSERT(label_stmt != NULL);

     curprint ( string(label_stmt->get_label().str()) + ":");
   }

void
Unparse_Java::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
  ROSE_ASSERT(while_stmt != NULL);

  curprint ( string("while" ) + "(");
  info.set_inConditional();
  
  unparseStatement(while_stmt->get_condition(), info);
  info.unset_inConditional();
  curprint ( string(")"));
  if(while_stmt->get_body()) {
    unp->cur.format(while_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
    unparseStatement(while_stmt->get_body(), info);
    unp->cur.format(while_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  }
  else if (!info.SkipSemiColon()) { curprint ( string(";")); }

}

void
Unparse_Java::unparseDoWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgDoWhileStmt* dowhile_stmt = isSgDoWhileStmt(stmt);
  ROSE_ASSERT(dowhile_stmt != NULL);

  curprint ( string("do "));
  unp->cur.format(dowhile_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
  unparseStatement(dowhile_stmt->get_body(), info);
  unp->cur.format(dowhile_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  curprint ( string("while " ) + "(");
  SgUnparse_Info ninfo(info);
  ninfo.set_inConditional();

  //we need to keep the properties of the prevnode (The next prevnode will set the
  //line back to where "do" was printed) 
// SgLocatedNode* tempnode = prevnode;

  unparseStatement(dowhile_stmt->get_condition(), ninfo);
  ninfo.unset_inConditional();
  curprint ( string(")")); 
  if (!info.SkipSemiColon()) { curprint ( string(";")); }
}

void
Unparse_Java::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
  
     ROSE_ASSERT(switch_stmt != NULL);

     curprint ( string("switch("));
  // unparseExpression(switch_stmt->get_item_selector(), info);
     
     SgUnparse_Info ninfo(info);
     ninfo.set_SkipSemiColon();
     ninfo.set_inConditional();
     unparseStatement(switch_stmt->get_item_selector(), ninfo);
     curprint ( string(")"));

     if ( (switch_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(switch_stmt->get_body(), info);
   }

void
Unparse_Java::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
     ROSE_ASSERT(case_stmt != NULL);

     curprint ( string("case "));
     unparseExpression(case_stmt->get_key(), info);
     curprint ( string(":"));

  // if(case_stmt->get_body())
     if ( (case_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(case_stmt->get_body(), info);
   }

void
Unparse_Java::unparseTryStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTryStmt* try_stmt = isSgTryStmt(stmt);
     ROSE_ASSERT(try_stmt != NULL);

     curprint ( string("try "));
  
     unp->cur.format(try_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(try_stmt->get_body(), info);
     unp->cur.format(try_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  
     SgStatementPtrList::iterator i=try_stmt->get_catch_statement_seq().begin();
     while (i != try_stmt->get_catch_statement_seq().end())
        {
          unparseStatement(*i, info);
          i++;
        }
   }

void
Unparse_Java::unparseCatchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCatchOptionStmt* catch_statement = isSgCatchOptionStmt(stmt);
     ROSE_ASSERT(catch_statement != NULL);

     curprint ( string("catch " ) + "(");
     if (catch_statement->get_condition())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inVarDecl();

          ninfo.set_SkipSemiColon();
          ninfo.set_SkipClassSpecifier();
          unparseStatement(catch_statement->get_condition(), ninfo);
        }

     curprint ( string(")"));
  // if (catch_statement->get_condition() == NULL) prevnode = catch_statement;

     unp->cur.format(catch_statement->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(catch_statement->get_body(), info);
     unp->cur.format(catch_statement->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
   }

void
Unparse_Java::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
     ROSE_ASSERT(default_stmt != NULL);

     curprint ( string("default:"));
  // if(default_stmt->get_body()) 
     if ( (default_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(default_stmt->get_body(), info);
   }

void
Unparse_Java::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
  ROSE_ASSERT(break_stmt != NULL);

  curprint ( string("break; "));
}

void
Unparse_Java::unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgContinueStmt* continue_stmt = isSgContinueStmt(stmt);
  ROSE_ASSERT(continue_stmt != NULL);

  curprint ( string("continue; "));
}

void
Unparse_Java::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
     ROSE_ASSERT(return_stmt != NULL);

     curprint ( string("return "));
     SgUnparse_Info ninfo(info);

     if (return_stmt->get_expression())
        {
          unparseExpression(return_stmt->get_expression(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_Java::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
  ROSE_ASSERT(goto_stmt != NULL);

  curprint ( string("goto " ) + goto_stmt->get_label()->get_label().str());
  if (!info.SkipSemiColon()) { curprint ( string(";")); }
}


void
Unparse_Java::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTypedefDeclaration* typedef_stmt = isSgTypedefDeclaration(stmt);
     ROSE_ASSERT(typedef_stmt != NULL);

  // printf ("In unp->u_type->unparseTypeDefStmt() = %p \n",typedef_stmt);
  // curprint ( string("\n /* In unp->u_type->unparseTypeDefStmt() */ \n";

  // info.display ("At base of unp->u_type->unparseTypeDefStmt()");
   }

void
Unparse_Java::unparseTemplateDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
   }

void
Unparse_Java::unparseDeclarationModifier(SgDeclarationModifier& mod, SgUnparse_Info& info) {
       if (mod.isJavaAbstract()) curprint("abstract ");
       if (mod.isFinal()) curprint("final ");
       unparseAccessModifier(mod.get_accessModifier(), info);
       unparseTypeModifier(mod.get_typeModifier(), info);
       unparseStorageModifier(mod.get_storageModifier(), info);
}

void
Unparse_Java::unparseAccessModifier(SgAccessModifier& mod, SgUnparse_Info& info) {
    if      (mod.isPublic())    curprint("public ");
    else if (mod.isProtected()) curprint("protected ");
    else if (mod.isPrivate())   curprint("private ");
}

void
Unparse_Java::unparseStorageModifier(SgStorageModifier& mod, SgUnparse_Info& info) {
    if (mod.isStatic()) curprint("static ");
}

void
Unparse_Java::unparseConstVolatileModifier(SgConstVolatileModifier& mod, SgUnparse_Info& info) {
    if (mod.isVolatile()) curprint("volatile ");
    if (mod.isJavaTransient()) curprint("transient ");
}

void
Unparse_Java::unparseTypeModifier(SgTypeModifier& mod, SgUnparse_Info& info) {
    unparseConstVolatileModifier(mod.get_constVolatileModifier(), info);
}

void
Unparse_Java::unparseFunctionModifier(SgFunctionModifier& mod, SgUnparse_Info& info) {
    if (mod.isJavaSynchronized()) curprint("synchronized ");
    if (mod.isJavaNative()) curprint("native ");
    if (mod.isJavaStrictfp()) curprint("strictfp ");
}


