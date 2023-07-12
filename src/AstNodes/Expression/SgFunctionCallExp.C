#include <sage3basic.h>

void
SgFunctionCallExp::post_construction_initialization()
   {
       SgCallExpression::post_construction_initialization();
   }


SgType*
SgFunctionCallExp::get_type() const
   {
  // DQ (7/20/2006): Peter's patch now allows this function to be simplified to the following (suggested by Jeremiah).
     SgType* returnType = NULL;

     ROSE_ASSERT(p_function != NULL);
     SgType* likelyFunctionType = p_function->get_type();
     ROSE_ASSERT(likelyFunctionType != NULL);
     while (likelyFunctionType && isSgTypedefType(likelyFunctionType))
        {
          likelyFunctionType = isSgTypedefType(likelyFunctionType)->get_base_type();
          ROSE_ASSERT(likelyFunctionType != NULL);
        }

     SgFunctionType* functionType = isSgFunctionType(likelyFunctionType);
     if (functionType == NULL)
        {
       // DQ (8/26/2012): Decrease the volume of warnings from this part of the code.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          static int count = 0;
          if (count++ % 100 == 0)
             {
               printf ("Warning: unexpected type found for likelyFunctionType = %p = %s \n",likelyFunctionType,likelyFunctionType->class_name().c_str());
             }
#endif
       // DQ (7/15/2007): Handle case of typedef of function type
       // Milind Chabbi, after discussing with DQ (7/29/2013), we need to strip all. See test: CompileTests/Cxx_tests/test2013_milind_01.C
          likelyFunctionType = likelyFunctionType->stripType(SgType::STRIP_MODIFIER_TYPE |
                                                             SgType::STRIP_REFERENCE_TYPE |
                                                             SgType::STRIP_RVALUE_REFERENCE_TYPE |
                                                             SgType::STRIP_POINTER_TYPE |
                                                             SgType::STRIP_ARRAY_TYPE |
                                                             SgType::STRIP_TYPEDEF_TYPE);

          functionType = isSgFunctionType(likelyFunctionType);
          if (functionType == NULL) {
              return SgTypeUnknown::createType();
          }
        }
     ROSE_ASSERT(functionType);
     returnType = functionType->get_return_type();
     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
