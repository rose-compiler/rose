// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "fixupCopy.h"

// This file implementes support for the AST copy fixup.  It is specific to:
// 1) Construction of symbols, and
// 2) setup of symbol tables

void
SgInitializedName::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgInitializedName::fixupCopy_symbols() %p = %s \n",this,this->get_name().str());
#endif

// DQ (11/7/2007): I think that there is nothing to do here specific to symbol and symbol tables.
   }


void
SgStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgVariableDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgLocatedNode::fixupCopy_symbols(copy,help);
   }


void
SgExpression::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgExpression::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     Rose_STL_Container<SgNode*> children_original = const_cast<SgExpression*>(this)->get_traversalSuccessorContainer();
     Rose_STL_Container<SgNode*> children_copy     = const_cast<SgNode*>(copy)->get_traversalSuccessorContainer();
     ROSE_ASSERT (children_original.size() == children_copy.size());

     for (Rose_STL_Container<SgNode*>::const_iterator
            i_original = children_original.begin(),
            i_copy = children_copy.begin();
          i_original != children_original.end(); ++i_original, ++i_copy) {
       if (*i_original == NULL) continue;
       (*i_original)->fixupCopy_symbols(*i_copy,help);
     }

     SgLocatedNode::fixupCopy_symbols(copy,help);
   }


void
SgLocatedNode::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgLocatedNode::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
   }


