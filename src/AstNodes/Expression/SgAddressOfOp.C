#include <sage3basic.h>

SgType*
SgAddressOfOp::get_type() const
   {
  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)
  // This function returns a pointer to the type return from get_operand()->get_type().

  // \todo PP (12/7/22): generate Ada access types instead of pointer types
  //                     if get_type is called from Ada context.

     ROSE_ASSERT(get_operand() != NULL);
     SgType* baseType = get_operand()->get_type();
     ROSE_ASSERT(baseType != NULL);

  // DQ (7/31/2006): Suggested change by Jeremiah.
     SgClassDefinition* classDefinition = NULL;

  // DQ (7/31/2006): check if this is a data member of a class
  // (and save the class for the SgPointerMemberType::createType() function!)
     SgVarRefExp* varRefExp = isSgVarRefExp(get_operand());
     if (varRefExp != NULL)
        {
          ROSE_ASSERT(varRefExp->get_symbol() != NULL);
          ROSE_ASSERT(varRefExp->get_symbol()->get_declaration() != NULL);
          SgInitializedName* variable = varRefExp->get_symbol()->get_declaration();
          ROSE_ASSERT(variable != NULL);
          SgScopeStatement* scope = variable->get_scope();
          ROSE_ASSERT(scope != NULL);

          classDefinition = isSgClassDefinition(scope);
        }

     SgType* returnType = NULL;
     if (classDefinition != NULL)
        {
          SgClassType* classType = classDefinition->get_declaration()->get_type();
          returnType = SgPointerMemberType::createType(baseType,classType);
        }
       else
        {
       // Milind Chabbi (8/1/2013) We must strip the reference else we will get pointer to a reference.
       // See CompileTests/Cxx_tests/test2004_157.C run under extractFunctionArgumentsNormalization
       // TODO: should we strip STRIP_TYPEDEF_TYPE? Not sure, need to talk to Dan.
       // DQ (7/16/2014): Dan says no. Don't strip the typedef or we could generate references to types
       // in the AST that might be private or inaccessible when we compile the unparsed the source code).
       // TV (11/13/2018): Added r-value reference handling
          returnType = SgPointerType::createType(baseType->stripType(SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE));

       // DQ (7/16/2014): In the case of UPC shared type for the lhs_operand, we need to return a type that is marked as
       // shared (even if the type on the rhs_operand is non-shared) (see UPC_tests/test2014_56.c for an example).
          returnType = checkForSharedTypeAndReturnSharedType(returnType);
        }

   //  ROSE_ASSERT(returnType != NULL);
     return returnType;
   }


// DQ (7/16/2014): Shared types from expressions in UPC have some expected symantics that are not a matter of returning the result of get_type().
// This function supports the get_type() function.
SgType*
SgAddressOfOp::checkForSharedTypeAndReturnSharedType( SgType* possibleReturnType ) const
   {
  // DQ (7/16/2014): In the case of UPC shared type for the lhs_operand, we need to return a type that is marked as
  // shared (even if the type on the rhs_operand is non-shared) (see UPC_tests/test2014_56.c for an example).

     SgType* returnType = possibleReturnType;

     ROSE_ASSERT(get_operand()->get_type() != NULL);
     SgModifierType* mod_type = isSgModifierType(get_operand()->get_type());
     if (mod_type != NULL)
        {
          if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
             {
               returnType = SageBuilder::buildUpcSharedType(possibleReturnType);
             }
        }
       else
        {
          SgPointerType* pointerType = isSgPointerType(get_operand()->get_type());
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
                    returnType = SageBuilder::buildUpcSharedType(possibleReturnType);
                  }
             }
        }

     return returnType;
   }
