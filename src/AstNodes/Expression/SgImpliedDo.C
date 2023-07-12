#include <sage3basic.h>

void
SgImpliedDo::post_construction_initialization()
   { }

SgType*
SgImpliedDo::get_type() const
   {
     printf ("In SgImpliedDo::get_type() \n");

     return SgTypeDefault::createType();
   }
