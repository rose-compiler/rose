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


//
// 
//
void 
Unparse_Java::unparseJavaFile(SgSourceFile *sourcefile, SgUnparse_Info& info) {
    if (sourcefile -> attributeExists("error")) {
// TODO: Remove this !
/*
cout << "*** @ Don't unparseJavaFile " << sourcefile -> getFileName()
<< endl;
cout.flush();
*/
        sourcefile -> set_unparserErrorCode(1);
        return;
    }

// TODO: Remove this !
/*
cout << "*** @ unparseJavaFile " << sourcefile -> getFileName()
<< endl;
cout.flush();
*/

    SgJavaPackageStatement *package_statement = sourcefile -> get_package();
// TODO: Remove this !
/*
cout << "*** @ unparseJavaFile on " << sourcefile -> getFileName()
<< endl;
cout.flush();
*/

    //
    // Process the package declaration statement.
    //
    if (package_statement) {
        SgName package_name = package_statement -> get_name();
        if (package_name.getString().size() > 0) { // not the null package name?
            unparseStatement(package_statement, info);
        }
    }
// TODO: Remove this !
/*
else{
cout << "*** @ No package statement" << endl;
cout.flush();
}
*/

    //
    // Process the import declaration statements.
    //
    if (sourcefile -> get_import_list()) {
        vector<SgJavaImportStatement *> &import_list = sourcefile -> get_import_list() -> get_java_import_list();
        for (size_t i = 0; i < import_list.size(); i++) {
            SgJavaImportStatement *import_declaration = import_list[i];
// TODO: Remove this !
/*
cout << "*** @ Import declaration " << i << endl;
cout.flush();
*/
            unparseStatement(import_declaration, info);
        }
    }
// TODO: Remove this !
/*
if (import_list.size() == 0) {
cout << "*** @ No imports" << endl;
cout.flush();
}
*/

    //
    // Process the class declarations.
    //
    if (sourcefile -> get_class_list()) {
        vector<SgClassDeclaration *> &type_list = sourcefile -> get_class_list() -> get_java_class_list();
        for (size_t i = 0; i < type_list.size(); i++) {
            SgClassDeclaration *type_declaration = type_list[i];
// TODO: Remove this !
/*
cout << "*** @ type " << i << ": " << type_declaration -> get_qualified_name().str() << endl;
cout.flush();
AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) type_declaration -> getAttribute("sourcefile");
ROSE_ASSERT(isSgSourceFile(attribute -> getNode()));
ROSE_ASSERT(attribute -> getNode() == sourcefile);
*/

            unparseStatement(type_declaration, info);
        }
    }
// TODO: Remove this !
/*
if (type_list.size() == 0) {
cout << "*** @ No types" << endl;
cout.flush();
}
*/
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
     ASSERT_not_null(con_init);
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

     ASSERT_not_null(stmt);
// TODO: Remove this!
/*
cout << "*** Processing statement " << stmt -> class_name() << endl;
cout.flush();
*/

     curprint_indented("", info);
     switch (stmt->variantT())
        {
       // DQ (3/14/2011): Need to move the Java specific unparse member functions from the base class to this function.

//          case V_SgGlobal:                   cout << "Got it !!!" << endl; /* unparseGlobalStmt   (stmt, info); */ break;

       // declarations
       // case V_SgInterfaceStatement:     unparseInterfaceStmt(stmt, info); break;
          case V_SgVariableDeclaration:    unparseVarDeclStmt  (stmt, info); break;
          case V_SgVariableDefinition:     unparseVarDefnStmt  (stmt, info); break;

       // executable statements, control flow
          case V_SgBasicBlock:             unparseBasicBlockStmt (stmt, info); break;
          case V_SgIfStmt:                 unparseIfStmt         (stmt, info); break;
          case V_SgJavaSynchronizedStatement: unparseSynchronizedStmt (stmt, info); break;
          case V_SgJavaThrowStatement:     unparseThrowStmt      (stmt, info); break;
          case V_SgJavaForEachStatement:   unparseForEachStmt    (stmt, info); break;

          case V_SgWhileStmt:              unparseWhileStmt      (stmt, info); break;
          case V_SgSwitchStatement:        unparseSwitchStmt     (stmt, info); break;
          case V_SgCaseOptionStmt:         unparseCaseStmt       (stmt, info); break;
          case V_SgDefaultOptionStmt:      unparseDefaultStmt    (stmt, info); break;
          case V_SgBreakStmt:              unparseBreakStmt      (stmt, info); break;
          case V_SgJavaLabelStatement:     unparseLabelStmt      (stmt, info); break;
          case V_SgGotoStatement:          unparseGotoStmt       (stmt, info); break;
          case V_SgReturnStmt:             unparseReturnStmt     (stmt, info); break;
          case V_SgAssertStmt:             unparseAssertStmt     (stmt, info); break;
          case V_SgNullStatement:          curprint("");/* Tab over for stmt*/ break;

          case V_SgForStatement:           unparseForStmt(stmt, info);          break; 
          case V_SgFunctionDeclaration:    unparseFuncDeclStmt(stmt, info);     break;
          case V_SgFunctionDefinition:     unparseFuncDefnStmt(stmt, info);     break;
          case V_SgMemberFunctionDeclaration: unparseMFuncDeclStmt(stmt, info); break;

          case V_SgJavaPackageStatement:   unparseJavaPackageStmt(stmt, info);  break;
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

        bool printSemicolon;
        switch (stmt->variantT()) {
            case V_SgClassDeclaration:
            case V_SgClassDefinition:
            case V_SgMemberFunctionDeclaration:
            case V_SgFunctionDefinition:
            case V_SgFunctionParameterList:
            case V_SgForInitStatement:
            case V_SgBasicBlock:
            case V_SgWhileStmt:
            case V_SgForStatement:
            case V_SgJavaForEachStatement:
            case V_SgIfStmt:
            case V_SgSwitchStatement:
            case V_SgTryStmt:
            case V_SgCaseOptionStmt:
            case V_SgCatchOptionStmt:
            case V_SgDefaultOptionStmt:
            case V_SgJavaLabelStatement:
            case V_SgJavaSynchronizedStatement:

                printSemicolon = false;
                break;
            case V_SgVariableDeclaration: // charles4 09/23/2011 -- Shouldn't this be the default initialization!
                printSemicolon = ! info.SkipSemiColon();
                break;
            default:
                printSemicolon = true;
        }
        if (printSemicolon) curprint(";");

        bool printNewline;
        switch (stmt->variantT()) {
            case V_SgForInitStatement:
            case V_SgFunctionParameterList:
                printNewline = false;
                break;
            default:
                printNewline = true;
        }
        if (printNewline) unp->cur.insert_newline();
   }

// TODO: Remove this!
/*
void
Unparse_Java::unparseGlobalStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgGlobal* globalScope = isSgGlobal(stmt);
     ASSERT_not_null(globalScope);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (globalScope);
#endif


#if 0
     int declarationCounter = 0;
#endif

  // Setup an iterator to go through all the statements in the top scope of the file.
     SgDeclarationStatementPtrList & globalStatementList = globalScope->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = globalStatementList.begin();
     while ( statementIterator != globalStatementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ASSERT_not_null(currentStatement);

#if 0
          printf ("In unparseGlobalStmt(): declaration #%d is %p = %s = %s \n",declarationCounter++,currentStatement,currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
#endif

          if (ROSE_DEBUG > 3)
             {
            // (*primary_os)
               cout << "In run_unparser(): getLineNumber(currentStatement) = "
#if 1
                    << currentStatement->get_file_info()->displayString()
#else
                    << Rose::getLineNumber(currentStatement)
                    << " getFileName(currentStatement) = " 
                    << Rose::getFileName(currentStatement)
#endif
                    << " unp->cur_index = " 
                    << unp->cur_index
                    << endl;
             }

       // DQ (6/4/2007): Make a new SgUnparse_Info object for each statement in global scope
       // This should permit children to set the current_scope and not effect other children
       // see test2007_56.C for example "namespace A { extern int x; } int A::x = 42;"
       // Namespace definition scope should not effect scope set in SgGlobal.
       // unparseStatement(currentStatement, info);
          SgUnparse_Info infoLocal(info);
          unparseStatement(currentStatement, infoLocal);

       // Go to the next statement
          statementIterator++;
        }

  // DQ (5/27/2005): Added support for compiler-generated statements that might appear at the end of the applications
  // printf ("At end of unparseGlobalStmt \n");
  // outputCompilerGeneratedStatements(info);

  // DQ (4/21/2005): Output a new line at the end of the file (some compilers complain if this is not present)
     unp->cur.insert_newline(1);
}
*/

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
     SgJavaImportStatement* importDeclaration = isSgJavaImportStatement(stmt);
     ASSERT_not_null(importDeclaration);

     curprint("import ");
     if (importDeclaration -> get_declarationModifier().get_storageModifier().isStatic())
         curprint("static ");
     unparseName(importDeclaration->get_path(), info);
     if (importDeclaration -> get_containsWildCard())
         curprint(".*");
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
     ASSERT_not_null(namespaceDeclaration);
     curprint ( string("namespace "));

  // This can be an empty string (in the case of an unnamed namespace)
     SgName name = namespaceDeclaration->get_name();
     curprint ( name.str());

     unparseStatement(namespaceDeclaration->get_definition(),info);
   }

