#include <sage3basic.h>

void
SgUnknownArrayOrFunctionReference::post_construction_initialization()
   { }

SgType*
SgUnknownArrayOrFunctionReference::get_type() const
   {
     printf ("In SgUnknownArrayOrFunctionReference::get_type(): Not clear what this type should be (returning SgTypeDefault). \n");
     return SgTypeDefault::createType();
   }