void
SgScopeStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
#if 0
     printf ("Inside of SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p (calling SgStatement::fixupCopy_symbols()) \n",this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgStatement::fixupCopy_symbols(copy,help);

#if 0
     printf ("DONE: Inside of SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p (calling SgStatement::fixupCopy_symbols()) \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement* copyScopeStatement = isSgScopeStatement(copy);
     ROSE_ASSERT(copyScopeStatement != NULL);

  // The symbol table should not have been setup yet!
  // ROSE_ASSERT(copyScopeStatement->get_symbol_table()->size() == 0);
     ROSE_ASSERT(copyScopeStatement->symbol_table_size() == 0);

#if 0
     printf ("Inside of SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p (calling SageInterface::rebuildSymbolTable()) \n",this,this->class_name().c_str(),copy);
#endif

     SageInterface::rebuildSymbolTable(copyScopeStatement);

#if 0
     printf ("DONE: Inside of SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p (calling SageInterface::rebuildSymbolTable()) \n",this,this->class_name().c_str(),copy);
     printf ("Inside of SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p (calling SageInterface::fixupReferencesToSymbols()) \n",this,this->class_name().c_str(),copy);
#endif

  // DQ (3/1/2009): After rebuilding the symbol table, we have to reset references 
  // to old symbols (from the original symbol table) to the new symbols just built.
     SageInterface::fixupReferencesToSymbols(this,copyScopeStatement,help);

#if 0
     printf ("DONE: Inside of SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p (calling SageInterface::fixupReferencesToSymbols()) \n",this,this->class_name().c_str(),copy);
#endif

  // printf ("\nLeaving SgScopeStatement::fixupCopy_symbols() for %p = %s copy = %p \n\n",this,this->class_name().c_str(),copy);
   }



void
SgGlobal::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgGlobal::fixupCopy_symbols() for %p copy = %p \n",this,copy);
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
          (*i_original)->fixupCopy_symbols(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_symbols(copy,help);

  // printf ("\nLeaving SgGlobal::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

// JJW 2/1/2008 Added support for statement expressions
void
SgExprStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgSgExprStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgExprStatement* es_copy = isSgExprStatement(copy);
     ROSE_ASSERT(es_copy != NULL);

     SgExpression* expression_original = this->get_expression();
     SgExpression* expression_copy     = es_copy->get_expression();

     expression_original->fixupCopy_symbols(expression_copy, help);

  // Call the base class fixupCopy member function
     SgStatement::fixupCopy_symbols(copy,help);

  // printf ("\nLeaving SgExprStatement::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

// DQ (10/6/2007): Added fixup function to set scopes not set properly by the ROSETTA generated copy!
void
SgBasicBlock::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgBasicBlock::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
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
          (*i_original)->fixupCopy_symbols(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_symbols(copy,help);

  // printf ("\nLeaving SgBasicBlock::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

void
SgDeclarationStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDeclarationStatement::fixupCopy_symbols() for %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

  // Call the base class fixupCopy member function (this will setup the parent)
     SgStatement::fixupCopy_symbols(copy,help);
   }


void
SgFunctionDeclaration::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgFunctionDeclaration::fixupCopy_symbols(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgFunctionDeclaration* functionDeclaration_copy = isSgFunctionDeclaration(copy);
     ROSE_ASSERT(functionDeclaration_copy != NULL);

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_parameterList() != NULL);
     get_parameterList()->fixupCopy_symbols(functionDeclaration_copy->get_parameterList(),help);

  // Setup the details in the SgFunctionDefinition (this may have to rebuild the sysmbol table)
  // printf ("In SgFunctionDeclaration::fixupCopy_symbols(): this->get_definition() = %p \n",this->get_definition());
     if (this->get_definition() != NULL)
        {
       // DQ (3/15/2014): The defining declaration should not be marked (isForward() == true).
          if (isForward() == true)
             {
               printf ("Error: The defining declaration should not be marked (isForward() == true) \n");
               printf ("SgFunctionDeclaration::fixupCopy_symbols(): (isForward() == true): functionDeclaration_copy = %p = %s \n",functionDeclaration_copy,functionDeclaration_copy->class_name().c_str());
               printf ("   --- functionDeclaration_copy->get_firstNondefiningDeclaration() = %p \n",functionDeclaration_copy->get_firstNondefiningDeclaration());
               printf ("   --- functionDeclaration_copy->get_definingDeclaration()         = %p \n",functionDeclaration_copy->get_definingDeclaration());

               functionDeclaration_copy->get_file_info()->display("SgFunctionDeclaration::fixupCopy_scopes(): (isForward() == true): debug");

            // Reset this!
            // functionDeclaration_copy->unsetForward();
             }
          ROSE_ASSERT(isForward() == false);

       // DQ (2/26/2009): Handle special cases where the copyHelp function is non-trivial.
       // Is every version of copyHelp object going to be a problem?

       // For the outlining, our copyHelp object does not copy defining function declarations 
       // and substitutes a non-defining declarations, so if the copy has been built this way 
       // then skip trying to reset the SgFunctionDefinition.
       // printf ("In SgFunctionDeclaration::fixupCopy_symbols(): functionDeclaration_copy->get_definition() = %p \n",functionDeclaration_copy->get_definition());
       // this->get_definition()->fixupCopy_symbols(functionDeclaration_copy->get_definition(),help);
          if (functionDeclaration_copy->get_definition() != NULL)
             {
               this->get_definition()->fixupCopy_symbols(functionDeclaration_copy->get_definition(),help);
             }

       // If this is a declaration with a definition then it is a defining declaration
       // functionDeclaration_copy->set_definingDeclaration(functionDeclaration_copy);
        }

  // printf ("\nLeaving SgFunctionDeclaration::fixupCopy_symbols(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
   }

// DQ (10/6/2007): Added fixup function to set scopes not set properly by the ROSETTA generated copy!
void
SgFunctionParameterList::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFunctionParameterList::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);

     SgFunctionParameterList* copyFunctionParameterList = isSgFunctionParameterList(copy);
     ROSE_ASSERT(copyFunctionParameterList != NULL);

     const SgInitializedNamePtrList & parameterList_original = this->get_args();
     SgInitializedNamePtrList & parameterList_copy           = copyFunctionParameterList->get_args();

     SgInitializedNamePtrList::const_iterator i_original = parameterList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = parameterList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != parameterList_original.end()) && (i_copy != parameterList_copy.end()) )
        {
          (*i_original)->fixupCopy_symbols(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }


void
SgMemberFunctionDeclaration::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("In SgMemberFunctionDeclaration::fixupCopy_symbols(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgMemberFunctionDeclaration* memberFunctionDeclaration_copy = isSgMemberFunctionDeclaration(copy);
     ROSE_ASSERT(memberFunctionDeclaration_copy != NULL);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_CtorInitializerList() != NULL);
     get_CtorInitializerList()->fixupCopy_symbols(memberFunctionDeclaration_copy->get_CtorInitializerList(),help);

  // Call the base class fixupCopy member function
     SgFunctionDeclaration::fixupCopy_symbols(copy,help);
   }


