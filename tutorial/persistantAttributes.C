// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

class persistantAttribute : public AstAttribute
   {
     public:
          int value;
          persistantAttribute (int v) : value(v) {}
   };

class visitorTraversalSetAttribute : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversalSetAttribute::visit(SgNode* n)
   {
     if (isSgForStatement(n) != NULL)
        {
          printf ("Found a for loop (set the attribute) ... \n");

       // Build an attribute (on the heap)
          AstAttribute* newAttribute = new persistantAttribute(5);
          ROSE_ASSERT(newAttribute != NULL);

       // Add it to the AST (so it can be found later in another pass over the AST)
          n->addNewAttribute("MyNewAttribute",newAttribute);
        }
   }

class visitorTraversalReadAttribute : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversalReadAttribute::visit(SgNode* n)
   {
     if (isSgForStatement(n) != NULL)
        {
          printf ("Found a for loop (read the attribute) ... \n");

       // Add it to the AST (so it can be found later in another pass over the AST)
       // AstAttribute* existingAttribute = n->attribute["MyNewAttribute"];
       // DQ (1/2/2006): Added support for new attribute interface.
       // printf ("visitorTraversalReadAttribute::visit(): using new attribute interface \n");
       // AstAttribute* existingAttribute = n->attribute()["MyNewAttribute"];
          AstAttribute* existingAttribute = n->getAttribute("MyNewAttribute");
          ROSE_ASSERT(existingAttribute != NULL);

          printf ("Existing attribute at %p value = %d \n",n,dynamic_cast<persistantAttribute*>(existingAttribute)->value);
        }
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object to set persistant AST attributes
     visitorTraversalSetAttribute exampleTraversalSettingAttribute;

  // Call the traversal starting at the project node of the AST
     exampleTraversalSettingAttribute.traverseInputFiles(project,preorder);

  // Build the traversal object to read any existing AST attributes
     visitorTraversalReadAttribute exampleTraversalReadingAtribute;

  // Call the traversal starting at the project node of the AST
     exampleTraversalReadingAtribute.traverseInputFiles(project,preorder);

     return 0;
   }
