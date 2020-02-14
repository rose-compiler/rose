// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "rose_config.h"

#include "fixupDefiningAndNondefiningDeclarations.h"

// required for rprintf
using namespace Sawyer::Message;

void fixupAstDefiningAndNondefiningDeclarations( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup defining and non-defining declarations:");

  // This simplifies how the traversal is called!
     FixupAstDefiningAndNondefiningDeclarations astFixupTraversal;

#if 0
  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
#else
  // DQ (1/29/2007): This traversal now uses the memory pool (so that we will visit declaration hidden in types (e.g. SgClassType)
  // SgClassType::traverseMemoryPoolNodes(v);
     astFixupTraversal.traverseMemoryPool();
#endif

#if 0
  // DQ (3/4/2007): temp debugging code to check where defining and non-defining declarations were being set equal!
     extern SgDeclarationStatement* saved_declaration;
     ROSE_ASSERT(saved_declaration != NULL);
     mprintf ("saved_declaration = %p saved_declaration->get_definingDeclaration() = %p saved_declaration->get_firstNondefiningDeclaration() = %p \n",
          saved_declaration,saved_declaration->get_definingDeclaration(),saved_declaration->get_firstNondefiningDeclaration());
     ROSE_ASSERT(saved_declaration->get_definingDeclaration() != saved_declaration->get_firstNondefiningDeclaration());
#endif
   }

