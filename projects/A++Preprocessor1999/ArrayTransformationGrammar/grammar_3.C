// MACHINE GENERATED SOURCE FILE --- DO NOT MODIFY! 

#ifdef HAVE_CONFIG_H 
#include "config.h" 
#endif 

#include "rose.h" 
#include "ROSE_Transformations.h" 

extern struct { ROSE_GrammarVariants variant; char *name; } arrayGrammarTerminalNames[143];


struct 
   { 
     ROSE_TransformableGrammarVariants variant; 
     char *name;                   
   } transformableGrammarTerminalNames[13] = {  
          {ROSE_TransformableNodeTag, "ROSE_TransformableNode"}, 
          {ROSE_TransformableStatementTag, "ROSE_TransformableStatement"}, 
          {ROSE_NonTransformableStatementTag, "ROSE_NonTransformableStatement"}, 
          {ROSE_TransformableStatementBlockTag, "ROSE_TransformableStatementBlock"}, 
          {ROSE_TransformableExpressionTag, "ROSE_TransformableExpression"}, 
          {ROSE_TransformableOperatorExpressionTag, "ROSE_TransformableOperatorExpression"}, 
          {ROSE_TransformableUnaryOperatorExpressionTag, "ROSE_TransformableUnaryOperatorExpression"}, 
          {ROSE_TransformableBinaryOperatorExpressionTag, "ROSE_TransformableBinaryOperatorExpression"}, 
          {ROSE_TransformableBinaryOperatorEqualsTag, "ROSE_TransformableBinaryOperatorEquals"}, 
          {ROSE_TransformableBinaryOperatorNonAssignmentTag, "ROSE_TransformableBinaryOperatorNonAssignment"}, 
          {ROSE_TransformableOperandExpressionTag, "ROSE_TransformableOperandExpression"}, 
          {ROSE_NonTransformableExpressionTag, "ROSE_NonTransformableExpression"}, 
          {ROSE_TRANSFORMABLE_LAST_TAG, "last tag" } 
  }; 
ROSE_TransformableNode::~ROSE_TransformableNode ()
   {
  // delete roseSubTree;
     roseSubTree = NULL;
   }

void ROSE_TransformableNode::setUp ( ROSE_TransformableGrammarVariants newId, ROSE_Node* newSubTree )
   {
     ROSE_ASSERT (newId != ROSE_TRANSFORMABLE_LAST_TAG);
  // ROSE_ASSERT (newId != ROSE_TransformableNodeTag);
     setVariant(newId);      // Default value until reset properly
     ROSE_ASSERT (newSubTree != NULL);
     setRoseSubTree(newSubTree); // Pointer to Sage Tree (is it the copy?)
     setParseError(FALSE);   // The current state will generate an error

     ROSE_ASSERT (!error());
   }

ROSE_TransformableNode::ROSE_TransformableNode ()
   {
  // The default constructor is not useful in the sense that more 
  // information is required to properly setup the ROSE_TransformableNode base class
#if 0
     setUp (ROSE_TRANSFORMABLE_LAST_TAG,NULL);
#else
     id          = ROSE_TRANSFORMABLE_LAST_TAG;  // Default value until reset properly
     roseSubTree = NULL;           // Pointer to Sage Tree (is it the copy?)
     parseError  = TRUE;        // The current state will generate an error
#endif
   }

#if 0
ROSE_TransformableNode::ROSE_TransformableNode ( ROSE_GrammarVariants newId, SgNode* newSubTree )
   {
     setUp (newId,newSubTree);
   }
#endif

ROSE_TransformableNode::ROSE_TransformableNode ( ROSE_Node* newSubTree )
   {
     setUp (ROSE_TransformableNodeTag,newSubTree);
   }

ROSE_TransformableGrammarVariants ROSE_TransformableNode::getVariant()
   {
     ROSE_ASSERT (!error());
     return id;
   }

void ROSE_TransformableNode::setVariant (  ROSE_TransformableGrammarVariants newId )
   {
     id = newId;
   }

#if 0
// This is a pure virtual function and so it should not be defined
ROSE_TransformableNode* ROSE_TransformableNode::transform()
   {
     printf ("ERROR: Base class ROSE_TransformableNode::transform() called \n");
     ROSE_ABORT();

     return NULL;
   }
#endif

Boolean ROSE_TransformableNode::error()
   {
  // Error recovery mechanism (for the parser)
     ROSE_ASSERT (!parseError);
     return parseError;
   }

void ROSE_TransformableNode::setParseError ( Boolean X )
   {
     parseError = X;
   }

const ROSE_Node* ROSE_TransformableNode::getRoseSubTree()
   {
     ROSE_ASSERT (!error());
     return roseSubTree;
   }

void ROSE_TransformableNode::setRoseSubTree ( const ROSE_Node* node )
   {
  // I think we want to avoid letting the input be a NULL pointer
  // ROSE_ASSERT (node != NULL);
     roseSubTree = (ROSE_Node*) node;
   }


ROSE_TransformableStatement::~ROSE_TransformableStatement ()
   {
  // Nothing to do here!
   }

ROSE_TransformableStatement::ROSE_TransformableStatement ( ROSE_Statement* stmt )
   : ROSE_TransformableNode (stmt)
   {
     ROSE_ASSERT (stmt != NULL);

  // setVariant ( ROSE_TransformableStatementTag );
  // setParseError(FALSE);
     ROSE_TransformableNode::setUp (ROSE_TransformableStatementTag,stmt);

     localDataBase = NULL;
     setRoseStatement (stmt);
   }

ROSE_TransformableNode*
ROSE_TransformableStatement::transform()
   {
     ROSE_ASSERT (!error());

     ROSE_Statement* roseStmt = (ROSE_Statement*) getRoseSubTree();
     ROSE_ASSERT (roseStmt != NULL);
     SgStatement* originalStatement = (SgStatement*) roseStmt->getSageSubTree();
     ROSE_ASSERT (originalStatement != NULL);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("In ROSE_TransformableStatement::transform(): (file name = %s line number = %d) \n",
               ROSE::getFileName(originalStatement),ROSE::getLineNumber(originalStatement));

  // Call the transform member function (it will recursively traverse the ROSE Transformation Tree)
     ROSE_NonTransformableExpression* nonTransformableExpression = (ROSE_NonTransformableExpression*) getExpression()->transform();
     ROSE_ASSERT (nonTransformableExpression != NULL);

  // Get the ROSE_C_Expression out of the ROSE_NonTransformableExpression
     ROSE_C_Expression* roseTransformedExpression = (ROSE_C_Expression*) nonTransformableExpression->getRoseSubTree();
     ROSE_ASSERT (roseTransformedExpression != NULL);

  // get the Sage Expression object out of the ROSE_C_Expression
     SgExpression* sageTransformedExpression = (SgExpression*) roseTransformedExpression->getSageSubTree();
     ROSE_ASSERT (sageTransformedExpression != NULL);

  // Now put the sageTransformedExpression into a SgExprStatement
     SgExprStatement* sageTransformedStatement = new SgExprStatement (ROSE_Node::buildSageFileInfo(),sageTransformedExpression);
     ROSE_ASSERT (sageTransformedStatement != NULL);

#if 0
     printf ("\n\n");
     showSgStatement (cout, sageTransformedStatement , "sageTransformedStatement inside of ROSE_TransformableStatement::transform()");

     printf ("\n\n");
     printf ("Now unparse the statement: \n");
     sageTransformedStatement->logical_unparse(NO_UNPARSE_INFO,cout);
     printf ("\n\n");
     cout << endl << endl;
#endif

  // This is the function which uses the newly built Sage Statement as the inner loop body of the a for loop
  // and supporting variable declarations that are also introduced into the Sage Tree. This completes the 
  // transformation for a single statement.  Later we might want to locate this modification of the Sage Tree
  // within another location within ROSE, so that the final editing of the Sage tree is abstracted from the
  // definition of what ever transformation we do.

  // This was the old way of calling the buildForLoopStructureAndModifySageTree 
  // function, now it is a member function!
  // ROSE_ASSERT (globalArrayAssignmentUsingTransformationGrammar != NULL);
  // globalArrayAssignmentUsingTransformationGrammar->buildForLoopStructureAndModifySageTree ( originalStatement, sageTransformedStatement );
     buildForLoopStructureAndModifySageTree ( originalStatement, sageTransformedStatement );

#if 0
     printf ("\n\n");
     printf ("Exiting after first transformation! ( In ROSE_TransformableStatement::transform() ) \n");
     ROSE_ABORT();
#endif
     return this;
   }