void
Unparse_Java::unparseNamespaceDefinitionStatement ( SgStatement* stmt, SgUnparse_Info & info )
   {
     ASSERT_not_null(stmt);
     SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(stmt);
     ASSERT_not_null(namespaceDefinition);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (namespaceDefinition);
#endif

     SgUnparse_Info ninfo(info);

     ASSERT_not_null(namespaceDefinition->get_namespaceDeclaration());
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
          ASSERT_not_null(currentStatement);

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
     ASSERT_not_null(namespaceAliasDeclaration);

     curprint ( string("\nnamespace "));
     curprint ( namespaceAliasDeclaration->get_name().str());
     curprint ( string(" = "));
     ASSERT_not_null(namespaceAliasDeclaration->get_namespaceDeclaration());
     curprint ( namespaceAliasDeclaration->get_namespaceDeclaration()->get_name().str());
     curprint ( string(";\n"));
   }

void
Unparse_Java::unparseUsingDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(stmt);
     ASSERT_not_null(usingDirective);
   }

void
Unparse_Java::unparseUsingDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(stmt);
     ASSERT_not_null(usingDeclaration);

     curprint ( string("\nusing "));

     curprint ( string(";\n"));
   }

void 
Unparse_Java::unparseTemplateInstantiationDirectiveStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(stmt);
     ASSERT_not_null(templateInstantiationDirective);

     SgDeclarationStatement* declarationStatement = templateInstantiationDirective->get_declaration();
     ASSERT_not_null(declarationStatement);
   }


void
Unparse_Java::unparseTemplateInstantiationDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(stmt);
     ASSERT_not_null(templateInstantiationDeclaration);

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(templateInstantiationDeclaration);
     ASSERT_not_null(classDeclaration);

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
     ASSERT_not_null(templateInstantiationFunctionDeclaration);
     ASSERT_not_null(templateInstantiationFunctionDeclaration->get_file_info());

     SgFunctionDeclaration* functionDeclaration = 
          isSgFunctionDeclaration(templateInstantiationFunctionDeclaration);

     ASSERT_not_null(functionDeclaration);

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
     ASSERT_not_null(templateInstantiationMemberFunctionDeclaration);

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
     ASSERT_not_null(basic_stmt);

     curprint ("{");
     unp->cur.insert_newline();
     foreach (SgStatement* stmt, basic_stmt->get_statements()) {
         unparseNestedStatement(stmt, info);
         unp->cur.insert_newline();
     }
     curprint_indented ("}", info);
   }


void
Unparse_Java::unparseCaseOrDefaultBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ASSERT_not_null(basic_stmt);

     // curprint ("{");
     foreach (SgStatement* stmt, basic_stmt->get_statements()) {
         unp->cur.insert_newline();
         unparseNestedStatement(stmt, info);
     }
     // curprint_indented ("}", info);
   }

