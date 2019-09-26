// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "fixupCopy.h"

// This file implementes support for the AST copy fixup.  It is specific to:
// 1) variable reference expressions (SgVarRefExp)
// 2) function reference expressions (SgFunctionRefExp)
// 3) member function reference expressions (SgMemberFunctionRefExp)

void
SgInitializedName::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgInitializedName::fixupCopy_references() %p = %s \n",this,this->get_name().str());
#endif
   }


void
SgStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgLocatedNode::fixupCopy_references(copy,help);
   }

void
SgExpression::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgExpression::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     Rose_STL_Container<SgNode*> children_original = const_cast<SgExpression*>(this)->get_traversalSuccessorContainer();
     Rose_STL_Container<SgNode*> children_copy     = const_cast<SgNode*>(copy)->get_traversalSuccessorContainer();
     ROSE_ASSERT (children_original.size() == children_copy.size());

     for (Rose_STL_Container<SgNode*>::const_iterator
            i_original = children_original.begin(),
            i_copy = children_copy.begin();
          i_original != children_original.end(); ++i_original, ++i_copy) {
       if (*i_original == NULL) continue;
       (*i_original)->fixupCopy_references(*i_copy,help);
     }

     SgLocatedNode::fixupCopy_references(copy,help);
   }


void
SgLocatedNode::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgLocatedNode::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // printf ("Inside of SgLocatedNode::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);

