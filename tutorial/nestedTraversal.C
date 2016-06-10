// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

class nestedVisitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
     if (isSgFunctionDeclaration(n) != NULL)
        {
          printf ("Found a function declaration ... \n");

       // Build the nested traversal object
          nestedVisitorTraversal exampleTraversal;

       // Call the traversal starting at the project node of the AST (traverse in postorder just to be different)
       // Note that we call the traverse function instead of traverseInputFiles, because we are not starting at 
       // the AST root.
          exampleTraversal.traverse(n,postorder);
        }
   }

void nestedVisitorTraversal::visit(SgNode* n)
   {
     if (isSgFunctionDefinition(n) != NULL)
        {
          printf ("Found a function definition within the function declaration ... \n");
        }
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     if (SgProject::get_verbose() > 0)
          printf ("In visitorTraversal.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);

     return 0;
   }