void
SgTemplateDeclaration::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nInside of SgTemplateDeclaration::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopy() member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);
   }


void
SgTemplateInstantiationDefn::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTemplateInstantiationDefn::fixupCopy_symbols() for class = %s class definition %p = %s copy = %p \n",this->get_declaration()->get_name().str(),this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgClassDefinition::fixupCopy_symbols(copy,help);
   }


void
SgTemplateInstantiationDecl::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationDecl::fixupCopy_symbols(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgTemplateInstantiationDecl* templateClassDeclaration_copy = isSgTemplateInstantiationDecl(copy);
     ROSE_ASSERT(templateClassDeclaration_copy != NULL);

  // Also call the base class version of the fixupCopycopy() member function
     SgClassDeclaration::fixupCopy_symbols(copy,help);

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateClassDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateClassDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy_symbols(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        }
#endif
   }

void
SgTemplateInstantiationMemberFunctionDecl::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationMemberFunctionDecl::fixupCopy_symbols(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgTemplateInstantiationMemberFunctionDecl* templateMemberFunctionDeclaration_copy = isSgTemplateInstantiationMemberFunctionDecl(copy);
     ROSE_ASSERT(templateMemberFunctionDeclaration_copy != NULL);

  // Also call the base class version of the fixupCopycopy() member function
     SgMemberFunctionDeclaration::fixupCopy_symbols(copy,help);

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateMemberFunctionDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateMemberFunctionDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy_symbols(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        }
#endif
   }


void
SgTemplateInstantiationFunctionDecl::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationFunctionDecl::fixupCopy_symbols(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration_copy = isSgTemplateInstantiationFunctionDecl(copy);
     ROSE_ASSERT(templateFunctionDeclaration_copy != NULL);

  // Also call the base class version of the fixupCopycopy() member function
     SgFunctionDeclaration::fixupCopy_symbols(copy,help);

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateFunctionDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateFunctionDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy_symbols(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        } 
#endif
   }


void
SgFunctionDefinition::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFunctionDefinition::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgFunctionDefinition* functionDefinition_copy = isSgFunctionDefinition(copy);
     ROSE_ASSERT(functionDefinition_copy != NULL);

     ROSE_ASSERT(get_body() != NULL);
     get_body()->fixupCopy_symbols(functionDefinition_copy->get_body(),help);

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_symbols(copy,help);

  // printf ("\nLeaving SgFunctionDefinition::fixupCopy_symbols() for %p = %s copy = %p \n\n",this,this->class_name().c_str(),copy);
   }

void
SgVariableDeclaration::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgVariableDeclaration::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);

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

       // printf ("In SgVariableDeclaration::fixupCopy_symbols(): Calling fixupCopy on %p = %s \n",baseTypeDeclaration_original,baseTypeDeclaration_original->class_name().c_str());

          baseTypeDeclaration_original->fixupCopy_symbols(baseTypeDeclaration_copy,help);
        }

     const SgInitializedNamePtrList & variableList_original = this->get_variables();
     SgInitializedNamePtrList & variableList_copy           = variableDeclaration_copy->get_variables();

  // printf ("Inside of SgVariableDeclaration::fixupCopy_symbols(): variableList_original.size() = %ld \n",(long)variableList_original.size());

     ROSE_ASSERT(variableList_original.size() == variableList_copy.size());

     SgInitializedNamePtrList::const_iterator i_original = variableList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = variableList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != variableList_original.end()) && (i_copy != variableList_copy.end()) )
        {
       // printf ("Looping over the initialized names in the variable declaration variable = %p = %s \n",(*i_copy),(*i_copy)->get_name().str());

          (*i_original)->fixupCopy_symbols(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }

void
SgClassDeclaration::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgClassDeclaration::fixupCopy_symbols() for class = %s = %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this->get_name().str(),this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

     SgClassDeclaration* classDeclaration_copy = isSgClassDeclaration(copy);
     ROSE_ASSERT(classDeclaration_copy != NULL);

     SgClassDefinition* classDefinition_original = this->get_definition();
     SgClassDefinition* classDefinition_copy     = classDeclaration_copy->get_definition();

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);

     if (isForward() == false)
        {
          classDefinition_original->fixupCopy_symbols(classDefinition_copy,help);
        }
   }

