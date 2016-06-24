// Example of code to change the types of variables in an applications: changes "float" to "double".
// This is a prelude to a translator for Fortran to fixup literal constants to be type REAL*8 from 
// their defaul in F77 as REAL*4.  This example will be finished once the ROSE Fortran support is 
// available.

#include "rose.h"

using namespace std;

/*
   Design of this code.
      Inputs: source code (file.C)
      Outputs: instrumented source code (rose_file.C and file.o)

   Properties of instrumented source code:
       1) added declaration for coverage support function
          (either forward function declaration or a #include to include a header file).
       2) Each function in the source program is instrumented to include a call to the
          coverage support function/
*/


// Simple ROSE traversal class: This allows us to visit all the functions and add
// new code to instrument/record their use.
class TypeChangeTraversal : public SgSimpleProcessing
   {
     public:
       // required visit function to define what is to be done
          void visit ( SgNode* node );
   };

void
TypeChangeTraversal::visit ( SgNode* node )
   {
     switch(node->variantT()) 
        {
          case V_SgVariableDeclaration:
             {
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
               SgInitializedNamePtrList::iterator i = variableDeclaration->get_variables().begin();
               while ( i != variableDeclaration->get_variables().end() )
                  {
                    SgType* variableType = (*i)->get_type();
                    ROSE_ASSERT(variableType != NULL);
                    SgTypeFloat* floatVariableType = isSgTypeFloat(variableType);
                    if ( floatVariableType != NULL )
                       {
                      // Found a variable decalaration of type float, change to double
                         (*i)->set_type(SgTypeDouble::createType());
                       }
                    i++;
                  }
             }
          default:
             {
            // Nothing to do here
             }
        }
   }

int
main ( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build ROSE AST
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     generateDOT ( *project );

  // Call traversal to do instrumentation (put instumentation into the AST).
     TypeChangeTraversal treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );

  // Generate Code and compile it with backend (vendor) compiler to generate object code
  // or executable (as specified on commandline using vendor compiler's command line).
  // Returns error code form compilation using vendor's compiler.
     return backend(project);
   }

