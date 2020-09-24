/* Unparse_Jovial.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparser.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

using namespace std;

void replaceString (std::string& str, const std::string& from, const std::string& to);

//-----------------------------------------------------------------------------------
//  void Unparse_Jovial::unparseType
//
//  General function that is called when unparsing a Jovial type. Then it routes
//  to the appropriate function to unparse each Jovial type.
//-----------------------------------------------------------------------------------
void
Unparse_Jovial::unparseType(SgType* type, SgUnparse_Info& info)
   {
     ASSERT_not_null(type);

     switch (type->variantT())
        {
          case V_SgTypeVoid:                                                             break;

       // Primitive types
          case V_SgTypeInt:         unparseJovialType(isSgTypeInt(type), info);          break;
          case V_SgTypeUnsignedInt: unparseJovialType(isSgTypeUnsignedInt(type), info);  break;
          case V_SgTypeFloat:       unparseJovialType(isSgTypeFloat(type), info);        break;
          case V_SgTypeFixed:       unparseJovialType(isSgTypeFixed(type), info);        break;

          case V_SgTypeBool:        unparseJovialType(isSgTypeBool(type), info);         break;
          case V_SgTypeChar:        unparseJovialType(isSgTypeChar(type), info);         break;
          case V_SgTypeString:      unparseJovialType(isSgTypeString(type), info);       break;

          case V_SgModifierType:     unparseJovialType(isSgModifierType(type), info);    break;
          case V_SgJovialBitType:    unparseJovialType(isSgJovialBitType(type), info);   break;
          case V_SgJovialTableType:  unparseJovialType(isSgJovialTableType(type), info); break;
          case V_SgArrayType:        unparseJovialType(isSgArrayType(type), info);       break;
          case V_SgEnumType:         unparseJovialType(isSgEnumType(type), info);        break;
          case V_SgFunctionType:     unparseJovialType(isSgFunctionType(type), info);    break;
          case V_SgPointerType:      unparseJovialType(isSgPointerType(type), info);     break;
          case V_SgTypedefType:      unparseJovialType(isSgTypedefType(type), info);     break;

          default:
               cout << "Unparse_Jovial::unparseType for type " << type->class_name() << " is unimplemented." << endl;
               ROSE_ASSERT(false);
               break;
        }
   }

void
Unparse_Jovial::unparseTypeDesc(SgType* type, SgUnparse_Info& info)
   {
     ASSERT_not_null(type);

     switch (type->variantT())
        {
          case V_SgTypeInt:         curprint("S");       break;
          case V_SgTypeUnsignedInt: curprint("U");       break;
          case V_SgTypeFloat:       curprint("F");       break;
          case V_SgTypeFixed:       curprint("A");       break;
          case V_SgTypeChar:        curprint("C");       break;
          case V_SgTypeString:      curprint("C");       break;
          case V_SgPointerType:     curprint("P");       break;
          case V_SgJovialBitType:   curprint("B");       break;
          default:
             std::cerr << "Unparse_Jovial::unparseTypeDesc for type " << type->class_name() << " case default reached \n";
             ROSE_ASSERT(false);
        }
   }

void
Unparse_Jovial::unparseTypeSize(SgType* type, SgUnparse_Info& info)
   {
     ASSERT_not_null(type);

  // Warning: This function may be called if there is an initializer because the type
  // could be wrapped as an SgModifierType.  When the compool rcmp file is unparsed
  // this won't be called which leads to confusion. Care must be taken if the default
  // for the size value isn't appropriate for the type, in particular, SgJovialBitType.
  //
     switch (type->variantT())
        {
          case V_SgTypeFixed:     unparseTypeSize(isSgTypeFixed(type), info);     break;
          case V_SgTypeString:    unparseTypeSize(isSgTypeString(type), info);    break;
          case V_SgJovialBitType: unparseTypeSize(isSgJovialBitType(type), info); break;
          default:
             {
                SgExpression* size = type->get_type_kind();
                if (size != NULL)
                   {
                      curprint(" ");
                      unparseExpression(size,info);
                   }
             }
        }
   }

void
Unparse_Jovial::unparseTypeSize(SgTypeFixed* fixed_type, SgUnparse_Info& info)
   {
      ASSERT_not_null(fixed_type);

      SgExpression* scale    = fixed_type->get_scale();
      SgExpression* fraction = fixed_type->get_fraction();

      ASSERT_not_null(scale);

      curprint(" ");
      unparseExpression(scale, info);
      if (fraction != NULL)
         {
            curprint(",");
            unparseExpression(fraction, info);
         }
   }

void
Unparse_Jovial::unparseTypeSize(SgTypeString* string_type, SgUnparse_Info& info)
   {
      ASSERT_not_null(string_type);

      SgExpression* size = string_type->get_lengthExpression();
      if (size != NULL)
         {
            curprint(" ");
            unparseExpression(size,info);
         }
   }

void
Unparse_Jovial::unparseTypeSize(SgJovialBitType* bit_type, SgUnparse_Info& info)
   {
      ASSERT_not_null(bit_type);

      SgExpression* size = bit_type->get_size();
      if (size != NULL)
         {
            curprint(" ");
            unparseExpression(size,info);
         }
   }

template <class T> void
Unparse_Jovial::unparseJovialType(T* type, SgUnparse_Info& info)
   {
      ASSERT_not_null(type);
      unparseTypeDesc(type, info);
      unparseTypeSize(type, info);
   }

void
Unparse_Jovial::unparseJovialType(SgArrayType* array_type, SgUnparse_Info& info)
  {
     ASSERT_not_null(array_type);
     curprint("(");
     UnparseLanguageIndependentConstructs::unparseExprList(array_type->get_dim_info(), info);
     curprint(") ");

     unparseType(array_type->get_base_type(), info);
  }

void
Unparse_Jovial::unparseJovialType(SgPointerType* pointer_type, SgUnparse_Info& info)
  {
     ASSERT_not_null(pointer_type);
     unparseTypeDesc(pointer_type, info);
     curprint(" ");

  // The type name is optional
     if (SgNamedType* named_type = isSgNamedType(pointer_type->get_base_type()))
        {
           curprint(named_type->get_name());
        }
     else if (SgTypeUnknown* unknown_type = isSgTypeUnknown(pointer_type->get_base_type()))
        {
           if (unknown_type->get_has_type_name())
              {
                 curprint(unknown_type->get_type_name());
              }
        }
  }

void
Unparse_Jovial::unparseJovialType(SgEnumType* enum_type, SgUnparse_Info& info)
  {
     ASSERT_not_null(enum_type);
  // TODO - for now only unparse the name
     curprint(enum_type->get_name());
  }

void
Unparse_Jovial::unparseJovialType(SgFunctionType* function_type, SgUnparse_Info& info)
  {
     ROSE_ASSERT(function_type);
     curprint(" ");
     unparseType(function_type->get_return_type(), info);
  }

void
Unparse_Jovial::unparseJovialType(SgModifierType* modifier_type, SgUnparse_Info& info)
  {
  // An SgModifierType is used when an ItemTypeDescription has an initializer.  The modifier type appears to
  // be inserted after the parsing phase and I assume it is because the presence of an initializer causes the
  // type to be wrapped with an SgModifierType with isConst and isStatic set. This required the creation of
  // isJovialStatic to correctly unparse when the Jovial source actually has the STATIC keyword.

     ROSE_ASSERT(modifier_type);
     SgType* base_type = modifier_type->get_base_type();
     ROSE_ASSERT(base_type);

  // SgModifierType is also used to mark R,T,Z (round, truncate, trancate towards zero).
  // If not used for (R,T,Z), unwrap and then unparse the base type (this should fix recurring problems).
     if (modifier_type->get_typeModifier().isRound()     ||
         modifier_type->get_typeModifier().isTruncate()  ||
         modifier_type->get_typeModifier().isTruncateTowardsZero())
     {
        unparseTypeDesc(base_type, info);
        if      (modifier_type->get_typeModifier().isRound())               curprint(",R");
        else if (modifier_type->get_typeModifier().isTruncate())            curprint(",T");
        else if (modifier_type->get_typeModifier().isTruncateTowardsZero()) curprint(",Z");
        unparseTypeSize(base_type, info);
     }
     else
     {
        unparseType(base_type, info);
     }
  }

void
Unparse_Jovial::unparseJovialType(SgJovialTableType* table_type, SgUnparse_Info& info)
  {
     ASSERT_not_null(table_type);

     SgType* base_type = table_type->get_base_type();
     std::string type_name = table_type->get_name();

  // TODO: There is a better way to do this by seeing if variableDeclarationContainsBaseTypeDefineingDeclaration (need function)
     bool is_anonymous = (type_name.find("_anon_typeof_") != std::string::npos);

     SgExprListExp* dim_info = table_type->get_dim_info();
     if (dim_info != NULL)
        {
           unparseDimInfo(dim_info, info);
        }

     if (info.inVarDecl() && is_anonymous == false)
        {
           curprint(type_name);
        }
     else if (base_type != NULL)
        {
        // Unparse base type directly if present and not in a variable declaration context
           unparseType(base_type, info);
        }
  }

void
Unparse_Jovial::unparseJovialType(SgTypedefType* type_def, SgUnparse_Info& info)
  {
     ROSE_ASSERT(type_def);
     curprint(type_def->get_name());
  }