#ifndef CXX_IS_ROSE_CODE_GENERATION
  // Fixup references in SgStatements and SgExpressions
  // Define a traversal to update the references to symbols (per statement)
     class Traversal : public AstSimpleProcessing
        {
          private:
               SgCopyHelp & helpSupport;

          public:
               Traversal (SgCopyHelp & help) : helpSupport(help) {}

               void visit(SgNode* n)
                  {
                    switch(n->variantT())
                       {
                         case V_SgVarRefExp:
                            {
                              SgVarRefExp* varRefExp = isSgVarRefExp(n);
                              ROSE_ASSERT(varRefExp != NULL);
                              SgVariableSymbol* variableSymbol_original = varRefExp->get_symbol();
                              ROSE_ASSERT(variableSymbol_original != NULL);
                              SgInitializedName* initializedName_original = variableSymbol_original->get_declaration();
                              if (initializedName_original != NULL) { // Try to do this first, because the symbol may not have been copied
                             // ROSE_ASSERT(initializedName_original->get_symbol_from_symbol_table());
                                SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(initializedName_original);
                                if (i != helpSupport.get_copiedNodeMap().end()) {
                                  SgInitializedName* initializedName_copy = isSgInitializedName(i->second);
                                  ROSE_ASSERT(initializedName_copy != NULL);
                                  SgVariableSymbol* symbol_copy = isSgVariableSymbol(initializedName_copy->get_symbol_from_symbol_table());
                                  if (symbol_copy) {
                                 // printf ("Inside of SgStatement::fixupCopy_references(): symbol_copy = %p \n",symbol_copy);
                                    varRefExp->set_symbol(symbol_copy);
                                  }
                                }
                              } else { // This is used for cases such as __PRETTY_FUNCTION__ whose symbols are not in a symbol table
                                SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(variableSymbol_original);
                                if (i != helpSupport.get_copiedNodeMap().end()) {
                                  SgVariableSymbol* symbol_copy = isSgVariableSymbol(i->second);
                                  if (symbol_copy) {
                                 // printf ("Inside of SgStatement::fixupCopy_references(): symbol_copy = %p \n",symbol_copy);
                                    varRefExp->set_symbol(symbol_copy);
                                  }
                                }
                              }

                           // printf ("Inside of SgStatement::fixupCopy_references(): i != helpSupport.get_copiedNodeMap().end() = %s \n",
                           //      (i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                              break;
                            }

                         case V_SgFunctionRefExp:
                            {
                              SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(n);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgFunctionSymbol* functionSymbol_original = functionRefExp->get_symbol();
                              ROSE_ASSERT(functionSymbol_original != NULL);
                              SgFunctionDeclaration* functionDeclaration_original = functionSymbol_original->get_declaration();
                              ROSE_ASSERT(functionDeclaration_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(functionDeclaration_original);

                           // printf ("Inside of SgStatement::fixupCopy_references(): (case SgFunctionRefExp) i != helpSupport.get_copiedNodeMap().end() = %s \n",
                           //      (i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgFunctionDeclaration* functionDeclaration_copy = isSgFunctionDeclaration(i->second);
                                   ROSE_ASSERT(functionDeclaration_copy != NULL);
                                   SgSymbol* symbol_copy = functionDeclaration_copy->get_symbol_from_symbol_table();
                                // printf ("Inside of SgStatement::fixupCopy_references(): symbol_copy = %p \n",symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgFunctionSymbol* functionSymbol_copy = isSgFunctionSymbol(symbol_copy);
                                        ROSE_ASSERT(functionSymbol_copy != NULL);
                                        functionRefExp->set_symbol(functionSymbol_copy);
                                      }
                                     else
                                      {
                                        if (SgProject::get_verbose() > 0)
                                             printf ("Error: could not find symbol associated with functionDeclaration_copy = %p = %s = %s \n",
                                                  functionDeclaration_copy,functionDeclaration_copy->class_name().c_str(),SageInterface::get_name(functionDeclaration_copy).c_str());
                                     // ROSE_ASSERT(false);
                                      }
                                 }
                              break;
                            }

                         case V_SgMemberFunctionRefExp:
                            {
                              SgMemberFunctionRefExp* functionRefExp = isSgMemberFunctionRefExp(n);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgMemberFunctionSymbol* functionSymbol_original = functionRefExp->get_symbol();
                              ROSE_ASSERT(functionSymbol_original != NULL);
                              SgMemberFunctionDeclaration* functionDeclaration_original = functionSymbol_original->get_declaration();
                              ROSE_ASSERT(functionDeclaration_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(functionDeclaration_original);
#if 0
                              printf ("Inside of SgStatement::fixupCopy_references(): (case SgMemberFunctionRefExp) functionSymbol_original = %p i != helpSupport.get_copiedNodeMap().end() = %s \n",
                                   functionSymbol_original,(i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");
#endif
                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgMemberFunctionDeclaration* functionDeclaration_copy = isSgMemberFunctionDeclaration(i->second);
                                   ROSE_ASSERT(functionDeclaration_copy != NULL);
                                   SgSymbol* symbol_copy = functionDeclaration_copy->get_symbol_from_symbol_table();
                                // printf ("Inside of SgStatement::fixupCopy_references(): symbol_copy = %p \n",symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgMemberFunctionSymbol* functionSymbol_copy = isSgMemberFunctionSymbol(symbol_copy);
                                        ROSE_ASSERT(functionSymbol_copy != NULL);
                                        functionRefExp->set_symbol(functionSymbol_copy);
                                      }
                                     else
                                      {
                                        if (SgProject::get_verbose() > 0)
                                             printf ("Error: could not find symbol associated with (member) functionDeclaration_copy = %p = %s = %s \n",
                                                  functionDeclaration_copy,functionDeclaration_copy->class_name().c_str(),SageInterface::get_name(functionDeclaration_copy).c_str());
                                     // ROSE_ASSERT(false);
                                      }
                                 }
                              break;
                            }

                      // DQ (10/16/2007): Note that labels are handled by the SgLabelStatement and the SgGotoStatement 
                      // directly and are not required to be processed here in the SgStatement.
                         case V_SgLabelStatement:
                            {
                           // printf ("Inside of SgStatement::fixupCopy_references(): we might have to handle SgLabelStatement \n");
                              break;
                            }

                          default:
                            {
                           // Nothing to do for this case
                            }
                       }
                  }
        };

  // Build an run the traversal defined above.
     Traversal t(help);
     t.traverse(copy,preorder);

     SgLocatedNode* copyLocatedNode = isSgLocatedNode(copy);
     ROSE_ASSERT(copyLocatedNode != NULL);

  // DQ (10/24/2007): New test.
     ROSE_ASSERT(copyLocatedNode->variantT() == this->variantT());
#endif

#if DEBUG_FIXUP_COPY
     printf ("Leaving SgLocatedNode::fixupCopy_references() \n\n");
#endif
   }


void
SgScopeStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgScopeStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement* copyScopeStatement = isSgScopeStatement(copy);
     ROSE_ASSERT(copyScopeStatement != NULL);

  // Call the base class fixupCopy member function
     SgStatement::fixupCopy_references(copy,help);
   }


