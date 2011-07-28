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
 * JavaCodeGeneration_locatedNode::unparse is called, and for statements, 
 * JavaCodeGeneration_locatedNode::unparseStatement is called.
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


JavaCodeGeneration_locatedNode::JavaCodeGeneration_locatedNode(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
  // Nothing to do here!
   }

JavaCodeGeneration_locatedNode::~JavaCodeGeneration_locatedNode()
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
JavaCodeGeneration_locatedNode::unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info)
   {
     ROSE_ASSERT(con_init != NULL);
  /* code inserted from specification */

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("\n\nInside of JavaCodeGeneration_locatedNode::unparseOneElemConInit (%p) \n",con_init);
     curprint( "\n /* Inside of JavaCodeGeneration_locatedNode::unparseOneElemConInit */ \n");
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving of JavaCodeGeneration_locatedNode::unparseOneElemConInit \n\n\n");
     curprint( "\n /* Leaving of JavaCodeGeneration_locatedNode::unparseOneElemConInit */ \n");
#endif
   }

void
JavaCodeGeneration_locatedNode::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function

     ROSE_ASSERT(stmt != NULL);

#if 0
  // Debugging support
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
     if (declarationStatement != NULL)
        {
          curprint( "/* In unparseLanguageSpecificStatement(): declarationStatement->get_declarationModifier().isFriend() = ");
          declarationStatement->get_declarationModifier().isFriend() ? curprint("true") : curprint("false");
          curprint( "*/ \n ");
        }
#endif

#if 0
     curprint ( string("\n/* Top of unparseLanguageSpecificStatement (JavaCodeGeneration_locatedNode) " ) + stmt->class_name() + " */\n ");
     ROSE_ASSERT(stmt->get_startOfConstruct() != NULL);
  // ROSE_ASSERT(stmt->getAttachedPreprocessingInfo() != NULL);
     int numberOfComments = -1;
     if (stmt->getAttachedPreprocessingInfo() != NULL)
          numberOfComments = stmt->getAttachedPreprocessingInfo()->size();
     curprint ( string("/* startOfConstruct: file = " ) + stmt->get_startOfConstruct()->get_filenameString()
         + " raw filename = " + stmt->get_startOfConstruct()->get_raw_filename()
         + " raw line = "     + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_line())
         + " raw column = "   + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_col())
         + " #comments = "    + StringUtility::numberToString(numberOfComments)
         + " */\n ");
#endif

#if ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE
     printf ("In unparseLanguageSpecificStatement(): file = %s line = %d \n",stmt->get_startOfConstruct()->get_filenameString().c_str(),stmt->get_startOfConstruct()->get_line());
#endif

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
               printf("JavaCodeGeneration_locatedNode::unparseLanguageSpecificStatement: Error: No handler for %s (variant: %d)\n",stmt->sage_class_name(), stmt->variantT());
               ROSE_ASSERT(false);
               break;
             }
        }
   }

void
JavaCodeGeneration_locatedNode::unparseImportDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseNamespaceDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseNamespaceDefinitionStatement ( SgStatement* stmt, SgUnparse_Info & info )
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
JavaCodeGeneration_locatedNode::unparseNamespaceAliasDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseUsingDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(stmt);
     ROSE_ASSERT (usingDirective != NULL);
   }

void
JavaCodeGeneration_locatedNode::unparseUsingDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(stmt);
     ROSE_ASSERT (usingDeclaration != NULL);

     curprint ( string("\nusing "));

     curprint ( string(";\n"));
   }

void 
JavaCodeGeneration_locatedNode::unparseTemplateInstantiationDirectiveStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(stmt);
     ROSE_ASSERT(templateInstantiationDirective != NULL);

     SgDeclarationStatement* declarationStatement = templateInstantiationDirective->get_declaration();
     ROSE_ASSERT(declarationStatement != NULL);
   }


void
JavaCodeGeneration_locatedNode::unparseTemplateInstantiationDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseTemplateInstantiationFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ROSE_ASSERT(basic_stmt != NULL);

  // unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::before);

     unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
     curprint ( string("{"));
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

     if (basic_stmt->get_asm_function_body().empty() == false)
        {
       // This is an asm function body.
          curprint (basic_stmt->get_asm_function_body());

       // Make sure this is a function definition.
          ROSE_ASSERT(isSgFunctionDefinition(basic_stmt->get_parent()) != NULL);
        }

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (basic_stmt);
#endif

     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     while(p != basic_stmt->get_statements().end())
        { 
          ROSE_ASSERT((*p) != NULL);

          unparseStatement((*p), info);

          p++;
        }

     unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::inside);

     unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
     curprint ( string("}"));
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
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

