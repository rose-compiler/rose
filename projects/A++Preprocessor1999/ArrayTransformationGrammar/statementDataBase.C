#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"

#include "ROSE_Transformations.h"

// include "transformation_3.h"

StatementDataBase::~StatementDataBase ()
   {
     numberOfRhsOperands = 0;
     lhsOperand = NULL;
     for (int i=0; i < MAX_NUMBER_OF_OPERANDS; i++)
          rhsOperand[i] = NULL;
   }

StatementDataBase::StatementDataBase ( ROSE_TransformableStatement* transformableStatement )
   {
  // Call the traverse and build the data base
     dataBaseTransformableStatement = transformableStatement;

     numberOfRhsOperands = 0;
     lhsOperand = NULL;
     for (int i=0; i < MAX_NUMBER_OF_OPERANDS; i++)
          rhsOperand[i] = NULL;

  // ROSE_ASSERT (transformableStatement->getVariant() == ROSE_TransformableStatementTag);
  // If this is a statement block then return an error
  // traverse (this,transformableStatement);
   }

void
StatementDataBase::display( const char* label )
   {
     ROSE_ASSERT (dataBaseTransformableStatement != NULL);

#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 0)
        {
          ROSE_Statement* roseStatement = (ROSE_Statement*) dataBaseTransformableStatement->getRoseSubTree();
          SgStatement* sageStatement = roseStatement->getSageStatement();
          ROSE_ASSERT (sageStatement != NULL);
          printf ("Inside of StatementDataBase::display ( SgStatement* stmt ) File: %s  Line: %d \n",
               ROSE::getFileName(sageStatement),ROSE::getLineNumber(sageStatement));
        }
#endif

  // Call the traverse and build the data base

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
        {
          printf ("numberOfRhsOperands = %d \n",numberOfRhsOperands);
          printf ("lhsOperand = %s \n", (lhsOperand == NULL) ? "NULL POINTER" : "VALID POINTER");
          for (int i=0; i < numberOfRhsOperands+3; i++)
               printf ("rhsOperand[%d] = %s \n",i,(rhsOperand[i] == NULL) ? "NULL POINTER" : "VALID POINTER");
        }
   }

int
StatementDataBase::getNumberOfRhsOperands()
   {
     ROSE_ASSERT (numberOfRhsOperands > 0);
     ROSE_ASSERT (numberOfRhsOperands < 10); // Temporary code!
     return numberOfRhsOperands;
   }

void
StatementDataBase::setStatementDimension()
   {
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Compute the statement dimension in StatementDataBase::getStatementDimension() \n");

     ROSE_ASSERT (numberOfRhsOperands > 0);

     int lhsNumberOfDimensions = lhsOperand->getStatementDimension();

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("lhsNumberOfDimensions = %d \n",lhsNumberOfDimensions);
     
     int numberOfDimensions = lhsNumberOfDimensions;

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("getNumberOfRhsOperands() = %d \n",getNumberOfRhsOperands());

     for (int i=0; i < getNumberOfRhsOperands(); i++)
        {
          ROSE_ASSERT (rhsOperand[i] != NULL);
	
          int rhsDimension = rhsOperand[i]->getStatementDimension();

          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
               printf ("rhsDimension = %d \n",rhsDimension);
     
          if (rhsDimension == 0)
             {
            // It is not an error for the lhsNumberOfDimensions to be different if the rhsDimension is 0
            // ROSE_ASSERT (lhsNumberOfDimensions == rhsDimension);
	     }
            else
	     {
               if (lhsNumberOfDimensions > 0) 
                  {
                    ROSE_ASSERT (lhsNumberOfDimensions == rhsDimension); 
                  }
                 else
                  {
                    numberOfDimensions = rhsDimension;
                  }
	     }
        }

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("LEAVING: Compute the statement dimension in StatementDataBase::getStatementDimension() (numberOfDimensions = %d)\n",numberOfDimensions);
     statementDimension = numberOfDimensions;
   }


