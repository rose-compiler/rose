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


void
FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope ( SgScopeStatement* referencedScope, SgScopeStatement* currentScope, SgAccessModifier::access_modifier_enum accessLevel )
   {
     ROSE_ASSERT(referencedScope != NULL);
     ROSE_ASSERT(currentScope    != NULL);

     SgSymbolTable* symbolTable = referencedScope->get_symbol_table();
     ROSE_ASSERT(symbolTable != NULL);
     
#if 0
     printf ("AST Fixup: Building Symbol Table for %p = %s at: \n",scope,scope->sage_class_name());
     referencedScope->get_file_info()->display("Symbol Table Location");
#endif

     SgClassDefinition* classDefinition = isSgClassDefinition(referencedScope);
     if (classDefinition != NULL)
        {
       // If this is a class definition, then we need to make sure that we only for alias symbols for those declarations.
#if ALIAS_SYMBOL_DEBUGGING
          printf ("Injection of symbols from a class definition needs to respect access privledge (private, protected, public) declarations \n");
#endif
        }

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

       // DQ (6/22/2011): For now skip the handling of alias symbol from other scopes.
       // ROSE_ASSERT(isSgAliasSymbol(symbol) == NULL);
          if (isSgAliasSymbol(symbol) != NULL)
             {
#if ALIAS_SYMBOL_DEBUGGING
               printf ("WARNING: Not clear if we want to nest SgAliasSymbol inside of SgAliasSymbol \n");
#endif
             }

          SgNode* symbolBasis = symbol->get_symbol_basis();
          ROSE_ASSERT(symbolBasis != NULL);
#if ALIAS_SYMBOL_DEBUGGING
          printf ("symbolBasis = %p = %s \n",symbolBasis,symbolBasis->class_name().c_str());
#endif
       // SgDeclarationStatement* declarationFromSymbol = symbol->get_declaration();
          SgDeclarationStatement* declarationFromSymbol = isSgDeclarationStatement(symbolBasis);

          SgAccessModifier::access_modifier_enum declarationAccessLevel = SgAccessModifier::e_unknown;

       // ROSE_ASSERT(declarationFromSymbol != NULL);
          if (declarationFromSymbol != NULL)
             {
            // DQ (6/22/2011): Can I or should I do relational operatios on enum values (note that the values are designed to allow this).
               declarationAccessLevel = declarationFromSymbol->get_declarationModifier().get_accessModifier().get_modifier();
             }
            else
             {
               SgInitializedName* initializedNameFromSymbol = isSgInitializedName(symbolBasis);
               ROSE_ASSERT(initializedNameFromSymbol != NULL);
               ROSE_ASSERT(initializedNameFromSymbol->get_declptr() != NULL);

               declarationAccessLevel = initializedNameFromSymbol->get_declptr()->get_declarationModifier().get_accessModifier().get_modifier();
             }

#if ALIAS_SYMBOL_DEBUGGING
          printf ("declarationAccessLevel = %d accessLevel = %d \n",declarationAccessLevel,accessLevel);
#endif
          if (declarationAccessLevel >= accessLevel)
             {
            // This declaration is visible, so build an alias.

            // SgAliasSymbol* aliasSymbol = new SgAliasSymbol (SgSymbol *alias=NULL, bool isRenamed=false, SgName new_name="")
               SgAliasSymbol* aliasSymbol = new SgAliasSymbol (symbol);

#if ALIAS_SYMBOL_DEBUGGING
               printf ("Adding symbol to new scope as a SgAliasSymbol = %p \n",aliasSymbol);
#endif
            // Use the current name and the alias to the symbol
               currentScope->insert_symbol(name, aliasSymbol);
             }
            else
             {
#if ALIAS_SYMBOL_DEBUGGING
               printf ("NO SgAliasSymbol ADDED (wrong permissions): declarationFromSymbol = %p \n",declarationFromSymbol);
#endif
             }
#if 0
       // Older version of code...
       // SgAliasSymbol* aliasSymbol = new SgAliasSymbol (SgSymbol *alias=NULL, bool isRenamed=false, SgName new_name="")
          SgAliasSymbol* aliasSymbol = new SgAliasSymbol (symbol);

       // Use the current name and the alias to the symbol
          currentScope->insert_symbol(name, aliasSymbol);
#endif

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
               injectSymbolsFromReferencedScopeIntoCurrentScope(referencedScope,currentScope,SgAccessModifier::e_default);
             }

