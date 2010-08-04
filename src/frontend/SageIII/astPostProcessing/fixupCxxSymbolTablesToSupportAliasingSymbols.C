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

#if 1
  // DQ (4/17/2010): Comment this new option out for now while I focus on getting the 
  // language only configure options into place.

#if ALIAS_SYMBOL_DEBUGGING
     printf ("Inside of fixupAstSymbolTablesToSupportAliasedSymbols(node = %p = %s) \n",node,node->class_name().c_str());
#endif

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

#if 0
       // DQ (5/20/2010): Added test (note that SgEnumFieldSymbol fails this test).
       // SgDeclarationStatement* decl = (SgDeclarationStatement*)aliasSymbol->get_declaration();
          SgDeclarationStatement* decl = aliasSymbol->get_declaration();
          if (decl == NULL)
             {
            // Investigate this error!
               printf ("Error in conversion to SgAliasSymbol: original symbol = %p = %s \n",symbol,symbol->class_name().c_str());
             }
          ROSE_ASSERT(decl != NULL);
#endif
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

     SgUseStatement* useDeclaration = isSgUseStatement(node);
     if (useDeclaration != NULL)
        {
       // This must be done in the Fortran AST construction since aliased symbols must be inserted
       // before they are looked up as part of name resolution of variable, functions, and types.
       // For C++ we can be more flexible and support the construction of symbol aliases within 
       // post-processing.
        }

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
            // DQ (5/21/2010): Handle case of using "std" (predefined namespace in C++), but it not having been explicitly defined (see test2005_57.C).
               if (namespaceDeclaration->get_name() != "std")
                  {
                    printf ("ERROR: namespaceDeclaration has no valid definition \n");
                    namespaceDeclaration->get_startOfConstruct()->display("ERROR: namespaceDeclaration has no valid definition");

                 // DQ (5/20/2010): Added assertion to trap this case.
                    printf ("Exiting because referencedScope could not be identified.\n");
                    ROSE_ASSERT(false);
                  }
             }

       // Note that "std", as a predefined namespace, can have a null definition, so we can't 
       // insist that we inject all symbols in namespaces that we can't see explicitly.
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




























