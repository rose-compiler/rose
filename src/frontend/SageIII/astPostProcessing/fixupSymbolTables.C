// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupSymbolTables.h"

void fixupGlobalFunctionSymbolTable (SgFunctionTypeTable* globalFunctionTypeSymbolTable)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup global symbol table:");

  // printf ("Fixup the global function type symbol table ... \n");

  // printf ("Sorry, Fixup of the global function type symbol table not yet implemented globalFunctionTypeSymbolTable->get_function_type_table()->size() = %ld \n",
  //      SgNode::get_globalFunctionTypeTable()->get_function_type_table()->size());
     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() == globalFunctionTypeSymbolTable);

     FixUpGlobalFunctionTypeTable v;
     SgFunctionType::traverseMemoryPoolNodes(v);

  // DQ (2/3/2007): Also add in the member function types, since we could have pointers to these
  // referenced from global scope and we need to resolve sets of functions that could be called
  // from both function pointers and member functions pointers.
     SgMemberFunctionType::traverseMemoryPoolNodes(v);

#if 0
  // debugging
     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_function_type_table() != NULL);
     SgNode::get_globalFunctionTypeTable()->get_function_type_table()->print("In FixupAstSymbolTables::visit(): printing out the symbol tables");
#endif
   }

void
FixUpGlobalFunctionTypeTable::visit(SgNode* node)
   {
  // This function will be used to visit every node in the SgFunctionType memory pool and the SgMemberFunctionType
  // memory pool and insert any symbols that are missing after the initial construction of the AST.
  // Note that the reason why we save the construction of this table to a post-processing step is that
  // we have to first have the final function names and scope names as required to build the final mangled names.
  // Since many function will have the same function type and function types are shared, the symbols for 
  // any given function type could have already been placed into the function type symbol tabel and
  // we have to test each function type, which forces a call to get the mangled name for each function type.

  // We could improve the performance by optimizing the computation of mangled names.
  // We could also improve the performance by optimizing the globalFunctionTypeSymbolTable->lookup_function_type().

#define DEBUG_FUNCTION_TYPE_TABLE 0

#if DEBUG_FUNCTION_TYPE_TABLE
  // compute some statistical data about redundant function types
     static int numberOfFunctionTypesProcessed = 0;
     static int numberOfRedudantFunctionTypesProcessed = 0;
     numberOfFunctionTypesProcessed++;
#endif

  // DQ (1/26/2007): Added fixup to place function types into the global function type symbol table.
     SgFunctionType* functionType = isSgFunctionType(node);
     ROSE_ASSERT(functionType != NULL);

#if DEBUG_FUNCTION_TYPE_TABLE
     printf ("Processing SgFunctionType = %p = %s numberOfFunctionTypesProcessed = %d \n",functionType,functionType->class_name().c_str(),numberOfFunctionTypesProcessed);
#endif

     SgFunctionTypeTable* globalFunctionTypeSymbolTable = SgNode::get_globalFunctionTypeTable();
     ROSE_ASSERT(globalFunctionTypeSymbolTable != NULL);

#if DEBUG_FUNCTION_TYPE_TABLE
     printf ("Processing SgFunctionType: calling functionType->get_mangled(): functionType = %p = %s \n",functionType,functionType->class_name().c_str());
#endif

  // DQ (3/10/2007): The computation of the mangled name data is only 0.254 sec of the total time of 6.765 sec for an example file (test2001_11.C)
     const SgName mangleTypeName = functionType->get_mangled();

#if DEBUG_FUNCTION_TYPE_TABLE
     printf ("AFTER call to functionType->get_mangled(): mangleTypeName = %s \n",mangleTypeName.str());
#endif

  // DQ (3/10/2007): This symbol table test (together with the insertion of the symbol) is expensive (26/27ths of the cost)
     if (globalFunctionTypeSymbolTable->lookup_function_type(mangleTypeName) == NULL)
        {
#if DEBUG_FUNCTION_TYPE_TABLE
       // printf ("Function type not in table, ADDING it: SgFunctionType = %p = %s \n",functionType,functionType->get_mangled().str());
          printf ("Function type not in table, ADDING it: SgFunctionType = %p mangleTypeName = %s \n",functionType,mangleTypeName.str());
#endif
          globalFunctionTypeSymbolTable->insert_function_type(mangleTypeName,functionType);
        }
       else
        {
       // printf ("Function type already in the table, SKIP adding it, this is a redundantly generated function type: SgFunctionType = %p = %s \n",functionType,functionType->get_mangled().str());
#if DEBUG_FUNCTION_TYPE_TABLE
          numberOfRedudantFunctionTypesProcessed++;
          printf ("Function type already in the table: numberOfFunctionTypesProcessed = %ld numberOfRedudantFunctionTypesProcessed = %ld \n",numberOfFunctionTypesProcessed,numberOfRedudantFunctionTypesProcessed);
#endif
        }
   }