void
SgClassDefinition::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgClassDefinition::fixupCopy_symbols() for class = %s class definition %p = %s copy = %p \n",this->get_declaration()->get_name().str(),this,this->class_name().c_str(),copy);
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

          (*i_original)->fixupCopy_symbols(*i_copy,help);

       // DQ (11/7/2007): This was already setup on fixupCopy_scopes, but we can test it here.
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
          (*j_original)->fixupCopy_symbols(*j_copy,help);

          j_original++;
          j_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_symbols(copy,help);

  // printf ("\nLeaving SgClassDefinition::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }


void
SgBaseClass::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgBaseClass::fixupCopy_symbols() for baseclass = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgBaseClass* baseClass_copy = isSgBaseClass(copy);
     ROSE_ASSERT(baseClass_copy != NULL);

     const SgNonrealBaseClass* nrBaseClass = isSgNonrealBaseClass(this);
     SgNonrealBaseClass* nrBaseClass_copy = isSgNonrealBaseClass(copy);

     if (this->get_base_class() != NULL) {
       ROSE_ASSERT(baseClass_copy->get_base_class());

       ROSE_ASSERT(nrBaseClass == NULL);
       ROSE_ASSERT(nrBaseClass_copy == NULL);

       this->get_base_class()->fixupCopy_symbols(baseClass_copy->get_base_class(),help);
     } else if (nrBaseClass != NULL) {
       ROSE_ASSERT(nrBaseClass->get_base_class_nonreal() != NULL);

       ROSE_ASSERT(nrBaseClass_copy != NULL);
       ROSE_ASSERT(nrBaseClass_copy->get_base_class_nonreal() != NULL);

       nrBaseClass->get_base_class_nonreal()->fixupCopy_symbols(nrBaseClass_copy->get_base_class_nonreal(),help);
     } else {
       ROSE_ASSERT(false);
     }
   }


void
SgLabelStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgLabelStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgLabelStatement* labelStatement_copy = isSgLabelStatement(copy);
     ROSE_ASSERT(labelStatement_copy != NULL);

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy_symbols(copy,help);
   }

void
SgGotoStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgGotoStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy_symbols(copy,help);
   }

void
SgAdaExitStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgAdaExitStmt::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy_symbols(copy,help);
   }

void
SgAdaLoopStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgAdaLoopStmt::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy_symbols(copy,help);
   }

void
SgAdaAcceptStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgAdaLoopStmt::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy_symbols(copy,help);
   }


void
SgTypedefDeclaration::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTypedefDeclaration::fixupCopy_symbols() for typedef name = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif
#if 0
     printf ("Inside of SgTypedefDeclaration::fixupCopy_symbols() for typedef name = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif
  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);

  // DQ (10/14/2007): Handle the case of a type defined in the base type of the typedef (similar problem for SgVariableDeclaration).
     if (this->get_typedefBaseTypeContainsDefiningDeclaration() == true)
        {
          SgTypedefDeclaration* typedefDeclaration_copy = isSgTypedefDeclaration(copy);
          ROSE_ASSERT(typedefDeclaration_copy != NULL);

          ROSE_ASSERT(typedefDeclaration_copy->get_typedefBaseTypeContainsDefiningDeclaration() == true);
          SgDeclarationStatement* baseTypeDeclaration_original = this->get_baseTypeDefiningDeclaration();
          SgDeclarationStatement* baseTypeDeclaration_copy     = typedefDeclaration_copy->get_baseTypeDefiningDeclaration();
          ROSE_ASSERT(baseTypeDeclaration_original != NULL);
          ROSE_ASSERT(baseTypeDeclaration_copy != NULL);
#if 0
          printf ("In SgTypedefDeclaration::fixupCopy_symbols(): calling baseTypeDeclaration_original->fixupCopy_symbols(baseTypeDeclaration_copy = %p) \n",baseTypeDeclaration_copy);
#endif
          baseTypeDeclaration_original->fixupCopy_symbols(baseTypeDeclaration_copy,help);
#if 0
          printf ("DONE: In SgTypedefDeclaration::fixupCopy_symbols(): calling baseTypeDeclaration_original->fixupCopy_symbols(baseTypeDeclaration_copy = %p) \n",baseTypeDeclaration_copy);
#endif
        }