List<SgName>*
StatementDataBase::computeUniqueOperands()
   {
     ROSE_ASSERT (numberOfRhsOperands > 0);
     ROSE_ASSERT (numberOfRhsOperands < 10); // Temporary code!

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Inside of StatementDataBase::computeUniqueOperands() \n");

     List<SgName> * returnList = new List<SgName>;
     ROSE_ASSERT (returnList != NULL);
     SgName* lhsName = new SgName(lhsOperand->getVariableName());
     ROSE_ASSERT (lhsName != NULL);

     returnList->addElement(*lhsName,0);
     ROSE_ASSERT (returnList->getLength() == 1);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("lhsName = %s \n",(*returnList)[0].str());

#if 1
     for (int i=0; i < numberOfRhsOperands; i++)
        {
          SgName currentVariableName = rhsOperand[i]->getVariableName();
       // printf ("currentVariableName = %s \n",currentVariableName.str());
          ROSE_ASSERT (currentVariableName.str() != NULL);

          Boolean found = FALSE;
       // if (ROSE::isSameName(currentVariableName,lhsOperand->getVariableName()) == TRUE)
          if ( currentVariableName == lhsOperand->getVariableName() ||
               currentVariableName == SgName("default_C_ExpressionName") )
             {
               if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                    printf ("Found %s on the lhs \n",currentVariableName.str());
               found = TRUE;
             }
            else
             {
               if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                    printf ("name: %s not found on the lhs \n",currentVariableName.str());
               for (int j=i-1; j >= 0; j--)
                  {
                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         printf ("Looking back through the history of what operands we have! \n");
                 // if (ROSE::isSameName(currentVariableName,rhsOperand[j]->getVariableName()) == TRUE)
                    if ( currentVariableName == rhsOperand[j]->getVariableName() )
                       {
                         if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                              printf ("Found %s on the rhs (position=%d) \n",currentVariableName.str(),j);
                         found = TRUE;
                       }
                      else
                       {
                         if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                              printf ("Name: %s NOT found on the rhs (position=%d) \n",currentVariableName.str(),j);
                       }
                  }
             }

        if (!found)
           {
             if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                  printf ("Add %s to list of unique elements \n",currentVariableName.str());
             ROSE_ASSERT (returnList != NULL);
             SgName* rhsName = new SgName(rhsOperand[i]->getVariableName());
             ROSE_ASSERT (rhsName != NULL);
             returnList->addElement(rhsName,i+1);
             if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                  printf ("AFTER addElement: List element %d: Name = %s \n",i+1,(*returnList)[i+1].str());
           }
          else
           {
             if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                  printf ("Skip adding %s to list of unique elements \n",currentVariableName.str());
             ROSE_ASSERT (returnList != NULL);
           }
        }

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          for (int k=0; k < returnList->getLength(); k++)  
               printf ("List element %d: Name = %s \n",k,(*returnList)[k].str());
#endif

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("returnList->getLength() = %d \n",returnList->getLength());

     ROSE_ASSERT (returnList->getLength() <= 2);
     ROSE_ASSERT (returnList != NULL);
     return returnList;
   }


void
StatementDataBase::traverse ( ROSE_TransformableStatement* transformableStatement )
   {
  // This function implements part of the data base mechanism

     const int LOCAL_ROSE_DEBUG = ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) ) ? TRUE : FALSE;
     const char* notImplemented = " -- Not implemented in StatementDataBase::traverse(ROSE_TransformableStatement*)";

     ROSE_ASSERT (transformableStatement != NULL);
     ROSE_ASSERT (transformableStatement->localDataBase == NULL);

     ROSE_TransformableGrammarVariants expressionVariant = transformableStatement->getVariant();

     StatementDataBase* localStatementDataBase = NULL;
     switch (expressionVariant)
        {
          case ROSE_TransformableStatementTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableStatementTag" << notImplemented << endl;
               localStatementDataBase = new StatementDataBase(transformableStatement);
               ROSE_ASSERT (localStatementDataBase != NULL);
               transformableStatement->localDataBase = localStatementDataBase;
               traverse (localStatementDataBase,transformableStatement->getExpression());

               ROSE_ASSERT (localStatementDataBase != NULL);
               localStatementDataBase->display("in traverse()");
               break;
             }
          case ROSE_TransformableNodeTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableNodeTag" << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_NonTransformableStatementTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_NonTransformableStatementTag" << notImplemented << endl;
            // ROSE_ABORT();
               break;
             }
          case ROSE_TransformableStatementBlockTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableStatementBlockTag" << notImplemented << endl;
            // ROSE_ABORT();

               ROSE_TransformableStatementBlock* transformableStatementBlock = 
                    (ROSE_TransformableStatementBlock*) transformableStatement;

            // Now we go through the ROSE_Statement object in the ROSE_StatmentBlock and 
            // build ROSE_TransformableStatement objects from each one and place them into 
            // our new ROSE_TransformableStatementBlock object (part of the transformable grammar).
               for (int i=0; i < transformableStatementBlock->getNumberOfStatements(); i++)
                  {
                    traverse (transformableStatementBlock->operator[](i));
                  }
               break;
             }
          default:
               printf ("ERROR: default reached in ArrayAssignmentUsingTransformationGrammar::"
                       "StatementDataBase::traverse(ROSE_TransformableStatement*) \n");
               ROSE_ABORT();
        }

  // Initialize the internal statment dimension
     if (localStatementDataBase != NULL)
          localStatementDataBase->setStatementDimension();
   }


