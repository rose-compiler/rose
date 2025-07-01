#include <sage3basic.h>

SgType*
SgArrowStarOp::get_type() const
   {
  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)
  // For the SgArrowStarOp, the type is the type of the rhs operand (e.g. "A->*x" where "x" is a member function pointer of the pointer to the class "A").
     ROSE_ASSERT(get_lhs_operand() != NULL);
     ROSE_ASSERT(get_rhs_operand() != NULL);

     SgType* someType = get_rhs_operand()->get_type();

     SgType* returnType = NULL;
     keepStripping:
     ROSE_ASSERT (someType != NULL);
     switch(someType->variantT())
        {
       // These cases all have base types and the pointer dereferencing returns the base type
       // But since p_base_type is placed at each of the different SgType IR nodes we have
       // to handle each case separately.
          case V_SgPointerType:
             {
               SgPointerType* pointerType = isSgPointerType(someType);
               returnType = pointerType->get_base_type();
               break;
             }
       // DQ (7/31/2006): Added a new case so now this code is no longer a clone of what is used in SgPointerDerefExp and SgPntrArrRefExp.
          case V_SgPointerMemberType:
             {
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType(someType);
               returnType = pointerMemberType->get_base_type();
               break;
             }
          case V_SgArrayType:
             {
               SgArrayType* arrayType = isSgArrayType(someType);
               returnType = arrayType->get_base_type();
               break;
             }
          case V_SgTypedefType:
             {
               SgTypedefType* typedefType = isSgTypedefType(someType);
               someType = typedefType->get_base_type();
               goto keepStripping;
             }
          case V_SgReferenceType:
             {
               SgReferenceType* referenceType = isSgReferenceType(someType);
               someType = referenceType->get_base_type();
               goto keepStripping;
             }
          case V_SgModifierType:
             {
               SgModifierType* modifierType = isSgModifierType(someType);
               someType = modifierType->get_base_type();
               goto keepStripping;
             }

          // PP: adding missing cases
          case V_SgTypeUnknown:
          case V_SgNonrealType:
             {
               returnType = SgTypeUnknown::createType();
               break;
             }

          default:
             {
#if defined(ROSE_USE_EDG_VERSION_4) || defined(ROSE_USE_CLANG_FRONTEND)
            // DQ (1/19/2017): I think we want to fix this similarly to the case of SgDotStarExp::get_type() (above)
            // printf ("Using default case in SgArrowStarOp::get_type() differently with EDG version 4.x someType = %s \n",someType->class_name().c_str());
               returnType = someType;
#else
               printf ("Error: default reached in In SgArrowStarOp::get_type() someType = %s \n",someType->class_name().c_str());
               ROSE_ASSERT(false);
#endif
             }
        }

     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
