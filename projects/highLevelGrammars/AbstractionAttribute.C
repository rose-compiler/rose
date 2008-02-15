// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

// #include <string>
// #include <iomanip>
#include "rose.h"
// #include "AstTests.h"

#include <algorithm>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include "nodeAndEdgeTypes.h"

// Support for overloaded operators
#include "AbstractionAttribute.h"

SgExpression* 
ExpressionStatementAttribute::getExpression ( SgExprStatement* expressionStatement )
   {
     SgExpression* returnExpression = expressionStatement->get_expression();
     ROSE_ASSERT(returnExpression != NULL);
     return returnExpression;
   }

list<SgNode*>
ExpressionStatementAttribute::getOperatorNodes ( SgExprStatement* expressionStatement )
   {
  // build list of SgNodes associated with this operator
     list<SgNode*> operatorNodeList;

     operatorNodeList.push_back(expressionStatement);

  // DQ (11/8/2006): The SgExpressionRoot is not longer used.
  // Include the expression root in the operator definition
  // operatorNodeList.push_back(expressionStatement->get_expression());

     SgExpression* returnExpression = expressionStatement->get_expression();
     if (isSgInitializer(returnExpression) != NULL)
        {
          operatorNodeList.push_back(returnExpression);
        }

     return operatorNodeList;
   }


SgExpression*
ExpressionAttribute::chaseFunctionCall ( SgExpression* exp )
   {
  // SgInitializer* initializer = isSgInitializer(returnExpression);
     SgExpression* returnExpression = exp;

  // look a little deeper for the next SgFunction
     switch(exp->variantT())
        {
          case V_SgAssignInitializer:
             {
               SgAssignInitializer* assignmentInitializer = isSgAssignInitializer(exp);
               returnExpression = assignmentInitializer->get_operand();
               break;
             }

          default:
               printf ("Error: default reached \n");
        }

     ROSE_ASSERT (returnExpression != NULL);

     return returnExpression;
   }

list<SgNode*>
BinaryOperatorAttribute::getOperatorNodes(SgFunctionCallExp* functionCallExpression)
   {
  
  // NodeType graphNode(fc,"color=red ");
  // listOfNodes.push_back(graphNode);

     list<SgNode*> operatorNodeList;

     ROSE_ASSERT (functionCallExpression != NULL);
     operatorNodeList.push_back(functionCallExpression);

     ROSE_ASSERT (functionCallExpression->get_function() != NULL);
     operatorNodeList.push_back(functionCallExpression->get_function());

  // Get the number of parameters to this function
     SgExprListExp* exprListExp = functionCallExpression->get_args();
     ROSE_ASSERT (exprListExp != NULL);

     operatorNodeList.push_back(exprListExp);

     SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
     int numberOfParameters = expressionPtrList.size();

     SgExpression* returnExpression = NULL;

  // Member function binary operators take only 1 parameter while global 
  // (most often friend) function binary operators take 2 paramters.
     if (numberOfParameters == 1)
        {
       // This this should be a member function of a class (chek it)
          SgDotExp* dotExpression = isSgDotExp(functionCallExpression->get_function());
          ROSE_ASSERT (dotExpression != NULL);
          returnExpression = dotExpression->get_lhs_operand();

          operatorNodeList.push_back(dotExpression);

          if (isSgInitializer(returnExpression) != NULL)
             {
               operatorNodeList.push_back(returnExpression);
               returnExpression = chaseFunctionCall(returnExpression);
             }

          operatorNodeList.push_back(dotExpression->get_rhs_operand());


       // rhs is the first (only) parameter
          returnExpression = *(expressionPtrList.begin());
          if (isSgInitializer(returnExpression))
             {
               operatorNodeList.push_back(returnExpression);
               returnExpression = chaseFunctionCall(returnExpression);
             }
        }
       else
        {
          ROSE_ASSERT (numberOfParameters == 2);

       // lhs is the first parameter
          returnExpression = *(expressionPtrList.begin());
          if (isSgInitializer(returnExpression) != NULL)
             {
               operatorNodeList.push_back(returnExpression);
               returnExpression = chaseFunctionCall(returnExpression);
             }

       // rhs is the last (2nd) parameter
          returnExpression = *(expressionPtrList.rbegin());
          if (isSgInitializer(returnExpression) != NULL)
             {
               operatorNodeList.push_back(returnExpression);
               returnExpression = chaseFunctionCall(returnExpression);
             }
        }

     ROSE_ASSERT (operatorNodeList.size() > 0);
     return operatorNodeList;
   }


SgExpression* 
BinaryOperatorAttribute::getLhsOperand ( SgFunctionCallExp* functionCallExpression )
   {
/*
     A function call (SgFunctionCallExp) contains:
         function:
              either a function reference expression (SgFunctionRefExp) 
              OR
              a dotExp (containing member function reference expression (SgMemberFunctionRefExp)
         function arguments:
              an expression list (SgExpreListExp)

     From the input function call (for an overloader operator)
     generate a pointer to the lhs operand.
*/

     ROSE_ASSERT (functionCallExpression != NULL);

  // Get the number of parameters to this function
     SgExprListExp* exprListExp = functionCallExpression->get_args();
     ROSE_ASSERT (exprListExp != NULL);

     SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
     int numberOfParameters = expressionPtrList.size();

     SgExpression* returnExpression = NULL;

  // Member function binary operators take only 1 parameter while global 
  // (most often friend) function binary operators take 2 paramters.
     if (numberOfParameters == 1)
        {
       // This should be a member function of a class (chek it)
          SgDotExp* dotExpression = isSgDotExp(functionCallExpression->get_function());
          ROSE_ASSERT (dotExpression != NULL);
          returnExpression = dotExpression->get_lhs_operand();

          if (isSgInitializer(returnExpression) != NULL)
               returnExpression = chaseFunctionCall(returnExpression);
        }
       else
        {
          ROSE_ASSERT (numberOfParameters == 2);

       // lhs is the first parameter
          returnExpression = *(expressionPtrList.begin());

          if (isSgInitializer(returnExpression) != NULL)
               returnExpression = chaseFunctionCall(returnExpression);

#if 0
          SgInitializer* initializer = isSgInitializer(returnExpression);
          if (initializer != NULL)
             {
            // look a little deeper for the next SgFunction
               switch(initializer->variantT())
                  {
                    case V_SgAssignInitializer:
                       {
                         SgAssignInitializer* assignmentInitializer = isSgAssignInitializer(initializer);
                         returnExpression = assignmentInitializer->get_operand();
                         break;
                       }
                    default:
                         printf ("Error: default reached \n");
                  }

               ROSE_ASSERT (returnExpression != NULL);
             }
#endif
        }

     ROSE_ASSERT (returnExpression != NULL);
     return returnExpression;
   }

