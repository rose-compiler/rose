#include <sage3basic.h>

SgType*
SgPointerDerefExp::get_type() const
   {
  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)
  // DQ (7/28/2006): This code can't be used for both the SgPointerDerefExp and the SgPntrArrRefExp IR nodes because
  // it is a bit different (one node is a unary operator and the other is a binary operator).
  // This function returns the base type of the type returned from get_operand()->get_type() (which should be a SgPointerType).

     ROSE_ASSERT(get_operand() != NULL);

     SgType* someType = get_operand()->get_type();
     ROSE_ASSERT(someType != NULL);

     if (p_expression_type != NULL)
        {
          printf ("In SgPointerDerefExp::get_type(): p_expression_type = %s \n",p_expression_type->class_name().c_str());
        }

  // This code should be shared between the SgPntrArrRefExp and the SgPointerDerefExp IR nodes
  // A better idea would be to have a function that strips off types based on a set of flags
  // that would control stripping of pointer references, array references, C++ references,
  // modifiers, and typedefs.

     SgType* returnType = NULL;
     keepStripping:
     ROSE_ASSERT (someType);
     switch(someType->variantT())
        {
       // These cases all have base types and the pointer dereferencing returns the base type
       // But since p_base_type is placed at each of the different SgType IR nodes we have
       // to handle each case separately.
          case V_SgPointerType:
             {
               SgPointerType* pointerType = isSgPointerType(someType);
               returnType = pointerType->get_base_type();

            // DQ (6/11/2014): We also need to take any associated SgModifierType node chains used to modity the SgPointerType (e.g. defining const or UPC shared type attributes).
               returnType = returnType->stripType (SgType::STRIP_MODIFIER_TYPE);
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

       // DQ (5/10/2016): Added this for the case of the C++11 support.
          case V_SgRvalueReferenceType:
             {
               SgRvalueReferenceType* rvalueReferenceType = isSgRvalueReferenceType(someType);
            // returnType = rvalueReferenceType;
               someType = rvalueReferenceType->get_base_type();
               goto keepStripping;
             }

       // DQ (9/27/2012): Added case to avoid falling through the default case (more template support).
          case V_SgTemplateType:
             {
            // DQ (9/27/2012): I think that this is the correct handling of the return value generation.
               returnType = someType;
               break;
             }

       // DQ (9/27/2012): The dereference of a function type is a function call so the type is it's return type (I think)
          case V_SgMemberFunctionType:
             {
               SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(someType);
            // returnType = functionType->get_return_type();
               returnType = memberFunctionType;
               break;
             }

       // DQ (5/25/2013): Added this for the case of the new EDG interface.
       // DQ (6/18/2007): The dereference of a function type is a function call so the type is it's return type (I think)
          case V_SgFunctionType:
             {
               SgFunctionType* functionType = isSgFunctionType(someType);
            // returnType = functionType->get_return_type();
               returnType = functionType;
               break;
             }

       // DQ (5/25/2013): Added this for the case of the new EDG interface.
          case V_SgClassType:
             {
               SgClassType* classType = isSgClassType(someType);
               returnType = classType;
               break;
             }

       // DQ (7/13/2013): Added this for the case of the new EDG interface.
          case V_SgTypeVoid:
             {
               SgTypeVoid* voidType = isSgTypeVoid(someType);
               returnType = voidType;
               break;
             }

       // DQ (2/16/2014): Added this for the case of the new EDG interface (sometimes this case is required).
          case V_SgTypeUnknown:
             {
               returnType = someType;
               break;
             }

       // PP (7/9/21): Support Ada types
          case V_SgAdaAccessType:
             {
               returnType = isSgAdaAccessType(someType)->get_base_type();
               ROSE_ASSERT(returnType && ("access"));
               // \todo shall we strip modifiers, similar to SgPointerType ??
               break;
             }

          default:
             {
            // DQ (10/23/2016): Commented out message to avoid output spew.
            // DQ (12/31/2008): This is the new behavior for EDG version 4.x.
            // printf ("Warning: default reached in In SgPointerDerefExp::get_type() someType = %s \n",someType->class_name().c_str());
               returnType = someType;
             }
        }

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