void
SgGlobal::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgGlobal::fixupCopy_references() for %p copy = %p \n",this,copy);
#endif

     SgGlobal* global_copy = isSgGlobal(copy);
     ROSE_ASSERT(global_copy != NULL);

     const SgDeclarationStatementPtrList & statementList_original = this->getDeclarationList();
     const SgDeclarationStatementPtrList & statementList_copy     = global_copy->getDeclarationList();

     SgDeclarationStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgDeclarationStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
          (*i_original)->fixupCopy_references(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_references(copy,help);

  // printf ("\nLeaving SgGlobal::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

// JJW 2/1/2008 -- Added fixup to allow statement expressions to be handled
void
SgExprStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgExprStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgExprStatement* es_copy = isSgExprStatement(copy);
     ROSE_ASSERT(es_copy != NULL);

     SgExpression* expression_original = this->get_expression();
     SgExpression* expression_copy =     es_copy->get_expression();

     expression_original->fixupCopy_references(expression_copy, help);

  // Call the base class fixupCopy member function
     SgStatement::fixupCopy_references(copy,help);

  // printf ("\nLeaving SgBasicBlock::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

// DQ (10/6/2007): Added fixup function to set scopes not set properly by the ROSETTA generated copy!
void
SgBasicBlock::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgBasicBlock::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgBasicBlock* block_copy = isSgBasicBlock(copy);
     ROSE_ASSERT(block_copy != NULL);

     const SgStatementPtrList & statementList_original = this->getStatementList();
     const SgStatementPtrList & statementList_copy     = block_copy->getStatementList();

  // Check that this need not be handled as a special case such as SgIfStmt.
     if (this->containsOnlyDeclarations() == true)
        {
          ROSE_ASSERT(this->getDeclarationList().size() == statementList_original.size());
        }
       else
        {
          ROSE_ASSERT(this->getStatementList().size() == statementList_original.size());
        }

     SgStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
          (*i_original)->fixupCopy_references(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_references(copy,help);

  // printf ("\nLeaving SgBasicBlock::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

void
SgDeclarationStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDeclarationStatement::fixupCopy_references() for %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

  // Call the base class fixupCopy member function (this will setup the parent)
     SgStatement::fixupCopy_references(copy,help);
   }


void
SgFunctionDeclaration::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgFunctionDeclaration::fixupCopy_references(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgFunctionDeclaration* functionDeclaration_copy = isSgFunctionDeclaration(copy);
     ROSE_ASSERT(functionDeclaration_copy != NULL);

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_references(copy,help);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_parameterList() != NULL);
     get_parameterList()->fixupCopy_references(functionDeclaration_copy->get_parameterList(),help);

  // Setup the details in the SgFunctionDefinition (this may have to rebuild the sysmbol table)
  // printf ("In SgFunctionDeclaration::fixupCopy_references(): this->get_definition() = %p \n",this->get_definition());
     if (this->get_definition() != NULL)
        {
          ROSE_ASSERT(isForward() == false);

       // DQ (2/26/2009): Handle special cases where the copyHelp function is non-trivial.
       // Is every version of copyHelp object going to be a problem?

       // For the outlining, our copyHelp object does not copy defining function declarations 
       // and substitutes a non-defining declarations, so if the copy has been built this way 
       // then skip trying to reset the SgFunctionDefinition.
       // printf ("In SgFunctionDeclaration::fixupCopy_references(): functionDeclaration_copy->get_definition() = %p \n",functionDeclaration_copy->get_definition());
       // this->get_definition()->fixupCopy_references(functionDeclaration_copy->get_definition(),help);
          if (functionDeclaration_copy->get_definition() != NULL)
             {
               this->get_definition()->fixupCopy_references(functionDeclaration_copy->get_definition(),help);
             }

       // If this is a declaration with a definition then it is a defining declaration
       // functionDeclaration_copy->set_definingDeclaration(functionDeclaration_copy);
        }

  // printf ("\nLeaving SgFunctionDeclaration::fixupCopy_references(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
   }

