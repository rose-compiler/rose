/* Unparse_Ada.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparser.h" //charles4:  I replaced this include:   #include "unparseX10.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

using namespace std;

void replaceString (std::string& str, const std::string& from, const std::string& to);

//-----------------------------------------------------------------------------------
//  void Unparse_Ada::unparseType
//
//  General function that gets called when unparsing an Ada type. Then it routes
//  to the appropriate function to unparse each Ada type.
//-----------------------------------------------------------------------------------
void
Unparse_Ada::unparseType(SgType* type, SgUnparse_Info& info)
   {

     ASSERT_not_null(type);

     switch (type->variantT())
        {
#if 0
          case V_SgTypeVoid:       unparseTypeVoid( isSgTypeVoid(type), info); break;

          case V_SgTypeSignedChar: unparseTypeSignedChar( isSgTypeSignedChar(type), info); break;
          case V_SgTypeWchar:      unparseTypeWchar( isSgTypeWchar(type), info); break;
          case V_SgTypeShort:      unparseTypeShort( isSgTypeShort(type), info); break;
          case V_SgTypeInt:        unparseTypeInt( isSgTypeInt(type), info); break;
          case V_SgTypeLong:       unparseTypeLong( isSgTypeLong(type), info); break;
          case V_SgTypeFloat:      unparseTypeFloat( isSgTypeFloat(type), info); break;
          case V_SgTypeDouble:     unparseTypeDouble( isSgTypeDouble(type), info); break;
          case V_SgTypeBool:       unparseTypeBool( isSgTypeBool(type), info); break;

          case V_SgArrayType:      unparseArrayType( isSgArrayType(type), info); break;
          case V_SgTypedefType:    unparseTypedefType( isSgTypedefType(type), info); break;
          case V_SgClassType:      unparseClassType( isSgClassType(type), info); break;
          case V_SgEnumType:       unparseEnumType( isSgEnumType(type), info); break;
          case V_SgModifierType:   unparseModifierType( isSgModifierType(type), info); break;

       // DQ (9/5/2011): Added support for X10 generics.
          case V_SgJavaQualifiedType:  unparseX10QualifiedType(isSgJavaQualifiedType(type), info); break;
          case V_SgJavaParameterType:  unparseClassType(isSgJavaParameterType(type), info); break;
          case V_SgJavaParameterizedType:  unparseX10ParameterizedType(isSgJavaParameterizedType(type), info); break;
          case V_SgJavaWildcardType:  unparseX10WildcardType(isSgJavaWildcardType(type), info); break;
          case V_SgJavaUnionType:  unparseX10UnionType(isSgJavaUnionType(type), info); break;
#endif
          default:
               cout << "Unparse_Ada::unparseType(" << type->class_name() << "*,info) is unimplemented." << endl;
               ROSE_ASSERT(false);
               break;
        }
   }

