#include <sage3basic.h>

// DQ (1/13/2014): This function helps to provide a uniform interface even though
// the type is held in a field called p_expression_type.  This supports Java annotations
// for the SgJavaMarkerAnnotation, SgJavaSingleMemberAnnotation, and SgJavaNormalAnnotation.
SgType*
SgJavaAnnotation::get_type() const
   {
  // This function returns an explicitly stored type

     SgType* returnType = p_expression_type;

#if 0
     printf ("In SgJavaAnnotation::get_type() \n");
#endif

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
