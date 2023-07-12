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

void
SgStringVal::post_construction_initialization()
   {
  // We can't initialize this to NULL since it might have just been set!
  // p_value = (char*)0L;
   }

void SgStringVal::set_usesSingleQuotes(bool usesSingleQuotes)
{
  if (usesSingleQuotes)
  {
    set_stringDelimiter('\'');
  }
  else if (get_usesSingleQuotes())
  {
    // unset only if the current delimiter uses single quotes
    set_stringDelimiter(0);
  }
}

void SgStringVal::set_usesDoubleQuotes(bool usesDoubleQuotes)
{
  if (usesDoubleQuotes)
  {
    set_stringDelimiter('"');
  }
  else if (get_usesDoubleQuotes())
  {
    // unset only if the current delimiter uses double quotes
    set_stringDelimiter(0);
  }
}
