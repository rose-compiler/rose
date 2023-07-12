#include <sage3basic.h>

void
SgAsteriskShapeExp::post_construction_initialization()
   { }

SgType*
SgAsteriskShapeExp::get_type() const
   {
     return SgTypeDefault::createType();
   }