void JavaCodeGeneration_locatedNode::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseForInitStmt (SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseForStmt(SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseExceptionSpecification(const SgTypePtrList& exceptionSpecifierList, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT(!"Unexpected function declaration. All java functions should be SgMemberFunctions");
   }


// NOTE: Bug in Sage: No file information provided for FuncDeclStmt. 
void
JavaCodeGeneration_locatedNode::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDefnStmt() \n");
     curprint ( string("/* Inside of JavaCodeGeneration_locatedNode::unparseFuncDefnStmt */"));
#endif

     SgFunctionDefinition* funcdefn_stmt = isSgFunctionDefinition(stmt);
     ROSE_ASSERT(funcdefn_stmt != NULL);

  // Unparse any comments of directives attached to the SgFunctionParameterList
     unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::after);
   }

void
JavaCodeGeneration_locatedNode::unparseFunctionParameterList(SgStatement* stmt, SgUnparse_Info& info)
{
    SgFunctionParameterList* param_list = isSgFunctionParameterList(stmt);
    ROSE_ASSERT(param_list != NULL);

    SgInitializedNamePtrList& names = param_list->get_args();
    SgInitializedNamePtrList::iterator name_it;
    for (name_it = names.begin(); name_it != names.end(); name_it++) {
        if (name_it != names.begin())
            curprint(", ");
        SgInitializedName* iname = *name_it;
        unp->u_type->unparseType(iname->get_type(), info);
        curprint(iname->get_name().getString());
    }
}

string
JavaCodeGeneration_locatedNode::trimGlobalScopeQualifier ( string qualifiedName )
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
JavaCodeGeneration_locatedNode::unparseMFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgMemberFunctionDeclaration* mfuncdecl_stmt = isSgMemberFunctionDeclaration(stmt);
     ROSE_ASSERT(mfuncdecl_stmt != NULL);

     //TODO should there be forward declarations or nondefining declarations?
     if (mfuncdecl_stmt->isForward()) {
         cout << "unparser: skipping forward mfuncdecl: "
              << mfuncdecl_stmt->get_qualified_name().getString()
              << endl;
         return;
     } else if (mfuncdecl_stmt->get_definition() == NULL) {
         cout << "unparser: skipping nondefining mfuncdecl: "
              << mfuncdecl_stmt->get_qualified_name().getString()
              << endl;
         return;
     }

     //TODO more complete handling of modifiers
     SgStorageModifier& storage = mfuncdecl_stmt->get_declarationModifier().get_storageModifier();
     SgAccessModifier& access = mfuncdecl_stmt->get_declarationModifier().get_accessModifier();
     SgSpecialFunctionModifier& special = mfuncdecl_stmt->get_specialFunctionModifier();
     if (access.isPublic()) curprint("public ");
     if (storage.isStatic()) curprint("static ");

     //TODO remove when specialFxnModifier.isConstructor works
     bool name_match = mfuncdecl_stmt->get_name() == mfuncdecl_stmt->get_associatedClassDeclaration()->get_name();
     if (name_match && !special.isConstructor()) {
         cout << "unparser: method " << mfuncdecl_stmt->get_qualified_name().getString()
              << " should be marked isConstructor" << endl;
     }

     // unparse type, unless this a constructor
     if (!special.isConstructor() && !name_match) {
         info.set_isTypeFirstPart();
         unp->u_type->unparseType(mfuncdecl_stmt->get_type(), info);
         info.unset_isTypeFirstPart();
     }

     curprint(mfuncdecl_stmt->get_name().getString());
     curprint("(");
     unparseStatement(mfuncdecl_stmt->get_parameterList(), info);
     curprint(") {\n");
     unparseStatement(mfuncdecl_stmt->get_definition(), info);
     curprint("}\n");

#if 0
     curprint ( string("\n/* Inside of JavaCodeGeneration_locatedNode::unparseMFuncDeclStmt */ \n") ); 
#endif

#if OUTPUT_DEBUGGING_FUNCTION_NAME
     printf ("Inside of unparseMFuncDeclStmt() name = %s  transformed = %s prototype = %s \n",
         mfuncdecl_stmt->get_qualified_name().str(),
         isTransformed (mfuncdecl_stmt) ? "true" : "false",
         (mfuncdecl_stmt->get_definition() == NULL) ? "true" : "false");
#endif

  // Unparse any comments of directives attached to the SgCtorInitializerList
     if (mfuncdecl_stmt->get_CtorInitializerList() != NULL)
          unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), info, PreprocessingInfo::after);
   }

