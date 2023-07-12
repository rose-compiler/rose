#include <sage3basic.h>

SgType*
SgStringVal::get_type(void) const
   {
#if 0
     printf ("In SgStringVal::get_type() \n");
#endif

  // Since this is a literal, it must have a defined size (even if it is length zero).
     size_t stringSize = get_value().size();

  // DQ (10/5/2010): Modified to used new API (only lengthExpression is supported).
  // Use the literal size and assume that there is no associated expression.
  // return SgTypeString::createType(NULL,stringSize);
     Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
     SgIntVal* lengthExpression = new SgIntVal(fileInfo,(int)stringSize,"");
     ROSE_ASSERT(lengthExpression != NULL);
     return SgTypeString::createType(lengthExpression);
   }
