// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AstConsistencyTests.h"

#include "nameQuery.h"
#include "rewrite.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// ************************************************
//         Published Interface functions
// ************************************************


void
LowLevelRewrite::remove(SgStatement* astNode)
{
     removeStatement(astNode);
}


void
LowLevelRewrite::insert(SgStatement* targetStatement, SgStatementPtrList newStatementList, bool insertBeforeNode)
{
        //George Vulov 6/15/2010 - These functions have been gutted and just call through to SageInterface
        //The implmementations were not updated to work correctly with the new AST
        //This class as a whole is deprecated.
        SageInterface::insertStatementList(targetStatement, newStatementList, insertBeforeNode);
}


void
LowLevelRewrite::replace(SgStatement* targetStatement, SgStatementPtrList newStatementList)
{
        //George Vulov 6/15/2010 - These functions have been gutted and just call through to SageInterface
        //The implmementations were not updated to work correctly with the new AST
        //This class as a whole is deprecated.
        SageInterface::insertStatementListBefore(targetStatement, newStatementList);
        SageInterface::removeStatement(targetStatement);
}


void
LowLevelRewrite::insert(SgStatement* targetStatement, SgStatement* newStatement, bool insertBeforeNode)
{
        //George Vulov 6/15/2010 - These functions have been gutted and just call through to SageInterface
        //The implmementations were not updated to work correctly with the new AST
        //This class as a whole is deprecated.
        SageInterface::insertStatement(targetStatement, newStatement, insertBeforeNode);
}


void
LowLevelRewrite::replace(SgStatement* targetStatement, SgStatement* newStatement)
{
        //George Vulov 6/15/2010 - These functions have been gutted and just call through to SageInterface
        //The implmementations were not updated to work correctly with the new AST
        //This class as a whole is deprecated.
        SageInterface::replaceStatement(targetStatement, newStatement);
}

// ************************************************

void
LowLevelRewrite::
insertAtTopOfScope ( 
   const SgStatementPtrList & newTransformationStatementList, 
   AttachedPreprocessingInfoType & commentsAndDirectives,
   SgScopeStatement* currentScope )
   {
  // Insert at top of list (pull the elements off the bottom of the new statement list to get the order correct
  // printf ("Insert new statements (new statement list size = %zu) at the top of the block %s (in reverse order to preset the order in the final block) \n",newTransformationStatementList.size(),currentScope->sage_class_name());

     ROSE_ASSERT (currentScope != NULL);

     int currentNumberOfStatements = (currentScope->containsOnlyDeclarations() == true) ? 
                                          currentScope->getDeclarationList().size() :
                                          currentScope->getStatementList().size();

     int numberOfNewStatementToInsert = newTransformationStatementList.size();
     int numberOfExpectedStatements = currentNumberOfStatements + numberOfNewStatementToInsert;

#if 0
     if (isSgGlobal(currentScope) != NULL)
        {
          printf ("Error: can't specify a SgGlobal as the location of where a new statement will be added \n");
          ROSE_ABORT();
        }
#endif

#if 0
     printf ("currentScope->sage_class_name() = %s \n",currentScope->sage_class_name());
  // printf ("currentScope->unparseToString() = %s \n",currentScope->unparseToString().c_str());
#endif

     if (isSgForStatement(currentScope) != NULL)
        {
          printf ("Error: can't specify a SgForStatement as the location of where a new statement will be added \n");
          printf ("       ***** Specify the basic block instead ***** \n");
          ROSE_ABORT();
        }

  // Loop through the statements in the list
     SgStatementPtrList::const_reverse_iterator transformationStatementIterator;
     for (transformationStatementIterator = newTransformationStatementList.rbegin();
          transformationStatementIterator != newTransformationStatementList.rend();
          transformationStatementIterator++)
        {
#if 0
          string unparsedDeclarationCodeString = (*transformationStatementIterator)->unparseToString();
          printf ("unparsedDeclarationCodeString = %s \n",unparsedDeclarationCodeString.c_str());
#endif

       // list<string> typeNameStringList = NameQuery::getTypeNamesQuery ( *transformationStatementIterator );
       // list<string> typeNameStringList = NameQuery::querySubTree ( *transformationStatementIterator, NameQuery::VariableTypeNames );

       // Generates an error in the query
       // list<string> typeNameStringList = NameQuery::querySubTree ( *transformationStatementIterator, NameQuery::TypeNames );
          Rose_STL_Container<string> typeNameStringList = NameQuery::querySubTree ( *transformationStatementIterator, NameQuery::ClassDeclarationNames );

#if 0
          printf ("typeNameStringList = %s \n",StringUtility::listToString(typeNameStringList).c_str());
          printf ("Exiting after printing the typeNameList ... \n");
          ROSE_ABORT();
#endif

          if ( typeNameStringList.size() > 0 )
             {
            // printf ("This statement has a dependence upon a variable of some type \n");
               printf ("typeNameStringList.size() = %ld \n",(long)typeNameStringList.size());
               printf ("typeNameStringList = %s \n",StringUtility::listToString(typeNameStringList).c_str());

            // handle special case
               insertStatementUsingDependenceInformation (
                    currentScope,
                    *transformationStatementIterator,
                    typeNameStringList );
             }
            else
             {
            // Put the new statement at the front (without regard to any dependencies)
            // insertStatement ( currentScope, *transformationStatementIterator );
            // printf ("Put the new statement at the front (without regard to any dependencies) \n");
               bool atTop = true;
            // printf ("currentScope->sage_class_name() = %s \n",currentScope->sage_class_name());
               currentScope->insertStatementInScope ( *transformationStatementIterator, atTop );
#if 0
               printf ("Exiting after call to currentScope->insertStatementInScope() \n");
               ROSE_ABORT();
#endif
             }
        }

  // Get first statement in scope
     SgStatement* firstStatementInScope = (currentScope->containsOnlyDeclarations() == true) ? 
                                          *(currentScope->getDeclarationList().begin()) :
                                          *(currentScope->getStatementList().begin());
     ROSE_ASSERT (firstStatementInScope != NULL);

     AttachedPreprocessingInfoType::iterator i;
     for (i = commentsAndDirectives.begin(); i != commentsAndDirectives.end(); i++)
        {
       // printf ("Relavant comment or directive attached to starting marker declaration = %s \n",(*i)->getString());
          ROSE_ASSERT ( (*i)->getRelativePosition() == PreprocessingInfo::before);
       // firstStatementInScope->addToAttachedPreprocessingInfo( *i , SgLocatedNode::appendPreprocessingInfoToList );
          firstStatementInScope->addToAttachedPreprocessingInfo( *i , PreprocessingInfo::after );
        }

     int finalNumberOfStatements = (currentScope->containsOnlyDeclarations() == true) ?
                                          currentScope->getDeclarationList().size() :
                                          currentScope->getStatementList().size();

#if 0
     printf ("Number of old statements in scope after insert = %d \n",currentNumberOfStatements);
     printf ("Number of new statements in scope after insert = %d \n",numberOfNewStatementToInsert);
     printf ("Number of final statements in scope after insert = %d \n",finalNumberOfStatements);
#endif

     ROSE_ASSERT (numberOfExpectedStatements == finalNumberOfStatements);

#if 0
     printf ("Print out all the statements \n");
     if (currentScope->containsOnlyDeclarations() == true)
        {
          SgDeclarationStatementPtrList::iterator i;
          for (i = currentScope->getDeclarationList().begin(); i != currentScope->getDeclarationList().end(); i++)
             {
               string s = (*i)->unparseToString();
               ROSE_ASSERT (s.c_str() != NULL);
               printf ("     s = %s \n",s.c_str());
             }
        }
       else
        {
          SgStatementPtrList::iterator i;
          for (i = currentScope->getStatementList().begin(); i != currentScope->getStatementList().end(); i++)
             {
               string s = (*i)->unparseToString();
               ROSE_ASSERT (s.c_str() != NULL);
               printf ("     s = %s \n",s.c_str());
             }
        }
#endif

#if 0
     printf ("Exiting at base of SynthesizedAttribute::insertAtTopOfScope() \n");
     ROSE_ABORT();
#endif
   }

