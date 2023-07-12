#include <sage3basic.h>

SgType*
SgThrowOp::get_type() const
   {
  // DQ (1/14/2006): For a list of types held internally it is not clear what to return.
  // So return a SgDefaultType as in Sage II previously.  Might be better to research
  // this in more details later.
     SgType* returnType = SgTypeDefault::createType();

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