// DQ (10/6/2007): Added fixup function to set scopes not set properly by the ROSETTA generated copy!
void
SgFunctionParameterList::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFunctionParameterList::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy_references(copy,help);

     SgFunctionParameterList* copyFunctionParameterList = isSgFunctionParameterList(copy);
     ROSE_ASSERT(copyFunctionParameterList != NULL);

     const SgInitializedNamePtrList & parameterList_original = this->get_args();
     SgInitializedNamePtrList & parameterList_copy           = copyFunctionParameterList->get_args();

     SgInitializedNamePtrList::const_iterator i_original = parameterList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = parameterList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != parameterList_original.end()) && (i_copy != parameterList_copy.end()) )
        {
          (*i_original)->fixupCopy_references(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }


void
SgMemberFunctionDeclaration::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("In SgMemberFunctionDeclaration::fixupCopy_references(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgMemberFunctionDeclaration* memberFunctionDeclaration_copy = isSgMemberFunctionDeclaration(copy);
     ROSE_ASSERT(memberFunctionDeclaration_copy != NULL);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_CtorInitializerList() != NULL);
     get_CtorInitializerList()->fixupCopy_references(memberFunctionDeclaration_copy->get_CtorInitializerList(),help);

  // Call the base class fixupCopy member function
     SgFunctionDeclaration::fixupCopy_references(copy,help);
   }


void
SgTemplateDeclaration::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nInside of SgTemplateDeclaration::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy_references(copy,help);
   }


void
SgTemplateInstantiationDefn::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTemplateInstantiationDefn::fixupCopy_references() for class = %s class definition %p = %s copy = %p \n",this->get_declaration()->get_name().str(),this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgClassDefinition::fixupCopy_references(copy,help);
   }


void
SgTemplateInstantiationDecl::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationDecl::fixupCopy_references(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgTemplateInstantiationDecl* templateClassDeclaration_copy = isSgTemplateInstantiationDecl(copy);
     ROSE_ASSERT(templateClassDeclaration_copy != NULL);

  // Also call the base class version of the fixupCopycopy() member function
     SgClassDeclaration::fixupCopy_references(copy,help);

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateClassDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateClassDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy_references(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        }
#endif
   }

void
SgTemplateInstantiationMemberFunctionDecl::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationMemberFunctionDecl::fixupCopy_references(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgTemplateInstantiationMemberFunctionDecl* templateMemberFunctionDeclaration_copy = isSgTemplateInstantiationMemberFunctionDecl(copy);
     ROSE_ASSERT(templateMemberFunctionDeclaration_copy != NULL);

  // Also call the base class version of the fixupCopycopy() member function
     SgMemberFunctionDeclaration::fixupCopy_references(copy,help);

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateMemberFunctionDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateMemberFunctionDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy_references(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        }
#endif
   }


void
SgTemplateInstantiationFunctionDecl::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationFunctionDecl::fixupCopy_references(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration_copy = isSgTemplateInstantiationFunctionDecl(copy);
     ROSE_ASSERT(templateFunctionDeclaration_copy != NULL);

  // Also call the base class version of the fixupCopycopy() member function
     SgFunctionDeclaration::fixupCopy_references(copy,help);

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateFunctionDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateFunctionDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy_references(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        } 
#endif
   }


void
SgFunctionDefinition::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFunctionDefinition::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgFunctionDefinition* functionDefinition_copy = isSgFunctionDefinition(copy);
     ROSE_ASSERT(functionDefinition_copy != NULL);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_body() != NULL);
     get_body()->fixupCopy_references(functionDefinition_copy->get_body(),help);

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_references(copy,help);

  // printf ("\nLeaving SgFunctionDefinition::fixupCopy_references() for %p = %s copy = %p \n\n",this,this->class_name().c_str(),copy);
   }

