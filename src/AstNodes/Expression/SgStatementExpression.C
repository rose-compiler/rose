#include <sage3basic.h>

SgType*
SgStatementExpression::get_type() const
   {
     SgType* returnType = NULL;

     ROSE_ASSERT(p_statement != NULL);

  // DQ (7/21/2006): For now we implement this partially and fix it up later.
  // We might want to store the type explicitly since it might be difficult
  // to compute for complex sequences of statements.  It appears that the
  // type is just the type of the expression in the last statement of the block,
  // but it is not clear if it can be more complex than this!

  // The rules are: The last thing in the compound statement should be an expression
  // followed by a semicolon; the value of this subexpression serves as the value of
  // the entire construct. (If you use some other kind of statement last within the
  // braces, the construct has type void, and thus effectively no value.).

  // printf ("SgStatementExpression::get_type() for GNU Statement Expression extension not implemented p_statement = %p = %s \n",p_statement,p_statement->class_name().c_str());

  // DQ (8/6/2006): The rules for the type of a statement expression are that it is the
  // type of the last expression in the statement list or void if the last statement is
  // not an expression (expression statement, SgExpressionStatement).
     SgBasicBlock* block = isSgBasicBlock(p_statement);
     ROSE_ASSERT(block != NULL);

     SgStatementPtrList & statementList = block->getStatementList();

  // DQ (4/8/2015): Empty statement expressions have to be allowed.
     if (statementList.empty() == true)
        {
       // DQ (4/8/2015): After discusion, this might be better setup as a SgTypeUnknown instead of SgTypeVoid.
          returnType = SgTypeVoid::createType();
        }
       else
        {
          SgStatement* lastStatement = *(statementList.rbegin());
          SgExprStatement* expressionStatement = isSgExprStatement(lastStatement);
          if (expressionStatement != NULL)
             {
            // The type of the statement expression is the
            // SgExpression* expression = expressionStatement->get_the_expr();
               SgExpression* expression = expressionStatement->get_expression();
               ROSE_ASSERT(expression != NULL);
               returnType = expression->get_type();
             }
            else
             {
            // This is the default type if last statement in block is not an expression statement (C++ standard)
               returnType = SgTypeVoid::createType();
             }
        }

     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
