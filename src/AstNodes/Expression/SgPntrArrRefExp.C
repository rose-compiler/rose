#include <sage3basic.h>

SgType*
SgPntrArrRefExp::get_type() const
   {
  // DQ (1/16/2006): In this function we want to return the base type of the array being referenced (lhs->get_type()).
  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)

     if (p_expression_type != NULL)
        {
          printf ("In SgPntrArrRefExp::get_type(): p_expression_type = %s \n",p_expression_type->class_name().c_str());
        }

     ROSE_ASSERT(get_lhs_operand() != NULL);
     ROSE_ASSERT(get_rhs_operand() != NULL);

  // (10/25/2024): In C and C++, E1[E2] is definitionally equivalent to *(E1+E2).
  // Therefore, we can borrow the type inference code from SgBinaryOp.C to compute the type of E1+E2 before proceeding with type stripping.

     SgType* lhsType = get_lhs_operand()->get_type();
     ROSE_ASSERT(lhsType != NULL);

     SgType* rhsType = get_rhs_operand()->get_type();
     ROSE_ASSERT(rhsType != NULL);

     SgType *lhsBase = SageInterface::getElementType(lhsType),
            *rhsBase = SageInterface::getElementType(rhsType);

     SgType* someType;

     if (lhsBase != NULL && rhsBase == NULL)
        {
          someType = SgPointerType::createType(lhsBase);
        }
     else if (lhsBase == NULL && rhsBase != NULL)
        {
          someType = SgPointerType::createType(rhsBase);
        }
     else
        {
          someType = lhsType;
        }

     ROSE_ASSERT(someType != NULL);

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
               // PP (1/10/23): Ada support: arrvar(2..5) returns an array segment.
               SgExprListExp* indices = isSgExprListExp(get_rhs_operand());
               const bool     isArraySubRange = (  indices
                                                && SageInterface::is_Ada_language()
                                                && isSgRangeExp(indices->get_expressions().at(0))
                                                );

               returnType = isArraySubRange ? someType
                                            : isSgArrayType(someType)->get_base_type();
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

       // DQ (9/12/2010): Added SgTypeFloat support to handle Fortran support (in file LANL_POP/grid.F90).
          case V_SgTypeFloat:
             {
            // DQ (8/17/2010): I think that this is the correct handling of the return value generation.
               returnType = someType;
               break;
             }

       // DQ (10/3/2006): Added case of array reference of string type (assume it returns a char).
       // Once we have a wcharString type then we have to add that case to this list as well!
          case V_SgTypeString:
             {
            // SgTypeString* stringType = isSgTypeString(someType);
            // someType = stringType->get_base_type();
               returnType = SgTypeChar::createType();
               break;
             }

          // PP (7/9/21): Support Ada types
          case V_SgAdaSubtype:
             {
               someType = isSgAdaSubtype(someType)->get_base_type();
               goto keepStripping;
             }

       // DQ (12/31/2008): This is a better fit for the new EDG/ROSE translation interface.
          default:
             {

               returnType = someType;
             }
        }

     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
