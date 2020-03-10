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
     ROSE_ASSERT(type != NULL);

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
          case V_SgJovialTableType:  unparseJovialType(isSgJovialTableType(type), info); break;
          case V_SgArrayType:        unparseJovialType(isSgArrayType(type), info);       break;
          case V_SgEnumType:         unparseJovialType(isSgEnumType(type), info);        break;
          case V_SgFunctionType:     unparseJovialType(isSgFunctionType(type), info);    break;
          case V_SgPointerType:      unparseJovialType(isSgPointerType(type), info);     break;

          default:
               cout << "Unparse_Jovial::unparseType for type " << type->class_name() << " is unimplemented." << endl;
               ROSE_ASSERT(false);
               break;
        }
   }

void
Unparse_Jovial::unparseTypeDesc(SgType* type, SgUnparse_Info& info)
   {
     ROSE_ASSERT(type != NULL);

     switch (type->variantT())
        {
          case V_SgTypeInt:         curprint("S");       break;
          case V_SgTypeUnsignedInt: curprint("U");       break;
          case V_SgTypeFloat:       curprint("F");       break;
          case V_SgTypeFixed:       curprint("A");       break;
          case V_SgTypeBool:        curprint("B");       break;
          case V_SgTypeChar:        curprint("C");       break;
          case V_SgTypeString:      curprint("C");       break;
          case V_SgPointerType:     curprint("P");       break;
          default:
             std::cerr << "Unparse_Jovial::unparseTypeDesc for type " << type->class_name() << " case default reached \n";
             ROSE_ASSERT(false);
        }
   }

void
Unparse_Jovial::unparseTypeSize(SgType* type, SgUnparse_Info& info)
   {
     ROSE_ASSERT(type != NULL);

     switch (type->variantT())
        {
          case V_SgTypeFixed:   unparseTypeSize(isSgTypeFixed(type), info);    break;
          case V_SgTypeString:  unparseTypeSize(isSgTypeString(type), info);   break;
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
      ROSE_ASSERT(fixed_type != NULL);

      SgExpression* scale    = fixed_type->get_scale();
      SgExpression* fraction = fixed_type->get_fraction();

      ROSE_ASSERT(scale != NULL);

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
      ROSE_ASSERT(string_type != NULL);

      SgExpression* size = string_type->get_lengthExpression();
      if (size != NULL)
         {
            curprint(" ");
            unparseExpression(size,info);
         }
   }


template <class T> void
Unparse_Jovial::unparseJovialType(T* type, SgUnparse_Info& info)
   {
      ROSE_ASSERT(type != NULL);
      unparseTypeDesc(type, info);
      unparseTypeSize(type, info);
   }

void
Unparse_Jovial::unparseJovialType(SgArrayType* array_type, SgUnparse_Info& info)
  {
     ROSE_ASSERT(array_type != NULL);
     curprint("(");
     UnparseLanguageIndependentConstructs::unparseExprList(array_type->get_dim_info(), info);
     curprint(") ");

     unparseType(array_type->get_base_type(), info);
  }

void
Unparse_Jovial::unparseJovialType(SgPointerType* pointer_type, SgUnparse_Info& info)
  {
     ROSE_ASSERT(pointer_type != NULL);
     unparseTypeDesc(pointer_type, info);
     curprint(" ");

     SgNamedType* named_type = isSgNamedType(pointer_type->get_base_type());
     if (named_type != NULL)
        {
           curprint(named_type->get_name());
        }
     else  std::cerr << "WARNING UNIMPLEMENTED: unparseJovialType - named_type is NULL\n";
  }

void
Unparse_Jovial::unparseJovialType(SgEnumType* enum_type, SgUnparse_Info& info)
  {
     ROSE_ASSERT(enum_type != NULL);
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

  // SgModifierType is also used to mark R,T,Z (round, truncate, trancate towards zero).
     ROSE_ASSERT(modifier_type);

     SgType* base_type = modifier_type->get_base_type();
     ROSE_ASSERT(base_type);

     unparseTypeDesc(base_type, info);

     if      (modifier_type->get_typeModifier().isRound())               curprint(",R");
     else if (modifier_type->get_typeModifier().isTruncate())            curprint(",T");
     else if (modifier_type->get_typeModifier().isTruncateTowardsZero()) curprint(",Z");

     unparseTypeSize(base_type, info);
  }

void
Unparse_Jovial::unparseJovialType(SgJovialTableType* table_type, SgUnparse_Info& info)
  {
     ROSE_ASSERT(table_type != NULL);

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
