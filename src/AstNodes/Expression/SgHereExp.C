#include <sage3basic.h>

void
SgHereExp::post_construction_initialization()
   {
   }

SgType*
SgHereExp::get_type() const
   {
#if 0
     printf ("In SgHereExp::get_type() \n");
#endif

     SgType* returnType = SgTypeDefault::createType();
     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
