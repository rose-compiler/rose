#include <sage3basic.h>

SgType*
SgDotStarOp::get_type() const
   {
  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)

  // For the SgDotStarOp, the type is the type of the rhs operand (e.g. "A.*x" where "x" is a member function pointer of the class "A").

  // Comment from Jeremiah (7/20/2006)
  // In SOURCE_DOT_STAR_OPERATOR_EXPRESSION's get_type() method, the expression type is computed as the type of the RHS operand.
  // This is most likely a pointer to member type, though, not the type of the underlying member (assuming the types of
  // pointer-to-member expressions are set correctly).  The same issue applies to SOURCE_ARROW_STAR_OPERATOR_EXPRESSION.

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
          default:
             {
               returnType = someType;
             }
        }

     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
