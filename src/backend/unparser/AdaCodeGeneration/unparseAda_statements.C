/* unparseX10_statements.C
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
 * Unparse_X10::unparse is called, and for statements, 
 * Unparse_X10::unparseStatement is called.
 *
 */
#include "sage3basic.h"
#include "unparser.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

using namespace std;

#include "sage_support.h"

// #ifdef ROSE_BUILD_X10_LANGUAGE_SUPPORT
// using namespace Rose::Frontend::X10::X10c;
// #endif

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS  0
#define OUTPUT_DEBUGGING_UNPARSE_INFO        0

// Output the class name and function names as we unparse (for debugging)
#define OUTPUT_DEBUGGING_CLASS_NAME    0
#define OUTPUT_DEBUGGING_FUNCTION_NAME 0
#define OUTPUT_HIDDEN_LIST_DATA 0


Unparse_Ada::Unparse_Ada(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
  // Nothing to do here!
   }


Unparse_Ada::~Unparse_Ada()
   {
  // Nothing to do here!
   }


void 
Unparse_Ada::unparseAdaFile(SgSourceFile *sourcefile, SgUnparse_Info& info) 
   {

     SgGlobal* globalScope = sourcefile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

     unparseStatement(globalScope, info);

  // Output a simple predefined file to test the compilation.
     string simpleTestProgram = "procedure Unit_1 is \n   begin \n      null; \n   end; \n";

     curprint(simpleTestProgram);
   }


void
Unparse_Ada::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function

     ROSE_ASSERT(stmt != NULL);

  // curprint_indented("", info);

     switch (stmt->variantT())
        {
       // case V_SgGlobal:                   cout << "Got it !!!" << endl; /* unparseGlobalStmt   (stmt, info); */ break;

#if 0
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

          case V_SgJavaPackageStatement:   unparseX10PackageStmt(stmt, info);  break;
          case V_SgClassDeclaration:       unparseClassDeclStmt(stmt, info);    break;
          case V_SgClassDefinition:        unparseClassDefnStmt(stmt, info);    break;
          case V_SgEnumDeclaration:        unparseEnumDeclStmt(stmt, info);     break;
          case V_SgExprStatement:          unparseExprStmt(stmt, info);         break;

          case V_SgDoWhileStmt:            unparseDoWhileStmt(stmt, info);      break;

          case V_SgTryStmt:                unparseTryStmt(stmt, info);          break;
          case V_SgCatchOptionStmt:        unparseCatchStmt(stmt, info);        break;

          case V_SgContinueStmt:           unparseContinueStmt(stmt, info);     break;

       // case V_SgAsmStmt:                unparseAsmStmt(stmt, info);          break;
#if 0
          case V_SgFinishStmt:           unparseFinishStmt((SgFinishStmt *)stmt, info);           break;
          case V_SgAtStmt:               unparseAtStmt((SgAtStmt *)stmt, info);           break;
          case V_SgAsyncStmt:            unparseAsyncStmt((SgAsyncStmt *)stmt, info);        break;
#endif

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

       // DQ (4/16/2011): Added X10 specific IR node for "import" statements.
          case V_SgJavaImportStatement:                unparseImportDeclarationStatement(stmt, info); break;
#endif

          case V_SgNamespaceDeclarationStatement:
          case V_SgFunctionDeclaration:
             {
               // currently ignored
               std::cerr << "skip " << stmt->class_name() << " in ADA unparser" << std::endl; 
               break;
             }
          default:
             {
               printf("Unparse_Ada::unparseLanguageSpecificStatement: Error: No handler for %s (variant: %d)\n",stmt->class_name().c_str(), stmt->variantT());
               ROSE_ASSERT(false);
               break;
             }
        }
   }

