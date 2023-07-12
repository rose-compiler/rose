#include <sage3basic.h>

SgType*
SgLambdaExp::get_type() const
   {
  // DQ (9/2/2014): I think we want this to be the return type of the function (deduced or not).
     return SageBuilder::buildVoidType(); // TODO this is wrong
   }