void
LowLevelRewrite::
insertStatementUsingDependenceInformation ( 
   SgScopeStatement* currentScope,
   SgStatement* newTransformationStatement,
   Rose_STL_Container<string> & typeNameStringList )
   {
     ROSE_ASSERT(currentScope != NULL);
     ROSE_ASSERT(newTransformationStatement != NULL);
     ROSE_ASSERT(typeNameStringList.size() > 0);

#if 0
     printf ("ERROR: not being used presently insertStatementUsingDependenceInformation() \n");
     ROSE_ABORT();
#endif

     int statementCounter         = 0;
     int previousStatementCounter = 0;

  // Declaration furthest in source sequence of all variables referenced in code to be inserted (last in source sequence order)
     SgStatementPtrList::iterator furthestDeclarationInSourceSequence;

     printf ("In insertStatementUsingDependenceInformation(): currentScope is a %s \n",currentScope->sage_class_name());

  // Get a reference to the statement list out of the basic block
     SgBasicBlock* currentBlock = dynamic_cast<SgBasicBlock*>(currentScope);
     ROSE_ASSERT (currentBlock != NULL);

#if 0
     string unparsedDeclarationCodeString = (*transformationStatementIterator)->unparseToString();
     ROSE_ASSERT (unparsedDeclarationCodeString.c_str() != NULL);
     printf ("unparsedDeclarationCodeString = %s \n",unparsedDeclarationCodeString.c_str());
#endif

  // There should be at least one type in the statement
     ROSE_ASSERT (typeNameStringList.size() > 0);
  // printf ("typeNameStringList.size() = %zu \n",typeNameStringList.size());

  // printf ("This statement has a dependence upon a variable of some type \n");

  // Loop over all the types and get list of variables of each type
  // (so they can be declared properly when the transformation is compiled)
     Rose_STL_Container<string>::iterator typeListStringElementIterator;
     for (typeListStringElementIterator = typeNameStringList.begin();
          typeListStringElementIterator != typeNameStringList.end();
          typeListStringElementIterator++)
        {
       // printf ("Type = %s \n",(*typeListStringElementIterator).c_str());

       // Find a list of names of variable of type (*listStringElementIterator)
       // list<string> operandNameStringList =
       //      NameQuery::getVariableNamesWithTypeNameQuery
       //         ( *transformationStatementIterator, *typeListStringElementIterator );
       // list<string> operandNameStringList = NameQuery::getVariableNamesWithTypeNameQuery ( newTransformationStatement, *typeListStringElementIterator );
          Rose_STL_Container<string> operandNameStringList =
               NameQuery::querySubTree ( 
                    newTransformationStatement, 
                    *typeListStringElementIterator, 
                    NameQuery::VariableNamesWithTypeName );

       // There should be at least one variable of that type in the statement
          ROSE_ASSERT (operandNameStringList.size() > 0);
       // printf ("operandNameStringList.size() = %zu \n",operandNameStringList.size());

       // Loop over all the types and get list of variable of each type
          Rose_STL_Container<string>::iterator variableListStringElementIterator;
          for (variableListStringElementIterator = operandNameStringList.begin();
               variableListStringElementIterator != operandNameStringList.end();
               variableListStringElementIterator++)
             {
#if 0
               printf ("Type = %s Variable = %s \n",
                    (*typeListStringElementIterator).c_str(),
                    (*variableListStringElementIterator).c_str());
#endif
               string variableName = *variableListStringElementIterator;
               string typeName     = *typeListStringElementIterator;

               SgName name = variableName.c_str();
            // SgVariableSymbol* symbol = currentBlock->lookup_var_symbol(name);
               SgVariableSymbol* symbol = currentScope->lookup_var_symbol(name);
               if ( symbol != NULL )
                  {
                 // found a variable with name -- make sure that the declarations 
                 // represented by *transformationStatementIterator are inserted 
                 // after their declaration.
#if 0
                    printf ("Found a valid symbol corresponding to Type = %s Variable = %s (must be defined in the local scope) \n",
                         (*typeListStringElementIterator).c_str(),
                         (*variableListStringElementIterator).c_str());
#endif
                    ROSE_ASSERT (symbol != NULL);
                    SgInitializedName* declarationInitializedName = symbol->get_declaration();
                    ROSE_ASSERT (declarationInitializedName != NULL);
                    SgDeclarationStatement* declarationStatement  =
                         declarationInitializedName->get_declaration();
                    ROSE_ASSERT (declarationStatement != NULL);
#if 0
                    printf ("declarationStatementString located at line = %d of file = %s \n",
                         ROSE::getLineNumber(declarationStatement),
                         ROSE::getFileName(declarationStatement));
                    string declarationStatementString = declarationStatement->unparseToString();
                    printf ("declarationStatementString = %s \n",declarationStatementString.c_str());
#endif
                    statementCounter = 1;

                    SgStatementPtrList & currentStatementList = currentBlock->get_statements();

                    SgStatementPtrList::iterator i = currentStatementList.begin();
                    bool declarationFound = false;
                    while ( ( i != currentStatementList.end() ) && ( declarationFound == false ) )
                       {
                      // searching for the declarationStatement
#if 0
                         printf ("statementCounter = %d previousStatementCounter = %d \n",
                              statementCounter,previousStatementCounter);
                         string currentStatementString = (*i)->unparseToString();
                         printf ("currentStatementString = %s \n",currentStatementString.c_str());
#endif
                         if ( (*i == declarationStatement) &&
                              (statementCounter > previousStatementCounter) )
                            {
                           // printf ("Found the declarationStatement at position (statementCounter = %d previousStatementCounter = %d) \n",statementCounter,previousStatementCounter);
                              declarationFound = true;
                            }
                           else
                            {
                           // printf ("Not the declaration we are looking for! \n");
                              i++;
                              statementCounter++;
                            }
                       }

                 // Save a reference to the variable declaration that is furthest in
                 // the source sequence so that we can append the new statement just
                 // after it (so that variables referenced in the new statement will
                 // be defined).
                    if ( (statementCounter > previousStatementCounter) && ( declarationFound == true ) )
                       {
                         previousStatementCounter = statementCounter;
                         furthestDeclarationInSourceSequence = i;
                       }
#if 0
                    printf ("AFTER LOOP OVER STATEMENTS: previousStatementCounter = %d \n",previousStatementCounter);
                    string lastStatementString = (*furthestDeclarationInSourceSequence)->unparseToString();
                    printf ("lastStatementString = %s \n",lastStatementString.c_str());
#endif
                  }
                 else
                  {
                 // DQ: This case should never happen (I think)
                 // If the variable is not found then insert the new statement at the front of the list
                 // DQ (4/11/2003): This can happen if the transformation declarares the variable and uses it.
#if 1
                    printf ("Can NOT find a valid symbol corresponding to Type = %s Variable = %s (so it is not declared in the local scope) \n",
                         (*typeListStringElementIterator).c_str(),
                         (*variableListStringElementIterator).c_str());
//                  ROSE_ABORT();
#endif
                 // If it does happen then we can't just ignore the statement and forget to insert it!!!
//                  currentStatementList.push_front(*transformationStatementIterator);
                  }
#if 0
               printf ("BOTTOM OF LOOP OVER VARIABLES: previousStatementCounter = %d \n",previousStatementCounter);
#endif
             }

          if (statementCounter > previousStatementCounter)
               previousStatementCounter = statementCounter;

#if 0
          printf ("BOTTOM OF LOOP OVER TYPES: previousStatementCounter = %d \n",previousStatementCounter);
#endif
#if 0
          printf ("Exiting in insertSourceCode(): transformationStatementIterator loop (type = %s) ... \n",(*typeListStringElementIterator).c_str());
          ROSE_ABORT();
#endif
        }

#if 0
     printf ("Exiting in loop insertSourceCode (type = %s) ... \n",(*typeListStringElementIterator).c_str());
     ROSE_ABORT();
#endif
  // Now append the new statement AFTER the declaration that we have found
  // currentStatementList.insert(*targetDeclarationStatementIterator,*transformationStatementIterator);
  // currentStatementList.insert(lastStatementIterator,*transformationStatementIterator);
  // printf ("BEFORE ADDING NEW STATEMENT: previousStatementCounter = %d \n",previousStatementCounter);
     if (previousStatementCounter == 0)
        {
       // printf ("##### Prepend new statement to the top of the local scope \n");
       // currentStatementList.push_front(*transformationStatementIterator);
       // currentBlock->prepend_statement (*transformationStatementIterator);
          currentBlock->prepend_statement (newTransformationStatement);
        }
       else
        {
       // printf ("##### Append the new statement after the last position where a dependent variable is declared in the local scope \n");
       // Use new function added to append/prepend at a specified location in the list of statements
       // currentBlock->append_statement (furthestDeclarationInSourceSequence,*transformationStatementIterator);
          SgStatementPtrList::iterator furthestDeclarationInSourceSequence_copy = furthestDeclarationInSourceSequence;
          ++furthestDeclarationInSourceSequence_copy;
          currentBlock->get_statements().insert(furthestDeclarationInSourceSequence_copy, newTransformationStatement);
          newTransformationStatement->set_parent(currentBlock);
        }

#if 0
     printf ("Exiting at BASE of AST_Rewrite::insertStatementUsingDependenceInformation() \n");
     ROSE_ABORT();
#endif
   }


