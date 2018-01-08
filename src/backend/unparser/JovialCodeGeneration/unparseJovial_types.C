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
#if 0
          case V_SgTypeVoid:       unparseTypeVoid( isSgTypeVoid(type), info); break;

          case V_SgTypeSignedChar: unparseTypeSignedChar( isSgTypeSignedChar(type), info); break;
          case V_SgTypeWchar:      unparseTypeWchar( isSgTypeWchar(type), info); break;
          case V_SgTypeShort:      unparseTypeShort( isSgTypeShort(type), info); break;
#endif
          case V_SgTypeInt:           curprint("S");                             break;
          case V_SgTypeUnsignedInt:   curprint("U");                             break;
          case V_SgTypeBool:          curprint("B");                             break;

#if 0
          case V_SgTypeLong:       unparseTypeLong( isSgTypeLong(type), info); break;
          case V_SgTypeFloat:      unparseTypeFloat( isSgTypeFloat(type), info); break;
          case V_SgTypeDouble:     unparseTypeDouble( isSgTypeDouble(type), info); break;

          case V_SgArrayType:      unparseArrayType( isSgArrayType(type), info); break;
          case V_SgTypedefType:    unparseTypedefType( isSgTypedefType(type), info); break;
          case V_SgClassType:      unparseClassType( isSgClassType(type), info); break;
          case V_SgEnumType:       unparseEnumType( isSgEnumType(type), info); break;
          case V_SgModifierType:   unparseModifierType( isSgModifierType(type), info); break;
#endif

          default:
               cout << "Unparse_Jovial::unparseType(" << type->class_name() << "*,info) is unimplemented." << endl;
               ROSE_ASSERT(false);
               break;
        }
   }
