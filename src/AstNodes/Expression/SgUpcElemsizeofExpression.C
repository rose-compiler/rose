#include <sage3basic.h>

void
SgUpcElemsizeofExpression::post_construction_initialization()
   {
   }

// DQ (2/12/2011): Copied this version from SgSizeOfOp::get_type().
SgType*
SgUpcElemsizeofExpression::get_type() const
   {
  // This function returns an unsigned integer representing the size of type
  // (the unsigned int should match size_t, which may or may not be defined).

     ROSE_ASSERT(p_expression != NULL || p_operand_type != NULL);

     SgType* returnType = SgTypeUnsignedInt::createType();

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
