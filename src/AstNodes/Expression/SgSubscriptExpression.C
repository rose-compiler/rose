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
     bool isStrideNullExpression     = (isSgNullExpression(get_stride()) != NULL);

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
          if (isUpperBoundNullExpression == true)
             {
            // Not a problem since the lower bound was specified
             }
            else
             {
            // Just make sure that they are the same type, not clear how important this is!
               SgType* upperBoundType = get_upperBound()->get_type();
            // ROSE_ASSERT(upperBoundType->variantT() == returnType->variantT());
               if (upperBoundType->variantT() != returnType->variantT())
                  {
                 // These are a different type..., I wonder why
                    printf ("In SgSubscriptExpression::get_type(): Note that lowerBound = %s and upperBound = %s are different types \n",returnType->class_name().c_str(),upperBoundType->class_name().c_str());
                    this->get_startOfConstruct()->display("In SgSubscriptExpression::get_type(): lowerBound and upperBound are different types");
                  }
             }
        }

     return returnType;
   }