#if 0
     printf ("Leaving SgTypedefDeclaration::fixupCopy_symbols() for typedef name = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif
   }


void
SgEnumDeclaration::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgEnumDeclaration::fixupCopy_symbols() for %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);

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
          (*i_original)->fixupCopy_symbols(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }


void
SgNamespaceDeclarationStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgNamespaceDeclarationStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
#if 0
     printf ("Inside of SgNamespaceDeclarationStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);

     SgNamespaceDeclarationStatement* namespaceDeclaration_copy = isSgNamespaceDeclarationStatement(copy);
     ROSE_ASSERT(namespaceDeclaration_copy != NULL);

     SgNamespaceDefinitionStatement* namespaceDefinition_original = this->get_definition();
     SgNamespaceDefinitionStatement* namespaceDefinition_copy     = namespaceDeclaration_copy->get_definition();

     ROSE_ASSERT(namespaceDefinition_original != NULL);
     ROSE_ASSERT(namespaceDefinition_copy != NULL);

     namespaceDefinition_original->fixupCopy_symbols(namespaceDefinition_copy,help);
#if 0
     printf ("Leaving SgNamespaceDeclarationStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
   }


void
SgNamespaceDefinitionStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgNamespaceDefinitionStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
#if 0
     printf ("Inside of SgNamespaceDefinitionStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
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
#if 0
          printf ("Inside of SgNamespaceDefinitionStatement::fixupCopy_symbols() for %p = %s copy = %p (in loop over statements) \n",this,this->class_name().c_str(),copy);
#endif
          (*i_original)->fixupCopy_symbols(*i_copy,help);

          i_original++;
          i_copy++;
        }
#if 0
     printf ("Leaving SgNamespaceDefinitionStatement::fixupCopy_symbols() for %p = %s copy = %p (call SgScopeStatement::fixupCopy_symbols()) \n",this,this->class_name().c_str(),copy);
#endif
  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy_symbols(copy,help);
#if 0
     printf ("Leaving SgNamespaceDefinitionStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
   }


void
SgTemplateInstantiationDirectiveStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTemplateInstantiationDirectiveStatement::fixupCopy_symbols() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy_symbols(copy,help);
   }



void
SgProject::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgProject::fixupCopy_symbols() \n");
#endif

     SgProject* project_copy = isSgProject(copy);
     ROSE_ASSERT(project_copy != NULL);

  // Call fixup on all fo the files (SgFile objects)
     for (int i = 0; i < numberOfFiles(); i++)
        {
          SgFile & file = get_file(i);
          SgFile & file_copy = project_copy->get_file(i);
          file.fixupCopy_symbols(&file_copy,help);
        }
   }

void
SgSourceFile::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFile::fixupCopy_symbols() \n");
#endif

     SgSourceFile* file_copy = isSgSourceFile(copy);
     ROSE_ASSERT(file_copy != NULL);

  // Call fixup on the global scope
     ROSE_ASSERT(get_globalScope() != NULL);
     ROSE_ASSERT(file_copy->get_globalScope() != NULL);
     get_globalScope()->fixupCopy_symbols(file_copy->get_globalScope(),help);
   } 


