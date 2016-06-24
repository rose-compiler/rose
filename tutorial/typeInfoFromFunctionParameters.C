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

     int functionCounter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(functionDeclaration != NULL);

       // DQ (3/5/2006): Only output the non-compiler generated IR nodes
          if ( (*i)->get_file_info()->isCompilerGenerated() == false)
             {
               SgFunctionParameterList* functionParameters = functionDeclaration->get_parameterList();
               ROSE_ASSERT(functionDeclaration != NULL);

            // output the function number and the name of the function
               printf ("Non-compiler generated function name #%3d is %s \n",functionCounter++,functionDeclaration->get_name().str());

               SgInitializedNamePtrList & parameterList = functionParameters->get_args();
               int parameterCounter = 0;
               for (SgInitializedNamePtrList::iterator j = parameterList.begin(); j != parameterList.end(); j++)
                  {
                    SgType* parameterType = (*j)->get_type();
                    printf ("   parameterType #%2d = %s \n",parameterCounter++,parameterType->unparseToString().c_str());
                  }
             }
            else
             {
               printf ("Compiler generated function name #%3d is %s \n",functionCounter++,functionDeclaration->get_name().str());
             }
        }

     return 0;
   }

