#include "sage3basic.h"

#include "untypedBuilder.h"

namespace UntypedBuilder {

SgFile::languageOption_enum language_enum = SgFile::e_error_language;

void set_language(SgFile::languageOption_enum current_language)
{
   language_enum = current_language;
}


SgUntypedScope* buildUntypedScope(const std::string & label)
   {
      SgUntypedDeclarationStatementList* decl_list = new SgUntypedDeclarationStatementList();
      ROSE_ASSERT(decl_list);
      SageInterface::setSourcePosition(decl_list);

      SgUntypedStatementList* stmt_list = new SgUntypedStatementList();
      ROSE_ASSERT(stmt_list);
      SageInterface::setSourcePosition(stmt_list);

      SgUntypedFunctionDeclarationList* func_list = new SgUntypedFunctionDeclarationList();
      ROSE_ASSERT(func_list);
      SageInterface::setSourcePosition(func_list);

      SgUntypedScope* scope = new SgUntypedScope(label, decl_list, stmt_list, func_list);
      ROSE_ASSERT(scope);
      SageInterface::setSourcePosition(scope);

      return scope;
   }


SgUntypedType* buildType(SgUntypedType::type_enum type_enum)
{
   SgUntypedExpression* type_kind = NULL;
   bool has_kind = false;
   bool is_literal = false;
   bool is_class = false;
   bool is_intrinsic = true;
   bool is_constant = false;
   bool is_user_defined = false;
   SgUntypedExpression* char_length_expr = NULL;
   std::string char_length;
   bool char_length_is_string = false;

   SgUntypedExprListExpression* modifiers = new SgUntypedExprListExpression();
   ROSE_ASSERT(modifiers);

   SgUntypedType* type = NULL;

   switch(language_enum)
    {
      case SgFile::e_Fortran_language:
        {
          switch(type_enum)
            {
             case SgUntypedType::e_unknown:
               {
                 type = new SgUntypedType("UNKNOWN",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_implicit:
               {
                 type = new SgUntypedType("IMPLICIT",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_void:
               {
                 type = new SgUntypedType("void",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_int:
               {
                 type = new SgUntypedType("integer",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_float:
               {
                 type = new SgUntypedType("real",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_complex:
               {
                 type = new SgUntypedType("complex",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_bool:
               {
                 type = new SgUntypedType("logical",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_char:
             case SgUntypedType::e_string:
               {
                 type = new SgUntypedType("character",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             default:
               {
                 fprintf(stderr, "UntypedBuilder::buildType: unimplemented for Fortran type_enum %d \n", type_enum);
                 ROSE_ASSERT(0);  // NOT IMPLEMENTED
               }
            }
          break;
        }

      case SgFile::e_Jovial_language:
        {
          switch(type_enum)
            {
             case SgUntypedType::e_unknown:
               {
                 type = new SgUntypedType("UNKNOWN",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_void:
               {
                 type = new SgUntypedType("void",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_int:
               {
                 type = new SgUntypedType("S",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_uint:
               {
                 type = new SgUntypedType("U",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_float:
               {
                 type = new SgUntypedType("F",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_bool:
               {
                 type = new SgUntypedType("boolean_literal",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_bit:
               {
                 type = new SgUntypedType("B",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_string:
               {
                 type = new SgUntypedType("C",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             default:
               {
                 fprintf(stderr, "UntypedBuilder::buildType: unimplemented for Jovial type_enum %d \n", type_enum);
                 ROSE_ASSERT(0);  // NOT IMPLEMENTED
               }
            }
          break;
        }

      case SgFile::e_Cobol_language:
        {
          switch(type_enum)
            {
             case SgUntypedType::e_unknown:
               {
                 type = new SgUntypedType("UNKNOWN",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             case SgUntypedType::e_void:
               {
                 type = new SgUntypedType("void",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum);
                 break;
               }
             default:
               {
                 fprintf(stderr, "UntypedBuilder::buildType: unimplemented for Cobol type_enum %d \n", type_enum);
                 ROSE_ASSERT(0);  // NOT IMPLEMENTED
               }
            }
          break;
        }

      default:
        {
          fprintf(stderr, "UntypedBuilder::buildType: unimplemented for language_enum %d \n", language_enum);
          ROSE_ASSERT(0);  // NOT IMPLEMENTED
        }

    } // switch(language_enum)

   return type;
}

SgUntypedArrayType* buildArrayType(SgUntypedType::type_enum type_enum, SgUntypedExprListExpression* shape, int rank)
{
   SgUntypedExpression* type_kind = NULL;
   bool has_kind = false;
   bool is_literal = false;
   bool is_class = false;
   bool is_intrinsic = true;
   bool is_constant = false;
   bool is_user_defined = false;
   SgUntypedExpression* char_length_expr = NULL;
   std::string char_length;
   bool char_length_is_string = false;

   SgUntypedExprListExpression* modifiers = new SgUntypedExprListExpression();
   ROSE_ASSERT(modifiers);

   SgUntypedArrayType* type = NULL;

   ROSE_ASSERT(shape != NULL);

   switch(language_enum)
    {
      case SgFile::e_Fortran_language:
        {
          switch(type_enum)
            {
             case SgUntypedType::e_unknown:
               {
                 type = new SgUntypedArrayType("UNKNOWN",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_implicit:
               {
                 type = new SgUntypedArrayType("IMPLICIT",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_void:
               {
                 type = new SgUntypedArrayType("void",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_int:
               {
                 type = new SgUntypedArrayType("integer",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_float:
               {
                 type = new SgUntypedArrayType("real",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_bool:
               {
                 type = new SgUntypedArrayType("logical",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             default:
               {
                 fprintf(stderr, "UntypedBuilder::buildArrayType: unimplemented for Fortran type_enum %d \n", type_enum);
                 ROSE_ASSERT(0);  // NOT IMPLEMENTED
               }
            }
          break;
        }

      case SgFile::e_Jovial_language:
        {
          switch(type_enum)
            {
             case SgUntypedType::e_int:
               {
                 type = new SgUntypedArrayType("S",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_uint:
               {
                 type = new SgUntypedArrayType("U",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_float:
               {
                 type = new SgUntypedArrayType("F",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             case SgUntypedType::e_bit:
               {
                 type = new SgUntypedArrayType("B",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
                                          is_user_defined,char_length_expr,char_length,char_length_is_string,modifiers,type_enum,
                                          shape, rank);
                 break;
               }
             default:
               {
                 fprintf(stderr, "UntypedBuilder::buildArrayType: unimplemented for Jovial type_enum %d \n", type_enum);
                 ROSE_ASSERT(0);  // NOT IMPLEMENTED
               }
            }
          break;
        }

      default:
        {
          fprintf(stderr, "UntypedBuilder::buildArrayType: unimplemented for language_enum %d \n", language_enum);
          ROSE_ASSERT(0);  // NOT IMPLEMENTED
        }

    } // switch(language_enum)

   return type;
}


SgUntypedNullExpression* buildUntypedNullExpression()
{
   SgUntypedNullExpression* expr = new SgUntypedNullExpression();
   SageInterface::setOneSourcePositionForTransformation(expr);
   return expr;
}


} // namespace UntypedBuilder