void
SgVariableDeclaration::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgVariableDeclaration::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy_references(copy,help);

     SgVariableDeclaration* variableDeclaration_copy = isSgVariableDeclaration(copy);
     ROSE_ASSERT(variableDeclaration_copy != NULL);

  // DQ (10/14/2007): Handle the case of a type defined in the base type of the typedef (similar problem for SgVariableDeclaration).
  // printf ("this = %p this->get_variableDeclarationContainsBaseTypeDefiningDeclaration() = %s \n",this,this->get_variableDeclarationContainsBaseTypeDefiningDeclaration() ? "true" : "false");
     if (this->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true)
        {
          ROSE_ASSERT(variableDeclaration_copy->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true);
          SgDeclarationStatement* baseTypeDeclaration_original = this->get_baseTypeDefiningDeclaration();
          SgDeclarationStatement* baseTypeDeclaration_copy     = variableDeclaration_copy->get_baseTypeDefiningDeclaration();

          ROSE_ASSERT(baseTypeDeclaration_original != NULL);
          ROSE_ASSERT(baseTypeDeclaration_copy != NULL);

       // printf ("In SgVariableDeclaration::fixupCopy_references(): Calling fixupCopy on %p = %s \n",baseTypeDeclaration_original,baseTypeDeclaration_original->class_name().c_str());

          baseTypeDeclaration_original->fixupCopy_references(baseTypeDeclaration_copy,help);
        }

     const SgInitializedNamePtrList & variableList_original = this->get_variables();
     SgInitializedNamePtrList & variableList_copy           = variableDeclaration_copy->get_variables();

  // printf ("Inside of SgVariableDeclaration::fixupCopy_references(): variableList_original.size() = %ld \n",(long)variableList_original.size());

     ROSE_ASSERT(variableList_original.size() == variableList_copy.size());

     SgInitializedNamePtrList::const_iterator i_original = variableList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = variableList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != variableList_original.end()) && (i_copy != variableList_copy.end()) )
        {
       // printf ("Looping over the initialized names in the variable declaration variable = %p = %s \n",(*i_copy),(*i_copy)->get_name().str());

          (*i_original)->fixupCopy_references(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }

void
SgClassDeclaration::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgClassDeclaration::fixupCopy_references() for class = %s = %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this->get_name().str(),this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_references(copy,help);

     if (isForward() == false)
        {
          SgClassDeclaration* classDeclaration_copy = isSgClassDeclaration(copy);
          ROSE_ASSERT(classDeclaration_copy != NULL);

          SgClassDefinition* classDefinition_original = this->get_definition();
          SgClassDefinition* classDefinition_copy     = classDeclaration_copy->get_definition();

          classDefinition_original->fixupCopy_references(classDefinition_copy,help);
        }
   }

void
SgClassDefinition::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgClassDefinition::fixupCopy_references() for class = %s class definition %p = %s copy = %p \n",this->get_declaration()->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgClassDefinition* classDefinition_copy = isSgClassDefinition(copy);
     ROSE_ASSERT(classDefinition_copy != NULL);

     SgBaseClassPtrList::const_iterator i_original = this->get_inheritances().begin();
     SgBaseClassPtrList::iterator i_copy           = classDefinition_copy->get_inheritances().begin();
     ROSE_ASSERT(this->get_inheritances().size() == classDefinition_copy->get_inheritances().size());

     while ( (i_original != this->get_inheritances().end()) && (i_copy != classDefinition_copy->get_inheritances().end()) )
        {
       // Check the parent pointer to make sure it is properly set
          ROSE_ASSERT( (*i_original)->get_parent() != NULL);
          ROSE_ASSERT( (*i_original)->get_parent() == this);

          (*i_original)->fixupCopy_references(*i_copy,help);

       // DQ (11/7/2007): This is no longer required now that we separate out the fixup of the parent/scope from the symbols and the references.
       // (*i_copy)->set_parent(classDefinition_copy);

          ROSE_ASSERT( (*i_copy)->get_parent() != NULL);
          ROSE_ASSERT( (*i_copy)->get_parent() == classDefinition_copy);

          i_original++;
          i_copy++;
        }

     const SgDeclarationStatementPtrList & statementList_original = this->getDeclarationList();
     const SgDeclarationStatementPtrList & statementList_copy     = classDefinition_copy->getDeclarationList();

     SgDeclarationStatementPtrList::const_iterator j_original = statementList_original.begin();
     SgDeclarationStatementPtrList::const_iterator j_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (j_original != statementList_original.end()) && (j_copy != statementList_copy.end()) )
        {
          (*j_original)->fixupCopy_references(*j_copy,help);

          j_original++;
          j_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_references(copy,help);

  // printf ("\nLeaving SgClassDefinition::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }


void
SgBaseClass::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgBaseClass::fixupCopy_references() for baseclass = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgBaseClass* baseClass_copy = isSgBaseClass(copy);
     ROSE_ASSERT(baseClass_copy != NULL);

     const SgNonrealBaseClass* nrBaseClass = isSgNonrealBaseClass(this);
     SgNonrealBaseClass* nrBaseClass_copy = isSgNonrealBaseClass(copy);

     if (this->get_base_class() != NULL) {
       ROSE_ASSERT(baseClass_copy->get_base_class());

       ROSE_ASSERT(nrBaseClass == NULL);
       ROSE_ASSERT(nrBaseClass_copy == NULL);

       this->get_base_class()->fixupCopy_references(baseClass_copy->get_base_class(),help);
     } else if (nrBaseClass != NULL) {
       ROSE_ASSERT(nrBaseClass->get_base_class_nonreal() != NULL);

       ROSE_ASSERT(nrBaseClass_copy != NULL);
       ROSE_ASSERT(nrBaseClass_copy->get_base_class_nonreal() != NULL);

       nrBaseClass->get_base_class_nonreal()->fixupCopy_references(nrBaseClass_copy->get_base_class_nonreal(),help);
     } else {
       ROSE_ASSERT(false);
     }
   }