ROSE_Statement* 
ROSE_TransformableStatement::getRoseStatement ()
   {
  // ROSE_ASSERT (!error());
     return (ROSE_Statement*) getRoseSubTree(); 
   }

void 
ROSE_TransformableStatement::setRoseStatement ( ROSE_Statement* stmt )
   {
     ROSE_ASSERT (stmt != NULL);
     setRoseSubTree(stmt);
   }

ROSE_TransformableBinaryOperatorEquals* 
ROSE_TransformableStatement::getExpression ()
   {
     ROSE_ASSERT (transformableBinaryOperatorEquals != NULL);
     return transformableBinaryOperatorEquals;
   }

void 
ROSE_TransformableStatement::setExpression ( ROSE_TransformableBinaryOperatorEquals* expr )
   {
     ROSE_ASSERT (expr != NULL);
     transformableBinaryOperatorEquals = expr;
   }

#if 0
void
ROSE_TransformableStatement::setUpDataBase()
   {
  // This function calls the data base mechanism on the 
  // ArrayAssignmentUsingTransformationGrammar object.

     localDataBase = setUpDataBase();
   }
#endif

int ROSE_TransformableStatement::numberOfRhsOperands()
   {
  // This function searches the tree and counts the number of operands on the Rhs
  // This could be part of a function that searches the statement and identifies
  // all the data that we require to know about a statement and puts it into a small
  // data base.

  // We want to access the data from the data base build locally

  // return ZERO for now until this is implemented!
  // printf ("Sorry, not implemented ROSE_TransformableStatement::numberOfRhsOperands() \n");
  // ROSE_ABORT();

     ROSE_ASSERT (localDataBase != NULL);
     return localDataBase->getNumberOfRhsOperands();
   }

// These are located in the ROSE_TransformableStatement object
void
ROSE_TransformableStatement::oncePerScopeSubtransformation ( SgStatement *currentStatement )
   {
  // This subtransformation is introduced once for every scope where a statement is found and transformed
  // Later we want to return a modified SgStatementPtrList object so that the modifiecation of the
  // Sage tree is separated from the development of the transformation.

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Introduce UNIQUE LOCAL SCOPE subtransformation! \n");

     ROSE_ASSERT (currentStatement != NULL);
     SgStatementPtrList & localUniqueList = 
          ArrayAssignmentUsingTransformationGrammar::getTransformationTemplateData()->uniqueStatementList;

  // printf ("localUniqueList.size() = %d \n",localUniqueList.size());
     ROSE_ASSERT (localUniqueList.size() > 0);

  // Loop though the statements which represent the Lhs subtransformation
     SgStatementPtrList::iterator i;
     for (i = localUniqueList.begin(); i != localUniqueList.end(); i++)
        {
          Boolean inFront = TRUE;
#ifdef USE_SAGE3
          SgStatement* supportingStatement = *i;
#else
          SgStatement* supportingStatement = (*i).irep();
#endif
          char* indexName = "rose_index";
          SgStatement* copyOfSupportingStatement =
               ArrayAssignmentUsingTransformationGrammar::copy(supportingStatement,"GLOBAL_INDEX_NAME",indexName);

          ROSE_ASSERT (copyOfSupportingStatement->get_parent() == NULL);

          currentStatement->get_parent()->insert_statement(currentStatement,copyOfSupportingStatement,inFront);

#if 0
          ROSE_ASSERT (copyOfSupportingStatement->get_parent() == NULL);
          copyOfSupportingStatement->set_parent(newVariableDeclaration);
#endif
          ROSE_ASSERT (copyOfSupportingStatement->get_parent() != NULL);
        }
   }

void
ROSE_TransformableStatement::oncePerTransformationSubtransformation ( SgStatement *currentStatement )
   {
  // printf ("Introduce Lhs subtransformation! \n");

     SgStatementPtrList & lhsList =
          ArrayAssignmentUsingTransformationGrammar::getTransformationTemplateData()->lhsStatementList;

  // printf ("lhsList.size() = %d \n",lhsList.size());
     ROSE_ASSERT (lhsList.size() > 0);

     ROSE_ASSERT (localDataBase != NULL);
     ROSE_ASSERT (localDataBase->lhsOperand != NULL);
     SgName lhsName        = localDataBase->lhsOperand->getVariableName();
     SgName lhsNamePointer = localDataBase->lhsOperand->getVariableNamePointer();

     SgStatementPtrList::iterator i;
     for (i = lhsList.begin(); i != lhsList.end(); i++)
        {
       // Make a copy of the statement from the header file (since we 
       // will edit it to make it have the correct names)
#ifdef USE_SAGE3
          SgStatement* supportingStatement = *i;
#else
          SgStatement* supportingStatement = (*i).irep();
#endif

#if 0
          SgScopeStatement* originalOuterScope = isSgScopeStatement (supportingStatement->get_parent());
          ROSE_ASSERT (originalOuterScope != NULL);
          printf ("Now unparse the original whole scope \n");
          originalOuterScope->logical_unparse(NO_UNPARSE_INFO,cout);
          cout << endl << endl;
#endif

          SgStatement *newStatement = ArrayAssignmentUsingTransformationGrammar::copy (supportingStatement,"LHS_ARRAY",lhsName.str());
          ROSE_ASSERT (newStatement != NULL);
          newStatement = ArrayAssignmentUsingTransformationGrammar::copy (newStatement,"LHS_ARRAY_DATA_POINTER",lhsNamePointer.str());
          ROSE_ASSERT (newStatement != NULL);

       // Since we know what we want to see here we can make this specific
       // to statement "double* RESTRICT a = A.getDataPointer();"
       // editLhsSpecificSubTransformation ((*i).irep(),"lhs","LHS");
       // editLhsSpecificSubTransformation (newStatement,"A_rosePrimativeVariable","A");
#if 0
          printf ("########################################################## \n");
          printf ("########################################################## \n");
          printf ("Original Lhs Statement: \n");
       // showSgStatement (cout,(*i).irep(), "Called from ROSE_TransformableStatement::oncePerTransformationSubtransformation" );
          printf ("########################################################## \n");
          printf ("########################################################## \n");

          printf ("########################################################## \n");
          printf ("########################################################## \n");
          printf ("New Lhs Statement: \n");
       // showSgStatement (cout,newStatement, "Called from ROSE_TransformableStatement::oncePerTransformationSubtransformation" );
          printf ("########################################################## \n");
          printf ("########################################################## \n");
#endif
          Boolean inFront = TRUE;
          currentStatement->get_parent()->insert_statement(currentStatement,newStatement,inFront);

#if 0
          SgScopeStatement* newOuterScope = isSgScopeStatement (newStatement->get_parent());
          ROSE_ASSERT (newOuterScope != NULL);
          printf ("Now unparse the whole scope \n");
          newOuterScope->logical_unparse(NO_UNPARSE_INFO,cout);
          cout << endl << endl;
#endif
#if 0
          printf ("Exiting after editing Lhs Statement! \n");
          ROSE_ABORT();
#endif
        }
   }

