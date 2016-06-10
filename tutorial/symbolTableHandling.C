// This example shows how to visit the symbols in the global scopes symbol table.

#include "rose.h"

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
     SgGlobal* globalScope = isSgGlobal(n);
     if (globalScope != NULL)
        {
          printf ("Looking for \"x\" in the global symbol table ... \n");
          SgSymbol* symbol = globalScope->lookup_var_symbol("x");
          ROSE_ASSERT(symbol != NULL);

       // Iterate through the symbol table
          SgSymbolTable::BaseHashType* hashTable = globalScope->get_symbol_table()->get_table();
          SgSymbolTable::hash_iterator i = hashTable->begin();
          while (i != hashTable->end())
             {
               printf ("hash tabel entry: i->first = %s \n",i->first.str());
               i++;
             }
        }
#if 0
     SgScopeStatement* scope = isSgScopeStatement(n);
     if (scope != NULL)
        {
          printf ("Looking for \"x\" in the global symbol table ... \n");
          SgSymbol* symbol = globalScope->lookup_var_symbol("x");
          ROSE_ASSERT(symbol != NULL);
        }
#endif
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

     printf ("\n\n");
     printf ("Now output all the symbols in each symbol table \n");
     SageInterface::outputLocalSymbolTables(project);
     printf ("\n\n");

     return 0;
   }