void
fixupAstSymbolTables( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup symbol tables:");

  // DQ (1/31/2006): Modified to build all types in the memory pools
  // DQ (6/26/2005): The global function type symbol table should be rebuilt (since the names of templates 
  // used in qualified names of types have been reset (in post processing).
  // extern SgFunctionTypeTable Sgfunc_type_table;
  // fixupGlobalFunctionSymbolTable(Sgfunc_type_table);
     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
     fixupGlobalFunctionSymbolTable(SgNode::get_globalFunctionTypeTable());

  // DQ (5/16/2006): Fixup the parent of the globalFunctionTypeTable to be either the SgProject or SgFile
     if (SgNode::get_globalFunctionTypeTable()->get_parent() == NULL)
        {
          SgProject* project = isSgProject(node);
          SgFile*    file    = isSgFile(node);
          if (project != NULL || file != NULL)
               SgNode::get_globalFunctionTypeTable()->set_parent(node);
        }

  // DQ (7/25/2010): Set the parent of the global type table to the SgProject
     if (SgNode::get_globalTypeTable()->get_parent() == NULL)
        {
          SgProject* project = isSgProject(node);
          SgFile*    file    = isSgFile(node);
          if (project != NULL || file != NULL)
               SgNode::get_globalTypeTable()->set_parent(node);
        }

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer2 ("Fixup local symbol tables:");

  // Now fixup the local symbol tables
  // This simplifies how the traversal is called!
     FixupAstSymbolTables astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

void
FixupAstSymbolTables::visit ( SgNode* node )
   {
  // DQ (6/27/2005): Output the local symbol table from each scope.
  // printf ("node = %s \n",node->sage_class_name());

     SgScopeStatement* scope = isSgScopeStatement(node);
     if (scope != NULL)
        {
#if 0
          printf ("AST Fixup: Fixup Symbol Table for %p = %s at: \n",scope,scope->class_name().c_str());
#endif
          SgSymbolTable* symbolTable = scope->get_symbol_table();
          if (symbolTable == NULL)
             {
#if 0
               printf ("AST Fixup: Fixup Symbol Table for %p = %s at: \n",scope,scope->class_name().c_str());
               scope->get_file_info()->display("Symbol Table Location");
#endif
               SgSymbolTable* tempSymbolTable = new SgSymbolTable();
               ROSE_ASSERT(tempSymbolTable != NULL);

            // name this table as compiler generated! The name is a static member used to store 
            // state for the next_symbol() functions. It is meaningless to set these.
            // tempSymbolTable->set_name("compiler-generated symbol table");

               scope->set_symbol_table(tempSymbolTable);

            // reset the symbolTable using the get_symbol_table() member function
               symbolTable = scope->get_symbol_table();
               ROSE_ASSERT(symbolTable != NULL);

            // DQ (2/16/2006): Set this parent directly (now tested)
               symbolTable->set_parent(scope);
               ROSE_ASSERT(symbolTable->get_parent() != NULL);
             }
          ROSE_ASSERT(symbolTable != NULL);

          if (symbolTable->get_parent() == NULL)
             {
               printf ("Warning: Fixing up symbolTable, calling symbolTable->set_parent() (parent not previously set) \n");
               symbolTable->set_parent(scope);
             }
          ROSE_ASSERT(symbolTable->get_parent() != NULL);

       // Make sure that the internal hash table used in the symbol table is also present!
          if (symbolTable->get_table() == NULL)
             {
            // DQ (6/27/2005): There are a lot of these built, perhaps more than we really need!
#if 0
               printf ("AST Fixup: Building internal Symbol Table hash table (rose_hash_multimap) for %p = %s at: \n",
                    scope,scope->sage_class_name());
               scope->get_file_info()->display("Symbol Table Location");
#endif
               rose_hash_multimap* internalHashTable = new rose_hash_multimap();
               ROSE_ASSERT(internalHashTable != NULL);
               symbolTable->set_table(internalHashTable);
             }
          ROSE_ASSERT(symbolTable->get_table() != NULL);

          SgSymbolTable::BaseHashType* internalTable = symbolTable->get_table();
          ROSE_ASSERT(internalTable != NULL);


       // DQ (6/23/2011): Note: Declarations that reference types that have not been seen yet may be placed into the 
       // wronge scope, then later when we see the correct scope we have a symbol in two or more symbol tables.  The 
       // code below detects and fixes this problem.

       // DQ (6/16/2011): List of symbols we need to remove from symbol tables where they are multibily represented.
          std::vector<SgSymbol*> listOfSymbolsToRemove;

       // DQ (6/12/2011): Fixup symbol table by removing symbols that are not associated with a declaration in the current scope.
          int idx = 0;
          SgSymbolTable::hash_iterator i = internalTable->begin();
          while (i != internalTable->end())
             {
            // DQ: removed SgName casting operator to char*
            // cout << "[" << idx << "] " << (*i).first.str();
               ROSE_ASSERT ( (*i).first.str() != NULL );
               ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );
#if 0
               printf ("Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) sage_class_name() = %s \n",
                       idx,(*i).first.str(),(*i).second->class_name().c_str());
#endif
               SgSymbol* symbol = isSgSymbol((*i).second);
               ROSE_ASSERT ( symbol != NULL );

            // We have to look at each type of symbol separately!  This is because there is no virtual function,
            // the reason for this is that each get_declaration() function returns a different type!
            // ROSE_ASSERT ( symbol->get_declaration() != NULL );
               switch(symbol->variantT())
                  {
                    case V_SgClassSymbol:
                       {
                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                         ROSE_ASSERT(classSymbol != NULL);
                         ROSE_ASSERT(classSymbol->get_declaration() != NULL);

                         SgDeclarationStatement* declarationToFindInScope = NULL;

                      // Search for the declaration in the associated scope.
                         declarationToFindInScope = classSymbol->get_declaration();
                         ROSE_ASSERT(declarationToFindInScope != NULL);

                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationToFindInScope);
                         ROSE_ASSERT(classDeclaration != NULL);

                         SgName name = classDeclaration->get_name();

                      // SgType* declarationType = declarationToFindInScope->get_type();
                         SgType* declarationType = classDeclaration->get_type();
                         ROSE_ASSERT(declarationType != NULL);

                         if (declarationToFindInScope->get_definingDeclaration() != NULL)
                            {
                              declarationToFindInScope = declarationToFindInScope->get_definingDeclaration();
                              SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(declarationToFindInScope);
                              ROSE_ASSERT(definingClassDeclaration != NULL);

                           // SgType* definingDeclarationType = declarationToFindInScope->get_type();
                              SgType* definingDeclarationType = definingClassDeclaration->get_type();
                              ROSE_ASSERT(definingDeclarationType != NULL);

                           // DQ (6/22/2011): This assertion fails for CompileTests/copyAST_tests/copytest2007_24.C
                           // A simple rule that all declarations should follow (now that we have proper global type tables).
                           // ROSE_ASSERT(definingDeclarationType == declarationType);
                              if (definingDeclarationType != declarationType)
                                 {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                                   printf ("In fixupSymbolTables.C: Note that definingDeclarationType != declarationType \n");
#endif
                                 }
                            }

                         SgNamedType* namedType = isSgNamedType(declarationType);
                         ROSE_ASSERT(namedType != NULL);

                         SgDeclarationStatement* declarationAssociatedToType = namedType->get_declaration();
                         ROSE_ASSERT(declarationAssociatedToType != NULL);
#if 0
                         printf ("Found a symbol without a matching declaration in the current scope (declList): declarationToFindInScope = %p = %s \n",declarationToFindInScope,declarationToFindInScope->class_name().c_str());
                         printf ("Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) class_name() = %s \n",idx,(*i).first.str(),(*i).second->class_name().c_str());
#endif
                         SgScopeStatement* scopeOfDeclarationToFindInScope      = declarationToFindInScope->get_scope();
                         SgScopeStatement* scopeOfDeclarationAssociatedWithType = declarationAssociatedToType->get_scope();
#if 0
                         printf ("declarationToFindInScope = %p declarationToFindInScope->get_scope() = %p = %s \n",declarationToFindInScope,declarationToFindInScope->get_scope(),declarationToFindInScope->get_scope()->class_name().c_str());
                         printf ("declarationAssociatedToType = %p declarationAssociatedToType->get_scope() = %p = %s \n",declarationAssociatedToType,declarationAssociatedToType->get_scope(),declarationAssociatedToType->get_scope()->class_name().c_str());
#endif
                         if (scopeOfDeclarationToFindInScope != scopeOfDeclarationAssociatedWithType)
                            {
                           // DQ (6/12/2011): Houston, we have a problem!  The trick is to fix it...
                           // A symbol has been placed into a scope when we could not be certain which scope it should be placed.
                           // We have a default of placing such symbols into the global scope, but it might be better to just have 
                           // a special scope where such symbols could be placed so that we could have them separate from the global 
                           // scope and then fix them up more clearly.

                           // Note that test2011_80.C still fails but the AST is at least correct (I think).
                              SgGlobal* scopeOfDeclarationToFindInScope_GlobalScope      = isSgGlobal(scopeOfDeclarationToFindInScope);
                           // SgGlobal* scopeOfDeclarationAssociatedWithType_GlobalScope = isSgGlobal(scopeOfDeclarationAssociatedWithType);

                              if (scopeOfDeclarationToFindInScope_GlobalScope != NULL)
                                 {
                                // In general which ever scope is the global scope is where the error is...???
                                // This is because when we don't know where to put a symbol (e.g. from a declaration of a pointer) we put it into global scope.
                                // There is even an agrument that this is correct as a default for C/C++, but only if it must exist (see test2011_80.C).
                                // Remove the symbol from the symbol table of the global scope.

#if 0
                                   printf ("Remove the associated symbol in the current symbol table \n");
#endif

                                // DQ (6/22/2011): This assertion fails for CompileTests/copyAST_tests/copytest2007_24.C
                                // ROSE_ASSERT (declarationToFindInScope->get_scope() == declarationAssociatedToType->get_scope());
#if 0
                                   if (declarationToFindInScope->get_scope() != declarationAssociatedToType->get_scope())
                                        printf ("In fixupSymbolTables.C: Note that declarationToFindInScope->get_scope() != declarationAssociatedToType->get_scope() \n");
#endif
                                 }
                                else
                                 {
                                   listOfSymbolsToRemove.push_back(classSymbol);
                                 }
                            }
                      // ROSE_ASSERT (declarationToFindInScope->get_scope() == declarationAssociatedToType->get_scope());

                         break;
                       }

                    default:
                       {
                      // It night be there are are no other types of symbols to consider...

                      // printf ("Ignoring non SgClassSymbols (fixupSymbolTables.C) symbol = %s \n",symbol->class_name().c_str());
                      // ROSE_ASSERT(false);
                       }
                  }

            // Increment iterator!
               i++;

            // Increment counter!
               idx++;
             }

       // DQ (6/18/2011): Now that we are through with the symbol table we can support removal of any 
       // identified problematic symbol without worrying about STL iterator invalidation.
          for (size_t j = 0; j < listOfSymbolsToRemove.size(); j++)
             {
            // Remove these symbols.
               SgSymbol* removeSymbol = listOfSymbolsToRemove[j];
               ROSE_ASSERT(removeSymbol != NULL);
               SgSymbolTable* associatedSymbolTable = isSgSymbolTable(removeSymbol->get_parent());
               ROSE_ASSERT(associatedSymbolTable != NULL);

               ROSE_ASSERT(associatedSymbolTable == symbolTable);

               associatedSymbolTable->remove(removeSymbol);

               printf ("Redundant symbol removed...from symbol table \n");
            // ROSE_ASSERT(false);
             }
#if 0
       // debugging
          symbolTable->print("In FixupAstSymbolTables::visit(): printing out the symbol tables");
#endif
        }
   }