#if 0
 // DQ (3/28/2017): Eliminate warning about unused function from Clang.

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
    case V_SgJavaLabelStatement: return countElsesNeededToPreventDangling(isSgJavaLabelStatement(s)->get_statement());
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
#endif


void Unparse_Java::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgIfStmt* if_stmt = isSgIfStmt(stmt);
    ASSERT_not_null(if_stmt);

    SgExprStatement* cond_stmt = isSgExprStatement(if_stmt->get_conditional());
    ROSE_ASSERT(cond_stmt != NULL && "expected an SgExprStatement in SgIfStmt::p_conditional");
    SgExpression* conditional = cond_stmt->get_expression();

    curprint("if (");
    unparseExpression(conditional, info);
    curprint(")");

    if (if_stmt->get_true_body() != NULL) {
        curprint(" ");
        unparseStatement(if_stmt->get_true_body(), info);
    } else {
        curprint(";");
    }

    if (if_stmt->get_false_body() != NULL) {
        curprint_indented("else ", info);
        unparseStatement(if_stmt->get_false_body(), info);
    }
}

void Unparse_Java::unparseSynchronizedStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgJavaSynchronizedStatement *sync_stmt = isSgJavaSynchronizedStatement(stmt);
    ASSERT_not_null(sync_stmt);

    curprint("synchronized (");
    unparseExpression(sync_stmt->get_expression(), info);
    curprint(")");

    unparseStatement(sync_stmt->get_body(), info);
}

void Unparse_Java::unparseThrowStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgJavaThrowStatement *throw_stmt = isSgJavaThrowStatement(stmt);
    ASSERT_not_null(throw_stmt);

    curprint("throw ");
    unparseExpression(throw_stmt->get_throwOp()->get_operand(), info);
}

void Unparse_Java::unparseForEachStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgJavaForEachStatement *foreach_stmt = isSgJavaForEachStatement(stmt);
    ASSERT_not_null(foreach_stmt);

    curprint("for (");

    ROSE_ASSERT(foreach_stmt -> get_element()->get_variables().size() == 1);
    unparseVarDeclStmt(foreach_stmt -> get_element(), info);

    curprint(" : ");
    unparseExpression(foreach_stmt -> get_collection(), info);
    curprint(")");

    if (foreach_stmt->get_loop_body() != NULL) {
        curprint(" ");
        unparseStatement(foreach_stmt -> get_loop_body(), info);
    } else {
        curprint(";");
    }
}

void
Unparse_Java::unparseInitializedName(SgInitializedName* init_name, SgUnparse_Info& info) {
    AstSgNodeAttribute *alias_attribute = (AstSgNodeAttribute *) init_name -> getAttribute("real_name");
    if (alias_attribute) { // is this the name of a parameter with an alias?
        init_name = isSgInitializedName(alias_attribute -> getNode());
    }

    if (init_name -> attributeExists("final")) {
        curprint("final ");
    }

// TODO: Remove this !!!
/*
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) init_name -> getAttribute("var_args");
    if (attribute) {
        SgType *element_type = isSgType(attribute -> getNode());
        unparseType(element_type, info);
        curprint("... ");
    }
    else {
        unparseType(init_name->get_type(), info);
        curprint(" ");
    }

    //
    // Catch arguments may have union types. Check for that condition here.
    //
    AstSgNodeListAttribute *type_attribute = (AstSgNodeListAttribute *) init_name -> getAttribute("union_type");
    if (type_attribute != NULL) {
        std::vector<SgNode *> &type_list = type_attribute -> getNodeList();
        for (int k = 0; k < type_list.size(); k++) {
            curprint("| ");
            SgType *type = isSgType(type_list[k]);
            unparseType(type, info);
            curprint(" ");
        }
    }
*/
/*
if (! init_name -> attributeExists("type")){
if (init_name -> get_type() == NULL)
cout << "The SgInitialized name " 
     << init_name->get_name()
     << " has no type"
<< endl;
else
cout << "The SgInitialized name " 
     << init_name->get_name()
     << " has type " 
     << (isSgClassType(init_name -> get_type()) ? isSgClassType(init_name -> get_type()) -> get_qualified_name().getString() : init_name -> get_type() -> class_name())
     << " but has no type attribute"
<< endl;
cout.flush();
}
*/

    AstRegExAttribute *attribute = (AstRegExAttribute *) init_name -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else if (init_name -> attributeExists("var_args")) {
        SgArrayType *array_type = isSgArrayType(init_name -> get_type());
        ROSE_ASSERT(array_type);
        SgType *element_type = array_type -> get_base_type();
        unparseType(element_type ,info);
        curprint("...");
    }
    else {
        unparseType(init_name -> get_type() ,info);
    }
    curprint(" ");

    unparseName(init_name -> get_name(), info);

    if (init_name->get_initializer() != NULL) {
        curprint(" = ");
        unparseExpression(init_name->get_initializer(), info);
    }
}

void
Unparse_Java::unparseForInitStmt (SgStatement* stmt, SgUnparse_Info& info) {
    SgForInitStatement* forInitStmt = isSgForInitStatement(stmt);
    ASSERT_not_null(forInitStmt);

    SgStatementPtrList& stmts = forInitStmt->get_init_stmt();
    SgStatementPtrList::iterator stmt_it;
    for (stmt_it = stmts.begin(); stmt_it != stmts.end(); stmt_it++) {
        if (stmt_it != stmts.begin())
            curprint(", ");

        // charles4 08/06/2011: A for statement initializer can be a variable declaration or an expression statement.
        if (isSgVariableDeclaration(*stmt_it)) {
            if (stmt_it == stmts.begin()) // The first declaration in the list?
                unparseVarDeclStmt(*stmt_it, info);
            else {
                SgVariableDeclaration *vardecl_stmt = (SgVariableDeclaration *) *stmt_it;
                foreach (SgInitializedName* init_name, vardecl_stmt->get_variables()) {
                    unparseName(init_name->get_name(), info);

                    if (init_name->get_initializer() != NULL) {
                        curprint(" = ");
                        unparseExpression(init_name->get_initializer(), info);
                    }
               }
            }
        }
        else {
            SgExprStatement* stmt = isSgExprStatement(*stmt_it);
            ROSE_ASSERT(stmt != NULL && "expected an SgExprStatement or an SgVariableDeclaration in SgForInitStmt");
            unparseExprStmt(stmt, info);
        }
    }
}