void
LowLevelRewrite::
insertBeforeCurrentStatement ( 
   const SgStatementPtrList & currentStatementList, 
   AttachedPreprocessingInfoType & commentsAndDirectives,
   SgStatement* astNode )
   {
     bool removeOriginalStatement = false;
     bool prependNewCode          = true;
     ROSE_ASSERT (currentStatementList.size() > 0 || commentsAndDirectives.size() > 0);
     ROSE_ASSERT (dynamic_cast<SgGlobal*>(astNode) == NULL);
     replaceAppendPrependTreeFragment (currentStatementList,commentsAndDirectives,astNode,removeOriginalStatement,prependNewCode);
   }

void
LowLevelRewrite::
insertReplaceCurrentStatement ( 
   const SgStatementPtrList & currentStatementList, 
   AttachedPreprocessingInfoType & commentsAndDirectives,
   SgStatement* astNode )
   {
     bool removeOriginalStatement = true;
     bool prependNewCode          = true;
     ROSE_ASSERT (currentStatementList.size() > 0 || commentsAndDirectives.size() > 0);

  // Can't replace the global scope (I think that it makes no sense to do so)
     ROSE_ASSERT (dynamic_cast<SgGlobal*>(astNode) == NULL);

     bool blockInSwitchStmt     = (isSgBasicBlock(astNode) != NULL) &&
                                  (isSgSwitchStatement(astNode->get_parent()) != NULL);
     bool blockInCaseOptionStmt = (isSgBasicBlock(astNode) != NULL) &&
                                  (isSgCaseOptionStmt(astNode->get_parent()) != NULL);
     bool specialHandling       = blockInSwitchStmt || blockInCaseOptionStmt;

     if (specialHandling == true)
        {
       // Special handling for replacement of SgBasicBlock where it forms the body 
       // of a certain types of statements (mostly SgScopeStatement objects).
          printf ("Handle special case \n");

          ROSE_ASSERT (currentStatementList.size() == 1);
          SgBasicBlock* newBody = isSgBasicBlock( *(currentStatementList.begin()) );
          ROSE_ASSERT (newBody != NULL);

          SgStatement* parentStatement = isSgStatement(astNode->get_parent());
          ROSE_ASSERT (parentStatement != NULL);
          printf ("parentStatement = %s \n",parentStatement->sage_class_name());

          switch (parentStatement->variantT())
             {
               case V_SgSwitchStatement:
                  {
                    SgSwitchStatement* switchStatement = isSgSwitchStatement(parentStatement);
                    switchStatement->set_body(newBody);
                    break;
                  }
               case V_SgCaseOptionStmt:
                  {
                    SgCaseOptionStmt* caseStatement = isSgCaseOptionStmt(parentStatement);
                    caseStatement->set_body(newBody);
                    break;
                  }
               default:
                   printf ("Error: default reached in switch \n");
                   ROSE_ASSERT (false);
             }

       // ROSE_ASSERT (false);
        }
       else
        {
       // This is the normal case
          replaceAppendPrependTreeFragment (currentStatementList,commentsAndDirectives,astNode,removeOriginalStatement,prependNewCode);
        }
   }

void
LowLevelRewrite::
insertAfterCurrentStatement ( 
   const SgStatementPtrList & currentStatementList, 
   AttachedPreprocessingInfoType & commentsAndDirectives,
   SgStatement* astNode )
   {
     bool removeOriginalStatement = false;
     bool prependNewCode          = false;
     ROSE_ASSERT (currentStatementList.size() > 0 || commentsAndDirectives.size() > 0);
     ROSE_ASSERT (dynamic_cast<SgGlobal*>(astNode) == NULL);
     replaceAppendPrependTreeFragment (currentStatementList,commentsAndDirectives,astNode,removeOriginalStatement,prependNewCode);
   }

void
LowLevelRewrite::
insertAtBottomOfScope ( 
   const SgStatementPtrList & newTransformationStatementList, 
   AttachedPreprocessingInfoType & commentsAndDirectives,
   SgScopeStatement* currentScope )
   {
  // Put the new statements at the end of the list (traverse the new statements from first to last)
  // But put it before any return statement! So find the last statement!

     bool atTop = false;

  // printf ("Insert new statements at the bottom of the block \n");
     SgStatementPtrList::const_iterator transformationStatementIterator;
     for (transformationStatementIterator = newTransformationStatementList.begin();
          transformationStatementIterator != newTransformationStatementList.end();
          transformationStatementIterator++)
        {
          if (currentScope->containsOnlyDeclarations() == true)
             {
            // Lists of declarations statements can't contain a return statement so we don't have to
            // test for it.
               currentScope->insertStatementInScope ( *transformationStatementIterator, atTop );
             }
            else
             {
            // If there is a RETURN_STMT in the block then insert the new statement just before the
            // existing RETURN_STMT
               SgStatementPtrList & currentStatementList          = currentScope->getStatementList();
               SgStatementPtrList::reverse_iterator lastStatement = currentStatementList.rbegin();
               if ( (*lastStatement)->variantT() == V_SgReturnStmt)
                  {
                    bool inFront = true;
                    currentScope->insert_statement ( *lastStatement, *transformationStatementIterator, inFront );
                  }
                 else
                  {
                    currentScope->insertStatementInScope ( *transformationStatementIterator, atTop );
                  }
             }
        }

  // Get last statement in scope
     SgStatement* lastStatementInScope = (currentScope->containsOnlyDeclarations() == true) ? 
                                          *(currentScope->getDeclarationList().rbegin()) :
                                          *(currentScope->getStatementList().rbegin());
     ROSE_ASSERT (lastStatementInScope != NULL);

     AttachedPreprocessingInfoType::iterator i;
     for (i = commentsAndDirectives.begin(); i != commentsAndDirectives.end(); i++)
        {
       // printf ("Relavant comment or directive attached to starting marker declaration = %s \n",(*i)->getString());
          ROSE_ASSERT ( (*i)->getRelativePosition() == PreprocessingInfo::before);

          if (lastStatementInScope->variantT() != V_SgReturnStmt)
               (*i)->setRelativePosition(PreprocessingInfo::after);
       // lastStatementInScope->addToAttachedPreprocessingInfo( *i , SgLocatedNode::appendPreprocessingInfoToList );
          lastStatementInScope->addToAttachedPreprocessingInfo( *i , PreprocessingInfo::after );
        }
   }


