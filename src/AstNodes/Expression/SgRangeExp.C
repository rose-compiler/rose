#include <sage3basic.h>

SgRangeExp* SgRangeExp::append(SgExpression *exp)
{
  //start:end
  //or start:increment:end

  ROSE_ASSERT(exp != NULL);

  if(p_start == NULL)
  {
    p_start = exp;
  }
  else if(p_end == NULL)
  {
    p_end = exp;
  }
  else if(p_stride == NULL)
  {
    //Swap stride with end. 1:2 then 1:2:3 comes, 2 was end. now 2 becomes stride
    p_stride = p_end;
    p_end = exp;
  }
  else
  {
    //cannot do more appends. Max 3 items allowed. Raise an error
    return NULL;
  }

  exp->set_parent(this);
  return this;
}


SgType* SgRangeExp::get_type() const
{
  ASSERT_not_null(p_start);

  return SgRangeType::createType(p_start->get_type());
}


