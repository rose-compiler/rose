#include <sage3basic.h>

SgType*
SgNaryBooleanOp::get_type() const
   {
     return SageBuilder::buildVoidType();
   }
