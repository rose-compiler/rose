#include <sage3basic.h>

SgType*
SgSuperExp::get_type() const
   {
     ROSE_ASSERT(get_class_symbol() != NULL);
     ROSE_ASSERT(get_class_symbol()->get_type() != NULL);

#if 0
     printf ("In SgSuperExp::get_type() \n");
#endif

     return SgPointerType::createType(get_class_symbol()->get_type());
   }
