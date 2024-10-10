
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseType 0

//-----------------------------------------------------------------------------------
//  void Unparse_Type::unparseType
//
//  General function that gets called when unparsing a C++ type. Then it routes
//  to the appropriate function to unparse each C++ type.
//-----------------------------------------------------------------------------------
void Unparse_Type::unparseType(SgType* type, SgUnparse_Info& info) {
     ASSERT_not_null(type);
#if DEBUG__unparseType
     printf ("Enter Unparse_Type::unparseType():\n");
     printf ("  type = %p = %s\n", type, type->class_name().c_str());
     printf ("  info.isTypeFirstPart()  = %s\n", info.isTypeFirstPart()  ? "true" : "false");
     printf ("  info.isTypeSecondPart() = %s\n", info.isTypeSecondPart() ? "true" : "false");
     printf ("  info.SkipBaseType()     = %s\n", info.SkipBaseType() ? "true" : "false");
#endif
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     bool using_generated_name = false;
     std::string generated_name;

     SgNode* nodeReferenceToType = info.get_reference_node_for_qualification();

     SgInitializedName * init_name_reference_node = isSgInitializedName(nodeReferenceToType);
     if (init_name_reference_node != NULL) {
       if (init_name_reference_node->get_auto_decltype() != NULL) {
         nodeReferenceToType = NULL;
       }
     }

     if (nodeReferenceToType != NULL) {
       std::map<SgNode*,std::string>::iterator i = SgNode::get_globalTypeNameMap().find(nodeReferenceToType);
       if (i != SgNode::get_globalTypeNameMap().end()) {
         if (!info.isTypeSecondPart()) {
           using_generated_name = true;
         }
         generated_name = i->second.c_str();
       }
     }
#if DEBUG__unparseType
     printf ("  using_generated_name = %s\n", using_generated_name ? "true" : "false");
     printf ("  generated_name = %s\n", generated_name.c_str());
#endif
     if (using_generated_name) {
       if ( (  info.isTypeFirstPart() && !info.SkipBaseType()                             ) ||
            ( !info.isTypeFirstPart() && !info.isTypeSecondPart() && !info.SkipBaseType() )
       ) {
         curprint(generated_name);
         curprint(" ");
       }
     } else {
       switch (type->variant()) {
         case T_UNKNOWN:
           if (!info.isTypeSecondPart())
             curprint ( get_type_name(type) + " ");
           break;
         case T_CHAR:
         case T_SIGNED_CHAR:
         case T_UNSIGNED_CHAR:
         case T_SHORT:
         case T_SIGNED_SHORT:
         case T_UNSIGNED_SHORT:
         case T_INT:
         case T_SIGNED_INT:
         case T_UNSIGNED_INT:
         case T_LONG:
         case T_SIGNED_LONG:
         case T_UNSIGNED_LONG:
         case T_VOID:
         case T_GLOBAL_VOID:
         case T_WCHAR:
         case T_CHAR16:
         case T_CHAR32:
         case T_FLOAT:
         case T_DOUBLE:
         case T_FLOAT80:
         case T_FLOAT128:
         case T_LONG_LONG:
         case T_UNSIGNED_LONG_LONG:
         case T_SIGNED_LONG_LONG:
         case T_SIGNED_128BIT_INTEGER:
         case T_UNSIGNED_128BIT_INTEGER:
         case T_MATRIX:
         case T_TUPLE:
         case T_LONG_DOUBLE:
         case T_STRING:
         case T_BOOL:
         case T_COMPLEX:
         case T_IMAGINARY:
         case T_DEFAULT:
         case T_ELLIPSE:
           if ( !( info.isWithType() && info.SkipBaseType() ) && !info.isTypeSecondPart() )
             curprint ( get_type_name(type) + " ");
           break;

         case T_POINTER:            unparsePointerType(type, info);          break;
         case T_MEMBER_POINTER:     unparseMemberPointerType(type, info);    break;
         case T_REFERENCE:          unparseReferenceType(type, info);        break;
         case T_RVALUE_REFERENCE:   unparseRvalueReferenceType(type, info);  break;
         case T_CLASS:              unparseClassType(type, info);            break;
         case T_ENUM:               unparseEnumType(type, info);             break;
         case T_TYPEDEF:            unparseTypedefType(type, info);          break;
         case T_MODIFIER:           unparseModifierType(type, info);         break;
         case T_MEMBERFUNCTION:     unparseMemberFunctionType(type, info);   break;
         case T_ARRAY:              unparseArrayType(type, info);            break;
         case T_TEMPLATE:           unparseTemplateType(type, info);         break;
         case T_AUTO:               unparseAutoType(type, info);             break;
         case T_NULLPTR:            unparseNullptrType(type, info);          break;
         case T_DECLTYPE:           unparseDeclType(type, info);             break;
         case T_TYPEOF_TYPE:        unparseTypeOfType(type, info);           break;
         case T_NONREAL:            unparseNonrealType(type, info);          break;
         case T_PARTIAL_FUNCTION:
         case T_FUNCTION:           unparseFunctionType(type, info);         break;

         // Liao (10/4/2021): Added so that we could get past this call in the Ada2Cpp translator (fix later).
         case T_ADA_SUBTYPE:
         case T_ADA_MODULAR_TYPE:
         case T_ADA_DERIVED_TYPE:
         case T_ADA_ACCESS_TYPE:
         case T_ADA_DISCRETE_TYPE:
         case T_ADA_FORMAL_TYPE: {
           printf ("Warning: SgAda*Type is appearing in call to unparseType from Ada2Cpp (allow this for now) \n");
           break;
         }
         case T_JAVA_WILD: {
           printf ("ERROR: SgJavaWildcardType is appearing in call to unparseType from graph generation (allow this for now) \n");
           break;
         }
         case T_LABEL: {
           printf ("ERROR: Unparse_Type::unparseType(): SgTypeLabel is appearing in test2007_168.f90 (where it had not appeared before) (allow this for now) \n");
           break;
         }
         default: {
           printf("Error: Unparse_Type::unparseType(): Default case reached in switch: Unknown type %p = %s \n",type,type->class_name().c_str());
           ROSE_ABORT();
         }
       }
     }
#if DEBUG__unparseType
     printf ("Leaving Unparse_Type::unparseType()\n");
#endif
   }