void
SgLabelStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgLabelStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy_references(copy,help);
   }

void
SgGotoStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgGotoStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy_references(copy,help);
   }

void
SgTypedefDeclaration::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTypedefDeclaration::fixupCopy_references() for typedef name = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_references(copy,help);

     SgTypedefDeclaration* typedefDeclaration_copy = isSgTypedefDeclaration(copy);
     ROSE_ASSERT(typedefDeclaration_copy != NULL);

  // DQ (10/14/2007): Handle the case of a type defined in the base type of the typedef (similar problem for SgVariableDeclaration).
     if (this->get_typedefBaseTypeContainsDefiningDeclaration() == true)
        {
          ROSE_ASSERT(typedefDeclaration_copy->get_typedefBaseTypeContainsDefiningDeclaration() == true);
          SgDeclarationStatement* baseTypeDeclaration_original = this->get_baseTypeDefiningDeclaration();
          SgDeclarationStatement* baseTypeDeclaration_copy     = typedefDeclaration_copy->get_baseTypeDefiningDeclaration();
          ROSE_ASSERT(baseTypeDeclaration_original != NULL);
          ROSE_ASSERT(baseTypeDeclaration_copy != NULL);

          baseTypeDeclaration_original->fixupCopy_references(baseTypeDeclaration_copy,help);
        }
   }


void
SgEnumDeclaration::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgEnumDeclaration::fixupCopy_references() for %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_references(copy,help);

     SgEnumDeclaration* enumDeclaration_copy = isSgEnumDeclaration(copy);
     ROSE_ASSERT(enumDeclaration_copy != NULL);
     
  // DQ (10/17/2007): fixup the type used to make sure it has the declaration set the AST copy.
     SgEnumType* enum_type_original = this->get_type();
     ROSE_ASSERT(enum_type_original != NULL);

     SgEnumType* enum_type_copy = enumDeclaration_copy->get_type();
     ROSE_ASSERT(enum_type_copy != NULL);

  // Now reset the enum fields.
     const SgInitializedNamePtrList & enumFieldList_original = this->get_enumerators();
     SgInitializedNamePtrList & enumFieldList_copy           = enumDeclaration_copy->get_enumerators();

     SgInitializedNamePtrList::const_iterator i_original = enumFieldList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = enumFieldList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != enumFieldList_original.end()) && (i_copy != enumFieldList_copy.end()) )
        {
          (*i_original)->fixupCopy_references(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }


void
SgNamespaceDeclarationStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgNamespaceDeclarationStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_references(copy,help);

     SgNamespaceDeclarationStatement* namespaceDeclaration_copy = isSgNamespaceDeclarationStatement(copy);
     ROSE_ASSERT(namespaceDeclaration_copy != NULL);

     SgNamespaceDefinitionStatement* namespaceDefinition_original = this->get_definition();
     SgNamespaceDefinitionStatement* namespaceDefinition_copy     = namespaceDeclaration_copy->get_definition();

     ROSE_ASSERT(namespaceDefinition_original != NULL);
     ROSE_ASSERT(namespaceDefinition_copy != NULL);

     namespaceDefinition_original->fixupCopy_references(namespaceDefinition_copy,help);
   }


