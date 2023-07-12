#include <sage3basic.h>

void
SgUnaryOp::post_construction_initialization()
   {
     if (get_operand())
          get_operand()->set_parent(this);
     p_mode = SgUnaryOp::prefix;
   }

SgExpression*
SgUnaryOp::get_operand() const
   { return get_operand_i(); }

void
SgUnaryOp::set_operand(SgExpression * exp)
   {
     set_operand_i(exp);
     if (exp)
          exp->set_parent(this);
   }

SgType*
SgUnaryOp::get_type() const
   {
  // DQ (1/14/2006): Get the type from the operand
     ROSE_ASSERT(get_operand() != NULL);

     SgType* returnType = get_operand()->get_type();

     return returnType;
   }

int
SgUnaryOp::length() const
   { return 1;}

bool
SgUnaryOp::empty() const
   { return 0; }

SgExpression*
SgUnaryOp::get_next (int &n) const
   {
     if(n)
        {
          return NULL;
        }
       else
        {
          n++;
          return get_operand();
        }
   }

int
SgUnaryOp::replace_expression (SgExpression *o, SgExpression *n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if (get_operand() == o)
        {
          set_operand(n);
          return 1;
        }
       else
        {
          printf ("Warning: inside of SgUnaryOp::replace_expression original SgExpression unidentified \n");
          return 0;
        }
   }