void
LowLevelRewrite::
replaceAppendPrependTreeFragment (
   const SgStatementPtrList & newTransformationStatementList,
   AttachedPreprocessingInfoType & commentsAndDirectives,
   SgStatement* astNode,
   bool removeOriginalStatement,
   bool prependNewCode )
   {
  // This function replaces or just appends the transformation to the astNode in the user's
  // application's AST with the fragment of the AST representing the transformation in the
  // AST_Fragment object.
  // printf ("Inside of AST_Fragment::replace() member function \n");

  // display ("Inside of AST_Fragment::replace");
  // printf ("Before the switch statement: targetNodeToReplace->sage_class_name = %s \n",
  //      targetNodeToReplace->sage_class_name());

  // Move any associated astNode around to preserve the semantics of new located node insertion
  // This is the one place where this function is called
     reassociatePreprocessorDeclarations (newTransformationStatementList,astNode,prependNewCode,removeOriginalStatement);

#if 0
     printf ("In replaceAppendPrependTreeFragment(): newTransformationStatementList.size() = %zu commentsAndDirectives.size() = %d \n",
          newTransformationStatementList.size(),commentsAndDirectives.size());
#endif

     if ( removeOriginalStatement && 
          newTransformationStatementList.size() == 0 && 
          commentsAndDirectives.size() == 0 )
        {
           printf ("ERROR: Replacement of statement with neither statement nor comment/directive is not allowed \n");
           ROSE_ABORT();
        }

  // Test for special cases of replacing a SgBasicBlock as part of compound statement
     SgStatement* currentStatement = isSgStatement(astNode);
     SgStatement* newStatement     = NULL;
     if (newTransformationStatementList.size() == 1)
        {
          newStatement = *(newTransformationStatementList.begin());
        }

#if 0
     printf ("In replaceAppendPrependTreeFragment(): current position in AST: astNode = %s \n",astNode->sage_class_name());
     if (newStatement != NULL)
          printf ("In replaceAppendPrependTreeFragment(): newStatement = %s \n",newStatement->sage_class_name());
#endif

  // It should be impossible for a SgFunctionDefinition to be generated from a string 
  // (though it could be generated explicitly). Verify that we don't have one for now.
     ROSE_ASSERT (isSgFunctionDefinition(newStatement) == NULL);

     SgBasicBlock*            newBlock               = isSgBasicBlock(newStatement);
     SgForInitStatement*      newForInitStatement    = isSgForInitStatement(newStatement);
     SgFunctionParameterList* newParameterList       = isSgFunctionParameterList(newStatement);
  // SgFunctionDefinition*    newFunctionDefinition  = isSgFunctionDefinition(newStatement);
  // SgClassDefinition*       newClassDefinition     = isSgClassDefinition(newStatement);

  // A SgFunctionDefinition is often represented by a SgFunctionDeclaration with it is generated from an input string
  // SgFunctionDeclaration*   newFunctionDeclaration = isSgFunctionDeclaration(newStatement);
     SgFunctionDeclaration*   newFunctionDeclaration = NULL;

     SgForInitStatement*      currentParentForInitStatement = isSgForInitStatement(currentStatement->get_parent());

  // If currently at a function definition then permit replacement of the new definition 
  // found within the new declaration, else avoid handling as a special case!
     if (isSgFunctionDefinition(currentStatement) != NULL)
          newFunctionDeclaration = isSgFunctionDeclaration(newStatement);

     bool foundRelavantTargetParentStatement = (currentParentForInitStatement != NULL);
     bool foundRelavantTargetStatement = 
          (newBlock != NULL) || 
          (newForInitStatement != NULL) || 
          (newParameterList != NULL) || 
          (newFunctionDeclaration != NULL) || 
       // (newFunctionDefinition != NULL) || 
       // (newClassDefinition != NULL) || 
          (foundRelavantTargetParentStatement == true);

#if 0
     printf ("foundRelavantTargetParentStatement = %s \n",foundRelavantTargetParentStatement ? "true" : "false");
     printf ("foundRelavantTargetStatement       = %s \n",foundRelavantTargetStatement       ? "true" : "false");
#endif

     bool processedReplacementOfTargetStatement = false;
     if ( (removeOriginalStatement == true) && (foundRelavantTargetStatement == true) )
        {
       // Handle special cases
          SgStatement* parentStatement = isSgStatement(currentStatement->get_parent());
       // printf ("Handle special case replacement: parentStatement = %s \n",parentStatement->sage_class_name());
          ROSE_ASSERT (parentStatement != NULL);
          switch (parentStatement->variantT())
             {
            // Case of replacing the body of a case statement
               case V_SgCaseOptionStmt:
                  {
                    SgCaseOptionStmt* caseStatement = isSgCaseOptionStmt(parentStatement);
                    caseStatement->set_body(newBlock);
                    processedReplacementOfTargetStatement = true;
                    break;
                  }

            // Case of replacing the body of a default statement
               case V_SgDefaultOptionStmt:
                  {
                    SgDefaultOptionStmt* defaultStatement = isSgDefaultOptionStmt(parentStatement);
                    defaultStatement->set_body(newBlock);
                    processedReplacementOfTargetStatement = true;
                    break;
                  }

            // Case of replacing the body of a for statement
               case V_SgForStatement:
                  {
                    SgForStatement* forStatement = isSgForStatement(parentStatement);
                    if (newBlock != NULL)
                       {
                         forStatement->set_loop_body(newBlock);
                       }
                      else
                       {
                      // forStatement->set_init_stmt(newForInitStatement);
                         printf ("Sorry, not implemented forStatement->set_init_stmt(currentForInitStatement) not called! \n");
                         ROSE_ASSERT (false);
                       }
                    processedReplacementOfTargetStatement = true;
                    break;
                  }

            // Case of replacing the body of a function definition or member function definition statement
               case V_SgFunctionDeclaration:
               case V_SgMemberFunctionDeclaration:
                  {
                    SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration(parentStatement);
                    if (newParameterList != NULL)
                       {
                      // functionDeclarationStatement->set_parameters(newParameterList);
                         printf ("Sorry, not implemented functionDeclarationStatement->set_parameters(newParameterList) not called! \n");
                         ROSE_ASSERT (false);
                       }

                    if (newFunctionDeclaration != NULL)
                       {
                      // If the declaration is specificed then get the definition, 
                      // since the body is located there.
                         SgFunctionDefinition* newFunctionDefinition = newFunctionDeclaration->get_definition();
                         ROSE_ASSERT (newFunctionDefinition != NULL);
                         functionDeclarationStatement->set_definition(newFunctionDefinition);
                       }

                    processedReplacementOfTargetStatement = true;
                    break;
                  }

            // Case of replacing the body of a default statement
               case V_SgFunctionDefinition:
                  {
                    SgFunctionDefinition* functionDefinitionStatement = isSgFunctionDefinition(parentStatement);
                    if (newBlock != NULL)
                       {
                         functionDefinitionStatement->set_body(newBlock);
                       }

                    processedReplacementOfTargetStatement = true;
                    break;
                  }

            // Case of replacing the body of a default statement
               case V_SgForInitStatement:
                  {
                    SgForInitStatement* forInitStatement = isSgForInitStatement(parentStatement);
                    SgStatementPtrList & list = forInitStatement->get_init_stmt();
                    SgStatementPtrList::iterator i = list.begin();
                    while ( (i != list.end()) && ((*i) != currentStatement) )
                       {
                         i++;
                       }
                    if (i != list.end())
                       {
                         list.insert(i,newStatement);

                      // DQ (9/25/2007): Moved from std::list to std::vector uniformally in ROSE
                      // list.remove(currentStatement);
                         list.erase(find(list.begin(),list.end(),currentStatement));
                       }
                      else
                       {
                         printf ("Error: V_SgForInitStatement case in low level rewrite (element not found) \n");
                         ROSE_ASSERT (false);
                       }

                    processedReplacementOfTargetStatement = true;
                    break;
                  }

               default:
                  {
                    printf ("Error: special cases of replacing a SgBasicBlock (default reached) \n");
                    ROSE_ASSERT (false);
                    break;
                  }
             }
        }

  // printf ("processedReplacementOfTargetStatement = %s \n",processedReplacementOfTargetStatement ? "true" : "false");
     if (processedReplacementOfTargetStatement == false)
        {
       // printf ("prependNewCode = %s \n",prependNewCode ? "true" : "false");

       // Handle the typical case not replacing a basic block
          if (prependNewCode == true)
             {
            // Insert at top of list (pull the elements off the bottom of the new statement list to get the order correct
            // printf ("Insert new statements before the target statement (newTransformationStatementList.size() = %d) \n",
            //      newTransformationStatementList.size());
               SgStatementPtrList::const_iterator transformationStatementIterator;
               for (transformationStatementIterator = newTransformationStatementList.begin();
                    transformationStatementIterator != newTransformationStatementList.end();
                    transformationStatementIterator++)
                  {
                    LowLevelRewrite::appendPrependTreeFragment (*transformationStatementIterator,astNode,prependNewCode);
                  }

            // Attach comments/directives
               AttachedPreprocessingInfoType::iterator i;
               for (i = commentsAndDirectives.begin(); i != commentsAndDirectives.end(); i++)
                  {
                 // printf ("Relavant comment or directive attached to starting marker declaration = %s \n",(*i)->getString());
                    ROSE_ASSERT ( (*i)->getRelativePosition() == PreprocessingInfo::before);
#if 0
                    Sg_File_Info *fileInfo = astNode->get_file_info();
                    ROSE_ASSERT (fileInfo != NULL);
                    printf ("fileInfo->get_filename() = %s \n",fileInfo->get_filename());
#endif
                 // astNode->addToAttachedPreprocessingInfo( *i , SgLocatedNode::appendPreprocessingInfoToList );
                    astNode->addToAttachedPreprocessingInfo( *i , PreprocessingInfo::after );
                  }

            // See what we have
            // printf ("See what we have: %s \n",astNode->unparseToCompleteString().c_str());
             }
            else
             {
            // Put the new statements at the end of the list (traverse the new statements from first to last)
            // printf ("Insert new statements after the target statement (newTransformationStatementList.size() = %d) \n",
            //      newTransformationStatementList.size());
               SgStatementPtrList::const_reverse_iterator transformationStatementIterator;
               for (transformationStatementIterator = newTransformationStatementList.rbegin();
                    transformationStatementIterator != newTransformationStatementList.rend();
                    transformationStatementIterator++)
                  {
                    LowLevelRewrite::appendPrependTreeFragment (*transformationStatementIterator,astNode,prependNewCode);
                  }

            // Attach comments/directives
               AttachedPreprocessingInfoType::iterator i;
               for (i = commentsAndDirectives.begin(); i != commentsAndDirectives.end(); i++)
                  {
                 // printf ("Relavant comment or directive attached to starting marker declaration = %s \n",(*i)->getString());

                 // Independent of how they appear relative to the nodes in the intermediate files 
                 // where they were first compiled, we want to place them after the current statement 
                 // (explicitly) so we have to change the relative position or find the next statement 
                 // to add the nodes to (which is problematic since the next statement is not 
                 // guarenteed to exist).
                    (*i)->setRelativePosition(PreprocessingInfo::after);

                    ROSE_ASSERT ( (*i)->getRelativePosition() == PreprocessingInfo::after);
                 // astNode->addToAttachedPreprocessingInfo( *i , SgLocatedNode::appendPreprocessingInfoToList );
                    astNode->addToAttachedPreprocessingInfo( *i , PreprocessingInfo::after );
                  }
             }

       // printf ("removeOriginalStatement = %s \n",removeOriginalStatement ? "true" : "false");
          if (removeOriginalStatement == true)
             {
               LowLevelRewrite::removeStatement(astNode);
             }
        }
   }

