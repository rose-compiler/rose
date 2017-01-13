// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

class MyTraversal
   : public SgSimpleProcessing
   {
     public:
          void visit(SgNode* astNode);
   };

#if 0
Notes (put into documentation):
There are several types of statements which the AST rewrite mechanism can not 
currently process.  Below we enumerate these and explain why each is difficult 
or not currently possible:
     1) Why we have to skip SgCaseOptionStmt statements.
        Example of code in generated intermedate file for a SgCaseOptionStmt:
             int GlobalScopePreambleStart;
             int GlobalScopePreambleEnd;
             int CurrentLocationTopOfScopeStart;
             int CurrentLocationTopOfScopeEnd;
             int CurrentLocationBeforeStart;
             int CurrentLocationBeforeEnd;
             int CurrentLocationReplaceStart;
             case 0:{y++;break;}
             int CurrentLocationReplaceEnd;
             int CurrentLocationAfterStart;
             int CurrentLocationAfterEnd;
             int CurrentLocationBottomOfScopeStart;
             int CurrentLocationBottomOfScopeEnd;
        The problem is that marker declaration that appear after the SgCaseOptionStmt 
        are included in the scope of the SgCaseOptionStmt while those that appear 
        before it are not in the same scope.

     2) SgDefaultOptionStmt (see reason #1 above).
     3) SgCtorInitializerList
        This case would have to generate special handling to be generated in the 
        intermediate file and likely it would require special handling isolated 
        from the AST.  Likely this case can be handled in the future with extra work.
     4) SgFunctionParameterList (see reason #3 above).
     5) SgClassDefinition
        Since the SgClassDefinition is so structurally tied to the SgClassDeclaration 
        it make more sense to process the SgClassDeclaration associated with the 
        SgClassDefinition instead of the SgClassDefinition directly.  Presently the 
        processing of the SgClassDefinition is not supported through any indirect 
        processing of the SgClassDeclaration, this could be implemented in the future.
     6) SgGlobal
        This case is not implemented, it would require special handling, it might be 
        implemented in the future.
     7) SgBasicBlock used in a SgForStatement
        Because of the declaration of the for loop index variable, this case would 
        require special handling. This case could be implemented in the future.
     8) SgBasicBlock used in a SgFunctionDefinition
        Because of the declaration of the function parameter variable, this case would 
        require special handling. This case could be implemented in the future.
     9) SgBasicBlock used in a SgSwitchStatement
        Example of code in generated intermedate file for a SgBasicBlock used in 
        SgSwitchStatement:
             int main()
             { /* local stack #0 */
             int x;
             int y;
             switch(x)
             { /* local stack #1 */ 
             int GlobalScopePreambleStart;
             int GlobalScopePreambleEnd;
             int CurrentLocationTopOfScopeStart;
             int CurrentLocationTopOfScopeEnd;
             int CurrentLocationBeforeStart;
             int CurrentLocationBeforeEnd;
             int CurrentLocationReplaceStart;
             {case 0:{y++;break;}default:{y++;break;}}
             int CurrentLocationReplaceEnd;
             int CurrentLocationAfterStart;
             int CurrentLocationAfterEnd;
             int CurrentLocationBottomOfScopeStart;
             int CurrentLocationBottomOfScopeEnd;
             /* Reference marker variables to avoid compiler warnings */
                };     };
        This is more difficult because the declaration markers must appear after 
        the "{ /* local stack #1 */" but then the statement "{case 0:{y++;break;}default:{y++;break;}}"
        cannot appear after a switch.  Likely it is not possible to fix this case due to the
        design and constraints of the C++ language (design and limitations of the switch statement).
        This is not a serious problem, it just means that the whole switch statement must
        be operated upon instead of the block within the switch statement separately.
#endif