SgExpression* 
BinaryOperatorAttribute::getRhsOperand ( SgFunctionCallExp* functionCallExpression )
   {
     ROSE_ASSERT (functionCallExpression != NULL);

  // Get the number of parameters to this function
     SgExprListExp* exprListExp = functionCallExpression->get_args();
     ROSE_ASSERT (exprListExp != NULL);

     SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
     int numberOfParameters = expressionPtrList.size();

     SgExpression* returnExpression = NULL;

  // Member function binary operators take only 1 parameter while global 
  // (most often friend) function binary operators take 2 paramters.
     if (numberOfParameters == 1)
        {
       // rhs is the first (only) parameter
          returnExpression = *(expressionPtrList.begin());
          if (isSgInitializer(returnExpression))
               returnExpression = chaseFunctionCall(returnExpression);
        }
       else
        {
          ROSE_ASSERT (numberOfParameters == 2);

       // lhs is the last parameter
          returnExpression = *(expressionPtrList.rbegin());
          if (isSgInitializer(returnExpression))
               returnExpression = chaseFunctionCall(returnExpression);
        }

     ROSE_ASSERT (returnExpression != NULL);
     return returnExpression;
   }

int
ParenthesisOperatorAttribute::getNumberOfIndexOperands(SgFunctionCallExp* functionCallExpression)
   {
  // get number of operands
     ROSE_ASSERT (functionCallExpression != NULL);

  // Get the number of parameters to this function
     SgExprListExp* exprListExp = functionCallExpression->get_args();
     ROSE_ASSERT (exprListExp != NULL);

     SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
     int numberOfParameters = expressionPtrList.size();

     return numberOfParameters;
   }

SgExpression* 
ParenthesisOperatorAttribute::getIndex ( SgFunctionCallExp* functionCallExpression, unsigned int n )
   {
  // get the nth operand from the index list
     ROSE_ASSERT (functionCallExpression != NULL);

  // Get the number of parameters to this function
     SgExprListExp* exprListExp = functionCallExpression->get_args();
     ROSE_ASSERT (exprListExp != NULL);

     SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
  // int numberOfParameters = expressionPtrList.size();

     unsigned int counter = 0;
     SgExpression* returnExpression = NULL;
     for (SgExpressionPtrList::iterator i = expressionPtrList.begin(); i != expressionPtrList.end(); i++)
        {
          if (counter++ == n)
               returnExpression = *i;
        }

     ROSE_ASSERT (counter <= expressionPtrList.size());
     if (isSgInitializer(returnExpression) != NULL)
          returnExpression = chaseFunctionCall(returnExpression);

     ROSE_ASSERT (returnExpression != NULL);
     return returnExpression;
   }

SgExpression*
ParenthesisOperatorAttribute::getIndexedOperand(SgFunctionCallExp* functionCallExpression)
   {
  // get the indexed operand

     SgExpression* returnExpression = NULL;

  // This should be a member function of a class (chek it)
     SgDotExp* dotExpression = isSgDotExp(functionCallExpression->get_function());
     ROSE_ASSERT (dotExpression != NULL);
     returnExpression = dotExpression->get_lhs_operand();

     if (isSgInitializer(returnExpression) != NULL)
          returnExpression = chaseFunctionCall(returnExpression);

     ROSE_ASSERT (returnExpression != NULL);
     return returnExpression;
   }

list<SgNode*>
ParenthesisOperatorAttribute::getOperatorNodes(SgFunctionCallExp* functionCallExpression)
   {
  // build list of SgNodes associated with this operator
     list<SgNode*> operatorNodeList;

     SgExprListExp* exprListExp = functionCallExpression->get_args();
     ROSE_ASSERT (exprListExp != NULL);
     operatorNodeList.push_back(exprListExp);

     SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
     for (SgExpressionPtrList::iterator i = expressionPtrList.begin(); i != expressionPtrList.end(); i++)
        {
          if (isSgInitializer(*i) != NULL)
               operatorNodeList.push_back(*i);
        }

  // This should be a member function of a class (chek it)
     SgDotExp* dotExpression = isSgDotExp(functionCallExpression->get_function());
     ROSE_ASSERT (dotExpression != NULL);

     operatorNodeList.push_back(dotExpression);
     operatorNodeList.push_back(dotExpression->get_rhs_operand());

     SgExpression* returnExpression = dotExpression->get_rhs_operand();
     if (isSgInitializer(returnExpression) != NULL)
        {
          operatorNodeList.push_back(returnExpression);
          returnExpression = chaseFunctionCall(returnExpression);
        }

     return operatorNodeList;
   }