void
SgIfStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgIfStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // SgStatement::fixupCopy_symbols(copy,help);
     SgScopeStatement::fixupCopy_symbols(copy,help);

     SgIfStmt* ifStatement_copy = isSgIfStmt(copy);
     ROSE_ASSERT(ifStatement_copy != NULL);

  // The symbol table should not have been setup yet!
  // ROSE_ASSERT(ifStatement_copy->get_symbol_table()->size() == 0);

     this->get_conditional()->fixupCopy_symbols(ifStatement_copy->get_conditional(),help);

     SgStatement* thsTruBody = this->get_true_body();
     ROSE_ASSERT(thsTruBody != NULL);
     SgStatement* ifStmtCopyTruBody = ifStatement_copy->get_true_body();
     ROSE_ASSERT(ifStmtCopyTruBody != NULL);
     SgScopeStatement* scopeStmntCopyTrueBody = isSgScopeStatement(ifStmtCopyTruBody);
     if (scopeStmntCopyTrueBody != NULL) {
     // ROSE_ASSERT(scopeStmntCopyTrueBody->get_symbol_table() != NULL);
     // ROSE_ASSERT(scopeStmntCopyTrueBody->get_symbol_table()->size()  == 0);
        ROSE_ASSERT(scopeStmntCopyTrueBody->symbol_table_size() == 0);
     }

  // printf ("\nProcess the TRUE body of the SgIfStmt \n\n");

     thsTruBody->fixupCopy_symbols(ifStmtCopyTruBody,help);

     ROSE_ASSERT((this->get_false_body() != NULL) == (ifStatement_copy->get_false_body() != NULL));
     SgScopeStatement* scopeStmntCopyFalseBody = isSgScopeStatement(ifStatement_copy->get_false_body());
     if (scopeStmntCopyFalseBody != NULL) {
     // ROSE_ASSERT(scopeStmntCopyFalseBody->get_symbol_table() != NULL);
     // ROSE_ASSERT(scopeStmntCopyFalseBody->get_symbol_table()->size()  == 0);
        ROSE_ASSERT(scopeStmntCopyFalseBody->symbol_table_size() == 0);
     }

  // printf ("\nProcess the FALSE body of the SgIfStmt \n\n");

     if (this->get_false_body() != NULL) {
       this->get_false_body()->fixupCopy_symbols(ifStatement_copy->get_false_body(),help);
     }

  // printf ("\nLeaving SgIfStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

void
SgForStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgForStatement::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgForStatement* forStatement_copy = isSgForStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

  // DQ (11/7/2007): Now that we separate the fixup of scopes and parent from the symbol table
  // construction these can be called in any order.

     SgScopeStatement::fixupCopy_symbols(copy,help);

  // This are important because there could be a scope setup within this call (e.g. "for ( class X {} x = 0; x != 1; x++) {}")
  // but this is not a common case.
     ROSE_ASSERT(this->get_for_init_stmt() != NULL);
     this->get_for_init_stmt()->fixupCopy_symbols(forStatement_copy->get_for_init_stmt(),help);

     ROSE_ASSERT(this->get_test() != NULL);
     this->get_test()->fixupCopy_symbols(forStatement_copy->get_test(),help);

     ROSE_ASSERT(this->get_increment() != NULL);
     this->get_increment()->fixupCopy_symbols(forStatement_copy->get_increment(),help);

     ROSE_ASSERT(this->get_loop_body() != NULL);
     this->get_loop_body()->fixupCopy_symbols(forStatement_copy->get_loop_body(),help);
   }

void
SgRangeBasedForStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgRangeBasedForStatement::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgRangeBasedForStatement* forStatement_copy = isSgRangeBasedForStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

     SgScopeStatement::fixupCopy_symbols(copy,help);

     ROSE_ASSERT(this->get_iterator_declaration() != NULL);
     this->get_iterator_declaration()->fixupCopy_symbols(forStatement_copy->get_iterator_declaration(),help);

     ROSE_ASSERT(this->get_range_declaration() != NULL);
     this->get_range_declaration()->fixupCopy_symbols(forStatement_copy->get_range_declaration(),help);

     ROSE_ASSERT(this->get_begin_declaration() != NULL);
     this->get_begin_declaration()->fixupCopy_symbols(forStatement_copy->get_begin_declaration(),help);

     ROSE_ASSERT(this->get_end_declaration() != NULL);
     this->get_end_declaration()->fixupCopy_symbols(forStatement_copy->get_end_declaration(),help);

     ROSE_ASSERT(this->get_not_equal_expression() != NULL);
     this->get_not_equal_expression()->fixupCopy_symbols(forStatement_copy->get_not_equal_expression(),help);

     ROSE_ASSERT(this->get_increment_expression() != NULL);
     this->get_increment_expression()->fixupCopy_symbols(forStatement_copy->get_increment_expression(),help);

     ROSE_ASSERT(this->get_loop_body() != NULL);
     this->get_loop_body()->fixupCopy_symbols(forStatement_copy->get_loop_body(),help);
   }