void 
MyTraversal::visit ( SgNode* astNode )
   {
  // Nodes to currently avoid (restrictions on the replace mechanism)
#if 1
     bool skipReplace = (isSgGlobal(astNode)                           != NULL) ||
                        (isSgForStatement(astNode->get_parent())       != NULL) ||
                        (isSgSwitchStatement(astNode->get_parent())    != NULL) ||
                      // DQ (7/19/2005): Uncommented. I think it should never have been run, 
                      // since it is clearly detailed above as a case that is not supported 
                      // yet (not implemented).
                        (isSgFunctionDefinition(astNode->get_parent()) != NULL) ||
                        (isSgClassDefinition(astNode)                  != NULL) ||
                        (isSgFunctionParameterList(astNode)            != NULL) ||
                        (isSgCtorInitializerList(astNode)              != NULL) ||
                        (isSgCaseOptionStmt(astNode)                   != NULL) ||
                        (isSgDefaultOptionStmt(astNode)                != NULL);
#else
     bool skipReplace = false;
#endif

//                      (isSgForInitStatement(astNode->get_parent())   != NULL) ||
//                      (isSgBasicBlock(astNode)                       != NULL) ||
//                      (isSgFunctionDefinition(astNode)               != NULL) ||
//                      (isSgFunctionDeclaration(astNode)              != NULL);

     SgStatement* statement = isSgStatement(astNode);
     if ( (statement != NULL) && (!skipReplace) )
        {
       // At each statement replace it with itself
          string statementSourceCode = statement->unparseToString();
          printf ("\n\n\n\n################################################### \n");
          printf ("##### statement = %s scope = %s statementSourceCode = %s \n",
               statement->sage_class_name(),statement->get_scope()->sage_class_name(),statementSourceCode.c_str());
          printf ("################################################### \n");

       // MiddleLevelRewrite::ScopeIdentifierEnum scope = MidLevelCollectionTypedefs::StatementScope;
          MiddleLevelRewrite::ScopeIdentifierEnum scope = MidLevelCollectionTypedefs::SurroundingScope;

          MiddleLevelRewrite::insert(statement,statementSourceCode,scope,MidLevelCollectionTypedefs::ReplaceCurrentPosition);

#if 1
       // Output the current scope to see transformation
          SgScopeStatement* parentScope = statement->get_scope();
          ROSE_ASSERT(parentScope != NULL);
          string parentScopeString = parentScope->unparseToString();
          printf ("################################################### \n");
          printf ("statement = %s parentScope = %s parentScopeString = \n%s\n",
               statement->sage_class_name(),parentScope->sage_class_name(),parentScopeString.c_str());
          printf ("################################################### \n");
#endif
        }
       else
        {
          printf ("##### Skipping node (skipReplace = %s) astNode = (is a statement = %s) = %s \n",
               (skipReplace == true) ? "true " : "false",(isSgStatement(astNode) != NULL) ? "true " : "false",astNode->sage_class_name());
        }
   }

int
main( int argc, char * argv[] )
   {
     SgProject* project = frontend (argc,argv);

#if 1
  // DQ (2/6/2004): These tests fail in Coco for test2004_14.C
     printf ("Running agressive (slow) internal consistancy tests! \n");
     AstTests::runAllTests(const_cast<SgProject*>(project));
#else
     printf ("Skipped agressive (slow) internal consistancy tests! \n");
#endif

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
  // generatePDF(*project);

  // Output the source code file (as represented by the SAGE AST) as a DOT file (with bookmarks)
  // generateDOT(*project);

     MyTraversal myTraversal;
     myTraversal.traverseInputFiles (project,postorder);

#if 1
  // DQ (3/25/2006): These tests currently fail for the tests/nonsmoke/functional/roseTests/astRewriteTests/inputProgram2.C
  // DQ (2/6/2004): These tests fail in Coco for test2004_14.C
     printf ("Running agressive (slow) internal consistancy tests (after rewrite operation)! \n");
     AstTests::runAllTests(const_cast<SgProject*>(project));
#else
     printf ("Skipped agressive (slow) internal consistancy tests (after rewrite operation)! \n");
#endif

     return backend(project);
   }

