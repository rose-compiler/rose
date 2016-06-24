#include "rose.h"

class Visitor: public AstSimpleProcessing
   {
     public:
     virtual void visit(SgNode* n);
   };

void
Visitor::visit(SgNode* n)
   {
  // Find the specific function "goo" ...
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
     if (functionDeclaration != NULL)
        {
          if (functionDeclaration->get_name().getString() == "goo")
             {
            // Loop over the function parameters...
               SgInitializedNamePtrList & argList = functionDeclaration->get_args();
               for (size_t i=0; i < argList.size(); i++)
                  {
                 // Get each function parameter...
                    SgInitializedName* var = argList[i];

                 // Only transform pointer types (since "restrict" does not make since on non-pointer types)...
                    SgPointerType* pointerType = isSgPointerType(var->get_type());
                    if (pointerType != NULL)
                       {
                      // Convert this to a restrict pointer.
                         SgModifierType * modifierType = new SgModifierType(pointerType);

                      // Mark this type as "restrict"
                         modifierType->get_typeModifier().setRestrict();

                         var->set_type(modifierType);
                       }
                  }
             }
        }
   }

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);
     Visitor v;
     v.traverse(project, postorder);
     generateAstGraph(project, 4000);
     return backend(project);
   }


/*

INPUT CODE:

// this code works as input and is output with restrict (no transformation)
int foo(int * __restrict__ a)
{
   return a[0] ;
}

// setRestrict.C transform the input parameter to be a "restrict" pointer
int goo(int *b)
{
  return b[0] ;
}

OUTPUT CODE:
// this code works as input and in output with restrict

int foo(int *__restrict__ a)
{
  return a[0];
}

// this code does not use restrict and no transformation seems to cause restrict to be output (for the "b" function parameter)

int goo(int *__restrict__ b)
{
  return b[0];
}

*/
