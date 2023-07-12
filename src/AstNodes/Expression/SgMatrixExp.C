#include <sage3basic.h>

SgType*
SgMatrixExp::get_type() const
   {
#if 0
     printf ("In SgMatrixExp::get_type() \n");
#endif
     return SageBuilder::buildMatrixType();
   }
