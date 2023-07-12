#include <sage3basic.h>

SgType*
SgArrowExp::get_type() const
   {
  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)
  // For the SgArrowExp, the type is the type of the rhs operand (e.g. "Aptr->x" where "x" is the data member of the pointer to class "A").
     ROSE_ASSERT(get_lhs_operand() != NULL);
     ROSE_ASSERT(get_rhs_operand() != NULL);

     SgType* returnType = get_rhs_operand()->get_type();
     ROSE_ASSERT(returnType != NULL);

  // DQ (7/16/2014): In the case of UPC shared type for the lhs_operand, we need to return a type that is marked as
  // shared (even if the type on the rhs_operand is non-shared) (see UPC_tests/test2014_56.c for an example).
     returnType = checkForSharedTypeAndReturnSharedType(returnType);

     return returnType;
   }


SgType*
SgArrowExp::checkForSharedTypeAndReturnSharedType( SgType* possibleReturnType ) const
   {
  // DQ (7/16/2014): In the case of UPC shared type for the lhs_operand, we need to return a type that is marked as
  // shared (even if the type on the rhs_operand is non-shared) (see UPC_tests/test2014_56.c for an example).

     SgType* returnType = possibleReturnType;

     ROSE_ASSERT(get_lhs_operand()->get_type() != NULL);
     SgModifierType* mod_type = isSgModifierType(get_lhs_operand()->get_type());
     if (mod_type != NULL)
        {
          if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
             {
               returnType = SageBuilder::buildUpcSharedType(returnType);
             }
        }
       else
        {
          SgPointerType* pointerType = isSgPointerType(get_lhs_operand()->get_type());
          if (pointerType != NULL)
             {
               SgType* base_type = pointerType->get_base_type();
               SgModifierType* modifier_type = isSgModifierType(base_type);
               while (modifier_type != NULL && modifier_type->get_typeModifier().get_upcModifier().get_isShared() == false)
                  {
                    modifier_type = isSgModifierType(modifier_type->get_base_type());
                  }
               if (modifier_type != NULL && modifier_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                  {
                    returnType = SageBuilder::buildUpcSharedType(returnType);
                  }
             }
        }

     return returnType;
   }