void
LowLevelRewrite::
reassociatePreprocessorDeclarations (
   const SgStatementPtrList & newTransformationStatementList,
   SgNode* astNode,
   bool prependNewCode,
   bool removeExistingNode )
   {
  // If a new located node is inserted before or after the input astNode, then we have to 
  // move any associated preprocessor declarations and comments to the statement before or 
  // after so that the order of preprocessor declarations and located nodes will be preserved 
  // and the semantics of inserting before or after preprocessor declarations (e.g. #include 
  // declarations). If the located node is to be removed then we have to first process its
  // preprocessor declarations so that they will not be lost (removed with the astNode when 
  // it is removed).

  // printf ("In LowLevelRewrite::reassociatePreprocessorDeclarations() astNode = %s \n",astNode->unparseToString().c_str());

  // printf ("Some aspects of the LowLevelRewrite::reassociatePreprocessorDeclarations() function may be redundent! \n");

  // printf ("newTransformationStatementList.size() = %zu \n",newTransformationStatementList.size());

     if (newTransformationStatementList.size() == 0)
        {
       // printf ("Exiting because newTransformationStatementList.size() == 0 \n");
          return;
        }

  // printf ("astNode->sage_class_name() = %s \n",astNode->sage_class_name());
  // printf ("astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());

  // Comments/directives might be attached to the basic block associated with a function definition
  // so we call reassociatePreprocessorDeclarations() recursively to handle this
     switch (astNode->variantT())
        {
          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
               ROSE_ASSERT (functionDeclaration != NULL);
               SgFunctionDefinition* functionDefinition   = functionDeclaration->get_definition();
               if (functionDefinition != NULL)
                    reassociatePreprocessorDeclarations (newTransformationStatementList,functionDefinition,prependNewCode,removeExistingNode);
               break;
             }

          case V_SgFunctionDefinition:
             {
               SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(astNode);
               ROSE_ASSERT (functionDefinition != NULL);
               SgBasicBlock* basicBlock = functionDefinition->get_body();
               ROSE_ASSERT (basicBlock != NULL);
               reassociatePreprocessorDeclarations (newTransformationStatementList,basicBlock,prependNewCode,removeExistingNode);
               break;
             }

          default:
             {
            // Cases not handled yet, comments/directives may be attached to these 
            // or the SgBasicBlocks that they contain!
               ROSE_ASSERT (astNode->variantT() != V_SgTemplateInstantiationDecl);
               ROSE_ASSERT (astNode->variantT() != V_SgEnumDeclaration);
               ROSE_ASSERT (astNode->variantT() != V_SgTemplateDeclaration);
               ROSE_ASSERT (astNode->variantT() != V_SgTypedefDeclaration);
               ROSE_ASSERT (astNode->variantT() != V_SgVariableDefinition);

            // DQ (11/23/2003): commented out case of SgClassDeclaration
            // ROSE_ASSERT (astNode->variantT() != V_SgClassDeclaration);

            // Comments/directives attached to the bottom of a variable declaration are accessible directly
            // ROSE_ASSERT (astNode->variantT() != V_SgVariableDeclaration);
               break;
             }
        }

  // Check for zero length list
     ROSE_ASSERT (newTransformationStatementList.size() > 0);

     SgLocatedNode* locatedNode = dynamic_cast<SgLocatedNode*>(astNode);

  // I think we can assert this for now (perhaps not later if we permit insertion of non located nodes)
     ROSE_ASSERT (locatedNode != NULL);

  // We can't remove the elements of the list while we are traversing the list so 
  // we have to save the references to the elements to be removed and remove them 
  // after traversing the list.
     AttachedPreprocessingInfoType preprocessorDeclarationsToRemove;

  // Get attached preprocessing info
     AttachedPreprocessingInfoType *comments = locatedNode->getAttachedPreprocessingInfo();

     SgStatementPtrList::const_iterator firstTransformationStatementIterator;
     SgStatementPtrList::const_reverse_iterator lastTransformationStatementIterator;
     firstTransformationStatementIterator = newTransformationStatementList.begin();
     lastTransformationStatementIterator  = newTransformationStatementList.rbegin();

#if 0
     printf ("*firstTransformationStatementIterator = %s \n",
          (*firstTransformationStatementIterator)->unparseToString().c_str());
     printf ("*lastTransformationStatementIterator  = %s \n",
          (*lastTransformationStatementIterator)->unparseToString().c_str());