void
StatementDataBase::traverse ( StatementDataBase* localStatementDataBase, ROSE_TransformableExpression* transformableExpression )
   {
  // This function implements part of the data base mechanism

     const int LOCAL_ROSE_DEBUG = ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) ) ? TRUE : FALSE;
     const char* notImplemented = " -- Not implemented in StatementDataBase::traverse(StatementDataBase*, ROSE_TransformableExpression*)";

     ROSE_ASSERT (localStatementDataBase  != NULL);
     ROSE_ASSERT (transformableExpression != NULL);
     ROSE_TransformableGrammarVariants expressionVariant = transformableExpression->getVariant();

     switch (expressionVariant)
        {
          case ROSE_TransformableNodeTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableNodeTag" << notImplemented << endl;
               ROSE_ABORT();
               break;
             }
          case ROSE_TransformableExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableExpressionTag" << notImplemented << endl;
               ROSE_ABORT();
               break;
             }
          case ROSE_TransformableOperatorExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableOperatorExpressionTag" << notImplemented << endl;
               ROSE_ABORT();
               break;
             }
          case ROSE_TransformableUnaryOperatorExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableUnaryOperatorExpressionTag" << notImplemented << endl;
               ROSE_ABORT();
               break;
             }
          case ROSE_TransformableBinaryOperatorExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableBinaryOperatorExpressionTag" << notImplemented << endl;
               ROSE_ABORT();
               break;
             }
          case ROSE_TransformableBinaryOperatorEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableBinaryOperatorEqualsTag" << notImplemented << endl;
               ROSE_TransformableBinaryOperatorEquals* transformableBinaryEquals = 
                    (ROSE_TransformableBinaryOperatorEquals*) transformableExpression;
#if 0
               ROSE_Expression* roseExpression = transformableBinaryEquals->getRoseExpression()->getLhsRoseExpression();
	       ROSE_ASSERT (roseExpression != NULL);
	       ROSE_ASSERT (roseExpression->getVariant() == ROSE_ArrayOperandExpressionTag);

               OperandDataBase* lhsOperandDataBase = new OperandDataBase ( roseExpression );
#else
               ROSE_TransformableOperandExpression* lhsOperand = (ROSE_TransformableOperandExpression*) transformableBinaryEquals->getLhsTransformableExpression();
               OperandDataBase* lhsOperandDataBase = new OperandDataBase ( lhsOperand );
#endif
               ROSE_ASSERT (lhsOperandDataBase != NULL);

               localStatementDataBase->lhsOperand = lhsOperandDataBase;
               traverse (localStatementDataBase,transformableBinaryEquals->getRhsTransformableExpression());
            // ROSE_ABORT();
               break;
             }
          case ROSE_TransformableBinaryOperatorNonAssignmentTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableBinaryOperatorNonAssignmentTag" << notImplemented << endl;
            // ROSE_ABORT();
               ROSE_TransformableBinaryOperatorExpression* transformableBinaryOperator = 
                    (ROSE_TransformableBinaryOperatorExpression*) transformableExpression;
               traverse (localStatementDataBase,transformableBinaryOperator->getLhsTransformableExpression());
               traverse (localStatementDataBase,transformableBinaryOperator->getRhsTransformableExpression());
               break;
             }
          case ROSE_TransformableOperandExpressionTag: 
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableOperandExpressionTag" << notImplemented << endl;
            // ROSE_ABORT();
               ROSE_TransformableOperandExpression* operand = (ROSE_TransformableOperandExpression*) transformableExpression;
               OperandDataBase* operandDataBase = new OperandDataBase ( operand );
               ROSE_ASSERT (operandDataBase != NULL);

               localStatementDataBase->rhsOperand[localStatementDataBase->numberOfRhsOperands] = operandDataBase;
               localStatementDataBase->numberOfRhsOperands++;
               break;
             }
          case ROSE_NonTransformableExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_NonTransformableExpressionTag" << notImplemented << endl;
               ROSE_ABORT();
               break;
             }
          case ROSE_TRANSFORMABLE_LAST_TAG:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TRANSFORMABLE_LAST_TAG" << notImplemented << endl;
               ROSE_ABORT();
               break;
             }
          default:
               printf ("ERROR: default reached in ArrayAssignmentUsingTransformationGrammar::"
                       "StatementDataBase::traverse(ROSE_TransformableExpression*) \n");
               ROSE_ABORT();
	}
   }


