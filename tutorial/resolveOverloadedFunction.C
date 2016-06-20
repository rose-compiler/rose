// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

using namespace std;

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

  // Build a list of functions within the AST
     Rose_STL_Container<SgNode*> functionCallList = NodeQuery::querySubTree (project,V_SgFunctionCallExp);

     int functionCounter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = functionCallList.begin(); i != functionCallList.end(); i++)
        {
          SgExpression* functionExpression = isSgFunctionCallExp(*i)->get_function();
          ROSE_ASSERT(functionExpression != NULL);

          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExpression);

          SgFunctionSymbol* functionSymbol = NULL;
          if (functionRefExp != NULL)
             {
            // Case of non-member function
               functionSymbol = functionRefExp->get_symbol();
             }
            else
             {
            // Case of member function (hidden in rhs of binary dot operator expression)
               SgDotExp* dotExp = isSgDotExp(functionExpression);
               ROSE_ASSERT(dotExp != NULL);

               functionExpression = dotExp->get_rhs_operand();
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionExpression);
               ROSE_ASSERT(memberFunctionRefExp != NULL);

               functionSymbol = memberFunctionRefExp->get_symbol();
             }
          
          ROSE_ASSERT(functionSymbol != NULL);

          SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
          ROSE_ASSERT(functionDeclaration != NULL);
          
       // Output mapping of function calls to function declarations
          printf ("Location of function call #%d at line %d resolved by overloaded function declared at line %d \n",
               functionCounter++,
               isSgFunctionCallExp(*i)->get_file_info()->get_line(),
               functionDeclaration->get_file_info()->get_line());
        }

     return 0;
   }

