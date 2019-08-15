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

          default:
               cout << "Unparse_Jovial::unparseType for type " << type->class_name() << " is unimplemented." << endl;
               ROSE_ASSERT(false);
               break;
        }
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

     SgDeclarationStatement* type_decl = type->getAssociatedDeclaration();
     ROSE_ASSERT(type_decl);

     SgExprListExp* dim_info = table_type->get_dim_info();
     if (dim_info != NULL)
        {
           unparseDimInfo(dim_info, info);
        }

     SgType* base_type = table_type->get_base_type();

     if (base_type == NULL)
        {
           cout << "--> unparse TableType: base type is NULL \n";
           return;
        }

     ROSE_ASSERT(base_type);

     unparseType(base_type, info);
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