void
ROSE_TransformableStatement::repeatedForEachTransformationSubtransformation ( SgStatement *currentStatement )
   {
  // printf ("Introduce Rhs substransformation! \n");

     ROSE_ASSERT (localDataBase != NULL);
     List<SgName>* uniqueNamesList = localDataBase->computeUniqueOperands();
     ROSE_ASSERT (uniqueNamesList != NULL);

#if 0
     printf ("uniqueNamesList->getLength() = %d \n",uniqueNamesList->getLength());
     for (int i=0; i < uniqueNamesList->getLength(); i++)
        {
          printf ("Introduce Rhs Operand substransformation: i = %d name = %s \n",i,(*uniqueNamesList)[i].str());
        }
#endif

  // Since the Lhs has already been setup we start the index into the list at 1 (instead of 0)!
  // for (int rhsOperandIndex = 0; rhsOperandIndex < numberOfRhsOperands(); rhsOperandIndex++)
     for (int rhsOperandIndex = 1; rhsOperandIndex < uniqueNamesList->getLength(); rhsOperandIndex++)
        {
       // printf ("Introduce Rhs Operand substransformation: name = %s \n",(*uniqueNamesList)[rhsOperandIndex].str());

#if 0
          ROSE_ASSERT (localDataBase != NULL);
          ROSE_ASSERT (localDataBase->rhsOperand[rhsOperandIndex] != NULL);
          ROSE_ASSERT (localDataBase->rhsOperand[rhsOperandIndex]->getVariableName() != NULL);
          char* rhsName        = localDataBase->rhsOperand[rhsOperandIndex]->getVariableName();
          char* rhsNamePointer = localDataBase->rhsOperand[rhsOperandIndex]->getVariableNamePointer();
#else
          SgName rhsName        = (*uniqueNamesList)[rhsOperandIndex];

      //  printf ("@@@@@@@@@@@@@@@@@ rhsName = %s \n",rhsName.str());

      //  char* rhsNamePointer = (*uniqueNamesList)[rhsOperandIndex]->getVariableNamePointer();
      //  char* rhsNamePointer = "DEFAULT_RHS_POINTER";
          SgName rhsNamePointer = ROSE::concatenate((*uniqueNamesList)[rhsOperandIndex],OperandDataBase::transformationVariableSuffix());
#endif

          SgStatementPtrList & rhsList = ArrayAssignmentUsingTransformationGrammar::getTransformationTemplateData()->rhsStatementList;

       // printf ("rhsList.size() = %d \n",rhsList.size());
          ROSE_ASSERT (rhsList.size() > 0);

          SgStatementPtrList::iterator i;
          for (i = rhsList.begin(); i != rhsList.end(); i++)
             {
            // Make a copy of the statement from the header file (since we
            // will edit it to make it have the correct names)
#ifdef USE_SAGE3
               SgStatement* supportingStatement = *i;
#else
               SgStatement* supportingStatement = (*i).irep();
#endif
            // SgStatement *newStatement = copy (supportingStatment,"","");
            // ROSE_ASSERT (newStatement != NULL);

            // Fix up later to avoid repeated names (best to put into separate transformation)
            // if (nameNotRepeated(rhsName) == TRUE)
               if (TRUE)
                  {
                 // SgStatement *newStatement = ArrayAssignmentUsingTransformationGrammar::copy (supportingStatement,"RHS_ARRAY","default_rhs_array");
                    SgStatement *newStatement = ArrayAssignmentUsingTransformationGrammar::copy (supportingStatement,"RHS_ARRAY",rhsName);
                    ROSE_ASSERT (newStatement != NULL);
                 // newStatement = ArrayAssignmentUsingTransformationGrammar::copy (newStatement,"RHS_ARRAY_DATA_POINTER","default_rhs_data_pointer");
                    newStatement = ArrayAssignmentUsingTransformationGrammar::copy (newStatement,"RHS_ARRAY_DATA_POINTER",rhsNamePointer);
                    ROSE_ASSERT (newStatement != NULL);

                 // Since we know what we want to see here we can make this specific
                 // to statement "double* RESTRICT b = B.getDataPointer();"
                 // editLhsSpecificSubTransformation ((*i).irep(),"lhs","LHS");
                 // editRhsSpecificSubTransformation (newStatement,"B_rosePrimativeVariable","B");
#if 0
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("NewStatement AFTER editRhsSpecificSubTransformation \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    showSgStatement (cout,newStatement,"newStatement AFTER editRhsSpecificSubTransformation");
#endif
#if 1
                    Boolean inFront = TRUE;
                    currentStatement->get_parent()->insert_statement(currentStatement,newStatement,inFront);
#endif
                  }
             }
        }

#if 0
     printf ("Exiting in ROSE_TransformableStatement::repeatedForEachTransformationSubtransformation() \n");
     ROSE_ABORT();
#endif
   }


void
ROSE_TransformableStatement::outerLoopStructureSubtransformation ( 
     SgStatement *currentStatement, 
     SgStatement* transformedStatement )
   {
  // printf ("Introduce Outer Loop Structure substransformation! \n");

  // CW: get the dimension from the data base.
  // for now I hope that it is correctly specified.
  // eventually I should take the dimension of the
  // lhs operand instead!
     ROSE_ASSERT(localDataBase!=NULL);
     int dimension=localDataBase->statementDimension;
  
     SgStatementPtrList & loopStructureList =
          ArrayAssignmentUsingTransformationGrammar::
               getTransformationTemplateData()->outerLoopStatementList[dimension-1];

  // Get a pointer to the body of the inner loop (a SgBasicBlock which is derived from a SgStatement)
     SgStatement* innerLoopBody = NULL;

  // printf ("loopStructureList.size() = %d \n",loopStructureList.size());
     ROSE_ASSERT (loopStructureList.size() > 0);

     SgStatementPtrList::iterator i=loopStructureList.begin();
     for (i; i != loopStructureList.end(); i++)
        {
       // One of the statements in the loopStructureList is a "for" loop so we
       // just want the innerLoopBody from that statement
#ifdef USE_SAGE3
          SgStatement* supportingStatement = *i;
#else
          SgStatement* supportingStatement = (*i).irep();
#endif

#if 0
          printf ("Now unparse the supportingStatement \n");
          supportingStatement->logical_unparse(NO_UNPARSE_INFO,cout);
          cout << endl << endl;
#endif
    
          ROSE_ASSERT (localDataBase != NULL);
          ROSE_ASSERT (localDataBase->lhsOperand != NULL);
       // ROSE_ASSERT (localDataBase->lhsOperand->getVariableName() != NULL);
          SgName lhsName = localDataBase->lhsOperand->getVariableName();
          SgName lhsNamePointer = localDataBase->lhsOperand->getVariableNamePointer();

          SgStatement *newStatement = 
               ArrayAssignmentUsingTransformationGrammar::copy (supportingStatement,"LHS_ARRAY",lhsName);
          ROSE_ASSERT (newStatement != NULL);
          newStatement = ArrayAssignmentUsingTransformationGrammar::copy (newStatement,"LHS_ARRAY_DATA_POINTER",lhsNamePointer);
          ROSE_ASSERT (newStatement != NULL);
          newStatement = ArrayAssignmentUsingTransformationGrammar::copy (newStatement,"GLOBAL_INDEX_NAME","rose_index");
#if 0
          printf ("Now unparse the supportingStatement 3 \n");
          newStatement->logical_unparse(NO_UNPARSE_INFO,cout);
          cout << endl << endl;
#endif
          ROSE_ASSERT (newStatement != NULL);

       // Save a reference to the innerloop body if this is a for loop!
          if (innerLoopBody == NULL)
               innerLoopBody = ArrayAssignmentUsingTransformationGrammar::getInnerLoopBody(newStatement);

          Boolean inFront = TRUE;
       // This causes an error internally in Sage (not sure why it does not work)
       // currentStatement->get_parent()->replace_statement(currentStatement,newStatement);
          currentStatement->get_parent()->insert_statement(currentStatement,newStatement,inFront);
        }

  // Now we have a reference to the inner loop body (where we will insert the transformedStatement)
     ROSE_ASSERT (innerLoopBody != NULL);

  // Now replace the innerloop body with the transformedStatement
     ArrayAssignmentUsingTransformationGrammar::editInnerLoopBodyToMatchCurrentStatement (innerLoopBody, transformedStatement);

#ifdef USE_SAGE3
  // The original statement is turned into a NULL statement since we could not replace it directly
  // we should be abel to fix this correctly using SAGE3! (but we have not done so yet!)
     ArrayAssignmentUsingTransformationGrammar::makeStatementNullStatement (currentStatement);
#else
  // The original statement is turned into a NULL statement since we could not replace it directly
  // (I could only get the inset function to work properly) 
     ArrayAssignmentUsingTransformationGrammar::makeStatementNullStatement (currentStatement);
#endif

#if 0
     SgStatementPtrList& tempLoopStructureList = ArrayAssignmentUsingTransformationGrammar::getTransformationTemplateData()->outerLoopStatementList;

  // Get a pointer to the body of the inner loop (a SgBasicBlock which is derived from a SgStatement)
  // SgStatement* innerLoopBody = NULL;

     printf ("tempLoopStructureList.size() = %d \n",loopStructureList.size());
     ROSE_ASSERT (tempLoopStructureList.size() > 0);
     ROSE_ASSERT (tempLoopStructureList.size() == 3);  // temp error checking code

     for (i = tempLoopStructureList.begin(); i != tempLoopStructureList.end(); i++)
        {
       // One of the statements in the loopStructureList is a "for" loop so we 
       // just want the innerLoopBody from that statement
#ifdef USE_SAGE3
          SgStatement* supportingStatement = *i;
#else
          SgStatement* supportingStatement = (*i).irep();
#endif
    
          printf ("Now unparse the original TransformationTemplateData() \n");
          supportingStatement->logical_unparse(NO_UNPARSE_INFO,cout);
          cout << endl << endl;
        }
#endif

#if 0
     printf ("Exiting in ROSE_TransformableStatement::outerLoopStructureSubtransformation() after validation and transformation of statement! \n");
     ROSE_ABORT();
#endif
   }


void
ROSE_TransformableStatement::buildForLoopStructureAndModifySageTree ( 
     SgStatement* sageStatement,
     SgStatement* transformedStatement )
   {
  // OK, now we have an "Expression Statement" that we want to transform
  // The strategy is to look in teh outer scope and find the statement within that 
  // scope so we can insert the new statements (there is often more than one) into 
  // the correct scope (the outer scope) ahead of the original statement (which is 
  // set to a NULL statement).
     ROSE_ASSERT (sageStatement != NULL);

     SgExprStatement* exprStmt = isSgExprStatement(sageStatement);
     ROSE_ASSERT (exprStmt != NULL);

     if (ROSE_DEBUG > 0)
        {
       // printf ("In ArrayAssignmentUsingTransformationGrammar::buildForLoopStructure(SgStatement,SgStatement) \n");
          printf ("Statement from File Name %s -- Line Number %d -- FOUND AN A++ STATEMENT (simple assignment) to transform! \n",
               ROSE::getFileName(exprStmt), ROSE::getLineNumber(exprStmt) );
        }

     ROSE_ASSERT (ArrayAssignmentUsingTransformationGrammar::templatesDataSetup() == TRUE);

  // Get current scope of statement (We keep the currrent scope on a stack so we can see everything in that scope)
  // SgBasicBlock *basicBlock = (SgBasicBlock*) (*currentSgBasicBlockStack.begin()).irep();
     SgBasicBlock *basicBlock = (SgBasicBlock*) ArrayAssignmentUsingTransformationGrammar::getCurrentScope();
     ROSE_ASSERT ( basicBlock != NULL);

  // cout << "Number of statements in block = " << basicBlock->get_statements().size() << endl;

  // Now iterate through the current scope (SgBasicBlock) to find the location of the current statement
  // (and the surrounding ones)
  // SgStatement *previousStatement = (*basicBlock->get_statements().begin()).irep();
     SgStatement *previousStatement = NULL;
     SgStatementPtrList::iterator statementIterator;
     for (statementIterator = basicBlock->get_statements().begin(); 
          statementIterator != basicBlock->get_statements().end();
          statementIterator++)
        {
       // Get the SgStatement from the SgStatementPrt using the irep() member function
#ifdef USE_SAGE3
          SgStatement* currentStatement = *statementIterator;
#else
          SgStatement *currentStatement = (*statementIterator).irep();
#endif
          ROSE_ASSERT (currentStatement != NULL);

       // Turn on and off internal ROSE debugging!
       // ROSE_DEBUG = (ROSE::getLineNumber(exprStmt) == 12) ? ROSE_DEBUG : ROSE_DEBUG;

       // How Transformations are organized:
       // We address the requirements of a general transformation which may be
       // broken up into multiple parts (subtransformations): 
       //    a) Global Unique Subtransformation: (one time) sections of the transformation program 
       //       tree which must be introduced into the user application's program tree (global 
       //       variables required, global functions that need be introduced), our current transformation 
       //       has none of these.
       //    b) Local Scope Unique Subtransformations: parts of the transformation common to each use 
       //       of the transformation but which are local in scope.
       //    c) Lhs Operand Specific Subtransformation: parts of the transformation which are specific to the Lhs
       //    d) Rhs Operand Specific Subtransformation: Parts which are specific to each Rhs operand
       //    e) Loop Structure Subtransformation:
       //    f) Loop Body

       // Things to do (subtransformations to address):
       //
       // 1) Global Unique Subtransformations (the current array assignment transformation 
       //    does not have a globally unique subtransformation).
       //     a) check if this transformation has been done previously
       // 2) Local Scope Unique Subtransformations
       //     a) check if this transformation has been done in this block
       //     b) introduce the transformations (edit the source subtransformation tree as required)
       // 3) Lhs Operand Specific Subtransformation (edit the source subtransformation tree as required)
       // 4) Rhs Operand Specific Subtransformation (edit the source subtransformation tree as required)

          if (currentStatement == exprStmt)
             {
            // printf ("SWAP STATEMENTS TO DO THE TRANSFORMATION! \n");

               SgStatement* seedStatement = ArrayAssignmentUsingTransformationGrammar::buildNullStatement();
	       ROSE_ASSERT (seedStatement != NULL);

               Sg_File_Info* fileInfo = ArrayAssignmentUsingTransformationGrammar::buildSageFileInfo();
               SgBasicBlock* newBasicBlock = new SgBasicBlock(fileInfo);
	       ROSE_ASSERT (newBasicBlock != NULL);
               newBasicBlock->append_statement(seedStatement);

               Boolean inFront = TRUE;
               currentStatement->get_parent()->insert_statement(currentStatement,newBasicBlock,inFront);
               SgStatement* originalArrayStatement = currentStatement;
               currentStatement = seedStatement;

            // The original statement is turned into a NULL statement since we could not replace it directly
            // (I could only get the inset function to work properly)
               ArrayAssignmentUsingTransformationGrammar::makeStatementNullStatement (originalArrayStatement);

            // We use this in many of the subtransformations
               SgStatementPtrList::iterator i;

               if ( ArrayAssignmentUsingTransformationGrammar::transformationIntroducedInGlobalScope() == FALSE )
                  {
                 // Nothing to introduce since this transformation has no globally scoped 
                 // required subtransformation

                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         printf ("Introduce GLOBAL substransformation! \n");
                  }

               if ( ArrayAssignmentUsingTransformationGrammar::transformationIntroducedInThisScope() == FALSE )
                  {
                 // uniqueSubTransformation(currentStatement);
                    oncePerScopeSubtransformation(currentStatement);
                  }

               oncePerTransformationSubtransformation (currentStatement);

               repeatedForEachTransformationSubtransformation (currentStatement);

               outerLoopStructureSubtransformation (currentStatement, transformedStatement);

#if 0
               SgScopeStatement* outerScope = isSgScopeStatement (currentStatement->get_parent());
               ROSE_ASSERT (outerScope != NULL);

	       printf ("Now unparse the previous scope \n");
               outerScope->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
#endif

               if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                    printf ("FINISHED TRANSFORMATION FOR THIS STATEMENT! \n");
             }

       // Save the previous Statement (I forget why I'm doing this)
       // I think it is because there is no way to see the previous statement 
       // because the iterator is not a bidirectional iterator!
          previousStatement = currentStatement;
        }

#if 0
     printf ("Exiting in ROSE_TransformableStatement::buildForLoopStructureAndModifySageTree() after validation and transformation of statement! \n");
     ROSE_ABORT();
#endif
   }



// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableStatement::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableStatementTag)
        {
          printf ("Error in ROSE_TransformableStatement::error(): ROSE_TransformableStatement object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableStatement::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableStatementTag);
     return ROSE_TransformableNode::error();
   }
ROSE_TransformableExpression::~ROSE_TransformableExpression ()
   {
   }

#if 0
ROSE_TransformableExpression::ROSE_TransformableExpression ()
   {
     ROSE_Node::setUp (ROSE_TransformableExpressionTag,NULL);
   }
#endif

ROSE_TransformableExpression::ROSE_TransformableExpression ( ROSE_Expression* expr )
   : ROSE_TransformableNode(expr)
   {
  // ArrayOperators are implemented as member functions or friend functions and so this function
  // looks for array operators by looking for either array friend functions or array member functions
  // the function name is required to be from a specific set of qualified names as well.

     ROSE_ASSERT (expr != NULL);
     ROSE_TransformableNode::setUp (ROSE_TransformableExpressionTag,expr);

  // showSgExpression (cout, expr, "Called from ROSE_TransformableExpression::ROSE_TransformableExpression ( SgExpression* expr )");
   }

ROSE_TransformableNode* ROSE_TransformableExpression::transform()
   {
     printf ("Sorry not implemented, ROSE_TransformableExpression::Tranform() \n");
     ROSE_ABORT();

  // getRoseExpression()->transform();

     return this;
   }

void ROSE_TransformableExpression::setRoseExpression ( ROSE_Expression* expr )
   {
     ROSE_ASSERT (expr != NULL);

     printf ("Not clear where we need this function! \n");
     ROSE_ABORT();

  // roseExpression = expr;
   }

ROSE_Expression* ROSE_TransformableExpression::getRoseExpression ()
   {
  // ROSE_ASSERT (roseExpression != NULL);
  // return roseExpression;

  // Casting this to a ROSE_Expression then seems to hide it being a derived type, could this be???
     return (ROSE_Expression*) getRoseSubTree();
   }

// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableExpression::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableExpressionTag)
        {
          printf ("Error in ROSE_TransformableExpression::error(): ROSE_TransformableExpression object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableExpression::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableExpressionTag);
     return ROSE_TransformableNode::error();
   }
ROSE_NonTransformableStatement::~ROSE_NonTransformableStatement ()
   {
   }

ROSE_NonTransformableStatement::ROSE_NonTransformableStatement ( ROSE_Statement* stmt )
   : ROSE_TransformableStatement (stmt)
   {
     ROSE_ASSERT (stmt != NULL);

  // setVariant ( ROSE_NonTransformableStatementTag );
  // setParseError(FALSE);
     ROSE_TransformableNode::setUp (ROSE_NonTransformableStatementTag,stmt);
   }

ROSE_TransformableNode* ROSE_NonTransformableStatement::transform()
   {
  // printf ("Sorry not implemented, ROSE_NonTransformableStatement::Tranform() \n");
  // ROSE_ABORT();

     ROSE_ASSERT (!error());

     ROSE_Statement* roseStmt = (ROSE_Statement*) getRoseSubTree();
     ROSE_ASSERT (roseStmt != NULL);
     SgStatement* stmt = (SgStatement*) roseStmt->getSageSubTree();
     ROSE_ASSERT (stmt != NULL);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("In ROSE_NonTransformableStatement::transform(): (file name = %s line number = %d) \n",
               ROSE::getFileName(stmt),ROSE::getLineNumber(stmt));

     return this;
   }

ROSE_Statement* ROSE_NonTransformableStatement::getRoseStatement ()
   {
  // ROSE_ASSERT (!error());
     return (ROSE_Statement*) getRoseSubTree(); 
   }

void ROSE_NonTransformableStatement::setRoseStatement ( ROSE_Statement* stmt )
   {
     ROSE_ASSERT (stmt != NULL);
     setRoseSubTree(stmt);
   }
// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_NonTransformableStatement::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_NonTransformableStatementTag)
        {
          printf ("Error in ROSE_NonTransformableStatement::error(): ROSE_NonTransformableStatement object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_NonTransformableStatement::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_NonTransformableStatementTag);
     return ROSE_TransformableNode::error();
   }
ROSE_TransformableStatementBlock::~ROSE_TransformableStatementBlock ()
   {
   }

ROSE_TransformableStatementBlock::ROSE_TransformableStatementBlock ( ROSE_StatementBlock* roseStatementBlock )
   : ROSE_TransformableStatement(roseStatementBlock)
   {
  // This function is a virtual member function 
     recognizeRoseSubTree (roseStatementBlock);
   }

ROSE_TransformableNode* ROSE_TransformableStatementBlock::transform()
   {
  // printf ("Sorry not implemented, ROSE_TransformableStatementBlock::Tranform() \n");
  // ROSE_ABORT();

     ROSE_ASSERT (!error());

     ROSE_Statement* roseStmt = (ROSE_Statement*) getRoseSubTree();
     ROSE_ASSERT (roseStmt != NULL);
     SgStatement* stmt = (SgStatement*) roseStmt->getSageSubTree();
     ROSE_ASSERT (stmt != NULL);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("In ROSE_TransformableStatementBlock::transform(): (file name = %s line number = %d)  roseStatementList.getLength() = %d \n",
               ROSE::getFileName(stmt),ROSE::getLineNumber(stmt),transformableStatementList.getLength());

  // ROSE_TransformableStatementBlock* roseTransformableStatementBlock = new ROSE_TransformableStatementBlock (roseStatementBlock);
  // ROSE_ASSERT (roseTransformableStatementBlock != NULL);
  // ROSE_ASSERT (!roseTransformableStatementBlock->error());

#if 1
  // if ( isSgScopeStatement(stmt) == NULL )
  //      showSgStatement (cout, stmt,"Called from ROSE_TransformableStatementBlock::transform()");

     ROSE_ASSERT (isSgScopeStatement(stmt) != NULL);
     SgScopeStatement *sageScopeStmt = isSgScopeStatement(stmt);
     ROSE_ASSERT (sageScopeStmt != NULL);
  // printf ("Push a new scope ON the stack! \n");
     globalArrayAssignmentUsingTransformationGrammar->pushCurrentScope (sageScopeStmt);
#endif
     for (int i=0; i < transformableStatementList.getLength(); i++)
        {
       // Not clear if we want to put this into another ROSE_TransformableStatementBlock
          ROSE_TransformableStatement* transformableStatement = 
               (ROSE_TransformableStatement*) transformableStatementList[i].transform();
          ROSE_ASSERT (transformableStatement != NULL);
        }
#if 1
  // printf ("Pop an existing scope OFF the stack! \n");
     ROSE_ASSERT (sageScopeStmt != NULL);
     globalArrayAssignmentUsingTransformationGrammar->popCurrentScope (sageScopeStmt);
#endif

#if 1
  // return roseTransformableStatementBlock;
#endif

  // Return the "this" pointer for now
     return this;
   }

void ROSE_TransformableStatementBlock::recognizeRoseSubTree ( ROSE_StatementBlock* roseStatementBlock )
   {
  // printf ("Sorry, not implemented: ROSE_TransformableStatementBlock::recognizeSageSubTree(SgBasicBlock*) \n");
     ROSE_TransformableNode::setUp (ROSE_TransformableStatementBlockTag,roseStatementBlock);
   }

int ROSE_TransformableStatementBlock::getNumberOfStatements ()
   {
     return transformableStatementList.getLength();
   }

void ROSE_TransformableStatementBlock::addTransformableStatementToBlock ( ROSE_TransformableStatement* transformableStatement )
   {
  // printf ("Sorry, not implemented: ROSE_TransformableStatementBlock::addArrayStatementToBlock(ROSE_Statement*) \n");
     transformableStatementList.addElement (*transformableStatement);
   }

ROSE_TransformableStatement* ROSE_TransformableStatementBlock::operator[](int i)
   {
     return &transformableStatementList[i];
   }
// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableStatementBlock::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableStatementBlockTag)
        {
          printf ("Error in ROSE_TransformableStatementBlock::error(): ROSE_TransformableStatementBlock object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableStatementBlock::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableStatementBlockTag);
     return ROSE_TransformableNode::error();
   }
ROSE_TransformableOperatorExpression::~ROSE_TransformableOperatorExpression()
   {
   }

#if 0
ROSE_TransformableOperatorExpression::ROSE_TransformableOperatorExpression()
   {
   }
#endif

ROSE_TransformableOperatorExpression::ROSE_TransformableOperatorExpression( ROSE_Expression* expr )
   : ROSE_TransformableExpression(expr)
   {
     ROSE_ASSERT (expr != NULL);
     ROSE_TransformableNode::setUp (ROSE_TransformableOperatorExpressionTag,expr);
   }

ROSE_TransformableNode* ROSE_TransformableOperatorExpression::transform()
   {
     printf ("Sorry not implemented, ROSE_TransformableOperatorExpression::Tranform() \n");
     ROSE_ABORT();

     return this;
   }

#if 0
void ROSE_TransformableOperatorExpression::addArgumentExpression ( ROSE_Expression* roseExpression )
   {
     ROSE_ASSERT (roseExpression != NULL);
     roseExpressionList.addElement(*roseExpression);
   }

int ROSE_TransformableOperatorExpression::numberOfArguments()
   {
     return roseExpressionList.getLength();
   }

void ROSE_TransformableOperatorExpression::setSageExpressionListExpression ( SgExprListExp* exprListExp )
   {
     ROSE_ASSERT (exprListExp != NULL);
     sageExpressionListExpression = exprListExp;
   }

SgExprListExp* ROSE_TransformableOperatorExpression::getSageExpressionListExpression()
   {
     ROSE_ASSERT (sageExpressionListExpression != NULL);
     return sageExpressionListExpression;
   }

List<ROSE_Expression> & ROSE_TransformableOperatorExpression::getRoseExpressionList ()
   {
     return roseExpressionList;
   }
#endif// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableOperatorExpression::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableOperatorExpressionTag)
        {
          printf ("Error in ROSE_TransformableOperatorExpression::error(): ROSE_TransformableOperatorExpression object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableOperatorExpression::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableOperatorExpressionTag);
     return ROSE_TransformableNode::error();
   }
ROSE_TransformableOperandExpression::~ROSE_TransformableOperandExpression ()
   {
   }

ROSE_TransformableOperandExpression::ROSE_TransformableOperandExpression ( ROSE_Expression* expr )
   : ROSE_TransformableExpression(expr)
   {
     ROSE_TransformableNode::setUp (ROSE_TransformableOperandExpressionTag,expr);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("$$$$$$$$$$ In ROSE_TransformableOperandExpression constructor: arrayGrammarTerminalNames[%d].name = %s \n",
               expr->getVariant(), arrayGrammarTerminalNames[expr->getVariant()].name);
   }

#if 1
ROSE_TransformableNode* ROSE_TransformableOperandExpression::transform()
   {
  // printf ("Sorry not implemented, ROSE_TransformableOperandExpression::Tranform() \n");
  // ROSE_ABORT();


  // We are at a leaf so we don't have to assemble the transformed expression 
  // from the collection of other pieces.  So the can get the transformation directly.
     ROSE_C_Expression* roseOperandExpression = (ROSE_C_Expression*) getRoseExpression()->transform();
     ROSE_ASSERT (roseOperandExpression != NULL);

     ROSE_NonTransformableExpression* returnTransformableOperandExpression =
          new ROSE_NonTransformableExpression (roseOperandExpression);
     ROSE_ASSERT (returnTransformableOperandExpression != NULL);

#if 0
     printf ("Exiting at base of ROSE_TransformableOperandExpression::transform() \n");
     ROSE_ABORT();
#endif

     return returnTransformableOperandExpression;
   }
#endif


// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableOperandExpression::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableOperandExpressionTag)
        {
          printf ("Error in ROSE_TransformableOperandExpression::error(): ROSE_TransformableOperandExpression object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableOperandExpression::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableOperandExpressionTag);
     return ROSE_TransformableNode::error();
   }
ROSE_NonTransformableExpression::~ROSE_NonTransformableExpression ()
   {
   }

#if 0
ROSE_NonTransformableExpression::ROSE_NonTransformableExpression ()
   {
     ROSE_Node::setUp (ROSE_NonTransformableExpressionTag,NULL);
   }
#endif

ROSE_NonTransformableExpression::ROSE_NonTransformableExpression ( ROSE_Expression* expr )
   : ROSE_TransformableExpression(expr)
   {
  // ArrayOperators are implemented as member functions or friend functions and so this function
  // looks for array operators by looking for either array friend functions or array member functions
  // the function name is required to be from a specific set of qualified names as well.

     ROSE_ASSERT (expr != NULL);
     ROSE_TransformableNode::setUp (ROSE_NonTransformableExpressionTag,expr);

  // showSgExpression (cout, expr, "Called from ROSE_NonTransformableExpression::ROSE_NonTransformableExpression ( SgExpression* expr )");
   }

ROSE_TransformableNode* ROSE_NonTransformableExpression::transform()
   {
     printf ("Sorry not implemented, ROSE_NonTransformableExpression::Tranform() \n");
     ROSE_ABORT();

  // getRoseExpression()->transform();

     return this;
   }

// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_NonTransformableExpression::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_NonTransformableExpressionTag)
        {
          printf ("Error in ROSE_NonTransformableExpression::error(): ROSE_NonTransformableExpression object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_NonTransformableExpression::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_NonTransformableExpressionTag);
     return ROSE_TransformableNode::error();
   }
  // ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableUnaryOperatorExpression::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableUnaryOperatorExpressionTag)
        {
          printf ("Error in ROSE_TransformableUnaryOperatorExpression::error(): ROSE_TransformableUnaryOperatorExpression object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableUnaryOperatorExpression::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableUnaryOperatorExpressionTag);
     return ROSE_TransformableNode::error();
   }
ROSE_TransformableBinaryOperatorExpression::~ROSE_TransformableBinaryOperatorExpression ()
   {
   }

#if 0
ROSE_TransformableBinaryOperatorExpression::ROSE_TransformableBinaryOperatorExpression ()
   {
     ROSE_TransformableNode::setUp (ROSE_TransformableBinaryOperatorExpressionTag,NULL);
   }
#endif

ROSE_TransformableBinaryOperatorExpression::ROSE_TransformableBinaryOperatorExpression ( ROSE_Expression* expr )
   : ROSE_TransformableOperatorExpression(expr)
   {
     ROSE_ASSERT (expr != NULL);
     ROSE_TransformableNode::setUp (ROSE_TransformableBinaryOperatorExpressionTag,expr);
  // setLhsRoseExpression (expr->getLhsRoseExpression());
  // setRhsRoseExpression (expr->getRhsRoseExpression());
   }

ROSE_TransformableNode* ROSE_TransformableBinaryOperatorExpression::transform()
   {
     printf ("Sorry not implemented, ROSE_TransformableBinaryOperatorExpression::Tranform() \n");
     ROSE_ABORT();

     return NULL;
   }


// Member functions specific to ROSE_TransformableBinaryOperatorExpression

// Get/Set for the SAGE subtrees
ROSE_Expression* ROSE_TransformableBinaryOperatorExpression::getLhsRoseExpression()
   {
     ROSE_Expression* returnExpression = NULL;
  // returnExpression = ((SgBinaryOp*) getSageSubTree())->get_lhs_operand();
     returnExpression = ((ROSE_BinaryArrayOperator*) getRoseSubTree())->getLhsRoseExpression();

     ROSE_ASSERT (returnExpression != NULL);
     ROSE_ASSERT (!returnExpression->error());

     return returnExpression;
   }

void ROSE_TransformableBinaryOperatorExpression::setLhsRoseExpression ( ROSE_Expression* expr )
   {
     printf ("Sorry not implemented, ROSE_TransformableBinaryOperatorExpression::setLhsRoseExpression() \n");
     ROSE_ABORT();
   }

ROSE_Expression* ROSE_TransformableBinaryOperatorExpression::getRhsRoseExpression()
   {
     ROSE_Expression* returnExpression = NULL;

     returnExpression = ((ROSE_BinaryArrayOperator*) getRoseSubTree())->getRhsRoseExpression();
  // returnExpression = rhsTransformableExpression;
  // returnExpression = ((ROSE_Expression*) getRoseSubTree())->getRhsRoseExpression();

     ROSE_ASSERT (returnExpression != NULL);
     return returnExpression;
   }

void ROSE_TransformableBinaryOperatorExpression::setRhsRoseExpression ( ROSE_Expression* expr )
   {
     printf ("Sorry not implemented, ROSE_TransformableBinaryOperatorExpression::setRhsRoseExpression() \n");
     ROSE_ABORT();
   }

// Get/Set for the ROSE_Transformable subtrees
void ROSE_TransformableBinaryOperatorExpression::setLhsTransformableExpression ( ROSE_TransformableExpression* expr )
   {
     ROSE_ASSERT (expr != NULL);
     lhsTransformableExpression = expr;
   }

ROSE_TransformableExpression* ROSE_TransformableBinaryOperatorExpression::getLhsTransformableExpression ()
   {
     ROSE_ASSERT (lhsTransformableExpression != NULL);
     return lhsTransformableExpression;
   }

void ROSE_TransformableBinaryOperatorExpression::setRhsTransformableExpression ( ROSE_TransformableExpression* expr )
   {
     ROSE_ASSERT (expr != NULL);
     rhsTransformableExpression = expr;
   }

ROSE_TransformableExpression* ROSE_TransformableBinaryOperatorExpression::getRhsTransformableExpression ()
   {
     ROSE_ASSERT (rhsTransformableExpression != NULL);
     return rhsTransformableExpression;
   }

#if 0
SgBinaryOp* ROSE_TransformableBinaryOperatorExpression::getSageBinaryOperator()
   {
  // We need to verify that this has been set before we try to retrive it!
     ROSE_ASSERT (sageBinaryOperator != NULL);
     return sageBinaryOperator;
   }

void ROSE_TransformableBinaryOperatorExpression::setSageBinaryOperator ( SgBinaryOp* newSageBinaryOperator )
   {
  // I think we want to avoid letting the input be a NULL pointer
  // ROSE_ASSERT (sageBinaryOperator != NULL);
     sageBinaryOperator = newSageBinaryOperator;
   }
#endif


ROSE_TransformableNode* ROSE_TransformableBinaryOperatorExpression::assembleTransform ( 
     ROSE_NonTransformableExpression* lhs,
     ROSE_NonTransformableExpression* rhs )
   {
  // The role of this function is the take the ROSE_Transformable grammar objects and
  // use them from the tranformations of the lhs and rhs to assemble the transformation
  // for the ROSE_TransformableBinaryOperatorExpression object.

     ROSE_Expression* roseLhsExpression = lhs->getRoseExpression ();
     ROSE_ASSERT (roseLhsExpression != NULL);

     ROSE_Expression* roseRhsExpression = rhs->getRoseExpression ();
     ROSE_ASSERT (roseRhsExpression != NULL);
     
     ROSE_ASSERT (!roseLhsExpression->error());
     ROSE_ASSERT (!roseRhsExpression->error());

     ROSE_BinaryArrayOperator* roseLocalExpression = (ROSE_BinaryArrayOperator*) getRoseExpression();
     ROSE_ASSERT (roseLocalExpression != NULL);
     ROSE_C_Expression* roseRootExpression =
          (ROSE_C_Expression*) (roseLocalExpression->transform (roseLhsExpression,roseRhsExpression));
     ROSE_ASSERT (roseRootExpression != NULL);

  // Return a ROSE_NonTransformableExpression object
     ROSE_NonTransformableExpression* nonTransformableExpression = new ROSE_NonTransformableExpression (roseRootExpression);
     ROSE_ASSERT (nonTransformableExpression != NULL);

     return nonTransformableExpression;
   }


// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableBinaryOperatorExpression::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableBinaryOperatorExpressionTag)
        {
          printf ("Error in ROSE_TransformableBinaryOperatorExpression::error(): ROSE_TransformableBinaryOperatorExpression object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableBinaryOperatorExpression::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableBinaryOperatorExpressionTag);
     return ROSE_TransformableNode::error();
   }
ROSE_TransformableBinaryOperatorEquals::~ROSE_TransformableBinaryOperatorEquals ()
   {
   }

#if 0
ROSE_TransformableBinaryOperatorEquals::ROSE_TransformableBinaryOperatorEquals ()
   {
     ROSE_TransformableNode::setUp (ROSE_TransformableBinaryOperatorEqualsTag,NULL);
   }
#endif

ROSE_TransformableBinaryOperatorEquals::ROSE_TransformableBinaryOperatorEquals ( ROSE_Expression* expr )
   : ROSE_TransformableBinaryOperatorExpression(expr)
   {
     ROSE_ASSERT (expr != NULL);
     ROSE_TransformableNode::setUp (ROSE_TransformableBinaryOperatorEqualsTag,expr);
  // setLhsRoseExpression ();
  // setRhsRoseExpression ();
   }

ROSE_TransformableNode* 
ROSE_TransformableBinaryOperatorEquals::transform()
   {
  // This function calls the transform functions on the Lhs and Rhs expressions and then assembles the 
  // representative transformation for the ROSE_TransformableBinaryOperatorEquals object.

  // printf ("Sorry not implemented, ROSE_TransformableBinaryOperatorEquals::Tranform() \n");
  // ROSE_ABORT();

#if 1
  // get the lhs and rhs rose expressions (from the lover level array grammar)
  // ROSE_TransformableOperandExpression* localLhsTransformableOperandExpression = 
  //      (ROSE_TransformableOperandExpression*) getLhsTransformableExpression();
  // ROSE_TransformableExpression* localRhsTransformableExpression = 
  //      (ROSE_TransformableExpression*) getRhsTransformableExpression();
     ROSE_TransformableOperandExpression* localLhsTransformableOperandExpression = 
          (ROSE_TransformableOperandExpression*) getLhsTransformableExpression();
     ROSE_TransformableExpression* localRhsTransformableExpression = 
          getRhsTransformableExpression();
     ROSE_ASSERT (localLhsTransformableOperandExpression != NULL);
     ROSE_ASSERT (localRhsTransformableExpression != NULL);

     ROSE_ASSERT (!localLhsTransformableOperandExpression->error());
     ROSE_ASSERT (!localLhsTransformableOperandExpression->getRoseExpression()->error());
     ROSE_ASSERT (!localRhsTransformableExpression->error());
     ROSE_ASSERT (!localRhsTransformableExpression->getRoseExpression()->error());

  // Call tranform() member function of the lhs and rhs expressions
     ROSE_NonTransformableExpression* localLhsNonTransformableExpression = 
          (ROSE_NonTransformableExpression*) localLhsTransformableOperandExpression->transform();
     ROSE_NonTransformableExpression* localRhsNonTransformableExpression = 
          (ROSE_NonTransformableExpression*) localRhsTransformableExpression->transform();
     ROSE_ASSERT (localLhsNonTransformableExpression != NULL);
     ROSE_ASSERT (localRhsNonTransformableExpression != NULL);

     ROSE_ASSERT (!localLhsNonTransformableExpression->error());
     ROSE_ASSERT (!localLhsNonTransformableExpression->getRoseExpression()->error());
     ROSE_ASSERT (!localRhsNonTransformableExpression->error());
     ROSE_ASSERT (!localRhsNonTransformableExpression->getRoseExpression()->error());

  // The transform(expr,expr) is a virtual function and we have to cast the return pointer
  // virtual functions must have the same return type from all classes (a C++ rule).
     ROSE_NonTransformableExpression* nonTransformableExpression = (ROSE_NonTransformableExpression*)
          assembleTransform (localLhsNonTransformableExpression,localRhsNonTransformableExpression);
     ROSE_ASSERT (nonTransformableExpression != NULL);

     return nonTransformableExpression;
#else
  // ROSE_Expression* roseLhsExpression = localLhsNonTransformableExpression->getRoseExpression ();
  // ROSE_Expression* roseRhsExpression = localRhsNonTransformableExpression->getRoseExpression ();
  // ROSE_ASSERT (roseLhsExpression != NULL);
  // ROSE_ASSERT (roseRhsExpression != NULL);

  // The shortest most direct form is (we can use this once the have everything debugged):
  // ROSE_Expression* roseLhsExpression =  getLhsRoseExpression()->transform()->getRoseExpression ();

     printf ("transform the Lhs in ROSE_TransformableBinaryOperatorEquals::transform() \n");
     ROSE_Expression* roseLhsExpression = (ROSE_Expression*) getLhsRoseExpression()->transform();

     printf ("transform the Rhs in ROSE_TransformableBinaryOperatorEquals::transform() \n");
     ROSE_Expression* roseRhsExpression = (ROSE_Expression*) getRhsRoseExpression()->transform();

     printf ("transform the ARRAY ASSIGNMENT OPERATOR in ROSE_TransformableBinaryOperatorEquals::transform() \n");
  // Now assemble the pieces into a ROSE_Expression
  // ROSE_Expression* roseRootExpression = (ROSE_BinaryArrayOperatorEquals*) roseSubTree())->transform (roseLhsExpression,roseRhsExpression);
  // ROSE_C_Expression* roseRootExpression = 
  //      (ROSE_C_Expression*) ( ((ROSE_Expression*) getRoseSubTree())->transform (roseLhsExpression,roseRhsExpression));
  // ROSE_C_Expression* roseRootExpression =
  //      (ROSE_C_Expression*) (getRoseTransformation()->transform (roseLhsExpression,roseRhsExpression));

     ROSE_BinaryArrayOperator* roseLocalExpression = (ROSE_BinaryArrayOperator*) getRoseExpression();
     ROSE_ASSERT (roseLocalExpression != NULL);
     ROSE_C_Expression* roseRootExpression =
          (ROSE_C_Expression*) (roseLocalExpression->assembleTransform (roseLhsExpression,roseRhsExpression));

  // printf ("Exiting at base of ROSE_TransformableBinaryOperatorEquals::transform() \n");
  // ROSE_ABORT();

#if 1
  // Return a ROSE_NonTransformableExpression object
     ROSE_NonTransformableExpression* nonTransformableExpression = new ROSE_NonTransformableExpression (roseRootExpression);
     ROSE_ASSERT (nonTransformableExpression != NULL);

     return nonTransformableExpression;
#else
     return NULL;
#endif
#endif
   }

#if 0
  This is defined in the base class!
ROSE_TransformableNode* ROSE_TransformableBinaryOperatorEquals::transform ( 
     ROSE_NonTransformableExpression* lhs,
     ROSE_NonTransformableExpression* rhs )
   {
  // The role of this function is the take the ROSE_Transformable grammar objects and
  // use them from the tranformations of the lhs and rhs to assemble the transformation
  // for the ROSE_TransformableBinaryOperatorEquals object.

     ROSE_Expression* roseLhsExpression = lhs->getRoseExpression ();
     ROSE_ASSERT (roseLhsExpression != NULL);

     ROSE_Expression* roseRhsExpression = rhs->getRoseExpression ();
     ROSE_ASSERT (roseRhsExpression != NULL);
     
     ROSE_ASSERT (!roseLhsExpression->error());
     ROSE_ASSERT (!roseRhsExpression->error());

     ROSE_BinaryArrayOperator* roseLocalExpression = (ROSE_BinaryArrayOperator*) getRoseExpression();
     ROSE_ASSERT (roseLocalExpression != NULL);
     ROSE_C_Expression* roseRootExpression =
          (ROSE_C_Expression*) (roseLocalExpression->assembleTransform (roseLhsExpression,roseRhsExpression));
     ROSE_ASSERT (roseRootExpression != NULL);

  // Return a ROSE_NonTransformableExpression object
     ROSE_NonTransformableExpression* nonTransformableExpression = new ROSE_NonTransformableExpression (roseRootExpression);
     ROSE_ASSERT (nonTransformableExpression != NULL);

     return nonTransformableExpression;
   }
#endif

// Not clear if this is needed!
ROSE_TransformableExpression* ROSE_TransformableBinaryOperatorEquals::getLhsTransformableExpression()
   {
  // We want to define a special implementation of this function since 
  // we want to return a ROSE_TransformableOperandExpression*

  // The transformable binary operator equals grammar includes only a RHS so that the 
  // definition includes a Lhs ROSE_ARrayOperandExpression (not a more general lhs expression).
#if 0
     printf ("ERROR, the Lhs Operand does not exist in the case of a Transformable Binary Operator Equals! \n");
     ROSE_ABORT();

     return (ROSE_TransformableOperandExpression*) ROSE_TransformableBinaryOperatorExpression::getLhsTransformableExpression();
#else
 // The ROSE_TransformableBinaryOperatorExpression::getLhsTransformableExpression() contains an assertion that the pointer is NOT NULL!
 //  ROSE_ASSERT (ROSE_TransformableBinaryOperatorExpression::getLhsTransformableExpression() == NULL);
     ROSE_Expression* roseExpression = getRoseExpression()->getLhsRoseExpression();
     ROSE_TransformableOperandExpression* returnTransformableOperandExpression =
          new ROSE_TransformableOperandExpression (roseExpression);

     ROSE_ASSERT (returnTransformableOperandExpression != NULL);

     return returnTransformableOperandExpression;
#endif
   }

ROSE_BinaryArrayOperatorEquals* ROSE_TransformableBinaryOperatorEquals::getRoseExpression ()
   {
     ROSE_BinaryArrayOperatorEquals* roseBinaryOperator = (ROSE_BinaryArrayOperatorEquals*) getRoseSubTree();

     return roseBinaryOperator;
   }












// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableBinaryOperatorEquals::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableBinaryOperatorEqualsTag)
        {
          printf ("Error in ROSE_TransformableBinaryOperatorEquals::error(): ROSE_TransformableBinaryOperatorEquals object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableBinaryOperatorEquals::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableBinaryOperatorEqualsTag);
     return ROSE_TransformableNode::error();
   }
ROSE_TransformableBinaryOperatorNonAssignment::~ROSE_TransformableBinaryOperatorNonAssignment ()
   {
   }

#if 0
ROSE_TransformableBinaryOperatorNonAssignment::ROSE_TransformableBinaryOperatorNonAssignment ()
   {
     ROSE_TransformableNode::setUp (ROSE_TransformableBinaryOperatorNonAssignmentTag,NULL);
   }
#endif

ROSE_TransformableBinaryOperatorNonAssignment::ROSE_TransformableBinaryOperatorNonAssignment ( ROSE_Expression* expr )
   : ROSE_TransformableBinaryOperatorExpression(expr)
   {
     ROSE_ASSERT (expr != NULL);
     ROSE_TransformableNode::setUp (ROSE_TransformableBinaryOperatorNonAssignmentTag,expr);
   }

ROSE_TransformableNode* ROSE_TransformableBinaryOperatorNonAssignment::transform()
   {
  // printf ("Sorry not implemented, ROSE_TransformableBinaryOperatorNonAssignment::Tranform() \n");
  // ROSE_ABORT();

     ROSE_TransformableExpression* localLhsTransformableExpression = 
          getLhsTransformableExpression();
     ROSE_TransformableExpression* localRhsTransformableExpression = 
          getRhsTransformableExpression();
     ROSE_ASSERT (localLhsTransformableExpression != NULL);
     ROSE_ASSERT (localRhsTransformableExpression != NULL);

     ROSE_ASSERT (!localLhsTransformableExpression->error());
     ROSE_ASSERT (!localLhsTransformableExpression->getRoseExpression()->error());
     ROSE_ASSERT (!localRhsTransformableExpression->error());
     ROSE_ASSERT (!localRhsTransformableExpression->getRoseExpression()->error());

  // Call tranform() member function of the lhs and rhs expressions
     ROSE_NonTransformableExpression* localLhsNonTransformableExpression = 
          (ROSE_NonTransformableExpression*) localLhsTransformableExpression->transform();
     ROSE_NonTransformableExpression* localRhsNonTransformableExpression = 
          (ROSE_NonTransformableExpression*) localRhsTransformableExpression->transform();
     ROSE_ASSERT (localLhsNonTransformableExpression != NULL);
     ROSE_ASSERT (localRhsNonTransformableExpression != NULL);

     ROSE_ASSERT (!localLhsNonTransformableExpression->error());
     ROSE_ASSERT (!localLhsNonTransformableExpression->getRoseExpression()->error());
     ROSE_ASSERT (!localRhsNonTransformableExpression->error());
     ROSE_ASSERT (!localRhsNonTransformableExpression->getRoseExpression()->error());

  // The transform(expr,expr) is a virtual function and we have to cast the return pointer
  // virtual functions must have the same return type from all classes (a C++ rule).
     ROSE_NonTransformableExpression* nonTransformableExpression = (ROSE_NonTransformableExpression*)
          assembleTransform (localLhsNonTransformableExpression,localRhsNonTransformableExpression);
     ROSE_ASSERT (nonTransformableExpression != NULL);

#if 0
     printf ("Exiting at base of ROSE_TransformableBinaryOperatorNonAssignment::transform() \n");
     ROSE_ABORT();
#endif

     return nonTransformableExpression;
   }

#if 0
ROSE_TransformableNode* ROSE_TransformableBinaryOperatorNonAssignment::transform (
     ROSE_NonTransformableExpression* lhs,
     ROSE_NonTransformableExpression* rhs )
   {
  // The role of this function is the take the ROSE_Transformable grammar objects and
  // use them from the tranformations of the lhs and rhs to assemble the transformation
  // for the ROSE_TransformableBinaryOperatorNonAssignment object.

     ROSE_Expression* roseLhsExpression = lhs->getRoseExpression ();
     ROSE_ASSERT (roseLhsExpression != NULL);

     ROSE_Expression* roseRhsExpression = rhs->getRoseExpression ();
     ROSE_ASSERT (roseRhsExpression != NULL);
     
     ROSE_ASSERT (!roseLhsExpression->error());
     ROSE_ASSERT (!roseRhsExpression->error());

     ROSE_BinaryArrayOperator* roseLocalExpression = (ROSE_BinaryArrayOperator*) getRoseExpression();
     ROSE_ASSERT (roseLocalExpression != NULL);
     ROSE_C_Expression* roseRootExpression =
          (ROSE_C_Expression*) (roseLocalExpression->assembleTransform (roseLhsExpression,roseRhsExpression));
     ROSE_ASSERT (roseRootExpression != NULL);

  // Return a ROSE_NonTransformableExpression object
     ROSE_NonTransformableExpression* nonTransformableExpression = new ROSE_NonTransformableExpression (roseRootExpression);
     ROSE_ASSERT (nonTransformableExpression != NULL);

     return nonTransformableExpression;
   }
#endif




// ********************************************************
// member functions common across all array grammar objects
// ********************************************************
Boolean ROSE_TransformableBinaryOperatorNonAssignment::error()
   {
  // Put error checking here

     ROSE_ASSERT (this != NULL);
     if (getVariant() != ROSE_TransformableBinaryOperatorNonAssignmentTag)
        {
          printf ("Error in ROSE_TransformableBinaryOperatorNonAssignment::error(): ROSE_TransformableBinaryOperatorNonAssignment object has a %s variant \n",arrayGrammarTerminalNames[getVariant()].name);
       // printf ("Error in ROSE_TransformableBinaryOperatorNonAssignment::error() \n");
          ROSE_ABORT();
        }

     ROSE_ASSERT (getVariant() == ROSE_TransformableBinaryOperatorNonAssignmentTag);
     return ROSE_TransformableNode::error();
   }
