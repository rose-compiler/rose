#include <sage3basic.h>

void
SgSubscriptExpression::post_construction_initialization()
   { }

SgType*
SgSubscriptExpression::get_type() const
   {
     SgType* returnType = NULL;

     bool isLowerBoundNullExpression = (isSgNullExpression(get_lowerBound()) != NULL);
     bool isUpperBoundNullExpression = (isSgNullExpression(get_upperBound()) != NULL);
     bool isStrideNullExpression     = (isSgNullExpression(get_stride()) != NULL); // blame initial commit

  // Even if the stride was not specified it should default to unit stride (value == 1).
     ROSE_ASSERT(isStrideNullExpression == false);

     if (isLowerBoundNullExpression == true)
        {
       // There was no lower bound specified
          if (isUpperBoundNullExpression == true)
             {
            // There was no upper bound specified, so we have to assume SgIntType is OK!
               returnType = SgTypeInt::createType();
             }
            else
             {
               returnType = get_upperBound()->get_type();
             }
        }
       else
        {
          returnType = get_lowerBound()->get_type();
        }

     return returnType;
   }
