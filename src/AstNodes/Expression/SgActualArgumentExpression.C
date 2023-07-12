#include <sage3basic.h>

void
SgActualArgumentExpression::post_construction_initialization()
   {
     ROSE_ASSERT(p_expression != NULL);
     p_expression->set_parent(this);
   }

SgType*
SgActualArgumentExpression::get_type() const
   {
     ROSE_ASSERT(p_expression != NULL);

#if 0
     printf ("In SgActualArgumentExpression::get_type() \n");
#endif

     return p_expression->get_type();
   }
