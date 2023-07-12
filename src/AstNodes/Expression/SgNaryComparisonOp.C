#include <sage3basic.h>

SgType*
SgNaryComparisonOp::get_type() const
   {
     printf ("In SgNaryComparisonOp::get_type() \n");

     return SageBuilder::buildBoolType();
   }