void
Unparse_Java::unparseForStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgForStatement* for_stmt = isSgForStatement(stmt);
    ASSERT_not_null(for_stmt);

    SgExprStatement* test_stmt = isSgExprStatement(for_stmt->get_test());
    ROSE_ASSERT(test_stmt != NULL && "expected SgForStatement::p_test to be an SgExprStatement");
    SgExpression* test_exp = test_stmt->get_expression();

    curprint("for (");
    unparseStatement(for_stmt->get_for_init_stmt(), info);
    curprint("; ");
    if (! isSgNullExpression(test_exp))
        unparseExpression(test_exp, info);
    curprint("; ");
    if (! isSgNullExpression(for_stmt->get_increment()))
        unparseExpression(for_stmt->get_increment(), info);
    curprint(")");

    if (for_stmt->get_loop_body() != NULL) {
        curprint(" ");
        unparseStatement(for_stmt->get_loop_body(), info);
    } else {
        curprint(";");
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

               ASSERT_not_null(*i);
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
     ASSERT_not_null(funcdefn_stmt);

     unparseStatement(funcdefn_stmt->get_body(), info);
   }

void
Unparse_Java::unparseFunctionParameterList(SgStatement* stmt, SgUnparse_Info& info)
{
    SgFunctionParameterList* param_list = isSgFunctionParameterList(stmt);
    ASSERT_not_null(param_list);

    SgInitializedNamePtrList& names = param_list->get_args();
    SgInitializedNamePtrList::iterator name_it;
    for (name_it = names.begin(); name_it != names.end(); name_it++) {
        if (name_it != names.begin())
            curprint(", ");
        SgInitializedName *iname = *name_it;

        AstRegExAttribute *attribute = (AstRegExAttribute *) iname -> getAttribute("type");
        if (attribute) {
            curprint(attribute -> expression);
        }
        else {
            unparseType(iname -> get_type(), info);
        }
        curprint(" ");
// TODO: Remove this !
//        unparseType(iname->get_type(), info);
//        curprint(" ");
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
     ASSERT_not_null(mfuncdecl_stmt);
     if (mfuncdecl_stmt -> attributeExists("compiler-generated")) { // Do not unparse compiler-generated functions
         return;
     }

    AstSgNodeListAttribute *annotations_attribute = (AstSgNodeListAttribute *) mfuncdecl_stmt -> getAttribute("annotations");
    if (annotations_attribute) {
        for (int i = 0; i < annotations_attribute -> size(); i++) {
            SgJavaAnnotation *annotation = isSgJavaAnnotation(annotations_attribute -> getNode(i));
            unparseExpression(annotation, info);
            unp -> cur.insert_newline();
            curprint_indented("", info);
        }
    }

//
// TODO: REMOVE THIS
// charles4 :  2/29/2012   I don't think this is needed!
/*
     //TODO should there be forward declarations or nondefining declarations?
     if (mfuncdecl_stmt-> isForward()) {
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
*/

     if (mfuncdecl_stmt->get_functionModifier().isJavaInitializer()) { // If this is an initializer block, process it here and return.
         if (mfuncdecl_stmt -> get_declarationModifier().get_storageModifier().isStatic()) {
             curprint("static ");
         }
         unparseBasicBlockStmt(mfuncdecl_stmt -> get_definition() -> get_body(), info);
         return;
     }

     unparseDeclarationModifier(mfuncdecl_stmt->get_declarationModifier(), info);

     unparseFunctionModifier(mfuncdecl_stmt->get_functionModifier(), info);

     if (mfuncdecl_stmt -> attributeExists("type_parameters")) {
         AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) mfuncdecl_stmt -> getAttribute("type_parameters");
         SgTemplateParameterList *type_list = isSgTemplateParameterList(attribute -> getNode());
         ROSE_ASSERT(type_list);
         unparseTypeParameters(type_list, info);
     }

     //
     // Unparse type, unless this a constructor then unparse name
     //
     if (mfuncdecl_stmt -> get_specialFunctionModifier().isConstructor()) {
         
         unparseName(isSgClassDeclaration( mfuncdecl_stmt -> get_associatedClassDeclaration() ) -> get_name(), info);
     }
     else {
// TODO: Remove this !
/*
         ROSE_ASSERT(mfuncdecl_stmt->get_type());
         ROSE_ASSERT(mfuncdecl_stmt->get_type()->get_return_type());
         unparseType(mfuncdecl_stmt->get_type()->get_return_type(), info);
*/
         AstRegExAttribute *attribute = (AstRegExAttribute *) mfuncdecl_stmt -> getAttribute("type");
         if (attribute) {
             curprint(attribute -> expression);
         }
         else {
             unparseType(mfuncdecl_stmt -> get_type() -> get_return_type(), info);
         }
         curprint(" ");
         unparseName(mfuncdecl_stmt->get_name(), info);
     }

     curprint("(");


#if 0 /* There's no need to call unparseStatement() to unparse the parameter list because that's done in the next
       * paragraph. [RPM 2012-05-23] */
     unparseStatement(mfuncdecl_stmt->get_parameterList(), info);
#endif

     SgInitializedNamePtrList& names = mfuncdecl_stmt->get_args();
     SgInitializedNamePtrList::iterator name_it;
     for (name_it = names.begin(); name_it != names.end(); name_it++) {
         if (name_it != names.begin()) {
             curprint(", ");
         }

         AstSgNodeListAttribute *annotations_attribute = (AstSgNodeListAttribute *) (*name_it) -> getAttribute("annotations");
         if (annotations_attribute) {
             for (int i = 0; i < annotations_attribute -> size(); i++) {
                 SgJavaAnnotation *annotation = isSgJavaAnnotation(annotations_attribute -> getNode(i));
                 unparseExpression(annotation, info);
                 curprint(" ");
             }
         }
         unparseInitializedName(*name_it, info);
     }

     AstRegExAttribute *exception_attribute = (AstRegExAttribute *) mfuncdecl_stmt -> getAttribute("exception");
     if (mfuncdecl_stmt -> get_declarationModifier().isJavaAbstract() || mfuncdecl_stmt -> get_functionModifier().isJavaNative()) {
         curprint(")");
         curprint(exception_attribute != NULL ? (" throws " + exception_attribute -> expression).c_str() : "");

         AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) mfuncdecl_stmt -> getAttribute("default");
         if (attribute) {
             curprint(" default ");
             unparseExpression((SgExpression *) attribute -> getNode(), info);
         }

         curprint(";");
     }
     else {
         curprint(") ");
         curprint(exception_attribute != NULL ?  ("throws " + exception_attribute -> expression + " ").c_str() : "");
         SgFunctionDefinition *function_definition = mfuncdecl_stmt->get_definition();
//
// charles4 10/10/2011: For some reason, when either of the 2 entry points below are invoked,
// the body of the function is not processed for the generated constructor... Why?
//
//     unparseStatement(function_definition, info);
//     unparseFuncDefnStmt(function_definition, info);
//
         unparseBasicBlockStmt(function_definition -> get_body(), info);
     }