void
FixupAstDefiningAndNondefiningDeclarations::visit ( SgNode* node )
   {
  // DQ (6/24/2005): Fixup of defining and non-defining declaration pointers for each SgDeclarationStatement
#if 0
      mprintf ("In FixupAstDefiningAndNondefiningDeclarations::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif

  // DQ (2/21/2007): Modified to force SgVariableDeclaration IR nodes to follow the rules.
  // DQ (1/29/2007) In the conversion of this from an AST traversal to a Memory Pool traversal we want to skip this IR node!
  // if ( isSgFunctionParameterList(node) != NULL || isSgVariableDeclaration(node) != NULL || isSgVariableDefinition(node) != NULL)
     if ( isSgFunctionParameterList(node) != NULL || isSgVariableDefinition(node) != NULL)
        {
       // mprintf ("Skipping case of SgFunctionParameterList or SgVariableDeclaration in FixupAstDefiningAndNondefiningDeclarations::visit() node = %p = %s \n",node,node->class_name().c_str());
          return;
        }

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     SgDeclarationStatement* definingDeclaration         = NULL;
     SgDeclarationStatement* firstNondefiningDeclaration = NULL;
     if (declaration != NULL)
        {
          ROSE_ASSERT(declaration != NULL);
       // mprintf ("This is a declaration statement (node = %p = %s = %s) \n",declaration,declaration->sage_class_name(),SageInterface::get_name(declaration).c_str());

          definingDeclaration         = declaration->get_definingDeclaration();
          firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
#if 0
          mprintf ("In FixupAstDefiningAndNondefiningDeclarations: node = %p = %s definingDeclaration = %p firstNondefiningDeclaration = %p \n",
               node,node->class_name().c_str(),definingDeclaration,firstNondefiningDeclaration);
#endif


#if 0
       // FMZ (6/8/2008): caused core dump when read in a .rmod file
          if (definingDeclaration == NULL && firstNondefiningDeclaration == NULL)  
             {
               mprintf ("Error: declaration = %p = %s definingDeclaration         = %p \n",declaration,declaration->sage_class_name(),definingDeclaration);
               mprintf ("Error: declaration = %p = %s firstNondefiningDeclaration = %p \n",declaration,declaration->sage_class_name(),firstNondefiningDeclaration);
             }
       // DQ (4/18/18): This can't be used for Fortran code.
          ROSE_ASSERT(definingDeclaration != NULL || firstNondefiningDeclaration != NULL);
#endif



#if 0
          mprintf ("In FixupAstDefiningAndNondefiningDeclarations: declaration            = %p = %s get_name() = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
          mprintf ("                                               definingDeclaration    = %p get_name() = %s \n",definingDeclaration,(definingDeclaration != NULL) ? SageInterface::get_name(definingDeclaration).c_str() : "empty name");
          mprintf ("                                               nondefiningDeclaration = %p get_name() = %s \n",firstNondefiningDeclaration,(firstNondefiningDeclaration != NULL) ? SageInterface::get_name(firstNondefiningDeclaration).c_str() : "empty name");
#endif

       // DQ (10/10/2006): Also set defining declaration of declarations that 
       // have a first_nondefining declaration with a valid defining declaration!
          if (definingDeclaration == NULL)
             {
            // DQ (10/10/2006): This should apply only to SgClassDeclarations (else fails later for cases below).
               SgClassDeclaration* firstNondefiningClassDeclaration = isSgClassDeclaration(firstNondefiningDeclaration);
               if ( (firstNondefiningClassDeclaration != NULL) && (firstNondefiningClassDeclaration->get_definingDeclaration() != NULL) )
                  {
                  // DQ (10/10/2006): This should have already been setup (it could have been a declaration built for a SgClassType and it was missed)!
                  // mprintf ("In FixupAstDefiningAndNondefiningDeclarations: fixup a non-defining class declaration with a NULL pointer, to its defining declaration, indirectly through its valid firstNondefiningDeclaration! \n");
                  // ROSE_ASSERT(false);

                     definingDeclaration = firstNondefiningClassDeclaration->get_definingDeclaration();
                     ROSE_ASSERT(definingDeclaration != NULL);
                     declaration->set_definingDeclaration(definingDeclaration);

                  // mprintf ("In FixupAstDefiningAndNondefiningDeclarations::visit(): declaration = %p firstNondefiningClassDeclaration->get_definingDeclaration() = %p \n",declaration,firstNondefiningClassDeclaration->get_definingDeclaration());
                  }

            // DQ (4/10/2016): Fixing bug pointed out by Tim at UCLA.
            // The issue is with a member function declaration, but could apply to friend functions, so implement fix in terms of SgFunctionDeclaration.
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
               if (functionDeclaration != NULL)
                  {
                    ROSE_ASSERT(firstNondefiningDeclaration != NULL);
                    if (firstNondefiningDeclaration->get_definingDeclaration() != NULL)
                       {
                      // We have identified an inconsistantcy where the defining function pointer has not be uniformally setup.
#if 0
                         mprintf ("We have identified an inconsistantcy where the defining function pointer has not be uniformally setup \n");
#endif
                         declaration->set_definingDeclaration(firstNondefiningDeclaration->get_definingDeclaration());
                       }
                  }

            // DQ (4/10/2016): Output a warning when this happens.
               if (firstNondefiningDeclaration == NULL)
                  {
#if 0
                 // DQ (11/5/2016): Comment this out to avoid output spew in Fortran test codes (see Fortran_tests/mpi_f08_interfaces_test.f03, I think).
                    mprintf ("Warning: declaration exists with firstNondefiningDeclaration == NULL: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
                  }
            // ROSE_ASSERT(firstNondefiningDeclaration != NULL);
            // if (firstNondefiningDeclaration->get_definingDeclaration() != NULL && declaration->get_definingDeclaration() == NULL)
               if (firstNondefiningDeclaration != NULL && firstNondefiningDeclaration->get_definingDeclaration() != NULL && declaration->get_definingDeclaration() == NULL)
                  {
                 // We have identified an inconsistantcy where the defining function pointer has not be uniformally setup.
#if 0
                    mprintf ("Warning: Fixup defining declarations: We have identified an inconsistantcy where the defining declaration has not be uniformally setup \n");
#endif
                    declaration->set_definingDeclaration(firstNondefiningDeclaration->get_definingDeclaration());
                  }
             }

       // DQ (12/14/2005): Test the new flag to tell us when we should have defered the 
       // setting of the scope (to after the scope was built).
          if (definingDeclaration != NULL || firstNondefiningDeclaration != NULL)
             {
               SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(definingDeclaration);
               if (definingClassDeclaration != NULL)
                  {
                    ROSE_ASSERT(definingClassDeclaration->get_fixupScope() == false);
                  }

               SgClassDeclaration* nondefiningClassDeclaration = isSgClassDeclaration(firstNondefiningDeclaration);
               if (nondefiningClassDeclaration != NULL)
                  {
                    if (nondefiningClassDeclaration->get_fixupScope() == true)
                       {
                         mprintf ("Error: nondefiningClassDeclaration = %p = %s = %s \n",
                              nondefiningClassDeclaration,
                              nondefiningClassDeclaration->class_name().c_str(),
                              SageInterface::get_name(nondefiningClassDeclaration).c_str());
                       }
                 // DQ (10/10/2006): added assertion that "nondefiningClassDeclaration->get_scope() != NULL"
                    ROSE_ASSERT(nondefiningClassDeclaration->get_scope() != NULL);

                    if (nondefiningClassDeclaration->get_fixupScope() == true)
                       {
                         mprintf ("Error: nondefiningClassDeclaration = %p = %s get_fixupScope() == false \n",nondefiningClassDeclaration,nondefiningClassDeclaration->get_name().str());
                       }
                    ROSE_ASSERT(nondefiningClassDeclaration->get_fixupScope() == false);
                  }
             }

       // DQ (10/22/2016): It might be that they don't match when using the ast merge mechanism, which is OK.
       // DQ (7/23/2005): The scopes should match!
          if (definingDeclaration != NULL && firstNondefiningDeclaration != NULL)
             {
               ROSE_ASSERT(definingDeclaration != NULL);
               ROSE_ASSERT(firstNondefiningDeclaration != NULL);

               SgScopeStatement* definingScope    =  definingDeclaration->get_scope();
               SgScopeStatement* nondefiningScope =  firstNondefiningDeclaration->get_scope();
               if (definingScope != nondefiningScope)
                  {
                 // This might still leave the scope of a non-defining declaration set incorrectly 
                 // if the definingDeclaration is not available.
                    if ( isSgNamespaceDefinitionStatement(definingScope)    == NULL || 
                         isSgNamespaceDefinitionStatement(nondefiningScope) == NULL )
                       {
                      // DQ (7/29/2005): It is so common for non-defining and defining declarations to be 
                      // declared in the same namespace but different scopes of the same namespace (a namespace 
                      // can have many different scopes) that we don't need to output any special information 
                      // about this case!
#if 0
                         mprintf ("AST Fixup: setting the scope of the nondefining declaration to scope of the defining declaration! \n");
                         mprintf ("definingDeclaration = %s get_scope() = %p = %s name = %s \n",
                              SageInterface::get_name(definingDeclaration).c_str(),
                              definingDeclaration->get_scope(),definingDeclaration->get_scope()->class_name().c_str(),
                              SageInterface::get_name(definingDeclaration->get_scope()).c_str());
                         mprintf ("firstNondefiningDeclaration = %s get_scope() = %p = %s = %s \n",
                              SageInterface::get_name(firstNondefiningDeclaration).c_str(),
                              firstNondefiningDeclaration->get_scope(),firstNondefiningDeclaration->get_scope()->class_name().c_str(),
                              SageInterface::get_name(firstNondefiningDeclaration->get_scope()).c_str());
#endif
                       }
#if 0
                    mprintf ("Resetting scope of firstNondefiningDeclaration = %p = %s old scope = %p = %s new scope = %p = %s \n",
                         firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),
                         firstNondefiningDeclaration->get_scope(),firstNondefiningDeclaration->get_scope()->class_name().c_str(),
                         definingDeclaration->get_scope(),definingDeclaration->get_scope()->class_name().c_str());
#endif
                 // DQ (2/25/2007): Need to fixup the symbol table entries.
                 // firstNondefiningDeclaration->set_scope(definingDeclaration->get_scope());

                    SgSymbol* symbolToMove = firstNondefiningDeclaration->get_symbol_from_symbol_table();

                 // DQ (2/25/2007): Since this is resetting the non-defining declaration it effects the symbol 
                 // generated for some declarations which reserve a non-defining declaration for use in their 
                 // associated symbols. We could fix this by unloading the symbol table from the previous scope
                 // and insert it into the defining scope!  This is important for the get_symbol_from_symbol_table()
                 // since it looks in the saved scope (changing it here casues causes the function to return a NULL
                 // pointer.
                    if (symbolToMove != NULL)
                       {
#if 0
                         mprintf ("Found a symbol to move as we reset the scope of firstNondefiningDeclaration = %p = %s to definingScope = %p \n",
                              firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),definingScope);
                         mprintf ("Removing symbol = %p from scope = %p \n",symbolToMove,nondefiningScope);
#endif
                      // DQ (4/18/2018): Modified to avoid removing the symbol if it is not present.  This might be 
                      // related to a bug fix to support symbols in namespaces and record them in both the namespace 
                      // definition where they are placed plus the global copy of the namespace definition.
                      // nondefiningScope->remove_symbol(symbolToMove);
                         if (nondefiningScope->symbol_exists(symbolToMove) == true)
                            {
                              nondefiningScope->remove_symbol(symbolToMove);
                            }
                           else
                            {
#if 0
                              mprintf ("AST Fixup: this symbol does not exist \n");
#endif
                            }

                      // DQ (2/25/2007): There could be multiple non-defining declarations such that the symbol might 
                      // already exist in the definingScope's symbol table.  (Confirmed to be true).
                      // ROSE_ASSERT (definingScope->symbol_exists(symbolToMove->get_name()) == false);
                      // if (definingScope->symbol_exists(symbolToMove->get_name()) == false)
                      // if (definingScope->symbol_exists(symbolToMove->get_name(),symbolToMove) == false)
                      // if (definingScope->symbol_exists(symbolToMove->get_name()) == false)
                      // if (definingScope->symbol_exists(symbolToMove->get_name()) == false)
                         if (definingScope->symbol_exists(symbolToMove->get_name(),symbolToMove) == false)
                            {
#if 0
                              mprintf ("calling insert symbolToMove = %p = %s = %s into newScope = %p \n",
                                   symbolToMove,symbolToMove->class_name().c_str(),symbolToMove->get_name().str(),definingScope);
#endif
                           // DQ (2/25/2007): It is OK for the name to exist (e.g. overloader functions of a struct 
                           // and a typedef with the same name).
                           // ROSE_ASSERT (definingScope->symbol_exists(symbolToMove->get_name()) == false);
                              ROSE_ASSERT (definingScope->symbol_exists(symbolToMove->get_name(),symbolToMove) == false);
                           // ROSE_ASSERT (definingScope->symbol_exists(symbolToMove) == false);
                              if (definingScope->symbol_exists(symbolToMove) == false)
                                 {
#if 0
                                   mprintf ("After existence tesst: calling insert symbolToMove = %p = %s = %s into newScope = %p = %s \n",
                                        symbolToMove,symbolToMove->class_name().c_str(),symbolToMove->get_name().str(),definingScope,definingScope->class_name().c_str());
#endif
                                   definingScope->insert_symbol(symbolToMove->get_name(),symbolToMove);

                                // DQ (3/5/2007): This will be caught later if we don't catch it now.
                                   ROSE_ASSERT(symbolToMove->get_parent() != NULL);
                                   ROSE_ASSERT(symbolToMove->get_parent() == definingScope->get_symbol_table());
                                   ROSE_ASSERT (definingScope->symbol_exists(symbolToMove) == true);
                                   ROSE_ASSERT(isSgSymbolTable(symbolToMove->get_parent()) != NULL);
                                   ROSE_ASSERT (isSgSymbolTable(symbolToMove->get_parent())->exists(symbolToMove) == true);
                                 }
                                else
                                 {
#if PRINT_DEVELOPER_WARNINGS
                                // Not sure if this is an error or not.
                                   mprintf ("Warning: skipped insert of symbolToMove = %p = %s = %s into newScope = %p (symbolToMove already exists) \n",
                                        symbolToMove,symbolToMove->class_name().c_str(),symbolToMove->get_name().str(),definingScope);
#endif
                                // DQ (3/5/2007): Note that it is OK to have multiple references to a symbol, but
                                // that parent symbol table of any symbol should reference the symbol (at least).

                                // DQ (3/5/2007): however, we should check to make sure that the parent is set properly
                                   ROSE_ASSERT(symbolToMove->get_parent() != NULL);
                                   if (symbolToMove->get_parent() != definingScope->get_symbol_table())
                                      {
#if 0
                                        mprintf ("Resetting the parent of the symbolToMove = %p = %s = %s from %p = %s to %p (symbol table) \n",
                                             symbolToMove,symbolToMove->class_name().c_str(),SageInterface::get_name(symbolToMove).c_str(),
                                             symbolToMove->get_parent(),symbolToMove->get_parent()->class_name().c_str(),definingScope->get_symbol_table());
#endif
                                        symbolToMove->set_parent(definingScope->get_symbol_table());
                                      }
                                   ROSE_ASSERT(symbolToMove->get_parent() == definingScope->get_symbol_table());
                                   ROSE_ASSERT (definingScope->symbol_exists(symbolToMove) == true);
                                   ROSE_ASSERT(isSgSymbolTable(symbolToMove->get_parent()) != NULL);
                                   ROSE_ASSERT (isSgSymbolTable(symbolToMove->get_parent())->exists(symbolToMove) == true);
                                 }
                            }
                       }

                 // DQ (2/25/2007): Now reset the scope to the new scope (the symbol has been transfered if it existed).
#if 0
                    mprintf ("Resetting the scope of firstNondefiningDeclaration = %p = %s from %p = %s to %p = %s \n",
                            firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),
                            firstNondefiningDeclaration->get_scope(),firstNondefiningDeclaration->get_scope()->class_name().c_str(),
                            definingScope,definingScope->class_name().c_str());
#endif

#if DEBUG_SAGE_ACCESS_FUNCTIONS
                 // DQ (6/12/2007): New access function tests using DEBUG_SAGE_ACCESS_FUNCTIONS and 
                 // DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION in sage3.h indicate this is required.
                    if (firstNondefiningDeclaration->get_scope() != NULL)
                       {
                         mprintf ("Note in FixupAstDefiningAndNondefiningDeclarations::visit(): overwriting firstNondefiningDeclaration = %p firstNondefiningDeclaration->get_scope() = %p with NULL before assignment to definingScope = %p \n",
                              firstNondefiningDeclaration,firstNondefiningDeclaration->get_scope(),definingScope);
                         firstNondefiningDeclaration->set_scope(NULL);
                       }
#endif
#if 0
                    mprintf ("Calling set_scope: firstNondefiningDeclaration = %p = %s = %s to definingScope = %p = %s = %s \n",
                            firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),SageInterface::get_name(firstNondefiningDeclaration).c_str(),
                            definingScope,definingScope->class_name().c_str(),SageInterface::get_name(definingScope).c_str());
#endif
                    firstNondefiningDeclaration->set_scope(definingScope);

                 // DQ (3/5/2007): We want the scope obtainted through the parent so that we can test the existance of firstNondefiningDeclaration in the child list
                    SgScopeStatement* firstNondefiningDeclarationScope = isSgScopeStatement(firstNondefiningDeclaration->get_parent());
                    if (firstNondefiningDeclarationScope == NULL)
                       {
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(firstNondefiningDeclaration->get_parent());

                      // Only report this if it is not someting defined in a typedef.
                         if (typedefDeclaration == NULL)
                            {
                              mprintf ("Error: firstNondefiningDeclaration->get_parent() = %p \n",firstNondefiningDeclaration->get_parent());
                              mprintf ("     firstNondefiningDeclaration = %p = %s \n",firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str());
                              if (firstNondefiningDeclaration->get_parent() != NULL)
                                 {
                                   mprintf ("     firstNondefiningDeclaration->get_parent() = %s \n",firstNondefiningDeclaration->get_parent()->class_name().c_str());
                                 }
                              firstNondefiningDeclaration->get_startOfConstruct()->display("firstNondefiningDeclarationScope == NULL: debug");

                           // DQ (3/4/2009): This test fails for the AST copy mechanism on test2005_163.C
                              mprintf ("Warning: failing test: firstNondefiningDeclarationScope != NULL \n");
                              mprintf ("This test fails for the AST copy mechanism on test2005_163.C \n");
                            }
                       }
                 // ROSE_ASSERT(firstNondefiningDeclarationScope != NULL);


                    bool lookForDeclarationInAssociatedScope = true;
                    SgTemplateInstantiationDecl* templateClassInstantiation = isSgTemplateInstantiationDecl(firstNondefiningDeclaration);
                    if (templateClassInstantiation != NULL)
                       {
                         SgClassType* classType = isSgClassType(templateClassInstantiation->get_type());
                         ROSE_ASSERT(classType != NULL);
                         if (classType->get_declaration() == templateClassInstantiation)
                            {
                           // This is a SgTemplateInstantiationDecl that was built to support creation of a type (could be a template parameter).
                           // As a result it may not exist in the child list of its parent which has been set to a scope where it was used.
                           // reset the parent to be the SgClassType.
// #if PRINT_DEVELOPER_WARNINGS
#if 0
                              mprintf ("Resetting the parent of the templateClassInstantiation = %p = %s to its SgClassType \n",
                                   templateClassInstantiation,templateClassInstantiation->get_name().str());
#endif
#if 0
                           // DQ (12/22/2019): Comment out output spew from multiple files on the command line seeing the same class definitions (no longer shared across files).
                              mprintf ("In FixupAstDefiningAndNondefiningDeclarations::visit() templateClassInstantiation->get_parent() = %p \n",templateClassInstantiation->get_parent());
                              if (templateClassInstantiation->get_parent() != NULL)
                                   mprintf ("In FixupAstDefiningAndNondefiningDeclarations::visit() templateClassInstantiation->get_parent() = %s \n",templateClassInstantiation->get_parent()->class_name().c_str());
#endif
                           // templateClassInstantiation->set_parent(classType);
                              if (templateClassInstantiation->get_parent() == NULL)
                                 {
                                   mprintf ("Resetting the parent of the templateClassInstantiation = %p = %s to its SgClassType \n",
                                        templateClassInstantiation,templateClassInstantiation->get_name().str());
                                   templateClassInstantiation->set_parent(classType);
                                 }
                                else
                                 {
#if 0
                                // DQ (12/22/2019): Comment out output spew from multiple files on the command line seeing the same class definitions (no longer shared across files).
                                   mprintf ("templateClassInstantiation parent is already set, skip resetting it to SgClassType \n");
#endif
                                 }

                              lookForDeclarationInAssociatedScope = false;
                            }
                       }
#if 0
                 // DQ (10/22/2016): Added case to eliminate output spew in mergeAST support and testing.
                    SgTemplateClassDeclaration* templateClassDeclaration = isSgTemplateClassDeclaration(firstNondefiningDeclaration);
                    if (templateClassDeclaration != NULL)
                       {
                         lookForDeclarationInAssociatedScope = false;
                       }
#endif
                 // DQ (10/22/2016): Added case to eliminate output spew in mergeAST support and testing.
                    SgNamespaceDefinitionStatement* namespaceDefinitionStatement = isSgNamespaceDefinitionStatement(firstNondefiningDeclarationScope);
                    if (namespaceDefinitionStatement != NULL)
                       {
                      // In the case of a namespace definition the declaration can be in the global namespace 
                      // definition instead and this is especially an issue for the AST merge mechanism.
                         lookForDeclarationInAssociatedScope = false;
                       }

                 // DQ (10/22/2016): Added case to eliminate output spew in mergeAST support and testing.
                    SgGlobal* globalScope = isSgGlobal(firstNondefiningDeclarationScope);
                    if (globalScope != NULL)
                       {
                      // This is an issue for the ASTmerge (suppressing output spew from mergeAST_tests directory).
                         lookForDeclarationInAssociatedScope = false;
                       }

                 // DQ (3/4/2009): Modified this test now that firstNondefiningDeclarationScope can maybe have a valid NULL value.
                 // if (lookForDeclarationInAssociatedScope == true)
                    if (lookForDeclarationInAssociatedScope == true && firstNondefiningDeclarationScope != NULL)
                       {
                      // DQ (3/5/2007): This is a relatively expensive tests since the SgStatementPtrList must be generated directly and can not be referenced.
                         const SgStatementPtrList & statementList = firstNondefiningDeclarationScope->generateStatementList();
                      // Make sure that the statement is in the list...
                         if (std::find(statementList.begin(),statementList.end(),firstNondefiningDeclaration) == statementList.end())
                            {
                              mprintf ("##### WARNING: in FixupAstDefiningAndNondefiningDeclarations::visit() statement = %p = %s not in child list of scope = %p = %s \n",
                                   firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),
                                   firstNondefiningDeclarationScope,firstNondefiningDeclarationScope->class_name().c_str());
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(firstNondefiningDeclaration);
                              if (classDeclaration != NULL)
                                 {
                                   mprintf (" --- classDeclaration = %p name = %s \n",classDeclaration,classDeclaration->get_name().str());
                                 }
#if 0
                              firstNondefiningDeclaration->get_startOfConstruct()->display("declaration: firstNondefiningDeclaration: debug");
                              firstNondefiningDeclarationScope->get_startOfConstruct()->display("scope: firstNondefiningDeclarationScope: debug");
#endif
                            }
                       }
                 // ROSE_ASSERT(find(statementList.begin(),statementList.end(),firstNondefiningDeclaration) != statementList.end());
                  }
#if 0
               mprintf ("Testing associated scope of declaration statment (node = %p = %s = %s) \n",
                    declaration,declaration->sage_class_name(),SageInterface::get_name(declaration).c_str());
               mprintf ("definingDeclaration->get_scope() = %p firstNondefiningDeclaration->get_scope() = %p \n",
                    definingDeclaration->get_scope(),firstNondefiningDeclaration->get_scope());
#endif
               ROSE_ASSERT(definingDeclaration->get_scope() == firstNondefiningDeclaration->get_scope());
             }
        }

     switch (node->variantT())
        {
          case V_SgEnumDeclaration:
             {
            // This should be set in the EDG/Sage III translation!

            // DQ (6/26/2005): Special case of enum declarations (no forward enum declarations are allowed in the 
            // C or C++ standard), support added for them because they are a common extension (except in gnu).
            // DQ (4/22/2007): However this is a common compiler extension 
            // for nearly all C compilers, except GNU, so we have tried to support it.
            // ROSE_ASSERT(declaration == definingDeclaration);
            // mprintf ("In FixupAstDefiningAndNondefiningDeclarations::visit(): declaration = %p definingDeclaration = %p \n",declaration,definingDeclaration);
               if (declaration != definingDeclaration)
                  {
                 // mprintf ("Note: in FixupAstDefiningAndNondefiningDeclarations::visit(), enum declaration not a defining declaration \n");
                 // declaration->get_startOfConstruct()->display("declaration != definingDeclaration for enum declaration");
                  }
               break;
             }

       // DQ (6/26/2005): These are likely somewhat special and such that they should be their own defining declarations, I think

       // This is a not well tested declaration within Sage III (but I think that any declaration must be a defining declaration)
          case V_SgAsmStmt:

       // These are special case declarations
          case V_SgFunctionParameterList:
          case V_SgCtorInitializerList:

       // A variable definition appears with a variable declaration, but a variable declaration can be a 
       // forward reference to the variable declaration containing the variable definitions (e.g. "extern int x;", 
       // is a forward declaration to the declaration of "x").
          case V_SgVariableDefinition:

       // A pragam can contain no references to it and so it's declaration is also it's definition
          case V_SgPragmaDeclaration:

       // These can appear multiple times and are not really associated with definitions 
       // (but for consistancy they are consired to be their own defining declaration).
          case V_SgUsingDirectiveStatement:
          case V_SgUsingDeclarationStatement:
          case V_SgNamespaceAliasDeclarationStatement:
          case V_SgTemplateInstantiationDirectiveStatement:

       // Shared by all the above cases!
             {
            // DQ (6/26/2005): I think that the C++ standard does not allow forward declarations for these either!
            // So the defining declaration should be the declaration itself (I think). Either that or we need to 
            // build a special non-defining declaration for these declarations.

#if 0
               mprintf ("In AST Fixup: declaration %p = %s definingDeclaration = %p \n",declaration,declaration->class_name().c_str(),definingDeclaration);
#endif
               if (declaration != definingDeclaration)
                  {
#if 0
                    mprintf ("Warning in AST Fixup: declaration %p = %s used to set definingDeclaration = %p \n",
                         declaration,declaration->class_name().c_str(),definingDeclaration);
#endif
                 // Make sure it was never previously set
                    ROSE_ASSERT(definingDeclaration == NULL);

                 // fix the problem here!
                 // mprintf ("In FixupAstDefiningAndNondefiningDeclarations: set_definingDeclaration of %p to %p \n",declaration,declaration);
                    declaration->set_definingDeclaration(declaration);

                 // reset the definingDeclaration
                    definingDeclaration = declaration->get_definingDeclaration();
                  }
               ROSE_ASSERT(declaration == definingDeclaration);

            // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
            // SgScopeStatement* declarationScope = NULL;

               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declaration);
               if (memberFunctionDeclaration != NULL)
                  {
                 // DQ (10/12/2007): This uses the previous semantics, this now always returns a valid pointer independent of if the class is defined.
                 // It also has the consition set backwards.
#if 0
                    if (memberFunctionDeclaration->get_associatedClassDeclaration() != NULL)
                       {
                         declarationScope = NULL;
                       }
                      else
                       {
                      // declarationScope = NULL;
                      // declarationScope = memberFunctionDeclaration->set_associatedClassDeclaration(classDeclaration)->findDefinition();
                         ROSE_ASSERT(definingDeclaration != NULL);
                         ROSE_ASSERT(memberFunctionDeclaration->get_associatedClassDeclaration() != NULL);
                         declarationScope = memberFunctionDeclaration->get_associatedClassDeclaration()->get_definition();
                         ROSE_ASSERT(declarationScope != NULL);
                       }
#else
                 // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
                 // DQ (10/12/2007): This should be a better implementation! Will be NULL if the class definition does not exist.
                 // declarationScope = memberFunctionDeclaration->get_class_scope();
#endif
                  }
                 else
                  {
                 // mprintf ("Calling declaration->get_startOfConstruct()->display() declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                 // declaration->get_startOfConstruct()->display("declaration location: debug");
                 // declarationScope = declaration->get_scope();
#if 0
                 // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
                    if (declaration->hasExplicitScope() == true)
                       {
                         declarationScope = declaration->get_scope();
                       }
#endif
                  }
#if 0
               mprintf ("declaration = %p definingDeclaration = %p \n",declaration,definingDeclaration);
#endif

            // DQ (9/12/2011): Static analysis wants an assertion here.
               ROSE_ASSERT(definingDeclaration != NULL);

            // if (declaration->get_scope() != definingDeclaration->get_scope())
               if (declaration->hasExplicitScope() == true && declaration->get_scope() != definingDeclaration->get_scope())
                  {
                    mprintf ("declaration         = %p = %s \n",declaration,SageInterface::get_name(declaration).c_str());
                    mprintf ("definingDeclaration = %p = %s \n",definingDeclaration,SageInterface::get_name(definingDeclaration).c_str());
                    mprintf ("Error: declaration->get_scope() = %p definingDeclaration->get_scope() = %p \n",declaration->get_scope(),definingDeclaration->get_scope());
                    if (declaration->get_scope() != NULL)
                         mprintf ("declaration->get_scope() = %s \n",declaration->get_scope()->class_name().c_str());
                    if (definingDeclaration->get_scope() != NULL)
                         mprintf ("definingDeclaration->get_scope() = %s \n",definingDeclaration->get_scope()->class_name().c_str());
                  }
               ROSE_ASSERT(declaration->hasExplicitScope() == false || declaration->get_scope() == definingDeclaration->get_scope());

               if (firstNondefiningDeclaration != NULL)
                  {
#if 0
                    mprintf ("declaration->get_scope() = %s = %p (parent = %p) firstNondefiningDeclaration->get_scope() = %p \n",
                         declaration->class_name().c_str(),declaration->get_scope(),declaration->get_parent(),firstNondefiningDeclaration->get_scope());
                    mprintf ("declaration->get_parent() = %p = %s \n",declaration->get_parent(),declaration->get_parent()->class_name().c_str());
#endif
#if 0
                    declaration->get_file_info()->display("declaration");
                    declaration->get_scope()->get_file_info()->display("declaration (scope)");
                    firstNondefiningDeclaration->get_scope()->get_file_info()->display("firstNondefiningDeclaration (scope)");
#endif
                 // DQ (2/12/2006): Set the scope to match the firstNondefiningDeclaration
                 // required for test2006_08.C to work: friend declarations should have matching scopes.
                 // DQ (2/16/2006): Let's only call set_scope if there is an explicit scope to set!
                 // So I added a virtual hasExplicitScope() member function so that we can know which 
                 // IR nodes have an explicit scope data member.
                    if (declaration->hasExplicitScope() == true)
                       {
#if 1
                         mprintf ("Resetting scope (declaration->hasExplicitScope() == true) of declaration = %p = %s old scope = %p = %s new scope = %p = %s \n",
                              declaration,declaration->class_name().c_str(),
                              declaration->get_scope(),declaration->get_scope()->class_name().c_str(),
                              firstNondefiningDeclaration->get_scope(),firstNondefiningDeclaration->get_scope()->class_name().c_str());
#endif
                         declaration->set_scope(firstNondefiningDeclaration->get_scope());
                       }
#if 0
                    mprintf ("declaration->get_scope() = %p firstNondefiningDeclaration->get_scope() = %p \n",
                         declaration->get_scope(),firstNondefiningDeclaration->get_scope());
#endif
                 // Note that the get_scope() funcion does not return a consistant scope
                 // for a defining and non-defining SgFunctionParameterList.
                 // mprintf ("WARNING: Fundamental problem in get_scope function! \n");
                 // ROSE_ASSERT(declaration->get_scope() == firstNondefiningDeclaration->get_scope());
                    ROSE_ASSERT(declaration->hasExplicitScope() == false || declaration->get_scope() == firstNondefiningDeclaration->get_scope());
                 // firstNondefiningDeclaration->set_scope();
                  }
                  else
                  { //Liao,10/31/2008
                     //dump some debugging information before assertion 
                    declaration->get_file_info()->display("fixupDefiningAndNondefiningDeclarations.C assertion:");

                    ROSE_ASSERT(firstNondefiningDeclaration != NULL);
                  }
               break;
             }

       // DQ (2/19/2006): This case was moved from being the collection of cases above
       // scopes cannot be compared so easily.
       // Namespaces can't appear without their definitions (or so it seems, tested)
          case V_SgNamespaceDeclarationStatement:
             {
            // This case is special, since there can be many declarations of the same namespace and 
            // each one can include a definition, however we don't consider each be be THE defining 
            // declaration so we hold the pointer to the one of them (the first one) and return it 
            // using the get_firstNondefiningDeclaration() member function).  The scope of namespace
            // is not held explicitly since forward declarations of namespaces are not possible and so
            // the concept of scope can be computed (structurally via the parent pointer).
            // But the scope of any declaration which has multiple associated declaration (forward references)
            // can be different SgNamespaceDefinitionStatement (scopes).  This make testing a bit more
            // complex since the concept of the same scope is no longer matching pointers to scopes if the
            // the scopes are SgNamespaceDefinitionStatement IR nodes (in this way C++ is a bit more complex
            // than C and other languages).
               ROSE_ASSERT(definingDeclaration == NULL);
               ROSE_ASSERT(firstNondefiningDeclaration != NULL);
               break;
             }

       // This case is a bit special
          case V_SgVariableDeclaration:

       // These can have forward declarations separated from their definitions
       // so a declaration may be either a defining or non-defining declaration.
          case V_SgTemplateDeclaration:
          case V_SgFunctionDeclaration:

       // DQ (12/5/2010): Added Fortran specific function like IR nodes.
          case V_SgEntryStatement:
          case V_SgProcedureHeaderStatement:
          case V_SgProgramHeaderStatement:

          case V_SgClassDeclaration:
          case V_SgTypedefDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
            // At the very least for any declaration, the firstNondefiningDeclaration should be a vailid pointer
            // it may or may not point to the current declaration (since multiple declarations may be non-defining).
               if (firstNondefiningDeclaration != NULL)
                  {
                 // It is OK for the firstNondefiningDeclaration to be NULL, it means that there was no previous forward declaration.
                 // mprintf ("Warning: declaration %p = %s has no firstNondefiningDeclaration = %p \n",
                 //      declaration,declaration->sage_class_name(),firstNondefiningDeclaration);
                  }
            // ROSE_ASSERT(firstNondefiningDeclaration != NULL);

            // The firstNondefiningDeclaration should be available for use by any IR node requiring
            // a reference to the declaration (which is why it is stored explicitly).  Thus the 
            // firstNondefiningDeclaration should never be the same as the definingDeclaration (if they are non-null)
               if (firstNondefiningDeclaration != NULL && firstNondefiningDeclaration == definingDeclaration)
                  {
#if 0
                    mprintf ("AST Fixup: declaration = %p = %s firstNondefiningDeclaration == definingDeclaration at: \n",
                         declaration,declaration->sage_class_name());
                    declaration->get_file_info()->display("firstNondefiningDeclaration == definingDeclaration");
#endif
                 // reset this to NULL since it is not a non-defining declaration (we might later want to build a
                 // non-defining declaration to have be referenced here, but it is not clear that that is required).
                    declaration->set_firstNondefiningDeclaration(NULL);

                 // reset the firstNondefiningDeclaration
                    firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
                  }
               ROSE_ASSERT(firstNondefiningDeclaration != definingDeclaration);

            // DQ (8/18/2005): New test added
               SgTemplateInstantiationMemberFunctionDecl* templateMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(declaration);
               if (templateMemberFunction != NULL)
                  {
                 // Look for the SgTempleteDeclaration (some of them can been hidden and are not traversed)
                    SgDeclarationStatement* templateDeclaration = templateMemberFunction->get_templateDeclaration();

                 // DQ (5/3/2012): We don't always have a template declaration, so I would like to weaken this test in ROSE.  We might be able to fix it later.
                 // ROSE_ASSERT(templateDeclaration != NULL);
                    if (templateDeclaration != NULL)
                       {
                 // ROSE_ASSERT(templateDeclaration->get_firstNondefiningDeclaration() != NULL);
#if 0
                 // DQ (3/4/2007): This can cause the defining and non-defining declarations to be the same, which is an violation of AST consistancy rules.
                    if (templateDeclaration->get_definingDeclaration() == NULL)
                       {
                         mprintf ("Setting previously unset definingDeclaration on SgTemplateDeclaration to itself  templateDeclaration = %p \n",templateDeclaration);
                         templateDeclaration->set_definingDeclaration(templateDeclaration);
                       }
                    ROSE_ASSERT(templateDeclaration->get_definingDeclaration() != NULL);
#else
                 // DQ (3/4/2007): Changed this test to avoid resetting the defiing declaration, it is OK for the defining 
                 // template declaration to not be present (since it can be nested in another outer template class declaration).
                    if (templateDeclaration->get_definingDeclaration() == NULL && templateDeclaration->get_firstNondefiningDeclaration() == NULL)
                       {
                         mprintf ("Error: there should be at least a defining or non-defining template declaration available templateDeclaration = %p \n",templateDeclaration);
                       }
                    ROSE_ASSERT(templateDeclaration->get_definingDeclaration() != NULL || templateDeclaration->get_firstNondefiningDeclaration() != NULL);

                 // DQ (3/4/2007): If this is true the AST Consistancy tests will catch this later!
                 // So do the tst here as close as possible to where the definingDeclaration is set.
                    ROSE_ASSERT(templateDeclaration->get_definingDeclaration() != templateDeclaration->get_firstNondefiningDeclaration());
#endif

                    ROSE_ASSERT(templateDeclaration->get_parent() != NULL);
                       }
                      else
                       {
                      // DQ (5/3/2012): Make it a warning to detect templateDeclaration == NULL.
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if 0
                         mprintf ("WARNING: In FixupAstDefiningAndNondefiningDeclarations: templateDeclaration == NULL (this is a recent EDG 4.3 issue). \n");
#endif
                       }
                  }

            // DQ (9/24/2007): This is an error reported by Andreas, fix it after Gergos checkin.
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(node);
               if ( (memberFunctionDeclaration != NULL) && (memberFunctionDeclaration->get_associatedClassDeclaration() == NULL) )
                  {
#if 0
                    mprintf ("Error: memberFunctionDeclaration->get_associatedClassDeclaration() == NULL for memberFunctionDeclaration = %p = %s \n",
                         memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str());
#endif
                 // mprintf ("Error: memberFunctionDeclaration->get_associatedClassDeclaration() == NULL for memberFunctionDeclaration = %p = %s = %s \n",
                 //      memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),SageInterface::get_name(memberFunctionDeclaration).c_str());
                 // ROSE_ASSERT(false);
                  }

               break;
             }
           
          default:
             {
            // Nothing to do here!
             }
        }



#if 0
     if (declaration != NULL)
        {
          mprintf ("Leaving FixupAstDefiningAndNondefiningDeclarations: node = %p = %s definingDeclaration = %p firstNondefiningDeclaration = %p \n",
               node,node->class_name().c_str(),definingDeclaration,firstNondefiningDeclaration);

#if 0
       // DQ (3/4/2007): temp debugging code to check where defining and non-defining declarations were being set equal!
          extern SgDeclarationStatement* saved_declaration;
          if (declaration == (SgDeclarationStatement*)(0x2a96f96480))
             {
               mprintf ("Setting the saved_declaration to %p \n",declaration);
               saved_declaration = declaration;
             }
#endif
        }
#endif
   }


