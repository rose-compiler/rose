#include <sage3basic.h>

SgType*
SgClassExp::get_type() const
   {
     ROSE_ASSERT(get_class_symbol() != NULL);
     ROSE_ASSERT(get_class_symbol()->get_type() != NULL);

#if 0
     printf ("In SgClassExp::get_type() \n");
#endif

     return get_class_symbol() -> get_type();
   }
