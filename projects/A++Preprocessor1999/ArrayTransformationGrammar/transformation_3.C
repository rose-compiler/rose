#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"

#include "ROSE_Transformations.h"

// include "transformation_3.h"

#if 1
#if 0
extern struct 
   { 
     ROSE_GrammarVariants variant; 
     char *name;                   
   } arrayGrammarTerminalNames[134];
#else
extern struct 
   { 
     ROSE_GrammarVariants variant; 
     char *name;
   } arrayGrammarTerminalNames[143];
#endif
#endif

// Global variable which seems to be required (I forget exactly why)
ArrayAssignmentUsingTransformationGrammar* globalArrayAssignmentUsingTransformationGrammar = NULL;

ArrayAssignmentUsingTransformationGrammar::~ArrayAssignmentUsingTransformationGrammar ()
   {
   // No variables to delete
   }

ArrayAssignmentUsingTransformationGrammar::ArrayAssignmentUsingTransformationGrammar ( SgFile *file )
   : ArrayAssignmentUsingGrammar(file)
   {
  // It is annoying that this must be initialized with the relative path
  // but we can fix that later (I guess)!

  // We now can test for the filename only (without the path)
  // char* localHeaderFileName = "../src/Transform_3/ROSE_TRANSFORMATION_SOURCE_3.h";
     char* localHeaderFileName = "ROSE_TRANSFORMATION_SOURCE_3.h";
     setTemplateHeaderFileName (localHeaderFileName);

#if 1
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("In ArrayAssignmentUsingTransformationGrammar constructor: headerFileName = %s \n",getTemplateHeaderFileName());
#endif
   }

// This function is a Virtual function in the base class (ROSE_TransformationBase)
ROSE_TransformableStatementBlock*
ArrayAssignmentUsingTransformationGrammar::pass( ROSE_StatementBlock* inputRoseProgramTree )
   {
  // A Pass is what calls all the functions to parse and transform a given file
     ROSE_ASSERT (inputRoseProgramTree != NULL);
     ROSE_TransformableStatementBlock* roseTransformableProgramTree = traverseProgramTree( inputRoseProgramTree );
     ROSE_ASSERT (roseTransformableProgramTree != NULL);

  // Now setup the data base
     setUpDataBase(roseTransformableProgramTree);

  // Now read in the transformation templates
     readTargetAndTransformationTemplates();

#if 0
  // Now perform the transformations
     if (ROSE::skip_transformation == FALSE)
        {
       // Make sure we don't call this for now while we debug the parsing into higher level grammars
       // ROSE_ASSERT (1 == 2);
          transform (roseTransformableProgramTree);
        }
#else
     transform (roseTransformableProgramTree);
#endif

     return roseTransformableProgramTree;
   }

// It is not clear yet how much of this function can be placed into the base class
// we will pursue that later when we have one transformation working and we want
// to develop more transformations.
ROSE_TransformableStatementBlock*
ArrayAssignmentUsingTransformationGrammar::traverseProgramTree ( ROSE_StatementBlock* roseProgramTree )
   {
  // Here we experiment with another approach implementing a new grammar specific
  // to our specific transformation.  If this works then we will place most of what is currently
  // in transformation_2 into the base class.

     ROSE_TransformableStatementBlock* roseStatementBlock = (ROSE_TransformableStatementBlock*) parse (roseProgramTree);
     ROSE_ASSERT (roseStatementBlock != NULL);
     return roseStatementBlock;
   }

