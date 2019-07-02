#include "sage3basic.h"

#include "untypedBuilder.h"
#include "general_language_translation.h"

namespace UntypedBuilder {

SgFile::languageOption_enum language_enum = SgFile::e_error_language;

void set_language(SgFile::languageOption_enum current_language)
{
   language_enum = current_language;
}


template <class ScopeClass>
ScopeClass* buildScope()
{
   return buildScope<ScopeClass>("");
}

template <class ScopeClass>
ScopeClass* buildScope(const std::string & label)
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

   ScopeClass* scope = new ScopeClass(label, decl_list, stmt_list, func_list);
   ROSE_ASSERT(scope);
   SageInterface::setSourcePosition(scope);

   return scope;
}

// Explicit instantiations for library usage
//
template SgUntypedScope* buildScope<SgUntypedScope>();
template SgUntypedScope* buildScope<SgUntypedScope>(const std::string & label);

template SgUntypedGlobalScope* buildScope<SgUntypedGlobalScope>();
template SgUntypedGlobalScope* buildScope<SgUntypedGlobalScope>(const std::string & label);

template SgUntypedFunctionScope* buildScope<SgUntypedFunctionScope>();
template SgUntypedFunctionScope* buildScope<SgUntypedFunctionScope>(const std::string & label);


SgUntypedType* buildType(SgUntypedType::type_enum type_enum, std::string name)
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
   SageInterface::setSourcePosition(modifiers);

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
             case SgUntypedType::e_user_defined:
               {
                 is_user_defined = true;
                 is_intrinsic = false;
                 type = new SgUntypedType(name,type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
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
             case SgUntypedType::e_table:
               {
                 is_user_defined = true;
                 is_class = true;
                 is_intrinsic = false;
                 type = new SgUntypedType(name,type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,
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
   SageInterface::setSourcePosition(modifiers);

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


// Build an untyped Jovial table type from a base type.
//
SgUntypedTableType* buildJovialTableType (std::string name, SgUntypedType* base_type,
                                          SgUntypedExprListExpression* shape, bool is_anonymous)
{
   ROSE_ASSERT(language_enum == SgFile::e_Jovial_language);

   ROSE_ASSERT(base_type->get_is_intrinsic() == true);

   SgUntypedTableType* type = NULL;
   int rank = shape->get_expressions().size();

   std::string table_type_name = name;

   type = new SgUntypedTableType(table_type_name, base_type, shape, rank);
   ROSE_ASSERT(type != NULL);

   if (is_anonymous)
      {
      // This type is anonymous so create a unique name
         char addr[64];
         sprintf(addr, "%p", type);
         table_type_name = "__anonymous_";
         table_type_name.append(addr);
         type->set_type_name(table_type_name);
      }

// This may not be needed: could set the default to this from e_unknown, could just use knowledge of the type itself
// since we can't overload type_enum_id.  Actually should be have its own distnct from base type!
   type->set_table_type_enum_id(SgUntypedType::e_table);
   type->set_type_enum_id(SgUntypedType::e_table);

   base_type->set_parent(type);

   return type;
}


SgUntypedInitializedName*
buildInitializedName(const std::string & name, SgUntypedType* type, SgUntypedExpression* initializer)
{
   ROSE_ASSERT(type);

   SgUntypedInitializedName* initialized_name = new SgUntypedInitializedName(type, name);
   ROSE_ASSERT(initialized_name);
   SageInterface::setSourcePosition(initialized_name);

   if (initializer != NULL) {
      initialized_name->set_has_initializer(true);
      initialized_name->set_initializer(initializer);
   }

   return initialized_name;
}


SgUntypedInitializedNameList*
buildInitializedNameList(SgUntypedInitializedName* initialized_name)
{
   SgUntypedInitializedNameList* name_list = new SgUntypedInitializedNameList();
   ROSE_ASSERT(name_list);
   SageInterface::setSourcePosition(name_list);

   if (initialized_name != NULL)
      {
      // This case works when there is only one variable
         name_list->get_name_list().push_back(initialized_name);
      }

   return name_list;
}


SgUntypedVariableDeclaration*
buildVariableDeclaration(const std::string & name, SgUntypedType* type, SgUntypedExprListExpression* attr_list, SgUntypedExpression* initializer)
{
   ROSE_ASSERT(type);
   ROSE_ASSERT(attr_list);

   std::string label = "";

   SgUntypedInitializedName* initialized_name = buildInitializedName(name, type, initializer);
   ROSE_ASSERT(initialized_name);

   SgUntypedInitializedNameList* var_name_list = buildInitializedNameList(initialized_name);
   ROSE_ASSERT(var_name_list);

   SgUntypedVariableDeclaration* variable_decl = new SgUntypedVariableDeclaration(label, type, attr_list, var_name_list);
   ROSE_ASSERT(variable_decl);
   SageInterface::setSourcePosition(variable_decl);

   return variable_decl;
}


// Build an untyped StructureDefinition. This version has a body and thus a scope.
// Source position for the initializer and modifier lists and table description should be set after construction.
SgUntypedStructureDefinition* buildStructureDefinition()
{
   int expr_enum;
   std::string label, type_name;
   bool has_body = true;
   bool has_type_name = false;

   SgUntypedStructureDefinition* table_desc = NULL;

   expr_enum = General_Language_Translation::e_struct_modifier_list;
   SgUntypedExprListExpression* attr_list = new SgUntypedExprListExpression(expr_enum);
   ROSE_ASSERT(attr_list);
   SageInterface::setSourcePosition(attr_list);

   expr_enum = General_Language_Translation::e_struct_initializer;
   SgUntypedExprListExpression* preset = new SgUntypedExprListExpression(expr_enum);
   ROSE_ASSERT(preset);
   SageInterface::setSourcePosition(preset);

   SgUntypedScope* scope = UntypedBuilder::buildScope<SgUntypedScope>(label);

   table_desc = new SgUntypedStructureDefinition(type_name, has_type_name, has_body, preset, attr_list, scope);
   ROSE_ASSERT(table_desc);
   SageInterface::setSourcePosition(table_desc);

   return table_desc;
}


// Build an untyped StructureDefinition. This version has a type name and no body.
// Source position for the initializer and table description should be set after construction.
SgUntypedStructureDefinition* buildStructureDefinition(std::string type_name)
{
   int expr_enum;
   bool has_body = false;
   bool has_type_name = true;

   SgUntypedScope* scope = NULL;
   SgUntypedStructureDefinition* struct_desc = NULL;

   expr_enum = General_Language_Translation::e_struct_modifier_list;
   SgUntypedExprListExpression* attr_list = new SgUntypedExprListExpression(expr_enum);
   ROSE_ASSERT(attr_list);
   SageInterface::setSourcePosition(attr_list);

   expr_enum = General_Language_Translation::e_struct_initializer;
   SgUntypedExprListExpression* preset = new SgUntypedExprListExpression(expr_enum);
   ROSE_ASSERT(preset);
   SageInterface::setSourcePosition(preset);

   struct_desc = new SgUntypedStructureDefinition(type_name, has_type_name, has_body, preset, attr_list, scope);
   ROSE_ASSERT(struct_desc);
   SageInterface::setSourcePosition(struct_desc);

   std::cout << "-x- build StructureDefinition " << struct_desc << std::endl;
   std::cout << "-x-       scope is " << struct_desc->get_scope() << std::endl;

   return struct_desc;
}


// Build an untyped JovialTableDescription. This version has a body and thus a scope.
// Source position for the initializer and modifier lists and table description should be set after construction.
SgUntypedStructureDefinition* buildJovialTableDescription()
{
   return buildStructureDefinition();
}


// Build an untyped JovialTableDescription. This version has a type name and no body.
// Source position for the initializer and table description should be set after construction.
SgUntypedStructureDefinition* buildJovialTableDescription(std::string type_name)
{
   return buildStructureDefinition(type_name);
}


SgUntypedNullExpression* buildUntypedNullExpression()
{
   SgUntypedNullExpression* expr = new SgUntypedNullExpression();
   ROSE_ASSERT(expr);
   SageInterface::setSourcePosition(expr);

   return expr;
}


} // namespace UntypedBuilder
