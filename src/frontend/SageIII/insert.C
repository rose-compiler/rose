
// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "rose.h"

// We need this header file so that we can use the global tree traversal mechanism
// #include "Cxx_GrammarTreeTraversalClass.h"

// We need this header file so that we can build the array statement specific transformations
// #include "arrayStatementTransformation.h"

// Main transfromation mechanism
// #include "programTransformation.h"

#include "nameQuery.h"

// Temp code placed here while we debug it

#if 0
void
SgNode::insertSourceCode ( SgProject & project,
                           string sourceCodeString,
                           string localDeclaration,
                           string globalDeclaration,
                           bool locateNewCodeAtTop,
                           bool isADeclaration )
   {
     insertSourceCode (
          project, sourceCodeString.c_str(), localDeclaration.c_str(), globalDeclaration.c_str(),
          locateNewCodeAtTop, isADeclaration );
   }
#endif

#if 0
void
SgNode::insertSourceCode ( SgProject & project,
                           const char* sourceCodeString,
                           const char* localDeclaration,
                           const char* globalDeclaration,
                           bool locateNewCodeAtTop,
                           bool isADeclaration )
   {
  // If this function is useful only for SgBasicBlock then it should be put into that class directly.
  // This function is used to insert code into AST object for which insertion make sense:
  // (specifically any BASIC_BLOCK_STMT)

     if (variant() != BASIC_BLOCK_STMT)
        {
          printf ("ERROR: insert only make since for BASIC_BLOCK_STMT statements (variant() == variant()) \n");
          ROSE_ABORT();
        }

     SgBasicBlock* currentBlock = isSgBasicBlock(this);
     ROSE_ASSERT (currentBlock != NULL);

  // printf ("##### Calling SgNode::generateAST() \n");

#if 0
     printf ("In insertSourceCode(): globalDeclaration = \n%s\n",globalDeclaration);
     printf ("In insertSourceCode(): localDeclaration  = \n%s\n",localDeclaration);
     printf ("In insertSourceCode(): sourceCodeString  = \n%s\n",sourceCodeString);
#endif

  // SgNode* newTransformationAST = generateAST (project,sourceCodeString,globalDeclaration);
  // ROSE_ASSERT (newTransformationAST != NULL);
     SgStatementPtrList* newTransformationStatementListPtr =
          generateAST (project,sourceCodeString,localDeclaration,globalDeclaration,isADeclaration);
     ROSE_ASSERT (newTransformationStatementListPtr != NULL);
     ROSE_ASSERT (newTransformationStatementListPtr->size() > 0);

  // printf ("##### DONE: Calling SgNode::generateAST() \n");

  // get a reference to the statement list out of the basic block
     SgStatementPtrList & currentStatementList = currentBlock->get_statements();

     if (locateNewCodeAtTop == true)
        {
       // Insert at top of list (pull the elements off the bottom of the new statement list to get the order correct
       // printf ("Insert new statements (new statement list size = %d) at the top of the block (in reverse order to preset the order in the final block) \n",newTransformationStatementListPtr->size());
          SgStatementPtrList::reverse_iterator transformationStatementIterator;
          for (transformationStatementIterator = newTransformationStatementListPtr->rbegin();
               transformationStatementIterator != newTransformationStatementListPtr->rend();
               transformationStatementIterator++)
             {
            // Modify where a statement is inserted to avoid dependent variables from being inserted
            // before they are declared.

            // Get a list of the variables
            // Generate the list of types used within the target subtree of the AST
               list<string> typeNameStringList = NameQuery::getTypeNamesQuery ( *transformationStatementIterator );

               int statementCounter         = 0;
               int previousStatementCounter = 0;

            // Declaration furthest in source sequence of all variables referenced in code to be inserted (last in source sequence order)
//             SgStatementPtrList::iterator furthestDeclarationInSourceSequence = NULL;
               SgStatementPtrList::iterator furthestDeclarationInSourceSequence;

#if 0
               string unparsedDeclarationCodeString = (*transformationStatementIterator)->unparseToString();
               ROSE_ASSERT (unparsedDeclarationCodeString.c_str() != NULL);
               printf ("unparsedDeclarationCodeString = %s \n",unparsedDeclarationCodeString.c_str());
#endif
               if ( typeNameStringList.size() > 0 )
                  {
                 // There should be at least one type in the statement
                    ROSE_ASSERT (typeNameStringList.size() > 0);
                 // printf ("typeNameStringList.size() = %d \n",typeNameStringList.size());

                 // printf ("This statement has a dependence upon a variable of some type \n");

                 // Loop over all the types and get list of variables of each type
                 // (so they can be declared properly when the transformation is compiled)
                    list<string>::iterator typeListStringElementIterator;
                    for (typeListStringElementIterator = typeNameStringList.begin();
                         typeListStringElementIterator != typeNameStringList.end();
                         typeListStringElementIterator++)
                       {
                      // printf ("Type = %s \n",(*typeListStringElementIterator).c_str());

                      // Find a list of names of variable of type (*listStringElementIterator)
                         list<string> operandNameStringList =
                              NameQuery::getVariableNamesWithTypeNameQuery
                                 ( *transformationStatementIterator, *typeListStringElementIterator );

                      // There should be at least one variable of that type in the statement
                         ROSE_ASSERT (operandNameStringList.size() > 0);
                      // printf ("operandNameStringList.size() = %d \n",operandNameStringList.size());

                      // Loop over all the types and get list of variable of each type
                         list<string>::iterator variableListStringElementIterator;
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
                              SgVariableSymbol* symbol = currentBlock->lookup_var_symbol(name);
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
                                        rose::getLineNumber(declarationStatement),
                                        rose::getFileName(declarationStatement));
                                   string declarationStatementString = declarationStatement->unparseToString();
                                   printf ("declarationStatementString = %s \n",declarationStatementString.c_str());
#endif
                                   statementCounter = 1;

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
                                // If the variable is not found then insert the new statement at the front of the list
#if 0
                                   printf ("Can NOT find a valid symbol corresponding to Type = %s Variable = %s (so it is not declared in the local scope) \n",
                                        (*typeListStringElementIterator).c_str(),
                                        (*variableListStringElementIterator).c_str());
#endif
                                // currentStatementList.push_front(*transformationStatementIterator);
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
#if 1
                 // printf ("BEFORE ADDING NEW STATEMENT: previousStatementCounter = %d \n",previousStatementCounter);
                    if (previousStatementCounter == 0)
                       {
                         printf ("##### Prepend new statement to the top of the local scope \n");
                      // currentStatementList.push_front(*transformationStatementIterator);
                         currentBlock->prepend_statement (*transformationStatementIterator);
                       }
                      else
                       {
                      // printf ("##### Append the new statement after the last position where a dependent variable is declared in the local scope \n");
                      // Use new function added to append/prepend at a specified location in the list of statements
                         currentBlock->append_statement (furthestDeclarationInSourceSequence,*transformationStatementIterator);
                       }
#else
                    SgStatementPtrList::iterator tempIterator = furthestDeclarationInSourceSequence;
                    tempIterator++;
                 // Handle the case of appending at the end of the list
                    if ( tempIterator == currentStatementList.end() )
                       {
                         currentBlock->append_statement (*transformationStatementIterator);
                       }
                      else
                       {
                         currentBlock->insert_statement (tempIterator,*transformationStatementIterator);
                       }
#endif
                  }
                 else
                  {
                 // This statement has no type information (so it has no dependence upon any non-primative type)
                 // "int x;" would be an example of a statement that would not generate a type (though perhaps it should?)
                 // printf ("This statement has no type information (so it has no dependence upon any non-primative type) \n");

                 // So this statment can be places at the front of the list of statements in this block
                 // *** Note that we can't use the STL function directly since it does not set the parent information ***
                 // currentStatementList.push_front(*transformationStatementIterator);
                    currentBlock->insert_statement (currentStatementList.begin(),*transformationStatementIterator);
                  }

#if 0
               string bottomUnparsedDeclarationCodeString = (*transformationStatementIterator)->unparseToString();
               ROSE_ASSERT (bottomUnparsedDeclarationCodeString.c_str() != NULL);
               printf ("bottomUnparsedDeclarationCodeString = %s \n",bottomUnparsedDeclarationCodeString.c_str());
#endif
             }

#if 0
          printf ("Exiting in insertSourceCode(): case of locateNewCodeAtTop == true ... \n");
          ROSE_ABORT();
#endif
        }
       else
        {
       // Put the new statements at the end of the list (traverse the new statements from first to last)
       // But put it before any return statement! So find the last statement!
          SgStatementPtrList::iterator lastStatement = currentStatementList.begin();
          bool foundEndOfList = false;
          while ( (foundEndOfList == false) && (lastStatement != currentStatementList.end()) )
             {
               SgStatementPtrList::iterator tempStatement = lastStatement;
               tempStatement++;
               if (tempStatement == currentStatementList.end())
                    foundEndOfList = true;
                 else
                    lastStatement++;
             }
          ROSE_ASSERT ( *lastStatement != NULL );

       // printf ("(*lastStatement)->sage_class_name() = %s \n",(*lastStatement)->sage_class_name());

       // printf ("Insert new statements at the bottom of the block \n");
          SgStatementPtrList::iterator transformationStatementIterator;
          for (transformationStatementIterator = newTransformationStatementListPtr->begin();
               transformationStatementIterator != newTransformationStatementListPtr->end();
               transformationStatementIterator++)
             {
            // If there is a RETURN_STMT in the block then insert the new statement just before the
            // existing RETURN_STMT
               if ( (*lastStatement)->variant() == RETURN_STMT)
                  {
                 // printf ("Backing away from the end of the list to find the last non-return statement \n");
                 // lastStatement--;
                    currentBlock->insert_statement(lastStatement,*transformationStatementIterator);
                  }
                 else
                  {
                    currentStatementList.push_back(*transformationStatementIterator);
                  }
             }
        }

  // printf ("$CLASSNAME::insertSourceCode taking (SgProject,char*,char*) not implemented yet! \n");
  // ROSE_ABORT();
   }
#endif