ROSE_TransformableStatement*
ArrayAssignmentUsingTransformationGrammar::parse ( ROSE_Statement* roseStatement )
   {
  // We must handle the cases of statements in our grammar
  // If might be better to have the grammar represent ROSE_C_Statement 
  // or ROSE_ArrayStatement

  // A parser is always written in terms of its grammar which it defines (or implements depending upon ones point of view).
  // This parser is parsing in terms of the grammar that defines our transformation (in this case the array assignment statement)
  // it could recognize that grammar directly or if it is simple enough, in terms of the lower level grammar (the array grammar)
  // The parsing of statements is particularly simple so that we CAN do this.  so we don't need to implement the recognition of
  // the non-terminals in our higher level grammar directly.  The case of parsing the expression is more complex and so we are required
  // to handle this case using the more formal process (of recognizing the higher level grammar in terms of the lower level grammar).

     ROSE_TransformableStatement* returnNode = NULL;

     ROSE_ASSERT (roseStatement != NULL);

#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 0)
        {
          SgStatement* sageStatement = roseStatement->getSageStatement();
          ROSE_ASSERT (sageStatement != NULL);
          printf ("Inside of ArrayAssignmentUsingTransformationGrammar::parse ( SgStatement* stmt ) File: %s  Line: %d \n",
               ROSE::getFileName(sageStatement),ROSE::getLineNumber(sageStatement));
        }
#endif

     ROSE_GrammarVariants statementVariant = roseStatement->getVariant();

#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 0)
        {
          printf ("statementVariant = %s \n",arrayGrammarTerminalNames[statementVariant].name);
        }
#endif

     switch (statementVariant)
      {
        case ROSE_C_StatementTag:
        case ROSE_ReturnStatementTag:
     // case ROSE_VariableDeclarationTag:  // this is the base class of the two following types
        case ROSE_C_VariableDeclarationTag:
        case ROSE_ArrayVariableDeclarationTag:
           {
          // returnNode = new ROSE_NonTransformableStatement (roseStatement);
          // ROSE_ASSERT (returnNode != NULL);

             ROSE_Statement* notTransformableStatement = roseStatement;
             ROSE_ASSERT (notTransformableStatement != NULL);

          // Our grammar has been extended to handle this error
             returnNode = new ROSE_NonTransformableStatement (notTransformableStatement);
          // returnNode = NULL;
             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        case ROSE_ExpressionStatementTag:
           {
             ROSE_TransformableStatement* transformableStatement = new ROSE_TransformableStatement (roseStatement);
	     ROSE_ASSERT (transformableStatement != NULL);

          // This is the error recovery mechanism
             if (transformableStatement->error())
                {
                  printf ("Error: transformableStatement->error() == TRUE \n");
                  ROSE_ABORT();

                  returnNode = NULL;
                }
               else
                {
                  ROSE_ExpressionStatement* roseExpressionStatement = isRoseExpressionStatement (roseStatement);
                  ROSE_Expression* roseExpression = roseExpressionStatement->getRoseExpression();
                  ROSE_ASSERT (roseExpression != NULL);

               // ROSE_TransformableExpression* transformableExpression = (ROSE_TransformableExpression*) parse (roseExpression);
                  ROSE_TransformableBinaryOperatorEquals* transformableBinaryEqualsExpression = 
                       (ROSE_TransformableBinaryOperatorEquals*) parse (roseExpression);
                  ROSE_ASSERT (transformableBinaryEqualsExpression != NULL);

               // Error recovery mechanism
                  if (transformableBinaryEqualsExpression->error())
                     {
                    // Skip the calls to the destructors for now and accept the memory leak
                    // delete transformableStatement;
                       printf ("Error: transformableBinaryEqualsExpression->error() == TRUE \n");
                       ROSE_ABORT();

                       transformableBinaryEqualsExpression = NULL;
                     }
                    else
                     {
                       transformableStatement->setExpression(transformableBinaryEqualsExpression);
                    // transformableStatement->setEqualsOperator (NULL);
                     }

                  returnNode = transformableStatement;
                  ROSE_ASSERT (returnNode != NULL);
                }

             break;
           }

        case ROSE_WhereStatementTag:
           {
             ROSE_StatementBlock* whereStatementBody = ((ROSE_WhereStatement*) roseStatement)->getRoseWhereStatementBlock();
             ROSE_ASSERT (whereStatementBody != NULL);
             returnNode = parse(whereStatementBody);
             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        case ROSE_ElseWhereStatementTag:
           {
             ROSE_StatementBlock* elseWhereStatementBody = ((ROSE_ElseWhereStatement*) roseStatement)->getRoseElseWhereStatementBlock();
             ROSE_ASSERT (elseWhereStatementBody != NULL);
             returnNode = parse(elseWhereStatementBody);
             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        case ROSE_DoWhileStatementTag:
           {
#if 0
             ROSE_StatementBlock* doStatementBody = roseStatement->getRoseStatementBlock();
             ROSE_ASSERT (doStatementBody != NULL);
             returnNode = parse(doStatementBody);
#endif
             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        case ROSE_WhileStatementTag:
           {
#if 0
             ROSE_StatementBlock* whileStatementBody = roseStatement->getRoseStatementBlock();
             ROSE_ASSERT (whileStatementBody != NULL);
             returnNode = parse(whileStatementBody);
#endif
             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        case ROSE_ForStatementTag:
           {
             ROSE_StatementBlock* forStatementBody = ((ROSE_ForStatement*) roseStatement)->getRoseForStatementBlock();
             ROSE_ASSERT (forStatementBody != NULL);
             returnNode = parse(forStatementBody);
             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        case ROSE_IfStatementTag:
           {
             ROSE_StatementBlock* ifStatementBody_True = ((ROSE_IfStatement*) roseStatement)->getTrueRoseStatementBlock();
             ROSE_ASSERT (ifStatementBody_True != NULL);
             returnNode = parse(ifStatementBody_True);

             ROSE_Statement* ifStatementBody_False = ((ROSE_IfStatement*) roseStatement)->getFalseRoseStatementBlock();
             if (ifStatementBody_False != NULL)
                {
                  returnNode = parse(ifStatementBody_False);
                }

             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        case ROSE_StatementBlockTag:
           {
             ROSE_StatementBlock* roseStatementBlock = (ROSE_StatementBlock*) roseStatement;
             ROSE_ASSERT (roseStatementBlock != NULL);

             ROSE_TransformableStatementBlock* transformableStatementBlock = new ROSE_TransformableStatementBlock (roseStatementBlock);
             ROSE_ASSERT (transformableStatementBlock != NULL);
	     
             if (transformableStatementBlock->error())
                {
                  printf ("Error: transformableStatementBlock->error() == TRUE \n");
                  ROSE_ABORT();

                  returnNode = NULL;
                }
               else
                {
               // Now we go through the ROSE_Statement object in the ROSE_StatmentBlock and 
               // build ROSE_TransformableStatement objects from each one and place them into 
               // our new ROSE_TransformableStatementBlock object (part of the transformable grammar).
                  for (int i=0; i < roseStatementBlock->getNumberOfStatements(); i++)
                     {
                       ROSE_TransformableStatement* transformationStatementWithinBlock = parse (roseStatementBlock->operator[](i));
                       ROSE_ASSERT (transformationStatementWithinBlock != NULL);

                    // Note that if a statement is not transformable the parser returns a 
                    // ROSE_NonTransformableStatement object (part of our grammar)
                       transformableStatementBlock->addTransformableStatementToBlock(transformationStatementWithinBlock);
                     }

                  returnNode = transformableStatementBlock;
                }

             ROSE_ASSERT (returnNode != NULL);
             break;
           }

        default:
             printf ("ERROR: default reached in ArrayAssignmentUsingTransformationGrammar::parse ( ROSE_Statement* roseStatement ) \n");
             ROSE_ABORT();
      }

     ROSE_ASSERT (returnNode != NULL);
     return returnNode;
   }

ROSE_TransformableExpression*
ArrayAssignmentUsingTransformationGrammar::parse ( ROSE_Expression* roseExpression )
   {
     int LOCAL_ROSE_DEBUG = ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) ) ? TRUE : FALSE;
     const char* notImplemented = " -- Not implemented in ArrayAssignmentUsingTransformationGrammar::parse ( ROSE_Expression* expr )";

     ROSE_TransformableExpression* returnExpression = NULL;
     ROSE_ASSERT (roseExpression != NULL);

  // printf ("Sorry, not implemented! ArrayAssignmentUsingTransformationGrammar::parse ( ROSE_Expression* roseExpression ) \n");
  // ROSE_ABORT();

     ROSE_TransformableGrammarVariants expressionVariant = getVariant ( roseExpression );

     switch (expressionVariant)
        {
          case ROSE_TransformableNodeTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableNodeTag" << notImplemented << endl;
               break;
             }
          case ROSE_TransformableStatementTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableStatementTag" << notImplemented << endl;
               break;
             }
          case ROSE_NonTransformableStatementTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_NonTransformableStatementTag" << notImplemented << endl;
               break;
             }
          case ROSE_TransformableStatementBlockTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableStatementBlockTag" << notImplemented << endl;
               break;
             }
          case ROSE_TransformableExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_TransformableOperatorExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableOperatorExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_TransformableUnaryOperatorExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableUnaryOperatorExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_TransformableBinaryOperatorExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableBinaryOperatorExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_TransformableBinaryOperatorEqualsTag:
             {
            // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableBinaryOperatorEqualsTag" << notImplemented << endl;

               ROSE_TransformableBinaryOperatorEquals* equalsOperator = new ROSE_TransformableBinaryOperatorEquals (roseExpression);
	       ROSE_ASSERT (equalsOperator != NULL);

               if (equalsOperator->error())
                  {
                    returnExpression = NULL;
                  }
                 else
                  {
                 // The lhs operand is part of the definition of the ROSE_TransformableBinaryOperatorEquals grammar (object)
                 // so we need only parse the rhs ROSE_Expression
                    ROSE_Expression* rhsRoseExpression = equalsOperator->getRhsRoseExpression();
                    ROSE_ASSERT (rhsRoseExpression != NULL);

                    ROSE_TransformableExpression* rhsExpression = parse (rhsRoseExpression);

                    equalsOperator->setRhsTransformableExpression(rhsExpression);
                    returnExpression = equalsOperator;
                  }

	       ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case ROSE_TransformableBinaryOperatorNonAssignmentTag:
             {
            // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableBinaryOperatorNonAssignmentTag" << notImplemented << endl;

               ROSE_TransformableBinaryOperatorNonAssignment* nonAssignmentOperator = new ROSE_TransformableBinaryOperatorNonAssignment (roseExpression);
	       ROSE_ASSERT (nonAssignmentOperator != NULL);

               if (nonAssignmentOperator->error())
                  {
                    returnExpression = NULL;
                  }
                 else
                  {
                 // In this case we have to parse both the lhs and the rhs!
                    ROSE_Expression* lhsRoseExpression = nonAssignmentOperator->getLhsRoseExpression();
                    ROSE_Expression* rhsRoseExpression = nonAssignmentOperator->getRhsRoseExpression();
                    ROSE_ASSERT (lhsRoseExpression != NULL);
                    ROSE_ASSERT (rhsRoseExpression != NULL);

                    ROSE_TransformableExpression* lhsExpression = parse (lhsRoseExpression);
                    ROSE_TransformableExpression* rhsExpression = parse (rhsRoseExpression);

                    nonAssignmentOperator->setLhsTransformableExpression(lhsExpression);
                    nonAssignmentOperator->setRhsTransformableExpression(rhsExpression);
                    returnExpression = nonAssignmentOperator;
                  }

	       ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case ROSE_TransformableOperandExpressionTag: 
             {
           // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TransformableOperandExpressionTag" << notImplemented << endl;

               if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                    printf ("arrayGrammarTerminalNames[roseExpression->getVariant()].name = %s \n",
                         arrayGrammarTerminalNames[roseExpression->getVariant()].name);

               ROSE_TransformableOperandExpression* operand = new ROSE_TransformableOperandExpression (roseExpression);
               ROSE_ASSERT (operand != NULL);
	       
               if (operand->error())
                  {
                    returnExpression = NULL;
                  }
                 else
                  {
                 // This this is a leaf of the ROSE tree we can not recurse further
                    returnExpression = operand;
                  }

               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case ROSE_NonTransformableExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_NonTransformableExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_TRANSFORMABLE_LAST_TAG:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_TRANSFORMABLE_LAST_TAG" << notImplemented << endl;
               break;
             }

          default:
               printf ("ERROR: default reached in parse (expr) \n");
               ROSE_ABORT();
        }

     ROSE_ASSERT (returnExpression != NULL);
     return returnExpression;
   }


ROSE_TransformableGrammarVariants
ArrayAssignmentUsingTransformationGrammar::getVariant ( ROSE_Node* roseNode )
   {
  // The purpose of this function is the return the transformableGrammar variant from 
  // the ROSE Grammar subtree (the array grammar).  In most case this is a fairly trivial
  // matter, later use of this mechanism (separating the selection of the transformation grammar
  // from the ROSE grammar) will make more sophisticated use of this mechanism.

     int LOCAL_ROSE_DEBUG = 1;
     const char* notImplemented = " -- Not implemented in ArrayAssignmentUsingTransformationGrammar::getVariant ( ROSE_Node* roseNode )";

     ROSE_ASSERT (roseNode != NULL);

     ROSE_TransformableGrammarVariants returnVariant = ROSE_TRANSFORMABLE_LAST_TAG;

#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 0)
        {
          printf ("roseNodeVariant = %s \n",arrayGrammarTerminalNames[roseNode->getVariant()].name);
        }
#endif

     switch (roseNode->getVariant())
        {
          case ROSE_C_ExpressionTag:
             {
            // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_C_ExpressionTag" << notImplemented << endl;

               returnVariant = ROSE_TransformableOperandExpressionTag;
               break;
             }
          case ROSE_UserFunctionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UserFunctionTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorMinusTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorMinusTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorPlusTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPlusTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorPrefixPlusPlusTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPrefixPlusPlusTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorPostfixPlusPlusTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPostfixPlusPlusTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorPrefixMinusMinusTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPrefixMinusMinusTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorPostfixMinusMinusTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPostfixMinusMinusTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorNOTTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorNOTTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorEqualsTag:
             {
            // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorEqualsTag" << notImplemented << endl;

               returnVariant = ROSE_TransformableBinaryOperatorEqualsTag;
               break;
             }
          case ROSE_BinaryArrayOperatorAddTag:
             {
            // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorAddTag" << notImplemented << endl;

               returnVariant = ROSE_TransformableBinaryOperatorNonAssignmentTag;
               break;
             }
          case ROSE_BinaryArrayOperatorAddEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorAddEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorMinusTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMinusTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorMinusEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMinusEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorMultiplyTag:
             {
            // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMultiplyTag" << notImplemented << endl;

               returnVariant = ROSE_TransformableBinaryOperatorNonAssignmentTag;
               break;
             }
          case ROSE_BinaryArrayOperatorMultiplyEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMultiplyEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorDivideTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorDivideTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorDivideEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorDivideEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorModuloTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorModuloTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorModuloEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorModuloEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorLTTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLTTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorLTEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLTEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorGTTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorGTTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorGTEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorGTEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorEquivalenceTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorEquivalenceTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorNOTEqualsTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorNOTEqualsTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorLogicalANDTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLogicalANDTag" << notImplemented << endl;
               break;
             }
          case ROSE_BinaryArrayOperatorLogicalORTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLogicalORTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayOperandBinaryArrayOperatorArrayOperandTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayOperatorArrayOperandTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayOperandBinaryArrayOperatorArrayExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayOperatorArrayExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorArrayOperandTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorArrayOperandTag" << notImplemented << endl;
               break;
             }
          case ROSE_UnaryArrayOperatorArrayExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorArrayExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperandTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperandTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperandTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperandTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_doubleArrayVariableExpressionTag:
             {
            // if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_doubleArrayVariableExpressionTag" << notImplemented << endl;
               returnVariant = ROSE_TransformableOperandExpressionTag;
               break;
             }
          case ROSE_floatArrayVariableExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_floatArrayVariableExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_intArrayVariableExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_intArrayVariableExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_doubleArrayVariableReferenceExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_doubleArrayVariableReferenceExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_floatArrayVariableReferenceExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_floatArrayVariableReferenceExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_intArrayVariableReferenceExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_intArrayVariableReferenceExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_doubleArrayVariablePointerExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_doubleArrayVariablePointerExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_floatArrayVariablePointerExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_floatArrayVariablePointerExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_intArrayVariablePointerExpressionTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_intArrayVariablePointerExpressionTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayParenthesisOperatorTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayParenthesisOperatorTag" << notImplemented << endl;
               returnVariant = ROSE_TransformableOperandExpressionTag;
               break;
             }
          case ROSE_ArrayCOSTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayCOSTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArraySINTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArraySINTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayTANTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayTANTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayMINTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayMINTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayMAXTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayMAXTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayFMODTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayFMODTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayMODTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayMODTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayPOWTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayPOWTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArraySIGNTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArraySIGNTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayLOGTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayLOGTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayLOG10Tag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayLOG10Tag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayEXPTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayEXPTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArraySQRTTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArraySQRTTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayFABSTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayFABSTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayCEILTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayCEILTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayFLOORTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayFLOORTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayABSTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayABSTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayTRANSPOSETag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayTRANSPOSETag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayACOSTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayACOSTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayASINTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayASINTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayATANTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayATANTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayCOSHTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayCOSHTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArraySINHTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArraySINHTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayTANHTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayTANHTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayACOSHTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayACOSHTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayASINHTag: 
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayASINHTag" << notImplemented << endl;
               break;
             }
          case ROSE_ArrayATANHTag:
             {
               if (LOCAL_ROSE_DEBUG > 0) cout << "ROSE_ArrayATANHTag" << notImplemented << endl;
               break;
             }
          default:
               printf ("ERROR: default reached in getVariant (ROSE_Node*) \n");
               ROSE_ABORT();
        }

     ROSE_ASSERT (returnVariant != ROSE_TRANSFORMABLE_LAST_TAG);
     return returnVariant;
   }







