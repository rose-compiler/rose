#include <sage3basic.h>

void
SgTypeTraitBuiltinOperator::post_construction_initialization()
   {
  // SgCallExpression::post_construction_initialization();
   }

SgType*
SgTypeTraitBuiltinOperator::get_type() const
   {
  // DQ (7/12/2013): These functions will almost always return a boolean type, but not always.

     SgType* returnType = NULL;

#if 0
     printf ("In SgTypeTraitBuiltinOperator::get_type() \n");
#endif

     if (get_name() == "__underlying_type")
        {
       // This returns an actual type and it is not clear now to handle this.
          returnType = SgTypeDefault::createType();
        }
       else
        {
          returnType = SgTypeBool::createType();
        }

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