#if 0
          printf ("Exiting at the base of FixupAstSymbolTablesToSupportAliasedSymbols::visit() \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (5/6/2011): Added support to build SgAliasSymbols in derived class scopes that reference the symbols of the base classes associated with protected and public declarations.
     SgClassDefinition* classDefinition = isSgClassDefinition(node);
     if (classDefinition != NULL)
        {
       // Handle any derived classes.
          SgBaseClassPtrList & baseClassList = classDefinition->get_inheritances();
          SgBaseClassPtrList::iterator i = baseClassList.begin();
          while (i != baseClassList.end())
             {
            // Check each base class.
               SgBaseClass* baseClass = *i;
               ROSE_ASSERT(baseClass != NULL);

            // printf ("baseClass->get_baseClassModifier().displayString()                      = %s \n",baseClass->get_baseClassModifier().displayString().c_str());
            // printf ("baseClass->get_baseClassModifier().get_accessModifier().displayString() = %s \n",baseClass->get_baseClassModifier().get_accessModifier().displayString().c_str());

            // if (baseClass->get_modifier() == SgBaseClass::e_virtual)
               if (baseClass->get_baseClassModifier().get_modifier() == SgBaseClassModifier::e_virtual)
                  {
                 // Not clear if virtual as a modifier effects the handling of alias symbols.
                 // printf ("Not clear if virtual as a modifier effects the handling of alias symbols. \n");
                  }

            // DQ (6/22/2011): Define the access level for alias symbol's declarations to be included.
               SgAccessModifier::access_modifier_enum accessLevel = baseClass->get_baseClassModifier().get_accessModifier().get_modifier();

               SgClassDeclaration* tmpClassDeclaration    = baseClass->get_base_class();
               ROSE_ASSERT(tmpClassDeclaration != NULL);
               SgClassDeclaration* targetClassDeclaration = isSgClassDeclaration(tmpClassDeclaration->get_definingDeclaration());
               ROSE_ASSERT(targetClassDeclaration != NULL);
               SgScopeStatement*   referencedScope  = targetClassDeclaration->get_definition();

            // We need this function to restrict it's injection of symbol to just those that are associated with public and protected declarations.
               injectSymbolsFromReferencedScopeIntoCurrentScope(referencedScope,classDefinition,accessLevel);

               i++;
             }
        }


     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL)
        {
#if ALIAS_SYMBOL_DEBUGGING
          printf ("Found a the SgFunctionDeclaration \n");
#endif
       // SgScopeStatement*  functionScope   = functionDeclaration->get_scope();
          SgScopeStatement*  currentScope    = isSgScopeStatement(functionDeclaration->get_parent());
          SgClassDefinition* classDefinition = isSgClassDefinition(currentScope);

          if (classDefinition != NULL)
             {
            // This is a function declared in a class definition, test of friend (forget why it is important to test for isOperator().
               if (functionDeclaration->get_declarationModifier().isFriend() == true || functionDeclaration->get_specialFunctionModifier().isOperator() == true)
                  {
                 // printf ("Process all friend function with a SgAliasSymbol to where they are declared in another scope (usually global scope) \n");
#if 0
                    SgName name = functionDeclaration->get_name();

                    SgSymbol* symbol = functionDeclaration->search_for_symbol_from_symbol_table();
                    ROSE_ASSERT ( symbol != NULL );

                    SgAliasSymbol* aliasSymbol = new SgAliasSymbol (symbol);

                 // Use the current name and the alias to the symbol
                    currentScope->insert_symbol(name,aliasSymbol);
#endif
#if 0
                    printf ("Error: friend functions not processed yet! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
        }          
   }




