#if OUTPUT_DEBUGGING_FUNCTION_NAME
     printf ("Inside of unparseMFuncDeclStmt() name = %s  transformed = %s prototype = %s \n",
         mfuncdecl_stmt->get_qualified_name().str(),
         isTransformed (mfuncdecl_stmt) ? "true" : "false",
         (mfuncdecl_stmt->get_definition() == NULL) ? "true" : "false");
#endif
   }

void
Unparse_Java::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgVariableDeclaration* vardecl_stmt = isSgVariableDeclaration(stmt);
    ASSERT_not_null(vardecl_stmt);
    AstSgNodeListAttribute *annotations_attribute = (AstSgNodeListAttribute *) vardecl_stmt -> getAttribute("annotations");
    if (annotations_attribute) {
        for (int i = 0; i < annotations_attribute -> size(); i++) {
            SgJavaAnnotation *annotation = isSgJavaAnnotation(annotations_attribute -> getNode(i));
            unparseExpression(annotation, info);
            unp -> cur.insert_newline();
            curprint_indented("", info);
        }
    }
    unparseDeclarationModifier(vardecl_stmt->get_declarationModifier(), info);
    foreach (SgInitializedName* init_name, vardecl_stmt->get_variables())
        unparseInitializedName(init_name, info);
}


void
Unparse_Java::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
     ASSERT_not_null(vardefn_stmt);
   }

void
Unparse_Java::initializeDeclarationsFromParent ( 
   SgDeclarationStatement* declarationStatement,
   SgClassDefinition* & cdefn,
   SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport )
   {
     ASSERT_not_null(declarationStatement);
   }


void
Unparse_Java::unparseJavaPackageStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgJavaPackageStatement *package_statement = isSgJavaPackageStatement(stmt);
    SgName package_name = package_statement -> get_name();
// TODO: remove this!
/*
    SgJavaPackageDeclaration *package_declaration = package_statement -> get_package_declaration();
    ROSE_ASSERT(package_declaration);
    SgClassDefinition *package_definition = package_declaration -> get_definition();
    ROSE_ASSERT(package_definition -> attributeExists("translated_package"));
    AstRegExAttribute *attribute = (AstRegExAttribute *) package_definition -> getAttribute("translated_package");
    ROSE_ASSERT(attribute);
    if (attribute -> expression.size() > 0) { // not the null package name?
        curprint("package ");
        curprint(attribute -> expression);
    }
*/
    AstSgNodeListAttribute *annotations_attribute = (AstSgNodeListAttribute *) package_statement -> getAttribute("annotations");
    if (annotations_attribute) {
        for (int i = 0; i < annotations_attribute -> size(); i++) {
            SgJavaAnnotation *annotation = isSgJavaAnnotation(annotations_attribute -> getNode(i));
            unparseExpression(annotation, info);
            unp -> cur.insert_newline();
            curprint_indented("", info);
        }
    }

    curprint("package ");
    curprint(package_name.getString());

    return;
}


void
Unparse_Java::unparseClassDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDeclaration *classdecl_stmt = isSgClassDeclaration(stmt);
     ASSERT_not_null(classdecl_stmt);
     ROSE_ASSERT(! classdecl_stmt -> get_explicit_anonymous());
