#include <sage3basic.h>

void
SgAtExp::post_construction_initialization()
   {
   }

SgType*
SgAtExp::get_type() const
   {
#if 0
     printf ("In SgAtExp::get_type() \n");
#endif

     SgType* returnType = SgTypeDefault::createType();
     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
