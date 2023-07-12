#include <sage3basic.h>

SgExpression*
SgAdaAncestorInitializer::get_operand() const
{ return get_ancestor(); }

void
SgAdaAncestorInitializer::post_construction_initialization()
   {
     if (get_operand() != NULL)
          get_operand()->set_parent(this);
   }

SgType*
SgAdaAncestorInitializer::get_type() const
   {
     ROSE_ASSERT(get_operand() != NULL);

#if 0
     printf ("In SgAdaAncestorInitializer::get_type() \n");
#endif

     return get_operand()->get_type();
   }

void
SgAdaAncestorInitializer::set_operand(SgExpression * exp)
   {
     set_ancestor(exp);

     if (exp)
          exp->set_parent(this);
   }

SgExpression*
SgAdaAncestorInitializer::get_next(int& n) const
   {
     if(n==0)
      {
        n=1;
        return get_operand();
      }

     return nullptr;
   }

int
SgAdaAncestorInitializer::replace_expression(SgExpression *o, SgExpression *n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if(get_operand()==o)
        {
          set_operand(n);
          return 1;
        }

     return 0;
   }