void
SgNamespaceDefinitionStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgNamespaceDefinitionStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgNamespaceDefinitionStatement* namespaceDefinition_copy = isSgNamespaceDefinitionStatement(copy);
     ROSE_ASSERT(namespaceDefinition_copy != NULL);

     const SgDeclarationStatementPtrList & statementList_original = this->getDeclarationList();
     const SgDeclarationStatementPtrList & statementList_copy     = namespaceDefinition_copy->getDeclarationList();

     SgDeclarationStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgDeclarationStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
          (*i_original)->fixupCopy_references(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_references(copy,help);

#if 0
     printf ("Leaving SgNamespaceDefinitionStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
   }


void
SgTemplateInstantiationDirectiveStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTemplateInstantiationDirectiveStatement::fixupCopy_references() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_references(copy,help);
   }



void
SgProject::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgProject::fixupCopy_references() \n");
#endif

     SgProject* project_copy = isSgProject(copy);
     ROSE_ASSERT(project_copy != NULL);

  // Call fixup on all fo the files (SgFile objects)
     for (int i = 0; i < numberOfFiles(); i++)
        {
          SgFile & file = get_file(i);
          SgFile & file_copy = project_copy->get_file(i);
          file.fixupCopy_references(&file_copy,help);
        }
   }

void
SgSourceFile::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFile::fixupCopy_references() \n");
#endif

     SgSourceFile* file_copy = isSgSourceFile(copy);
     ROSE_ASSERT(file_copy != NULL);

  // Call fixup on the global scope
     ROSE_ASSERT(get_globalScope() != NULL);
     ROSE_ASSERT(file_copy->get_globalScope() != NULL);
     get_globalScope()->fixupCopy_references(file_copy->get_globalScope(),help);
   } 


void
SgIfStmt::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgIfStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // SgStatement::fixupCopy_references(copy,help);
     SgScopeStatement::fixupCopy_references(copy,help);

     SgIfStmt* ifStatement_copy = isSgIfStmt(copy);
     ROSE_ASSERT(ifStatement_copy != NULL);

     this->get_conditional()->fixupCopy_references(ifStatement_copy->get_conditional(),help);

  // printf ("\nProcess the true body of the SgIfStmt \n\n");

     this->get_true_body()->fixupCopy_references(ifStatement_copy->get_true_body(),help);

  // printf ("\nProcess the false body of the SgIfStmt \n\n");

     if (this->get_false_body()) {
       this->get_false_body()->fixupCopy_references(ifStatement_copy->get_false_body(),help);
     }

  // printf ("\nLeaving SgIfStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

void
SgForStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgForStatement::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_references(copy,help);

     SgForStatement* forStatement_copy = isSgForStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

     ROSE_ASSERT(this->get_for_init_stmt() != NULL);
     this->get_for_init_stmt()->fixupCopy_references(forStatement_copy->get_for_init_stmt(),help);

     ROSE_ASSERT(this->get_test() != NULL);
     this->get_test()->fixupCopy_references(forStatement_copy->get_test(),help);

     ROSE_ASSERT(this->get_increment() != NULL);
     this->get_increment()->fixupCopy_references(forStatement_copy->get_increment(),help);

     ROSE_ASSERT(this->get_loop_body() != NULL);
     this->get_loop_body()->fixupCopy_references(forStatement_copy->get_loop_body(),help);
   }

void
SgRangeBasedForStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgRangeBasedForStatement::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_references(copy,help);

     SgRangeBasedForStatement* forStatement_copy = isSgRangeBasedForStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

     ROSE_ASSERT(this->get_iterator_declaration() != NULL);
     this->get_iterator_declaration()->fixupCopy_references(forStatement_copy->get_iterator_declaration(),help);

     ROSE_ASSERT(this->get_range_declaration() != NULL);
     this->get_range_declaration()->fixupCopy_references(forStatement_copy->get_range_declaration(),help);

     ROSE_ASSERT(this->get_begin_declaration() != NULL);
     this->get_begin_declaration()->fixupCopy_references(forStatement_copy->get_begin_declaration(),help);

     ROSE_ASSERT(this->get_end_declaration() != NULL);
     this->get_end_declaration()->fixupCopy_references(forStatement_copy->get_end_declaration(),help);

     ROSE_ASSERT(this->get_not_equal_expression() != NULL);
     this->get_not_equal_expression()->fixupCopy_references(forStatement_copy->get_not_equal_expression(),help);

     ROSE_ASSERT(this->get_increment_expression() != NULL);
     this->get_increment_expression()->fixupCopy_references(forStatement_copy->get_increment_expression(),help);

     ROSE_ASSERT(this->get_loop_body() != NULL);
     this->get_loop_body()->fixupCopy_references(forStatement_copy->get_loop_body(),help);
   }

