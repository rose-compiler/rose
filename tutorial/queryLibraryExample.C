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
     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);

     int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(functionDeclaration != NULL);

       // DQ (3/5/2006): Only output the non-compiler generated IR nodes
          if ( (*i)->get_file_info()->isCompilerGenerated() == false)
             {
            // output the function number and the name of the function
               printf ("Function #%2d name is %s at line %d \n",
                    counter++,functionDeclaration->get_name().str(),
                    functionDeclaration->get_file_info()->get_line());
             }
            else
             {
            // Output something about the compiler-generated builtin functions
               printf ("Compiler-generated (builtin) function #%2d name is %s \n",
                    counter++,functionDeclaration->get_name().str());
             }
        }

  // Note: Show composition of AST queries

     return 0;
   }