#endif

     if (comments != NULL)
        {
       // We need to move comments that are associated with appearing before this 
       // node to the previous node and comments appearing after this node to the 
       // next node.

       // printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
       // Forward iterator through comment list can only be used to processing 
       // certain types:
       //      1) if removeExistingNode == true , (*i)->relativePosition == PreprocessingInfo::after
       //      2) if removeExistingNode == false, !prependNewCode && (*i)->relativePosition == PreprocessingInfo::after

          AttachedPreprocessingInfoType::iterator forwardIterator;
          for (forwardIterator = comments->begin(); forwardIterator != comments->end(); forwardIterator++)
             {
               ROSE_ASSERT ( (*forwardIterator) != NULL);
#if 0
               printf ("          Attached Comment (relativePosition=%s): %s",
                    ((*forwardIterator)->relativePosition == PreprocessingInfo::before) ? "before" : "after",
                    (*forwardIterator)->getString());
#endif

            // Check for replacement (removal of this statement)
            // printf ("removeExistingNode = %s \n",removeExistingNode ? "true" : "false");
               if (removeExistingNode == true)
                  {
                 // Replacement case must copy all comments and spearate them between remaining neighboring statements

                    ROSE_ASSERT ( (*forwardIterator)->getRelativePosition() == PreprocessingInfo::before ||
                                  (*forwardIterator)->getRelativePosition() == PreprocessingInfo::after);

                 // Split all the preprocessor information between the first and 
                 // last statement in the new list of statements to be added.
                    if ( (*forwardIterator)->getRelativePosition() == PreprocessingInfo::after )
                       {
                      // Insert these after the last statement
                      // printf ("Insert these after the last statement \n");
                      // (*lastTransformationStatementIterator)->addToAttachedPreprocessingInfo(*forwardIterator,SgLocatedNode::appendPreprocessingInfoToList);
                         (*lastTransformationStatementIterator)->addToAttachedPreprocessingInfo(*forwardIterator,PreprocessingInfo::after);
                       }

                 // Accumulate a list of pointers to preprocessor information which we will later remove
                 // (STL does not permit us to remove elements from a list being traversed).
                    preprocessorDeclarationsToRemove.push_back(*forwardIterator);
                  }
                 else
                  {
                    if (!prependNewCode && (*forwardIterator)->getRelativePosition() == PreprocessingInfo::after)
                       {
                      // Insert these before the next statement
                      // printf ("Inner case: Insert these before the next statement \n");
                      // (*lastTransformationStatementIterator)->addToAttachedPreprocessingInfo(*forwardIterator,SgLocatedNode::appendPreprocessingInfoToList);
                         (*lastTransformationStatementIterator)->addToAttachedPreprocessingInfo(*forwardIterator,PreprocessingInfo::after);
                         preprocessorDeclarationsToRemove.push_back(*forwardIterator);
                       }
                  }
             }

       // Reverse iterator through comment list can only be used to processing 
       // certain types:
       //      1) if removeExistingNode == true , (*i)->relativePosition == PreprocessingInfo::before
       //      2) if removeExistingNode == false, prependNewCode && (*i)->relativePosition == PreprocessingInfo::before

          AttachedPreprocessingInfoType::reverse_iterator reverseIterator;
          for (reverseIterator = comments->rbegin(); reverseIterator != comments->rend(); reverseIterator++)
             {
               ROSE_ASSERT ( (*reverseIterator) != NULL);
#if 0
               printf ("          Attached Comment (relativePosition=%s): %s",
                    ((*reverseIterator)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*reverseIterator)->getString());
#endif
            // Check for replacement (removal of this statement)
            // printf ("removeExistingNode = %s \n",removeExistingNode ? "true" : "false");
               if (removeExistingNode == true)
                  {
                 // Replacement case must copy all comments and spearate them between remaining neighboring statements
                    ROSE_ASSERT ( (*reverseIterator)->getRelativePosition() == PreprocessingInfo::before ||
                                  (*reverseIterator)->getRelativePosition() == PreprocessingInfo::after);

                 // Split all the preprocessor information between the first and 
                 // last statement in the new list of statements to be added.
                    if ( (*reverseIterator)->getRelativePosition() == PreprocessingInfo::before )
                       {
                      // Insert these before the first statement
                      // printf ("Insert these before the first statement \n");
                     // (*firstTransformationStatementIterator)->addToAttachedPreprocessingInfo(*reverseIterator,SgLocatedNode::prependPreprocessingInfoToList);
                         (*firstTransformationStatementIterator)->addToAttachedPreprocessingInfo(*reverseIterator,PreprocessingInfo::before);
                       }

                 // Accumulate a list of pointers to preprocessor information which we will later remove
                 // (STL does not permit us to remove elements from a list being traversed).
                    preprocessorDeclarationsToRemove.push_back(*reverseIterator);
                  }
                 else
                  {
                    if (prependNewCode && (*reverseIterator)->getRelativePosition() == PreprocessingInfo::before)
                       {
                      // Insert these after the previous statement
                      // printf ("Insert these after the previous statement \n");

                      // reattach the comment on the current node to the end of
                      // the list of "before" elements on the new statement.
                      // (*firstTransformationStatementIterator)->addToAttachedPreprocessingInfo(*reverseIterator,SgLocatedNode::prependPreprocessingInfoToList);
                         (*firstTransformationStatementIterator)->addToAttachedPreprocessingInfo(*reverseIterator,PreprocessingInfo::before);
                         preprocessorDeclarationsToRemove.push_back(*reverseIterator);
                       }
                  }
             }

       // Now remove the list of accumulated references in to preprocessor information
       // We could delete the list but it does not harm to have an empty
       // list and there is not interface to allow us to remove the list.
          AttachedPreprocessingInfoType::iterator j;
          for (j = preprocessorDeclarationsToRemove.begin(); j != preprocessorDeclarationsToRemove.end(); j++)
             {
#if 0
               printf ("          Removing Comment (relativePosition=%s): %s",
                    ((*j)->relativePosition == PreprocessingInfo::before) ? "before" : "after",(*j)->getString());
#endif
            // Remove the element from the list of comments at the current astNode
            // comments->remove( (*j) );
               comments->erase(find(comments->begin(),comments->end(),*j));
             }
        }
       else
        {
       // printf ("In reassociatePreprocessorDeclarations(): No comments found! \n");
        }
   }



void
LowLevelRewrite::
markForOutputInCodeGenerationForRewrite ( SgStatement* newTransformationStatement )
   {
     string originatingFileName = newTransformationStatement->get_file_info()->get_raw_filename();
     string stripedFileName = StringUtility::stripPathFromFileName(originatingFileName);
     string targetSubstring = "rose_transformation_";
     string fileNamePrefix;
     if (stripedFileName.size() > targetSubstring.size())
          fileNamePrefix = stripedFileName.substr(0,targetSubstring.size());
     if (fileNamePrefix == targetSubstring)
        {
       // printf ("Marking this for output in transformed source code (found in a <%s> file) file = %s \n",targetSubstring.c_str(),originatingFileName.c_str());
          newTransformationStatement->get_file_info()->setOutputInCodeGeneration();
          ROSE_ASSERT(newTransformationStatement->get_file_info()->isOutputInCodeGeneration() == true);
        }
       else
        {
       // printf ("Skip marking this for output in transformed source code (not from a <%s> file) file = %s \n",targetSubstring.c_str(),originatingFileName.c_str());

       // DQ (3/25/2006): This is an interesting test
          if ( newTransformationStatement->get_file_info()->isOutputInCodeGeneration() == true )
             {
               newTransformationStatement->get_file_info()->display("Error: newTransformationStatement: debug");
             }
      //CI    ROSE_ASSERT(newTransformationStatement->get_file_info()->isOutputInCodeGeneration() == false);
        }
   }


