#include <sage3basic.h>

SgType*
SgComplexVal::get_type(void) const
   {
  // Use the stored SgType to return the correct SgTypeComplex using the correct precision.
     ROSE_ASSERT(p_precisionType != NULL);

#if 0
     printf ("In SgComplexVal::get_type() \n");
#endif

  // returns a shared SgTypeComplex type
     return SgTypeComplex::createType(p_precisionType);
   }
