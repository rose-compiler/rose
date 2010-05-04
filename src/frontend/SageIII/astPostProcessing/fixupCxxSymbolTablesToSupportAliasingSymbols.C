#include "sage3basic.h"
#include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"

#define ALIAS_SYMBOL_DEBUGGING 0

void
fixupAstSymbolTablesToSupportAliasedSymbols (SgNode* node)
   {
  // DQ (4/14/2010): For Cxx only.  
  // Adding support for symbol aliasing as a result of using declarations
  // (and other use directives, etc.).

     TimingPerformance timer1 ("Fixup symbol tables to support aliased symbols:");

  // Now fixup the local symbol tables
  // This simplifies how the traversal is called!
     FixupAstSymbolTablesToSupportAliasedSymbols astFixupTraversal;

#if 0
  // DQ (4/17/2010): Comment this new option out for now while I focus on getting the 
  // language only configure options into place.

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
#endif
   }


#if 0
void
trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(
     const SgName & variableName, SgScopeStatement* currentScope, 
     SgVariableSymbol* & variableSymbol, SgFunctionSymbol* & functionSymbol, 
     SgClassSymbol* & classSymbol);
#endif
 
void
FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope ( SgScopeStatement* referencedScope, SgScopeStatement* currentScope )
   {
     ROSE_ASSERT(referencedScope != NULL);
     ROSE_ASSERT(currentScope    != NULL);

     SgSymbolTable* symbolTable = referencedScope->get_symbol_table();
     ROSE_ASSERT(symbolTable != NULL);
     
#if 0
     printf ("AST Fixup: Building Symbol Table for %p = %s at: \n",scope,scope->sage_class_name());
     referencedScope->get_file_info()->display("Symbol Table Location");
#endif

     SgSymbolTable::BaseHashType* internalTable = symbolTable->get_table();
     ROSE_ASSERT(internalTable != NULL);

     int counter = 0;
     SgSymbolTable::hash_iterator i = internalTable->begin();
     while (i != internalTable->end())
        {
       // DQ: removed SgName casting operator to char*
       // cout << "[" << idx << "] " << (*i).first.str();
          ROSE_ASSERT ( (*i).first.str() != NULL );
          ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

#if ALIAS_SYMBOL_DEBUGGING
          printf ("Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) class_name() = %s \n",counter,(*i).first.str(),(*i).second->class_name().c_str());
#endif
          SgName name      = (*i).first;
          SgSymbol* symbol = (*i).second;

          ROSE_ASSERT ( symbol != NULL );

       // Make sure that this is not a SgLabelSymbol, I think these should not be aliased
       // (if only because I don't think that C++ support name qualification for labels).
          ROSE_ASSERT ( isSgLabelSymbol(symbol) == NULL );

       // SgAliasSymbol* aliasSymbol = new SgAliasSymbol (SgSymbol *alias=NULL, bool isRenamed=false, SgName new_name="")
          SgAliasSymbol* aliasSymbol = new SgAliasSymbol (symbol);

       // Use the current name and the alias to the symbol
          currentScope->insert_symbol(name, aliasSymbol);

       // Increment iterator and counter
          i++;
          counter++;
        }

#if 0
  // debugging
     symbolTable->print("In FixupAstSymbolTables::visit(): printing out the symbol tables");
#endif
   }