// TODO: Remove this!
/*
cout << "Processing class declaration " << classdecl_stmt -> get_qualified_name().str()
<< endl;
cout.flush();
*/

     AstSgNodeListAttribute *annotations_attribute = (AstSgNodeListAttribute *) classdecl_stmt -> getAttribute("annotations");
     if (annotations_attribute) {
         for (int i = 0; i < annotations_attribute -> size(); i++) {
             SgJavaAnnotation *annotation = isSgJavaAnnotation(annotations_attribute -> getNode(i));
             unparseExpression(annotation, info);
             unp -> cur.insert_newline();
             curprint_indented("", info);
         }
     }

     unparseDeclarationModifier(classdecl_stmt -> get_declarationModifier(), info);

     curprint(classdecl_stmt -> get_explicit_enum()
                       ? "enum "
                       : classdecl_stmt -> get_explicit_annotation_interface()
                                  ? "@interface "
                                  : classdecl_stmt -> get_explicit_interface()
                                             ? "interface "
                                             : "class ");

     unparseName(classdecl_stmt -> get_name(), info);

     if (classdecl_stmt -> attributeExists("type_parameters")) {
         AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) classdecl_stmt -> getAttribute("type_parameters");
         SgTemplateParameterList *type_list = isSgTemplateParameterList(attribute -> getNode());
         ROSE_ASSERT(type_list);
         unparseTypeParameters(type_list, info);
     }

     SgClassDefinition *class_def = classdecl_stmt -> get_definition();
     ASSERT_not_null(class_def);

     // SgBaseClassPtrList& bases = class_def -> get_inheritances();

     //
     // TODO: This can't work for type parameters as an SgJavaParameterizedType is not mapped one-to-one and onto
     //  with its associated SgClassDeclaration.  See alternate attributed ("parameter_type_bounds") code below.
     //
     /*
     int first_index = 0;
     if (bases.size() > 0) {
         SgBaseClass *super_class = isSgBaseClass(bases[0]);
         ROSE_ASSERT (super_class);
         if (! super_class -> get_base_class() -> get_explicit_interface()) {
             first_index++;
             curprint(" extends ");
             unparseBaseClass(super_class, info);
         }
     }

     if (bases.size() - first_index > 0) {
         curprint(classdecl_stmt -> get_explicit_interface() ? " extends " : " implements ");
         for (int i = first_index; i < bases.size(); i++) {
             SgBaseClass *iface = isSgBaseClass(bases[i]);
             ROSE_ASSERT(iface -> get_base_class() -> get_explicit_interface());
             unparseBaseClass(iface, info);
             if (i + 1 < bases.size()) {
                 curprint(", ");
             }
         }
     }
     */

     if (! classdecl_stmt -> get_explicit_annotation_interface()) {
// TODO: Remove this !
/*
         AstSgNodeListAttribute *attribute = ( AstSgNodeListAttribute *) class_def -> getAttribute("extensions");
         std::vector<SgNode *> &parm_list = attribute -> getNodeList();
         ROSE_ASSERT(parm_list.size() == bases.size());
         SgBaseClass *super_class = (parm_list.size() > 0 ? bases[0] : NULL);
         for (int k = 0; k < parm_list.size(); k++) {
             SgType *type = isSgType(parm_list[k]);
             if (k == 0) {
                 if (classdecl_stmt -> get_explicit_interface()) {
                     curprint(" extends "); // We are processing an interface.
                     unparseParameterType(type, info);
                     if (k + 1 < parm_list.size())
                         curprint(", ");
                 }
                 else if (super_class -> get_base_class() -> get_explicit_interface()) {
                     curprint(" implements ");
                     unparseParameterType(type, info);
                     if (k + 1 < parm_list.size())
                         curprint(", ");
                 }
                 else {
                     if (! classdecl_stmt -> get_explicit_enum()) { // Don't process super class for enumeration.
                         curprint(" extends ");
                         unparseParameterType(type, info);
                     }
                     if (k + 1 < parm_list.size())
                         curprint(" implements ");
                 }
             }
             else {
                 unparseParameterType(type, info);
                 if (k + 1 < parm_list.size())
                     curprint(", ");
             }
         }
*/

         ROSE_ASSERT(class_def -> attributeExists("extension_type_names"));
         AstRegExAttribute *extension_attribute = (AstRegExAttribute *) class_def -> getAttribute("extension_type_names");
         curprint(extension_attribute -> expression);
     }

     if (classdecl_stmt -> get_explicit_enum()) { // An enumeration?
         unparseEnumBody(class_def, info);
     }
     else {
         unparseStatement(class_def, info);
     }
   }

void
Unparse_Java::unparseClassDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ASSERT_not_null(classdefn_stmt);
// TODO: Remove this!
/*
cout << "*** Class " << classdefn_stmt -> get_declaration() -> get_qualified_name().str() << " contains " << classdefn_stmt -> get_members().size() << " statements" << endl;
for (int i = 0; i < classdefn_stmt -> get_members().size(); i++) {
  SgNode *statement = classdefn_stmt -> get_members()[i];
  cout << "    " << statement -> class_name() << endl;
}
cout.flush();
*/
     curprint(" {");
     unp -> cur.insert_newline();
     foreach (SgDeclarationStatement* child, classdefn_stmt->get_members()) {
         unparseNestedStatement(child, info);
     }
// Alternative code to the one above
/*
     for (int i = 0; i < classdefn_stmt -> get_members().size(); i++) {
        SgDeclarationStatement *declaration = isSgDeclarationStatement(classdefn_stmt -> get_members()[i]);
        ROSE_ASSERT(declaration);
        unparseNestedStatement(declaration, info);
     }
*/
     curprint_indented("}", info);
   }


void
Unparse_Java::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_stmt = isSgEnumDeclaration(stmt);
     ASSERT_not_null(enum_stmt);

   }

void
Unparse_Java::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ASSERT_not_null(expr_stmt);

     unparseExpression(expr_stmt->get_expression(), info);
   }

void Unparse_Java::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgJavaLabelStatement* label_stmt = isSgJavaLabelStatement(stmt);
     ASSERT_not_null(label_stmt);

     curprint ( string(label_stmt->get_label().str()) + ":");
     unparseStatement(label_stmt->get_statement(), info); // charles4: 8/25/2001 process the real stmt
   }

void
Unparse_Java::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
  ASSERT_not_null(while_stmt);

  SgExprStatement* cond_stmt = isSgExprStatement(while_stmt->get_condition());
  ROSE_ASSERT(cond_stmt != NULL && "Expecting an SgExprStatement in member SgWhileStmt::p_condition.");
  SgExpression* cond = cond_stmt->get_expression();
  ASSERT_not_null(cond);

  curprint("while (");
  unparseExpression(cond, info);
  curprint(")");

  if(while_stmt->get_body()) {
      curprint(" ");
      unparseStatement(while_stmt->get_body(), info);
  } else {
      curprint(";");
  }
}

void
Unparse_Java::unparseDoWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgDoWhileStmt* dowhile_stmt = isSgDoWhileStmt(stmt);
  ASSERT_not_null(dowhile_stmt);

  SgExprStatement* cond_stmt = isSgExprStatement(dowhile_stmt->get_condition());
  ROSE_ASSERT(cond_stmt != NULL && "Expecting an SgExprStatement in member SgDoWhileStmt::p_condition.");
  SgExpression* cond = cond_stmt->get_expression();
  ASSERT_not_null(cond);

  curprint("do ");
  unparseStatement(dowhile_stmt->get_body(), info);
  curprint_indented("while (", info);
  unparseExpression(cond, info);
  curprint(")");
}

void
Unparse_Java::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
  
     ASSERT_not_null(switch_stmt);

     curprint ( string("switch("));
     
     unparseExpression(((SgExprStatement *) switch_stmt->get_item_selector())-> get_expression(), info);

     curprint ( string(")"));

     if ( (switch_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(switch_stmt->get_body(), info);
   }

void
Unparse_Java::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
     ASSERT_not_null(case_stmt);

     curprint ( string("case "));
     unparseExpression(case_stmt->get_key(), info);
     curprint ( string(":"));

     if ((case_stmt->get_body() != NULL) && !info.SkipBasicBlock()) {
         unparseCaseOrDefaultBasicBlockStmt(case_stmt->get_body(), info);
     }
   }

