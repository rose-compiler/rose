#include <sage3basic.h>

SgExpression*
SgAssignInitializer::get_operand() const
   { return get_operand_i(); }

void
SgAssignInitializer::post_construction_initialization()
   {
     if (get_operand() != NULL)
          get_operand()->set_parent(this);
   }

SgType*
SgAssignInitializer::get_type() const
   {
     ROSE_ASSERT(get_operand() != NULL);

     SgType* returnType = p_expression_type ? p_expression_type : get_operand()->get_type();

     ROSE_ASSERT(returnType != NULL);

     if (!p_expression_type && get_operand()->variantT() != V_SgStringVal)
        {
          SgType* retElemType = SageInterface::getElementType(returnType);
          if (retElemType != NULL)
             {
               returnType = SgPointerType::createType(retElemType);
             }
        }

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }

void
SgAssignInitializer::set_operand(SgExpression * exp)
   {
     set_operand_i(exp);
     if (exp)
          exp->set_parent(this);
   }

SgExpression*
SgAssignInitializer::get_next(int& n) const
   {
     if(n==0)
        {
          n++;
          return get_operand();
        }
       else
          return 0;
   }

int
SgAssignInitializer::replace_expression(SgExpression *o, SgExpression *n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if(get_operand()==o)
        {
          set_operand(n);
          n->set_parent(this);
          return 1;
        }
       else
          return 0;
   }
