#include <sage3basic.h>

void
SgJovialPresetPositionExp::post_construction_initialization()
{
  ROSE_ASSERT(p_indices);
  ROSE_ASSERT(p_value);
  p_indices->set_parent(this);
  p_value->set_parent(this);
}

SgType*
SgJovialPresetPositionExp::get_type() const
{
  ROSE_ASSERT(p_value);
  return p_value->get_type();
}

int
SgJovialPresetPositionExp::replace_expression (SgExpression *o, SgExpression *n)
   {
     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if (p_indices == o) {
       p_indices = isSgExprListExp(n);
       return p_indices != nullptr;
     } else if (p_value) {
       p_value = n;
       return 1;
     } else {
       printf ("Warning: inside of SgUnaryOp::replace_expression original SgExpression unidentified \n");
       return 0;
     }
   }
