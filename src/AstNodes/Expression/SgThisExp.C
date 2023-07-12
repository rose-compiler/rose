#include <sage3basic.h>

SgType*
SgThisExp::get_type() const
   {
     SgType * type = NULL;
     if (get_class_symbol() != NULL) {
       type = get_class_symbol()->get_type();
     }
     if (get_nonreal_symbol() != NULL) {
       type = get_nonreal_symbol()->get_type();
     }

     ROSE_ASSERT(type != NULL);

     return SgPointerType::createType(type);
   }
