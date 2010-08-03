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

#if 0
  // compute some statistical data about redundant function types
     static int numberOfFunctionTypesProcessed = 0;
     static int numberOfRedudantFunctionTypesProcessed = 0;
     numberOfFunctionTypesProcessed++;
#endif

  // DQ (1/26/2007): Added fixup to place function types into the global function type symbol table.
     SgFunctionType* functionType = isSgFunctionType(node);
     ROSE_ASSERT(functionType != NULL);

     SgFunctionTypeTable* globalFunctionTypeSymbolTable = SgNode::get_globalFunctionTypeTable();
     ROSE_ASSERT(globalFunctionTypeSymbolTable != NULL);

  // printf ("Processing SgFunctionType = %p = %s \n",functionType,functionType->get_mangled().str());

  // DQ (3/10/2007): The computation of the mangled name data is only 0.254 sec of the total time of 6.765 sec for an example file (test2001_11.C)
     const SgName mangleTypeName = functionType->get_mangled();

  // DQ (3/10/2007): This symbol table test (together with the insertion of the symbol) is expensive (26/27ths of the cost)
     if (globalFunctionTypeSymbolTable->lookup_function_type(mangleTypeName) == NULL)
        {
       // printf ("Function type not in table, ADDING it: SgFunctionType = %p = %s \n",functionType,functionType->get_mangled().str());
          globalFunctionTypeSymbolTable->insert_function_type(mangleTypeName,functionType);
        }
       else
        {
       // printf ("Function type already in the table, SKIP adding it, this is a redundantly generated function type: SgFunctionType = %p = %s \n",functionType,functionType->get_mangled().str());
#if 0
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
          SgSymbolTable* symbolTable = scope->get_symbol_table();
          if (symbolTable == NULL)
             {
#if 0
               printf ("AST Fixup: Building Symbol Table for %p = %s at: \n",scope,scope->sage_class_name());
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

#if 0
       // DQ (8/2/2005): this has been moved to the AST consistancy tests
          SgSymbolTable::hash_iterator i = internalTable->begin();
          while (i != internalTable->end())
             {
            // DQ: removed SgName casting operator to char*
            // cout << "[" << idx << "] " << (*i).first.str();
               ROSE_ASSERT ( (*i).first.str() != NULL );
               ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

            // printf ("Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) sage_class_name() = %s \n",
            //      idx,(*i).first.str(),(*i).second->sage_class_name());

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
                         break;
                       }
                    case V_SgDefaultSymbol:
                       {
                         printf ("The SgDefaultSymbol should not be present in the AST \n");
                         ROSE_ASSERT(false);
                         break;
                       }
                    case V_SgEnumFieldSymbol:
                       {
                      // Note that the type returned by get_declaration is SgInitializedName and not any sort of SgDeclaration
                         SgEnumFieldSymbol* enumFieldSymbol = isSgEnumFieldSymbol(symbol);
                         ROSE_ASSERT(enumFieldSymbol != NULL);
                         ROSE_ASSERT(enumFieldSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgEnumSymbol:
                       {
                         SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                         ROSE_ASSERT(enumSymbol != NULL);
                         ROSE_ASSERT(enumSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgFunctionSymbol:
                       {
                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                         ROSE_ASSERT(functionSymbol != NULL);
                         ROSE_ASSERT(functionSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgFunctionTypeSymbol:
                       {
                      // Note that we check the get_type() function here and not get_declaration()
                         SgFunctionTypeSymbol* functionTypeSymbol = isSgFunctionTypeSymbol(symbol);
                         ROSE_ASSERT(functionTypeSymbol != NULL);
                         ROSE_ASSERT(functionTypeSymbol->get_type() != NULL);
                         break;
                       }
                    case V_SgLabelSymbol:
                       {
                         SgLabelSymbol* labelSymbol = isSgLabelSymbol(symbol);
                         ROSE_ASSERT(labelSymbol != NULL);
                         ROSE_ASSERT(labelSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgNamespaceSymbol:
                       {
                         SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);
                         ROSE_ASSERT(namespaceSymbol != NULL);
                         ROSE_ASSERT(namespaceSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgTemplateSymbol:
                       {
                         SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                         ROSE_ASSERT(templateSymbol != NULL);
                         ROSE_ASSERT(templateSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgTypedefSymbol:
                       {
                         SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                         ROSE_ASSERT(typedefSymbol != NULL);
                         ROSE_ASSERT(typedefSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgVariableSymbol:
                       {
                      // Note that the type returned by get_declaration is SgInitializedName and not any sort of SgDeclaration
                         SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
                         ROSE_ASSERT(variableSymbol != NULL);
                         ROSE_ASSERT(variableSymbol->get_declaration() != NULL);
                         break;
                       }
                    default:
                       {
                         printf ("Error: default reached in switch (AstFixes.C) symbol = %s \n",symbol->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }

            // Increment iterator!
               i++;
             }
#endif

#if 0
       // debugging
          symbolTable->print("In FixupAstSymbolTables::visit(): printing out the symbol tables");
#endif
        }
   }