void
FixupAstSymbolTablesToSupportAliasedSymbols::visit ( SgNode* node )
   {
  // DQ (11/24/2007): Output the current IR node for debugging the traversal of the Fortran AST.
  // printf ("node = %s \n",node->class_name().c_str());

#if 0
  // This must be done in the Fortran AST construction since aliased symbols must be inserted
  // before they are looked up as part of name resolution of variable, functions, and types.
     SgUseStatement* useDeclaration = isSgUseStatement(node);
     if (useDeclaration != NULL)
        {
          printf ("Found the SgUseDeclaration \n");
          SgScopeStatement* currentScope = useDeclaration->get_scope();

          SgName moduleName = useDeclaration->get_name();
          printf ("In FixupFortranUseDeclarations::visit(): name = %s \n",useDeclaration->get_name().str());

          SgClassSymbol* moduleSymbol = NULL;
          trace_back_through_parent_scopes_searching_for_module(moduleName,currentScope,moduleSymbol);

       // if ( (moduleSymbol == NULL) && (matchAgainstIntrinsicFunctionList(variableName.str()) == false) )
          if ( moduleSymbol == NULL )
             {
               printf ("Error: module not found! \n");
               ROSE_ASSERT(false);
             }
            else
             {
            // Found the module, now read the public members...
               printf ("Found the module, now read the public members...\n");

               SgClassDeclaration* nonDefiningClassDeclaration = moduleSymbol->get_declaration();
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(nonDefiningClassDeclaration->get_definingDeclaration());
               ROSE_ASSERT(classDeclaration != NULL);
               printf ("classDeclaration = %p classDeclaration->get_definition() = %p classDeclaration->get_definingDeclaration() = %p classDeclaration->get_firstNondefiningDeclaration() = %p \n",
                       classDeclaration,classDeclaration->get_definition(),classDeclaration->get_definingDeclaration(),classDeclaration->get_firstNondefiningDeclaration());
               ROSE_ASSERT(classDeclaration->get_definition() != NULL);

               SgModuleStatement* moduleStatement = isSgModuleStatement(classDeclaration);
               ROSE_ASSERT(moduleStatement != NULL);

               SgClassDefinition* classDefinition = moduleStatement->get_definition();
               ROSE_ASSERT(classDefinition != NULL);

            // DQ (9/29/2008): inject symbols from module into symbol table at current scope.
               SgSymbol* symbol = classDefinition->first_any_symbol();
               while(symbol != NULL)
                  {
                    bool isRenamed = false;
                    SgAliasSymbol* aliasSymbol = new SgAliasSymbol(symbol,isRenamed);
                    SgName declarationName = symbol->get_name();

                    printf ("Insert aliased symbol name = %s \n",declarationName.str());
                    currentScope->insert_symbol(declarationName,aliasSymbol);

                    symbol = classDefinition->next_any_symbol();
                  }
             }

          if (useDeclaration->get_rename_list() != NULL)
             {
               SgExpressionPtrList & rename_list = useDeclaration->get_rename_list()->get_expressions();
               printf ("rename_list.size() = %zu \n",rename_list.size());

#if 1
               for (size_t i = 0; i < rename_list.size(); i++)
                  {
                 // For each module listed build SgAliasSymbols and add them to the local symbol table.

                    SgExpression* renameExp = rename_list[i];
                    ROSE_ASSERT(renameExp != NULL);
                    printf ("In FixupFortranUseDeclarations::visit(): renameExp = %p = %s = %s \n",renameExp,renameExp->class_name().c_str(),SageInterface::get_name(renameExp).c_str());

                    SgVariableSymbol* variableSymbol = NULL;
                    SgFunctionSymbol* functionSymbol = NULL;
                    SgClassSymbol*    classSymbol    = NULL;
#if 0
                    trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol,classSymbol);
#endif
                  }
             }
#endif
        }
#endif




  // DQ (4/14/2010): Added this C++ specific support.
  // In the future we may want to support the injection of alias symbols for C++ "using" directives and "using" declarations.
     SgUsingDeclarationStatement* usingDeclarationStatement = isSgUsingDeclarationStatement(node);
     if (usingDeclarationStatement != NULL)
        {
#if ALIAS_SYMBOL_DEBUGGING
          printf ("Found the SgUsingDeclarationStatement \n");
#endif
          SgScopeStatement* currentScope = usingDeclarationStatement->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          SgDeclarationStatement* declaration     = usingDeclarationStatement->get_declaration();
          SgInitializedName*      initializedName = usingDeclarationStatement->get_initializedName();

       // Only one of these can be non-null.
          ROSE_ASSERT(initializedName != NULL || declaration != NULL);
          ROSE_ASSERT( (initializedName != NULL && declaration != NULL) == false);

          if (declaration != NULL)
             {
#if ALIAS_SYMBOL_DEBUGGING
               printf ("In FixupAstSymbolTablesToSupportAliasedSymbols::visit(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
             }
            else
             {
               if (initializedName != NULL)
                  {
#if ALIAS_SYMBOL_DEBUGGING
                    printf ("In FixupAstSymbolTablesToSupportAliasedSymbols::visit(): initializedName = %s \n",initializedName->get_name().str());
#endif
                  }
                 else
                  {
                    printf ("Error: both declaration and initializedName in SgUsingDeclarationStatement are NULL \n");
                    ROSE_ASSERT(false);
                  }
             }

#if 0
          printf ("Exiting at the base of FixupAstSymbolTablesToSupportAliasedSymbols::visit() \n");
          ROSE_ASSERT(false);
#endif
        }

     SgUsingDirectiveStatement* usingDirectiveStatement = isSgUsingDirectiveStatement(node);
     if (usingDirectiveStatement != NULL)
        {
#if ALIAS_SYMBOL_DEBUGGING
          printf ("Found the SgUsingDirectiveStatement \n");
#endif
          SgNamespaceDeclarationStatement* namespaceDeclaration = usingDirectiveStatement->get_namespaceDeclaration();
          ROSE_ASSERT(namespaceDeclaration != NULL);

          SgScopeStatement* currentScope    = usingDirectiveStatement->get_scope();

       // To be more specific this is really a SgNamespaceDefinitionStatement
          SgScopeStatement* referencedScope = namespaceDeclaration->get_definition();

          if (referencedScope == NULL)
             {
               printf ("ERROR: namespaceDeclaration has no valid definition \n");
               namespaceDeclaration->get_startOfConstruct()->display("ERROR: namespaceDeclaration has no valid definition");
             }

       // Note that std can have a null definition, so ignore this for now!
          if (referencedScope != NULL)
             {
               ROSE_ASSERT(referencedScope != NULL);
               ROSE_ASSERT(currentScope != NULL);
               injectSymbolsFromReferencedScopeIntoCurrentScope(referencedScope,currentScope);
             }

#if 0
          printf ("Exiting at the base of FixupAstSymbolTablesToSupportAliasedSymbols::visit() \n");
          ROSE_ASSERT(false);
#endif
        }
   }




