void
Unparse_Java::unparseTryStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTryStmt* try_stmt = isSgTryStmt(stmt);
     ASSERT_not_null(try_stmt);

     curprint ("try ");

     AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) try_stmt -> getAttribute("resources");
     if (attribute) {
         curprint ("(");
         for (int i = 0; i < attribute -> size(); i++) {
            SgVariableDeclaration *local_declaration = isSgVariableDeclaration(attribute -> getNode(i));
            AstSgNodeListAttribute *annotations_attribute = (AstSgNodeListAttribute *) local_declaration -> getAttribute("annotations");
            if (annotations_attribute) {
                for (int i = 0; i < annotations_attribute -> size(); i++) {
                    SgJavaAnnotation *annotation = isSgJavaAnnotation(annotations_attribute -> getNode(i));
                    unparseExpression(annotation, info);
                    curprint(" ");
                }
            }
             ROSE_ASSERT(local_declaration);
             if (i > 0) {
                 curprint ("; ");
             }
             vector<SgInitializedName *> &locals = local_declaration -> get_variables();
             ROSE_ASSERT(locals.size() == 1);
             unparseInitializedName(locals[0], info);
         }
         curprint (") ");
     }
  
     unp->cur.format(try_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(try_stmt->get_body(), info);
     unp->cur.format(try_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  
     SgStatementPtrList::iterator i=try_stmt->get_catch_statement_seq().begin();
     while (i != try_stmt->get_catch_statement_seq().end())
        {
          unparseStatement(*i, info);
          i++;
        }

     if (try_stmt -> get_finally_body()) { // charles4 09/23/2011
         curprint_indented("", info);
         curprint ("finally ");
         unparseStatement(try_stmt -> get_finally_body(), info);
     }
   }

void
Unparse_Java::unparseCatchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCatchOptionStmt* catch_statement = isSgCatchOptionStmt(stmt);
     ASSERT_not_null(catch_statement);

     curprint ( string("catch " ) + "(");
     if (catch_statement->get_condition())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inVarDecl();

          ninfo.set_SkipSemiColon();

          unparseVarDeclStmt(catch_statement->get_condition(), ninfo); // charles4 09/23/2011 - call VarDecl directly to prevent line break.
                                                                       // Old statement:  unparseStatement(catch_statement->get_condition(), ninfo);
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
     ASSERT_not_null(default_stmt);

     curprint ( string("default:"));

     if ((default_stmt->get_body() != NULL) && !info.SkipBasicBlock()) {
         unparseCaseOrDefaultBasicBlockStmt(default_stmt->get_body(), info);
     }
   }

void
Unparse_Java::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
  ASSERT_not_null(break_stmt);
  curprint ("break");
  if (break_stmt->get_do_string_label() != "") {
      curprint(" ");
      curprint(break_stmt->get_do_string_label());
  }

}

void
Unparse_Java::unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgContinueStmt* continue_stmt = isSgContinueStmt(stmt);
  ASSERT_not_null(continue_stmt);

  curprint ("continue");
  if (continue_stmt->get_do_string_label() != "") {
      curprint(" ");
      curprint(continue_stmt->get_do_string_label());
  }

}

void
Unparse_Java::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
     ASSERT_not_null(return_stmt);

     curprint ("return");

     if (return_stmt->get_expression()) {
         curprint(" ");
         unparseExpression(return_stmt->get_expression(), info);
     }
   }

void
Unparse_Java::unparseAssertStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgAssertStmt* assert_stmt = isSgAssertStmt(stmt);
     ASSERT_not_null(assert_stmt);

     curprint ("assert ");

     unparseExpression(assert_stmt->get_test(), info);

     if (assert_stmt->get_exception_argument()) {
         curprint(" : ");
         unparseExpression(assert_stmt->get_exception_argument(), info);
     }
   }

void
Unparse_Java::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
  ASSERT_not_null(goto_stmt);

  curprint ( string("goto " ) + goto_stmt->get_label()->get_label().str());
  if (!info.SkipSemiColon()) { curprint ( string(";")); }
}


void
Unparse_Java::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTypedefDeclaration* typedef_stmt = isSgTypedefDeclaration(stmt);
     ASSERT_not_null(typedef_stmt);

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

void
Unparse_Java::unparseBaseClass(SgBaseClass* base, SgUnparse_Info& info) {
    ASSERT_not_null(base);

    //
    // TODO: What about a SgJavaParameterizedType? See comments in unparseClassDeclStmt(...)
    //
    SgClassDeclaration* base_class = base -> get_base_class();
    SgClassType *class_type = base_class -> get_type();
    ROSE_ASSERT(class_type);
    unparseClassType(class_type, info);
}


void
Unparse_Java::unparseParameterType(SgType *bound_type, SgUnparse_Info& info) {
    ASSERT_not_null(bound_type);

    SgClassType *class_type = isSgClassType(bound_type);
    SgJavaParameterizedType *parameterized_type = isSgJavaParameterizedType(bound_type);
    ROSE_ASSERT(class_type || parameterized_type);

    if (class_type)
         unparseClassType(class_type, info);
    else unparseJavaParameterizedType(parameterized_type, info);
}