void
LowLevelRewrite::
appendPrependTreeFragment (
   SgStatement* newTransformationStatement,
   SgStatement* astNode,
   bool prependNewCode )
   {
#if 1

  // DQ (11/7/2003): Call Qing's implementation of insert_statement written at the SAGE III level

  // This function only supports the removal of a whole statement (not an expression within a statement)
     SgStatement* targetStatement = isSgStatement(astNode);
     ROSE_ASSERT (targetStatement != NULL);

#if 0
  // DQ (12/5/2004): Check if this is a node where the scope is stored explicitly
     if (newTransformationStatement->storesExplicitScope() == true)
        {
       // Now find the correct scope (using names and symbol tables!)

        }
#else
  // Initially lets derive the scope from the parent (then we will do a better job!)
     SgScopeStatement* parentScope = isSgScopeStatement(astNode->get_parent());
     if (parentScope == NULL)
          parentScope = astNode->get_scope();
     ROSE_ASSERT(parentScope != NULL);
     if (newTransformationStatement->hasExplicitScope() == true)
        {
          newTransformationStatement->set_scope(parentScope);
        }
#endif

     SgStatement * parentStatement = isSgStatement(targetStatement->get_parent());
  // printf ("Parent statement in LowLevelRewrite::appendPrependTreeFragment() = %s \n",parentStatement->class_name().c_str());
     if (parentStatement)
        {
       // DQ (1/11/2006): Need to make the new sttement as a transformation, but not always to be output!
       // We need to change the filename associated with the new statement to be inserted into the 
       // application's AST so that the unparser will correctly unparse it to the same file as the 
       // rest of the application.
       // char* filenameOfTargetStatement = targetStatement->get_file_info()->get_filename();
       // newTransformationStatement->get_file_info()->set_filename(filenameOfTargetStatement);
          newTransformationStatement->get_file_info()->setTransformation();
       // int fileIdOfTargetStatement = targetStatement->get_file_info()->get_file_id();
       // int fileIdOfNewStatement    = newTransformationStatement->get_file_info()->get_file_id();
       // if (fileIdOfTargetStatement == fileIdOfNewStatement)

       // DQ (3/25/2006): If this is from a header file and not in a rose_transformation_<original file name>.C 
       // then we should not mark it for output!
       // DQ (3/9/2006): Add that this should be output as well!
       // newTransformationStatement->get_file_info()->setOutputInCodeGeneration();
          markForOutputInCodeGenerationForRewrite (newTransformationStatement);
#if 0
          printf ("In LowLevelRewrite::appendPrependTreeFragment Statement to be inserted into AST = %s = %s \n",
               newTransformationStatement->class_name().c_str(),SageInterface::get_name(newTransformationStatement).c_str());
          newTransformationStatement->get_file_info()->display("In LowLevelRewrite (before insertion into AST): debug");
#endif
          parentStatement->insert_statement(targetStatement,newTransformationStatement,prependNewCode);
#if 0
          if (SageInterface::get_name(newTransformationStatement) == "TauTimer")
             {
               newTransformationStatement->get_file_info()->display("In LowLevelRewrite (after insertion into AST): debug");
               printf ("Exiting internally for case of class name == TauTimer \n");
               ROSE_ASSERT(false);
             }
#endif
        }
#else

     printf ("NOT USING QING'S SAGE III rewrite interface! \n");

  // This function should likely be moved into SgNode (where it was originally located)

  // error checking
     ROSE_ASSERT (newTransformationStatement != NULL);
     ROSE_ASSERT (astNode != NULL);
     ROSE_ASSERT (prependNewCode == true || prependNewCode == false);      

  // Rename variable internally yo make code more clear
     SgNode* targetNodeInExistingAST = astNode;

#if 0
     printf ("In LowLevelRewrite::appendPrependTreeFragment(): astNode = %s newTransformationStatement = %s prependNewCode = %s \n",astNode->sage_class_name(),newTransformationStatement->sage_class_name(),(prependNewCode == true) ? "true" : "false");
     printf ("newTransformationStatement = %s \n",newTransformationStatement->unparseToString().c_str());
#endif

  // Error checking: the target for append of prepent should not be a SgGlobal object!
     if (dynamic_cast<SgGlobal*>(targetNodeInExistingAST) != NULL)
        {
       // This is an unacceptable case.  We can't append or prepend something to the global scope!
          printf ("ERROR: attempt made to append/prepend/replace new transformation to the global scope! (not allowed). \n");
          ROSE_ABORT();
        }
     ROSE_ASSERT (dynamic_cast<SgGlobal*>(targetNodeInExistingAST) == NULL);

  // Make sure this is not an expression (can't handle anything but statements currently)
     ROSE_ASSERT ( isSgExpression(targetNodeInExistingAST) == NULL );
     ROSE_ASSERT ( isSgStatement (targetNodeInExistingAST) != NULL );

  // The target AST node to prepend/replace/append is a Statement 
  // (the easier case that we want to get working first).
  // printf ("Statement to prepend/replace/append is a %s \n",targetNodeToReplace->sage_class_name());
     SgStatement* targetStatement = isSgStatement(targetNodeInExistingAST);
     ROSE_ASSERT (targetStatement != NULL);

  // printf ("targetStatement->sage_class_name = %s \n",targetStatement->sage_class_name());

     ROSE_ASSERT (targetStatement->get_file_info() != NULL);
     ROSE_ASSERT (targetStatement->get_file_info()->get_filename() != NULL);

  // char* filenameOfTargetStatement = ROSE::getFileName(targetStatement);
     char* filenameOfTargetStatement = targetStatement->get_file_info()->get_filename();
     ROSE_ASSERT (filenameOfTargetStatement != NULL);

     printf ("Statement to replace is from file: %s \n",filenameOfTargetStatement);

  // SgStatement* parentStatement = isSgStatement(targetStatement->get_parent());
  // SgStatement* parentStatement = isSgGlobal(targetStatement->get_parent());
  // ROSE_ASSERT (parentStatement != NULL);

  // error checking
     ROSE_ASSERT (targetStatement->get_parent() != NULL);
     ROSE_ASSERT (isSgStatement(targetStatement->get_parent()) != NULL);

  // Error checking
     if ( (isSgGlobal(targetStatement->get_parent()) == NULL) &&
          (isSgClassDefinition(targetStatement->get_parent()) == NULL) &&
          (isSgBasicBlock(targetStatement->get_parent()) == NULL) )
        {
       // It doesn't make sense to append or prepend statements if the targetStatement is not in
       // either a basic block or global scope.
#if 1
          printf ("targetStatement->sage_class_name = %s \n",targetStatement->sage_class_name());
          printf ("targetStatement->get_parent()->sage_class_name = %s \n",
               targetStatement->get_parent()->sage_class_name());
#endif
          printf ("ERROR: targetStatement for append/prepend must be located in either a basic block or global scoope to make sense! \n");
          ROSE_ABORT();
        }

  // printf ("In appendPrependStatement(): Source string = %s \n",newTransformationStatement->unparseToString().c_str());
  // cout << "cout In appendPrependStatement(): Source string = " << newTransformationStatement->unparseToString() << endl;

  // The real difference between these two branches is that one has to handle 
  // declarationsStatements and the other is more general to all statements.
  // There has to be two of them because the STL details are different for 
  // each of them. I could not find a way around this problem (without using
  // function templates which is not portable).

#if 1
     if (isSgGlobal(targetStatement->get_parent()) != NULL)
#else
     SgStatement* parentOfTargetStatement = targetStatement->get_parent();
     switch (parentOfTargetStatement->variantT())
        {
          case V_SgGlobal:
          case V_SgClassDefinition:
#endif
        {
       // printf ("Target statement is in the global scope! \n");

       // SgStatement* parentStatement = isSgGlobal(targetStatement->get_parent());
       // ROSE_ASSERT (parentStatement != NULL);

       // Handle the case when the target statement is in a list of declaration statements
       // This is a separate case because the statement list is a list of declarations 
       // The difference in the type forces the STL list to be different and this forces 
       // us to handle this as a separate case.

#if 1
          SgGlobal* targetStatementParent = isSgGlobal(targetStatement->get_parent());
          ROSE_ASSERT ( targetStatementParent != NULL );
       // printf ("targetStatementParent->sage_class_name() = %s \n",targetStatementParent->sage_class_name());
       // SgStatementPtrList & list = isSgGlobal(targetStatement->get_parent())->get_declarations();
          SgDeclarationStatementPtrList & parentStatementList = targetStatementParent->get_declarations();
#else
          SgScopeStatement* targetStatementParent = NULL;
          SgGlobal* targetStatementParentGlobal                   = isSgGlobal(targetStatement->get_parent());
          SgClassDefinition* targetStatementParentClassDefinition = isSgClassDefinition(targetStatement->get_parent());

          SgDeclarationStatementPtrList* parentStatementListPointer = NULL;
          if (targetStatementParentGlobal != NULL)
               parentStatementListPointer = &(targetStatementParentGlobal->get_declarations());
          if (targetStatementParentClassDefinition != NULL)
               parentStatementListPointer = &(targetStatementParentClassDefinition->get_members());
          ROSE_ASSERT ( parentStatementListPointer != NULL );
          SgDeclarationStatementPtrList & parentStatementList = *parentStatementListPointer;
#endif

       // printf ("In appendPrependTreeFragment(): parentStatementList.size() = %zu \n",parentStatementList.size());

#if 0
#ifdef SAGE_INTERNAL_DEBUG
       // error checking
          if ( SAGE_DEBUG > -1 )
             {
               SgDeclarationStatementPtrList::iterator statementIterator;
               printf ("BEFORE insertion of new declaration: targetStatementParent list.size() = %zu statements are: \n",parentStatementList.size());
               int counter = 0;
               for ( statementIterator = parentStatementList.begin(); 
                     statementIterator != parentStatementList.end();
                     statementIterator++ )
                  {
                 // find the target node within the list of statements contained in the parent's statement list
                 // printf ("(*statementIterator) = %p \n",(*statementIterator));
                    ROSE_ASSERT ( (*statementIterator) != NULL );
                    ROSE_ASSERT ( (*statementIterator)->get_file_info() != NULL );
                    printf ("     Global Scope declaration #%d is a (*statementIterator)->sage_class_name() = %s from file = %s \n",
                         counter,(*statementIterator)->sage_class_name(),
                         (*statementIterator)->get_file_info()->get_filename() );

                    SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration(*statementIterator);
                    if (functionDeclarationStatement != NULL)
                         printf("        function name = %s \n",functionDeclarationStatement->get_name().str() );

                    SgVariableDeclaration* variableDeclarationStatement = isSgVariableDeclaration(*statementIterator);
                    if (variableDeclarationStatement != NULL)
                         printf("        variable name = %s \n","not implented");
                    counter++;
                  }
            // printf ("After test of list elements \n");
             }
#endif
#endif

       // The input statement needs to be converted to a declaration statement
       // (which it should be if it goes in the global scope)
          SgDeclarationStatement* newDeclarationStatement = isSgDeclarationStatement( newTransformationStatement);
          ROSE_ASSERT (newDeclarationStatement != NULL);

       // Rather than looping through the parent scope we could have used the STL find 
       // function to build the iterator required by the insert_declaration member function
          SgDeclarationStatementPtrList::iterator parentStatementIterator;
          for (parentStatementIterator = parentStatementList.begin();
               parentStatementIterator != parentStatementList.end();
               parentStatementIterator++)
             {
               ROSE_ASSERT ((*parentStatementIterator)->get_file_info() != NULL);
               ROSE_ASSERT ((*parentStatementIterator)->get_file_info()->get_filename() != NULL);

               if (*parentStatementIterator == targetNodeInExistingAST )
                  {
                 // printf ("Found \"this\" in parent scope (declarations in global scope) \n");
                 // ROSE_ABORT();

                 // We need to change the filename associated with the new statement to be inserted into the 
                 // application's AST so that the unparser will correctly unparse it to be the same file as the 
                 // rest of the application.
                 // printf ("Resetting the filename in rewriteLowLevelInterface(global of class declaration scope): (original filename = %s new filename = %s) \n",
                 //      newDeclarationStatement->get_file_info()->get_filename(),filenameOfTargetStatement);
                    newDeclarationStatement->get_file_info()->set_filename(filenameOfTargetStatement);

                    if ( prependNewCode == true )
                       {
                      // printf ("Inserting the new declaration into the global scope ... \n");
                      // printf ("   New declaration is a newDeclarationStatement->sage_class_name() = %s \n",
                      //      newDeclarationStatement->sage_class_name());
                         targetStatementParent->insert_declaration ( parentStatementIterator, newDeclarationStatement );
                       }
                      else
                       {
#if 0
                      // Use new function added to append/prepend at a specified location in the list of statements
                         currentBlock->append_statement (parentStatementIterator,newDeclarationStatement);
#else
                         SgDeclarationStatementPtrList::iterator tempIterator = parentStatementIterator;
                         tempIterator++;
                      // Handle the case of appending at the end of the list
                         if ( tempIterator == parentStatementList.end() )
                            {
                              targetStatementParent->append_declaration (newDeclarationStatement);
                            }
                           else
                            {
                              targetStatementParent->insert_declaration (tempIterator,newDeclarationStatement);
                            }
#endif
                       }
                  }
             }

       // printf ("In appendPrependTreeFragment(): parentStatementList.size() = %zu \n",parentStatementList.size());

#if 0
#ifdef SAGE_INTERNAL_DEBUG
       // error checking
          if ( SAGE_DEBUG > -1 )
             {
               SgDeclarationStatementPtrList::iterator statementIterator;
               printf ("AFTER insertion of new declaration: targetStatementParent list.size() = %zu statements are: \n",parentStatementList.size());
               int counter = 0;
               for ( statementIterator = parentStatementList.begin(); 
                     statementIterator != parentStatementList.end();
                     statementIterator++ )
                  {
                 // find the target node within the list of statements contained in the parent's statement list
                 // printf ("(*statementIterator) = %p \n",(*statementIterator));
                    ROSE_ASSERT ( (*statementIterator) != NULL );
                    ROSE_ASSERT ( (*statementIterator)->get_file_info() != NULL );

                    printf ("     Global Scope declaration #%d is a (*statementIterator)->sage_class_name() = %s from file = %s \n",
                         counter,(*statementIterator)->sage_class_name(),
                         (*statementIterator)->get_file_info()->get_filename() );

                    SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration(*statementIterator);
                    if (functionDeclarationStatement != NULL)
                         printf("        function name = %s \n",functionDeclarationStatement->get_name().str() );

                    SgVariableDeclaration* variableDeclarationStatement = isSgVariableDeclaration(*statementIterator);
                    if (variableDeclarationStatement != NULL)
                         printf("        variable name = %s \n","not implented");
                    counter++;
                  }
            // printf ("After test of list elements \n");
             }
#endif
#endif

       // printf ("After insertion of new list elements \n");
        }
       else
        {
       // printf ("Case of non-global scope \n");

       // printf ("This case works, but we are debugging the other case! \n");
       // ROSE_ABORT();

       // not in global scope
          ROSE_ASSERT ( targetStatement != NULL );
          ROSE_ASSERT ( targetStatement->get_parent() != NULL );
          ROSE_ASSERT ( isSgGlobal(targetStatement->get_parent()) == NULL );

       // The target statement sets inside of a basic block (I don't think there is any other possibility)
       // printf ("targetStatement->sage_class_name() = %s \n",targetStatement->sage_class_name());
       // printf ("targetStatement->get_parent()->sage_class_name() = %s \n",targetStatement->get_parent()->sage_class_name());
          ROSE_ASSERT ( isSgBasicBlock(targetStatement->get_parent()) != NULL );

       // The input statement needs to be converted to a declaration statement
       // (which it should be if it goes in the global scope)
          SgStatement* newStatement = isSgStatement( newTransformationStatement);
          ROSE_ASSERT (newStatement != NULL);

          SgBasicBlock* parentStatementBasicBlock = isSgBasicBlock(targetStatement->get_parent());
          ROSE_ASSERT ( parentStatementBasicBlock != NULL );
          SgStatementPtrList & parentStatementList = parentStatementBasicBlock->get_statements();

       // printf ("In appendPrependTreeFragment(): parentStatementList.size() = %zu \n",parentStatementList.size());

       // Rather than looping through the parent scope we could have used the STL find 
       // function to build the iterator required by the insert_declaration member function
          SgStatementPtrList::iterator parentStatementIterator;
          for (parentStatementIterator = parentStatementList.begin();
               parentStatementIterator != parentStatementList.end();
               parentStatementIterator++)
             {
               ROSE_ASSERT ((*parentStatementIterator)->get_file_info() != NULL);
               ROSE_ASSERT ((*parentStatementIterator)->get_file_info()->get_filename() != NULL);

               if (*parentStatementIterator == targetNodeInExistingAST )
                  {
                 // printf ("Found \"this\" in parent scope (declarations in global scope) \n");
                 // ROSE_ABORT();

                 // We need to change the filename associated with the new statement to be inserted into the 
                 // application's AST so that the unparser will correctly unparse it to the same file as the 
                 // rest of the application.
                 // printf ("Resetting the filename in rewriteLowLevelInterface(non-global scope): (original filename = %s new filename = %s) \n",
                 //      newDeclarationStatement->get_file_info()->get_filename(),filenameOfTargetStatement);
                    newStatement->get_file_info()->set_filename(filenameOfTargetStatement);

                    if ( prependNewCode == true )
                       {
#if 0
                         printf ("Inserting the new statement into the basic block ... \n");
                         printf ("   New statement is a newStatement->sage_class_name() = %s \n",
                              newStatement->sage_class_name());
#endif
                         parentStatementBasicBlock->insert_statement ( parentStatementIterator, newStatement );
                       }
                      else
                       {
#if 1
                      // Use new function added to append/prepend at a specified location in the list of statements
                         parentStatementBasicBlock->append_statement (parentStatementIterator,newStatement);
#else
                         SgStatementPtrList::iterator tempIterator = parentStatementIterator;
                         tempIterator++;
                      // Handle the case of appending at the end of the list
                         if ( tempIterator == parentStatementList.end() )
                            {
                              parentStatementBasicBlock->append_statement (newStatement);
                            }
                           else
                            {
                              parentStatementBasicBlock->insert_statement (tempIterator,newStatement);
                            }
#endif
                       }
                  }
             }

       // printf ("After insertion of new list elements \n");
       // printf ("In appendPrependTreeFragment(): parentStatementList.size() = %zu \n",parentStatementList.size());
        }
#endif
   }

//! This function qualifies a statement as to if it can be removed from the AST or not.
//! Not all statements can be removed, e.g. the SgBasicBlock in a SgForStatement or similar
//! statement containing a SgBasicBlock to represent a body.  Only a statement in
//! an AST node representing a list can be removed (I don't know of any exceptions).
 //! By definition, the statement is removeable if it's parent is a container.
bool
LowLevelRewrite::
isRemovableStatement ( SgStatement* s )
   {
  // This code is based on the logic defined in the following function
  // bool AstTests::isProblematic(SgNode* node)
  //      { return numSuccContainers(node)>1 || (numSuccContainers(node)>0 && numSingleSuccs(node)>0); }

     ROSE_ASSERT(s != NULL);
     SgStatement* parentNode = isSgStatement(s->get_parent());
     ROSE_ASSERT(parentNode != NULL);

     bool isContainer    = (AstTests::numSuccContainers(parentNode) == 1);
     bool isNonContainer = ( (AstTests::numSuccContainers(parentNode) == 0) && (AstTests::numSingleSuccs(parentNode) > 0) );

     ROSE_ASSERT (isContainer == !isNonContainer);

     return isContainer;
   }
 
void
LowLevelRewrite::
removeStatement ( SgStatement* astNode )
   {
  // DQ (11/7/2003): Call Qing's implementation of remove_statement written at the SAGE III level

  // This function only supports the removal of a whole statement (not an expression within a statement)
     SgStatement* targetStatement = isSgStatement(astNode);
     ROSE_ASSERT (targetStatement != NULL);

     SgStatement * parentStatement = isSgStatement(targetStatement->get_parent());

  // Can't assert this since SgFile is the parent of SgGlobal, and SgFile is not a statement.
  // Even so SgGlobal can't be removed from SgFile, but isRemovableStatement() takes a SgStatement.
  // ROSE_ASSERT (parentStatement != NULL);

     if (parentStatement != NULL)
        {
          bool isRemovable = isRemovableStatement(targetStatement);
#if 0
          printf ("In parentStatement = %s remove targetStatement = %s (isRemovable = %s) \n",
               parentStatement->sage_class_name(),
               targetStatement->sage_class_name(),
               isRemovable ? "true" : "false");
#endif

          if ( isRemovable == true )
               parentStatement->remove_statement(targetStatement);
        }
   }







