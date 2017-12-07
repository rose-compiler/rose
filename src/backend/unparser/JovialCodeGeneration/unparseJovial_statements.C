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

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

using namespace std;

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
     ROSE_ASSERT(globalScope != NULL);

     curprint("START\n");
     unparseStatement(globalScope, info);
     curprint("TERM\n");
   }


void
Unparse_Jovial::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function

     ROSE_ASSERT(stmt != NULL);

  // curprint_indented("", info);

     switch (stmt->variantT())
        {
       // case V_SgGlobal:                   cout << "Got it !!!" << endl; /* unparseGlobalStmt   (stmt, info); */ break;


       // program units
       // case V_SgModuleStatement:            unparseModuleStmt(stmt, info);       break;
          case V_SgProgramHeaderStatement:     unparseProgHdrStmt(stmt, info);      break;
       // case V_SgProcedureHeaderStatement:   unparseProcHdrStmt(stmt, info);      break;

       // declarations
       // case V_SgFunctionDeclaration:        unparseFuncDeclStmt(stmt, info);     break;
          case V_SgFunctionDefinition:         unparseFuncDefnStmt(stmt, info);     break;

          case V_SgVariableDeclaration:        unparseVarDeclStmt  (stmt, info);    break;

       // executable statements, control flow
          case V_SgBasicBlock:                 unparseBasicBlockStmt (stmt, info);  break;

          case V_SgExprStatement:              unparseExprStmt(stmt, info);         break;

#if 0
       // declarations
          case V_SgVariableDefinition:     unparseVarDefnStmt  (stmt, info); break;

       // executable statements, control flow
          case V_SgIfStmt:                 unparseIfStmt         (stmt, info); break;

          case V_SgWhileStmt:              unparseWhileStmt      (stmt, info); break;
          case V_SgSwitchStatement:        unparseSwitchStmt     (stmt, info); break;
          case V_SgCaseOptionStmt:         unparseCaseStmt       (stmt, info); break;
          case V_SgDefaultOptionStmt:      unparseDefaultStmt    (stmt, info); break;
          case V_SgBreakStmt:              unparseBreakStmt      (stmt, info); break;
          case V_SgGotoStatement:          unparseGotoStmt       (stmt, info); break;
          case V_SgReturnStmt:             unparseReturnStmt     (stmt, info); break;
          case V_SgAssertStmt:             unparseAssertStmt     (stmt, info); break;
          case V_SgNullStatement:          curprint("");/* Tab over for stmt*/ break;

          case V_SgForStatement:           unparseForStmt(stmt, info);          break; 

          case V_SgEnumDeclaration:        unparseEnumDeclStmt(stmt, info);     break;

          case V_SgDoWhileStmt:            unparseDoWhileStmt(stmt, info);      break;

          case V_SgContinueStmt:           unparseContinueStmt(stmt, info);     break;

          case V_SgTypedefDeclaration:     unparseTypeDefStmt(stmt, info);      break;

          case V_SgForInitStatement:                   unparseForInitStmt(stmt, info); break;

          case V_SgFunctionParameterList:  unparseFunctionParameterList(stmt, info); break;

          case V_SgUsingDirectiveStatement:            unparseUsingDirectiveStatement (stmt, info);            break;
          case V_SgUsingDeclarationStatement:          unparseUsingDeclarationStatement (stmt, info);          break;
#endif

          default:
             {
               printf("Unparse_Jovial::unparseLanguageSpecificStatement: Error: No handler for %s (variant: %d)\n",stmt->class_name().c_str(), stmt->variantT());
               ROSE_ASSERT(false);
               break;
             }
        }
   }


//----------------------------------------------------------------------------
//  Unparse_Jovial::MODULES
//----------------------------------------------------------------------------

void 
Unparse_Jovial::unparseProgHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);

     SgProgramHeaderStatement* prog = isSgProgramHeaderStatement(stmt);
     ROSE_ASSERT(prog);

     curprint("PROGRAM ");
     curprint(prog->get_name().str());
     curprint(" ;\n");

     unparseStatement(prog->get_definition(), ninfo);

 //  unparseStatementNumbersSupport(mod->get_end_numeric_label(),info);

  // TODO - unparse non-nested-subroutines
   }

void
Unparse_Jovial::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionDefinition* funcdef = isSgFunctionDefinition(stmt);
     ROSE_ASSERT(funcdef != NULL);

     curprint("BEGIN\n");

  // unparse the body of the function
     if (funcdef->get_body())
        {
          unparseStatement(funcdef->get_body(), info);
        }

     curprint("END\n");
   }


//----------------------------------------------------------------------------
//  Unparse_Jovial::<executable statements, control flow>
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ROSE_ASSERT(basic_stmt != NULL);

#if 0
  // DQ (10/6/2008): Adding space here is required to get "else if" blocks formatted correctly (at least).
     unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
#endif

     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     for ( ; p != basic_stmt->get_statements().end(); ++p)
     {
          unparseStatement((*p), info);
     }

#if 0
  // DQ (10/6/2008): This does not appear to be required (passes all tests).
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
#endif
   }

void
Unparse_Jovial::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(vardecl != NULL);
  
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
     ROSE_ASSERT(type);

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(variableDeclaration != NULL);

#if 0
     if (variableDeclaration->get_declarationModifier().get_typeModifier().isStatic() == true)
        {
           curprint("STATIC ");
        }
#endif

     curprint("ITEM ");
     curprint(name.str());
     curprint(" ");

     unparseType(type, info);

      if (init != NULL)
      {
         curprint(" = ");
         SgInitializer* initializer = isSgInitializer(init);
         ROSE_ASSERT(initializer != NULL);
      // TODO
      // unparseExpression(initializer, info);
      }

     curprint(" ;\n");
   }

void
Unparse_Jovial::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ROSE_ASSERT(expr_stmt != NULL);
     ROSE_ASSERT(expr_stmt->get_expression());

     unparseExpression(expr_stmt->get_expression(), info);

     unp->u_sage->curprint_newline();
   }

