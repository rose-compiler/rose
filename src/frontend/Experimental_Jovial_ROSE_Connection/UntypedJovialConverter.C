#include "sage3basic.h"
#include "UntypedJovialConverter.h"
#include "Jovial_to_ROSE_translation.h"

#define DEBUG_UNTYPED_CONVERTER 0

using namespace Untyped;
using std::cout;
using std::endl;

void
UntypedJovialConverter::convertLabel (SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                      SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
{
   std::string label_name = ut_stmt->get_label_string();
   if (!label_name.empty())
      {
         char* next;
      // SageInterface::setFortranNumericLabel(sg_stmt, std::stoul(label_name));
      // The modifications in setFortranNumericLabel should be moved to SageInterface

      // TODO - This will need to handle multiple labels per Jovial statement
      // TODO - Change Fortran specific name

         setFortranNumericLabel(sg_stmt, strtoul(label_name.c_str(),&next,10), label_type, label_scope);
         ROSE_ASSERT(next != label_name.c_str());
      }
}

SgType*
UntypedJovialConverter::convertSgUntypedType (SgUntypedType* ut_type, SgScopeStatement* scope)
{
   SgType* sg_type = NULL;

// Temporary assertions as this conversion is completed
   ROSE_ASSERT(ut_type->get_is_intrinsic() == true);
   ROSE_ASSERT(ut_type->get_is_literal() == false);
   ROSE_ASSERT(ut_type->get_is_class() == false);
   ROSE_ASSERT(ut_type->get_is_constant() == false);
   ROSE_ASSERT(ut_type->get_is_user_defined() == false);

   SgExpression*   kindExpression = NULL;
   SgExpression* lengthExpression = NULL;

   if (ut_type->get_has_kind())
      {
         SgExpressionPtrList children;
         SgUntypedExpression* ut_kind = ut_type->get_type_kind();
      // TODO - figure out how to handle operators (or anything with children)
         ROSE_ASSERT(isSgUntypedValueExpression(ut_kind) != NULL || isSgUntypedReferenceExpression(ut_kind) != NULL);
         kindExpression = convertSgUntypedExpression(ut_kind, children, scope);
      }
   if (ut_type->get_char_length_is_string())
      {
         SgExpressionPtrList children;
         SgUntypedExpression* ut_length = ut_type->get_char_length_expression();
      // TODO - figure out how to handle operators (or anything with children)
         ROSE_ASSERT(isSgUntypedValueExpression(ut_length) != NULL || isSgUntypedReferenceExpression(ut_length) != NULL);
         lengthExpression = convertSgUntypedExpression(ut_length, children, scope);
      }

// TODO - determine if SageBuilder can be used (or perhaps should be updated)
   switch(ut_type->get_type_enum_id())
      {
        case SgUntypedType::e_void:           sg_type = SageBuilder::buildVoidType();              break;
        case SgUntypedType::e_int:            sg_type = SgTypeInt::createType(0, kindExpression);  break;
        case SgUntypedType::e_uint:           sg_type = SgTypeUnsignedInt::createType(kindExpression); break;
        case SgUntypedType::e_float:          sg_type = SgTypeFloat::createType(kindExpression);   break;
        case SgUntypedType::e_double:         sg_type = SageBuilder::buildDoubleType();            break;

     // complex types
        case SgUntypedType::e_complex:        sg_type = SgTypeComplex::createType(SgTypeFloat::createType(kindExpression), kindExpression); break;
        case SgUntypedType::e_double_complex: sg_type = SgTypeComplex::createType(SgTypeDouble::createType());                              break;

        case SgUntypedType::e_bit:            sg_type = SgTypeBool::createType(kindExpression);    break;

     // character and string types
        case SgUntypedType::e_char:
           {
              if (lengthExpression)
                 {
                    sg_type = SgTypeString::createType(lengthExpression, kindExpression);          break;
                 }
              else
                 {
                    sg_type = SgTypeChar::createType(kindExpression);                              break;
                 }
           }

        default:
           {
              fprintf(stderr, "UntypedJovialConverter::convertSgUntypedType: failed to find known type, enum is %d \n", ut_type->get_type_enum_id());
              ROSE_ASSERT(0);
           }
      }

// TODO - determine if this is necessary
   if (kindExpression != NULL)
      {
         kindExpression->set_parent(sg_type);
      }
   if (lengthExpression != NULL)
      {
         lengthExpression->set_parent(sg_type);
      }

   ROSE_ASSERT(sg_type != NULL);

   return sg_type;
}
