#include <sage3basic.h>

SgType* SgAdaAttributeExp::get_type() const
{
  return get_attrType();
}

int SgAdaAttributeExp::replace_expression(SgExpression* orig, SgExpression* repl)
{
  if (orig == this->get_object())
  {
    this->set_object(repl);
    repl->set_parent(this);
    orig->set_parent(nullptr);
    return 1;
  } 

  SgExprListExp* repllst = isSgExprListExp(repl);
  if (repllst == nullptr)
    return 0;

  if (isSgExprListExp(orig) == this->get_args())
  { 
    this->set_args(repllst);
    repl->set_parent(this);
    orig->set_parent(nullptr);
    return 1;
  }  

  return 0;
}
