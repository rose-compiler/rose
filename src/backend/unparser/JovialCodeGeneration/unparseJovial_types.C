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
       // Primitive types
          case V_SgTypeInt:         curprint("S"); unparseTypeSize(type, info);  break;
          case V_SgTypeUnsignedInt: curprint("U"); unparseTypeSize(type, info);  break;
          case V_SgTypeFloat:       curprint("F"); unparseTypeSize(type, info);  break;
          case V_SgTypeBool:        curprint("B"); unparseTypeSize(type, info);  break;
          case V_SgTypeChar:        curprint("C"); unparseTypeSize(type, info);  break;
          case V_SgTypeString:      curprint("C"); unparseTypeSize(type, info);  break;
          case V_SgTypeVoid:                                                     break;

          case V_SgTypeFixed:          unparseTypeFixed(type, info);             break;

          case V_SgJovialTableType:    unparseTableType(type, info);             break;
          case V_SgArrayType:          unparseArrayType(type, info);             break;

          case V_SgFunctionType:
             {
                SgFunctionType* func_type = isSgFunctionType(type);
                ROSE_ASSERT(func_type);
                curprint(" ");
                unparseType(func_type->get_return_type(), info);
                break;
             }
        case V_SgModifierType:
           {
           // This appears when an ItemTypeDescription has an initializer. I'm not sure why
           // SgModifiertype appears as the typeptr of an initialized name.  The base_ptr is
           // the correct type so it is unparsed here as a HACK!  The modifier type appears to
           // inserted after the parsing phase and I assume it is because the presence of an
           // initializer causes the type to be wrapped with an SgModifierType with isConst
           // and isStatic set. This required the creation of isJovialStatic to correctly
           // unparse when the Jovial source actually has the STATIC keyword.

           // TODO - move this to a function
              SgModifierType* modifier_type = isSgModifierType(type);
              ROSE_ASSERT(modifier_type);
              SgType* base_type = modifier_type->get_base_type();
              ROSE_ASSERT(base_type);
              unparseType(base_type, info);
              break;
           }

        case V_SgEnumType:
           {
           // TODO - create function for this
              SgEnumType* enum_type = isSgEnumType(type);
           // TODO - for now only unparse the name
              curprint(enum_type->get_name());
              break;
           }

          default:
               cout << "Unparse_Jovial::unparseType for type " << type->class_name() << " is unimplemented." << endl;
               ROSE_ASSERT(false);
               break;
        }
   }

void
Unparse_Jovial::unparseTypeFixed(SgType* type, SgUnparse_Info& info)
   {
      SgTypeFixed* fixed_type = isSgTypeFixed(type);
      ROSE_ASSERT(fixed_type != NULL);

      curprint("A ");

      SgExpression* scale    = fixed_type->get_scale();
      SgExpression* fraction = fixed_type->get_fraction();

      ROSE_ASSERT(scale != NULL);
      ROSE_ASSERT(fraction != NULL);

      unparseExpression(scale, info);
      curprint(",");
      unparseExpression(fraction, info);
   }

void
Unparse_Jovial::unparseTypeSize(SgType* type, SgUnparse_Info& info)
   {
      ROSE_ASSERT(type != NULL);

      SgExpression* size = type->get_type_kind();

      if (size == NULL)
         {
         // look for a character length
            SgTypeString * string_type = isSgTypeString(type);
            if (string_type != NULL)
               {
                  size = string_type->get_lengthExpression();
               }
         }

      if (size != NULL)
         {
            curprint(" ");
            unparseExpression(size,info);
         }
   }

void
Unparse_Jovial::unparseTableType(SgType* type, SgUnparse_Info& info)
  {
     SgJovialTableType* table_type = isSgJovialTableType(type);
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

void
Unparse_Jovial::unparseArrayType(SgType* type, SgUnparse_Info& info)
  {
     SgArrayType* array_type = isSgArrayType(type);
     ROSE_ASSERT(array_type != NULL);

     curprint("(");
     UnparseLanguageIndependentConstructs::unparseExprList(array_type->get_dim_info(), info);
     curprint(") ");

     unparseType(array_type->get_base_type(), info);
  }