void
SgForInitStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgForInitStatement::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_symbols(copy,help);

     SgForInitStatement* forStatement_copy = isSgForInitStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

     const SgStatementPtrList & statementList_original = this->get_init_stmt();
     const SgStatementPtrList & statementList_copy     = forStatement_copy->get_init_stmt();

     SgStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
        // printf ("In SgForInitStatement::fixupCopy_symbols(): Calling fixup for *i_copy = %p = %s \n",(*i_copy),(*i_copy)->class_name().c_str());
          (*i_original)->fixupCopy_symbols(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Relavant data member is: SgStatementPtrList        p_init_stmt
   }

void
SgCatchStatementSeq::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCatchStatementSeq::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_symbols(copy,help);

     SgCatchStatementSeq* catchStatement_copy = isSgCatchStatementSeq(copy);
     ROSE_ASSERT(catchStatement_copy != NULL);

     printf ("SgCatchStatementSeq::fixupCopy_symbols(): Sorry not implemented \n");

  // The relavant data member here is a SgStatementPtrList      p_catch_statement_seq

  // ROSE_ASSERT(this->get_body() != NULL);
  // this->get_body()->fixupCopy_symbols(catchStatement_copy->get_body(),help);
   }

void
SgWhileStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgWhileStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_symbols(copy,help);

     SgWhileStmt* whileStatement_copy = isSgWhileStmt(copy);
     ROSE_ASSERT(whileStatement_copy != NULL);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy_symbols(whileStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_symbols(whileStatement_copy->get_body(),help);
   }

void
SgDoWhileStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDoWhileStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_symbols(copy,help);

     SgDoWhileStmt* doWhileStatement_copy = isSgDoWhileStmt(copy);
     ROSE_ASSERT(doWhileStatement_copy != NULL);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy_symbols(doWhileStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_symbols(doWhileStatement_copy->get_body(),help);
   }


void
SgSwitchStatement::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgSwitchStatement::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy_symbols(copy,help);

     SgSwitchStatement* switchStatement_copy = isSgSwitchStatement(copy);
     ROSE_ASSERT(switchStatement_copy != NULL);

     ROSE_ASSERT(this->get_item_selector() != NULL);
     this->get_item_selector()->fixupCopy_symbols(switchStatement_copy->get_item_selector(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_symbols(switchStatement_copy->get_body(),help);
   }



void
SgTryStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTryStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_symbols(copy,help);

     SgTryStmt* tryStatement_copy = isSgTryStmt(copy);
     ROSE_ASSERT(tryStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_symbols(tryStatement_copy->get_body(),help);
   }

void
SgCatchOptionStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCatchOptionStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     printf ("SgCatchOptionStmt::fixupCopy_symbols(): Sorry not implemented \n");

     SgScopeStatement::fixupCopy_symbols(copy,help);

     SgCatchOptionStmt* catchOptionStatement_copy = isSgCatchOptionStmt(copy);
     ROSE_ASSERT(catchOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_trystmt() != NULL);
  // I think this might cause endless recursion, so comment out for now!
  // this->get_trystmt()->fixupCopy_symbols(catchOptionStatement_copy->get_trystmt(),help);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy_symbols(catchOptionStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_symbols(catchOptionStatement_copy->get_body(),help);
   }

void
SgCaseOptionStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCaseOptionStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_symbols(copy,help);

     SgCaseOptionStmt* caseOptionStatement_copy = isSgCaseOptionStmt(copy);
     ROSE_ASSERT(caseOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_symbols(caseOptionStatement_copy->get_body(),help);
   }

void
SgDefaultOptionStmt::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDefaultOptionStmt::fixupCopy_symbols() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy_symbols(copy,help);

     SgDefaultOptionStmt* defaultOptionStatement_copy = isSgDefaultOptionStmt(copy);
     ROSE_ASSERT(defaultOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy_symbols(defaultOptionStatement_copy->get_body(),help);
   }

void
SgTemplateArgument::fixupCopy_symbols(SgNode* copy, SgCopyHelp & help) const
   {
     SgTemplateArgument* templateArgument_copy = isSgTemplateArgument(copy);
     ROSE_ASSERT(templateArgument_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateArgument::fixupCopy_symbols(): this = %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
   }