void
Unparse_Java::unparseEnumBody(SgClassDefinition *class_definition, SgUnparse_Info& info) {
    curprint(" {");
    unp->cur.insert_newline();

    std::vector<SgDeclarationStatement *> members = class_definition -> get_members();

    //
    // If an Enum type contains enum constants, they must appear first in the body.
    //
    int last_enum_constant_index = members.size(); // assume all the members are enum-constants or a compiler-generated member
    for (size_t i = 0; i < members.size(); i++) {
        SgDeclarationStatement *member = members[i];

        //
        // Skip all enum constants and compiler-generated members while keeping track of the last enum-constant encountered.
        //
        if (member -> attributeExists("enum-constant") || member -> attributeExists("compiler-generated")) {
            continue;
        }
        else {
            last_enum_constant_index = i;
            break;
        }
    }

    //
    // If an Enum contains enum constants, process them first.
    //
    for (int i = 0; i < last_enum_constant_index; i++) {
        SgVariableDeclaration *enum_constant = isSgVariableDeclaration(members[i]);
        if (enum_constant) { // An enum constant?
            ROSE_ASSERT(enum_constant -> attributeExists("enum-constant"));

            AstSgNodeListAttribute *annotations_attribute = (AstSgNodeListAttribute *) enum_constant -> getAttribute("annotations");
            if (annotations_attribute) {
                for (int i = 0; i < annotations_attribute -> size(); i++) {
                    SgJavaAnnotation *annotation = isSgJavaAnnotation(annotations_attribute -> getNode(i));
                    unparseExpression(annotation, info);
                    unp -> cur.insert_newline();
                    curprint_indented("", info);
                }
            }

            vector<SgInitializedName *> &vars = enum_constant -> get_variables();
            ROSE_ASSERT(vars.size() == 1);
            info.inc_nestingLevel();
            curprint_indented(vars[0] -> get_name().getString(), info);

            //
            // If this Enum constant has an initializer, process it.
            //
            if (vars[0] -> get_initializer() != NULL) {
                SgAssignInitializer *initializer = isSgAssignInitializer(vars[0] -> get_initializer());
                ROSE_ASSERT(initializer);
                SgNewExp *new_expression = isSgNewExp(initializer -> get_operand());
                ROSE_ASSERT(new_expression);
                ROSE_ASSERT(new_expression -> get_constructor_args());
                SgConstructorInitializer *init = new_expression -> get_constructor_args();
                ROSE_ASSERT(init);

                //
                // If this Enum constant initializer accepts arguments (parameters), process them.
                //
                vector<SgExpression *> args = init -> get_args() -> get_expressions();
                if (args.size() > 0) {
                    curprint(" (");
                    for (size_t i = 0; i < args.size(); i++) {
                        unparseExpression(args[i], info);
                        if (i + 1 < args.size())
                            curprint(", ");
                    }
                    curprint(")");
                }

                //
                // If this Enum constant initializer contains a body, output the body.
                //
                if (new_expression -> attributeExists("body")) {
                    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) new_expression -> getAttribute("body");
                    SgClassDeclaration *class_declaration = isSgClassDeclaration(attribute -> getNode());
                    ROSE_ASSERT(class_declaration);
                    unparseClassDefnStmt(class_declaration -> get_definition(), info);
                }
            }

            if (i + 1 != last_enum_constant_index) {
                curprint(",");
            }
            unp -> cur.insert_newline();
            info.dec_nestingLevel();
        }
    }

    unp->cur.insert_newline();
    info.dec_nestingLevel();
    curprint(";");
    unp->cur.insert_newline();

    //
    // Now, process the remaining members of the Enum body following the Enum constants.
    //
    info.inc_nestingLevel();
    for (size_t i = last_enum_constant_index; i < members.size(); i++) {
        SgDeclarationStatement *member = members[i];
        unparseStatement(member, info);
    }
    info.dec_nestingLevel();

    curprint_indented("}", info);
}


void 
Unparse_Java::unparseTypeParameters(SgTemplateParameterList *type_list, SgUnparse_Info& info) {
    curprint("<");
    for (size_t i = 0; i < type_list -> get_args().size(); i++) {
        SgClassType *parameter_type = isSgClassType(type_list -> get_args()[i] -> get_type());
        ROSE_ASSERT(parameter_type);

// TODO: Remove this !
/*
        curprint(parameter_type -> get_name().getString());
        SgClassDeclaration *parameter_class_declaration = isSgClassDeclaration(parameter_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(parameter_class_declaration);
        SgClassDefinition *parameter_class_definition = parameter_class_declaration -> get_definition();
        ROSE_ASSERT(parameter_class_definition);

        SgBaseClassPtrList &bases = parameter_class_definition -> get_inheritances();
*/

             //
             // TODO: This can't work for type parameters as an SgJavaParameterizedType is not mapped one-to-one and onto
             //  with its associated SgClassDeclaration.  See alternate attributed ("parameter_type_bounds") code below.
             //
             /*
             int first_index = 0;
             if (bases.size() > 0) {
                 SgBaseClass *super_class = isSgBaseClass(bases[0]);
                 ROSE_ASSERT (super_class);
                 if (! super_class -> get_base_class() -> get_explicit_interface()) {
                     first_index++;
                     curprint(" extends ");
                     unparseParameterType(super_class, info);
                 }
             }
             if (bases.size() - first_index > 0) {
                 curprint(" & ");
                 for (int i = first_index; i < bases.size(); i++) {
                     SgBaseClass *interface = isSgBaseClass(bases[i]);
                     ROSE_ASSERT(interface -> get_base_class() -> get_explicit_interface());
                     unparseParameterType(interface, info);
                     if (i + 1 < bases.size()) {
                         curprint(", ");
                     }
                 }
             }
             */

// TODO: Remove this !
/*
        bool has_extends = parameter_class_definition -> attributeExists("parameter_type_bounds_with_extends");
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *)
                                            (has_extends ? parameter_class_definition -> getAttribute("parameter_type_bounds_with_extends")
                                                         : parameter_class_definition -> getAttribute("parameter_type_bounds"));
        std::vector<SgNode *> &parm_list = attribute -> getNodeList();
        ROSE_ASSERT(parm_list.size() == bases.size());
        SgBaseClass *super_class = (parm_list.size() > 0 ? bases[0] : NULL);
        for (int k = 0; k < parm_list.size(); k++) {
            SgType *type = isSgType(parm_list[k]);
            if (k == 0 && has_extends) {
                curprint(" extends ");
            }
            else {
                curprint(" & ");
            }
            unparseParameterType(type, info);
        }
*/

        AstRegExAttribute *attribute = (AstRegExAttribute *) parameter_type -> getAttribute("type");
        if (attribute) {
            curprint(attribute -> expression);
        }
        else {
            unparseType(parameter_type, info);
        }

        if (i + 1 < type_list->get_args().size()) {
            curprint(", ");
        }
    }
    curprint(">");
}

