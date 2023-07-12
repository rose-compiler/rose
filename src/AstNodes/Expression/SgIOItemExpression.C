#include <sage3basic.h>

void
SgIOItemExpression::post_construction_initialization()
   { }

SgType*
SgIOItemExpression::get_type() const
   {
     printf ("In SgIOItemExpression::get_type(): Not clear what this type should be. \n");
     ROSE_ASSERT(false);

     return NULL;
   }
