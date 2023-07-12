#include <sage3basic.h>

void
SgDeleteExp::post_construction_initialization()
   {
     if(p_variable)
          p_variable->set_parent(this);
   }

SgType*
SgDeleteExp::get_type() const
   {
  // The delete operator always returns "void" as a type.  This used to return SgDefaultType (but that didn't seem quite right)
     SgType* returnType = SgTypeVoid::createType();

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }

SgExpression*
SgDeleteExp::get_next(int& n) const
   {
     if(n==0)
        {
          n++;
          return get_variable();
        }
       else
        {
          return 0;
        }
   }

int
SgDeleteExp::replace_expression(SgExpression *o, SgExpression *n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if (get_variable() == o)
        {
          set_variable(n);
          n->set_parent(this);
          return 1;
        }
       else
        {
          return 0;
        }
   }
