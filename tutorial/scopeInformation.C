// This example shows the scope of each statement and name (variable names, base class names, etc.).

#include "rose.h"

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
  // There are three types ir IR nodes that can be queried for scope:
  //   - SgStatement, and 
  //   - SgInitializedName
     SgStatement* statement = isSgStatement(n);
     if (statement != NULL)
        {
          SgScopeStatement* scope = statement->get_scope();
          ROSE_ASSERT(scope != NULL);
          printf ("SgStatement       = %12p = %30s has scope = %12p = %s (total number = %d) \n",
               statement,statement->class_name().c_str(),
               scope,scope->class_name().c_str(),(int)scope->numberOfNodes());
        }

     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL)
        {
          SgScopeStatement* scope = initializedName->get_scope();
          ROSE_ASSERT(scope != NULL);
          printf ("SgInitializedName = %12p = %30s has scope = %12p = %s (total number = %d)\n",
               initializedName,initializedName->get_name().str(),
               scope,scope->class_name().c_str(),(int)scope->numberOfNodes());
        }
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);

     printf ("Number of scopes (SgScopeStatement) = %d \n",(int)SgScopeStatement::numberOfNodes());
     printf ("Number of scopes (SgBasicBlock)     = %d \n",(int)SgBasicBlock::numberOfNodes());

#if 0
     printf ("\n\n");
     printf ("Now output all the symbols in each symbol table \n");
     SageInterface::outputLocalSymbolTables(project);
     printf ("\n\n");
#endif

     return 0;
   }