void
JavaCodeGeneration_locatedNode::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseVarDeclStmt(%p) \n",stmt);
     ROSE_ASSERT(info.get_current_scope() != NULL);
     printf ("An the current scope is (from info): info.get_current_scope() = %p = %s = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str(),SageInterface::get_name(info.get_current_scope()).c_str());
     curprint ( string("\n /* Inside of unparseVarDeclStmt() */ \n"));
#endif

     SgVariableDeclaration* vardecl_stmt = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(vardecl_stmt != NULL);

#if 0
     printf ("Leaving unparseVarDeclStmt() \n");
     curprint ( string("/* Leaving unparseVarDeclStmt() */ \n";
#endif
   }


void
JavaCodeGeneration_locatedNode::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
     ROSE_ASSERT(vardefn_stmt != NULL);
   }


void
JavaCodeGeneration_locatedNode::initializeDeclarationsFromParent ( 
   SgDeclarationStatement* declarationStatement,
   SgClassDefinition* & cdefn,
   SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport )
   {
     ROSE_ASSERT(declarationStatement != NULL);
   }


void
JavaCodeGeneration_locatedNode::unparseClassDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDeclaration* classdecl_stmt = isSgClassDeclaration(stmt);
     ROSE_ASSERT(classdecl_stmt != NULL);

#if 0
  // printf ("Inside of JavaCodeGeneration_locatedNode::unparseClassDeclStmt(): classdecl_stmt->get_from_template() = %s \n",
  //      classdecl_stmt->get_from_template() ? "true" : "false");
     if (classdecl_stmt->get_from_template() == true)
          curprint ( string("/* Unparser comment: Templated Class Declaration Function */"));
     Sg_File_Info* classDeclarationfileInfo = classdecl_stmt->get_file_info();
     ROSE_ASSERT ( classDeclarationfileInfo != NULL );
     if ( classDeclarationfileInfo->isCompilerGenerated() == false)
          curprint ( string("\n/* file: " ) + classDeclarationfileInfo->get_filename() + " line: " + classDeclarationfileInfo->get_line() + " col: " + classDeclarationfileInfo->get_col() + " */ \n");
#endif

     curprint("class ");
     curprint(classdecl_stmt->get_name().getString());
     curprint(" {\n");
     unparseStatement(classdecl_stmt->get_definition(), info);
     curprint("}\n");
   }

void
JavaCodeGeneration_locatedNode::unparseClassDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ROSE_ASSERT(classdefn_stmt != NULL);

     foreach (SgDeclarationStatement* child, classdefn_stmt->get_members())
         unparseStatement(child, info);
   }


void
JavaCodeGeneration_locatedNode::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_stmt = isSgEnumDeclaration(stmt);
     ROSE_ASSERT(enum_stmt != NULL);

   }

void
JavaCodeGeneration_locatedNode::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ROSE_ASSERT(expr_stmt != NULL);

     SgUnparse_Info newinfo(info);

  // Expressions are another place where a class definition should NEVER be unparsed
     newinfo.set_SkipClassDefinition();

     if (expr_stmt->get_expression())
        {
          unparseExpression(expr_stmt->get_expression(), newinfo);
        }
       else
          assert(false);

     if (newinfo.inVarDecl())
        {
          curprint ( string(","));
        }
       else
          if (!newinfo.inConditional() && !newinfo.SkipSemiColon())
             {
               curprint ( string(";"));
             }
   }

void JavaCodeGeneration_locatedNode::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ROSE_ASSERT(label_stmt != NULL);

     curprint ( string(label_stmt->get_label().str()) + ":");
   }

void
JavaCodeGeneration_locatedNode::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
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
JavaCodeGeneration_locatedNode::unparseDoWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
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
JavaCodeGeneration_locatedNode::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseTryStmt(SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseCatchStmt(SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
     ROSE_ASSERT(default_stmt != NULL);

     curprint ( string("default:"));
  // if(default_stmt->get_body()) 
     if ( (default_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(default_stmt->get_body(), info);
   }

void
JavaCodeGeneration_locatedNode::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
  ROSE_ASSERT(break_stmt != NULL);

  curprint ( string("break; "));
}

void
JavaCodeGeneration_locatedNode::unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgContinueStmt* continue_stmt = isSgContinueStmt(stmt);
  ROSE_ASSERT(continue_stmt != NULL);

  curprint ( string("continue; "));
}

void
JavaCodeGeneration_locatedNode::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
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
JavaCodeGeneration_locatedNode::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
  ROSE_ASSERT(goto_stmt != NULL);

  curprint ( string("goto " ) + goto_stmt->get_label()->get_label().str());
  if (!info.SkipSemiColon()) { curprint ( string(";")); }
}


void
JavaCodeGeneration_locatedNode::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTypedefDeclaration* typedef_stmt = isSgTypedefDeclaration(stmt);
     ROSE_ASSERT(typedef_stmt != NULL);

  // printf ("In unp->u_type->unparseTypeDefStmt() = %p \n",typedef_stmt);
  // curprint ( string("\n /* In unp->u_type->unparseTypeDefStmt() */ \n";

  // info.display ("At base of unp->u_type->unparseTypeDefStmt()");
   }

void
JavaCodeGeneration_locatedNode::unparseTemplateDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
   }
 