void
SgForInitStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgForInitStatement::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_references(copy,help);

     SgForInitStatement* forStatement_copy = isSgForInitStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

     const SgStatementPtrList & statementList_original = this->get_init_stmt();
     const SgStatementPtrList & statementList_copy     = forStatement_copy->get_init_stmt();

     SgStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
        // printf ("In SgForInitStatement::fixupCopy_references(): Calling fixup for *i_copy = %p = %s \n",(*i_copy),(*i_copy)->class_name().c_str());
          (*i_original)->fixupCopy_references(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }

void
SgCatchStatementSeq::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCatchStatementSeq::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_references(copy,help);

     SgCatchStatementSeq* catchStatement_copy = isSgCatchStatementSeq(copy);
     ROSE_ASSERT(catchStatement_copy != NULL);

     printf ("SgCatchStatementSeq::fixupCopy_references(): Sorry not implemented \n");

  // The relavant data member here is a SgStatementPtrList      p_catch_statement_seq

  // ROSE_ASSERT(this->get_body() != NULL);
  // this->get_body()->fixupCopy_references(catchStatement_copy->get_body(),help);
   }

void
SgWhileStmt::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgWhileStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_references(copy,help);

     SgWhileStmt* whileStatement_copy = isSgWhileStmt(copy);
     ROSE_ASSERT(whileStatement_copy != NULL);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy_references(whileStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_references(whileStatement_copy->get_body(),help);
   }

void
SgDoWhileStmt::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDoWhileStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_references(copy,help);

     SgDoWhileStmt* doWhileStatement_copy = isSgDoWhileStmt(copy);
     ROSE_ASSERT(doWhileStatement_copy != NULL);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy_references(doWhileStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_references(doWhileStatement_copy->get_body(),help);
   }


void
SgSwitchStatement::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgSwitchStatement::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_references(copy,help);

     SgSwitchStatement* switchStatement_copy = isSgSwitchStatement(copy);
     ROSE_ASSERT(switchStatement_copy != NULL);

     ROSE_ASSERT(this->get_item_selector() != NULL);
     this->get_item_selector()->fixupCopy_references(switchStatement_copy->get_item_selector(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_references(switchStatement_copy->get_body(),help);
   }



void
SgTryStmt::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTryStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_references(copy,help);

     SgTryStmt* tryStatement_copy = isSgTryStmt(copy);
     ROSE_ASSERT(tryStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_references(tryStatement_copy->get_body(),help);
   }

void
SgCatchOptionStmt::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCatchOptionStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     printf ("SgCatchOptionStmt::fixupCopy_references(): Sorry not implemented \n");

     SgScopeStatement::fixupCopy_references(copy,help);

     SgCatchOptionStmt* catchOptionStatement_copy = isSgCatchOptionStmt(copy);
     ROSE_ASSERT(catchOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_trystmt() != NULL);
  // I think this might cause endless recursion, so comment out for now!
  // this->get_trystmt()->fixupCopy_references(catchOptionStatement_copy->get_trystmt(),help);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy_references(catchOptionStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_references(catchOptionStatement_copy->get_body(),help);
   }

void
SgCaseOptionStmt::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCaseOptionStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_references(copy,help);

     SgCaseOptionStmt* caseOptionStatement_copy = isSgCaseOptionStmt(copy);
     ROSE_ASSERT(caseOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_references(caseOptionStatement_copy->get_body(),help);
   }

void
SgDefaultOptionStmt::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDefaultOptionStmt::fixupCopy_references() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_references(copy,help);

     SgDefaultOptionStmt* defaultOptionStatement_copy = isSgDefaultOptionStmt(copy);
     ROSE_ASSERT(defaultOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_references(defaultOptionStatement_copy->get_body(),help);
   }

void
SgTemplateArgument::fixupCopy_references(SgNode* copy, SgCopyHelp & help) const
   {
     SgTemplateArgument* templateArgument_copy = isSgTemplateArgument(copy);
     ROSE_ASSERT(templateArgument_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateArgument::fixupCopy_references(): this = %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
   }


